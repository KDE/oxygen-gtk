# For config.h to be available
include_directories( ${CMAKE_BINARY_DIR} )

########### next target ###############
set( oxygen_gtk_demo_SOURCES
    oxygenbuttondemowidget.cpp
    oxygendemodialog.cpp
    oxygendemowidget.cpp
    oxygenframedemowidget.cpp
    oxygeninfowidget.cpp
    oxygeninputdemowidget.cpp
    oxygenlistdemowidget.cpp
    oxygensignal.cpp
    oxygensignalhandler.cpp
    oxygensliderdemowidget.cpp
    oxygentimer.cpp
    oxygentabdemowidget.cpp
    oxygenversion.cpp
    oxygengtkdemo_main.cpp
 )

IF( WIN32 )
    add_executable( oxygen-gtk-demo WIN32 ${oxygen_gtk_demo_SOURCES} )
ELSE( WIN32 )
    add_executable( oxygen-gtk-demo ${oxygen_gtk_demo_SOURCES} )
ENDIF( WIN32 )

target_link_libraries( oxygen-gtk-demo ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )
install( TARGETS oxygen-gtk-demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

########### next target ###############
if( BUILD_DECO_TEST )
    set( deco_test_SOURCES oxygenversion.cpp oxygengtkdeco_main.cpp )
    add_executable( oxygen-gtk-deco ${deco_test_SOURCES} )
    target_link_libraries( oxygen-gtk-deco ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} ${CMAKE_DL_LIBS} )
    install( TARGETS oxygen-gtk-deco DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif( BUILD_DECO_TEST )
