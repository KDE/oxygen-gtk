include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} )
include_directories( animations )

# Sources
set( OXYGEN-GTK-SOURCES
    oxygenapplicationname.cpp
    oxygencairocontext.cpp
    oxygencairopattern.cpp
    oxygencairoutils.cpp
    oxygencoloreffect.cpp
    oxygencolorutils.cpp
    oxygenfontinfo.cpp
    oxygengtkcellinfo.cpp
    oxygengtkicons.cpp
    oxygengtkrc.cpp
    oxygengtktypenames.cpp
    oxygengtkutils.cpp
    oxygenoptionmap.cpp
    oxygenpalette.cpp
    oxygenqtsettings.cpp
    oxygenrcstyle.cpp
    oxygenrgba.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenstylewrapper.cpp
    oxygentaboptions.cpp
    oxygentheme.cpp
    oxygentileset.cpp
    oxygenwindecobutton.cpp
    oxygenwindowmanager.cpp
    animations/oxygenanimations.cpp
    animations/oxygenbackgroundhintengine.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygencomboboxdata.cpp
    animations/oxygencomboboxentrydata.cpp
    animations/oxygendialogengine.cpp
    animations/oxygenhoverdata.cpp
    animations/oxygenmainwindowdata.cpp
    animations/oxygenmenushelldata.cpp
    animations/oxygensignal.cpp
    animations/oxygenscrollbardata.cpp
    animations/oxygenscrolledwindowdata.cpp
    animations/oxygentabwidgetdata.cpp
    animations/oxygentimer.cpp
    animations/oxygentreeviewdata.cpp
    animations/oxygentreeviewengine.cpp
 )

add_library( oxygen-gtk SHARED ${OXYGEN-GTK-SOURCES} )

# Add GTK's cflags
set( CMAKE_CXX_FLAGS " -Wall ${CMAKE_CXX_FLAGS} ${GTK_CFLAGS} -g" )
set( CMAKE_C_FLAGS " -Wall ${CMAKE_C_FLAGS} ${GTK_CFLAGS} -g" )

target_link_libraries( oxygen-gtk ${GTK_LINK_FLAGS} ${X11_X11_LIB} )

install( TARGETS oxygen-gtk DESTINATION ${INSTALL_PATH_GTK_ENGINES} )
