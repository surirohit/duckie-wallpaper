file(GLOB NEXUS_XCF images/*.xcf)
file(GLOB NEXUS_SOURCES src/*)

livewallpaper_plugin_c(
	nexus
	SOURCES ${NEXUS_SOURCES}
	SETTINGS nexus.xml
    RESOURCE nexus.gresource.xml
	ICON nexus.svg
)

livewallpaper_plugin_build_xcf(nexus FILES ${NEXUS_XCF})

