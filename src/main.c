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
#include <gdk/gdk.h>
#include <locale.h>
#include <glib/gi18n.h>
#include "application.h"

gint
main(gint argc, gchar **argv)
{
	LwApplication *app;
	gint retval;

    gchar *tmp = g_build_filename(g_get_home_dir(), ".local", "share", "glib-2.0", "schemas", NULL);
	g_setenv("GSETTINGS_SCHEMA_DIR", tmp, TRUE);
	g_free(tmp);
    setlocale(LC_ALL, "");
    textdomain(GETTEXT_PACKAGE);
    bindtextdomain(GETTEXT_PACKAGE, DATADIR"locale");

	/* Initialize external libraries */
	gdk_init(&argc, &argv);

	/* Run livewallpaper application */
	app = g_object_new(LW_TYPE_APPLICATION,
	                   "application-id", LW_BUS,
	                   "flags", G_APPLICATION_IS_SERVICE,
	                   NULL);

	retval = g_application_run(G_APPLICATION(app), argc, argv);

	/* Exit application */
	g_object_unref(app);

	return retval;
}
