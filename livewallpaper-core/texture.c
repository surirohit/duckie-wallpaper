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
 *
 * The LwTexture object is inspired by compiz' GLTexture class.
 *
 */

/**
 * SECTION: texture
 * @Short_description: OpenGL texture wrapper
 *
 * #LwTexture is an easy way to work with OpenGL textures.
 *
 * There are three ways to create a #LwTexture. The easiest way is
 * to load an image file (lw_texture_new_from_file()), but you can also
 * use an existing #GdkPixbuf (lw_texture_new_from_pixbuf()). The last
 * option is to create a texture directly out of in-memory image data
 * (lw_texture_new_from_data()). In each case you should free the #LwTexture
 * if you don't need it anymore by using g_object_unref().
 *
 * <note>
 *   <para>
 *     LwTexture only supports 2D textures at the moment.
 *   </para>
 * </note>
 *
 * Use lw_texture_enable() to bind the texture. This does everything that is
 * needed to use the texture. If you are finished with drawing, use lw_texture_disable().
 *
 * <example>
 *   <title>Using a LwTexture</title>
 *   <programlisting>
 * GError *error = NULL;
 * LwTexture *tex = lw_texture_new_from_file("/path/to/texture.image", &error);
 *
 * if(error != NULL)
 * {
 *     g_warning("Could not load the texture: %s", error->message);
 *     g_error_free(error);
 *     return;
 * }
 *
 * lw_texture_enable(tex);
 *
 * // Draw your textured object here, for example a quad
 * glBegin(GL_QUADS);
 *     glTexCoord2f(...);
 *     glVertex3f(...);
 *     ...
 * glEnd();
 *
 * lw_texture_disable(tex);</programlisting>
 * </example>
 */

#include <livewallpaper/core.h>


/**
 * LW_TEX_COORD_X:
 * @m: A #LwTextureMatrix
 * @px: A X coordinate
 *
 * Transforms the X coordinate by a 2D texture matrix.
 *
 * Returns: A transformed X coordinate
 */

/**
 * LW_TEX_COORD_Y:
 * @m: A #LwTextureMatrix
 * @py: A Y coordinate
 *
 * Transforms the Y coordinate by a 2D texture matrix.
 *
 * Returns: A transformed Y coordinate
 */

/**
 * LW_TEX_COORD_XY:
 * @m: A #LwTextureMatrix
 * @px: A X coordinate
 * @py: A Y coordinate
 *
 * Transforms the X coordinate by a 2D texture matrix.
 *
 * Use this macro if the #LwTextureMatrix rotates the texture coordinates,
 * otherwise use #LW_TEX_COORD_X.
 *
 * Returns: A transformed X coordinate
 */

/**
 * LW_TEX_COORD_YX:
 * @m: A #LwTextureMatrix
 * @px: A X coordinate
 * @py: A Y coordinate
 *
 * Transforms the Y coordinate by a 2D texture matrix.
 *
 * Use this macro if the #LwTextureMatrix rotates the texture coordinates,
 * otherwise use #LW_TEX_COORD_Y.
 *
 * Returns: A transformed Y coordinate
 */

/**
 * LwTextureMatrix:
 * @xx: The xx component (scale and rotation)
 * @yx: The yx component (rotation only)
 * @xy: The xy component (rotation only)
 * @yy: The yy component (scale and rotation)
 * @x0: The offset in x direction
 * @y0: The offset in y direction
 *
 * A 2D matrix to transform texture coordinates.
 */

static LwTextureMatrix identity_texture_matrix = {
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f
};

static gint max_texture_units = 0;

struct _LwTexturePrivate
{
	guint name;
	guint target;
	guint filter;
	guint wrap;

	gint width;
	gint height;

	guint bound_to;
};

enum
{
	PROP_0,

	PROP_TARGET,
	PROP_WIDTH,
	PROP_HEIGHT,

	N_PROPERTIES
};

/**
 * lw_texture_get_max_texture_units:
 *
 * Returns: The value of GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
 *
 * Since: 0.5
 */
gint
lw_texture_get_max_texture_units()
{
	if(max_texture_units == 0)
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);

	return max_texture_units;
}

/**
 * LwTexture:
 * @matrix: The #LwTextureMatrix of this texture
 *
 * A structure for easier OpenGL texture handling.
 */

G_DEFINE_TYPE(LwTexture, lw_texture, G_TYPE_OBJECT)

/**
 * lw_texture_new_from_file:
 * @path: Name of the file to load
 *
 * Creates a new texture by loading an image from a file. This function supports
 * all file formats supported by gdk-pixbuf. If %NULL is returned, then @error will
 * be set. Internally the image file will be loaded into a #GdkPixbuf first before
 * creating the #LwTexture.
 *
 * Returns: A new #LwTexture, or %NULL in case of an error. You should use
 *          g_object_unref() to free the #LwTexture.
 *
 * Since: 0.5
 */
LwTexture*
lw_texture_new_from_file(const gchar *path)
{
    GError *error = NULL;
	LwTexture *texture;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path, &error);
	if(error != NULL)
	{
        g_warning("Could not load the texture: %s", error->message);
		g_error_free(error);
        return NULL;
	}

	texture = lw_texture_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	return texture;
}

/**
 * lw_texture_new_from_resource:
 * @path: Name of the resource to load
 *
 * Creates a new texture by loading an image from a gresource. This function supports
 * all file formats supported by gdk-pixbuf. If %NULL is returned, then @error will
 * be set. Internally the image file will be loaded into a #GdkPixbuf first before
 * creating the #LwTexture.
 *
 * Returns: A new #LwTexture, or %NULL in case of an error. You should use
 *          g_object_unref() to free the #LwTexture.
 *
 * Since: 0.5
 */
LwTexture*
lw_texture_new_from_resource (const gchar *path)
{
    GError *error = NULL;
	LwTexture *texture;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_resource (path, &error);
	if(error != NULL)
    {
        g_warning("Could not load the texture: %s", error->message);
		g_error_free(error);
        return NULL;
	}

	texture = lw_texture_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	return texture;
}

/**
 * lw_texture_new_from_pixbuf:
 * @pixbuf: A GdkPixbuf holding the image data
 *
 * Creates a new texture by using the image data of the #GdkPixbuf.
 *
 * <note>
 *   <para>
 *     You have to free the @pixbuf by yourself. You can free it directly after
 *     calling this function, #LwTexture does not need it anymore.
 *   </para>
 * </note>
 *
 * Returns: A new #LwTexture. You should use g_object_unref() to free the #LwTexture.
 */
LwTexture*
lw_texture_new_from_pixbuf(GdkPixbuf *pixbuf)
{
	guint width  = gdk_pixbuf_get_width(pixbuf),
		  height = gdk_pixbuf_get_height(pixbuf),
	      format = (gdk_pixbuf_get_has_alpha(pixbuf)) ? GL_RGBA : GL_RGB;
	guchar *data = gdk_pixbuf_get_pixels(pixbuf);

	return lw_texture_new_from_data(data, width, height, format, GL_UNSIGNED_BYTE);
}

/**
 * lw_texture_new_from_data:
 * @data: A pointer to the image data
 * @width: Width of the image in pixels
 * @height: Height of the image in pixels
 * @format: Format of the image data
 * @type: Data type of the image data
 *
 * Creates a new texture out of in-memory image data. Internally <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml">glTexImage2D</ulink>
 * is used to create the texture. You can find a more detailed description of
 * @format and @type on the documentation page of <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml">glTexImage2D</ulink>.
 *
 * <note>
 *   <para>
 *     You have to free the image @data by yourself. You can free it directly after
 *     calling this function, #LwTexture does not need it anymore.
 *   </para>
 * </note>
 *
 * Returns: A new #LwTexture. You should use g_object_unref() to free the #LwTexture.
 */
LwTexture*
lw_texture_new_from_data(const guchar *data,
                         guint width,
                         guint height,
                         guint format,
                         guint type)
{
	LwTexture *texture;

	texture = g_object_new(LW_TYPE_TEXTURE,
	                       "target", GL_TEXTURE_2D,
	                       "width", width,
	                       "height", height,
	                       NULL);

	glBindTexture(lw_texture_get_target(texture),
	              lw_texture_get_name(texture));

	glTexImage2D(lw_texture_get_target(texture),
	             0,
	             GL_RGBA,
	             width, height,
	             0,
	             format,
	             type,
	             data);

	lw_texture_set_filter(texture, GL_NEAREST);
	lw_texture_set_wrap(texture, GL_CLAMP_TO_EDGE);

	return texture;
}

static void
lw_texture_set_property(GObject *object,
                        guint property_id,
                        const GValue *value,
                        GParamSpec *pspec)
{
	LwTexture *self = LW_TEXTURE(object);

	switch(property_id)
	{
		case PROP_TARGET:
			self->priv->target = g_value_get_uint(value);
			break;

		case PROP_WIDTH:
			self->priv->width = g_value_get_uint(value);
			self->matrix.xx = 1.0f / self->priv->width;
			break;

		case PROP_HEIGHT:
			self->priv->height = g_value_get_uint(value);
			self->matrix.yy = 1.0f / self->priv->height;
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_texture_get_property(GObject *object,
                        guint property_id,
                        GValue *value,
                        GParamSpec *pspec)
{
	LwTexture *self = LW_TEXTURE(object);

	switch(property_id)
	{
		case PROP_TARGET:
			g_value_set_uint(value, self->priv->target);
			break;

		case PROP_WIDTH:
			g_value_set_uint(value, self->priv->width);
			break;

		case PROP_HEIGHT:
			g_value_set_uint(value, self->priv->height);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

/**
 * lw_texture_get_name:
 * @self: A #LwTexture
 *
 * Returns: The texture name returned by <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glGenTextures.xml">glGenTextures</ulink>
 */
guint
lw_texture_get_name(LwTexture *self)
{
	return self->priv->name;
}

/**
 * lw_texture_get_target:
 * @self: A #LwTexture
 *
 * Returns the texture's target. At the moment only GL_TEXTURE_2D is supported, so this function
 * always returns GL_TEXTURE_2D.
 *
 * Returns: The texture target
 */
guint
lw_texture_get_target(LwTexture *self)
{
	return self->priv->target;
}

/**
 * lw_texture_set_filter:
 * @self: A #LwTexture
 * @filter: The new texture filter
 *
 * Sets the texture's minifying and magnification function. Possible values are GL_NEAREST and
 * GL_LINEAR. GL_NEAREST is generally faster than GL_LINEAR, but it produces sharper edges.
 * #LwTexture uses GL_NEAREST by default.
 *
 * To get a more detailed description of all possible filters, take a look at the documentation
 * page of <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml">glTexParameter</ulink>.
 * This functions sets the GL_TEXTURE_MIN_FILTER and GL_TEXTURE_MAG_FILTER at the same time, so
 * only filters supported by both are vaild arguments.
 */
void
lw_texture_set_filter(LwTexture *self, guint filter)
{
	glBindTexture(self->priv->target, self->priv->name);

	self->priv->filter = filter;

	glTexParameteri(self->priv->target, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(self->priv->target, GL_TEXTURE_MAG_FILTER, filter);

	glBindTexture(self->priv->target, 0);
}

/**
 * lw_texture_get_filter:
 * @self: A #LwTexture
 *
 * Returns: The currently applied texture filter
 */
guint
lw_texture_get_filter(LwTexture *self)
{
	return self->priv->filter;
}

/**
 * lw_texture_set_wrap:
 * @self: A #LwTexture
 * @wrap: The new wrap parameter
 *
 * Sets the wrap parameter for texture coordinate s and t. The wrap parameter is set
 * to GL_CLAMP_TO_EDGE by default.
 *
 * This functions calls <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml">glTexParameter</ulink>
 * for GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_T. You can find a complete list of all supported
 * parameters at the documentation page of <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml">glTexParameter</ulink>.
 */
void
lw_texture_set_wrap(LwTexture *self, guint wrap)
{
	glBindTexture(self->priv->target, self->priv->name);

	self->priv->wrap = wrap;

	glTexParameteri(self->priv->target, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(self->priv->target, GL_TEXTURE_WRAP_T, wrap);

	glBindTexture(self->priv->target, 0);
}

/**
 * lw_texture_get_wrap:
 * @self: A #LwTexture
 *
 * Returns: The current wrap parameter
 */
guint
lw_texture_get_wrap(LwTexture *self)
{
	return self->priv->wrap;
}

/**
 * lw_texture_get_width:
 * @self: A #LwTexture
 *
 * Returns: The width of the texture
 */
guint
lw_texture_get_width(LwTexture *self)
{
	return self->priv->width;
}

/**
 * lw_texture_get_height:
 * @self: A #LwTexture
 *
 * Returns: The height of the texture
 */
guint
lw_texture_get_height(LwTexture *self)
{
	return self->priv->height;
}

/**
 * lw_texture_enable:
 * @self: A #LwTexture
 *
 * Enables the texture target.
 */
void
lw_texture_enable(LwTexture *self)
{
	glEnable(self->priv->target);
	lw_texture_bind(self);
}

/**
 * lw_texture_disable:
 * @self: A #LwTexture
 *
 * Disables the texture target.
 */
void
lw_texture_disable(LwTexture *self)
{
	lw_texture_unbind(self);
	glDisable(self->priv->target);
}

/**
 * lw_texture_bind:
 * @self: A #LwTexture
 *
 * Uses lw_texture_bind_to() to bind this texture to the texture unit 0. If you just need one texture
 * this is the function to use. If you want to use multiple textures with a fragment shader, take a look
 * at lw_program_set_texture() or lw_texture_bind_to().
 *
 * Since: 0.5
 */
void
lw_texture_bind(LwTexture *self)
{
	lw_texture_bind_to(self, 0);
}

/**
 * lw_texture_bind_to:
 * @self: A #LwTexture
 * @unit: The texture unit to use from 0 to lw_texture_get_max_texture_units()
 *
 * Binds the texture @self to the specified texture unit. If you just need one texture, the
 * lw_texture_bind() function might be simpler to use. You usually do not have to use this
 * function directly if you use lw_program_set_texture(). 
 *
 * Before binding the specified texture to its target using <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml">glBindTexture</ulink>,
 * it switches to the specified texture unit using <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glActiveTexture.xml">glActiveTexture</ulink>.
 *
 * Returns: %TRUE on success and %FALSE if @unit is not a valid texture unit
 *
 * Since: 0.5
 */
gboolean
lw_texture_bind_to(LwTexture *self, guint unit)
{
	if((int) unit >= max_texture_units)
	{
		g_warning("lw_texture_bind_to(): Could not bind to texture unit %d, "
		          "only %d texture units availabe", unit, max_texture_units);
		return FALSE;
	}

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(self->priv->target, self->priv->name);
	self->priv->bound_to = unit;

	return TRUE;
}

/**
 * lw_texture_unbind:
 * @self: A #LwTexture
 *
 * Unbinds the texture by binding 0 to the texture's target. This operation
 * switches to the texture unit specified by lw_texture_bind() or lw_texture_bind_to()
 * using <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glActiveTexture.xml">glActiveTexture</ulink>.
 * This function unbinds every texture currently bound to the same target and texture unit of @self,
 * even if @self is not the active texture.
 *
 * Since: 0.5
 */
void
lw_texture_unbind(LwTexture *self)
{
	glActiveTexture(GL_TEXTURE0 + self->priv->bound_to);
	glBindTexture(self->priv->target, 0);
}

static void
lw_texture_init(LwTexture *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_TEXTURE,
	                                         LwTexturePrivate);

	self->priv->target = GL_TEXTURE_2D;
	self->priv->filter = GL_NEAREST;
	self->priv->wrap = GL_CLAMP_TO_EDGE;

	self->matrix = identity_texture_matrix;

	glGenTextures(1, &(self->priv->name));

	lw_texture_get_max_texture_units();
}

static void
lw_texture_finalize(GObject *object)
{
	LwTexture *self = LW_TEXTURE(object);

	if(self->priv->name)
		glDeleteTextures(1, &(self->priv->name));

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_texture_parent_class)->finalize(object);
}

static void
lw_texture_class_init(LwTextureClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_texture_set_property;
	gobject_class->get_property = lw_texture_get_property;
	gobject_class->finalize = lw_texture_finalize;

	g_type_class_add_private(klass, sizeof(LwTexturePrivate));

	/**
	 * LwTexture:target:
	 *
	 * The texture's target
	 */
	g_object_class_install_property(gobject_class, PROP_TARGET, g_param_spec_uint("target", "Target", "The texture's target", 0, G_MAXUINT, GL_TEXTURE_2D,  G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * LwTexture:width:
	 *
	 * The width of the texture
	 */
	g_object_class_install_property(gobject_class, PROP_WIDTH, g_param_spec_uint("width", "Width", "The width of the texture", 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * LwTexture:height:
	 *
	 * The height of the texture
	 */
	g_object_class_install_property(gobject_class, PROP_HEIGHT, g_param_spec_uint("height", "Height", "The height of the texture", 0, G_MAXUINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

