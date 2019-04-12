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

#ifndef _LW_COMMAND_LINE_H_
#define _LW_COMMAND_LINE_H_

G_BEGIN_DECLS

#define LW_TYPE_COMMAND_LINE            (lw_command_line_get_type())
#define LW_COMMAND_LINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_COMMAND_LINE, LwCommandLine))
#define LW_IS_COMMAND_LINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_COMMAND_LINE))
#define LW_COMMAND_LINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_COMMAND_LINE, LwCommandLineClass))
#define LW_IS_COMMAND_LINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_COMMAND_LINE))
#define LW_COMMAND_LINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_COMMAND_LINE, LwCommandLineClass))

typedef struct _LwCommandLine LwCommandLine;
typedef struct _LwCommandLineClass LwCommandLineClass;

typedef struct _LwCommandLinePrivate LwCommandLinePrivate;

struct _LwCommandLine
{
	GObject parent_instance;

	/*< private >*/
	LwCommandLinePrivate *priv;
};

struct _LwCommandLineClass
{
	GObjectClass parent_class;
};

GType lw_command_line_get_type(void);

LwCommandLine *lw_command_line_parse(gint *argc, gchar ***argv);

gboolean lw_command_line_get_version(LwCommandLine *self);

G_END_DECLS

#endif /* _LW_COMMAND_LINE_H_ */
