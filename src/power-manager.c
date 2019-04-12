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

#include <livewallpaper/core.h>

#include "power-manager.h"

struct _LwPowerManagerPrivate
{
	GSettings *settings;
	gboolean restart;

	UpClient *upclient;
	UpDevice *battery;
	gulong level_handler_id;

	guint pm_mode;
};

enum
{
	PROP_0,

	PROP_PM_MODE,

	N_PROPERTIES
};

G_DEFINE_TYPE(LwPowerManager, lw_power_manager, G_TYPE_OBJECT)


static gboolean
lw_power_manager_get_on_battery(LwPowerManager *self)
{
	return up_client_get_on_battery(self->priv->upclient);
}

static gboolean
lw_power_manager_get_on_low_battery(LwPowerManager *self)
{
#if UP_CHECK_VERSION(0, 99, 0)
	if(self->priv->battery != NULL && lw_power_manager_get_on_battery(self))
	{
		guint warning_level;
		g_object_get(self->priv->battery, "warning-level", &warning_level, NULL);

		if(warning_level == UP_DEVICE_LEVEL_LOW || warning_level == UP_DEVICE_LEVEL_CRITICAL)
			return TRUE;
	}

	return FALSE;
#else
	return up_client_get_on_low_battery(self->priv->upclient);
#endif
}

static void
lw_power_manager_changed(G_GNUC_UNUSED GObject *object, LwPowerManager *self)
{
	/* Stop LiveWallpaper if laptop is on (low) battery */
	if((self->priv->pm_mode == 2 && lw_power_manager_get_on_battery(self)) ||
	   (self->priv->pm_mode == 1 && lw_power_manager_get_on_low_battery(self)))
	{
		g_settings_set_boolean(self->priv->settings, "active", FALSE);
		self->priv->restart = TRUE;
	}

	/*
	 * Start LiveWallpaper if laptop is on power (= not on battery) and the power manager
	 * stopped it
	 */
	if(self->priv->restart && g_settings_get_boolean(self->priv->settings, "pm-restart") &&
	   !lw_power_manager_get_on_battery(self))
	{
		g_settings_set_boolean(self->priv->settings, "active", TRUE);
	}
}

static void
lw_power_manager_active_changed(G_GNUC_UNUSED GSettings *settings, G_GNUC_UNUSED gchar *key, LwPowerManager *self)
{
	/* If the user starts LiveWallpaper, power manager should not be able to restart it */
	self->priv->restart = FALSE;
}

static void
lw_power_manager_set_property(GObject *object,
                              guint property_id,
                              const GValue *value,
                              GParamSpec *pspec)
{
	LwPowerManager *self = LW_POWER_MANAGER(object);

	switch(property_id)
	{
		case PROP_PM_MODE:
			self->priv->pm_mode = g_value_get_uint(value);
			lw_power_manager_changed(NULL, self);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_power_manager_get_property(GObject *object,
                              guint property_id,
                              GValue *value,
                              GParamSpec *pspec)
{
	LwPowerManager *self = LW_POWER_MANAGER(object);

	switch(property_id)
	{
		case PROP_PM_MODE:
			g_value_set_uint(value, self->priv->pm_mode);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static UpDevice*
lw_power_manager_find_battery(LwPowerManager *self)
{
	GPtrArray *devices = NULL;
	guint i;

#if !UP_CHECK_VERSION(0, 99, 0)
	if(!up_client_enumerate_devices_sync(self->priv->upclient, NULL, NULL))
	{
		return NULL;
	}
#endif

	devices = up_client_get_devices(self->priv->upclient);
	for(i = 0; i < devices->len; i++)
	{
		guint kind;
		UpDevice *device = g_ptr_array_index(devices, i);

		g_object_get(device, "kind", &kind, NULL);
		if(kind == UP_DEVICE_KIND_BATTERY)
		{
			g_ptr_array_unref(devices);
			return g_object_ref(device);
		}
	}

	g_ptr_array_unref(devices);
	return NULL;
}

#if UP_CHECK_VERSION(0, 99, 0)
static void
lw_power_manager_property_changed(G_GNUC_UNUSED GObject *object, G_GNUC_UNUSED GParamSpec *pspec, LwPowerManager *self)
{
	lw_power_manager_changed(NULL, self);
}
#endif

static void
lw_power_manager_devices_changed(G_GNUC_UNUSED UpClient *client, G_GNUC_UNUSED UpDevice *device, LwPowerManager *self)
{
#if UP_CHECK_VERSION(0, 99, 0)
	if(self->priv->level_handler_id != 0)
	{
		g_signal_handler_disconnect(self->priv->battery, self->priv->level_handler_id);
		self->priv->level_handler_id = 0;
	}
#endif

	if(self->priv->battery != NULL) g_object_unref(self->priv->battery);
	self->priv->battery = lw_power_manager_find_battery(self);

#if UP_CHECK_VERSION(0, 99, 0)
	if(self->priv->battery != NULL)
		self->priv->level_handler_id = g_signal_connect(self->priv->battery, "notify::warning-level", G_CALLBACK(lw_power_manager_property_changed), self);
#endif

	lw_power_manager_changed(NULL, self);
}

static void
lw_power_manager_init(LwPowerManager *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_POWER_MANAGER,
	                                         LwPowerManagerPrivate);

	self->priv->settings = g_settings_new(LW_SETTINGS);
	self->priv->restart = FALSE;
	self->priv->upclient = up_client_new();

	self->priv->battery = NULL;
	self->priv->level_handler_id = 0;

	lw_settings_bind_enum(self->priv->settings, "pm-mode", self, "pm-mode", G_SETTINGS_BIND_GET);
	g_signal_connect(self->priv->settings, "changed::active",
	                 G_CALLBACK(lw_power_manager_active_changed), self);
	lw_power_manager_changed(NULL, self);

#if UP_CHECK_VERSION(0, 99, 0)
	g_signal_connect(self->priv->upclient, "notify::on-battery", G_CALLBACK(lw_power_manager_property_changed), self);
#else
	g_signal_connect(self->priv->upclient, "changed", G_CALLBACK(lw_power_manager_changed), self);
#endif

	/*
	 * Own a reference of the battery to figure out, if we are on low battery.
	 * Maybe the battery gets removed and so we have to connect to these signals.
	 */
	g_signal_connect(self->priv->upclient, "device-added", G_CALLBACK(lw_power_manager_devices_changed), self);
	g_signal_connect(self->priv->upclient, "device-removed", G_CALLBACK(lw_power_manager_devices_changed), self);
	lw_power_manager_devices_changed(NULL, NULL, self);
}

static void
lw_power_manager_dispose(GObject *object)
{
	LwPowerManager *self = LW_POWER_MANAGER(object);

	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->upclient);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_power_manager_parent_class)->dispose(object);
}

static void
lw_power_manager_class_init(LwPowerManagerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_power_manager_set_property;
	gobject_class->get_property = lw_power_manager_get_property;
	gobject_class->dispose = lw_power_manager_dispose;

	g_type_class_add_private(klass, sizeof(LwPowerManagerPrivate));

	/* Install properties */
	g_object_class_install_property(gobject_class,
	                                PROP_PM_MODE,
	                                g_param_spec_uint("pm-mode",
	                                                  "Autostop on battery",
	                                                  "Selects whether LiveWallpaper should automatically stop when the laptop runs on battery or on low battery",
	                                                  0, 2, 2,
	                                                  G_PARAM_READWRITE));
}
