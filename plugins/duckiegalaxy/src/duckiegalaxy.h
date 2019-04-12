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
 * Copyright (C) 2012-2016 Aurélien   Rivière <aurelien.riv@gmail.com>
 *
 */

#ifndef _DUCKIEGALAXY_H_
#define _DUCKIEGALAXY_H_

G_BEGIN_DECLS

#define DUCKIEGALAXY_TYPE_PLUGIN            (duckiegalaxy_plugin_get_type())
#define DUCKIEGALAXY_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUCKIEGALAXY_TYPE_PLUGIN, DuckieGalaxyPlugin))
#define DUCKIEGALAXY_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUCKIEGALAXY_TYPE_PLUGIN))
#define DUCKIEGALAXY_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUCKIEGALAXY_TYPE_PLUGIN, DuckieGalaxyPluginClass))
#define DUCKIEGALAXY_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUCKIEGALAXY_TYPE_PLUGIN))
#define DUCKIEGALAXY_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUCKIEGALAXY_TYPE_PLUGIN, DuckieGalaxyPluginClass))

typedef struct _DuckieGalaxyPlugin DuckieGalaxyPlugin;
typedef struct _DuckieGalaxyPluginClass DuckieGalaxyPluginClass;

typedef struct _DuckieGalaxyPluginPrivate DuckieGalaxyPluginPrivate;

struct _DuckieGalaxyPlugin {
	PeasExtensionBase parent_instance;

	/*< private >*/
	DuckieGalaxyPluginPrivate *priv;
};

struct _DuckieGalaxyPluginClass {
	PeasExtensionBaseClass parent_class;
};

GType duckiegalaxy_plugin_get_type(void);

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* _DUCKIEGALAXY_H_ */
