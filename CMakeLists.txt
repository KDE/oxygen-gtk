project( oxygen-gtk )

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

# install prefix
include( UsePkgConfig )
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

    exec_program(
        ${PKGCONFIG_EXECUTABLE}
        ARGS --variable=prefix gtk+-2.0
        OUTPUT_VARIABLE GTK_PREFIX )

    set(CMAKE_INSTALL_PREFIX
        ${GTK_PREFIX} CACHE PATH "GTK install prefix" FORCE
        )

endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# X11
find_package( X11 REQUIRED )

# PKG_CONFIG
find_package(PkgConfig REQUIRED )

# GTK
pkg_check_modules( GTK REQUIRED gtk+-2.0 )

# Cairo
pkg_check_modules( CAIRO REQUIRED cairo )

# DBus
pkg_check_modules( DBUS dbus-glib-1 )

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
link_directories( ${GTK_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS} ${DBUS_LIBRARY_DIRS} )
include_directories( ${GTK_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRD} ${DBUS_INCLUDE_DIRS} ${X11_X11_INCLUDE_PATH} )

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
