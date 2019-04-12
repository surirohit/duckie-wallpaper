/*
 *
 * LiveWallpaper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2012-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

#include "config.h"

#include <glib.h>
#include <gio/gio.h>
#include <libpeas/peas.h>

#include <GL/glew.h>

#include <livewallpaper/core.h>

#include "clock.h"
#include "command-line.h"
#include "plugins-engine.h"
#include "window.h"
#include "opengl-window.h"
#include "application.h"
#include "power-manager.h"
#include "fps-visualizer.h"

typedef struct _LwSource
{
	GSource source;

	LwApplication *application;
} LwSource;

struct _LwApplicationPrivate
{
	GSettings *settings;
	LwClock *clock;

	gboolean active;
	gboolean desktop_icons;

	LwWindow *win;

	gint n_outputs;
	GList *outputs;

	LwPluginsEngine *plugins_engine;
	LwWallpaper *wallpaper;

	GSource *source;

	LwPowerManager *pm;
	LwFPSVisualizer *fps;
};

enum
{
	PROP_0,

	PROP_ACTIVE,
    PROP_DESKTOP_ICONS,

	N_PROPERTIES
};

G_DEFINE_TYPE(LwApplication, lw_application, G_TYPE_APPLICATION)


static void
lw_application_set_property(GObject *object,
                            guint property_id,
                            const GValue *value,
                            GParamSpec *pspec)
{
	LwApplication *self = LW_APPLICATION(object);

	switch(property_id)
	{
		case PROP_ACTIVE:
			self->priv->active = g_value_get_boolean(value);
			if(self->priv->active)
				lw_window_show(self->priv->win, self->priv->desktop_icons);
			else
				lw_window_hide(self->priv->win);
			break;
		case PROP_DESKTOP_ICONS:
			self->priv->desktop_icons= g_value_get_boolean(value);
            lw_window_hide(self->priv->win);
            lw_window_show(self->priv->win, self->priv->desktop_icons);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_application_get_property(GObject *object,
                            guint property_id,
                            GValue *value,
                            GParamSpec *pspec)
{
	LwApplication *self = LW_APPLICATION(object);

	switch(property_id)
	{
		case PROP_ACTIVE:
			g_value_set_boolean(value, self->priv->active);
			break;
		case PROP_DESKTOP_ICONS:
			g_value_set_boolean(value, self->priv->desktop_icons);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

#define lw_application_adjust_viewport()                        \
	if(self->priv->wallpaper && self->priv->n_outputs == 1)     \
	{                                                           \
		LwOutput *o = self->priv->outputs->data;                \
		lw_output_make_current(o);                              \
		lw_wallpaper_adjust_viewport(self->priv->wallpaper, o); \
	}

#define lw_application_restore_viewport()                     \
	if(self->priv->wallpaper && self->priv->n_outputs == 1)   \
		lw_wallpaper_restore_viewport(self->priv->wallpaper);

static void
lw_application_load_wallpaper_plugin(PeasEngine *engine, PeasPluginInfo *info, LwApplication *self)
{
	if(!peas_engine_provides_extension(engine, info, LW_TYPE_WALLPAPER))
		return;

	self->priv->wallpaper = LW_WALLPAPER(peas_engine_create_extension(engine, info, LW_TYPE_WALLPAPER, NULL));
	lw_wallpaper_init_plugin(self->priv->wallpaper);

	lw_application_adjust_viewport();
}

static void
lw_application_unload_wallpaper_plugin(PeasEngine *engine, PeasPluginInfo *info, LwApplication *self)
{
	if(!peas_engine_provides_extension(engine, info, LW_TYPE_WALLPAPER))
		return;

	lw_application_restore_viewport();

	g_clear_object(&self->priv->wallpaper);
}

static void
lw_application_update_outputs(G_GNUC_UNUSED LwWindow *win, LwApplication *self)
{
	lw_application_restore_viewport();

	/* Update outputs */
	self->priv->outputs = lw_window_get_outputs(self->priv->win);

	if(g_settings_get_enum(self->priv->settings, "multioutput-mode") == 0)
	{
		/* Merge all outputs to one big output */
		GList *i;
		LwOutput *output;
		GdkRectangle g = { 0 };

		for(i = self->priv->outputs; i; i = i->next)
		{
			GdkRectangle t;

			output = i->data;
			t.x = lw_output_get_x(output);
			t.y = lw_output_get_y(output);
			t.width = lw_output_get_width(output);
			t.height = lw_output_get_height(output);

			gdk_rectangle_union(&g, &t, &g);
		}

		output = g_object_new(LW_TYPE_OUTPUT,
		                      "x", g.x,
		                      "y", g.y,
		                      "width", g.width,
		                      "height", g.height,
		                      NULL);

		self->priv->outputs = g_list_prepend(NULL, output);
	}

	self->priv->n_outputs = g_list_length(self->priv->outputs);

	lw_application_adjust_viewport();
}

static void
lw_application_update_outputs_from_settings(G_GNUC_UNUSED GSettings *settings,
                                            G_GNUC_UNUSED gchar* key,
                                            LwApplication *self)
{
	lw_application_update_outputs(self->priv->win, self);
}

static gboolean
lw_application_prepare(GSource *source, gint *timeout)
{
	LwApplication *self = ((LwSource*) source)->application;
    *timeout = self->priv->active ? lw_clock_get_ms_to_sleep(self->priv->clock) : 100;
	return *timeout == 0;
}

static gboolean
lw_application_check(GSource *source)
{
	LwApplication *self = ((LwSource*) source)->application;
	return self->priv->active && lw_clock_get_ms_to_sleep(self->priv->clock) == 0;
}

static gboolean
lw_application_dispatch(GSource *source,
                        G_GNUC_UNUSED GSourceFunc callback,
                        G_GNUC_UNUSED gpointer user_data)
{
	LwApplication *self = ((LwSource*) source)->application;

	lw_clock_start_frame(self->priv->clock);

	if(self->priv->wallpaper)
	{
		/* Paint all outputs */
		GList *outputs = self->priv->outputs;

		/* Prepare paint */
		lw_wallpaper_prepare_paint(self->priv->wallpaper,
                                   lw_clock_get_ms_since_last_frame(self->priv->clock));
		for(; outputs; outputs = outputs->next)
		{
			LwOutput *o;
			if(self->priv->n_outputs > 1)
			{
				/* Get current output */
				o = outputs->data;

				/* Adjust viewport */
				lw_output_make_current(o);

				lw_wallpaper_adjust_viewport(self->priv->wallpaper, o);
			}
			else    /* Get current output */
				o = self->priv->outputs->data;

			/* Paint */
			lw_wallpaper_paint(self->priv->wallpaper, o);
			lw_fps_visualizer_paint(self->priv->fps, o);

			if(self->priv->n_outputs > 1)   /* Restore viewport */
				lw_wallpaper_restore_viewport(self->priv->wallpaper);
		}

		/* Done paint */
		lw_wallpaper_done_paint(self->priv->wallpaper);
	}
	else
	{
		/* Clear window because there is no active wallpaper */
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	lw_window_swap_buffers(self->priv->win);
	lw_clock_end_frame(self->priv->clock);

	return TRUE;
}

static int
lw_application_local_command_line(GApplication *application,
                                  gchar ***argv,
                                  gint *exit_status)
{
	gint argc = g_strv_length(*argv);
	LwCommandLine *command_line = lw_command_line_parse(&argc, argv);

	if(lw_command_line_get_version (command_line))
	{
		printf (LW_VERSION "\n");
		g_application_quit(application);
		*exit_status = 0;
		return TRUE;
	}

	g_object_unref(command_line);

	/* Chain up to the parent class */
	return G_APPLICATION_CLASS(lw_application_parent_class)
	           ->local_command_line(application, argv, exit_status);
}

#define LW_REQUIRES(extension)                                                        \
	if(! extension ) {                                                                \
		g_critical("Unable to start LiveWallpaper: " #extension " is not available"); \
		g_application_quit(application);                                              \
	}

static void
lw_application_startup(GApplication *application)
{
	LwApplication *self = LW_APPLICATION(application);
	GError *error;
	GLenum err;

	/* Create window */
	self->priv->win = g_object_new(LW_TYPE_OPENGL_WINDOW, NULL);
	if((error = lw_window_get_error(self->priv->win)) != NULL)
	{
		g_critical("Window creation failed: %s", error->message);
		g_error_free(error);
		g_application_quit(application);
	}

	lw_window_make_current(self->priv->win);

	/* Initialize GLEW */
	err = glewInit();
	if(err != GLEW_OK)
	{
		g_critical("Could not initialize glew: %s", glewGetErrorString(err));
		g_application_quit(application);
	}

	/* Check OpenGL version and extensions */
	LW_REQUIRES(GLEW_VERSION_1_4)
	LW_REQUIRES(GLEW_ARB_texture_non_power_of_two)
	LW_REQUIRES(GLEW_ARB_point_sprite)
	LW_REQUIRES(GLEW_ARB_shader_objects)
	LW_REQUIRES(GLEW_ARB_vertex_shader)
	LW_REQUIRES(GLEW_ARB_fragment_shader)
	LW_REQUIRES(GLEW_ARB_vertex_buffer_object)

	/* Initialize OpenGL */
	glEnable(GL_SCISSOR_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	/* Initialize outputs */
	lw_application_update_outputs(self->priv->win, self);
	g_signal_connect(self->priv->win, "outputs-changed",
	                 G_CALLBACK(lw_application_update_outputs), self);
	g_signal_connect(self->priv->settings, "changed::multioutput-mode",
	                 G_CALLBACK(lw_application_update_outputs_from_settings), self);

	/* Initialize plugins engine */
	self->priv->plugins_engine = lw_plugins_engine_new();
	g_signal_connect_after(self->priv->plugins_engine, "load-plugin",
	                       G_CALLBACK(lw_application_load_wallpaper_plugin), self);
	g_signal_connect      (self->priv->plugins_engine, "unload-plugin",
	                       G_CALLBACK(lw_application_unload_wallpaper_plugin), self);
	g_settings_bind(self->priv->settings, "active-plugin",
	                self->priv->plugins_engine, "wallpaper-plugin",
	                G_SETTINGS_BIND_GET);

	/* Set active to TRUE at start and bind it to the property */
	g_settings_set_boolean(self->priv->settings, "active", TRUE);
	g_settings_bind(self->priv->settings, "active",
	                self, "active", G_SETTINGS_BIND_GET);
	g_settings_bind(self->priv->settings, "desktop-icons",
	                self, "desktop-icons", G_SETTINGS_BIND_GET);

	/* Init additional objects */
	self->priv->pm = g_object_new(LW_TYPE_POWER_MANAGER, NULL);
	self->priv->fps = lw_fps_visualizer_new(self->priv->clock);

	/* We need to hold the application to keep LiveWallpaper running */
	g_application_hold(application);

	/* Chain up to the parent class */
	G_APPLICATION_CLASS(lw_application_parent_class)->startup(application);
}

static GSourceFuncs source_funcs = {
	lw_application_prepare,
	lw_application_check,
	lw_application_dispatch,
	NULL
};

static void
lw_application_init(LwApplication *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_APPLICATION,
	                                         LwApplicationPrivate);

	/* Initialize settings */
	self->priv->settings = g_settings_new(LW_SETTINGS);

	/* Initialize clock */
	self->priv->clock = lw_clock_new();
	g_settings_bind(self->priv->settings, "fps-limit",
	                self->priv->clock,    "fps-limit",
	                G_SETTINGS_BIND_GET);

	/* Connect LiveWallpaper to the main loop */
	self->priv->source = g_source_new(&source_funcs, sizeof(LwSource));
	((LwSource*) self->priv->source)->application = self;

	g_source_set_name(self->priv->source, "LiveWallpaper");
	g_source_set_can_recurse(self->priv->source, FALSE);
	g_source_attach(self->priv->source, NULL);
}

static void
lw_application_dispose(GObject *object)
{
	LwApplication *self = LW_APPLICATION(object);

	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->clock);
	g_clear_object(&self->priv->win);
	g_clear_object(&self->priv->plugins_engine);
	g_clear_object(&self->priv->pm);
	g_clear_object(&self->priv->fps);

	/* Disconnect LiveWallpaper from main loop */
	g_source_destroy(self->priv->source);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_application_parent_class)->dispose(object);
}

static void
lw_application_class_init(LwApplicationClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	GApplicationClass *gapplication_class = G_APPLICATION_CLASS(klass);
    GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };
	gobject_class->set_property = lw_application_set_property;
	gobject_class->get_property = lw_application_get_property;
	gobject_class->dispose = lw_application_dispose;

	gapplication_class->startup = lw_application_startup;
	gapplication_class->local_command_line = lw_application_local_command_line;

	g_type_class_add_private(klass, sizeof(LwApplicationPrivate));

	/* Install properties */
    obj_properties[PROP_ACTIVE] = g_param_spec_boolean("active",
                                                       "Start or stop the live wallpaper",
                                                       "Set whether the live wallpaper is running or not",
                                                       TRUE, G_PARAM_READWRITE);
    obj_properties[PROP_DESKTOP_ICONS] = g_param_spec_boolean("desktop-icons",
                                                              "Show desktop icons (Nautilus only)",
                                                              "Draw LiveWallpaper behind the other desktop applications",
                                                              TRUE, G_PARAM_READWRITE);
    g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}
