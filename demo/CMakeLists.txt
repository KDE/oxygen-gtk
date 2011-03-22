# Add debug cflags
set( CMAKE_CXX_FLAGS " -Wall -g ${CMAKE_CXX_FLAGS}" )
set( CMAKE_C_FLAGS " -Wall -g ${CMAKE_C_FLAGS}" )

########### next target ###############
set( oxygen_gtk_demo_SOURCES
    oxygenbuttondemowidget.cpp
    oxygendemodialog.cpp
    oxygendemowidget.cpp
    oxygeninputdemowidget.cpp
    oxygengtkdemo_main.cpp
    oxygensignal.cpp
 )

add_executable( oxygen-gtk3-demo ${oxygen_gtk_demo_SOURCES} )
target_link_libraries( oxygen-gtk3-demo ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )
install( TARGETS oxygen-gtk3-demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

# ########### next target ###############
# set( deco_test_SOURCES oxygengtkdeco_main.cpp )
# add_executable( oxygen-gtk-deco ${deco_test_SOURCES} )
# target_link_libraries( oxygen-gtk-deco ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )
# install( TARGETS oxygen-gtk-deco DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
