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
 * Copyright (C) 2012-2016 Aurélien   Rivière <aurelien.riv@gmail.com>
 *
 */

#include <glib-object.h>
#include <livewallpaper/core.h>
#include "light.h"

#define DUCKIEGALAXY_SHADER "resource:///net/launchpad/livewallpaper/plugins/duckiegalaxy/shader/"

struct _DuckieGalaxyLightProgramPrivate
{
	LwHSL *outer_color;
	LwHSL *inner_color;

	gdouble color_radius;
};

enum
{
    PROP_0,

	PROP_OUTER_COLOR,
	PROP_INNER_COLOR,

	PROP_COLOR_RADIUS,

    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

G_DEFINE_TYPE(DuckieGalaxyLightProgram, duckiegalaxy_light_program, LW_TYPE_PROGRAM)


DuckieGalaxyLightProgram*
duckiegalaxy_light_program_new()
{
	DuckieGalaxyLightProgram* prog = g_object_new(DUCKIEGALAXY_TYPE_LIGHT_PROGRAM, NULL);

	lw_program_create_and_attach_shader_from_resource (LW_PROGRAM(prog), DUCKIEGALAXY_SHADER "vert.glsl", GL_VERTEX_SHADER);
	lw_program_create_and_attach_shader_from_resource (LW_PROGRAM(prog), DUCKIEGALAXY_SHADER "frag.glsl", GL_FRAGMENT_SHADER);
	lw_program_link(LW_PROGRAM(prog));

	return prog;
}

static void
duckiegalaxy_light_program_set_property(GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec)
{
	DuckieGalaxyLightProgram *self = DUCKIEGALAXY_LIGHT_PROGRAM(object);

	switch(property_id)
	{
		case PROP_OUTER_COLOR:
			lw_hsl_free(self->priv->outer_color);
			self->priv->outer_color = lw_rgb_to_hsl(g_value_get_boxed(value));
			break;

		case PROP_INNER_COLOR:
			lw_hsl_free(self->priv->inner_color);
			self->priv->inner_color = lw_rgb_to_hsl(g_value_get_boxed(value));
			break;

		case PROP_COLOR_RADIUS:
			self->priv->color_radius = g_value_get_double(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
duckiegalaxy_light_program_get_property(GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec)
{
	DuckieGalaxyLightProgram *self = DUCKIEGALAXY_LIGHT_PROGRAM(object);

	switch(property_id)
	{
		case PROP_OUTER_COLOR:
			g_value_take_boxed(value, lw_hsl_to_rgb(self->priv->outer_color));
			break;

		case PROP_INNER_COLOR:
			g_value_take_boxed(value, lw_hsl_to_rgb(self->priv->inner_color));
			break;

		case PROP_COLOR_RADIUS:
			g_value_set_double(value, self->priv->color_radius);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

void
duckiegalaxy_light_program_set_uniform(DuckieGalaxyLightProgram *self)
{
	int location = lw_program_get_uniform_location(LW_PROGRAM(self), "innerColor");
#define color self->priv->inner_color
	glUniform3f(location, color->hue, color->saturation, color->lightness);
#undef color

	location = lw_program_get_uniform_location(LW_PROGRAM(self), "outerColor");
#define color self->priv->outer_color
	glUniform3f(location, color->hue, color->saturation, color->lightness);
#undef color

	location = lw_program_get_uniform_location(LW_PROGRAM(self), "colorRadius");
	glUniform1f(location, self->priv->color_radius);
}

static void
duckiegalaxy_light_program_init(DuckieGalaxyLightProgram *self)
{
	LwHSL color = {1.0, 1.0, 1.0};
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, DUCKIEGALAXY_TYPE_LIGHT_PROGRAM,
	                                         DuckieGalaxyLightProgramPrivate);

	self->priv->outer_color = lw_hsl_copy(&color);
	self->priv->inner_color = lw_hsl_copy(&color);
	self->priv->color_radius = 1.0;
}

static void
duckiegalaxy_light_program_finalize(GObject *object)
{
	DuckieGalaxyLightProgram *self = DUCKIEGALAXY_LIGHT_PROGRAM(object);

	lw_hsl_free(self->priv->outer_color);
	lw_hsl_free(self->priv->inner_color);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(duckiegalaxy_light_program_parent_class)->finalize(object);
}

static void
duckiegalaxy_light_program_class_init(DuckieGalaxyLightProgramClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = duckiegalaxy_light_program_set_property;
	gobject_class->get_property = duckiegalaxy_light_program_get_property;
	gobject_class->finalize = duckiegalaxy_light_program_finalize;

	g_type_class_add_private(klass, sizeof(DuckieGalaxyLightProgramPrivate));

    obj_properties[PROP_OUTER_COLOR]  = g_param_spec_boxed ("outer-color",  "Outer Color",  "Color of the outer parts of the galaxy", GDK_TYPE_RGBA, G_PARAM_READWRITE);
    obj_properties[PROP_INNER_COLOR]  = g_param_spec_boxed ("inner-color",  "Inner Color",  "Color in the centre of the galaxy",      GDK_TYPE_RGBA, G_PARAM_READWRITE);
    obj_properties[PROP_COLOR_RADIUS] = g_param_spec_double("color-radius", "Color radius", "Radius of the radial color gradient",    0.0, 2.0, 1.0, G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}
