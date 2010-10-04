include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} )

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
    oxygentheme.c
    oxygentileset.cpp
 )

add_library( oxygen-gtk SHARED ${OXYGEN-GTK-SOURCES} )

# Add GTK's cflags
set( CMAKE_CXX_FLAGS " -Wall -Werror ${CMAKE_CXX_FLAGS} ${GTK_CFLAGS}" )
set( CMAKE_C_FLAGS " -Wall -Werror ${CMAKE_C_FLAGS} ${GTK_CFLAGS}" )

target_link_libraries( oxygen-gtk ${GTK_LINK_FLAGS} ${X11_X11_LIB} )

install( TARGETS oxygen-gtk DESTINATION ${INSTALL_PATH_GTK_ENGINES} )
