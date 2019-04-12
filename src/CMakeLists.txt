configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
)

# include and link directories
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_BINARY_DIR}/include
	${CMAKE_SOURCE_DIR}/include
	${DEPS_INCLUDE_DIRS}
)

link_directories(
	${DEPS_LIBRARY_DIRS}
)

### LiveWallpaper executable ###
set(LW_SOURCES
	main.c
	application.c
	command-line.c
	window.c
	opengl-window.c
	clock.c
	plugins-engine.c
	power-manager.c
	fps-visualizer.c
)

add_executable(livewallpaper ${LW_SOURCES})
target_link_libraries(livewallpaper livewallpaper-core -lX11 ${DEPS_LIBRARIES})

set_target_properties(
	livewallpaper PROPERTIES
	COMPILE_FLAGS "${DEPS_CFLAGS_STR}"
	LINK_FLAGS "${DEPS_LDFLAGS_STR}"
)

# set rpath if liblivewallpaper-core.so is not installed in a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" _is_system_dir)
if("${_is_system_dir}" STREQUAL "-1")
	set_target_properties(
		livewallpaper PROPERTIES
		INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
	)
endif("${_is_system_dir}" STREQUAL "-1")

install(TARGETS livewallpaper DESTINATION bin)

