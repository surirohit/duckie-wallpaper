###############################################################################
#
# LiveWallpaper
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
# This module provides the following macros:
# (inspired by CompizPlugin.cmake)
#
# livewallpaper_plugin_c(<plugin name>
#                        SOURCES source1 source2 ...
#                        SETTINGS settings.xml
#                        [RESOURCE resource.gresource.xml]
#                        [ICON icon]
#                        [PKGDEPS dep1 dep2 ...]
#                        [LDFLAGSADD flag1 flag2 ...]
#                        [CFLAGSADD flag1 flag2 ...]
#                        [LIBRARIES flag1 flag2 ...]
#                        [LIBDIRS dir1 dir2 ...]
#                        [INCDIRS dir1 dir2 ...])
#
# or for python plugins:
#
# livewallpaper_plugin_python(<plugin name>
#                             [SOURCES source1 source2]
#                             [PACKAGES package1 package2]
#                             SETTINGS settings.xml
#                             [RESOURCE resource.gresource.xml]
#                             [ICON icon])
#
# SOURCES    = all source files of the plugin
# PACKAGES   = python packages to install into the plugin's binary dir
# SETTINGS   = the plugin's settings file (see documentation for further information)
# ICON       = the plugin's icon.svg file
# PKGDEPS    = pkgconfig dependencies
# LDFLAGSADD = flags added to the link command
# CFLAGSADD  = flags added to the compile command
# LIBRARIES  = libraries added to the link command
# LIBDIRS    = additional link directories
# INCDIRS    = additional include directories
#
# These macros are used to compile and install plugins and their settings.
#
# To convert GIMP xcf files to png during the build and install them
# (instead of having the source file and the usable file on your project), use:
#
# livewallpaper_plugin_build_xcf(<plugin>
#                                FILES file1 file2 ...
#                                [OTHER PARAMETERS])
#
# To install additional data files use:
#
# livewallpaper_plugin_install_data(<plugin name>
#                                   FILES file1 file2 ...
#                                   [OTHER PARAMETERS])
#
# or for directories:
#
# livewallpaper_plugin_install_data(<plugin name>
#                                   DIRECTORY dir1 dir2 ...
#                                   [OTHER PARAMETERS])
#
# This macro is only a wrapper for cmake's install command, so the documentation
# of all OTHER PARAMETERS can be found here:
#     http://www.cmake.org/cmake/help/v2.8.0/cmake.html#command:install
# You can use every parameter specified there except the DESTINATION parameter.
# The DESTINATION will always be the data dir of the given plugin.
#
#
# The following variables will be used by those macros:
#
# LIVEWALLPAPER_PLUGIN_INSTALL_TYPE = (package | livewallpaper | local (default))
#     package       = install into ${CMAKE_INSTALL_PREFIX}
#     livewallpaper = install into livewallpaper prefix
#                     (as specified in the livewallpaper.pc file)
#     local         = install into home directory
#
###############################################################################

include(LiveWallpaperCommon)

_find_required_program(GLIB_COMPILE_SCHEMAS_EXECUTABLE glib-compile-schemas)
_find_required_program(GLIB_COMPILE_RESOURCE_EXECUTABLE glib-compile-resources)
_find_required_program(XCF2PNG_EXECUTABLE xcf2png)

include(CMakeParseArguments)

macro(_init_directories)
	if(LIVEWALLPAPER_PLUGIN_INSTALL_TYPE)
		set(
			LIVEWALLPAPER_PLUGIN_INSTALL_TYPE ${LIVEWALLPAPER_PLUGIN_INSTALL_TYPE} CACHE STRING
			"Where a plugin should be installed \(package | livewallpaper | local\)"
		)
	else(LIVEWALLPAPER_PLUGIN_INSTALL_TYPE)
		set(
			LIVEWALLPAPER_PLUGIN_INSTALL_TYPE "local" CACHE STRING
			"Where a plugin should be installed \(package | livewallpaper | local\)"
		)
	endif(LIVEWALLPAPER_PLUGIN_INSTALL_TYPE)

	if("${LIVEWALLPAPER_PLUGIN_INSTALL_TYPE}" STREQUAL "package")
		set(PLUGIN_LIBDIR     ${CMAKE_INSTALL_PREFIX}/lib/livewallpaper/plugins)
		set(PLUGIN_DATADIR    ${CMAKE_INSTALL_PREFIX}/share/livewallpaper/plugins)
		set(PLUGIN_SCHEMA_DIR ${CMAKE_INSTALL_PREFIX}/share/glib-2.0/schemas)
		set(PLUGIN_ICON_DIR   ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
	elseif("${LIVEWALLPAPER_PLUGIN_INSTALL_TYPE}" STREQUAL "livewallpaper")
		set(PLUGIN_LIBDIR     ${LIVEWALLPAPER_PREFIX}/lib/livewallpaper/plugins)
		set(PLUGIN_DATADIR    ${LIVEWALLPAPER_PREFIX}/share/livewallpaper/plugins)
		set(PLUGIN_SCHEMA_DIR ${LIVEWALLPAPER_PREFIX}/share/glib-2.0/schemas)
		set(PLUGIN_ICON_DIR   ${LIVEWALLPAPER_PREFIX}/share/icons/hicolor/scalable/apps)
	else("${LIVEWALLPAPER_PLUGIN_INSTALL_TYPE}" STREQUAL "local")
		set(PLUGIN_LIBDIR     $ENV{HOME}/.local/share/livewallpaper/plugins)
		set(PLUGIN_DATADIR    $ENV{HOME}/.local/share/livewallpaper/plugins)
		set(PLUGIN_SCHEMA_DIR $ENV{HOME}/.local/share/glib-2.0/schemas)
		set(PLUGIN_ICON_DIR   $ENV{HOME}/.local/share/icons/hicolor/scalable/apps)
	endif("${LIVEWALLPAPER_PLUGIN_INSTALL_TYPE}" STREQUAL "package")
endmacro(_init_directories)

macro(_livewallpaper_plugin_generic plugin settings resource icon)
	# Look for a .plugin.in file
	find_file(
		_${plugin}_plugin_in ${plugin}.plugin.in
		PATHS ${CMAKE_CURRENT_SOURCE_DIR}
		NO_DEFAULT_PATH
	)

	# If there is a .plugin.in file, configure it
	# otherwise look for a .plugin file
	if(_${plugin}_plugin_in)
		set(_${plugin}_plugin ${CMAKE_CURRENT_BINARY_DIR}/${plugin}.plugin)
		configure_file(
			${_${plugin}_plugin_in}
			${_${plugin}_plugin}
		)
	else(_${plugin}_plugin_in)
		find_file(
			_${plugin}_plugin ${plugin}.plugin
			PATHS ${CMAKE_CURRENT_SOURCE_DIR}
			NO_DEFAULT_PATH
		)
	endif(_${plugin}_plugin_in)

	mark_as_advanced(_${plugin}_plugin _${plugin}_plugin_in)

	# Finally install the .plugin file
	if(_${plugin}_plugin)
		install(
			FILES ${_${plugin}_plugin}
			DESTINATION ${PLUGIN_LIBDIR}/${plugin}
		)
	endif(_${plugin}_plugin)


    # Install icon
    if (NOT ${icon} STREQUAL "")
        get_filename_component(ICON_EXT ${icon} EXT)
        if(NOT "${ICON_EXT}" STREQUAL ".svg")
            message(FATAL_ERROR "Icon for \"${plugin}\" plugin is not a svg image.")
        endif(NOT "${ICON_EXT}" STREQUAL ".svg")

        install(
            FILES ${icon}
            DESTINATION ${PLUGIN_ICON_DIR}
            RENAME livewallpaper-${plugin}.svg
        )
    endif (NOT ${icon} STREQUAL "")


	livewallpaper_generate_schema(_${plugin}_schema ${settings})

	# Install settings
	install(
		FILES ${settings}
		DESTINATION ${PLUGIN_LIBDIR}/${plugin}
		RENAME ${plugin}.xml
	)
	install(
		FILES ${_${plugin}_schema}
		DESTINATION ${PLUGIN_SCHEMA_DIR}
	)


	# Generate .png files from .xcf files
	# The .png files are not generated here. The livewallpaper_plugin_build_xcf macro generates
	# them. This is just a dummy command to add dependencies to the .png files
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${plugin}-xcf
		COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/${plugin}-xcf
	)


    # Generate and install .gresource file
    set(_${plugin}_gresource)
    if (NOT ${resource} STREQUAL "")
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${plugin}.gresource
			COMMAND ${GLIB_COMPILE_RESOURCE_EXECUTABLE}
				--sourcedir=${CMAKE_CURRENT_SOURCE_DIR}
				--sourcedir=${CMAKE_CURRENT_BINARY_DIR}
				--target=${CMAKE_CURRENT_BINARY_DIR}/${plugin}.gresource
				${CMAKE_CURRENT_SOURCE_DIR}/${resource}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${resource}
				${CMAKE_CURRENT_BINARY_DIR}/${plugin}-xcf
		)

        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${plugin}.gresource
            DESTINATION ${PLUGIN_DATADIR}/${plugin}
        )

	    set(_${plugin}_gresource ${CMAKE_CURRENT_BINARY_DIR}/${plugin}.gresource)
    endif (NOT ${resource} STREQUAL "")


	# add target to generate all other files required by the plugin
	add_custom_target(${plugin}-resources
		DEPENDS ${_${plugin}_schema} ${CMAKE_CURRENT_BINARY_DIR}/${plugin}-xcf ${_${plugin}_gresource}
	)
	add_dependencies(${plugin} ${plugin}-resources)
endmacro(_livewallpaper_plugin_generic)

macro(livewallpaper_plugin_c plugin)
	string(TOUPPER ${plugin} _PLUGIN)

	cmake_parse_arguments(${_PLUGIN} "" "SETTINGS;RESOURCE;ICON" "SOURCES;PKGDEPS;LDFLAGSADD;CFLAGSADD;LIBRARIES;LIBDIRS;INCDIRS" ${ARGN})
	_init_directories()

	# Check additional dependencies
	if(${_PLUGIN}_PKGDEPS)
		find_package(PkgConfig REQUIRED)

		pkg_check_modules(_${plugin}_deps REQUIRED ${${_PLUGIN}_PKGDEPS})

		list(APPEND ${_PLUGIN}_CFLAGSADD ${_${plugin}_deps_CFLAGS})
		list(APPEND ${_PLUGIN}_LDFLAGSADD ${_${plugin}_deps_LDFLAGS})
		list(APPEND ${_PLUGIN}_LIBRARIES ${_${plugin}_deps_LIBRARIES})
		list(APPEND ${_PLUGIN}_LIBDIRS ${_${plugin}_deps_LIBRARY_DIRS})
		list(APPEND ${_PLUGIN}_INCDIRS ${_${plugin}_deps_INCLUDE_DIRS})
	endif(${_PLUGIN}_PKGDEPS)

	# Compile plugin
	include_directories(
		${${_PLUGIN}_INCDIRS}
		${LIVEWALLPAPER_INCLUDEDIR}
		${LIVEWALLPAPER_INCLUDE_DIRS}
	)

	link_directories(
		${${_PLUGIN}_LIBDIRS}
		${LIVEWALLPAPER_LIBDIR}
		${LIVEWALLPAPER_LIBRARY_DIRS}
	)

	add_library(
		${plugin} SHARED ${${_PLUGIN}_SOURCES}
	)

	install(
		TARGETS ${plugin}
		DESTINATION ${PLUGIN_LIBDIR}/${plugin}
	)

	target_link_libraries(
		${plugin}
		${${_PLUGIN}_LIBRARIES}
		${LIVEWALLPAPER_LIBRARIES}
	)

	# Set cflags and ldflags
	set(_${plugin}_properties)
	foreach(_flag ${${_PLUGIN}_CFLAGSADD} ${LIVEWALLPAPER_CFLAGS})
		list(APPEND _${plugin}_properties COMPILE_FLAGS ${_flag})
	endforeach(_flag)
	foreach(_flag ${${_PLUGIN}_LDFLAGSADD} ${LIVEWALLPAPER_LDFLAGS})
		list(APPEND _${plugin}_properties LINK_FLAGS ${_flag})
	endforeach(_flag)

	set_target_properties(
		${plugin} PROPERTIES
		${_${plugin}_properties}
	)

	_livewallpaper_plugin_generic(${plugin} ${${_PLUGIN}_SETTINGS} "${${_PLUGIN}_RESOURCE}" "${${_PLUGIN}_ICON}")
endmacro(livewallpaper_plugin_c)

macro(livewallpaper_plugin_python plugin)
	string(TOUPPER ${plugin} _PLUGIN)

	cmake_parse_arguments(${_PLUGIN} "" "SETTINGS;RESOURCE;ICON" "SOURCES;PACKAGES" ${ARGN})
	_init_directories()

	# Create empty target for a python plugin
	add_custom_target(${plugin} ALL)

	# Install python files
	install(
		FILES ${${_PLUGIN}_SOURCES}
		DESTINATION ${PLUGIN_LIBDIR}/${plugin}
	)

	# Install packages
	install(
		DIRECTORY ${${_PLUGIN}_PACKAGES}
		DESTINATION ${PLUGIN_LIBDIR}/${plugin}
	)

	_livewallpaper_plugin_generic(${plugin} ${${_PLUGIN}_SETTINGS} "${${_PLUGIN}_RESOURCE}" "${${_PLUGIN}_ICON}")
endmacro(livewallpaper_plugin_python)

macro(livewallpaper_plugin_build_xcf plugin)
	cmake_parse_arguments(_XCF "" "DESTINATION" "FILES" ${ARGN})

	# Get default path if no destination directory is given
	if("${_XCF_DESTINATION}" STREQUAL "")
		set(_XCF_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/images")
	endif("${_XCF_DESTINATION}" STREQUAL "")

	# Get absolute destination path
	if(NOT "${_XCF_DESTINATION}" MATCHES "^/.*")
		set(_XCF_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${_XCF_DESTINATION}")
	endif(NOT "${_XCF_DESTINATION}" MATCHES "^/.*")

	# Build every png file
    foreach (_xcf ${_XCF_FILES})
    	get_filename_component(_name ${_xcf} NAME)
        string(REPLACE ".xcf" ".png" _png ${_name})

        add_custom_command(
        	OUTPUT ${_XCF_DESTINATION}/${_png}
        	COMMAND mkdir -p ${_XCF_DESTINATION}
        	COMMAND ${XCF2PNG_EXECUTABLE} ${_xcf} -o ${_XCF_DESTINATION}/${_png}
        	DEPENDS ${_xcf}
        )

        add_custom_command(
        	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${plugin}-xcf
        	DEPENDS ${_XCF_DESTINATION}/${_png}
        	APPEND
        )
    endforeach(_xcf)
endmacro(livewallpaper_plugin_build_xcf)

macro(livewallpaper_plugin_install_data plugin)
	_init_directories()

	install(${ARGN} DESTINATION ${PLUGIN_DATADIR}/${plugin})
endmacro(livewallpaper_plugin_install_data)

macro(livewallpaper_plugin_post_install)
    install(CODE "execute_process(COMMAND ${GLIB_COMPILE_SCHEMAS_EXECUTABLE} ${PLUGIN_SCHEMA_DIR})")
endmacro(livewallpaper_plugin_post_install)

