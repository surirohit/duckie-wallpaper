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

#ifndef _LW_BUFFER_H_
#define _LW_BUFFER_H_

G_BEGIN_DECLS

#define LW_TYPE_BUFFER            (lw_buffer_get_type())
#define LW_BUFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_BUFFER, LwBuffer))
#define LW_IS_BUFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_BUFFER))
#define LW_BUFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_BUFFER, LwBufferClass))
#define LW_IS_BUFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_BUFFER))
#define LW_BUFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_BUFFER, LwBufferClass))

typedef struct _LwBuffer LwBuffer;
typedef struct _LwBufferClass LwBufferClass;

typedef struct _LwBufferPrivate LwBufferPrivate;

struct _LwBuffer
{
	/*< private >*/
	GObject parent_instance;

	LwBufferPrivate *priv;
};

struct _LwBufferClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_buffer_get_type(void);

LwBuffer *lw_buffer_new(guint usage);

guint lw_buffer_get_name(LwBuffer *self);
guint lw_buffer_get_target(LwBuffer *self);
guint lw_buffer_get_usage(LwBuffer *self);

void lw_buffer_bind(LwBuffer *self);
void lw_buffer_unbind(LwBuffer *self);

/*
gboolean lw_buffer_is_normalized(LwBuffer *self);
void lw_buffer_set_normalized(LwBuffer *self, gboolean normalized);
*/

void lw_buffer_set_data(LwBuffer *self, guint size, gpointer data);
void lw_buffer_set_sub_data(LwBuffer *self, guint offset, guint size, gpointer data);

gpointer lw_buffer_get_data(LwBuffer *self, guint offset, guint size);
/*void lw_buffer_get_data(LwBuffer *self, guint offset, guint size, gpointer data);*/

G_END_DECLS

#endif /* _LW_BUFFER_H_ */

