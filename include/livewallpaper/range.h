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

#ifndef _LW_RANGE_H_
#define _LW_RANGE_H_

#define LW_TYPE_RANGE (lw_range_get_type())

typedef struct _LwRange LwRange;

struct _LwRange
{
	gfloat min;
	gfloat max;
};

GType lw_range_get_type(void);

LwRange *lw_range_copy(const LwRange *range);
void lw_range_free(LwRange *range);

#define lw_range_rand(r)  ((int) rand2f((r).min, (r).max))
#define lw_range_randf(r) (rand2f((r).min, (r).max))

#define lw_range_clamp(r, v)  ((int) clampf(v, (r).min, (r).max))
#define lw_range_clampf(r, v) (clampf(v, (r).min, (r).max))

gboolean lw_range_get_range_mapping(GValue *value, GVariant *variant, gpointer p G_GNUC_UNUSED);
GVariant *lw_range_set_range_mapping(const GValue *value, const GVariantType *type G_GNUC_UNUSED, gpointer p G_GNUC_UNUSED);

#endif /* _LW_RANGE_H_ */

