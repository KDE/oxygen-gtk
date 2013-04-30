/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
#include "oxygentimeline.h"
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
    /*
    Note: the default values set in the constructor are actually ignored.
    Real default values are set via Oxygen::QtSettings::loadKdeGlobalsOptions,
    from the oxygenrc file provided with oxygen-gtk
    */
    QtSettings::QtSettings( void ):
        _kdeIconTheme( "oxygen" ),
        _kdeFallbackIconTheme( "gnome" ),
        _inactiveChangeSelectionColor( false ),
        _useIconEffect( true ),
        _checkBoxStyle( CS_CHECK ),
        _tabStyle( TS_SINGLE ),
        _scrollBarAddLineButtons( 2 ),
        _scrollBarSubLineButtons( 1 ),
        _toolBarDrawItemSeparator( true ),
        _tooltipTransparent( true ),
        _tooltipDrawStyledFrames( true ),
        _viewDrawFocusIndicator( true ),
        _viewDrawTreeBranchLines( true ),
        _viewDrawTriangularExpander( true ),
        _viewTriangularExpanderSize( ArrowSmall ),
        _viewInvertSortIndicator( false ),
        _menuHighlightMode( MM_DARK ),
        _windowDragEnabled( true ),
        _windowDragMode( WD_FULL ),
        _startDragDist( 4 ),
        _startDragTime( 500 ),
        _animationsEnabled( true ),
        _genericAnimationsEnabled( true ),
        _menuBarAnimationType( Fade ),
        _menuAnimationType( Fade ),
        _toolBarAnimationType( Fade ),
        _genericAnimationsDuration( 150 ),
        _menuBarAnimationsDuration( 150 ),
        _menuBarFollowMouseAnimationsDuration( 80 ),
        _menuAnimationsDuration( 150 ),
        _menuFollowMouseAnimationsDuration( 40 ),
        _toolBarAnimationsDuration( 50 ),
        _buttonSize( ButtonDefault ),
        _frameBorder( BorderDefault ),
        _windecoBlendType( FollowStyleHint ),
        _activeShadowConfiguration( Palette::Active ),
        _inactiveShadowConfiguration( Palette::Inactive ),
        _argbEnabled( true ),
        _initialized( false ),
        _kdeColorsInitialized( false ),
        _gtkColorsInitialized( false ),
        _KDESession( false ),
        _provider( gtk_css_provider_new() )
    {}

    //_________________________________________________________
    bool QtSettings::initialize( unsigned int flags )
    {

        const bool forced( flags&Forced );

        if( _initialized && !forced ) return false;
        _initialized = true;

        if( g_getenv( "KDE_FULL_SESSION" ) )
        { _KDESession = true; }

        // init application name
        if( flags & AppName )
        {
            initUserConfigDir();
            initApplicationName();
            initArgb();
        }

        // keep track of whats changed
        bool changed( false );

        // configuration path
        {
            const PathList old( _kdeConfigPathList );
            _kdeConfigPathList = kdeConfigPathList();
            changed |= (old != _kdeConfigPathList );
        }

        // icon path
        {
            const PathList old( _kdeIconPathList );
            _kdeIconPathList = kdeIconPathList();
            changed |= (old != _kdeIconPathList );
        }

        // load kdeglobals and oxygen option maps
        const bool kdeGlobalsChanged = loadKdeGlobals();
        const bool oxygenChanged = loadOxygen();

        // do nothing if settings not changed
        if( !(changed||kdeGlobalsChanged||oxygenChanged) ) return false;

        // dialog button ordering
        /* TODO: in principle this should be needed only once */
        GtkSettings* settings( gtk_settings_get_default() );
        gtk_settings_set_long_property( settings, "gtk-alternative-button-order", 1, "oxygen-gtk" );

        // clear gtkrc
        _css.clear();

        // kde globals options
        if( flags & KdeGlobals )
        { loadKdeGlobalsOptions(); }

        // oxygen options
        if( flags & Oxygen )
        { loadOxygenOptions(); }

        #if !OXYGEN_FORCE_KDE_ICONS_AND_FONTS
        // TODO: Add support for gtk schemes when not _KDESession
        if( _KDESession )
        #endif
        {

            // reload fonts
            if( flags & Fonts )
            { loadKdeFonts(); }

            // reload icons
            #if OXYGEN_ICON_HACK
            if( flags & Icons )
            { loadKdeIcons(); }
            #endif

        }

        // color palette
        if( flags & Colors )
        {
            loadKdePalette( forced );
            generateGtkColors();
        }

        // apply extra programatically set metrics
        loadExtraOptions();

        // print generated Gtkrc and commit
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::initialize - GtkCss: " << std::endl;
        std::cerr << _css << std::endl;
        #endif

        // pass all resources to gtk and clear
        _css.commit( _provider );

        if( GdkScreen* screen = gdk_screen_get_default() )
        {
            gtk_style_context_remove_provider_for_screen( screen, GTK_STYLE_PROVIDER( _provider ) );
            gtk_style_context_add_provider_for_screen( screen, GTK_STYLE_PROVIDER( _provider ), GTK_STYLE_PROVIDER_PRIORITY_THEME + 10 );
        }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::initialize - done. " << std::endl;
        #endif

        return true;

    }

    //_________________________________________________________
    bool QtSettings::runCommand( const std::string& command, char*& result ) const
    {

        if( FILE* fp = popen( command.c_str(), "r" ) )
        {

            // read command output. Make sure that the buffer is large enough to read the entire
            // output, by multiplying its initial size by two as long as the last character is not '\n'
            // note that the allocated string must be freed by the calling method
            gulong bufSize=512;
            size_t currentOffset=0;
            result= static_cast<char*>(g_malloc(bufSize));
            while( fgets( result+currentOffset, bufSize-currentOffset, fp ) && result[strlen(result)-1] != '\n' )
            {
                currentOffset = bufSize-1;
                bufSize *= 2;
                result = static_cast<char*>( g_realloc( result, bufSize ) );
            }

            pclose(fp);
            return true;

        } else return false;

    }

    //_________________________________________________________
    bool QtSettings::loadKdeGlobals( void )
    {

        // save backup
        OptionMap old = _kdeGlobals;

        // clear and reload
        _kdeGlobals.clear();
        for( PathList::const_reverse_iterator iter = _kdeConfigPathList.rbegin(); iter != _kdeConfigPathList.rend(); ++iter )
        {
            const std::string filename( sanitizePath( *iter + "/kdeglobals" ) );
            _kdeGlobals.merge( OptionMap( filename ) );
            monitorFile( filename );
        }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeGlobals - kdeglobals: " << std::endl;
        std::cerr << _kdeGlobals << std::endl;
        #endif

        // check change
        return old != _kdeGlobals;

    }

    //_________________________________________________________
    bool QtSettings::loadOxygen( void )
    {

        // save backup
        OptionMap old = _oxygen;

        // clear and reload
        _oxygen.clear();
        for( PathList::const_reverse_iterator iter = _kdeConfigPathList.rbegin(); iter != _kdeConfigPathList.rend(); ++iter )
        {
            const std::string filename( sanitizePath( *iter + "/oxygenrc" ) );
            _oxygen.merge( filename );
            monitorFile( filename );
        }

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadOxygen - Oxygenrc: " << std::endl;
        std::cerr << _oxygen << std::endl;
        #endif

        // check change
        return old != _oxygen;

    }

    //_________________________________________________________
    PathList QtSettings::kdeConfigPathList( void ) const
    {

        PathList out;

        // load icon install prefix
        gchar* path = 0L;
        if( runCommand( "kde4-config --path config", path ) && path )
        {

            out.split( path );
            g_free( path );

        } else {

            out.push_back( userConfigDir() );

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
        if( runCommand( "kde4-config --path icon", path ) && path )
        {
            out.split( path );
            g_free( path );
        }

        // make sure defaultKdeIconPath is included in the list
        if( std::find( out.begin(), out.end(), _defaultKdeIconPath ) == out.end() )
        { out.push_back( _defaultKdeIconPath ); }

        return out;

    }

    //_________________________________________________________
    void QtSettings::initUserConfigDir( void )
    {

        // create directory name
        _userConfigDir = std::string( g_get_user_config_dir() ) + "/oxygen-gtk";

        // make sure that corresponding directory does exist
        struct stat st;
        if( stat( _userConfigDir.c_str(), &st ) != 0 )
        { mkdir( _userConfigDir.c_str(), S_IRWXU|S_IRWXG|S_IRWXO ); }

        // note: in some cases, the target might exist and not be a directory
        // nothing we can do about it. We won't overwrite the file to prevent dataloss

    }

    //_________________________________________________________
    void QtSettings::initArgb( void )
    {
        // get program name
        const char* appName = g_get_prgname();
        if( !appName ) return;

        // user-defined configuration file
        const std::string userConfig( userConfigDir() + "/argb-apps.conf");

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
            std::cerr << "Oxygen::QtSettings::initArgb - ARGB hack is disabled; program name: " << appName << std::endl;
            std::cerr << "Oxygen::QtSettings::initArgb - if disabling ARGB hack helps, please add this string:\n\ndisable:" << appName << "\n\nto ~/.config/oxygen-gtk/argb-apps.conf\nand report it here: https://bugs.kde.org/show_bug.cgi?id=260640" << std::endl;
            _argbEnabled = false;
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
            { std::cerr << "Oxygen::QtSettings::initArgb - ARGB config file \"" << configFile << "\" not found" << std::endl; }

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
            { std::cerr << "Oxygen::QtSettings::initArgb - user-defined ARGB config file \"" << userConfig << "\" not found - only system-wide one will be used" << std::endl; }

        }

        // load options into a string
        while( std::getline( userIn, contents, '\n' ) )
        {
            if( contents.empty() || contents[0] == '#' ) continue;
            lines.push_back( contents );
        }

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
                    _argbEnabled = enabled;
                    break;
                }
            }

        }

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
    void QtSettings::loadKdeIcons( void )
    {

        // update icon search path
        // put existing default path in a set
        PathSet searchPath( defaultIconSearchPath() );

        // add kde's path. Loop is reversed because added path must be prepended.
        for( PathList::const_reverse_iterator iter = _kdeIconPathList.rbegin(); iter != _kdeIconPathList.rend(); ++iter )
        {

            // remove trailing backslash, if any
            std::string path( *iter );
            if( path.empty() ) continue;
            if( path[path.size()-1] == '/' ) path = path.substr( 0, path.size()-1 );

            // check if already present and prepend if not
            if( searchPath.find( path ) == searchPath.end() )
            { gtk_icon_theme_prepend_search_path(gtk_icon_theme_get_default(), path.c_str() ); }
        }

        // load icon theme and path to gtk
        _iconThemes.clear();
        _kdeIconTheme = _kdeGlobals.getValue( "[Icons]", "Theme", "oxygen" );

        // store to settings
        GtkSettings* settings( gtk_settings_get_default() );

        gtk_settings_set_string_property( settings, "gtk-icon-theme-name", _kdeIconTheme.c_str(), "oxygen-gtk" );
        gtk_settings_set_string_property( settings, "gtk-fallback-icon-theme", _kdeFallbackIconTheme.c_str(), "oxygen-gtk" );

        // load icon sizes from kde
        // const int desktopIconSize( _kdeGlobals.getOption( "[DesktopIcons]", "Size" ).toInt( 48 ) );
        const int dialogIconSize( _kdeGlobals.getOption( "[DialogIcons]", "Size" ).toInt( 32 ) );
        const int panelIconSize( _kdeGlobals.getOption( "[PanelIcons]", "Size" ).toInt( 32 ) );
        const int mainToolbarIconSize( _kdeGlobals.getOption( "[MainToolbarIcons]", "Size" ).toInt( 22 ) );
        const int smallIconSize( _kdeGlobals.getOption( "[SmallIcons]", "Size" ).toInt( 16 ) );
        const int toolbarIconSize( _kdeGlobals.getOption( "[ToolbarIcons]", "Size" ).toInt( 22 ) );

        // set gtk icon sizes
        _icons.setIconSize( "panel-menu", smallIconSize );
        _icons.setIconSize( "panel", panelIconSize );
        _icons.setIconSize( "gtk-small-toolbar", toolbarIconSize );
        _icons.setIconSize( "gtk-large-toolbar", mainToolbarIconSize );
        _icons.setIconSize( "gtk-dnd", mainToolbarIconSize );
        _icons.setIconSize( "gtk-button", smallIconSize );
        _icons.setIconSize( "gtk-menu", smallIconSize );
        _icons.setIconSize( "gtk-dialog", dialogIconSize );
        _icons.setIconSize( "", smallIconSize );

        // load translation table, generate full translation list, and path to gtk
        _icons.loadTranslations( sanitizePath( std::string( GTK_THEME_DIR ) + "/icons4" ) );

        // generate full path list
        PathList iconThemeList;
        addIconTheme( iconThemeList, _kdeIconTheme );
        addIconTheme( iconThemeList, _kdeFallbackIconTheme );

        // load translation table
        _icons.loadTranslations( sanitizePath( std::string( GTK_THEME_DIR ) + "/icons4" ) );

        // generate internal icons factory
        _icons.generate( iconThemeList );

    }

    //_________________________________________________________
    PathSet QtSettings::defaultIconSearchPath( void ) const
    {
        PathSet searchPath;

        // load icon theme
        GtkIconTheme* theme( gtk_icon_theme_get_default() );
        if( !GTK_IS_ICON_THEME( theme ) ) return searchPath;

        // get default
        gchar** gtkSearchPath;
        int nElements;

        gtk_icon_theme_get_search_path( theme, &gtkSearchPath, &nElements );
        for( int i=0; i<nElements; i++ )
        { searchPath.insert( gtkSearchPath[i] ); }

        // free
        g_strfreev( gtkSearchPath );

        return searchPath;
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

        // load effects
        const ColorUtils::Effect disabledEffect( Palette::Disabled, _kdeGlobals );
        const ColorUtils::Effect inactiveEffect( Palette::Inactive, _kdeGlobals );

        // generate inactive and disabled palette from active, applying effects from kdeglobals
        _inactiveChangeSelectionColor = ( _kdeGlobals.getOption( "[ColorEffects:Inactive]", "ChangeSelectionColor" ).toVariant<std::string>( "false" ) == "true" );
        _palette.generate( Palette::Active, Palette::Inactive, inactiveEffect, _inactiveChangeSelectionColor );
        _palette.generate( Palette::Active, Palette::Disabled, disabledEffect );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdePalette - disabled effect: " << std::endl;
        std::cerr << disabledEffect << std::endl;

        std::cerr << "Oxygen::QtSettings::loadKdePalette - inactive effect: " << std::endl;
        std::cerr << inactiveEffect << std::endl;

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

        /*
        TODO:
        look at default values from GtkCssProvider for better style definitions
        (class based, rather than widget based)
        */

        // customize gtk palette
        _palette.setGroup( Palette::Active );

        // default
        _css.setCurrentSection( Gtk::CSS::defaultSection() );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Window ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::WindowText ) ) );

        _css.addSection( "*:selected" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Selected ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::WindowText ) ) );

        _css.addSection( "*:hover" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Hover ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::WindowText ) ) );

        _css.addSection( ".highlight" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Focus ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::WindowText ) ) );

        _css.addSection( "*:insensitive" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Disabled, Palette::Window ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::Disabled, Palette::WindowText ) ) );

        // entries
        _css.addSection(
            ".view, "
            ".entry, "
            "GtkEntry" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Base ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::Text ) ) );

        _css.addSection(
            ".view:insensitive, "
            ".entry:insensitive, "
            "GtkEntry:insensitive, "
            "GtkTreeView:insensitive, "
            "GtkTextView:insensitive, "
            "GtkIconView:insensitive" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Disabled, Palette::Base ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::Disabled, Palette::Text ) ) );

        _css.addSection(
            ".view:selected, "
            ".entry:selected, "
            "GtkEntry:selected, "
            "GtkTreeView:selected, "
            "GtkTextView:selected" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Selected ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::SelectedText ) ) );

        // sidebars
        _css.addSection( "GtkAssistant .sidebar" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Disabled, Palette::Base ) ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::Disabled, Palette::Text ) ) );

         // buttons and comboboxes
        _css.addSection( "GtkButton" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Button ) ) );

        _css.addSection( "GtkButton:insensitive" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Disabled, Palette::Button ) ) );

        _css.addSection(
            "GtkButton>GtkLabel, "
            "GtkButton>GtkAlignment>GtkBox>GtkLabel, "
            "GtkComboBox>GtkCellView" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::ButtonText ) ) );

        _css.addSection(
            "GtkButton>GtkLabel:insensitive, "
            "GtkButton>GtkAlignment>GtkBox>GtkLabel:insensitive, "
            "GtkComboBox>GtkCellView:insensitive" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::Disabled, Palette::ButtonText ) ) );

        // checkboxes and radio buttons
        _css.addSection( "GtkCheckButton>GtkLabel" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::WindowText ) ) );

        _css.addSection( "GtkCheckButton>GtkLabel:insensitive" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::Disabled, Palette::WindowText ) ) );

        // tooltips
        _css.addSection( "GtkWindow#gtk-tooltip" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, _palette.color( Palette::Tooltip ) ) );
        _css.addToCurrentSection( "  padding: 3px;" );

        _css.addSection( "GtkWindow#gtk-tooltip GtkLabel" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, _palette.color( Palette::TooltipText ) ) );

        // rubber band selection (at least for nautilus)
        // FIXME: is Palette::Selected the proper color? Is 0.35 the proper alpha?
        _css.addSection( ".view.rubberband,.rubberband" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BORDER_COLOR, _palette.color( Palette::Selected ) ) );
        {
            std::ostringstream color;
            color << "  " << GTK_STYLE_PROPERTY_BACKGROUND_COLOR << ": alpha ("<< (std::string)_palette.color( Palette::Selected ) << ", 0.35);";
            _css.addToCurrentSection( color.str() );
        }

        /*
        this is stupid. Setting the color to all widgets, like above, erases the .info, warning, etc. colors.
        Need to reset them here. This is copied from gtkcssprovider.c
        */
        _css.addSection( ".info" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, "@info_bg_color" ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, "@info_fg_color" ) );

        _css.addSection( ".warning" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, "@warning_bg_color" ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, "@warning_fg_color" ) );

        _css.addSection( ".question" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, "@question_bg_color" ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, "@question_fg_color" ) );

        _css.addSection( ".error" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, "@error_bg_color" ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_COLOR, "@error_fg_color" ) );

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

        // pass fonts to GtkSettings and css
        if( fonts[FontInfo::Default].isValid() )
        {
            // pass to settings
            GtkSettings* settings( gtk_settings_get_default() );
            gtk_settings_set_string_property( settings, "gtk-font-name", fonts[FontInfo::Default].toString( false ).c_str(), "oxygen-gtk" );

            _css.setCurrentSection( Gtk::CSS::defaultSection() );
            _css.addToCurrentSection( Gtk::CSSOption<std::string>( "font", fonts[FontInfo::Default].toString( false ) ) );

        }

        if( fonts[FontInfo::Menu].isValid() )
        {
            _css.addSection( "GtkMenuItem" );
            _css.addToCurrentSection( Gtk::CSSOption<std::string>( "font", fonts[FontInfo::Menu].toString( false ) ) );
        }

        if( fonts[FontInfo::ToolBar].isValid() )
        {
            _css.addSection( "GtkToolbar" );
            _css.addToCurrentSection( Gtk::CSSOption<std::string>( "font", fonts[FontInfo::ToolBar].toString( false ) ) );
        }

        // don't check for section and tag presence - use default font if not present
        _WMFont=FontInfo::fromKdeOption( _kdeGlobals.getValue( "[WM]", "activeFont", "Sans Serif,8,-1,5,50,0,0,0,0,0" ) );


        _css.addSection( "GtkAssistant .sidebar .highlight" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( "font-weight", "bold" ) );

    }

    //_________________________________________________________
    void QtSettings::loadKdeGlobalsOptions( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::QtSettings::loadKdeGlobalsOptions" << std::endl;
        #endif

        // toolbar style
        const std::string toolbarTextPosition( _kdeGlobals.getOption( "[Toolbar style]", "ToolButtonStyle" ).toVariant<std::string>( "TextBelowIcon" ) );
        GtkToolbarStyle toolbarStyle( GTK_TOOLBAR_BOTH );
        if( toolbarTextPosition == "TextOnly" ) toolbarStyle = GTK_TOOLBAR_TEXT;
        else if( toolbarTextPosition == "TextBesideIcon" ) toolbarStyle = GTK_TOOLBAR_BOTH_HORIZ;
        else if( toolbarTextPosition == "NoText" ) toolbarStyle = GTK_TOOLBAR_ICONS;

        GtkSettings* settings( gtk_settings_get_default() );
        gtk_settings_set_long_property( settings, "gtk-toolbar-style", toolbarStyle, "oxygen-gtk" );

        // dialog button ordering
        gtk_settings_set_long_property( settings, "gtk-alternative-button-order", 1, "oxygen-gtk" );

        // icons on buttons
        if( _kdeGlobals.getValue( "[KDE]", "ShowIconsOnPushButtons", "true" ) == "false" )
        { gtk_settings_set_long_property( settings, "gtk-button-images", 0, "oxygen-gtk" ); }

        // active icon effects
        _useIconEffect = _kdeGlobals.getOption( "[MainToolbarIcons]", "ActiveEffect" ).toVariant<std::string>( "gamma" ) != "none";

        // start drag time and distance
        _startDragDist = _kdeGlobals.getOption( "[KDE]", "StartDragDist" ).toVariant<int>( 4 );
        _startDragTime = _kdeGlobals.getOption( "[KDE]", "StartDragTime" ).toVariant<int>( 500 );

    }

    //_________________________________________________________
    void QtSettings::loadOxygenOptions( void )
    {

        // background pixmap
        _backgroundPixmap = _oxygen.getValue( "[Common]", "BackgroundPixmap", "" );

        // checkbox style
        _checkBoxStyle = (_oxygen.getValue( "[Style]", "CheckBoxStyle", "CS_CHECK" ) == "CS_CHECK") ? CS_CHECK:CS_X;

        // checkbox style
        _tabStyle = (_oxygen.getValue( "[Style]", "TabStyle", "TS_SINGLE" ) == "TS_SINGLE") ? TS_SINGLE:TS_PLAIN;

        // scrollbar buttons
        _scrollBarAddLineButtons = _oxygen.getOption( "[Style]", "ScrollBarAddLineButtons" ).toVariant<int>( 2 );
        _scrollBarSubLineButtons = _oxygen.getOption( "[Style]", "ScrollBarSubLineButtons" ).toVariant<int>( 1 );

        // toolbar separators
        _toolBarDrawItemSeparator = _oxygen.getOption( "[Style]", "ToolBarDrawItemSeparator" ).toVariant<std::string>("true") == "true";

        // tooltips
        _tooltipTransparent = _oxygen.getOption( "[Style]", "ToolTipTransparent" ).toVariant<std::string>("true") == "true";
        _tooltipDrawStyledFrames = _oxygen.getOption( "[Style]", "ToolTipDrawStyledFrames" ).toVariant<std::string>("true") == "true";

        // focus indicator in views
        _viewDrawFocusIndicator = _oxygen.getOption( "[Style]", "ViewDrawFocusIndicator" ).toVariant<std::string>("true") == "true";

        // tree branch lines
        _viewDrawTreeBranchLines = _oxygen.getOption( "[Style]", "ViewDrawTreeBranchLines" ).toVariant<std::string>("true") == "true";

        // triangular expanders
        _viewDrawTriangularExpander = _oxygen.getOption( "[Style]", "ViewDrawTriangularExpander" ).toVariant<std::string>("true") == "true";

        // triangular expander (arrow) size
        std::string expanderSize( _oxygen.getOption( "[Style]", "ViewTriangularExpanderSize" ).toVariant<std::string>("TE_SMALL") );
        if( expanderSize == "TE_NORMAL" ) _viewTriangularExpanderSize = ArrowNormal;
        else if( expanderSize == "TE_TINY" ) _viewTriangularExpanderSize = ArrowTiny;
        else _viewTriangularExpanderSize = ArrowSmall;

        // invert view sort indicators
        _viewInvertSortIndicator = _oxygen.getOption( "[Style]", "ViewInvertSortIndicator" ).toVariant<std::string>("false") == "true";

        // menu highlight mode
        std::string highlightMode( _oxygen.getOption( "[Style]", "MenuHighlightMode" ).toVariant<std::string>("MM_DARK") );
        if( highlightMode == "MM_SUBTLE" ) _menuHighlightMode = MM_SUBTLE;
        else if( highlightMode == "MM_STRONG" ) _menuHighlightMode = MM_STRONG;
        else _menuHighlightMode = MM_DARK;

        // window drag mode
        _windowDragEnabled = _oxygen.getOption( "[Style]", "WindowDragEnabled" ).toVariant<std::string>("true") == "true";

        std::string windowDragMode( _oxygen.getOption( "[Style]", "WindowDragMode" ).toVariant<std::string>("WD_FULL") );
        if( windowDragMode == "WD_MINIMAL" ) _windowDragMode = WD_MINIMAL;
        else _windowDragMode = WD_FULL;

        // animations
        _animationsEnabled = ( _oxygen.getOption( "[Style]", "AnimationsEnabled" ).toVariant<std::string>("true") == "true" );
        _genericAnimationsEnabled = ( _oxygen.getOption( "[Style]", "GenericAnimationsEnabled" ).toVariant<std::string>("true") == "true" );

        // menubar animation type
        std::string menuBarAnimationType( _oxygen.getValue( "[Style]", "MenuBarAnimationType", "MB_FADE") );
        if( menuBarAnimationType == "MB_NONE" ) _menuBarAnimationType = None;
        else if( menuBarAnimationType == "MB_FADE" ) _menuBarAnimationType = Fade;
        else if( menuBarAnimationType == "MB_FOLLOW_MOUSE" ) _menuBarAnimationType = FollowMouse;

        // menubar animation type
        std::string menuAnimationType( _oxygen.getValue( "[Style]", "MenuAnimationType", "ME_FADE") );
        if( menuAnimationType == "ME_NONE" ) _menuAnimationType = None;
        else if( menuAnimationType == "ME_FADE" ) _menuAnimationType = Fade;
        else if( menuAnimationType == "ME_FOLLOW_MOUSE" ) _menuAnimationType = FollowMouse;

        // toolbar animation type
        std::string toolBarAnimationType( _oxygen.getValue( "[Style]", "ToolBarAnimationType", "TB_FADE") );
        if( toolBarAnimationType == "TB_NONE" ) _toolBarAnimationType = None;
        else if( toolBarAnimationType == "TB_FADE" ) _toolBarAnimationType = Fade;
        else if( toolBarAnimationType == "TB_FOLLOW_MOUSE" ) _toolBarAnimationType = FollowMouse;

        // animations duration
        _genericAnimationsDuration = _oxygen.getOption( "[Style]", "GenericAnimationsDuration" ).toVariant<int>(150);
        _menuBarAnimationsDuration = _oxygen.getOption( "[Style]", "MenuBarAnimationsDuration" ).toVariant<int>(150);
        _menuBarFollowMouseAnimationsDuration = _oxygen.getOption( "[Style]", "MenuBarFollowMouseAnimationsDuration" ).toVariant<int>(80);
        _menuAnimationsDuration = _oxygen.getOption( "[Style]", "MenuAnimationsDuration" ).toVariant<int>(150);
        _menuFollowMouseAnimationsDuration = _oxygen.getOption( "[Style]", "MenuFollowMouseAnimationsDuration" ).toVariant<int>(40);
        _toolBarAnimationsDuration = _oxygen.getOption( "[Style]", "ToolBarAnimationsDuration" ).toVariant<int>(50);

        // animation steps
        TimeLine::setSteps( _oxygen.getOption( "[Style]", "AnimationSteps" ).toVariant<int>( 10 ) );

        // window decoration button size
        std::string buttonSize( _oxygen.getValue( "[Windeco]", "ButtonSize", "Normal") );
        if( buttonSize == "Small" ) _buttonSize = ButtonSmall;
        else if( buttonSize == "Large" ) _buttonSize = ButtonLarge;
        else if( buttonSize == "Very Large" ) _buttonSize = ButtonVeryLarge;
        else if( buttonSize == "Huge" ) _buttonSize = ButtonHuge;
        else _buttonSize = ButtonDefault;

        // window decoration frame border size
        std::string frameBorder(  _oxygen.getValue( "[Windeco]", "FrameBorder", "Normal") );
        if( frameBorder == "No Border" ) _frameBorder = BorderNone;
        else if( frameBorder == "No Side Border" ) _frameBorder = BorderNoSide;
        else if( frameBorder == "Tiny" ) _frameBorder = BorderTiny;
        else if( frameBorder == "Large" ) _frameBorder = BorderLarge;
        else if( frameBorder == "Very Large" ) _frameBorder = BorderVeryLarge;
        else if( frameBorder == "Huge" ) _frameBorder = BorderHuge;
        else if( frameBorder == "Very Huge" ) _frameBorder = BorderVeryHuge;
        else if( frameBorder == "Oversized" ) _frameBorder = BorderOversized;
        else _frameBorder = BorderDefault;

        // window decoration title alignment
        std::string titleAlign( _oxygen.getValue( "[Windeco]", "TitleAlignment", "Center" ) );
        if( titleAlign == "Left" ) _titleAlignment = PANGO_ALIGN_LEFT;
        else if( titleAlign == "Center" ) _titleAlignment = PANGO_ALIGN_CENTER;
        else if( titleAlign == "Right" ) _titleAlignment = PANGO_ALIGN_RIGHT;
        else _titleAlignment = PANGO_ALIGN_CENTER;

        // Windeco radial gradient enable option
        std::string wdBlendType( _oxygen.getValue( "[Windeco]", "BlendColor", "Follow Style Hint" ) );
        if( wdBlendType == "Follow Style Hint" ) _windecoBlendType=FollowStyleHint;
        else if( wdBlendType == "Radial Gradient" ) _windecoBlendType=RadialGradient;
        else if( wdBlendType == "Solid Color" ) _windecoBlendType=SolidColor;
        else _windecoBlendType=FollowStyleHint;

        // shadow configurations
        _activeShadowConfiguration.initialize( _oxygen );
        _inactiveShadowConfiguration.initialize( _oxygen );

        #if OXYGEN_DEBUG
        std::cerr << _activeShadowConfiguration << std::endl;
        std::cerr << _inactiveShadowConfiguration << std::endl;
        #endif

        // copy relevant options to to gtk
        // scrollbar width
        _css.addSection( "GtkScrollbar" );
        _css.addToCurrentSection( Gtk::CSSOption<int>(
            "-GtkRange-slider-width",
            _oxygen.getOption( "[Style]", "ScrollBarWidth" ).toVariant<int>(15) ) );

        _css.addToCurrentSection( Gtk::CSSOption<bool>("-GtkScrollbar-has-backward-stepper", _scrollBarSubLineButtons > 0 ) );
        _css.addToCurrentSection( Gtk::CSSOption<bool>("-GtkScrollbar-has-forward-stepper", _scrollBarAddLineButtons > 0 ) );

        // note the inversion for add and sub, due to the fact that kde options refer to the button location, and not its direction
        _css.addToCurrentSection( Gtk::CSSOption<bool>("-GtkScrollbar-has-secondary-backward-stepper", _scrollBarAddLineButtons > 1 ) );
        _css.addToCurrentSection( Gtk::CSSOption<bool>("-GtkScrollbar-has-secondary-forward-stepper", _scrollBarSubLineButtons > 1 ) );

        // mnemonics
        GtkSettings* settings( gtk_settings_get_default() );
        if( _oxygen.hasOption( "[Style]", "MnemonicsMode" ) )
        {

            const std::string mnemonicsMode( _oxygen.getOption( "[Style]", "MnemonicsMode" ).toVariant<std::string>("MN_ALWAYS") );
            if( mnemonicsMode == "MN_NEVER" )
            {

                gtk_settings_set_long_property( settings, "gtk-enable-mnemonics", false, "oxygen-gtk" );
                gtk_settings_set_long_property( settings, "gtk-auto-mnemonics", false, "oxygen-gtk" );

            } else if( mnemonicsMode == "MN_AUTO" ) {

                gtk_settings_set_long_property( settings, "gtk-enable-mnemonics", true, "oxygen-gtk" );
                gtk_settings_set_long_property( settings, "gtk-auto-mnemonics", true, "oxygen-gtk" );

            } else {

                gtk_settings_set_long_property( settings, "gtk-enable-mnemonics", true, "oxygen-gtk" );
                gtk_settings_set_long_property( settings, "gtk-auto-mnemonics", false, "oxygen-gtk" );

            }

        } else {

            // for backward compatibility
            const bool showMnemonics( _oxygen.getOption( "[Style]", "ShowMnemonics" ).toVariant<std::string>("true") == "true" );
            if( showMnemonics )
            {

                gtk_settings_set_long_property( settings, "gtk-enable-mnemonics", true, "oxygen-gtk" );
                gtk_settings_set_long_property( settings, "gtk-auto-mnemonics", false, "oxygen-gtk" );

            } else {

                gtk_settings_set_long_property( settings, "gtk-enable-mnemonics", false, "oxygen-gtk" );
                gtk_settings_set_long_property( settings, "gtk-auto-mnemonics", false, "oxygen-gtk" );

            }

        }

    }

    //_________________________________________________________
    void QtSettings::loadExtraOptions( void )
    {

        #if GTK_CHECK_VERSION( 3, 3, 0 )

        // button padding
        _css.setCurrentSection( "GtkButton" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "3px 4px 2px" ) );

        // pathbar button margins
        _css.addSection( "GtkPathBar>GtkToggleButton" );
        _css.addToCurrentSection(
            ( gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL ) ?
            Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "3px 2px 2px 12px;" ):
            Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "3px 12px 2px 2px;" ) );

        _css.addSection( "NautilusPathBar>GtkToggleButton" );
        _css.addToCurrentSection(
            ( gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL ) ?
            Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "2px 2px 2px 12px;" ):
            Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "2px 12px 2px 2px;" ) );

        // spinbox buttons
        _css.addSection( "GtkSpinButton.button" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BACKGROUND_COLOR, "alpha( #900185, 0 )" ) );
        _css.addToCurrentSection( ( gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL ) ?
            Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "0px 12px 0px 0px" ):
            Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "0px 0px 0px 12px" ) );

        // entries
        _css.addSection( "GtkEntry" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "4px 7px" ) );

        #else

        // pathbar button margins
        _css.addSection( "GtkPathBar>GtkToggleButton" );
        _css.addToCurrentSection( (gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL ) ?
            "  -GtkButton-inner-border: 1px 0px 0px 10px;":
            "  -GtkButton-inner-border: 1px 10px 0px 0px;" );

        // pathbar button margins
        _css.addSection( "NautilusPathBar>GtkToggleButton" );
        _css.addToCurrentSection( (gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL ) ?
            "  -GtkButton-inner-border: 0px 0px 0px 10px;":
            "  -GtkButton-inner-border: 0px 10px 0px 0px;" );

        // button padding
        _css.setCurrentSection( Gtk::CSS::defaultSection() );
        _css.addToCurrentSection( "  -GtkButton-inner-border: 1px 2px 0px;" );
        _css.addToCurrentSection( "  -GtkCalendar-inner-border: 0px;" );

        // toggle button
        _css.addSection( "GtkToggleButton" );
        _css.addToCurrentSection( "  -GtkButton-inner-border: 1px 0px 0px;" );

        // entries
        _css.addSection( "GtkEntry" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "2px 5px" ) );

        #endif

        #if GTK_CHECK_VERSION( 3, 7, 0 )

        // menu
        // since gtk 3.7.0, GtkMenu-horizontal-padding and vertical-padding (from gtk.css) are ignored
        // need to use css padding and border-width instead
        _css.addSection( "GtkMenu" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "5px 3px" ) );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_BORDER_WIDTH, "0px" ) );

        _css.addSection( "GtkMenu>GtkMenuItem" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "5px 4px" ) );

        _css.addSection( "GtkMenu>GtkSeparatorMenuItem" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "1px 4px" ) );

        #else

        // menu padding
        _css.setCurrentSection( Gtk::CSS::defaultSection() );
        _css.addToCurrentSection( "  -GtkMenu-horizontal-padding: 3px;" );
        _css.addToCurrentSection( "  -GtkMenu-vertical-padding: 5px;" );

        _css.addSection( "GtkMenu>GtkMenuItem" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "5px 1px" ) );

        _css.addSection( "GtkMenu>GtkSeparatorMenuItem" );
        _css.addToCurrentSection( Gtk::CSSOption<std::string>( GTK_STYLE_PROPERTY_PADDING, "1px 1px" ) );

        #endif

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
    void QtSettings::monitorFile( const std::string& filename )
    {

        // check if file was already added
        if( _monitoredFiles.find( filename ) != _monitoredFiles.end() )
        { return; }

        // check file existence
        if( !std::ifstream( filename.c_str() ) )
        { return; }

        // create FileMonitor
        FileMonitor monitor;
        monitor.file = g_file_new_for_path( filename.c_str() );
        if( ( monitor.monitor = g_file_monitor( monitor.file, G_FILE_MONITOR_NONE, 0L, 0L ) ) )
        {

            // insert in map
            _monitoredFiles.insert( std::make_pair( filename, monitor ) );

        } else {

            // clear file and return
            g_object_unref( monitor.file );
            return;

        }

    }

    //_________________________________________________________
    void QtSettings::clearMonitoredFiles( void )
    {
        for( FileMap::iterator iter = _monitoredFiles.begin(); iter != _monitoredFiles.end(); iter++ )
        {
            iter->second.signal.disconnect();
            g_object_unref( iter->second.file );
            g_object_unref( iter->second.monitor );
        }

        _monitoredFiles.clear();
    }

}
