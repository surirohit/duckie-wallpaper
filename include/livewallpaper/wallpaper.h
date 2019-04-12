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

#ifndef _LW_WALLPAPER_H_
#define _LW_WALLPAPER_H_

G_BEGIN_DECLS

#define LW_TYPE_WALLPAPER                (lw_wallpaper_get_type())
#define LW_WALLPAPER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_WALLPAPER, LwWallpaper))
#define LW_IS_WALLPAPER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_WALLPAPER))
#define LW_WALLPAPER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LW_TYPE_WALLPAPER, LwWallpaperInterface))

typedef struct _LwWallpaper LwWallpaper;
typedef struct _LwWallpaperInterface LwWallpaperInterface;

struct _LwWallpaperInterface
{
	/*< private >*/
	GTypeInterface parent_iface;

	/*< public >*/
	void (*init_plugin) (LwWallpaper *self);

	void (*adjust_viewport) (LwWallpaper *self, LwOutput *output);

	void (*prepare_paint) (LwWallpaper *self, gint ms_since_last_paint);
	void (*paint) (LwWallpaper *self, LwOutput *output);
	void (*done_paint) (LwWallpaper *self);

	void (*restore_viewport) (LwWallpaper *self);
};

GType lw_wallpaper_get_type(void);

void lw_wallpaper_init_plugin(LwWallpaper *self);

void lw_wallpaper_adjust_viewport(LwWallpaper *self, LwOutput *output);

void lw_wallpaper_prepare_paint(LwWallpaper *self, gint ms_since_last_paint);
void lw_wallpaper_paint(LwWallpaper *self, LwOutput *output);
void lw_wallpaper_done_paint(LwWallpaper *self);

void lw_wallpaper_restore_viewport(LwWallpaper *self);

GResource *lw_wallpaper_load_gresource (LwWallpaper *self, const gchar *filename);

G_END_DECLS

#endif /* _LW_WALLPAPER_H_ */

