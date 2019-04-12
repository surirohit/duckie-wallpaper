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

#ifndef _LW_APPLICATION_H_
#define _LW_APPLICATION_H_

G_BEGIN_DECLS

#define LW_TYPE_APPLICATION            (lw_application_get_type())
#define LW_APPLICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_APPLICATION, LwApplication))
#define LW_IS_APPLICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_APPLICATION))
#define LW_APPLICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_APPLICATION, LwApplicationClass))
#define LW_IS_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_APPLICATION))
#define LW_APPLICATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_APPLICATION, LwApplicationClass))

typedef struct _LwApplication LwApplication;
typedef struct _LwApplicationClass LwApplicationClass;

typedef struct _LwApplicationPrivate LwApplicationPrivate;

struct _LwApplication
{
	GApplication parent_instance;

	/*< private >*/
	LwApplicationPrivate *priv;
};

struct _LwApplicationClass
{
	GApplicationClass parent_class;
};

GType lw_application_get_type(void);

G_END_DECLS

#endif /* _LW_APPLICATION_H_ */

