/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

#include "config.h"
#include "oxygenstylewrapper.h"
#include "oxygenrcstyle.h"
#include "oxygenstyle.h"
#include "oxygenwindecooptions.h"

#include <gmodule.h>
#include <gtk/gtk.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>

//_________________________________________________
extern "C" G_MODULE_EXPORT void theme_init( GTypeModule* );
extern "C" G_MODULE_EXPORT void theme_exit( void );
extern "C" G_MODULE_EXPORT GtkRcStyle* theme_create_rc_style( void );
extern "C" G_MODULE_EXPORT const gchar* g_module_check_init( GModule* );

// exports for WM theming
extern "C" G_MODULE_EXPORT void drawWindowDecoration(cairo_t*, unsigned long,gint,gint,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindecoButton(cairo_t*, unsigned long,unsigned long, unsigned long,gint,gint,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindecoShapeMask(cairo_t*, unsigned long,gint,gint,gint,gint);

//_________________________________________________
void enableARGB()
{

    // get program name
    const char* appName = g_get_prgname();
    if( !appName ) return;

    // user configuration directory
    const std::string userConfigDir( std::string( g_get_user_config_dir() ) + "/oxygen-gtk" );

    // create config directory and config file if they don't exist
    {

        // try create directory
        struct stat st;
        if( stat( userConfigDir.c_str(), &st ) != 0 )
        { mkdir( userConfigDir.c_str(), S_IRWXU|S_IRWXG|S_IRWXO ); }

        // note: in some cases, the target might exist and not be a directory
        // nothing we can do about it. We won't overwrite the file to prevent dataloss

    }

    // user-defined configuration file
    const std::string userConfig( userConfigDir + "/argb-apps.conf");

    // make sure user configuration file exists
    std::ofstream newConfig( userConfig.c_str(), std::ios::app );
    if( newConfig )
    {
        // if the file is empty (newly created), put a hint there
        if( !newConfig.tellp() )
        { newConfig << "# argb-apps.conf\n# Put your user-specific ARGB app settings here\n\n"; }
        newConfig.close();

    }

    // check for ARGB hack being disabled
    if(g_getenv("OXYGEN_DISABLE_ARGB_HACK"))
    {
        std::cerr << "Oxygen: ARGB hack is disabled; program name: " << appName << std::endl;
        std::cerr << "Oxygen: if disabling ARGB hack helps, please add this string:\n\ndisable:" << appName << "\n\nto ~/.config/oxygen-gtk/argb-apps.conf\nand report it here: https://bugs.kde.org/show_bug.cgi?id=260640" << std::endl;
        return;
    }

    // get debug flag from environement
    const bool OXYGEN_ARGB_DEBUG = g_getenv("OXYGEN_ARGB_DEBUG");

    // read blacklist
    // system-wide configuration file
    const std::string configFile( std::string( GTK_THEME_DIR ) + "/argb-apps.conf" );
    std::ifstream systemIn( configFile.c_str() );
    if( !systemIn )
    {

        if( G_UNLIKELY(OXYGEN_DEBUG||OXYGEN_ARGB_DEBUG) )
        { std::cerr << "ARGB: Oxygen::theme_init - ARGB config file \"" << configFile << "\" not found" << std::endl; }

    }

    // load options into a string
    std::string contents;
    std::vector<std::string> lines;
    while( std::getline( systemIn, contents, '\n' ) )
    {
        if( contents.empty() || contents[0] == '#' ) continue;
        lines.push_back( contents );
    }

    // user specific blacklist
    std::ifstream userIn( userConfig.c_str() );
    if( !userIn )
    {

        if( G_UNLIKELY(OXYGEN_DEBUG||OXYGEN_ARGB_DEBUG) )
        { std::cerr << "ARGB: Oxygen::theme_init - user-defined ARGB config file \"" << userConfig << "\" not found - only system-wide one will be used" << std::endl; }

    }

    // load options into a string
    while( std::getline( userIn, contents, '\n' ) )
    {
        if( contents.empty() || contents[0] == '#' ) continue;
        lines.push_back( contents );
    }

    // by default argb support is enabled
    bool useRgba( true );

    // true if application was found in one of the lines
    bool found( false );
    for( std::vector<std::string>::const_reverse_iterator iter = lines.rbegin(); iter != lines.rend() && !found; ++iter )
    {

        // store line locally
        std::string contents( *iter );

        // split string using ":" as a delimiter
        std::vector<std::string> appNames;
        size_t position( std::string::npos );
        while( ( position = contents.find( ':' ) ) != std::string::npos )
        {
            std::string appName( contents.substr(0, position ) );
            if( !appName.empty() ) { appNames.push_back( appName ); }
            contents = contents.substr( position+1 );
        }

        if( !contents.empty() ) appNames.push_back( contents );
        if( appNames.empty() ) continue;

        // check line type
        bool enabled( true );
        if( appNames[0] == "enable" ) enabled = true;
        else if( appNames[0] == "disable" ) enabled = false;
        else continue;

        // compare application names to this application
        for( unsigned int i = 1; i < appNames.size(); i++ )
        {
            if( appNames[i] == "all" || appNames[i] == appName )
            {
                found = true;
                useRgba = enabled;
                break;
            }
        }

    }

    if( G_UNLIKELY(OXYGEN_DEBUG||OXYGEN_ARGB_DEBUG) )
    { std::cerr << "ARGB: Oxygen::init_theme - program: " << appName << " ARGB visual is " << (useRgba ? "":"not ") << "used" << std::endl; }

    // change default colormap when argb is enabled
    if( useRgba )
    {
        GdkScreen* screen = gdk_screen_get_default();
        if( screen )
        {
            GdkColormap* cmap=gdk_screen_get_rgba_colormap( screen );
            gtk_widget_push_colormap( cmap );
            gtk_widget_set_default_colormap( cmap );
        }
    }
}

//_________________________________________________
void theme_init( GTypeModule* module )
{
    oxygen_rc_style_register_type( module );
    oxygen_style_register_type( module );

    enableARGB();

    if( Oxygen::Style::instance().settings().applicationName().isOpenOffice() )
    { Oxygen::Style::instance().animations().setEnabled( false ); }
}

//_________________________________________________
void theme_exit( void )
{}

//_________________________________________________
GtkRcStyle* theme_create_rc_style( void )
{ return GTK_RC_STYLE( g_object_new( OXYGEN_TYPE_RC_STYLE, NULL ) ); }

//_________________________________________________
const gchar* g_module_check_init( GModule *module )
{
    return gtk_check_version(
        GTK_MAJOR_VERSION,
        GTK_MINOR_VERSION,
        GTK_MICRO_VERSION - GTK_INTERFACE_AGE );
}

//_________________________________________________
void drawWindowDecoration(cairo_t* context, unsigned long options, gint x,gint y,gint w,gint h)
{
    Oxygen::Style::instance().drawWindowDecoration( context, (Oxygen::WinDeco::Options) options, x, y, w, h);
}

//_________________________________________________
void drawWindecoButton(cairo_t* context, unsigned long buttonType,unsigned long buttonState, unsigned long windowState, gint x,gint y,gint w,gint h)
{
    Oxygen::Style::instance().drawWindecoButton(
        context, (Oxygen::WinDeco::ButtonType)buttonType,
        (Oxygen::WinDeco::ButtonStatus)buttonState, (Oxygen::WinDeco::Options) windowState, x, y, w, h);
}

//_________________________________________________
void drawWindecoShapeMask(cairo_t* context, unsigned long options, gint x,gint y,gint w,gint h)
{
    Oxygen::Style::instance().drawWindecoShapeMask( context, (Oxygen::WinDeco::Options) options, x, y, w, h);
}
