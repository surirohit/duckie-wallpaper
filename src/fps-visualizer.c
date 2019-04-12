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
 * Copyright (C) 2013-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

#include "config.h"

#include <glib.h>
#include <gio/gio.h>
#include <glib/gi18n.h>

#include <livewallpaper/core.h>

#include "clock.h"
#include "fps-visualizer.h"


struct _LwFPSVisualizerPrivate
{
	GSettings *settings;
	LwClock *clock;

	gboolean show_fps;

	LwCairoTexture *tex;
	guint current_fps;
	guint fps_width;
	guint fps_height;

    gchar *font;
    GdkRGBA bg_color;
	GdkRGBA fg_color;
};

enum
{
	PROP_0,

	PROP_SHOW_FPS,

    PROP_FONT,
    PROP_BG_COLOR,
    PROP_FG_COLOR,

	N_PROPERTIES
};

G_DEFINE_TYPE(LwFPSVisualizer, lw_fps_visualizer, G_TYPE_OBJECT)

LwFPSVisualizer*
lw_fps_visualizer_new(LwClock *clock)
{
	LwFPSVisualizer *self = g_object_new(LW_TYPE_FPS_VISUALIZER, NULL);

	self->priv->clock = g_object_ref(clock);

	return self;
}

static void
lw_fps_visualizer_set_property(GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
	LwFPSVisualizer *self = LW_FPS_VISUALIZER(object);

	switch(property_id)
	{
		case PROP_SHOW_FPS:
			self->priv->show_fps = g_value_get_boolean(value);
			break;

        case PROP_FONT:
            g_free(self->priv->font);
			self->priv->font = g_strdup(g_value_get_string(value));
            break;

        case PROP_BG_COLOR:
        	self->priv->bg_color = *((GdkRGBA*) g_value_get_boxed(value));
            break;

        case PROP_FG_COLOR:
            self->priv->fg_color = *((GdkRGBA*) g_value_get_boxed(value));
            break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
    self->priv->current_fps = -1;   /* Force the texture update */
}

static void
lw_fps_visualizer_get_property(GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec)
{
	LwFPSVisualizer *self = LW_FPS_VISUALIZER(object);

	switch(property_id)
	{
		case PROP_SHOW_FPS:
			g_value_set_boolean(value, self->priv->show_fps);
			break;

        case PROP_FONT:
            g_value_set_string(value, self->priv->font);
            break;

        case PROP_BG_COLOR:
            g_value_set_boxed(value, &self->priv->bg_color);
            break;

        case PROP_FG_COLOR:
            g_value_set_boxed(value, &self->priv->fg_color);
            break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static cairo_font_slant_t
pango_font_description_get_cairo_font_slant(PangoFontDescription *pfd)
{
	switch(pango_font_description_get_style(pfd))
	{
		case PANGO_STYLE_OBLIQUE:
			return CAIRO_FONT_SLANT_OBLIQUE;

		case PANGO_STYLE_ITALIC:
			return CAIRO_FONT_SLANT_ITALIC;

		case PANGO_STYLE_NORMAL:
		default:
			return CAIRO_FONT_SLANT_NORMAL;
	}
}

static cairo_font_weight_t
pango_font_description_get_cairo_font_weight(PangoFontDescription *pfd)
{
	if(pango_font_description_get_weight(pfd) > PANGO_WEIGHT_MEDIUM)
		return CAIRO_FONT_WEIGHT_BOLD;
	else
		return CAIRO_FONT_WEIGHT_NORMAL;
}

static void
lw_fps_visualizer_update_tex(LwFPSVisualizer *self)
{
    PangoFontDescription *pfd = pango_font_description_from_string(self->priv->font);
	cairo_t *cr = lw_cairo_texture_cairo_create(self->priv->tex);
	cairo_text_extents_t extents;
	guint fps_width, fps_height;
	gchar s[16];

	self->priv->current_fps = lw_clock_get_fps(self->priv->clock);
	g_snprintf(s, 16, _("%d FPS"), self->priv->current_fps);

	/* Select font */
	cairo_select_font_face(cr, pango_font_description_get_family(pfd),
                           pango_font_description_get_cairo_font_slant(pfd),
                           pango_font_description_get_cairo_font_weight(pfd));
	cairo_set_font_size(cr, pango_font_description_get_size (pfd)  / PANGO_SCALE);

	/* Get size of text */
	cairo_text_extents(cr, s, &extents);
	fps_width  = 20 + extents.width;
	fps_height = 20 + extents.height;

	if(fps_width  > 256) fps_width  = 256;
	if(fps_height > 256) fps_height = 256;

	/* Fill background */
	cairo_set_source_rgb(cr, self->priv->bg_color.red, self->priv->bg_color.green, self->priv->bg_color.blue);
	cairo_rectangle(cr, 0, 0, fps_width, fps_height);
	cairo_fill(cr);

	/* Show text */
	cairo_set_source_rgb(cr, self->priv->fg_color.red, self->priv->fg_color.green, self->priv->fg_color.blue);
	cairo_move_to(cr, 10 - extents.x_bearing, 10 + extents.height);
	cairo_show_text(cr, s);

	cairo_destroy(cr);
	lw_cairo_texture_update(self->priv->tex);
	self->priv->fps_width  = fps_width;
	self->priv->fps_height = fps_height;

    pango_font_description_free (pfd);
}

void
lw_fps_visualizer_paint(LwFPSVisualizer *self, LwOutput *output)
{
	guint output_width  = lw_output_get_width(output),
	      output_height = lw_output_get_height(output),
	      fps_width, fps_height;
	gboolean was_blending_enabled = FALSE;
	LwTextureMatrix m;

	if(!self->priv->show_fps) return;

	/* Update texture? */
	if(self->priv->current_fps != lw_clock_get_fps(self->priv->clock))
		lw_fps_visualizer_update_tex(self);

	fps_width  = self->priv->fps_width;
	fps_height = self->priv->fps_height;

	/* Adjust viewport */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, output_width, output_height, 0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if(glIsEnabled(GL_BLEND)) was_blending_enabled = TRUE;
	glDisable(GL_BLEND);

	glColor3f(1.0f, 1.0f, 1.0f);

	/* Paint texture */
	lw_texture_enable(LW_TEXTURE(self->priv->tex));
	m = LW_TEXTURE(self->priv->tex)->matrix;

	glBegin(GL_QUADS);
		glTexCoord2f(
			LW_TEX_COORD_X(m, 0.0f),
			LW_TEX_COORD_Y(m, 0.0f)
		);
		glVertex2f(output_width - fps_width, output_height - fps_height);

		glTexCoord2f(
			LW_TEX_COORD_X(m, 0.0f),
			LW_TEX_COORD_Y(m, fps_height)
		);
		glVertex2f(output_width - fps_width, output_height);

		glTexCoord2f(
			LW_TEX_COORD_X(m, fps_width),
			LW_TEX_COORD_Y(m, fps_height)
		);
		glVertex2f(output_width, output_height);

		glTexCoord2f(
			LW_TEX_COORD_X(m, fps_width),
			LW_TEX_COORD_Y(m, 0.0f)
		);
		glVertex2f(output_width, output_height - fps_height);
	glEnd();

	lw_texture_disable(LW_TEXTURE(self->priv->tex));

	/* Restore viewport */
	if(was_blending_enabled) glEnable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static void
lw_fps_visualizer_init(LwFPSVisualizer *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_FPS_VISUALIZER,
	                                         LwFPSVisualizerPrivate);

	self->priv->settings = g_settings_new(LW_SETTINGS);
	g_settings_bind(self->priv->settings, "show-fps",
	                self, "show-fps", G_SETTINGS_BIND_GET);
	g_settings_bind(self->priv->settings, "fps-font",
	                self, "fps-font", G_SETTINGS_BIND_GET);
    lw_settings_bind_color(self->priv->settings, "fps-bg-color",
	                       self, "fps-bg-color", G_SETTINGS_BIND_GET);
	lw_settings_bind_color(self->priv->settings, "fps-fg-color",
	                       self, "fps-fg-color", G_SETTINGS_BIND_GET);

	self->priv->tex = lw_cairo_texture_new(256, 256);
}

static void
lw_fps_visualizer_dispose(GObject *object)
{
	LwFPSVisualizer *self = LW_FPS_VISUALIZER(object);

	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->clock);
	g_clear_object(&self->priv->tex);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_fps_visualizer_parent_class)->dispose(object);
}

static void
lw_fps_visualizer_class_init(LwFPSVisualizerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_fps_visualizer_set_property;
	gobject_class->get_property = lw_fps_visualizer_get_property;
	gobject_class->dispose = lw_fps_visualizer_dispose;

	g_type_class_add_private(klass, sizeof(LwFPSVisualizerPrivate));

	/* Install properties */
	g_object_class_install_property(gobject_class,
	                                PROP_SHOW_FPS,
	                                g_param_spec_boolean("show-fps",
	                                                     "Show FPS",
	                                                     "Show the current frames per second",
	                                                     FALSE,
	                                                     G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class,
	                                PROP_FONT,
	                                g_param_spec_string("fps-font",
	                                                     "FPS visualizer's font",
	                                                     "The font used to display the current frames per second",
	                                                     "Sans 20",
	                                                     G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class,
	                                PROP_BG_COLOR,
                                    g_param_spec_boxed("fps-bg-color",
                                                       "FPS visualizer's background color",
                                                       "Color used as background for the area which shows the current frames per second",
                                                       GDK_TYPE_RGBA,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class,
	                                PROP_FG_COLOR,
                                    g_param_spec_boxed("fps-fg-color",
                                                       "FPS visualizer's font color",
                                                       "Color used to display the current frames per second",
                                                       GDK_TYPE_RGBA,
                                                       G_PARAM_READWRITE));
}
