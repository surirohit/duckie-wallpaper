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

#ifndef _LW_CAIRO_TEXTURE_H_
#define _LW_CAIRO_TEXTURE_H_

G_BEGIN_DECLS

#define LW_TYPE_CAIRO_TEXTURE            (lw_cairo_texture_get_type())
#define LW_CAIRO_TEXTURE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_TEXTURE, LwCairoTexture))
#define LW_IS_CAIRO_TEXTURE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_TEXTURE))
#define LW_CAIRO_TEXTURE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_TEXTURE, LwCairoTextureClass))
#define LW_IS_CAIRO_TEXTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_TEXTURE))
#define LW_CAIRO_TEXTURE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_TEXTURE, LwCairoTextureClass))

typedef struct _LwCairoTexture LwCairoTexture;
typedef struct _LwCairoTextureClass LwCairoTextureClass;

typedef struct _LwCairoTexturePrivate LwCairoTexturePrivate;

struct _LwCairoTexture
{
	/*< private >*/
	LwTexture parent_instance;

	LwCairoTexturePrivate *priv;
};

struct _LwCairoTextureClass
{
	/*< private >*/
	LwTextureClass parent_class;
};

GType lw_cairo_texture_get_type(void);

LwCairoTexture *lw_cairo_texture_new(guint width, guint height);

cairo_t *lw_cairo_texture_cairo_create(LwCairoTexture *self);

void lw_cairo_texture_update(LwCairoTexture *self);

G_END_DECLS

#endif /* _LW_CAIRO_TEXTURE_H_ */

