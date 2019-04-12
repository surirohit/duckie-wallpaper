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

/**
 * SECTION: output
 * @Short_description: Onscreen output area
 *
 * The LwOutput object specifies the position and the size of an onscreen
 * output area. Usually one LwOutput will be created for each physical monitor.
 */

#include <livewallpaper/core.h>


struct _LwOutputPrivate
{
	guint id;
	guint x;
	guint y;
	guint width;
	guint height;
};

enum
{
	PROP_0,

	PROP_ID,
	PROP_X,
	PROP_Y,
	PROP_WIDTH,
	PROP_HEIGHT,

	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

/**
 * LwOutput:
 *
 * Represents an output area. One #LwOutput will be created for each physical
 * monitor.
 *
 * If LiveWallpaper recognizes a new screen or if the screen resolution changes,
 * all outputs will be destroyed and LiveWallpaper will create new outputs. Just
 * keep this in mind if you associate data with a specific output, because the
 * output can be destroyed and you don't recognize it.
 * <note>
 *   <para>
 *     LiveWallpaper will never destroy an output without calling lw_wallpaper_restore_viewport().
 *   </para>
 * </note>
 */

G_DEFINE_TYPE(LwOutput, lw_output, G_TYPE_OBJECT)


static void
lw_output_set_property(GObject *object,
                       guint property_id,
                       const GValue *value,
                       GParamSpec *pspec)
{
	LwOutput *self = LW_OUTPUT(object);

	switch(property_id)
	{
		case PROP_X:
			self->priv->x = g_value_get_uint(value);
			break;

		case PROP_Y:
			self->priv->y = g_value_get_uint(value);
			break;

		case PROP_WIDTH:
			self->priv->width = g_value_get_uint(value);
			break;

		case PROP_HEIGHT:
			self->priv->height = g_value_get_uint(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_output_get_property(GObject *object,
                       guint property_id,
                       GValue *value,
                       GParamSpec *pspec)
{
	LwOutput *self = LW_OUTPUT(object);

	switch(property_id)
	{
		case PROP_ID:
			g_value_set_uint(value, lw_output_get_id(self));
			break;

		case PROP_X:
			g_value_set_uint(value, lw_output_get_x(self));
			break;

		case PROP_Y:
			g_value_set_uint(value, lw_output_get_y(self));
			break;

		case PROP_WIDTH:
			g_value_set_uint(value, lw_output_get_width(self));
			break;

		case PROP_HEIGHT:
			g_value_set_uint(value, lw_output_get_height(self));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

/**
 * lw_output_get_id:
 * @self: A #LwOutput
 *
 * Since LiveWallpaper 0.5 the size and position of an output cannot change. You can use
 * the ID to identify exactly this output.
 *
 * Returns: The ID of the output
 */
guint
lw_output_get_id(LwOutput *self)
{
	return self->priv->id;
}

/**
 * lw_output_get_x:
 * @self: A #LwOutput
 *
 * Returns: The position of the output in x direction
 */
guint
lw_output_get_x(LwOutput *self)
{
	return self->priv->x;
}

/**
 * lw_output_get_y:
 * @self: A #LwOutput
 *
 * Returns: The position of the output in y direction
 */
guint
lw_output_get_y(LwOutput *self)
{
	return self->priv->y;
}

/**
 * lw_output_get_width:
 * @self: A #LwOutput
 *
 * Returns: The width of the output
 */
guint
lw_output_get_width(LwOutput *self)
{
	return self->priv->width;
}

/**
 * lw_output_get_height:
 * @self: A #LwOutput
 *
 * Returns: The height of the output
 */
guint
lw_output_get_height(LwOutput *self)
{
	return self->priv->height;
}

/**
 * lw_output_get_aspect_ratio:
 * @self: A #LwOutput
 *
 * Returns the quotient of width and height if both are > 0 and 0 otherwise.
 *
 * Returns: width / height
 */
gdouble
lw_output_get_aspect_ratio(LwOutput *self)
{
	if(self->priv->width == 0 || self->priv->height == 0)
		return 0;

	return ((gdouble) self->priv->width) / ((gdouble) self->priv->height);
}

/**
 * lw_output_get_longest_side:
 * @self: A #LwOutput
 *
 * Returns the width if width > height, otherwise the height.
 *
 * Returns: The longest side in pixels
 *
 * Since: 0.5
 */
guint
lw_output_get_longest_side(LwOutput *self)
{
	if(self->priv->width > self->priv->height)
		return self->priv->width;
	else
		return self->priv->height;
}

/**
 * lw_output_get_shortest_side:
 * @self: A #LwOutput
 *
 * Returns the width, if width < height, otherwise the height.
 *
 * Returns: The shortest side in pixels
 *
 * Since: 0.5
 */
guint
lw_output_get_shortest_side(LwOutput *self)
{
	if(self->priv->height < self->priv->width)
		return self->priv->height;
	else
		return self->priv->width;
}

/**
 * lw_output_make_current:
 * @self: A #LwOutput
 *
 * Adjusts the OpenGL viewport to fit to the output. It also restricts the drawing
 * operations to the viewport using <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glScissor.xml">glScissor</ulink>.
 * LiveWallpaper will call this for you so you don't have to do this again in
 * lw_wallpaper_adjust_viewport().
 *
 * Since: 0.5
 */
void
lw_output_make_current(LwOutput *self)
{
	glViewport(self->priv->x, self->priv->y,
		self->priv->width, self->priv->height);
	glScissor(self->priv->x, self->priv->y,
		self->priv->width, self->priv->height);
}

static guint next_id = 0;

static void
lw_output_init(LwOutput *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_OUTPUT,
	                                         LwOutputPrivate);

	self->priv->id = next_id++;
}

static void
lw_output_class_init(LwOutputClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_output_set_property;
	gobject_class->get_property = lw_output_get_property;

	g_type_class_add_private(klass, sizeof(LwOutputPrivate));

	/**
	 * LwOutput:id:
	 *
	 * ID of the output
	 */
	obj_properties[PROP_ID] = g_param_spec_uint("id", "ID", "ID of the output", 0, G_MAXUINT, 0, G_PARAM_READABLE);

	/**
	 * LwOutput:x:
	 *
	 * The X coordinate of the output
	 */
	obj_properties[PROP_X] = g_param_spec_uint("x", "X", "The X coordinate of the output", 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

	/**
	 * LwOutput:y:
	 *
	 * The Y coordinate of the output
	 */
	obj_properties[PROP_Y] = g_param_spec_uint("y", "Y", "The Y coordinate of the output", 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

	/**
	 * LwOutput:width:
	 *
	 * The width of the output
	 */
	obj_properties[PROP_WIDTH] = g_param_spec_uint("width", "Width", "The width of the output", 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

	/**
	 * LwOutput:height:
	 *
	 * The height of the output
	 */
	obj_properties[PROP_HEIGHT] = g_param_spec_uint("height", "Height", "The height of the output", 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}

