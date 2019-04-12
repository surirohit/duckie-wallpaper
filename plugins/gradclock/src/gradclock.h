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
 * Copyright (C) 2013-2016 Koichi Akabe <vbkaisetsu@gmail.com>
 *
 */

#ifndef _GRADCLOCK_H_
#define _GRADCLOCK_H_

G_BEGIN_DECLS

#define GRADCLOCK_TYPE_PLUGIN            (gradclock_plugin_get_type())
#define GRADCLOCK_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRADCLOCK_TYPE_PLUGIN, GradClockPlugin))
#define GRADCLOCK_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRADCLOCK_TYPE_PLUGIN))
#define GRADCLOCK_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GRADCLOCK_TYPE_PLUGIN, GradClockPluginClass))
#define GRADCLOCK_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRADCLOCK_TYPE_PLUGIN))
#define GRADCLOCK_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GRADCLOCK_TYPE_PLUGIN, GradClockPluginClass))

typedef struct _GradClockPlugin GradClockPlugin;
typedef struct _GradClockPluginClass GradClockPluginClass;

typedef struct _GradClockPluginPrivate GradClockPluginPrivate;

struct _GradClockPlugin {
	PeasExtensionBase parent_instance;

	/*< private >*/
	GradClockPluginPrivate *priv;
};

struct _GradClockPluginClass {
	PeasExtensionBaseClass parent_class;
};

GType gradclock_plugin_get_type(void);

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* _GRADCLOCK_H_ */

