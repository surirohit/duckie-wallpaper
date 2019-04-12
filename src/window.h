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

#ifndef _LW_WINDOW_H_
#define _LW_WINDOW_H_

G_BEGIN_DECLS

#define LW_TYPE_WINDOW                (lw_window_get_type())
#define LW_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_WINDOW, LwWindow))
#define LW_IS_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_WINDOW))
#define LW_WINDOW_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LW_TYPE_WINDOW, LwWindowInterface))

typedef struct _LwWindow LwWindow;
typedef struct _LwWindowInterface LwWindowInterface;

struct _LwWindowInterface
{
	/*< private >*/
	GTypeInterface parent_iface;

	/*< public >*/
	GError* (*get_error) (LwWindow *self);
	gboolean (*is_current) (LwWindow *self);
	void (*make_current) (LwWindow *self);
	void (*swap_buffers) (LwWindow *self);
	void (*show) (LwWindow *self, gboolean behind_icons);
	void (*hide) (LwWindow *self);
	GList* (*get_outputs) (LwWindow *self);
};

GType lw_window_get_type(void);

GError *lw_window_get_error(LwWindow *self);
gboolean lw_window_is_current(LwWindow *self);
void lw_window_make_current(LwWindow *self);
void lw_window_swap_buffers(LwWindow *self);

void lw_window_show(LwWindow *self, gboolean behind_icons);
void lw_window_hide(LwWindow *self);

GList *lw_window_get_outputs(LwWindow *self);

/*
lw_window_raise(LwWindow *self)
lw_window_lower(LwWindow *self)
*/

G_END_DECLS

#endif /* _LW_WINDOW_H_ */

