########### next target ###############
set( deco_test_SOURCES oxygengtkdeco_main.cpp )

add_executable( oxygen-gtk-deco ${deco_test_SOURCES} )

target_link_libraries( oxygen-gtk-deco ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )
