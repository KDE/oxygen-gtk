/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* inspired notably from kdelibs/kdeui/color/kcolorutils.h
* Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
* Copyright (C) 2007 Thomas Zander <zander@kde.org>
* Copyright (C) 2007 Zack Rusin <zack@kde.org>
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

#include "oxygenqtsettings.h"
#include "oxygencolorutils.h"
#include "oxygenfontinfo.h"
#include "oxygengtkicons.h"
#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#ifndef _WIN32
#include <pwd.h>
#include <unistd.h>
#endif

#include <gtk/gtk.h>

namespace Oxygen
{

    //_________________________________________________
    const std::string QtSettings::_defaultKdeIconPrefix = "/usr/share/icons/";

    //_________________________________________________________
    QtSettings::QtSettings( void ):
        _checkBoxStyle( CS_CHECK ),
        _kdeIconTheme( "oxygen" ),
        _kdeFallbackIconTheme( "hicolor" )
    {}

    //_________________________________________________________
    void QtSettings::init( void )
    {

        // clear RC
        _rc.clear();

        // init application name
        initApplicationName();

        // reset path
        _kdeHome = kdeHome();
        _kdeIconPrefix = kdeIconPrefix();

        // reload icons
        _kdeIconPath = kdeIconPath();
        loadKdeIcons();

        // reload kdeGlobals
        _kdeGlobals.clear();
        _kdeGlobals.merge( readOptions( std::string( GTK_THEME_DIR ) + "/kdeglobals" ) );
        _kdeGlobals.merge( readOptions( sanitizePath( _kdeHome + "/share/config/kdeglobals" ) ) );

        // reload oxygen options
        std::string oxygen = sanitizePath( _kdeHome + "/share/config/oxygenrc" );
        _oxygen = readOptions( oxygen );

        // reload palette
        _palette.clear();
        loadKdePalette();
        generateGtkColors();

        // reload fonts
        loadKdeFonts();

        // contrast
        ColorUtils::setContrast( _kdeGlobals.getOption( "[KDE]", "contrast" ).toVariant<double>( 5 ) / 10 );

        // checkbox style
        _checkBoxStyle = (_oxygen.getValue( "[Style]", "CheckBoxStyle", "CS_CHECK" ) == "CS_CHECK") ? CS_CHECK:CS_X;

        // scrollbar width
        _rc.addSection( "oxygen-scrollbar", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<int>(
            "  GtkScrollbar::slider-width",
            _oxygen.getOption( "[Style]", "ScrollBarWidth" ).toVariant<int>(15) - 1 ) );
        _rc.addToRootSection( "class \"GtkScrollbar\" style \"oxygen-scrollbar\"" );

        // pass all resources to gtk
        gtk_rc_parse_string( _rc.toString().c_str() );

        // std::cout << _rc << std::endl;

    }

    //_________________________________________________________
    std::vector<std::string> QtSettings::kdeIconPath( void ) const
    {
        std::vector<std::string> out;
        std::vector<std::string> iconThemes;
        iconThemes.push_back( _kdeIconTheme );
        iconThemes.push_back( _kdeFallbackIconTheme );
        for( std::vector<std::string>::const_iterator iter = iconThemes.begin(); iter != iconThemes.end(); iter++ )
        {

            out.push_back( sanitizePath( _kdeHome + "/share/config/icons/" + *iter ) );
            out.push_back( sanitizePath( _kdeIconPrefix + *iter ) );
        }

        return out;

    }

    //_________________________________________________________
    std::string QtSettings::home( void ) const
    {

        const char *home=0L;

        #ifdef _WIN32
        home = getenv("HOMEPATH");
        #else

        struct passwd *p=getpwuid(getuid());
        if( p ) home=p->pw_dir;
        else {

            char *env=getenv("HOME");
            if(env) home=env;
        }

        if(!home) home="/tmp";
        #endif

        return std::string( home );
    }

    //_________________________________________________________
    std::string QtSettings::kdeHome( void ) const
    {

        char* kdeHome;

        // try run using kde4-config command
        if( g_spawn_command_line_sync( "kde4-config --expandvars --localprefix", &kdeHome, 0L, 0L, 0L ) )
        {
            int len=strlen(kdeHome);
            if(len>1 && kdeHome[len-1]=='\n')
            { kdeHome[len-1]='\0'; }

        } else kdeHome=0L;

        if( kdeHome ) {

            return std::string( kdeHome );

        } else if( char *env=getenv( getuid() ? "KDEHOME" : "KDEROOTHOME") ) {

            return std::string( env );

        } else {

            std::ostringstream ostr;
            ostr << home() << "/" << ".kde4";
            return ostr.str();

        }

    }

    //_________________________________________________________
    std::string QtSettings::kdeIconPrefix( void ) const
    {
        char* kdeIconPrefix;
        if( g_spawn_command_line_sync( "kde4-config --expandvars --install icon", &kdeIconPrefix, 0L, 0L, 0L ) )
        {

            int len=strlen(kdeIconPrefix);
            if(len>1 && kdeIconPrefix[len-1]=='\n') kdeIconPrefix[len-1]='\0';
            return kdeIconPrefix;

        } else return _defaultKdeIconPrefix;

    }

    //_________________________________________________________
    void QtSettings::initApplicationName( void )
    {
        const char* applicationName = g_get_prgname();
        if( !applicationName ) return;

        std::string appName( applicationName );

        if( appName == "firefox-bin" ) _applicationName = Firefox;
        else if( appName == "thunderbird-bin" ) _applicationName = Thunderbird;
        else if( appName == "gimp" ) _applicationName = Gimp;
        else appName = Unknown;

    }

    //_________________________________________________________
    void QtSettings::loadKdeIcons( void )
    {

        // load translation table
        GtkIcons icons;
        icons.loadTranslations( std::string( GTK_THEME_DIR ) + "/icons4" );
        icons.generate( _rc, _kdeIconPath );

        // set theme names into gtk
        std::ostringstream themeNameStr;
        themeNameStr << "gtk-icon-theme-name=\"" << _kdeIconTheme << "\"" << std::endl;
        themeNameStr << "gtk-fallback-icon-theme=\"" << _kdeFallbackIconTheme << "\"";
        _rc.addToHeaderSection( themeNameStr.str() );

    }

    //_________________________________________________________
    void QtSettings::loadKdePalette( void )
    {
        _palette.setColor( Palette::Active, Palette::Window, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Window]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::WindowText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Window]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Button, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::ButtonText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Focus, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "DecorationFocus" ) ) );
        _palette.setColor( Palette::Active, Palette::Hover, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "DecorationHover" ) ) );

        _palette.setColor( Palette::Active, Palette::Selected, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Selection]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::SelectedText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Selection]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Tooltip, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Tooltip]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::TooltipText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Tooltip]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Base, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::Text, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "ForegroundNormal" ) ) );

        // copy to inactive and disabled palette
        _palette.copy( Palette::Active, Palette::Inactive );
        _palette.copy( Palette::Active, Palette::Disabled );

        // fill (overwrite) additional colors for disabled palette
        _palette.setColor( Palette::Disabled, Palette::WindowText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Window]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::ButtonText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::SelectedText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Selection]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::TooltipText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Tooltip]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::Text, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "ForegroundInactive" ) ) );

    }

    //_________________________________________________________
    void QtSettings::loadKdeFonts( void )
    {

        // try load default font
        FontInfo::Map fonts;
        FontInfo defaultFont;
        if( _kdeGlobals.hasOption( "[General]", "font" ) )
        {

            defaultFont = FontInfo::fromKdeOption( _kdeGlobals.getValue( "[General]", "font", "" ) );

        } else {

            std::cout << "Oxygen::QtSettings::loadKdeFonts - cannot load default font" << std::endl;

        }

        fonts[FontInfo::Default] = defaultFont;

        // load extra fonts
        typedef std::map<FontInfo::FontType, std::string> FontNameMap;
        FontNameMap optionNames;
        optionNames.insert( std::make_pair( FontInfo::Desktop, "desktopFont" ) );
        optionNames.insert( std::make_pair( FontInfo::Fixed, "fixed" ) );
        optionNames.insert( std::make_pair( FontInfo::Menu, "menuFont" ) );
        optionNames.insert( std::make_pair( FontInfo::ToolBar, "toolBarFont" ) );
        for( FontNameMap::const_iterator iter = optionNames.begin(); iter != optionNames.end(); iter++ )
        {
            FontInfo local;
            if( _kdeGlobals.hasOption( "[General]", iter->second ) )
            {

                local = FontInfo::fromKdeOption( _kdeGlobals.getValue( "[General]", iter->second, "" ) );

            } else {

                std::cout << "Oxygen::QtSettings::loadKdeFonts - cannot load font named " << iter->second << std::endl;
                local = defaultFont;

            }

            // store in map
            fonts[iter->first] = local;

        }

        // pass fonts to RC
        _rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-font-name", fonts[FontInfo::Default] ) );

        _rc.addSection( "oxygen-default-font", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  font_name", fonts[FontInfo::Default] ) );
        _rc.addToRootSection( "widget_class \"*\" style \"oxygen-default-font\"" );

        _rc.addSection( "oxygen-menu-font", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  font_name", fonts[FontInfo::Menu] ) );
        _rc.addToRootSection( "widget_class \"*.*MenuItem.*\" style \"oxygen-menu-font\"" );

        _rc.addSection( "oxygen-toolbar-font", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  font_name", fonts[FontInfo::ToolBar] ) );
        _rc.addToRootSection( "widget_class \"*.*Toolbar.*\" style \"oxygen-toolbar-font\"" );

    }

    //_________________________________________________________
    void QtSettings::generateGtkColors( void )
    {


        // customize gtk palette
        _palette.setGroup( Palette::Active );
        _rc.addSection( "oxygen-menu-item", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::WindowText ) ) );
        _rc.addToRootSection( "widget_class \"*Menu*\" style \"oxygen-menu-item\"" );

        _rc.addSection( "oxygen-spinbutton", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Base ) ) );
        _rc.addToRootSection( "class \"GtkSpinButton\" style \"oxygen-spinbutton\"" );

        // tooltips
        _rc.addSection( "oxygen-tooltips", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Tooltip ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::TooltipText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<int>( "  xthickness", 3 ) );
        _rc.addToCurrentSection( Gtk::RCOption<int>( "  ythickness", 3 ) );

        // this is a mess.
        // see if this can be cleaned up
        _rc.addToRootSection( "class \"GtkTooltip\" style \"oxygen-tooltips\"" );
        _rc.addToRootSection( "class \"GtkTooltips\" style \"oxygen-tooltips\"" );

        _rc.addToRootSection( "widget \"gtk-tooltips\" style \"oxygen-tooltips\"" );
        _rc.addToRootSection( "widget \"gtk-tooltip\" style \"oxygen-tooltips\"" );
        _rc.addToRootSection( "widget \"gtk-tooltip.*\" style \"oxygen-tooltips\"" );
        _rc.addToRootSection( "widget \"gtk-tooltips\" style \"oxygen-tooltips\"" );
        _rc.addToRootSection( "widget \"gtk-tooltips.*\" style \"oxygen-tooltips\"" );
        _rc.addToRootSection( "widget_class \"*.<GtkTooltip>.*\" style \"oxygen-tooltips\"" );

    }

    //_________________________________________________________
    std::string QtSettings::sanitizePath( const std::string& path ) const
    {

        std::string out( path );
        size_t position( std::string::npos );
        while( ( position = out.find( "//" ) ) != std::string::npos )
        { out.replace( position, 2, "/" ); }

        return out;
    }

    //_________________________________________________________
    OptionMap QtSettings::readOptions( const std::string& filename ) const
    {

        OptionMap out;

        std::ifstream in( filename.c_str() );
        if( !in ) return out;

        std::string currentSection;
        char line[512];

        while( in.getline( line, 512, '\n' ) )
        {

            std::string currentLine( line );
            if( currentLine.empty() ) continue;

            // check if line is a section
            if( currentLine[0] == '[' )
            {

                size_t end( currentLine.rfind( ']' ) );
                if( end == std::string::npos ) continue;
                currentSection = currentLine.substr( 0, end+1 );

            } else if( currentSection.empty() ) {

                continue;

            }

            // check if line is a valid option
            size_t mid( currentLine.find( '=' ) );
            if( mid == std::string::npos ) continue;

            // insert new option in map
            out[currentSection].insert( Option( currentLine.substr( 0, mid ), currentLine.substr( mid+1 ) ) );

        }

        return out;

    }

}
