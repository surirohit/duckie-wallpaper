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

macro(_find_required_program var_name program_name)
	find_program(${var_name} ${program_name} ${ARGN})

	if(NOT ${var_name})
		message(FATAL_ERROR "${program_name} not found")
	endif(NOT ${var_name})

	mark_as_advanced(${var_name})
endmacro(_find_required_program)

_find_required_program(LW_GENERATE_SCHEMA_EXECUTABLE lw-generate-schema PATHS ${CMAKE_SOURCE_DIR}/tools)

macro(livewallpaper_generate_schema schema settings)
	# Read schema id from the settings file
	get_filename_component(_settings_path ${settings} ABSOLUTE)
	execute_process(
		COMMAND sed -n "s/.*schema.*id=\"\\([a-zA-Z\\.]*\\)\".*/\\1/ p" ${_settings_path}
		OUTPUT_VARIABLE _schema_id
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	# Generate .gschema.xml file
	set(${schema} ${CMAKE_CURRENT_BINARY_DIR}/${_schema_id}.gschema.xml)
	add_custom_command(
		OUTPUT ${${schema}}
		COMMAND ${LW_GENERATE_SCHEMA_EXECUTABLE} -d ${CMAKE_CURRENT_BINARY_DIR} ${_settings_path}
		DEPENDS ${settings}
	)
endmacro(livewallpaper_generate_schema)

