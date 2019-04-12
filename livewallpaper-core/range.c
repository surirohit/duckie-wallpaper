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
 * SECTION: range
 * @Short_description: handling a range of values
 * @Title: LwRange
 *
 * #LwRange is used to give a plugin more diversity by using random
 * values between a specific minimum and maximum value. The advantage
 * of #LwRange is, that you only have to handle one property instead of
 * two.
 *
 * The main purpose of #LwRange is to get a minimum and a maximum value from
 * the configurator, so the first step is to create such a key in your
 * <link linkend="tut-schema">GSettings schema file</link>.
 *
 * The next step is to create a property that can hold a #LwRange and bind
 * the property to the key from the schema file by using lw_settings_bind_range().
 * You have to create the property with g_param_spec_boxed() to hold a #LwRange.
 *
 * The <ulink url="https://code.launchpad.net/~fyrmir/livewallpaper/example-plugins">example plugins</ulink>
 * hold some example code for how to use a #LwRange.
 */

#include <livewallpaper/core.h>


/**
 * LwRange:
 * @min: lower endpoint of the range
 * @max: upper endpoint of the range
 *
 * Since: 0.4
 */

G_DEFINE_BOXED_TYPE(LwRange, lw_range, lw_range_copy, lw_range_free)

/**
 * lw_range_copy:
 * @range: A #LwRange
 *
 * Makes a copy of a #LwRange structure.
 *
 * Returns: A newly allocated #LwRange. It should be freed through lw_range_free().
 *
 * Since: 0.4
 */
LwRange*
lw_range_copy(const LwRange *range)
{
	return g_slice_dup(LwRange, range);
}

/**
 * lw_range_free:
 * @range: A #LwRange
 *
 * Frees a #LwRange structure created with lw_range_copy().
 *
 * Since: 0.4
 */
void
lw_range_free(LwRange *range)
{
	g_slice_free(LwRange, range);
}

/**
 * lw_range_rand:
 * @r: A #LwRange structure
 *
 * Returns: A random integer that is contained in the range @r
 *
 * Since: 0.4
 */

/**
 * lw_range_randf:
 * @r: A #LwRange structure
 *
 * Returns: A random float that is contained in the range @r
 *
 * Since: 0.4
 */

/**
 * lw_range_clamp:
 * @r: A #LwRange structure
 * @v: An int value
 *
 * Returns: An integer that is next to @v and contained in the range @r
 *
 * Since: 0.4
 */

/**
 * lw_range_clampf:
 * @r: A #LwRange structure
 * @v: A float value
 *
 * Returns: A float that is next to @v and contained in the range @r
 *
 * Since: 0.4
 */

/**
 * lw_range_get_range_mapping:
 * @value: Return location for the property value
 * @variant: The #GVariant
 * @p: User data that was specified when the binding was created
 *
 * This function is used to convert a #GSettings key to a #LwRange.
 * Use this function as get mapping function of g_settings_bind_with_mapping()
 * or take a look at the lw_settings_bind_range() macro.
 *
 * Returns: %TRUE if the conversion succeeded, %FALSE in case of an error
 *
 * Since: 0.4
 */
gboolean
lw_range_get_range_mapping(GValue *value, GVariant *variant, gpointer p G_GNUC_UNUSED)
{
	gdouble min, max;
	LwRange range;

	g_variant_get(variant, "(dd)", &min, &max);

	/* Make sure, min is always the minimum */
	if(min < max)
	{
		range.min = min;
		range.max = max;
	}
	else
	{
		range.min = max;
		range.max = min;
	}
	
	g_value_set_boxed(value, &range);
	return TRUE;
}

/**
 * lw_range_set_range_mapping:
 * @value: A #GValue containing the property value to map
 * @type: The #GVariantType to create
 * @p: User data that was specified when the binding was created
 *
 * This function is used to convert a #LwRange to a #GSettings key.
 * Use this function as set mapping function of g_settings_bind_with_mapping()
 * or take a look at the lw_settings_bind_range() macro.
 *
 * Returns: A new #GVariant holding the data from value, or %NULL in case of an error
 *
 * Since: 0.4
 */
GVariant*
lw_range_set_range_mapping(const GValue *value, const GVariantType *type G_GNUC_UNUSED, gpointer p G_GNUC_UNUSED)
{
	LwRange *range = g_value_get_boxed(value);

	return g_variant_new("(dd)", range->min, range->max);
}

