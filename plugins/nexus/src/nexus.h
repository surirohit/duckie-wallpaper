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

#ifndef _NEXUS_H_
#define _NEXUS_H_

G_BEGIN_DECLS

#define NEXUS_TYPE_PLUGIN            (nexus_plugin_get_type())
#define NEXUS_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NEXUS_TYPE_PLUGIN, NexusPlugin))
#define NEXUS_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NEXUS_TYPE_PLUGIN))
#define NEXUS_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NEXUS_TYPE_PLUGIN, NexusPluginClass))
#define NEXUS_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NEXUS_TYPE_PLUGIN))
#define NEXUS_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NEXUS_TYPE_PLUGIN, NexusPluginClass))

typedef struct _NexusPlugin NexusPlugin;
typedef struct _NexusPluginClass NexusPluginClass;

typedef struct _NexusPluginPrivate NexusPluginPrivate;

struct _NexusPlugin {
	PeasExtensionBase parent_instance;

	/*< private >*/
	NexusPluginPrivate *priv;
};

struct _NexusPluginClass {
	PeasExtensionBaseClass parent_class;
};

GType nexus_plugin_get_type(void);

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* _NEXUS_H_ */

