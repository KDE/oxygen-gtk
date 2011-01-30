include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} )
include_directories( animations )

# Sources
# please keep this list alphabetically sorted for clarity
set( OXYGEN-GTK-SOURCES
    animations/oxygenanimations.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygencomboboxdata.cpp
    animations/oxygencomboboxentrydata.cpp
    animations/oxygendialogengine.cpp
    animations/oxygenhook.cpp
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
    oxygenapplicationname.cpp
    oxygenargbhelper.cpp
    oxygencairocontext.cpp
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
    oxygenpathlist.cpp
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
)

add_library( oxygen-gtk SHARED ${OXYGEN-GTK-SOURCES} )

# Add GTK's cflags
set( CMAKE_CXX_FLAGS " -Wall ${CMAKE_CXX_FLAGS}" )
set( CMAKE_C_FLAGS " -Wall ${CMAKE_C_FLAGS}" )

target_link_libraries( oxygen-gtk ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )

install( TARGETS oxygen-gtk DESTINATION ${INSTALL_PATH_GTK_ENGINES} )
