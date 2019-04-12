/*
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
 * Copyright (C) 2014-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */
#include <glib.h>
#include <gio/gio.h>
#include <string.h>

static inline gboolean
needle_in_string(gchar *string, gchar *needle)
{
    gchar *lc_string = g_utf8_strdown(string, -1);
    gboolean result = strstr(lc_string, needle) != NULL;
    g_free(lc_string);

    return result;
}

gboolean
lwc_search_needle_in_ssk(void* self, gchar *schema_id, gchar *key, gchar *needle)
{
    GSettingsSchema* schema = g_settings_schema_source_lookup(g_settings_schema_source_get_default(),
                                                              schema_id, TRUE);
	GSettingsSchemaKey* ssk = g_settings_schema_get_key(schema, key);

	gboolean result = FALSE;
    result |= needle_in_string(g_settings_schema_key_get_summary(ssk), needle);
    result |= needle_in_string(g_settings_schema_key_get_description(ssk), needle);

	GVariant *range = g_settings_schema_key_get_range(ssk);
	result |= needle_in_string(g_variant_print(range, FALSE), needle);
	g_variant_unref(range);

	return result;
}

