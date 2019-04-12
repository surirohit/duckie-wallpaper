file(GLOB GALAXY_XCF images/*.xcf)
file(GLOB GALAXY_SOURCES src/*)

livewallpaper_plugin_c(
	galaxy
	SOURCES ${GALAXY_SOURCES}
	SETTINGS galaxy.xml
    RESOURCE galaxy.gresource.xml
	ICON galaxy.svg
)

livewallpaper_plugin_build_xcf(galaxy FILES ${GALAXY_XCF})

