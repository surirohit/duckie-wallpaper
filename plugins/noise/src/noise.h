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

#ifndef _NOISE_H_
#define _NOISE_H_

G_BEGIN_DECLS

#define NOISE_TYPE_PLUGIN            (noise_plugin_get_type())
#define NOISE_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NOISE_TYPE_PLUGIN, NoisePlugin))
#define NOISE_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NOISE_TYPE_PLUGIN))
#define NOISE_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NOISE_TYPE_PLUGIN, NoisePluginClass))
#define NOISE_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NOISE_TYPE_PLUGIN))
#define NOISE_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NOISE_TYPE_PLUGIN, NoisePluginClass))

typedef struct _NoisePlugin NoisePlugin;
typedef struct _NoisePluginClass NoisePluginClass;

typedef struct _NoisePluginPrivate NoisePluginPrivate;

struct _NoisePlugin {
	PeasExtensionBase parent_instance;

	/*< private >*/
	NoisePluginPrivate *priv;
};

struct _NoisePluginClass {
	PeasExtensionBaseClass parent_class;
};

GType noise_plugin_get_type(void);

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* _NOISE_H_ */

