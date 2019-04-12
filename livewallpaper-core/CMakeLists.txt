# include and link directories
include_directories(
	${CMAKE_SOURCE_DIR}/include
	${DEPS_INCLUDE_DIRS}
)

link_directories(
	${DEPS_LIBRARY_DIRS}
)

# public livewallpaper header
# Do not use file(GLOB ...) because the order is important!
set(_public_headers
	core.h
	util.h
	error.h
	color.h
	random.h
	range.h
	noise.h
	output.h
	texture.h
	cairo-texture.h
	shader.h
	program.h
	background.h
	wallpaper.h
	matrix.h
	buffer.h
)
foreach(_header ${_public_headers})
	# relative path to absolute path
	list(APPEND LW_PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/include/livewallpaper/${_header}")
endforeach(_header)

### livewallpaper core library ###
file(GLOB LW_CORE_SOURCES *.c)

add_library(livewallpaper-core SHARED ${LW_CORE_SOURCES})
target_link_libraries(livewallpaper-core ${DEPS_LIBRARIES})

set_target_properties(
	livewallpaper-core PROPERTIES
	COMPILE_FLAGS "${DEPS_CFLAGS_STR}"
	LINK_FLAGS "${DEPS_LDFLAGS_STR}"
	VERSION "${LIVEWALLPAPER_VERSION}"
)

install(TARGETS livewallpaper-core DESTINATION lib)

### gobject introspection ###
_find_required_program(G_IR_SCANNER_EXECUTABLE g-ir-scanner)
_find_required_program(G_IR_COMPILER_EXECUTABLE g-ir-compiler)
_find_required_program(SED_EXECUTABLE sed)

set(LW_GIR_NAMESPACE LW)
set(LW_GIR_VERSION 1.0)
set(LW_GIR_INCLUDES --include=GObject-2.0  --include=Gdk-3.0 --include=GdkPixbuf-2.0)
set(LW_GIR_PACKAGES --pkg=gobject-2.0 --pkg=gdk-3.0 --pkg=gdk-pixbuf-2.0)
set(LW_GIR_PREFIX Lw)
set(LW_GIR_FILES ${LW_PUBLIC_HEADERS} ${LW_CORE_SOURCES})

set(LW_GIR_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LW_GIR_NAMESPACE}-${LW_GIR_VERSION}.gir)
set(LW_GIR_TYPELIB ${CMAKE_CURRENT_BINARY_DIR}/${LW_GIR_NAMESPACE}-${LW_GIR_VERSION}.typelib)

add_custom_command(
	OUTPUT ${LW_GIR_OUTPUT}
	COMMAND ${G_IR_SCANNER_EXECUTABLE} --quiet --namespace=${LW_GIR_NAMESPACE}
		--nsversion=${LW_GIR_VERSION} ${LW_GIR_PACKAGES} ${LW_GIR_FILES}
		--identifier-prefix=${LW_GIR_PREFIX} --library=livewallpaper-core
		--library-path=${CMAKE_CURRENT_BINARY_DIR} ${LW_GIR_INCLUDES}
		-I${CMAKE_SOURCE_DIR}/include --output=${LW_GIR_OUTPUT}.in --warn-all
	COMMAND ${SED_EXECUTABLE} -e 's/name="wb/name="b/g' ${LW_GIR_OUTPUT}.in > ${LW_GIR_OUTPUT}
	DEPENDS livewallpaper-core
)

add_custom_command(
	OUTPUT ${LW_GIR_TYPELIB}
	COMMAND ${G_IR_COMPILER_EXECUTABLE} ${LW_GIR_OUTPUT} -o ${LW_GIR_TYPELIB}
	DEPENDS ${LW_GIR_OUTPUT}
)

add_custom_target(livewallpaper-typelib ALL DEPENDS ${LW_GIR_TYPELIB})

install(FILES ${LW_GIR_TYPELIB} DESTINATION lib/livewallpaper/girepository-1.0)

