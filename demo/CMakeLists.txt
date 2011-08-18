# For config.h to be available
include_directories( ${CMAKE_BINARY_DIR} )

########### next target ###############
set( oxygen_gtk_demo_SOURCES
    oxygenbuttondemowidget.cpp
    oxygendemodialog.cpp
    oxygendemowidget.cpp
    oxygenframedemowidget.cpp
    oxygeninputdemowidget.cpp
    oxygenlistdemowidget.cpp
    oxygensignal.cpp
    oxygensliderdemowidget.cpp
    oxygentimer.cpp
    oxygentabdemowidget.cpp
    oxygengtkdemo_main.cpp
 )

add_executable( oxygen-gtk-demo ${oxygen_gtk_demo_SOURCES} )
target_link_libraries( oxygen-gtk-demo ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )
install( TARGETS oxygen-gtk-demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

########### next target ###############
set( deco_test_SOURCES oxygengtkdeco_main.cpp )
add_executable( oxygen-gtk-deco ${deco_test_SOURCES} )
target_link_libraries( oxygen-gtk-deco ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} ${CMAKE_DL_LIBS} )
install( TARGETS oxygen-gtk-deco DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
