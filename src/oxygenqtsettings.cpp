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
#include "oxygengtkrc.h"
#include "config.h"

#include <gtk/gtk.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
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
        _tooltipTransparent( true ),
        _tooltipDrawStyledFrames( true ),
        _viewDrawFocusIndicator( true ),
        _viewDrawTreeBranchLines( true ),
        _viewDrawTriangularExpander( true ),
        _viewTriangularExpanderSize( ArrowSmall ),
        _menuHighlightMode( MM_DARK ),
        _windowDragEnabled( true ),
        _windowDragMode( WD_FULL ),
        _startDragDist( 4 ),
        _startDragTime( 500 ),
        _buttonSize( ButtonDefault ),
        _frameBorder( BorderDefault ),
        _activeShadowConfiguration( Palette::Active ),
        _inactiveShadowConfiguration( Palette::Inactive ),
        _initialized( false ),
        _kdeColorsInitialized( false ),
        _gtkColorsInitialized( false ),
        _KDESession( false )
    {}

    //_________________________________________________________
    void QtSettings::loadKdeGlobals( void )
    {

        _kdeGlobals.clear();
        for( PathList::const_reverse_iterator iter = _kdeConfigPathList.rbegin(); iter != _kdeConfigPathList.rend(); ++iter )
        { _kdeGlobals.merge( OptionMap( sanitizePath( *iter + "/kdeglobals" ) ) ); }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeGlobals - kdeglobals: " << std::endl;
        std::cerr << _kdeGlobals << std::endl;
        #endif

    }

    //_________________________________________________________
    void QtSettings::initialize( unsigned int flags )
    {

        const bool forced( flags&Forced );

        if( _initialized && !forced ) return;
        _initialized = true;

        if( g_getenv( "KDE_FULL_SESSION" ) )
        { _KDESession = true; }

        // init application name
        if( flags & AppName )
        { initApplicationName(); }

        // configuration path
        _kdeConfigPathList = kdeConfigPathList();

        // load kdeglobals
        loadKdeGlobals();

        #if !OXYGEN_FORCE_KDE_ICONS_AND_FONTS
        // TODO: Add support for gtk schemes when not _KDESession
        if( _KDESession )
        #endif
        {
            // reload icons
            #if OXYGEN_ICON_HACK
            if( flags & Icons )
            {
                _kdeIconPathList = kdeIconPathList();
                loadKdeIcons();
            }
            #endif

            // reload fonts
            if( flags & Fonts )
            { loadKdeFonts(); }

        }

        // kde globals options
        if( flags & KdeGlobals )
        { loadKdeGlobalsOptions(); }

        // oxygen options
        if( flags & Oxygen )
        { loadOxygenOptions(); }

        // color palette
        if( flags & Colors )
        {
            loadKdePalette( forced );
            generateGtkColors();
        }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::initialize - done. " << std::endl;
        #endif

        return;

    }

    //_________________________________________________________
    void QtSettings::initializeColors( bool forced )
    {

        // reload palette, if needed
        loadKdePalette( forced );

        if( _gtkColorsInitialized && !forced ) return;
        _gtkColorsInitialized = true;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::initializeColors." << std::endl;
        #endif

        generateGtkColors();

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::initializeColors - done. " << std::endl;
        #endif

        return;

    }

    //_________________________________________________________
    PathList QtSettings::kdeConfigPathList( void ) const
    {

        PathList out;

        // load icon install prefix
        char* path = 0L;
        if( g_spawn_command_line_sync( "kde4-config --path config", &path, 0L, 0L, 0L ) && path )
        {
            out.split( path );
        } else {
            const std::string userConfigDir( std::string( g_get_user_config_dir() ) + "/oxygen-gtk" );
            out.push_back( userConfigDir );
        }

        out.push_back( GTK_THEME_DIR );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::kdeConfigPathList - loading configuration from path: " << std::endl;
        std::cerr << out << std::endl;
        #endif

        return out;

    }

    //_________________________________________________________
    PathList QtSettings::kdeIconPathList( void ) const
    {

        // load icon install prefix
        PathList out;
        char* path = 0L;
        if( g_spawn_command_line_sync( "kde4-config --path icon", &path, 0L, 0L, 0L ) && path )
        { out.split( path ); }

        // make sure defaultKdeIconPath is included in the list
        if( std::find( out.begin(), out.end(), _defaultKdeIconPath ) == out.end() )
        { out.push_back( _defaultKdeIconPath ); }

        return out;

    }

    //_________________________________________________________
    void QtSettings::addIconTheme( PathList& pathList, const std::string& theme )
    {

        // do nothing if theme have already been included in the loop
        if( _iconThemes.find( theme ) != _iconThemes.end() ) return;
        _iconThemes.insert( theme );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::addIconTheme - adding " << theme << std::endl;
        #endif

        // add all possible path (based on _kdeIconPathList) and look for possible parent
        std::string parentTheme;
        for( PathList::const_iterator iter = _kdeIconPathList.begin(); iter != _kdeIconPathList.end(); ++iter )
        {

            // create path and check for existence
            std::string path( sanitizePath( *iter + '/' + theme ) );
            struct stat st;
            if( stat( path.c_str(), &st ) != 0 ) continue;

            // add to path list
            pathList.push_back( path );
            if( parentTheme.empty() )
            {
                const std::string index( sanitizePath( *iter + '/' + theme + "/index.theme" ) );
                OptionMap themeOptions( index );
                parentTheme = themeOptions.getValue( "[Icon Theme]", "Inherits" );
            }

        }

        // add parent if needed
        if( !parentTheme.empty() )
        {
            // split using "," as a separator
            PathList parentThemes( parentTheme, "," );
            for( PathList::const_iterator iter = parentThemes.begin(); iter != parentThemes.end(); ++iter )
            { addIconTheme( pathList, *iter ); }
        }

        return;

    }

    //_________________________________________________________
    void QtSettings::initApplicationName( void )
    {
        const char* applicationName = g_get_prgname();
        if( applicationName ) { _applicationName.parse( applicationName ); }

    }

    //_________________________________________________________
    void QtSettings::loadKdeIcons( void )
    {

        // local gtkrc
        Gtk::RC rc;

        // load icon theme and path to gtk
        _iconThemes.clear();
        _kdeIconTheme = _kdeGlobals.getOption( "[Icons]", "Theme" ).toVariant<std::string>("oxygen");

        std::ostringstream themeNameStr;
        themeNameStr << "gtk-icon-theme-name=\"" << _kdeIconTheme << "\"" << std::endl;
        themeNameStr << "gtk-fallback-icon-theme=\"" << _kdeFallbackIconTheme << "\"";
        rc.addToHeaderSection( themeNameStr.str() );

        // check show icons on push buttons kde option
        const std::string showIconsOnPushButton( _kdeGlobals.getValue( "[KDE]", "ShowIconsOnPushButtons", "true" ) );

        // add option
        if( showIconsOnPushButton == "false" )
        {
            rc.addToHeaderSection( "gtk-button-images = 0\n" );
        }

        // create icon translator, for stock icons
        GtkIcons icons;

        // load icon sizes from kde
        // const int desktopIconSize( _kdeGlobals.getOption( "[DesktopIcons]", "Size" ).toInt( 48 ) );
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
        icons.setIconSize( "gtk-dnd", mainToolbarIconSize );
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

        rc.merge( icons.generate( iconThemeList ) );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeIcons - Gtkrc: " << std::endl;
        std::cerr << rc << std::endl;
        #endif

        // pass all options to gtk
        rc.commit();

    }

    //_________________________________________________________
    void QtSettings::loadKdePalette( bool forced )
    {

        if( _kdeColorsInitialized && !forced ) return;
        _kdeColorsInitialized = true;

        // contrast
        ColorUtils::setContrast( _kdeGlobals.getOption( "[KDE]", "contrast" ).toVariant<double>( 7 ) / 10 );

        // palette
        _palette.clear();
        _palette.setColor( Palette::Active, Palette::Window, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Window]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::WindowText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Window]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Button, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::ButtonText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Button]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Selected, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Selection]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::SelectedText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Selection]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Tooltip, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Tooltip]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::TooltipText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:Tooltip]", "ForegroundNormal" ) ) );

        _palette.setColor( Palette::Active, Palette::Focus, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "DecorationFocus" ) ) );
        _palette.setColor( Palette::Active, Palette::Hover, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "DecorationHover" ) ) );

        _palette.setColor( Palette::Active, Palette::Base, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "BackgroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::BaseAlternate, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "BackgroundAlternate" ) ) );
        _palette.setColor( Palette::Active, Palette::Text, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "ForegroundNormal" ) ) );
        _palette.setColor( Palette::Active, Palette::NegativeText, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[Colors:View]", "ForegroundNegative" ) ) );

        _palette.setColor( Palette::Active, Palette::ActiveWindowBackground, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[WM]", "activeBackground" ) ) );
        _palette.setColor( Palette::Active, Palette::InactiveWindowBackground, ColorUtils::Rgba::fromKdeOption( _kdeGlobals.getValue( "[WM]", "inactiveBackground" ) ) );

        // generate inactive and disabled palette from active, applying effects from kdeglobals
        _inactiveChangeSelectionColor = ( _kdeGlobals.getOption( "[ColorEffects:Inactive]", "ChangeSelectionColor" ).toVariant<std::string>( "false" ) == "true" );
        _palette.generate( Palette::Active, Palette::Inactive, ColorUtils::Effect( Palette::Inactive, _kdeGlobals ), _inactiveChangeSelectionColor );
        _palette.generate( Palette::Active, Palette::Disabled, ColorUtils::Effect( Palette::Disabled, _kdeGlobals ) );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdePalette - palette: " << std::endl;
        std::cerr << _palette << std::endl;
        #endif

    }

    //_________________________________________________________
    void QtSettings::generateGtkColors( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::generateGtkColors" << std::endl;
        #endif

        // customize gtk palette
        _palette.setGroup( Palette::Active );

        // local gtkrc
        Gtk::RC rc;

        // default colors
        rc.setCurrentSection( Gtk::RC::defaultSection() );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Window ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[PRELIGHT]", _palette.color( Palette::Window ) ) );

        if( applicationName().isMozilla() || applicationName().isGoogleChrome() )
        {

            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[ACTIVE]", _palette.color( Palette::Window ) ) );

        } else {

            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[ACTIVE]", _palette.color( Palette::Inactive, Palette::Selected ) ) );

        }

        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[SELECTED]", _palette.color( Palette::Selected ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[INSENSITIVE]", _palette.color( Palette::Window ) ) );

        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::WindowText ) ) );

        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[NORMAL]", _palette.color( Palette::Base ) ) );

        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[ACTIVE]", _palette.color( Palette::Inactive, Palette::Selected ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[PRELIGHT]", _palette.color( Palette::Base ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[SELECTED]", _palette.color( Palette::Selected ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Base ) ) );

        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[NORMAL]", _palette.color( Palette::Text ) ) );

        if( _inactiveChangeSelectionColor ) rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::Text ) ) );
        else rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::SelectedText ) ) );

        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::Text ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[SELECTED]", _palette.color( Palette::SelectedText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Text ) ) );

        // buttons
        rc.addSection( "oxygen-buttons", Gtk::RC::defaultSection() );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Button ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[ACTIVE]", _palette.color( Palette::Button ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[PRELIGHT]", _palette.color( Palette::Button ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Button ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::ButtonText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::ButtonText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::ButtonText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::ButtonText ) ) );
        rc.addToRootSection( "class \"GtkOptionMenu\" style \"oxygen-buttons\"" );
        rc.addToRootSection( "widget_class \"*<GtkButton>.<GtkLabel>\" style \"oxygen-buttons\"" );

        rc.addSection( "oxygen-combobox", "oxygen-buttons" );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[NORMAL]", _palette.color( Palette::ButtonText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::ButtonText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::ButtonText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::ButtonText ) ) );        rc.addToRootSection( "class \"*Button\" style \"oxygen-buttons\"" );
        rc.addToRootSection( "widget_class \"*<GtkComboBox>.<GtkCellView>\" style \"oxygen-combobox\"" );

        // checkboxes and radio buttons
        rc.addSection( "oxygen-checkbox-buttons", "oxygen-buttons" );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::WindowText ) ) );
        rc.addToRootSection( "widget_class \"*<GtkCheckButton>.<GtkLabel>\" style \"oxygen-checkbox-buttons\"" );

        // progressbar labels
        rc.addSection( "oxygen-progressbar-labels", Gtk::RC::defaultSection() );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[NORMAL]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::WindowText ) ) );

        rc.addToRootSection( "class \"GtkProgressBar\" style \"oxygen-progressbar-labels\"" );

        // menu items
        rc.addSection( "oxygen-menubar-item", "oxygen-menu-font" );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[NORMAL]", _palette.color( Palette::Text ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::WindowText ) ) );

        if( _menuHighlightMode == MM_STRONG )
        {

            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::Text ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[SELECTED]", _palette.color( Palette::SelectedText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::SelectedText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::SelectedText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::SelectedText ) ) );

        } else {

            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::Text ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[SELECTED]", _palette.color( Palette::Text ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[ACTIVE]", _palette.color( Palette::Text ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::WindowText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[SELECTED]", _palette.color( Palette::WindowText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[ACTIVE]", _palette.color( Palette::WindowText ) ) );

        }
        rc.addToRootSection( "widget_class \"*<GtkMenuItem>.<GtkLabel>\" style \"oxygen-menubar-item\"" );


        if( _menuHighlightMode == MM_STRONG )
        {
            rc.addSection( "oxygen-menu-item", "oxygen-menubar-item" );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  text[PRELIGHT]", _palette.color( Palette::SelectedText ) ) );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[PRELIGHT]", _palette.color( Palette::SelectedText ) ) );
            rc.addToRootSection( "widget_class \"*<GtkMenu>.<GtkMenuItem>.<GtkLabel>\" style \"oxygen-menu-item\"" );
        }

        // text entries
        rc.addSection( "oxygen-entry", Gtk::RC::defaultSection() );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Base ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Base ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  base[INSENSITIVE]", _palette.color( Palette::Disabled, Palette::Base ) ) );
        rc.addToRootSection( "class \"GtkSpinButton\" style \"oxygen-entry\"" );
        rc.addToRootSection( "class \"GtkEntry\" style \"oxygen-entry\"" );
        rc.addToRootSection( "widget_class \"*<GtkComboBoxEntry>.<GtkButton>\" style \"oxygen-entry\"" );
        rc.addToRootSection( "widget_class \"*<GtkCombo>.<GtkButton>\" style \"oxygen-entry\"" );

        // tooltips
        rc.addSection( "oxygen-tooltips", Gtk::RC::defaultSection() );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  bg[NORMAL]", _palette.color( Palette::Tooltip ) ) );
        rc.addToCurrentSection( Gtk::RCOption<std::string>( "  fg[NORMAL]", _palette.color( Palette::TooltipText ) ) );
        rc.addToCurrentSection( Gtk::RCOption<int>( "  xthickness", 3 ) );
        rc.addToCurrentSection( Gtk::RCOption<int>( "  ythickness", 3 ) );
        rc.addToRootSection( "widget \"gtk-tooltip*\" style \"oxygen-tooltips\"" );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::generateGtkColors - Gtkrc: " << std::endl;
        std::cerr << rc << std::endl;
        #endif

        // commit to gtk
        rc.commit();

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

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::QtSettings::loadKdeFonts - cannot load default font" << std::endl;
            #endif

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

                #if OXYGEN_DEBUG
                std::cerr << "Oxygen::QtSettings::loadKdeFonts - cannot load font named " << iter->second << std::endl;
                #endif
                local = defaultFont;

            }

            // store in map
            fonts[iter->first] = local;

        }

        // local GtkRC
        Gtk::RC rc;

        // pass fonts to RC
        if( fonts[FontInfo::Default].isValid() )
        { rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-font-name", fonts[FontInfo::Default] ) ); }

        if( fonts[FontInfo::Default].isValid() )
        {
            rc.setCurrentSection( Gtk::RC::defaultSection() );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  font_name", fonts[FontInfo::Default] ) );
        }

        if( fonts[FontInfo::Menu].isValid() )
        {
            rc.addSection( "oxygen-menu-font", Gtk::RC::defaultSection() );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  font_name", fonts[FontInfo::Menu] ) );
            rc.addToRootSection( "widget_class \"*<GtkMenuItem>.<GtkLabel>\" style \"oxygen-menu-font\"" );
        }

        if( fonts[FontInfo::ToolBar].isValid() )
        {
            rc.addSection( "oxygen-toolbar-font", Gtk::RC::defaultSection() );
            rc.addToCurrentSection( Gtk::RCOption<std::string>( "  font_name", fonts[FontInfo::ToolBar] ) );
            rc.addToRootSection( "widget_class \"*<GtkToolbar>.*\" style \"oxygen-toolbar-font\"" );
        }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeFonts - Gtkrc: " << std::endl;
        std::cerr << rc << std::endl;
        #endif

        // pass all resources to gtk and clear
        rc.commit();

    }

    //_________________________________________________________
    void QtSettings::loadKdeGlobalsOptions( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeGlobalsOptions" << std::endl;
        #endif

        // local gtkrc
        Gtk::RC rc;

        // toolbar style
        std::string toolbarTextPosition( _kdeGlobals.getOption( "[Toolbar style]", "ToolButtonStyle" ).toVariant<std::string>( "TextBelowIcon" ) );
        if( toolbarTextPosition == "TextOnly" ) rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_TEXT" ) );
        else if( toolbarTextPosition == "TextBesideIcon" ) rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_BOTH_HORIZ" ) );
        else if( toolbarTextPosition == "NoText" ) rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_ICONS" ) );
        else rc.addToHeaderSection( Gtk::RCOption<std::string>( "gtk-toolbar-style", "GTK_TOOLBAR_BOTH" ) );

        // start drag time and distance
        _startDragDist = _kdeGlobals.getOption( "[KDE]", "StartDragDist" ).toVariant<int>( 4 );
        _startDragTime = _kdeGlobals.getOption( "[KDE]", "StartDragTime" ).toVariant<int>( 500 );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeGlobalsOptions - Gtkrc: " << std::endl;
        std::cerr << rc << std::endl;
        #endif

        // pass all resources to gtk and clear
        rc.commit();

    }

    //_________________________________________________________
    void QtSettings::loadOxygenOptions( void )
    {

        OptionMap oxygen;
        for( PathList::const_reverse_iterator iter = _kdeConfigPathList.rbegin(); iter != _kdeConfigPathList.rend(); ++iter )
        { oxygen.merge( OptionMap( sanitizePath( *iter + "/oxygenrc" ) ) ); }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadOxygenOptions - Oxygenrc: " << std::endl;
        std::cerr << oxygen << std::endl;
        #endif

        // checkbox style
        _checkBoxStyle = (oxygen.getValue( "[Style]", "CheckBoxStyle", "CS_CHECK" ) == "CS_CHECK") ? CS_CHECK:CS_X;

        // checkbox style
        _tabStyle = (oxygen.getValue( "[Style]", "TabStyle", "TS_SINGLE" ) == "TS_SINGLE") ? TS_SINGLE:TS_PLAIN;

        // colored scrollbars
        _scrollBarColored = oxygen.getOption( "[Style]", "ScrollBarColored" ).toVariant<std::string>("false") == "true";

        // colored scrollbars
        _scrollBarBevel = oxygen.getOption( "[Style]", "ScrollBarBevel" ).toVariant<std::string>("false") == "true";

        // scrollbar buttons
        _scrollBarAddLineButtons = oxygen.getOption( "[Style]", "ScrollBarAddLineButtons" ).toVariant<int>( 2 );
        _scrollBarSubLineButtons = oxygen.getOption( "[Style]", "ScrollBarSubLineButtons" ).toVariant<int>( 1 );

        // toolbar separators
        _toolBarDrawItemSeparator = oxygen.getOption( "[Style]", "ToolBarDrawItemSeparator" ).toVariant<std::string>("true") == "true";

        // tooltips
        _tooltipTransparent = oxygen.getOption( "[Style]", "ToolTipTransparent" ).toVariant<std::string>("true") == "true";
        _tooltipDrawStyledFrames = oxygen.getOption( "[Style]", "ToolTipDrawStyledFrames" ).toVariant<std::string>("true") == "true";

        // focus indicator in views
        _viewDrawFocusIndicator = oxygen.getOption( "[Style]", "ViewDrawFocusIndicator" ).toVariant<std::string>("true") == "true";

        // tree branch lines
        _viewDrawTreeBranchLines = oxygen.getOption( "[Style]", "ViewDrawTreeBranchLines" ).toVariant<std::string>("true") == "true";

        // triangular expanders
        _viewDrawTriangularExpander = oxygen.getOption( "[Style]", "ViewDrawTriangularExpander" ).toVariant<std::string>("true") == "true";

        // triangular expander (arrow) size
        std::string expanderSize( oxygen.getOption( "[Style]", "ViewTriangularExpanderSize" ).toVariant<std::string>("TE_SMALL") );
        if( expanderSize == "TE_NORMAL" ) _viewTriangularExpanderSize = ArrowNormal;
        else if( expanderSize == "TE_TINY" ) _viewTriangularExpanderSize = ArrowTiny;
        else _viewTriangularExpanderSize = ArrowSmall;

        // menu highlight mode
        std::string highlightMode( oxygen.getOption( "[Style]", "MenuHighlightMode" ).toVariant<std::string>("MM_DARK") );
        if( highlightMode == "MM_SUBTLE" ) _menuHighlightMode = MM_SUBTLE;
        else if( highlightMode == "MM_STRONG" ) _menuHighlightMode = MM_STRONG;
        else _menuHighlightMode = MM_DARK;

        // window drag mode
        _windowDragEnabled = oxygen.getOption( "[Style]", "WindowDragEnabled" ).toVariant<std::string>("true") == "true";

        std::string windowDragMode( oxygen.getOption( "[Style]", "WindowDragMode" ).toVariant<std::string>("WD_FULL") );
        if( windowDragMode == "WD_MINIMAL" ) _windowDragMode = WD_MINIMAL;
        else _windowDragMode = WD_FULL;

        // window decoration button size
        std::string buttonSize( oxygen.getValue( "[Windeco]", "ButtonSize", "Normal") );
        if( buttonSize == "Small" ) _buttonSize = ButtonSmall;
        else if( buttonSize == "Large" ) _buttonSize = ButtonLarge;
        else if( buttonSize == "Very Large" ) _buttonSize = ButtonVeryLarge;
        else if( buttonSize == "Huge" ) _buttonSize = ButtonHuge;
        else _buttonSize = ButtonDefault;

        // window decoration frame border size
        std::string frameBorder(  oxygen.getValue( "[Windeco]", "FrameBorder", "Normal") );
        if( frameBorder == "No Border" ) _frameBorder = BorderNone;
        else if( frameBorder == "No Side Border" ) _frameBorder = BorderNoSide;
        else if( frameBorder == "Tiny" ) _frameBorder = BorderTiny;
        else if( frameBorder == "Large" ) _frameBorder = BorderLarge;
        else if( frameBorder == "Very Large" ) _frameBorder = BorderVeryLarge;
        else if( frameBorder == "Huge" ) _frameBorder = BorderHuge;
        else if( frameBorder == "Very Huge" ) _frameBorder = BorderVeryHuge;
        else if( frameBorder == "Oversized" ) _frameBorder = BorderOversized;
        else _frameBorder = BorderDefault;

        // shadow configurations
        _activeShadowConfiguration.initialize( oxygen );
        _inactiveShadowConfiguration.initialize( oxygen );

        // local gtkrc
        Gtk::RC rc;

        // copy relevant options to to gtk
        // scrollbar width
        rc.setCurrentSection( Gtk::RC::defaultSection() );
        rc.addToCurrentSection( Gtk::RCOption<int>(
            "  GtkScrollbar::slider-width",
            oxygen.getOption( "[Style]", "ScrollBarWidth" ).toVariant<int>(15) - 1 ) );

        rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-backward-stepper", _scrollBarSubLineButtons > 0 ) );
        rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-forward-stepper", _scrollBarAddLineButtons > 0 ) );

        // note the inversion for add and sub, due to the fact that kde options refer to the button location, and not its direction
        rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-secondary-backward-stepper", _scrollBarAddLineButtons > 1 ) );
        rc.addToCurrentSection( Gtk::RCOption<bool>("  GtkScrollbar::has-secondary-forward-stepper", _scrollBarSubLineButtons > 1 ) );

        // mnemonics
        const bool showMnemonics( oxygen.getOption( "[Style]", "ShowMnemonics" ).toVariant<std::string>("true") == "true" );
        rc.addToHeaderSection( Gtk::RCOption<int>( "gtk-auto-mnemonics", !showMnemonics ) );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadOxygenOptions - Gtkrc: " << std::endl;
        std::cerr << rc << std::endl;
        #endif

        // pass all resources to gtk and clear
        rc.commit();

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

}
