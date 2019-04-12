option(USE_APPINDICATOR "Use libappindicator to show the indicator" OFF)
if(USE_APPINDICATOR)
	pkg_check_modules(APPINDICATOR appindicator3-0.1 REQUIRED)

	set(VALA_PACKAGES ${VALA_PACKAGES} appindicator3-0.1)
	set(VALA_FLAGS ${VALA_FLAGS} --define=USE_APPINDICATOR)

	add_definitions(${APPINDICATOR_CFLAGS})
	include_directories(${APPINDICATOR_INCLUDE_DIRS})
	link_directories(${APPINDICATOR_LIBRARY_DIRS})
	link_libraries(${APPINDICATOR_LIBRARIES})
endif(USE_APPINDICATOR)
 
### LiveWallpaper Indicator ###
file(GLOB LWI_VALA_SOURCES *.vala ../lw-ui.vala ../plugin.vala ${CMAKE_CURRENT_BINARY_DIR}/../config.vala)

vala_precompile(
	LWI_SOURCES ${LWI_VALA_SOURCES}
	PACKAGES ${VALA_PACKAGES}
	OPTIONS ${VALA_FLAGS}
)

add_executable(livewallpaper-indicator ${LWI_SOURCES})

install(TARGETS livewallpaper-indicator DESTINATION bin)

