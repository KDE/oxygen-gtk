include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} )
include_directories( animations )

# Sources
set( OXYGEN-GTK-SOURCES
    oxygencairocontext.cpp
    oxygencairopattern.cpp
    oxygencairoutils.cpp
    oxygencolorutils.cpp
    oxygenfontinfo.cpp
    oxygengtkicons.cpp
    oxygengtkrc.cpp
    oxygengtkutils.cpp
    oxygenmaps.cpp
    oxygenoptionmap.cpp
    oxygenqtsettings.cpp
    oxygenrcstyle.cpp
    oxygenrgba.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenstylewrapper.cpp
    oxygentaboptions.cpp
    oxygentheme.cpp
    oxygentileset.cpp
    animations/oxygenanimations.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygencomboboxdata.cpp
    animations/oxygenanimations.cpp
    animations/oxygenlineeditdata.cpp
    animations/oxygenmainwindowdata.cpp
    animations/oxygenmenushelldata.cpp
    animations/oxygenscrollbardata.cpp
    animations/oxygentabwidgetdata.cpp
 )

add_library( oxygen-gtk SHARED ${OXYGEN-GTK-SOURCES} )

# Add GTK's cflags
set( CMAKE_CXX_FLAGS " -Wall ${CMAKE_CXX_FLAGS} ${GTK_CFLAGS}" )
set( CMAKE_C_FLAGS " -Wall ${CMAKE_C_FLAGS} ${GTK_CFLAGS}" )

target_link_libraries( oxygen-gtk ${GTK_LINK_FLAGS} ${X11_X11_LIB} )

install( TARGETS oxygen-gtk DESTINATION ${INSTALL_PATH_GTK_ENGINES} )
