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
 * SECTION: cairo-texture
 * @Short_description: connection between OpenGL textures and the cairo graphics library
 *
 * The #LwCairoTexture object allows you to draw directly onto your OpenGL texture
 * by using the cairo graphics library.
 *
 * <example>
 *   <title>Using a LwCairoTexture</title>
 *   <programlisting>
 * GError *error = NULL;
 * LwCairoTexture *tex = lw_cairo_texture_new(WIDTH, HEIGHT, error);
 * cairo_t *cr;
 *
 * if(error != NULL)
 * {
 *     g_warning("Could not create cairo texture: %s", error->message);
 *     g_error_free(error);
 *     return;
 * }
 *
 * cr = lw_cairo_texture_cairo_create(tex);
 * // use cairo to draw onto your texture
 * ...
 * cairo_destroy(cr);
 *
 * // update the texture everytime you draw on it
 * lw_cairo_texture_update(tex);
 *
 * lw_texture_enable( LW_TEXTURE(tex) );
 * ...</programlisting>
 * </example>
 */

#include <livewallpaper/core.h>


struct _LwCairoTexturePrivate
{
	cairo_surface_t *surf;
	guchar *surf_data;
};

/**
 * LwCairoTexture:
 *
 * Paint on this OpenGL texture with cairo.
 */

G_DEFINE_TYPE(LwCairoTexture, lw_cairo_texture, LW_TYPE_TEXTURE)

/**
 * lw_cairo_texture_new:
 * @width: The width of the new texture
 * @height: The height of the new texture
 *
 * Creates a new cairo texture with a specified size.
 *
 * Returns: A new #LwCairoTexture or %NULL in case of an error. 
 *          Use g_object_unref() to free the #LwCairoTexture.
 */
LwCairoTexture*
lw_cairo_texture_new(guint width, guint height)
{
	cairo_surface_t *surf;
	cairo_status_t surf_status;
	guchar *surf_data = g_malloc0(4 * width * height);
	surf = cairo_image_surface_create_for_data(surf_data,
	                                           CAIRO_FORMAT_ARGB32,
	                                           width, height,
	                                           4 * width);
	surf_status = cairo_surface_status(surf);

	if(surf_status == CAIRO_STATUS_SUCCESS)
	{
		LwCairoTexture *cairo_tex = g_object_new(LW_TYPE_CAIRO_TEXTURE,
		                                         "width", width,
		                                         "height", height,
		                                         NULL);
		LwTexture *tex = LW_TEXTURE(cairo_tex);

		cairo_tex->priv->surf = surf;
		cairo_tex->priv->surf_data = surf_data;

		lw_cairo_texture_update(cairo_tex);

		lw_texture_set_filter(tex, GL_NEAREST);
		lw_texture_set_wrap(tex, GL_CLAMP_TO_EDGE);

		return cairo_tex;
	}

    g_warning("Could not create Cairo texture: %s", cairo_status_to_string(surf_status));
	cairo_surface_destroy(surf);
	g_free(surf_data);

	return NULL;
}

/**
 * lw_cairo_texture_cairo_create:
 * @self: A #LwCairoTexture
 *
 * Creates a cairo context for drawing to the texture. Call lw_cairo_texture_update()
 * to update the texture when you are done with drawing.
 *
 * Returns: A newly created cairo context. Free it with cairo_destroy() when you are done with drawing.
 */
cairo_t*
lw_cairo_texture_cairo_create(LwCairoTexture *self)
{
	return cairo_create(self->priv->surf);
}

/**
 * lw_cairo_texture_update:
 * @self: A #LwCairoTexture
 *
 * Updates the texture.
 */
void
lw_cairo_texture_update(LwCairoTexture *self)
{
	LwTexture *tex = LW_TEXTURE(self);

	glBindTexture(lw_texture_get_target(tex),
	              lw_texture_get_name(tex));

	glTexImage2D(lw_texture_get_target(tex),
	             0,
	             GL_RGBA,
	             lw_texture_get_width(tex),
	             lw_texture_get_height(tex),
	             0,
	             GL_BGRA,
	             GL_UNSIGNED_BYTE,
	             self->priv->surf_data);
}

static void
lw_cairo_texture_init(LwCairoTexture *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_CAIRO_TEXTURE,
	                                         LwCairoTexturePrivate);

	self->priv->surf = NULL;
	self->priv->surf_data = NULL;
}

static void
lw_cairo_texture_finalize(GObject *object)
{
	LwCairoTexture *self = LW_CAIRO_TEXTURE(object);

	if(self->priv->surf)
	{
		cairo_surface_destroy(self->priv->surf);
		self->priv->surf = NULL;
	}

	if(self->priv->surf_data)
	{
		g_free(self->priv->surf_data);
		self->priv->surf_data = NULL;
	}

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_cairo_texture_parent_class)->finalize(object);
}

static void
lw_cairo_texture_class_init(LwCairoTextureClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = lw_cairo_texture_finalize;

	g_type_class_add_private(klass, sizeof(LwCairoTexturePrivate));
}

