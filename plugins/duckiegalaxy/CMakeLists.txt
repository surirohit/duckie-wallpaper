file(GLOB DUCKIEGALAXY_XCF images/*.xcf)
file(GLOB DUCKIEGALAXY_SOURCES src/*)

livewallpaper_plugin_c(
	duckiegalaxy
	SOURCES ${DUCKIEGALAXY_SOURCES}
	SETTINGS duckiegalaxy.xml
    RESOURCE duckiegalaxy.gresource.xml
	ICON duckiegalaxy.svg
)

livewallpaper_plugin_build_xcf(duckiegalaxy FILES ${DUCKIEGALAXY_XCF})

