project( oxygen-gtk )
include( UsePkgConfig )

# Define package generation
set( CPACK_PACKAGE_NAME "oxygen-gtk" )
set( CPACK_SOURCE_PACKAGE_NAME "oxygen-gtk" )
set( CPACK_SOURCE_GENERATOR TGZ )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Oxygen Gtk Cairo theme" )
set( CPACK_PACKAGE_VENDOR "hugo.pereira@free.fr" )
set( CPACK_PACKAGE_VERSION_MAJOR "0" )
set( CPACK_PACKAGE_VERSION_MINOR "0" )
set( CPACK_PACKAGE_VERSION_PATCH "4" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}" )
include( CPack )

find_package( X11 REQUIRED )

# Debugging information
set( OXYGEN_DEBUG 0 )

# Find GTK
pkgconfig( gtk+-2.0 GTK_INCLUDE_DIR GTK_LIB_DIR GTK_LINK_FLAGS GTK_CFLAGS )

if( NOT GTK_INCLUDE_DIR )
	message( FATAL_ERROR "GTK was not found.  Ensure you have the GTK development libraries installed, and GTK's pkgconfig files are in your pkgconfig search path." )
endif( NOT GTK_INCLUDE_DIR )

message( STATUS "Found GTK include dir: ${GTK_INCLUDE_DIR}" )
message( STATUS "Found GTK library dir: ${GTK_LIB_DIR}" )

# Work out where to install stuff
exec_program(
	${PKGCONFIG_EXECUTABLE}
	ARGS --variable=gtk_binary_version gtk+-2.0
	OUTPUT_VARIABLE GTK_VERSION )

set( INSTALL_PATH_GTK_ENGINES "${GTK_LIB_DIR}/gtk-2.0/${GTK_VERSION}/engines" CACHE PATH "The directory in which to install the theme engine library( eg. /usr/lib/gtk-2.0/2.4.0/engines )" )
message( STATUS "Installing the GTK theme engine library to: ${INSTALL_PATH_GTK_ENGINES}" )

set( INSTALL_PATH_GTK_MODULES "${GTK_LIB_DIR}/gtk-2.0/modules" CACHE PATH "The directory in which to install the module library( eg. /usr/lib/gtk-2.0/modules )" )
message( STATUS "Installing the GTK module library to: ${INSTALL_PATH_GTK_MODULES}" )

exec_program(
	${PKGCONFIG_EXECUTABLE}
	ARGS --variable=prefix gtk+-2.0
	OUTPUT_VARIABLE GTK_PREFIX )

set( INSTALL_PATH_GTK_THEMES "${GTK_PREFIX}/share/themes" CACHE PATH "The directory in which to install the theme data( eg. /usr/share/themes )" )
set( GTK_THEME_DIR ${INSTALL_PATH_GTK_THEMES}/oxygen-gtk )
message( STATUS "Installing the GTK theme engine data to: ${INSTALL_PATH_GTK_THEMES}" )
message( STATUS "gtk_lib_dir ${GTK_LIB_DIR}" )

# Setup definitions and directories to be used in all subdirectories
link_directories( ${GTK_LIB_DIR} ${QT_LIBRARY_DIR} )
include_directories( ${GTK_INCLUDE_DIR} ${X11_X11_INCLUDE_PATH} )

# configure file
configure_file( config.h.cmake ${CMAKE_BINARY_DIR}/config.h )

# Subdirectories
add_subdirectory( src )
add_subdirectory( rc )
