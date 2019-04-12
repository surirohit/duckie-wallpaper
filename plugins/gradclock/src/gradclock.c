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
 * Copyright (C) 2013-2016 Koichi Akabe <vbkaisetsu@gmail.com>
 *
 */

#include <gdk/gdk.h>

#include <livewallpaper/core.h>
#include <libpeas/peas.h>

#include <math.h>
#include <time.h>

#include "gradclock_drawer.h"
#include "gradclock.h"

#define GRADCLOCK_RADIUS  0.40
#define GRADCLOCK_RADIUS_1  0.92
#define GRADCLOCK_RADIUS_2  0.90
#define GRADCLOCK_RADIUS_3  0.80
#define GRADCLOCK_RADIUS_4  0.78
#define GRADCLOCK_RADIUS_5  0.60

#define e	0.0005
#define anim_msec	500

struct _GradClockPluginPrivate
{
	GSettings *settings;
	gint tm_msec, tm_sec, tm_min, tm_hour;
	gint upd_sec, upd_min, upd_hour;
	gdouble anim_delta[anim_msec];
	gdouble per_sec, per_min, per_hour;
	gdouble sec_radius, min_radius, hour_radius;
	gdouble radius;
	LwCairoTexture *tex_sec, *tex_min, *tex_hour;
	LwBackground *background;
	GdkRGBA color_sec, color_min, color_hour;
	gdouble start_smoothness, end_smoothness;
	GdkColor primary_color, secondary_color;
};

enum
{
	PROP_0,

	COLOR_SECOND,
	COLOR_MINUTE,
	COLOR_HOUR,
	
	START_SMOOTHNESS,
	END_SMOOTHNESS,

	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void lw_wallpaper_iface_init(LwWallpaperInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(GradClockPlugin, gradclock_plugin, PEAS_TYPE_EXTENSION_BASE, 0,
                               G_IMPLEMENT_INTERFACE_DYNAMIC(LW_TYPE_WALLPAPER, lw_wallpaper_iface_init))

static void
gradclock_plugin_update_settings(GradClockPlugin *self)
{
	gint i;
	for(i = 0; i < anim_msec; i++)
	{
		/* Bezier curve + Newton's method */
		gdouble r1 = self->priv->start_smoothness, r2 = 1.0 - self->priv->end_smoothness;
		gdouble a = -3.0 * r2 + 3.0 * r1 + 1.0,
		        d = -(gdouble)i / anim_msec / a,
		        t = 0.5,
		        t2 = 0;

		while(t - t2 > e || t2 - t > e)
		{
			gdouble b = (3.0 * r2 - 6.0 * r1) / a,
			        c = 3.0 * r1 / a,
			        y = ((t + b) * t + c) * t + d,
			        grad = (3 * t + 2 * b) * t + c;
			if(grad == 0)
			{
				t += e / 2;
				continue;
			}
			t2 = t;
			t -= y / grad;
		}
		self->priv->anim_delta[i] = (3 - 2 * t) * t * t;
	}
	self->priv->tm_msec = 0;
}

static void
gradclock_plugin_update_textures(GradClockPlugin *self)
{
	cairo_t *cr;

	if (self->priv->tex_sec)
		g_object_unref (self->priv->tex_sec);
	if (self->priv->tex_min)
		g_object_unref (self->priv->tex_min);
	if (self->priv->tex_hour)
		g_object_unref (self->priv->tex_hour);

	self->priv->tex_sec = lw_cairo_texture_new(ceil(self->priv->radius) * 2, ceil(self->priv->radius) * 2);
	self->priv->tex_min = lw_cairo_texture_new(ceil(self->priv->radius * GRADCLOCK_RADIUS_2) * 2, ceil(self->priv->radius * GRADCLOCK_RADIUS_2) * 2);
	self->priv->tex_hour = lw_cairo_texture_new(ceil(self->priv->radius * GRADCLOCK_RADIUS_4) * 2, ceil(self->priv->radius * GRADCLOCK_RADIUS_4) * 2);
	if (self->priv->tex_sec == NULL || self->priv->tex_min == NULL || self->priv->tex_hour == NULL)
		return;

    cr = lw_cairo_texture_cairo_create (self->priv->tex_sec);
	draw_grad_circle (cr, self->priv->radius * GRADCLOCK_RADIUS_1, self->priv->radius, self->priv->radius, self->priv->radius, self->priv->color_sec);
	cairo_destroy (cr);
	lw_cairo_texture_update (self->priv->tex_sec);
	
	cr = lw_cairo_texture_cairo_create (self->priv->tex_min);
	draw_grad_circle (cr, self->priv->radius * GRADCLOCK_RADIUS_3, self->priv->radius * GRADCLOCK_RADIUS_2, self->priv->radius * GRADCLOCK_RADIUS_2, self->priv->radius * GRADCLOCK_RADIUS_2, self->priv->color_min);
	cairo_destroy (cr);
	lw_cairo_texture_update (self->priv->tex_min);
	
	cr = lw_cairo_texture_cairo_create (self->priv->tex_hour);
	draw_grad_circle (cr, self->priv->radius * GRADCLOCK_RADIUS_5, self->priv->radius * GRADCLOCK_RADIUS_4, self->priv->radius * GRADCLOCK_RADIUS_4, self->priv->radius * GRADCLOCK_RADIUS_4, self->priv->color_hour);
	cairo_destroy (cr);
	lw_cairo_texture_update (self->priv->tex_hour);
}

static void
gradclock_plugin_adjust_viewport(LwWallpaper *plugin, LwOutput *output)
{
	guint width  = lw_output_get_width(output),
	      height = lw_output_get_height(output);
	GradClockPlugin *self = GRADCLOCK_PLUGIN(plugin);

	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	/* Save and change texture environment */
	glPushAttrib(GL_TEXTURE_BIT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Save and reset the coordinate system before touching */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(width > height)
	{
		gdouble aspect = ((gdouble) height) / ((gdouble) width),
		        center_y = aspect * 0.5;
		glOrtho(-0.5, 0.5, -aspect * 0.5, aspect * 0.5, 0.0, 1.0);
		self->priv->radius = ((gdouble) height) * GRADCLOCK_RADIUS;
		self->priv->sec_radius = center_y * GRADCLOCK_RADIUS * 2.0;
		self->priv->min_radius = center_y * GRADCLOCK_RADIUS * GRADCLOCK_RADIUS_2 * 2.0;
		self->priv->hour_radius = center_y * GRADCLOCK_RADIUS * GRADCLOCK_RADIUS_4 * 2.0;
	}
	else
	{
		gdouble aspect = ((gdouble) width) / ((gdouble) height),
		        center_x = aspect * 0.5;
		glOrtho(-aspect * 0.5, aspect * 0.5, -0.5, 0.5, 0.0, 1.0);
		self->priv->radius = ((gdouble) width) * GRADCLOCK_RADIUS;
		self->priv->sec_radius = center_x * GRADCLOCK_RADIUS * 2.0;
		self->priv->min_radius = center_x * GRADCLOCK_RADIUS_2 * 2.0;
		self->priv->hour_radius = center_x * GRADCLOCK_RADIUS_4 * 2.0;
	}
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	gradclock_plugin_update_textures(self);
}

static void
gradclock_plugin_prepare_paint(LwWallpaper *plugin, gint ms_since_last_paint)
{
	GradClockPlugin *self = GRADCLOCK_PLUGIN(plugin);
	time_t timer;
	struct tm *datetime_now;
	time(&timer);
	datetime_now = localtime(&timer);
	if (self->priv->tm_sec != datetime_now->tm_sec)
	{
		self->priv->tm_sec = datetime_now->tm_sec;
		self->priv->tm_msec = 0;
		self->priv->upd_sec = 1;
	}
	else if (self->priv->upd_sec)
	{
		self->priv->tm_msec += ms_since_last_paint;
		if(self->priv->tm_msec >= anim_msec)
		{
			self->priv->tm_msec = anim_msec - 1;
			self->priv->upd_sec = 0;
			self->priv->upd_min = 0;
			self->priv->upd_hour = 0;
			self->priv->per_sec = ((gdouble) self->priv->tm_sec) / 60.0;
			self->priv->per_min = ((gdouble) self->priv->tm_min) / 60.0;
			self->priv->per_hour = ((gdouble) self->priv->tm_hour) / 12.0 - (self->priv->tm_hour >= 12 ? 1.0 : 0.0);
		}
	}
	if (self->priv->tm_min != datetime_now->tm_min)
	{
		self->priv->tm_min = datetime_now->tm_min;
		self->priv->upd_min = 1;
	}
	if (self->priv->tm_hour != datetime_now->tm_hour)
	{
		self->priv->tm_hour = datetime_now->tm_hour;
		self->priv->upd_hour = 1;
	}
	if (self->priv->upd_sec)
		self->priv->per_sec = ((gdouble) self->priv->tm_sec + self->priv->anim_delta[self->priv->tm_msec] - 1.0) / 60.0;

	if (self->priv->upd_min)
		self->priv->per_min = ((gdouble) self->priv->tm_min + self->priv->anim_delta[self->priv->tm_msec] - 1.0) / 60.0;

	if (self->priv->upd_hour)
		self->priv->per_hour = ((gdouble) self->priv->tm_hour + self->priv->anim_delta[self->priv->tm_msec] - 1.0) / 12.0 - (self->priv->tm_hour >= 12 ? 1.0 : 0.0);
}

#define DRAW_TEXTURE_TO_TARGET(x1, y1, x2, y2)	\
	glBegin(GL_QUADS);	\
		glTexCoord2d(1.0, 0.0);	\
		glVertex2f(x2, y2);	\
		glTexCoord2d(0.0, 0.0);	\
		glVertex2f(x1, y2);	\
		glTexCoord2d(0.0, 1.0);	\
		glVertex2f(x1, y1);	\
		glTexCoord2d(1.0, 1.0);	\
		glVertex2f(x2, y1);	\
	glEnd()

static void
gradclock_plugin_paint(LwWallpaper *plugin, LwOutput *output)
{
	GradClockPlugin *self = GRADCLOCK_PLUGIN(plugin);

	if (self->priv->tex_sec == NULL || self->priv->tex_min == NULL || self->priv->tex_hour == NULL)
		return;

	/* Clear color buffer and draw background image */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	lw_background_draw(self->priv->background, output);
	
	glRotatef (-self->priv->per_sec * 360, 0, 0, 1);
	lw_texture_enable(LW_TEXTURE(self->priv->tex_sec));
	DRAW_TEXTURE_TO_TARGET(-self->priv->sec_radius, -self->priv->sec_radius, self->priv->sec_radius, self->priv->sec_radius);
	lw_texture_disable(LW_TEXTURE(self->priv->tex_sec));
	glRotatef (self->priv->per_sec * 360, 0, 0, 1);
	
	glRotatef (-self->priv->per_min * 360, 0, 0, 1);
	lw_texture_enable(LW_TEXTURE(self->priv->tex_min));
	DRAW_TEXTURE_TO_TARGET(-self->priv->min_radius, -self->priv->min_radius, self->priv->min_radius, self->priv->min_radius);
	lw_texture_disable(LW_TEXTURE(self->priv->tex_min));
	glRotatef (self->priv->per_min * 360, 0, 0, 1);
	
	glRotatef (-self->priv->per_hour * 360, 0, 0, 1);
	lw_texture_enable(LW_TEXTURE(self->priv->tex_hour));
	DRAW_TEXTURE_TO_TARGET(-self->priv->hour_radius, -self->priv->hour_radius, self->priv->hour_radius, self->priv->hour_radius);
	lw_texture_disable(LW_TEXTURE(self->priv->tex_hour));
	glRotatef (self->priv->per_hour * 360, 0, 0, 1);
}

static void
gradclock_plugin_restore_viewport(G_GNUC_UNUSED LwWallpaper *plugin)
{
	/* Restore the coordinate system */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	/* Restore the original perspective */
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* Restore texture environment */
	glPopAttrib();

	/* Restore blending */
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

static void
gradclock_plugin_init(GradClockPlugin *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, GRADCLOCK_TYPE_PLUGIN,
	                                         GradClockPluginPrivate);

	self->priv->tex_sec = NULL;
}

static void
gradclock_plugin_init_plugin(LwWallpaper *plugin)
{
	GradClockPlugin *self = GRADCLOCK_PLUGIN(plugin);
	self->priv->settings = g_settings_new("net.launchpad.livewallpaper.plugins.gradclock");

	/* Load background image */
	self->priv->primary_color.red = 0x1100;
	self->priv->primary_color.green = 0x1100;
	self->priv->primary_color.blue = 0x1100;
	self->priv->secondary_color.red = 0x3300;
	self->priv->secondary_color.green = 0x3300;
	self->priv->secondary_color.blue = 0x3300;
	self->priv->background = lw_background_new_from_colors(&self->priv->primary_color, &self->priv->secondary_color, LwBackgroundVertical);

	LW_BIND_COLOR(self, "color-sec");
	LW_BIND_COLOR(self, "color-min");
	LW_BIND_COLOR(self, "color-hour");
	LW_BIND(self, "start-smoothness");
	LW_BIND(self, "end-smoothness");

	self->priv->upd_sec = 0;
	self->priv->upd_min = 0;
	self->priv->upd_hour = 0;
	gradclock_plugin_update_settings(self);
}

static void
gradclock_plugin_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GradClockPlugin *self = GRADCLOCK_PLUGIN(object);
	switch(property_id)
	{
		case COLOR_SECOND:
			self->priv->color_sec = *((GdkRGBA*)g_value_get_boxed(value));
			gradclock_plugin_update_textures(self);
			break;
		case COLOR_MINUTE:
			self->priv->color_min = *((GdkRGBA*)g_value_get_boxed(value));
			gradclock_plugin_update_textures(self);
			break;
		case COLOR_HOUR:
			self->priv->color_hour = *((GdkRGBA*)g_value_get_boxed(value));
			gradclock_plugin_update_textures(self);
			break;
		case START_SMOOTHNESS:
			self->priv->start_smoothness = g_value_get_double(value);
			gradclock_plugin_update_settings(self);
			break;
		case END_SMOOTHNESS:
			self->priv->end_smoothness = g_value_get_double(value);
			gradclock_plugin_update_settings(self);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
gradclock_plugin_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GradClockPlugin *self = GRADCLOCK_PLUGIN(object);
	switch(property_id)
	{
		case COLOR_SECOND:
			g_value_set_boxed(value, &self->priv->color_sec);
			break;
		case COLOR_MINUTE:
			g_value_set_boxed(value, &self->priv->color_min);
			break;
		case COLOR_HOUR:
			g_value_set_boxed(value, &self->priv->color_hour);
			break;
		case START_SMOOTHNESS:
			g_value_set_double(value, self->priv->start_smoothness);
			break;
		case END_SMOOTHNESS:
			g_value_set_double(value, self->priv->end_smoothness);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
gradclock_plugin_dispose(GObject *object)
{
	GradClockPlugin *self = GRADCLOCK_PLUGIN(object);

	if(self->priv->settings)
	{
		g_object_unref(self->priv->settings);
		self->priv->settings = NULL;
	}
	
	if(self->priv->background)
	{
		g_object_unref(self->priv->background);
		self->priv->background = NULL;
	}
	
	if (self->priv->tex_sec)
	{
		g_object_unref (self->priv->tex_sec);
		self->priv->tex_sec = NULL;
	}
	
	if (self->priv->tex_min)
	{
		g_object_unref (self->priv->tex_min);
		self->priv->tex_min = NULL;
	}
	
	if (self->priv->tex_hour)
	{
		g_object_unref (self->priv->tex_hour);
		self->priv->tex_hour = NULL;
	}

	/* Chain up to the parent class */
	G_OBJECT_CLASS(gradclock_plugin_parent_class)->dispose(object);
}

static void
gradclock_plugin_class_init(GradClockPluginClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = gradclock_plugin_set_property;
	gobject_class->get_property = gradclock_plugin_get_property;
	gobject_class->dispose = gradclock_plugin_dispose;

	g_type_class_add_private(klass, sizeof(GradClockPluginPrivate));

	obj_properties[COLOR_SECOND]     = g_param_spec_boxed ("color-sec",        "Second color",     "Color of the second circle",               GDK_TYPE_RGBA,    G_PARAM_READWRITE);
	obj_properties[COLOR_MINUTE]     = g_param_spec_boxed ("color-min",        "Minute color",     "Color of the minute circle",               GDK_TYPE_RGBA,    G_PARAM_READWRITE);
	obj_properties[COLOR_HOUR]       = g_param_spec_boxed ("color-hour",       "Hour color",       "Color of the hour circle",                 GDK_TYPE_RGBA,    G_PARAM_READWRITE);
	obj_properties[START_SMOOTHNESS] = g_param_spec_double("start-smoothness", "Start smoothness", "Smoothness at the start of the animation", 0.0f, 1.0f, 0.5f, G_PARAM_READWRITE);
	obj_properties[END_SMOOTHNESS]   = g_param_spec_double("end-smoothness",   "End smoothness",   "Smoothness at the end of the animation",   0.0f, 1.0f, 0.5f, G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}

static void
gradclock_plugin_class_finalize(G_GNUC_UNUSED GradClockPluginClass *klass)
{

}

static void
lw_wallpaper_iface_init(LwWallpaperInterface *iface)
{
	iface->init_plugin = gradclock_plugin_init_plugin;

	iface->adjust_viewport = gradclock_plugin_adjust_viewport;
	iface->prepare_paint = gradclock_plugin_prepare_paint;
	iface->paint = gradclock_plugin_paint;
	iface->restore_viewport = gradclock_plugin_restore_viewport;
}

G_MODULE_EXPORT void
peas_register_types(PeasObjectModule *module)
{
	gradclock_plugin_register_type(G_TYPE_MODULE(module));
	peas_object_module_register_extension_type(module, LW_TYPE_WALLPAPER, GRADCLOCK_TYPE_PLUGIN);
}

