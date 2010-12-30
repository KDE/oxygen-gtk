project( oxygen-gtk )
include( UsePkgConfig )

cmake_minimum_required(VERSION 2.4)

###########################
# Define package generation
###########################

set( CPACK_PACKAGE_NAME "oxygen-gtk" )
set( CPACK_SOURCE_PACKAGE_NAME "oxygen-gtk" )
set( CPACK_SOURCE_GENERATOR TBZ2 )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Oxygen Gtk theme" )
set( CPACK_PACKAGE_VENDOR "hugo@oxygen-icons.org" )
set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "0" )
set( CPACK_PACKAGE_VERSION_PATCH "0" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "oxygen-gtk-1.0.0" )
include( CPack )

########################
# Package specific flags
########################

# Debugging information
set( OXYGEN_DEBUG 0 )

# Replace gtk stock icons with oxygen-icons
# if set to 1
set( OXYGEN_ICON_HACK 1 )

# Force KDE icons and fonts when running in non-KDE
set( OXYGEN_FORCE_KDE_ICONS_AND_FONTS 1 )

# Resize combobox list to compensate for combobox button glow width (may result in flicker on combo popup)
set( ENABLE_COMBOBOX_LIST_RESIZE 1 )

##############
# Requirements
##############

# X11
find_package( X11 REQUIRED )

# install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

    exec_program(
        ${PKGCONFIG_EXECUTABLE}
        ARGS --variable=prefix gtk+-2.0
        OUTPUT_VARIABLE GTK_PREFIX )

    set(CMAKE_INSTALL_PREFIX
        ${GTK_PREFIX} CACHE PATH "GTK install prefix" FORCE
        )

endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# GTK
pkgconfig( gtk+-2.0 GTK_INCLUDE_DIR GTK_LIB_DIR GTK_LINK_FLAGS GTK_CFLAGS )

if( NOT GTK_INCLUDE_DIR )
	message( FATAL_ERROR "GTK was not found.  Ensure you have the GTK development libraries installed, and GTK's pkgconfig files are in your pkgconfig search path." )
endif( NOT GTK_INCLUDE_DIR )

message( STATUS "Found gtk include dir: ${GTK_INCLUDE_DIR}" )
message( STATUS "Found gtk library dir: ${GTK_LIB_DIR}" )

# Cairo
pkgconfig( cairo CAIRO_INCLUDE_DIR CAIRO_LIB_DIR CAIRO_LINK_FLAGS CAIRO_CFLAGS )
if( NOT CAIRO_INCLUDE_DIR )
	message( FATAL_ERROR "Cairo was not found.  Ensure you have the Cairo development libraries installed, and Cairo's pkgconfig files are in your pkgconfig search path." )
endif( NOT CAIRO_INCLUDE_DIR )

message( STATUS "Found cairo include dir: ${CAIRO_INCLUDE_DIR}" )
message( STATUS "Found cairo library dir: ${CAIRO_LIB_DIR}" )

##########################
# installation directories
##########################
exec_program(
	${PKGCONFIG_EXECUTABLE}
	ARGS --variable=gtk_binary_version gtk+-2.0
	OUTPUT_VARIABLE GTK_VERSION )

set( INSTALL_PATH_GTK_ENGINES "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/gtk-2.0/${GTK_VERSION}/engines" CACHE PATH "The directory in which to install the theme engine library( eg. /usr/lib/gtk-2.0/2.4.0/engines )" )
message( STATUS "Installing the GTK theme engine library to: ${INSTALL_PATH_GTK_ENGINES}" )

set( INSTALL_PATH_GTK_THEMES "${CMAKE_INSTALL_PREFIX}/share/themes" CACHE PATH "The directory in which to install the theme data( eg. /usr/share/themes )" )
set( GTK_THEME_DIR ${INSTALL_PATH_GTK_THEMES}/oxygen-gtk )
message( STATUS "Installing the GTK theme engine data to: ${GTK_THEME_DIR}" )

######################################################################
# Setup definitions and directories to be used for linking and include
######################################################################
link_directories( ${GTK_LIB_DIR} ${CAIRO_LIB_DIR} ${QT_LIBRARY_DIR} )
include_directories( ${GTK_INCLUDE_DIR} ${CAIRO_INCLUDE_DIR} ${X11_X11_INCLUDE_PATH} )

#############################
# generate configuration file
#############################
configure_file( config.h.cmake ${CMAKE_BINARY_DIR}/config.h )

################
# Subdirectories
################
add_subdirectory( src )
add_subdirectory( rc )
add_subdirectory( special-icons )
