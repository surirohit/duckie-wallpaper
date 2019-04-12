
file(GLOB GRADCLOCK_SOURCES src/*)

livewallpaper_plugin_c(
	gradclock
	SOURCES ${GRADCLOCK_SOURCES}
	SETTINGS gradclock.xml
	ICON gradclock.svg
)

