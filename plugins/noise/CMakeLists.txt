file(GLOB NOISE_XCF images/*.xcf)
file(GLOB NOISE_SOURCES src/*)

livewallpaper_plugin_c(
	noise
	SOURCES ${NOISE_SOURCES}
	SETTINGS noise.xml
    RESOURCE noise.gresource.xml
	ICON noise.svg
)

livewallpaper_plugin_build_xcf(noise FILES ${NOISE_XCF})

