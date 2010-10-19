/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
#include "oxygencoloreffect.h"
#include "oxygencolorutils.h"
#include "oxygenfontinfo.h"
#include "oxygengtkicons.h"
#include "config.h"

#include <gtk/gtk.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Oxygen
{

    //_________________________________________________________
    const std::string QtSettings::_defaultKdeIconPath = "/usr/share/icons/";

    //_________________________________________________________
    QtSettings::QtSettings( void ):
        _kdeIconTheme( "oxygen" ),
        _kdeFallbackIconTheme( "oxy-gnome" ),
        _inactiveChangeSelectionColor( false ),
        _checkBoxStyle( CS_CHECK ),
        _tabStyle( TS_SINGLE ),
        _scrollBarColored( false ),
        _scrollBarBevel( false ),
        _scrollBarAddLineButtons( 2 ),
        _scrollBarSubLineButtons( 1 ),
        _toolBarDrawItemSeparator( true ),
        _viewDrawFocusIndicator( true ),
        _tooltipTransparent( true ),
        _tooltipDrawStyledFrames( true ),
        _viewDrawTriangularExpander( true ),
        _viewTriangularExpanderSize( ArrowSmall ),
        _menuHighlightMode( MM_DARK )
    {}

    //_________________________________________________________
    void QtSettings::initialize( void )
    {

        // clear RC
        _rc.clear();

        // init application name
        initApplicationName();

        _kdeConfigPathList = kdeConfigPathList();

        // reload kdeGlobals and oxygen
        _kdeGlobals.clear();
        _oxygen.clear();
        for( PathList::const_reverse_iterator iter = _kdeConfigPathList.rbegin(); iter != _kdeConfigPathList.rend(); ++iter )
        {
            #if OXYGEN_DEBUG
            std::cout << "QtSettings::initialize - reading config from: " << *iter << std::endl;
            #endif

            _kdeGlobals.merge( readOptions( sanitizePath( *iter + "/kdeglobals" ) ) );
            _oxygen.merge( readOptions( sanitizePath( *iter + "/oxygenrc" ) ) );

        }

        #if OXYGEN_DEBUG
        std::cout << "QtSettings::initialize - Kdeglobals: " << std::endl;
        std::cout << _kdeGlobals << std::endl;

        std::cout << "QtSettings::initialize - Oxygenrc: " << std::endl;
        std::cout << _oxygen << std::endl;
        #endif

        // reload palette
        loadKdePalette();

        // reload icons
        _kdeIconPathList = kdeIconPathList();
        loadKdeIcons();

        // reload fonts
        loadKdeFonts();

        // kde globals options
        loadKdeGlobalsOptions();

        // oxygen options
        loadOxygenOptions();

        // gtk colors
        generateGtkColors();

        // pass all resources to gtk
        gtk_rc_parse_string( _rc.toString().c_str() );

        #if OXYGEN_DEBUG
        std::cout << _rc << std::endl;
        #endif

    }

    //_________________________________________________________
    void QtSettings::PathList::split( const std::string& path, const std::string& separator )
    {

        clear();
        std::string local( path );
        if( local.empty() ) return;
        if( local[local.size()-1] == '\n' ) local = local.substr( 0, local.size()-1 );

        size_t position( std::string::npos );
        while( ( position = local.find( separator ) ) != std::string::npos )
        {
            push_back( local.substr(0, position ) );
            local = local.substr( position + separator.length() );
        }

        if( !local.empty() ) push_back( local );
        return;

    }

    //_________________________________________________________
    std::string QtSettings::PathList::join( const std::string& separator ) const
    {
        std::ostringstream out;
        for( const_iterator iter = begin(); iter != end(); iter++ )
        {
            if( iter != begin() ) out << separator;
            out << *iter;
        }

        return out.str();
    }

    //_________________________________________________________
    QtSettings::PathList QtSettings::kdeConfigPathList( void ) const
    {

        PathList out;

        // load icon install prefix
        char* path = 0L;
        if( g_spawn_command_line_sync( "kde4-config --path config", &path, 0L, 0L, 0L ) && path )
        { out.split( path ); };

        out.push_back( GTK_THEME_DIR );
        return out;

    }

    //_________________________________________________________
    QtSettings::PathList QtSettings::kdeIconPathList( void ) const
    {

        // load icon install prefix
        PathList out;
        char* path = 0L;
        if( g_spawn_command_line_sync( "kde4-config --path icon", &path, 0L, 0L, 0L ) && path )
        { out.split( path ); }

        out.push_back( _defaultKdeIconPath );
        return out;

    }

    //_________________________________________________________
    void QtSettings::initApplicationName( void )
    {
        const char* applicationName = g_get_prgname();
        if( !applicationName ) return;

        std::string appName( applicationName );

        if( appName == "firefox-bin" || appName == "firefox" ) _applicationName = Firefox;
        else if( appName == "thunderbird-bin" || appName == "thunderbird" ) _applicationName = Thunderbird;
        else if( appName == "gimp" ) _applicationName = Gimp;
        else appName = Unknown;

    }

    //_________________________________________________________
    void QtSettings::addIconTheme( PathList& pathList, const std::string& theme )
    {

        // do nothing if theme have already been included in the loop
        if( _iconThemes.find( theme ) != _iconThemes.end() ) return;
        _iconThemes.insert( theme );

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::QtSettings::addIconTheme - adding " << theme << std::endl;
        #endif

        // add all possible path (based on _kdeIconPathList) and look for possible parent
        std::string parentTheme;
        for( PathList::const_iterator iter = _kdeIconPathList.begin(); iter != _kdeIconPathList.end(); ++iter )
        {

            pathList.push_back( sanitizePath( *iter + '/' + theme ) );
            if( parentTheme.empty() )
            {
                const std::string index( sanitizePath( *iter + '/' + theme + "/index.theme" ) );
                OptionMap themeOptions( readOptions( index ) );
                parentTheme = themeOptions.getValue( "[Icon Theme]", "Inherits" );
            }

        }

        // add parent if needed
        if( !parentTheme.empty() )
        { addIconTheme( pathList, parentTheme ); }

        return;

    }

    //_________________________________________________________
    void QtSettings::loadKdeIcons( void )
    {

        // load icon theme and path to gtk
        _iconThemes.clear();
        _kdeIconTheme = _kdeGlobals.getOption( "[Icons]", "Theme" ).toVariant<std::string>("oxygen");

        std::ostringstream themeNameStr;
        themeNameStr << "gtk-icon-theme-name=\"" << _kdeIconTheme << "\"" << std::endl;
        themeNameStr << "gtk-fallback-icon-theme=\"" << _kdeFallbackIconTheme << "\"";
        _rc.addToHeaderSection( themeNameStr.str() );

        // create icon translator, for stock icons
        GtkIcons icons;

        // load icon sizes from kde
        const int desktopIconSize( _kdeGlobals.getOption( "[DesktopIcons]", "Size" ).toInt( 48 ) );
        const int dialogIconSize( _kdeGlobals.getOption( "[DialogIcons]", "Size" ).toInt( 32 ) );
        const int panelIconSize( _kdeGlobals.getOption( "[PanelIcons]", "Size" ).toInt( 32 ) );
        const int mainToolbarIconSize( _kdeGlobals.getOption( "[MainToolbarIcons]", "Size" ).toInt( 22 ) );
        const int smallIconSize( _kdeGlobals.getOption( "[SmallIcons]", "Size" ).toInt( 16 ) );
        const int toolbarIconSize( _kdeGlobals.getOption( "[ToolbarIcons]", "Size" ).toInt( 22 ) );

        // set gtk icon sizes
        icons.setIconSize( "panel-menu", smallIconSize );
        icons.setIconSize( "panel", panelIconSize );
        icons.setIconSize( "gtk-small-toolbar", toolbarIconSize );
        icons.setIconSize( "gtk-large-toolbar", mainToolbarIconSize );
        icons.setIconSize( "gtk-dnd", desktopIconSize );
        icons.setIconSize( "gtk-button", smallIconSize );
        icons.setIconSize( "gtk-menu", smallIconSize );
        icons.setIconSize( "gtk-dialog", dialogIconSize );
        icons.setIconSize( "", smallIconSize );

        // load translation table, generate full translation list, and path to gtk
        icons.loadTranslations( sanitizePath( std::string( GTK_THEME_DIR ) + "/icons4" ) );

        // generate full path list
        PathList iconThemeList;
        addIconTheme( iconThemeList, _kdeIconTheme );
        addIconTheme( iconThemeList, _kdeFallbackIconTheme );

        icons.generate( _rc, iconThemeList );

    }

    //_________________________________________________________
    void QtSettings::loadKdePalette( void )
    {
        _palette.clear();
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
        _palette.setColor( Palette::Active, Palette::BaseAlternate, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "BackgroundAlternate" ) ) );
        _palette.setColor( Palette::Active, Palette::Text, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "ForegroundNormal" ) ) );

        // generate inactive and disabled palette from active, applying effects from kdeglobals
        _inactiveChangeSelectionColor = ( _kdeGlobals.getOption( "[ColorEffects:Inactive]", "ChangeSelectionColor" ).toVariant<std::string>( "false" ) == "true" );
        _palette.generate( Palette::Active, Palette::Inactive, ColorUtils::Effect( Palette::Inactive, _kdeGlobals ), _inactiveChangeSelectionColor );
        _palette.generate( Palette::Active, Palette::Disabled, ColorUtils::Effect( Palette::Disabled, _kdeGlobals ) );

        // fill (overwrite) additional colors for disabled palette
        _palette.setColor( Palette::Disabled, Palette::WindowText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Window]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::ButtonText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::SelectedText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Selection]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::TooltipText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Tooltip]", "ForegroundInactive" ) ) );
        _palette.setColor( Palette::Disabled, Palette::Text, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "ForegroundInactive" ) ) );

    }

    //_________________________________________________________
    void QtSettings::generateGtkColors( void )
    {

        // customize gtk palette
        _palette.setGroup( Palette::Active );

        // default colors
        _rc.addSection( "oxygen-colors", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Window ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[PRELIGHT]", _palette.color( Palette::Window ) ) );

        if( applicationName().isMozilla() )
        {

            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[ACTIVE]", _palette.color( Palette::Window ) ) );

        } else {

            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[ACTIVE]", _palette.color( Palette::Inactive, Palette::Selected ) ) );

        }

        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[SELECTED]", _palette.color( Palette::Selected ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[INSENSITIVE]", _palette.color( Palette::Window ) ) );

        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::WindowText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::WindowText ) ) );

        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[NORMAL]", _palette.color( Palette::Base ) ) );

        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[ACTIVE]", _palette.color( Palette::Inactive, Palette::Selected ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[PRELIGHT]", _palette.color( Palette::Base ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[SELECTED]", _palette.color( Palette::Selected ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Base ) ) );

        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[NORMAL]", _palette.color( Palette::Text ) ) );

        if( _inactiveChangeSelectionColor ) _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::Text ) ) );
        else _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::SelectedText ) ) );

        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::Text ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[SELECTED]", _palette.color( Palette::SelectedText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Text ) ) );

        _rc.addToRootSection( "widget_class \"*\" style \"oxygen-colors\"" );

        // buttons
        _rc.addSection( "oxygen-buttons", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Button ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::ButtonText ) ) );
        _rc.addToRootSection( "widget_class \"*<GtkButton>*\" style \"oxygen-buttons\"" );

        // menu items
        _rc.addSection( "oxygen-menu-item", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[NORMAL]", _palette.color( Palette::Text ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::WindowText ) ) );

        if( _menuHighlightMode == MM_STRONG )
        {

            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::SelectedText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[SELECTED]", _palette.color( Palette::SelectedText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::SelectedText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::SelectedText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::SelectedText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::SelectedText ) ) );

        } else {

            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::Text ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[SELECTED]", _palette.color( Palette::Text ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::Text ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::WindowText ) ) );
            _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::WindowText ) ) );

        }

        _rc.addToRootSection( "widget_class \"*<GtkMenuItem>*\" style \"oxygen-menu-item\"" );

        // text entries
        /*
        to match oxygen: the Window background is used for disabled entries,
        whereas the 'base' background is used when enabled
        */
        _rc.addSection( "oxygen-entry", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Base ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Window ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Window ) ) );
        _rc.addToRootSection( "widget_class \"*<GtkSpinButton>*\" style \"oxygen-entry\"" );
        _rc.addToRootSection( "widget_class \"*<GtkEntry>*\" style \"oxygen-entry\"" );
        _rc.addToRootSection( "widget_class \"*<GtkEntry>*\" style \"oxygen-entry\"" );
        _rc.addToRootSection( "widget_class \"*<GtkComboBoxEntry>*.*<GtkButton>*\" style \"oxygen-entry\"" );

        // tooltips
        _rc.addSection( "oxygen-tooltips", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Tooltip ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::TooltipText ) ) );
        _rc.addToCurrentSection( Gtk::RCOption<int>( "  xthickness", 3 ) );
        _rc.addToCurrentSection( Gtk::RCOption<int>( "  ythickness", 3 ) );
        _rc.addToRootSection( "widget \"gtk-tooltip*\" style \"oxygen-tooltips\"" );

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
        for( FontNameMap::const_iterator iter = optionNames.begin(); iter != optionNames.end(); ++iter )
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
    void QtSettings::loadKdeGlobalsOptions( void )
    {

        // toolbar style
        std::string toolbarTextPosition( _kdeGlobals.getOption( "[Toolbar style]", "ToolButtonStyle" ).toVariant<std::string>( "TextBelowIcon" ) );
        if( toolbarTextPosition == "TextOnly" ) _rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_TEXT" ) );
        else if( toolbarTextPosition == "TextBesideIcon" ) _rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_BOTH_HORIZ" ) );
        else if( toolbarTextPosition == "NoText" ) _rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_ICONS" ) );
        else _rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_BOTH" ) );

    }

    //_________________________________________________________
    void QtSettings::loadOxygenOptions( void )
    {

        // contrast
        ColorUtils::setContrast( _kdeGlobals.getOption( "[KDE]", "contrast" ).toVariant<double>( 7 ) / 10 );

        // checkbox style
        _checkBoxStyle = (_oxygen.getValue( "[Style]", "CheckBoxStyle", "CS_CHECK" ) == "CS_CHECK") ? CS_CHECK:CS_X;

        // checkbox style
        _tabStyle = (_oxygen.getValue( "[Style]", "TabStyle", "TS_SINGLE" ) == "TS_SINGLE") ? TS_SINGLE:TS_PLAIN;

        // colored scrollbars
        _scrollBarColored = _oxygen.getOption( "[Style]", "ScrollBarColored" ).toVariant<std::string>("false") == "true";

        // colored scrollbars
        _scrollBarBevel = _oxygen.getOption( "[Style]", "ScrollBarBevel" ).toVariant<std::string>("false") == "true";

        // scrollbar buttons
        _scrollBarAddLineButtons = _oxygen.getOption( "[Style]", "ScrollBarAddLineButtons" ).toVariant<int>( 2 );
        _scrollBarSubLineButtons = _oxygen.getOption( "[Style]", "ScrollBarSubLineButtons" ).toVariant<int>( 1 );

        // toolbar separators
        _toolBarDrawItemSeparator = _oxygen.getOption( "[Style]", "ToolBarDrawItemSeparator" ).toVariant<std::string>("true") == "true";

        // focus indicator in views
        _viewDrawFocusIndicator = _oxygen.getOption( "[Style]", "viewDrawFocusIndicator" ).toVariant<std::string>("true") == "true";

        // tooltips
        _tooltipTransparent = _oxygen.getOption( "[Style]", "ToolTipTransparent" ).toVariant<std::string>("true") == "true";
        _tooltipDrawStyledFrames = _oxygen.getOption( "[Style]", "ToolTipDrawStyledFrames" ).toVariant<std::string>("true") == "true";

        // triangular expanders
        _viewDrawTriangularExpander = _oxygen.getOption( "[Style]", "ViewDrawTriangularExpander" ).toVariant<std::string>("true") == "true";

        // triangular expander (arrow) size
        std::string expanderSize( _oxygen.getOption( "[Style]", "ViewTriangularExpanderSize" ).toVariant<std::string>("TE_SMALL") );
        if( expanderSize == "TE_NORMAL" ) _viewTriangularExpanderSize = ArrowNormal;
        else if( expanderSize == "TE_TINY" ) _viewTriangularExpanderSize = ArrowTiny;
        else _viewTriangularExpanderSize = ArrowSmall;

        // menu highlight mode
        std::string highlightMode( _oxygen.getOption( "[Style]", "MenuHighlightMode" ).toVariant<std::string>("MM_DARK") );
        if( highlightMode == "MM_SUBTLE" ) _menuHighlightMode = MM_SUBTLE;
        else if( highlightMode == "MM_STRONG" ) _menuHighlightMode = MM_STRONG;
        else _menuHighlightMode = MM_DARK;

        // copy relevant options to to gtk
        // scrollbar width
        _rc.addSection( "oxygen-scrollbar", "oxygen-default" );
        _rc.addToCurrentSection( Gtk::RCOption<int>(
            "  GtkScrollbar::slider-width",
            _oxygen.getOption( "[Style]", "ScrollBarWidth" ).toVariant<int>(15) - 1 ) );
        _rc.addToRootSection( "class \"GtkScrollbar\" style \"oxygen-scrollbar\"" );

        _rc.setCurrentSection( "oxygen-scrollbar" );
        _rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-backward-stepper", _scrollBarSubLineButtons > 0 ) );
        _rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-forward-stepper", _scrollBarAddLineButtons > 0 ) );

        // note the inversion for add and sub, due to the fact that kde options refer to the button location, and not its direction
        _rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-secondary-backward-stepper", _scrollBarAddLineButtons > 1 ) );
        _rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-secondary-forward-stepper", _scrollBarSubLineButtons > 1 ) );

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
        std::string currentLine;
        while( std::getline( in, currentLine, '\n' ) )
        {

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
            Option option( currentLine.substr( 0, mid ), currentLine.substr( mid+1 ) );
            option.setFile( filename );
            out[currentSection].insert( option );

        }

        return out;

    }

}
