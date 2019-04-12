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

#ifndef _LW_UTIL_H_
#define _LW_UTIL_H_

#define LW_BIND(obj, prop)                g_settings_bind (self->priv->settings, prop, obj, prop, G_SETTINGS_BIND_GET)
#define LW_BIND_COLOR(obj, prop)   lw_settings_bind_color (self->priv->settings, prop, obj, prop, G_SETTINGS_BIND_GET)
#define LW_BIND_ENUM(obj, prop)    lw_settings_bind_enum  (self->priv->settings, prop, obj, prop, G_SETTINGS_BIND_GET)
#define LW_BIND_RANGE(obj, prop)   lw_settings_bind_range (self->priv->settings, prop, obj, prop, G_SETTINGS_BIND_GET)

#define LW_OPENGL_1_4_HELPER(fun20, fun14, params) \
	(GLEW_VERSION_2_0) ? fun20 params : fun14 params
	
void lw_settings_bind_color(GSettings *settings, const gchar *key,
                            gpointer object, const gchar *property,
                            GSettingsBindFlags flags);

void lw_settings_bind_range(GSettings *settings, const gchar *key,
                            gpointer object, const gchar *property,
                            GSettingsBindFlags flags);

void lw_settings_bind_enum(GSettings *settings, const gchar *key,
                           gpointer object, const gchar *property,
                           GSettingsBindFlags flags);

GResource *lw_load_gresource (const gchar *path);
void lw_unload_gresource (GResource *resource);

#endif /* _LW_UTIL_H_ */

