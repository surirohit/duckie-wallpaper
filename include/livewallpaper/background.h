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

#ifndef _LW_BACKGROUND_H_
#define _LW_BACKGROUND_H_

G_BEGIN_DECLS

typedef enum
{
	LwBackgroundZoom,
	LwBackgroundCentered,
	LwBackgroundScaled,
	LwBackgroundStretched,
	LwBackgroundTiled
} LwBackgroundRenderType;

typedef enum
{
	LwBackgroundHorizontal,
	LwBackgroundVertical,
	LwBackgroundSolid
} LwBackgroundShadingType;

#define LW_TYPE_BACKGROUND            (lw_background_get_type())
#define LW_BACKGROUND(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_BACKGROUND, LwBackground))
#define LW_IS_BACKGROUND(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_BACKGROUND))
#define LW_BACKGROUND_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_BACKGROUND, LwBackgroundClass))
#define LW_IS_BACKGROUND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_BACKGROUND))
#define LW_BACKGROUND_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_BACKGROUND, LwBackgroundClass))

typedef struct _LwBackground LwBackground;
typedef struct _LwBackgroundClass LwBackgroundClass;

typedef struct _LwBackgroundPrivate LwBackgroundPrivate;

struct _LwBackground
{
	/*< private >*/
	GObject parent_instance;

	LwBackgroundPrivate *priv;
};

struct _LwBackgroundClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_background_get_type(void);

LwBackground *lw_background_new_from_file(const gchar *path, LwBackgroundRenderType type);
LwBackground *lw_background_new_from_resource (const gchar *path, LwBackgroundRenderType type);
LwBackground *lw_background_new_from_colors(GdkColor *primary_color, GdkColor *secondary_color, LwBackgroundShadingType type);
LwBackground *lw_background_new_from_texture(LwTexture *texture, LwBackgroundRenderType type);

void lw_background_draw(LwBackground *self, LwOutput *output);

G_END_DECLS

#endif /* _LW_BACKGROUND_H_ */

