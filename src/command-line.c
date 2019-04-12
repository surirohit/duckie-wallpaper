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

#include "config.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#include "command-line.h"


struct _LwCommandLinePrivate
{
	gboolean version;
};

G_DEFINE_TYPE(LwCommandLine, lw_command_line, G_TYPE_OBJECT)

static GOptionEntry options[] = {
	{
		"version", 'v', 0, G_OPTION_ARG_NONE, NULL,
		N_("Displays version number"), NULL
	},

	{ NULL }
};

LwCommandLine*
lw_command_line_parse(gint *argc, gchar ***argv)
{
	GError *error = NULL;
	LwCommandLine *self = g_object_new(LW_TYPE_COMMAND_LINE, NULL);
	GOptionContext *context = g_option_context_new(NULL);

	/* Set arg_data of all options */
	options[0].arg_data = &self->priv->version;

	g_option_context_add_main_entries(context, options, GETTEXT_PACKAGE);

	if(!g_option_context_parse(context, argc, argv, &error))
	{
		g_critical("Could not parse command line: %s\n", error->message);
		g_error_free(error);
	}

	g_option_context_free(context);
	return self;
}

gboolean
lw_command_line_get_version(LwCommandLine *self)
{
	return self->priv->version;
}

static void
lw_command_line_init(LwCommandLine *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_COMMAND_LINE,
	                                         LwCommandLinePrivate);

	self->priv->version = FALSE;
}

static void
lw_command_line_class_init(LwCommandLineClass *klass)
{
	g_type_class_add_private(klass, sizeof(LwCommandLinePrivate));
}
