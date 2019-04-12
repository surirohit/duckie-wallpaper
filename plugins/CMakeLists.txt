# set all required variables usually set by FindLivewallpaper.cmake
# LIVEWALLPAPER_VERSION is already set
set(LIVEWALLPAPER_PREFIX ${CMAKE_INSTALL_PREFIX})
set(LIVEWALLPAPER_INCLUDEDIR ${CMAKE_SOURCE_DIR}/include)
set(LIVEWALLPAPER_LIBDIR ${CMAKE_BINARY_DIR}/src)

set(LIVEWALLPAPER_LIBRARIES ${DEPS_LIBRARIES})
set(LIVEWALLPAPER_LIBRARY_DIRS ${DEPS_LIBRARY_DIRS})
set(LIVEWALLPAPER_LDFLAGS ${DEPS_LDFLAGS})
set(LIVEWALLPAPER_INCLUDE_DIRS ${DEPS_INCLUDE_DIRS})
set(LIVEWALLPAPER_CFLAGS ${DEPS_CFLAGS})

set(LIVEWALLPAPER_PLUGIN_INSTALL_TYPE "package")

# now include the plugin macros
include(LiveWallpaperPlugin)

# add plugins
add_subdirectory(galaxy)
add_subdirectory(nexus)
add_subdirectory(noise)
add_subdirectory(photoslide)
add_subdirectory(circles)
add_subdirectory(gradclock)
add_subdirectory(duckiegalaxy)
