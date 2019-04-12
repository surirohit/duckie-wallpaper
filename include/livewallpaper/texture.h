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

#ifndef _LW_TEXTURE_H_
#define _LW_TEXTURE_H_

G_BEGIN_DECLS

#define LW_TEX_COORD_X(m, px) ((m).xx * (px) + (m).x0)
#define LW_TEX_COORD_Y(m, py) ((m).yy * (py) + (m).y0)

#define LW_TEX_COORD_XY(m, px, py) ((m).xx * (px) + (m).xy * (py) + (m).x0)
#define LW_TEX_COORD_YX(m, px, py) ((m).yx * (px) + (m).yy * (py) + (m).y0)

typedef struct _LwTextureMatrix LwTextureMatrix;

struct _LwTextureMatrix
{
	gfloat xx, yx;
	gfloat xy, yy;
	gfloat x0, y0;
};

#define LW_TYPE_TEXTURE            (lw_texture_get_type())
#define LW_TEXTURE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_TEXTURE, LwTexture))
#define LW_IS_TEXTURE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_TEXTURE))
#define LW_TEXTURE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_TEXTURE, LwTextureClass))
#define LW_IS_TEXTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_TEXTURE))
#define LW_TEXTURE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_TEXTURE, LwTextureClass))

typedef struct _LwTexture LwTexture;
typedef struct _LwTextureClass LwTextureClass;

typedef struct _LwTexturePrivate LwTexturePrivate;

struct _LwTexture
{
	/*< private >*/
	GObject parent_instance;

	LwTexturePrivate *priv;

	/*< public >*/
	LwTextureMatrix matrix;
};

struct _LwTextureClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_texture_get_type(void);
LwTexture *lw_texture_new_from_resource (const gchar *path);
LwTexture *lw_texture_new_from_file(const gchar *path);
LwTexture *lw_texture_new_from_pixbuf(GdkPixbuf *pixbuf);
LwTexture *lw_texture_new_from_data(const guchar *data, guint width, guint height, guint format, guint type);

guint lw_texture_get_name(LwTexture *self);
guint lw_texture_get_target(LwTexture *self);

void lw_texture_set_filter(LwTexture *self, guint filter);
guint lw_texture_get_filter(LwTexture *self);

void lw_texture_set_wrap(LwTexture *self, guint wrap);
guint lw_texture_get_wrap(LwTexture *self);

guint lw_texture_get_width(LwTexture *self);
guint lw_texture_get_height(LwTexture *self);

void lw_texture_enable(LwTexture *self);
void lw_texture_disable(LwTexture *self);

void lw_texture_bind(LwTexture *self);
gboolean lw_texture_bind_to(LwTexture *self, guint unit);
void lw_texture_unbind(LwTexture *self);

gint lw_texture_get_max_texture_units();

G_END_DECLS

#endif /* _LW_TEXTURE_H_ */

