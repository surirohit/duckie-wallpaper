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

#ifndef _GALAXY_H_
#define _GALAXY_H_

G_BEGIN_DECLS

#define GALAXY_TYPE_PLUGIN            (galaxy_plugin_get_type())
#define GALAXY_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GALAXY_TYPE_PLUGIN, GalaxyPlugin))
#define GALAXY_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GALAXY_TYPE_PLUGIN))
#define GALAXY_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GALAXY_TYPE_PLUGIN, GalaxyPluginClass))
#define GALAXY_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GALAXY_TYPE_PLUGIN))
#define GALAXY_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GALAXY_TYPE_PLUGIN, GalaxyPluginClass))

typedef struct _GalaxyPlugin GalaxyPlugin;
typedef struct _GalaxyPluginClass GalaxyPluginClass;

typedef struct _GalaxyPluginPrivate GalaxyPluginPrivate;

struct _GalaxyPlugin {
	PeasExtensionBase parent_instance;

	/*< private >*/
	GalaxyPluginPrivate *priv;
};

struct _GalaxyPluginClass {
	PeasExtensionBaseClass parent_class;
};

GType galaxy_plugin_get_type(void);

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* _GALAXY_H_ */

