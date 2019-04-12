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

#ifndef _LW_CLOCK_H_
#define _LW_CLOCK_H_

G_BEGIN_DECLS

#define LW_TYPE_CLOCK            (lw_clock_get_type())
#define LW_CLOCK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_CLOCK, LwClock))
#define LW_IS_CLOCK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_CLOCK))
#define LW_CLOCK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_CLOCK, LwClockClass))
#define LW_IS_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_CLOCK))
#define LW_CLOCK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_CLOCK, LwClockClass))

typedef struct _LwClock LwClock;
typedef struct _LwClockClass LwClockClass;

typedef struct _LwClockPrivate LwClockPrivate;

struct _LwClock
{
	GObject parent_instance;

	/*< private >*/
	LwClockPrivate *priv;
};

struct _LwClockClass
{
	GObjectClass parent_class;
};

GType lw_clock_get_type(void);

LwClock *lw_clock_new();

guint lw_clock_get_fps(LwClock *self);
guint lw_clock_get_ms_to_sleep(LwClock *self);
guint lw_clock_get_ms_since_last_frame(LwClock *self);

void lw_clock_start_frame(LwClock *self);
void lw_clock_end_frame(LwClock *self);

/*
void lw_clock_start(char *action);
guint lw_clock_end(char *action, gboolean print);
*/

G_END_DECLS

#endif /* _LW_CLOCK_H_ */

