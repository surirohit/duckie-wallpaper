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
 * Copyright (C) 2013-2016 Koichi Akabe <vbkaisetsu@gmail.com>
 *
 */

#include <gdk/gdk.h>
#include <cairo.h>
#include <math.h>

#define _PI (float) 3.141593

void draw_grad_circle (cairo_t *cr, double inner, double outer, double center_x, double center_y, GdkRGBA color)
{
#if CAIRO_VERSION_MAJOR >= 1 && CAIRO_VERSION_MINOR >= 12
	cairo_pattern_t *pattern = cairo_pattern_create_mesh ();
	
	cairo_mesh_pattern_begin_patch (pattern);
	cairo_mesh_pattern_move_to (pattern, inner, 0.0);
	cairo_mesh_pattern_line_to (pattern, outer, 0.0);
	cairo_mesh_pattern_curve_to (pattern, outer, 0.552284749 * outer, 0.552284749 * outer, outer, 0.0, outer);
	cairo_mesh_pattern_line_to (pattern, 0.0, inner);
	cairo_mesh_pattern_curve_to (pattern, 0.552284749 * inner, inner, inner, 0.552284749 * inner, inner, 0.0);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 0, color.red, color.green, color.blue, 0.00);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 1, color.red, color.green, color.blue, 0.00);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 2, color.red, color.green, color.blue, 0.33 * color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 3, color.red, color.green, color.blue, 0.33 * color.alpha);
	cairo_mesh_pattern_end_patch (pattern);

	cairo_mesh_pattern_begin_patch (pattern);
	cairo_mesh_pattern_move_to (pattern, 0.0, inner);
	cairo_mesh_pattern_line_to (pattern, 0.0, outer);
	cairo_mesh_pattern_curve_to (pattern, -0.552284749 * outer, outer, -outer, 0.552284749 * outer, -outer, 0.0);
	cairo_mesh_pattern_line_to (pattern, -inner, 0.0);
	cairo_mesh_pattern_curve_to (pattern, -inner, 0.552284749 * inner, -0.552284749 * inner, inner, 0.0, inner);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 0, color.red, color.green, color.blue, 0.33 * color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 1, color.red, color.green, color.blue, 0.33 * color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 2, color.red, color.green, color.blue, 0.66 * color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 3, color.red, color.green, color.blue, 0.66 * color.alpha);
	cairo_mesh_pattern_end_patch (pattern);

	cairo_mesh_pattern_begin_patch (pattern);
	cairo_mesh_pattern_move_to (pattern, -inner, 0.0);
	cairo_mesh_pattern_line_to (pattern, -outer, 0.0);
	cairo_mesh_pattern_curve_to (pattern, -outer, -0.552284749 * outer, -0.552284749 * outer, -outer, 0.0, -outer);
	cairo_mesh_pattern_line_to (pattern, 0.0, -inner);
	cairo_mesh_pattern_curve_to (pattern, -0.552284749 * inner, -inner, -inner, -0.552284749 * inner, -inner, 0.0);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 0, color.red, color.green, color.blue, 0.66 * color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 1, color.red, color.green, color.blue, 0.66 * color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 2, color.red, color.green, color.blue, color.alpha);
	cairo_mesh_pattern_set_corner_color_rgba (pattern, 3, color.red, color.green, color.blue, color.alpha);
	cairo_mesh_pattern_end_patch (pattern);

	cairo_translate (cr, center_x, center_y);
	cairo_set_source (cr, pattern);
	
	cairo_new_path (cr);
	cairo_arc (cr, 0.0, 0.0, outer - 1, 0.0, 2.0 * _PI);
	cairo_new_sub_path (cr);
	cairo_arc (cr, 0.0, 0.0, inner + 1, 0.0, 2.0 * _PI);
#else
	cairo_pattern_t *pattern = cairo_pattern_create_linear (0.0, -outer,  0.0, outer);
	cairo_pattern_add_color_stop_rgba (pattern, 0, color.red, color.green, color.blue, color.alpha);
	cairo_pattern_add_color_stop_rgba (pattern, 1, color.red, color.green, color.blue, 0.0);
	
	cairo_translate (cr, center_x, center_y);
	cairo_move_to (cr, 0.0, outer);
	cairo_curve_to (cr, -0.552284749 * outer, outer, -outer, 0.552284749 * outer, -outer, 0.0);
	cairo_curve_to (cr, -outer, -0.552284749 * outer, -0.552284749 * outer, -outer, 0.0, -outer);
	cairo_line_to (cr, 0.0, -inner);
	cairo_curve_to (cr, -0.552284749 * inner, -inner, -inner, -0.552284749 * inner, -inner, 0.0);
	cairo_curve_to (cr, -inner, 0.552284749 * inner, -0.552284749 * inner, inner, 0.0, inner);
	cairo_close_path (cr);
#endif
	cairo_set_source (cr, pattern);
	cairo_translate (cr, -center_x, -center_y);
	cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
	cairo_fill (cr);
	cairo_pattern_destroy (pattern);
}

