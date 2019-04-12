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
 *
 * The color mapping functions are taken from livewallpaper-config. They
 * were originally inspirated by gnome-terminal (profile-editor.c).
 *
 * The color conversion functions are inspirated by GIMP:
 * http://git.gnome.org/browse/gimp/tree/libgimpcolor/gimpcolorspace.c?id=GIMP_2_6_11
 *
 */

/**
 * SECTION: color
 * @Short_description: various functions for color conversion / handling
 * @Title: Color Handling
 *
 * The following functions provide support for converting colors from one color
 * model to another color model. There are also some functions for getting/setting
 * #GdkRGBA objects from/to #GSettings strings.
 */

#include <livewallpaper/core.h>

/**
 * lw_color_get_color_mapping:
 * @value: Return location for the property value
 * @variant: The #GVariant
 * @p: User data that was specified when the binding was created
 *
 * This function is used to convert a color from a #GSettings string to a #GdkRGBA
 * object. Use this function as get mapping function of g_settings_bind_with_mapping()
 * or take a look at the lw_settings_bind_color() macro.
 *
 * Returns: %TRUE if the conversion succeeded, %FALSE in case of an error
 */
gboolean
lw_color_get_color_mapping(GValue *value, GVariant *variant, gpointer p G_GNUC_UNUSED)
{
	const gchar *s;
	GdkRGBA rgba;

	g_variant_get(variant, "&s", &s);
	if(!gdk_rgba_parse(&rgba, s)) return FALSE;

	g_value_set_boxed(value, &rgba);
	return TRUE;
}

/**
 * lw_color_set_color_mapping:
 * @value: A #GValue containing the property value to map
 * @type: The #GVariantType to create
 * @p: User data that was specified when the binding was created
 *
 * This function is used to convert a #GdkRGBA object to a #GSettings string. Use this
 * function as set mapping function of g_settings_bind_with_mapping() or take a look
 * at the lw_settings_bind_color() macro.
 *
 * Returns: A new #GVariant holding the data from value, or %NULL in case of an error
 */
GVariant*
lw_color_set_color_mapping(const GValue *value, const GVariantType *type G_GNUC_UNUSED, gpointer p G_GNUC_UNUSED)
{
	return g_variant_new_string( gdk_rgba_to_string(g_value_get_boxed(value)) );
}


/**
 * LwHSL:
 * @hue: The hue value from 0.0 to 1.0
 * @saturation: The saturation value from 0.0 to 1.0
 * @lightness: The lightness value from 0.0 to 1.0
 *
 * This structure represents a color in HSL color space.
 *
 * Since: 0.4
 */

G_DEFINE_BOXED_TYPE(LwHSL, lw_hsl, lw_hsl_copy, lw_hsl_free)

/**
 * lw_hsl_copy:
 * @hsl: A #LwHSL
 *
 * Makes a copy of a #LwHSL structure.
 *
 * Returns: A newly allocated #LwHSL. It should be freed through lw_hsl_free().
 *
 * Since: 0.4
 */
LwHSL*
lw_hsl_copy(const LwHSL *hsl)
{
	return g_slice_dup(LwHSL, hsl);
}

/**
 * lw_hsl_free:
 * @hsl: A #LwHSL
 *
 * Frees a #LwHSL structure created with lw_hsl_copy().
 *
 * Since: 0.4
 */
void
lw_hsl_free(LwHSL *hsl)
{
	g_slice_free(LwHSL, hsl);
}


/**
 * lw_rgb_to_hsl:
 * @rgb: A #GdkRGBA
 *
 * Converts a color from the RGB color model to the HSL color model.
 * The alpha value of the @rgb param is not used by this function.
 *
 * Returns: A newly allocated #LwHSL. It should be freed through lw_hsl_free().
 */
LwHSL*
lw_rgb_to_hsl(GdkRGBA *rgb)
{
	LwHSL hsl;
	gdouble min, max;
	gdouble delta;

#define r rgb->red
#define g rgb->green
#define b rgb->blue

#define h hsl.hue
#define s hsl.saturation
#define l hsl.lightness

	max = MAX(r, MAX(g, b));
	min = MIN(r, MIN(g, b));

	l = (max + min) / 2.0;

	if(max == min)
	{
		s = 0.0;
		h = 0.0;
	}
	else
	{
		delta = max - min;

		if(l <= 0.5)
			s = delta / (max + min);
		else
			s = delta / (2.0 - max - min);

		if(r == max)
			h = (g - b) / delta;
		else if(g == max)
			h = 2.0 + (b - r) / delta;
		else
			h = 4.0 + (r - g) / delta;

		h /= 6.0;

		if(h < 0.0)
			h += 1.0;
		else if(h > 1.0)
			h -= 1.0;
	}

#undef r
#undef g
#undef b

#undef h
#undef s
#undef l

	return lw_hsl_copy(&hsl);
}

static inline gdouble
hsl_value(gdouble n1, gdouble n2, gdouble hue)
{
	if(hue < 0.0)
		hue += 6.0;
	else if(hue > 6.0)
		hue -= 6.0;

	if(hue < 1.0)
		return n1 + (n2 - n1) * hue;
	else if(hue < 3.0)
		return n2;
	else if(hue < 4.0)
		return n1 + (n2 - n1) * (4.0 - hue);
	else
		return n1;
}

/**
 * lw_hsl_to_rgb:
 * @hsl: A #LwHSL
 *
 * Converts a color from the HSL color model to the RGB color model.
 * The alpha value of the result will be undefined.
 *
 * Returns: A newly allocated #GdkRGBA. It should be freed through gdk_rgba_free().
 */
GdkRGBA*
lw_hsl_to_rgb(LwHSL *hsl)
{
	GdkRGBA rgb;

#define r rgb.red
#define g rgb.green
#define b rgb.blue

#define h hsl->hue
#define s hsl->saturation
#define l hsl->lightness

	if(s == 0.0)
	{
		r = g = b = l;
	}
	else
	{
		gdouble m1, m2;

		/* Case 1 */
		if(l <= 0.5)
			m2 = l * (1.0 + s);

		/* Case 2 */
		else
			m2 = l + s - l * s;

		m1 = 2.0 * l - m2;

		r = hsl_value(m1, m2, h * 6.0 + 2.0);
		g = hsl_value(m1, m2, h * 6.0);
		b = hsl_value(m1, m2, h * 6.0 - 2.0);
	}

#undef r
#undef g
#undef b

#undef h
#undef s
#undef l

	return gdk_rgba_copy(&rgb);
}

