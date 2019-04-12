###############################################################################
#
# Find Livewallpaper cmake module
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Copyright (C) 2012-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
#
###############################################################################
#
# This module tries to find the livewallpaper core.
#
# Once done this will define:
#     LIVEWALLPAPER_FOUND          ... set to true if livewallpaper is found
#
# Variables set by pkg-config:
#     LIVEWALLPAPER_VERSION        ... version of the livewallpaper core
#     LIVEWALLPAPER_PREFIX         ... livewallpaper installation prefix
#     LIVEWALLPAPER_INCLUDEDIR     ... include-dir of the module
#     LIVEWALLPAPER_LIBDIR         ... lib-dir of the module
#
#     LIVEWALLPAPER_LIBRARIES      ... only the libraries (without the '-l')
#     LIVEWALLPAPER_LIBRARY_DIRS   ... the paths of the libraries (without the '-L')
#     LIVEWALLPAPER_LDFLAGS        ... all required linker flags
#     LIVEWALLPAPER_LDFLAGS_OTHER  ... all other linker flags
#     LIVEWALLPAPER_INCLUDE_DIRS   ... the '-I' preprocessor flags (without the '-I')
#     LIVEWALLPAPER_CFLAGS         ... all required compiler flags
#     LIVEWALLPAPER_CFLAGS_OTHER   ... all other compiler flags
#
###############################################################################

find_package(PkgConfig QUIET)
find_package(OpenGL QUIET)

if(PKG_CONFIG_FOUND)
	pkg_check_modules(LIVEWALLPAPER livewallpaper QUIET)
endif(PKG_CONFIG_FOUND)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	LiveWallpaper
	REQUIRED_VARS LIVEWALLPAPER_PREFIX OPENGL_FOUND PKG_CONFIG_FOUND
	VERSION_VAR LIVEWALLPAPER_VERSION
)

list(APPEND LIVEWALLPAPER_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND LIVEWALLPAPER_LIBRARIES ${OPENGL_LIBRARIES})

