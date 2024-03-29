# check requirements
find_package(Vala REQUIRED)

pkg_check_modules(GTK REQUIRED "gtk+-3.0 >= 3.10")

if(${GTK_VERSION} VERSION_GREATER "3.14" OR ${GTK_VERSION} VERSION_EQUAL "3.14")
	set(VALA_FLAGS ${VALA_FLAGS} --define=GTK_3_14_OR_HIGHER)
endif(${GTK_VERSION} VERSION_GREATER "3.14" OR ${GTK_VERSION} VERSION_EQUAL "3.14")

add_definitions(${GTK_CFLAGS})
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
link_libraries(${GTK_LIBRARIES})

# reset all c flags, set c flags for c files generated by valac
set(CMAKE_C_FLAGS "-w -DGETTEXT_PACKAGE=\\\"livewallpaper\\\"")

set(VALA_FLAGS "--target-glib=2.38")
set(VALA_PACKAGES gtk+-3.0 posix)

# enable debugging
if(ENABLE_DEBUG)
	set(VALA_FLAGS "${VALA_FLAGS} -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -ggdb")
endif(ENABLE_DEBUG)

# enable optimization
if(ENABLE_OPTIMIZATION)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif(ENABLE_OPTIMIZATION)

# shared config file
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.vala.in
	${CMAKE_CURRENT_BINARY_DIR}/config.vala
)

include(ValaPrecompile)
add_subdirectory(config)
add_subdirectory(indicator)

