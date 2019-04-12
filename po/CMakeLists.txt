find_package(Gettext REQUIRED)
mark_as_advanced(GETTEXT_MSGFMT_EXECUTABLE GETTEXT_MSGMERGE_EXECUTABLE)

file(GLOB PO_FILES *.po)
gettext_create_translations(livewallpaper.pot ALL ${PO_FILES})

