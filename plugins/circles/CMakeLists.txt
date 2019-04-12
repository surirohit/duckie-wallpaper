
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/circles.xml.in
	${CMAKE_CURRENT_BINARY_DIR}/circles.xml
)

livewallpaper_plugin_python(
	circles
	SOURCES circles.py
	SETTINGS ${CMAKE_CURRENT_BINARY_DIR}/circles.xml
	ICON circles.svg
)

livewallpaper_plugin_install_data(circles FILES color.svg)

