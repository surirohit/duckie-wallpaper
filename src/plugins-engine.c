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
 * Copyright (C) 2013-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

#include "config.h"

#include <glib.h>
#include <gio/gio.h>
#include <libpeas/peas.h>

#include <livewallpaper/core.h>

#include "plugins-engine.h"


struct _LwPluginsEnginePrivate
{
	GSettings *settings;

	PeasPluginInfo *wallpaper_plugin;
};

enum
{
	PROP_0,

	PROP_WALLPAPER_PLUGIN,
	PROP_OTHER_PLUGINS,

	N_PROPERTIES
};

G_DEFINE_TYPE(LwPluginsEngine, lw_plugins_engine, PEAS_TYPE_ENGINE)


static inline void
lw_plugins_engine_unload_wallpaper_plugin(LwPluginsEngine *self)
{
	if(self->priv->wallpaper_plugin != NULL)
	{
		peas_engine_unload_plugin(PEAS_ENGINE(self),
		                          self->priv->wallpaper_plugin);
		self->priv->wallpaper_plugin = NULL;
	}
}

static void
lw_plugins_engine_set_property(GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
	LwPluginsEngine *self = LW_PLUGINS_ENGINE(object);

	switch(property_id)
	{
		case PROP_WALLPAPER_PLUGIN:
		{
			const gchar *name = g_value_get_string(value);
			PeasPluginInfo *info = peas_engine_get_plugin_info(PEAS_ENGINE(self),
			                                                   name);

			/* Found new plugin? */
			if(info == NULL)
			{
				g_warning("Could not find plugin %s", name);
				lw_plugins_engine_unload_wallpaper_plugin(self);
				return;
			}

			/* Is currently loaded? */
			if(peas_plugin_info_is_loaded(info)) return;

			/* Unload current wallpaper and load new wallpaper */
			lw_plugins_engine_unload_wallpaper_plugin(self);
			peas_engine_load_plugin(PEAS_ENGINE(self), info);

			/* Implements LwWallpaper? */
			if(!peas_engine_provides_extension(PEAS_ENGINE(self),
			                                   info, LW_TYPE_WALLPAPER))
			{
				g_warning("Could not load plugin: %s is not a wallpaper plugin", name);
				peas_engine_unload_plugin(PEAS_ENGINE(self), info);
				return;
			}

			self->priv->wallpaper_plugin = info;

			break;
		}

		case PROP_OTHER_PLUGINS:
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_plugins_engine_get_property(GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec)
{
	LwPluginsEngine *self = LW_PLUGINS_ENGINE(object);

	switch(property_id)
	{
		case PROP_WALLPAPER_PLUGIN:
			g_value_set_string(value, peas_plugin_info_get_name(self->priv->wallpaper_plugin));
			break;

		case PROP_OTHER_PLUGINS:
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

LwPluginsEngine*
lw_plugins_engine_new()
{
	return g_object_new(LW_TYPE_PLUGINS_ENGINE, NULL);
}

static void
lw_plugins_engine_init(LwPluginsEngine *self)
{
	gchar *local_plugin_dir = g_build_filename(g_get_home_dir(),  ".local", "share",
	                                           "livewallpaper", "plugins", NULL);
	GError *error = NULL;

	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_PLUGINS_ENGINE,
	                                         LwPluginsEnginePrivate);

	/* Enable additional plugin loaders */
	peas_engine_enable_loader(PEAS_ENGINE(self), "python");

	/* Load required repositories */
	if(!g_irepository_require(g_irepository_get_default(),
	                          "Peas", "1.0", 0, &error))
	{
		g_critical("Could not load Peas repository: %s", error->message);
        g_clear_error(&error);
	}

	if(!g_irepository_require_private(g_irepository_get_default(),
	                                  LIBDIR "livewallpaper/girepository-1.0/",
	                                  "LW", "1.0", 0, &error))
	{
		g_critical("Could not load LW repository: %s", error->message);
		g_clear_error(&error);
	}

	/* Add plugin directories to the search path */
	peas_engine_add_search_path(PEAS_ENGINE(self), local_plugin_dir, NULL);

	peas_engine_add_search_path(PEAS_ENGINE(self),
	                            LIBDIR "livewallpaper/plugins/",
	                            DATADIR "livewallpaper/plugins/");

	g_free(local_plugin_dir);
}

static void
lw_plugins_engine_class_init(LwPluginsEngineClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_plugins_engine_set_property;
	gobject_class->get_property = lw_plugins_engine_get_property;

	g_type_class_add_private(klass, sizeof(LwPluginsEnginePrivate));

	/* Install properties */
	g_object_class_install_property(gobject_class,
	                                PROP_WALLPAPER_PLUGIN,
	                                g_param_spec_string("wallpaper-plugin",
	                                                    "Wallpaper Plugin",
	                                                    "ID of the wallpaper plugin",
	                                                    NULL,
	                                                    G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class,
	                                PROP_OTHER_PLUGINS,
	                                g_param_spec_boxed("other-plugins",
	                                                   "Other Plugins",
	                                                   "Placeholder",
	                                                   G_TYPE_STRV,
	                                                   G_PARAM_READWRITE));
}
