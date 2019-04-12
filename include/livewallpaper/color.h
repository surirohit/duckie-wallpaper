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

#ifndef _LW_COLOR_H_
#define _LW_COLOR_H_

gboolean lw_color_get_color_mapping(GValue *value, GVariant *variant, gpointer p G_GNUC_UNUSED);
GVariant *lw_color_set_color_mapping(const GValue *value, const GVariantType *type G_GNUC_UNUSED, gpointer p G_GNUC_UNUSED);


#define LW_TYPE_HSL (lw_hsl_get_type())

typedef struct _LwHSL LwHSL;

struct _LwHSL
{
	gdouble hue;
	gdouble saturation;
	gdouble lightness;
};

GType lw_hsl_get_type(void);

LwHSL *lw_hsl_copy(const LwHSL *hsl);
void lw_hsl_free(LwHSL *hsl);


LwHSL *lw_rgb_to_hsl(GdkRGBA *rgb);
GdkRGBA *lw_hsl_to_rgb(LwHSL *hsl);

#endif /* _LW_COLOR_H_ */

