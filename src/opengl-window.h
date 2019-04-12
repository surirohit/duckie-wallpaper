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

#ifndef _LW_OPENGL_WINDOW_H_
#define _LW_OPENGL_WINDOW_H_

G_BEGIN_DECLS

#define LW_TYPE_OPENGL_WINDOW            (lw_opengl_window_get_type())
#define LW_OPENGL_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_OPENGL_WINDOW, LwOpenGLWindow))
#define LW_IS_OPENGL_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_OPENGL_WINDOW))
#define LW_OPENGL_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_OPENGL_WINDOW, LwOpenGLWindowClass))
#define LW_IS_OPENGL_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_OPENGL_WINDOW))
#define LW_OPENGL_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_OPENGL_WINDOW, LwOpenGLWindowClass))

typedef struct _LwOpenGLWindow LwOpenGLWindow;
typedef struct _LwOpenGLWindowClass LwOpenGLWindowClass;

typedef struct _LwOpenGLWindowPrivate LwOpenGLWindowPrivate;

struct _LwOpenGLWindow
{
	GObject parent_instance;

	/*< private >*/
	LwOpenGLWindowPrivate *priv;
};

struct _LwOpenGLWindowClass
{
	GObjectClass parent_class;
};

GType lw_opengl_window_get_type(void);

G_END_DECLS

#endif /* _LW_OPENGL_WINDOW_H_ */

