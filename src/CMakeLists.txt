include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} )
include_directories( animations )

# Sources
# please keep this list alphabetically sorted for clarity
set( oxygen_gtk_SOURCES
    animations/oxygenanimations.cpp
    animations/oxygenarrowstatedata.cpp
    animations/oxygenbackgroundhintengine.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygencomboboxdata.cpp
    animations/oxygencomboboxentrydata.cpp
    animations/oxygendialogengine.cpp
    animations/oxygenflatwidgetengine.cpp
    animations/oxygenfollowmousedata.cpp
    animations/oxygenhook.cpp
    animations/oxygenhoverdata.cpp
    animations/oxygeninnershadowdata.cpp
    animations/oxygenmainwindowdata.cpp
    animations/oxygenmenubarstatedata.cpp
    animations/oxygenmenustatedata.cpp
    animations/oxygenmenuitemengine.cpp
    animations/oxygenmenuitemdata.cpp
    animations/oxygenpaneddata.cpp
    animations/oxygensignal.cpp
    animations/oxygenscrollbardata.cpp
    animations/oxygenscrollbarstatedata.cpp
    animations/oxygenscrolledwindowdata.cpp
    animations/oxygentabwidgetdata.cpp
    animations/oxygentabwidgetstatedata.cpp
    animations/oxygentimeline.cpp
    animations/oxygentimelineserver.cpp
    animations/oxygentimer.cpp
    animations/oxygentoolbarstatedata.cpp
    animations/oxygentreeviewdata.cpp
    animations/oxygentreeviewengine.cpp
    animations/oxygentreeviewstatedata.cpp
    animations/oxygenwidgetsizedata.cpp
    animations/oxygenwidgetstatedata.cpp
    animations/oxygenwidgetstateengine.cpp
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
    oxygenloghandler.cpp
    oxygenobjectcounter.cpp
    oxygenobjectcountermap.cpp
    oxygenoption.cpp
    oxygenoptionmap.cpp
    oxygenpalette.cpp
    oxygenpathlist.cpp
    oxygenpropertynames.cpp
    oxygenqtsettings.cpp
    oxygenrcstyle.cpp
    oxygenrgba.cpp
    oxygenshadowconfiguration.cpp
    oxygenshadowhelper.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenstyleoptions.cpp
    oxygenstylewrapper.cpp
    oxygentaboptions.cpp
    oxygentheme.cpp
    oxygentileset.cpp
    oxygenwindecobutton.cpp
    oxygenwindecooptions.cpp
    oxygenwindowmanager.cpp
    oxygenwindowshadow.cpp
)

add_library( oxygen-gtk SHARED ${oxygen_gtk_SOURCES} )

# Add debug cflags
set( CMAKE_CXX_FLAGS " -Wall -g ${CMAKE_CXX_FLAGS}" )
set( CMAKE_C_FLAGS " -Wall -g ${CMAKE_C_FLAGS}" )

target_link_libraries( oxygen-gtk ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )

install( TARGETS oxygen-gtk DESTINATION ${INSTALL_PATH_GTK_ENGINES} )
