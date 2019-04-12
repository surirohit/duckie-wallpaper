# translate desktop files
_find_required_program(INTLTOOL_MERGE_EXECUTABLE intltool-merge)

set(LW_DESKTOP_FILES
	livewallpaper.desktop
	livewallpaper-autostart.desktop
	livewallpaper-config.desktop
	livewallpaper-indicator.desktop
	livewallpaper-config.appdata.xml
)

foreach(_file ${LW_DESKTOP_FILES})
	string(REGEX REPLACE ".*\\.(.+)$" "\\1" _file_ext ${_file})
	add_custom_command(
		OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${_file}
		COMMAND ${INTLTOOL_MERGE_EXECUTABLE} --${_file_ext}-style -q ${CMAKE_SOURCE_DIR}/po ${CMAKE_CURRENT_SOURCE_DIR}/${_file}.in ${CMAKE_CURRENT_BINARY_DIR}/${_file}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_file}.in
	)
endforeach(_file)

add_custom_target(
	translations-desktop ALL
	DEPENDS ${LW_DESKTOP_FILES}
)
add_dependencies(translations-desktop translations)

# generate and install global livewallpaper settings
livewallpaper_generate_schema(LW_GSETTINGS_SCHEMA_FILE livewallpaper.xml)
install(FILES livewallpaper.xml DESTINATION lib/livewallpaper)
install(FILES ${LW_GSETTINGS_SCHEMA_FILE} DESTINATION share/glib-2.0/schemas)

# install XDG autostart files
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper-autostart.desktop
          ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper-indicator.desktop
	DESTINATION /etc/xdg/autostart
)

# install menu entries
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper.desktop
	      ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper-config.desktop
	DESTINATION share/applications
)

# install appdata file
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper-config.appdata.xml
	DESTINATION share/appdata
)

# pkgconfig file
set(LIVEWALLPAPER_REQUIRES_STRING "")
foreach(_dep ${LIVEWALLPAPER_REQUIRES})
	set(LIVEWALLPAPER_REQUIRES_STRING "${LIVEWALLPAPER_REQUIRES_STRING} ${_dep}")
endforeach(_dep)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/livewallpaper.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/livewallpaper.pc
	@ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper.pc DESTINATION lib/pkgconfig)

# Icons
install (DIRECTORY icons DESTINATION share)

# plugin file
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/livewallpaper.plugin.in
	${CMAKE_CURRENT_BINARY_DIR}/livewallpaper.plugin
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper.plugin DESTINATION lib/livewallpaper)

# add target for resource files
add_custom_target(
	livewallpaper-resources
	DEPENDS ${LW_GSETTINGS_SCHEMA_FILE}
)
add_dependencies(livewallpaper livewallpaper-resources)

