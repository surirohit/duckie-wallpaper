# find all required gtkdoc tools
_find_required_program(GTKDOC_SCAN_EXECUTABLE gtkdoc-scan)
_find_required_program(GTKDOC_SCANGOBJ_EXECUTABLE gtkdoc-scangobj)
_find_required_program(GTKDOC_MKDB_EXECUTABLE gtkdoc-mkdb)
_find_required_program(GTKDOC_MKHTML_EXECUTABLE gtkdoc-mkhtml)
_find_required_program(GTKDOC_FIXXREF_EXECUTABLE gtkdoc-fixxref)

set(LW_DOC_MODULE_NAME "livewallpaper-core")
set(LW_DOC_SOURCE_DIRS
	--source-dir=${CMAKE_SOURCE_DIR}/livewallpaper-core
	--source-dir=${CMAKE_SOURCE_DIR}/include/livewallpaper
)
file(GLOB LW_DOC_FILES ${CMAKE_SOURCE_DIR}/include/livewallpaper/*.h ${CMAKE_SOURCE_DIR}/livewallpaper-core/*.c)
set(GTKDOC_HTML_DIR /usr/share/gtk-doc/html)


# Copy additional files from doc to build dir
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/livewallpaper-core-docs.xml.in
	${CMAKE_CURRENT_BINARY_DIR}/livewallpaper-core-docs.xml
)

# Prepare gtkdoc-scangobj.cmake script
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/gtkdoc-scangobj.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/gtkdoc-scangobj.cmake
	@ONLY
)

### Setup ###
add_custom_command(
	OUTPUT setup-build.stamp
	COMMAND cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tut_settings.xml ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tut_livewallpaper_plugin.xml ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/livewallpaper-core-sections.txt ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND cmake -E touch setup-build.stamp
	DEPENDS livewallpaper-core-sections.txt tut_settings.xml tut_livewallpaper_plugin.xml
)

### Scanning header files ###
add_custom_command(
	OUTPUT scan-build.stamp
	COMMAND ${GTKDOC_SCAN_EXECUTABLE} --module=${LW_DOC_MODULE_NAME} ${LW_DOC_SOURCE_DIRS}
		--rebuild-types
	COMMAND cmake -P gtkdoc-scangobj.cmake
	COMMAND cmake -E touch scan-build.stamp
	DEPENDS setup-build.stamp livewallpaper-core gtkdoc-scangobj.cmake.in ${LW_DOC_FILES}
)

### Building XML ###
add_custom_command(
	OUTPUT xml-build.stamp
	COMMAND ${GTKDOC_MKDB_EXECUTABLE} --module=${LW_DOC_MODULE_NAME} ${LW_DOC_SOURCE_DIRS}
		--output-format=xml --xml-mode
	COMMAND cmake -E touch xml-build.stamp
	DEPENDS scan-build.stamp
)

### Building HTML ###
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html)
add_custom_command(
	OUTPUT html-build.stamp
	COMMAND ${GTKDOC_MKHTML_EXECUTABLE} ${LW_DOC_MODULE_NAME}
		${CMAKE_CURRENT_BINARY_DIR}/${LW_DOC_MODULE_NAME}-docs.xml
	COMMAND cmake -E touch ../html-build.stamp
	WORKING_DIRECTORY html
	DEPENDS xml-build.stamp ${CMAKE_CURRENT_BINARY_DIR}/livewallpaper-core-docs.xml
)

# Fix cross-references
add_custom_command(
	OUTPUT fixxref-build.stamp
	COMMAND ${GTKDOC_FIXXREF_EXECUTABLE} --module=${LW_DOC_MODULE_NAME}
		--module-dir=${CMAKE_CURRENT_BINARY_DIR}/html
		--html-dir=${GTKDOC_HTML_DIR}
	COMMAND cmake -E touch fixxref-build.stamp
	DEPENDS html-build.stamp
)

add_custom_target(doc ALL DEPENDS html-build.stamp fixxref-build.stamp)

# Install documentation
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION ${GTKDOC_HTML_DIR}/livewallpaper-core)

