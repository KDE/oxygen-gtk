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
set( CPACK_PACKAGE_VERSION_MINOR "1" )
set( CPACK_PACKAGE_VERSION_PATCH "0" )
set( CPACK_SOURCE_IGNORE_FILES "build" "^${PROJECT_SOURCE_DIR}.*/.git/" )

set( CPACK_SOURCE_PACKAGE_FILE_NAME "oxygen-gtk-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
include( CPack )
########################
# Package specific flags
########################

# Set to 1 to enable debugging information
if( NOT DEFINED OXYGEN_DEBUG )
    set( OXYGEN_DEBUG 0 )
endif( NOT DEFINED OXYGEN_DEBUG )

# Set to 1 to replace gtk stock icons
# with oxygen-icons
if( NOT DEFINED OXYGEN_ICON_HACK )
    set( OXYGEN_ICON_HACK 1 )
endif( NOT DEFINED OXYGEN_ICON_HACK )

# Set to 1 to force KDE icons and fonts when running in non-KDE
if( NOT DEFINED OXYGEN_FORCE_KDE_ICONS_AND_FONTS )
    set( OXYGEN_FORCE_KDE_ICONS_AND_FONTS 1 )
endif( NOT DEFINED OXYGEN_FORCE_KDE_ICONS_AND_FONTS )

# Set to 1 to resize combobox list to compensate for combobox button glow width
# (may result in flicker on combo popup)
if( NOT DEFINED ENABLE_COMBOBOX_LIST_RESIZE )
    set( ENABLE_COMBOBOX_LIST_RESIZE 1 )
endif( NOT DEFINED ENABLE_COMBOBOX_LIST_RESIZE )

# Set to 0 to disable inner shadows hack
# Note this flag is automatically set to 0 if GTK version is too old (< 2.24.2)
if( NOT DEFINED ENABLE_INNER_SHADOWS_HACK )
    set( ENABLE_INNER_SHADOWS_HACK 1 )
endif( NOT DEFINED ENABLE_INNER_SHADOWS_HACK )

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

endif()

# X11
find_package( X11 REQUIRED )

# PKG_CONFIG
find_package(PkgConfig REQUIRED )

# GTK
pkg_check_modules( GTK REQUIRED gtk+-2.0 )

# Cairo
pkg_check_modules( CAIRO REQUIRED cairo )

# DBus
set( HAVE_DBUS 0 )
pkg_check_modules( DBUS dbus-glib-1 )
if( DBUS_FOUND )
    set( HAVE_DBUS 1 )
endif()

##########################
# installation directories
##########################
exec_program(
	${PKGCONFIG_EXECUTABLE}
	ARGS --variable=gtk_binary_version gtk+-2.0
	OUTPUT_VARIABLE GTK_BIN_VERSION )

set( INSTALL_PATH_GTK_ENGINES "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/gtk-2.0/${GTK_BIN_VERSION}/engines" CACHE PATH "The directory in which to install the theme engine library( eg. /usr/lib/gtk-2.0/2.4.0/engines )" )
set( INSTALL_PATH_GTK_THEMES "${CMAKE_INSTALL_PREFIX}/share/themes" CACHE PATH "The directory in which to install the theme data( eg. /usr/share/themes )" )
set( GTK_THEME_DIR ${INSTALL_PATH_GTK_THEMES}/oxygen-gtk )

# adjust flags based on gtk version
if( ENABLE_INNER_SHADOWS_HACK AND GTK_VERSION VERSION_LESS 2.24.2 )
    message("  Disabling ENABLE_INNER_SHADOWS_HACK because Gtk version is too old (needed 2.24.2, found ${GTK_VERSION}) " )
    set( ENABLE_INNER_SHADOWS_HACK 0 )
endif()

#########
# summary
#########
message( "" )
message( "Flags: " )
message( "  HAVE_DBUS ${HAVE_DBUS}" )
message( "  OXYGEN_DEBUG ${OXYGEN_DEBUG}" )
message( "  OXYGEN_ICON_HACK ${OXYGEN_ICON_HACK}" )
message( "  OXYGEN_FORCE_KDE_ICONS_AND_FONTS ${OXYGEN_FORCE_KDE_ICONS_AND_FONTS}" )
message( "  ENABLE_COMBOBOX_LIST_RESIZE ${ENABLE_COMBOBOX_LIST_RESIZE}" )
message( "  ENABLE_INNER_SHADOWS_HACK ${ENABLE_INNER_SHADOWS_HACK}" )
message( "" )
message( "Path: " )
message( "  Installing the GTK theme engine library to: ${INSTALL_PATH_GTK_ENGINES}" )
message( "  Installing the GTK theme engine data to: ${GTK_THEME_DIR}" )
message( "" )

if( NOT DBUS_FOUND )
    message( "Warning: " )
    message( "dbus-glib-1 <http://dbus.freedesktop.org/releases/dbus-glib/> could not be found on your system. dbus is a simple inter process communication system. It is used for Gtk applications to be updated 'on fly' when KDE configuration is changed." )
    message( "" )
endif( NOT DBUS_FOUND )

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
add_subdirectory( demo )
