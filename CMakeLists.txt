project(livewallpaper C)
cmake_minimum_required(VERSION 2.8)

# version informations
set(LW_VERSION_MAJOR 0)
set(LW_VERSION_MINOR 5)
set(LW_VERSION_MICRO 0)
set(LIVEWALLPAPER_VERSION "${LW_VERSION_MAJOR}.${LW_VERSION_MINOR}.${LW_VERSION_MICRO}")

# use the LiveWallpaper cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(LiveWallpaperCommon)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -W -Wall -Wextra -Wno-missing-field-initializers")

# enable debugging
option(ENABLE_DEBUG "Enable debugging" OFF)
if(ENABLE_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -ggdb")
endif(ENABLE_DEBUG)

# enable optimization
option(ENABLE_OPTIMIZATION "Enable optimization" OFF)
if(ENABLE_OPTIMIZATION)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif(ENABLE_OPTIMIZATION)

# check dependencies
set(LIVEWALLPAPER_REQUIRES
    gobject-2.0
    gobject-introspection-1.0
    glib-2.0
    gio-2.0
    gdk-3.0
    gdk-pixbuf-2.0
    libpeas-1.0
    glew
    upower-glib
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DEPS REQUIRED ${LIVEWALLPAPER_REQUIRES})

find_package(OpenGL REQUIRED)

list(APPEND DEPS_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND DEPS_LIBRARIES ${OPENGL_LIBRARIES})

# convert cflags and ldflags list to a space separated string
set(DEPS_CFLAGS_STR "")
foreach(_flag ${DEPS_CFLAGS})
    set(DEPS_CFLAGS_STR "${DEPS_CFLAGS_STR} ${_flag}")
endforeach(_flag)

set(DEPS_LDFLAGS_STR "")
foreach(_flag ${DEPS_LDFLAGS})
    set(DEPS_LDFLAGS_STR "${DEPS_LDFLAGS_STR} ${_flag}")
endforeach(_flag)

# handle subdirectories
add_subdirectory(include)
add_subdirectory(livewallpaper-core)
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(cmake)
add_subdirectory(ui)
add_subdirectory(po)
add_subdirectory(plugins)
add_subdirectory(doc)
add_subdirectory(tools)

add_subdirectory(debian)

livewallpaper_plugin_post_install()

