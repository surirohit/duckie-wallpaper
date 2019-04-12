# generate resources.c
# no add_subdirectory because otherwise the c file is not visible to add_executable
_find_required_program(GLIB_COMPILE_RESOURCE_EXECUTABLE glib-compile-resources)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    COMMAND ${GLIB_COMPILE_RESOURCE_EXECUTABLE}
        --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/resources
        --target=${CMAKE_CURRENT_BINARY_DIR}/resources.c
        --c-name Lwc
        --generate-source
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/gresources.xml
)

### LiveWallpaper Configurator ###
file (GLOB LWC_VALA_SOURCES *.vala ../*.vala ${CMAKE_CURRENT_BINARY_DIR}/../config.vala)

vala_precompile(
	LWC_SOURCES ${LWC_VALA_SOURCES}
	PACKAGES ${VALA_PACKAGES}
	OPTIONS ${VALA_FLAGS} --gresources=${CMAKE_CURRENT_SOURCE_DIR}/resources/gresources.xml
)

# append additional c sources
list(APPEND LWC_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/resources.c" "${CMAKE_CURRENT_SOURCE_DIR}/search-workaround.c")

add_executable(livewallpaper-config ${LWC_SOURCES})
target_link_libraries(livewallpaper-config m)

install(TARGETS livewallpaper-config DESTINATION bin)
