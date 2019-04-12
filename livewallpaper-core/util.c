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

/**
 * SECTION: util
 * @Short_description: functions to simplify some common tasks
 * @Title: Miscellaneous Utilities
 *
 * This section provides several functions and macros that reduce the code
 * size or simplify some common tasks.
 */

#include <livewallpaper/core.h>

/**
 * LW_BIND:
 * @obj: A #GObject with a property named @prop.
 * @prop: The property and key to bind to. Property name and key name will both be set to @prop.
 *
 * This macro is used to make a very common call to g_settings_bind shorter. It binds a key
 * called @prop from the settings to a property called @prop of the #GObject @obj. For color,
 * enum or range properties see LW_BIND_COLOR(), LW_BIND_ENUM() or LW_BIND_RANGE().
 *
 * <note>
 *   <para>
 *     This macro assumes that self->priv->settings exists in the current context and is a #GSettings object,
 *     and that the property and key names are the same.
 *   </para>
 * </note>
 *
 * Since: 0.5
 */
 
/**
 * LW_BIND_COLOR:
 * @obj: A #GObject with a property named @prop.
 * @prop: The property and key to bind to. Property name and key name will both be set to @prop.
 *
 * The same as LW_BIND(), but it binds a color property to a settings key.
 *
 * <note>
 *   <para>
 *     This macro assumes that self->priv->settings exists in the current context and is a #GSettings object,
 *     and that the property and key names are the same.
 *   </para>
 * </note>
 *
 * Since: 0.5
 */
 
/**
 * LW_BIND_ENUM:
 * @obj: A #GObject with a property named @prop.
 * @prop: The property and key to bind to. Property name and key name will both be set to @prop.
 *
 * The same as LW_BIND(), but it binds an enum property to a settings key.
 * 
 * <note>
 *   <para>
 *     This macro assumes that self->priv->settings exists in the current context and is a #GSettings object,
 *     and that the property and key names are the same.
 *   </para>
 * </note>
 *
 * Since: 0.5
 */
 
/**
 * LW_BIND_RANGE:
 * @obj: A #GObject with a property named @prop.
 * @prop: The property and key to bind to. Property name and key name will both be set to @prop.
 *
 * The same as LW_BIND(), but it binds a range property to a settings key.
 * 
 * <note>
 *   <para>
 *     This macro assumes that self->priv->settings exists in the current context and is a #GSettings object,
 *     and that the property and key names are the same.
 *   </para>
 * </note>
 *
 * Since: 0.5
 */

/**
 * LW_OPENGL_1_4_HELPER
 * @fun20: Name of the function for OpenGL 2.0+
 * @fun14: Name of the function before OpenGL 2.0
 * @params: The function's parameters
 *
 * This macro calls @fun20 with @params as parameters if OpenGL 2.0 or later is available on the system. For
 * systems with OpenGL before version 2.0 it calls @fun14 with @params as parameters.
 *
 * Since: 0.5
 */

/**
 * lw_settings_bind_color:
 * @settings: A #GSettings object
 * @key: The key to bind
 * @object: (type GObject.Object): A #GObject
 * @property: The name of the property to bind
 * @flags: Flags for the binding
 *
 * This function calls g_settings_bind_with_mapping() with the required
 * mapping functions for colors. It uses lw_color_get_color_mapping()
 * and lw_color_set_color_mapping() as mapping functions.
 *
 * Since: 0.4
 */
void
lw_settings_bind_color(GSettings *settings,
                       const gchar *key,
                       gpointer object,
                       const gchar *property,
                       GSettingsBindFlags flags)
{
	g_settings_bind_with_mapping(settings, key, object, property, flags,
		lw_color_get_color_mapping, lw_color_set_color_mapping, NULL, NULL);
}

/**
 * lw_settings_bind_range:
 * @settings: A #GSettings object
 * @key: The key to bind
 * @object: (type GObject.Object): A #GObject
 * @property: The name of the property to bind
 * @flags: Flags for the binding
 *
 * This function calls g_settings_bind_with_mapping() with the required
 * mapping functions for #LwRange properties. It uses lw_range_get_range_mapping()
 * and lw_range_set_range_mapping() as mapping functions.
 *
 * Since: 0.4
 */
void
lw_settings_bind_range(GSettings *settings,
                       const gchar *key,
                       gpointer object,
                       const gchar *property,
                       GSettingsBindFlags flags)
{
	g_settings_bind_with_mapping(settings, key, object, property, flags,
		lw_range_get_range_mapping, lw_range_set_range_mapping, NULL, NULL);
}

typedef struct
{
	GSettings *settings;
	gchar *key;
} LwEnumBinding;

static gboolean
lw_enum_get_enum_mapping(GValue *value, G_GNUC_UNUSED GVariant *variant, LwEnumBinding *binding)
{
	g_value_set_uint(value, g_settings_get_enum(binding->settings, binding->key));
	return TRUE;
}

static GVariant*
lw_enum_set_enum_mapping(const GValue *value, G_GNUC_UNUSED const GVariantType *type, LwEnumBinding *binding)
{
	/*
	 * We set the enum here, even if it is not the purpose of this function.
	 * The disadvantage of this is, that it also calls lw_enum_get_enum_mapping.
	 */
    GVariant *variant;
    gchar *str;
	
    g_settings_set_enum(binding->settings, binding->key, g_value_get_uint(value));
    
    str = g_settings_get_string(binding->settings, binding->key);
	variant = g_variant_new_string(str);
    g_free (str);
    
    return variant;
}

static void
lw_enum_binding_free(LwEnumBinding *binding)
{
	g_free(binding->key);
	g_slice_free(LwEnumBinding, binding);
}

/**
 * lw_settings_bind_enum:
 * @settings: A #GSettings object
 * @key: The key to bind
 * @object: (type GObject.Object): A #GObject
 * @property: The name of the property to bind
 * @flags: Flags for the binding
 *
 * Creates a binding between an enum @key and a #guint @property. This function
 * passes all params to g_settings_bind_with_mapping() and sets the required
 * mapping functions.
 *
 * Since: 0.4
 */
void
lw_settings_bind_enum(GSettings *settings,
                      const gchar *key,
                      gpointer object,
                      const gchar *property,
                      GSettingsBindFlags flags)
{
	LwEnumBinding *binding = g_slice_new(LwEnumBinding);

	binding->settings = settings;
	binding->key = g_strdup(key);

	g_settings_bind_with_mapping(settings, key, object, property, flags,
	                             (GSettingsBindGetMapping) lw_enum_get_enum_mapping,
	                             (GSettingsBindSetMapping) lw_enum_set_enum_mapping,
	                             binding, (GDestroyNotify) lw_enum_binding_free);
}

/**
 * lw_load_gresource:
 * @path:  the path to the gresource file
 * 
 * Returns: a pointer to the GResource or NULL in case of fail
 *
 * Load a gresource file so that you can load resources in any function
 * of the application.
 * Displays an error in case of fail
 *
 * Since: 0.5
 */
GResource *
lw_load_gresource (const gchar *path)
{
    GError *error = NULL;
    GResource *resource = g_resource_load (path, &error);
    if (error)
    {
        g_warning ("Could not load resource : %s", error->message);
        g_error_free (error);
        return NULL;
    }
    g_resources_register (resource);
    return g_resource_ref(resource);
}

/**
 * lw_unload_gresource:
 * @resource: the GResource to unload
 *
 * Unload a gresource, making its files unavailable
 *
 * Since: 0.5
 */
void
lw_unload_gresource (GResource *resource)
{
    if (resource)
    {
        g_resources_unregister (resource);
        g_resource_unref (resource);
    }
}
