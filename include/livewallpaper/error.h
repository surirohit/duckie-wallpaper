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

#ifndef _LW_ERROR_H_
#define _LW_ERROR_H_

G_BEGIN_DECLS

#define LW_CORE_ERROR lw_core_error_quark()

GQuark lw_core_error_quark();

typedef enum
{
	LW_CORE_ERROR_FAILED,
	LW_CORE_ERROR_CAIRO_FAILED
} LwCoreError;

G_END_DECLS

#endif /* _LW_ERROR_H_ */

