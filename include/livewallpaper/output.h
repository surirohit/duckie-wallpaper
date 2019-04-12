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

#ifndef _LW_OUTPUT_H_
#define _LW_OUTPUT_H_

G_BEGIN_DECLS

#define LW_TYPE_OUTPUT            (lw_output_get_type())
#define LW_OUTPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_OUTPUT, LwOutput))
#define LW_IS_OUTPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_OUTPUT))
#define LW_OUTPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_OUTPUT, LwOutputClass))
#define LW_IS_OUTPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_OUTPUT))
#define LW_OUTPUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_OUTPUT, LwOutputClass))

typedef struct _LwOutput LwOutput;
typedef struct _LwOutputClass LwOutputClass;

typedef struct _LwOutputPrivate LwOutputPrivate;

struct _LwOutput
{
	/*< private >*/
	GObject parent_instance;

	LwOutputPrivate *priv;
};

struct _LwOutputClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_output_get_type(void);

guint lw_output_get_id(LwOutput *self);

guint lw_output_get_x(LwOutput *self);
guint lw_output_get_y(LwOutput *self);
guint lw_output_get_width(LwOutput *self);
guint lw_output_get_height(LwOutput *self);

gdouble lw_output_get_aspect_ratio(LwOutput *self);
guint lw_output_get_longest_side(LwOutput *self);
guint lw_output_get_shortest_side(LwOutput *self);

void lw_output_make_current(LwOutput *self);

G_END_DECLS

#endif /* _LW_OUTPUT_H_ */

