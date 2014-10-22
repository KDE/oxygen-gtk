project( oxygen-gtk )

cmake_minimum_required(VERSION 2.4)

###########################
# Define package generation
###########################

set( CPACK_PACKAGE_NAME "oxygen-gtk2" )
set( CPACK_SOURCE_PACKAGE_NAME "oxygen-gtk2" )
set( CPACK_SOURCE_GENERATOR TBZ2 )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Oxygen Gtk2 theme" )
set( CPACK_PACKAGE_VENDOR "hugo.pereira@free.fr" )
set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "4" )
set( CPACK_PACKAGE_VERSION_PATCH "6" )
set( CPACK_SOURCE_IGNORE_FILES "build" "^${PROJECT_SOURCE_DIR}.*/.git/" )

##################################
# Define oxygen-gtk version string
##################################
if( EXISTS "${CMAKE_SOURCE_DIR}/.git" )
    execute_process(COMMAND sh -c "cd ${CMAKE_SOURCE_DIR} && git describe 2>/dev/null"
        OUTPUT_VARIABLE OXYGEN_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif( EXISTS "${CMAKE_SOURCE_DIR}/.git" )
STRING(COMPARE NOTEQUAL "${OXYGEN_VERSION}" "" OXYGEN_VERSION_DETECTED )
if( NOT OXYGEN_VERSION_DETECTED )
    set( OXYGEN_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH} )
endif( NOT OXYGEN_VERSION_DETECTED )
message( "Detected oxygen version string: ${OXYGEN_VERSION}" )

set( CPACK_SOURCE_PACKAGE_FILE_NAME "oxygen-gtk2-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
include( CPack )
########################
# Package specific flags
########################

# Set to 1 to enable debugging information
if( NOT DEFINED OXYGEN_DEBUG )
    set( OXYGEN_DEBUG 0 )
endif( NOT DEFINED OXYGEN_DEBUG )

# Set to 1 to disable all signal hooks used for various features
if( NOT DEFINED DISABLE_SIGNAL_HOOKS )
    set( DISABLE_SIGNAL_HOOKS 0 )
endif( NOT DEFINED DISABLE_SIGNAL_HOOKS )

# Set to 1 to enable debug logs regarding inner shadows
if( NOT DEFINED OXYGEN_DEBUG_INNERSHADOWS )
    set( OXYGEN_DEBUG_INNERSHADOWS 0 )
endif( NOT DEFINED OXYGEN_DEBUG_INNERSHADOWS )

# Set to 1 to build windeco test app (normal users rarely need this)
if( NOT DEFINED BUILD_DECO_TEST )
    set( BUILD_DECO_TEST 0 )
endif( NOT DEFINED BUILD_DECO_TEST )

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

# set to 0 to disable groupbox hack
if( NOT DEFINED ENABLE_GROUPBOX_HACK )
    set( ENABLE_GROUPBOX_HACK 1 )
endif( NOT DEFINED ENABLE_GROUPBOX_HACK )

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

    message( "Info: using PKGCONFIG_EXECUTABLE to set the installation prefix" )
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
add_definitions( ${GTK_CFLAGS_OTHER} )

# Cairo
pkg_check_modules( CAIRO REQUIRED cairo )
add_definitions( ${CAIRO_CFLAGS_OTHER} )

##########################
# installation directories
##########################
exec_program(
    ${PKGCONFIG_EXECUTABLE}
    ARGS --variable=gtk_binary_version gtk+-2.0
    OUTPUT_VARIABLE GTK_BIN_VERSION )

if( DEFINED LIB_SUFFIX OR NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

    message( "Info: using LIB_SUFFIX to set the theme engine installation path" )
    set( INSTALL_PATH_GTK_ENGINES "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/gtk-2.0/${GTK_BIN_VERSION}/engines" CACHE PATH "The directory in which to install the theme engine library( eg. /usr/lib/gtk-2.0/2.4.0/engines )" )

else()

    exec_program(
        ${PKGCONFIG_EXECUTABLE}
        ARGS --variable=libdir gtk+-2.0
        OUTPUT_VARIABLE GTK_LIBDIR_PREFIX )

    message( "Info: using PKGCONFIG_EXECUTABLE to set the theme engine installation path" )
    set( INSTALL_PATH_GTK_ENGINES "${GTK_LIBDIR_PREFIX}/gtk-2.0/${GTK_BIN_VERSION}/engines" CACHE PATH "The directory in which to install the theme engine library( eg. /usr/lib/gtk-2.0/2.4.0/engines )" )

endif()

set( INSTALL_PATH_GTK_THEMES "${CMAKE_INSTALL_PREFIX}/share/themes" CACHE PATH "The directory in which to install the theme data( eg. /usr/share/themes )" )
set( GTK_THEME_DIR ${INSTALL_PATH_GTK_THEMES}/oxygen-gtk )

# First emit all warnings in one place
# Adjust flags based on gtk version and warn user if needed
if( ENABLE_INNER_SHADOWS_HACK AND GTK_VERSION VERSION_LESS 2.24.2 )
    message( "Warning: disabling inner shadows hack because GTK version is too old (needed 2.24.2, found ${GTK_VERSION})" )
    set( ENABLE_INNER_SHADOWS_HACK 0 )
endif( ENABLE_INNER_SHADOWS_HACK AND GTK_VERSION VERSION_LESS 2.24.2 )

#########
# summary
#########
message( "Flags: " )
message( "  OXYGEN_DEBUG ${OXYGEN_DEBUG}" )
message( "  DISABLE_SIGNAL_HOOKS ${DISABLE_SIGNAL_HOOKS}" )
message( "  BUILD_DECO_TEST ${BUILD_DECO_TEST}" )
message( "  OXYGEN_DEBUG_INNERSHADOWS ${OXYGEN_DEBUG_INNERSHADOWS}" )
message( "  OXYGEN_ICON_HACK ${OXYGEN_ICON_HACK}" )
message( "  OXYGEN_FORCE_KDE_ICONS_AND_FONTS ${OXYGEN_FORCE_KDE_ICONS_AND_FONTS}" )
message( "  ENABLE_COMBOBOX_LIST_RESIZE ${ENABLE_COMBOBOX_LIST_RESIZE}" )
message( "  ENABLE_INNER_SHADOWS_HACK ${ENABLE_INNER_SHADOWS_HACK}" )
message( "  ENABLE_GROUPBOX_HACK ${ENABLE_GROUPBOX_HACK}" )
message( "" )
message( "Paths: " )
message( "  Installing the GTK theme engine library to: ${INSTALL_PATH_GTK_ENGINES}" )
message( "  Installing the GTK theme engine data to: ${GTK_THEME_DIR}" )
message( "" )

##################
# uninstall target
##################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


######################################################################
# Setup definitions and directories to be used for linking and include
######################################################################
link_directories( ${GTK_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS} )
include_directories( ${GTK_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${X11_X11_INCLUDE_PATH} )

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

