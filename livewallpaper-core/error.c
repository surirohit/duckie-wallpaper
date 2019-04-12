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
 * SECTION: error
 * @Short_description: description of all error numbers
 * @Title: Error Numbers
 *
 * Description of all error numbers of the LiveWallpaper core.
 */

#include <livewallpaper/core.h>


/**
 * LW_CORE_ERROR:
 *
 * Error domain for the LiveWallpaper core library.
 */

/**
 * lw_core_error_quark:
 *
 * Returns: The LiveWallpaper core error #GQuark
 */
GQuark
lw_core_error_quark()
{
	return g_quark_from_static_string("lw-core-error-quark");
}

/**
 * LwCoreError:
 * @LW_CORE_ERROR_FAILED: Generic error
 * @LW_CORE_ERROR_CAIRO_FAILED: Calling a cairo function failed. For example the creation of a new cairo surface failed.
 *
 * Error codes returned by core functions.
 */

