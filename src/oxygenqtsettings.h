#ifndef oxygenqtsettings_h
#define oxygenqtsettings_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenanimationmodes.h"
#include "oxygenapplicationname.h"
#include "oxygengtkicons.h"
#include "oxygengtkrc.h"
#include "oxygenoption.h"
#include "oxygenoptionmap.h"
#include "oxygenpalette.h"
#include "oxygenshadowconfiguration.h"
#include "oxygensignal.h"
#include "oxygenpathlist.h"
#include "oxygenfontinfo.h"
#include "pango/pango.h"

#include <gio/gio.h>

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Oxygen
{

    class QtSettings
    {

        public:

        //! constructor
        QtSettings( void );

        //! destructor
        virtual ~QtSettings( void )
        { clearMonitoredFiles(); }

        //! initialization flags
        enum Flags
        {
            AppName = 1<<0,
            Icons = 1<<1,
            Fonts = 1<<2,
            KdeGlobals = 1<<3,
            Oxygen = 1<<4,
            Colors = 1<<5,
            Extra = 1<<7,
            All = AppName|Icons|Fonts|KdeGlobals|Oxygen|Colors|Extra,
            Forced = 1<<6
        };

        //! sets whether wm shadows are supported
        bool isWMShadowsSupported( void ) const
        { return _wmShadowsSupported; }

        //! returns user config dir
        std::string userConfigDir( void ) const
        { return _userConfigDir; }

        //! initialize
        bool initialize( unsigned int flags = All );

        //! palette
        const Palette& palette( void ) const
        { return _palette; }

        //! application name
        const ApplicationName& applicationName( void ) const
        { return _applicationName; }

        //!@name oxygen style options
        //@{

        //! use effect to render active (mouse-over) icons
        bool useIconEffect( void ) const
        { return _useIconEffect; }

        //! background gradient
        bool useBackgroundGradient( void ) const
        { return _useBackgroundGradient; }

        //! background pixmap
        const std::string& backgroundPixmap( void ) const
        { return _backgroundPixmap; }

        //! checkbox style
        enum CheckBoxStyle
        {
           CS_CHECK,
           CS_X
        };

        //! checkbox style
        CheckBoxStyle checkBoxStyle( void ) const
        { return _checkBoxStyle; }

        //! tab style
        enum TabStyle
        {
            TS_SINGLE,
            TS_PLAIN
        };

        //! checkbox style
        TabStyle tabStyle( void ) const
        { return _tabStyle; }

        //! 'add-line' buttons
        /*! corresponds to buttons located at the bottom (right) of vertical (horizontal) toolbars */
        int scrollBarAddLineButtons( void ) const
        { return _scrollBarAddLineButtons; }

        //! 'sub-line' buttons
        /*! corresponds to buttons located at the bottom (right) of vertical (horizontal) toolbars */
        int scrollBarSubLineButtons( void ) const
        { return _scrollBarSubLineButtons; }

        //! toolbar item separator
        bool toolBarDrawItemSeparator( void ) const
        { return _toolBarDrawItemSeparator; }

        //! focus indicator in lists (not supported yet)
        bool viewDrawFocusIndicator( void ) const
        { return _viewDrawFocusIndicator; }

        //! tree branch lines
        bool viewDrawTreeBranchLines( void ) const
        { return _viewDrawTreeBranchLines; }

        //! transparent tooltips
        bool tooltipTransparent( void ) const
        { return _tooltipTransparent; }

        //! styled frames in tooltips
        bool tooltipDrawStyledFrames( void ) const
        { return _tooltipDrawStyledFrames; }

        //! arrow size
        enum ArrowSize
        {
            ArrowNormal,
            ArrowSmall,
            ArrowTiny
        };

        //! tree expander style
        bool viewDrawTriangularExpander( void ) const
        { return _viewDrawTriangularExpander; }

        //! tree triangular expander size
        ArrowSize viewTriangularExpanderSize( void ) const
        { return _viewTriangularExpanderSize; }

        //! true if views sort order indicator arrow direction must be inverted
        bool viewInvertSortIndicator( void ) const
        { return _viewInvertSortIndicator; }

        //! menu highlight mode
        enum MenuHighlightMode
        {
            MM_DARK,
            MM_SUBTLE,
            MM_STRONG
        };

        MenuHighlightMode menuHighlightMode( void ) const
        { return _menuHighlightMode; }

        //! window drag mode
        bool windowDragEnabled( void ) const
        { return _windowDragEnabled; }

        //! window drag mode
        enum WindowDragMode
        {
            WD_MINIMAL,
            WD_FULL
        };

        //! window drag mode
        WindowDragMode windowDragMode( void ) const
        { return _windowDragMode; }

        //! use window manager to handle move-resize
        bool useWMMoveResize( void ) const
        { return _useWMMoveResize; }

        //! drag distance
        int startDragDist( void ) const
        { return _startDragDist; }

        //! drag delay
        int startDragTime( void ) const
        { return _startDragTime; }

        //@}

        //!@name animation enable state
        //@{

        //! all animations
        bool animationsEnabled( void ) const
        { return _animationsEnabled; }

        //! generic animations
        bool genericAnimationsEnabled( void ) const
        { return _genericAnimationsEnabled; }

        //@}

        //!@name animations type
        //@{

        AnimationType menuBarAnimationType( void ) const
        { return _menuBarAnimationType; }

        AnimationType menuAnimationType( void ) const
        { return _menuAnimationType; }

        AnimationType toolBarAnimationType( void ) const
        { return _toolBarAnimationType; }

        //@}

        //!@name animations duration
        //@{

        int genericAnimationsDuration( void ) const
        { return _genericAnimationsDuration; }

        int menuBarAnimationsDuration( void ) const
        { return _menuBarAnimationsDuration; }

        int menuBarFollowMouseAnimationsDuration( void ) const
        { return _menuBarFollowMouseAnimationsDuration; }

        int menuAnimationsDuration( void ) const
        { return _menuAnimationsDuration; }

        int menuFollowMouseAnimationsDuration( void ) const
        { return _menuFollowMouseAnimationsDuration; }

        int toolBarAnimationsDuration( void ) const
        { return _toolBarAnimationsDuration; }

        //@}

        //!@name window decoration options
        //@{

        //! button size enumeration
        enum ButtonSize {
            ButtonSmall = 18,
            ButtonDefault = 20,
            ButtonLarge = 24,
            ButtonVeryLarge = 32,
            ButtonHuge = 48
        };

        //! button size
        ButtonSize buttonSize( void ) const
        { return _buttonSize; }

        //! frame border enumeration
        enum FrameBorder {
            BorderNone = 0,
            BorderNoSide = 1,
            BorderTiny = 2,
            BorderDefault = 4,
            BorderLarge = 8,
            BorderVeryLarge = 12,
            BorderHuge = 18,
            BorderVeryHuge = 27,
            BorderOversized = 40
        };

        //! frame border
        FrameBorder frameBorder( void ) const
        { return _frameBorder; }

        enum WindecoBlendType {
            SolidColor,
            RadialGradient,
            FollowStyleHint
        };

        WindecoBlendType windecoBlendType( void ) const
        { return _windecoBlendType; }

        //! shadow configuration
        const ShadowConfiguration& shadowConfiguration( Palette::Group group ) const
        {
            switch( group )
            {
                default:
                case Palette::Inactive: return _inactiveShadowConfiguration;
                case Palette::Active: return _activeShadowConfiguration;
            }
        }

        int backgroundOpacity() const
        {
            return _backgroundOpacity;
        }

        //! windeco font
        const FontInfo& WMFont( void ) const
        { return _WMFont; }

        //! title alignment
        const PangoAlignment TitleAlignment( void ) const
        { return _titleAlignment; }

        //@}

        //! true if argb is enabled
        bool argbEnabled( void ) const
        { return _argbEnabled; }

        //! true if argb is enabled
        bool widgetExplorerEnabled( void ) const
        { return _widgetExplorerEnabled; }

        //! file monitor structure
        class FileMonitor
        {
            public:

            //! constructor
            FileMonitor( void ):
                file( 0L ),
                monitor( 0L )
            {}

            //! gfile pointer
            GFile* file;
            GFileMonitor* monitor;
            Signal signal;
        };

        //! set of monitored files
        typedef std::map<std::string, FileMonitor> FileMap;

        //! file monitors
        FileMap& monitoredFiles( void )
        { return _monitoredFiles; }

        protected:

        //! read output from a command - replacement for not always working g_spawn_command_line_sync()
        bool runCommand( const std::string& command, char*& result ) const;

        //! returns true if a given atom is supported
        bool isAtomSupported( const std::string& ) const;

        //! kdeglobals settings
        /*! returns true if changed */
        bool loadKdeGlobals( void );

        //! oxygen settings
        /*! returns true if changed */
        bool loadOxygen( void );

        //! kde configuration path
        PathList kdeConfigPathList( void ) const;

        //! kde icon path
        PathList kdeIconPathList( void ) const;

        //! add icon theme to path list, accounting for theme inheritance (recursively)
        void addIconTheme( PathList&, const std::string& );

        //! init user config dir
        void initUserConfigDir( void );

        //! init application name
        void initApplicationName( void )
        { _applicationName.initialize(); }

        //! init argb support
        void initArgb( void );

        //! load kde icons
        void loadKdeIcons( void );

        //! default icon search path
        PathSet defaultIconSearchPath( void ) const;

        //! load palette from kdeglobals
        void loadKdePalette( bool forced = false );

        //! update gtk colors
        /*! generates an RC string and pass it to gtk */
        void generateGtkColors( void );

        //! add Html link colors to RC's current section
        void addLinkColors( const std::string& );

        //! load fonts from kdeglobals and pass to gtk
        void loadKdeFonts( void );

        //! extra kde globals options
        void loadKdeGlobalsOptions( void );

        //! oxygen options (from oxygenrc)
        void loadOxygenOptions( void );

        //! extra metrics options
        void loadExtraOptions( void );

        //! css shadows
        void setupCssShadows( const std::string&, bool enabled );

        //! sanitize path
        std::string sanitizePath( const std::string& ) const;

        //! monitor file
        void monitorFile( const std::string& );

        //! clear monitored files
        void clearMonitoredFiles( void );

        private:

        //! application
        /*! this is needed to deal with some application hacks */
        ApplicationName _applicationName;

        //! true if window manager shadows are supported
        /*! if true, disable CSD shadows */
        bool _wmShadowsSupported;

        //! kde global options
        OptionMap _kdeGlobals;

        //! oxygen options
        OptionMap _oxygen;

        //! user config directory
        std::string _userConfigDir;

        //!@name icons
        //@{

        //! icon theme
        /*! todo: respect user settings here */
        std::string _kdeIconTheme;

        //! fallback icon theme
        /*! todo: respect user settings here */
        std::string _kdeFallbackIconTheme;

        //! loaded icon themes
        std::set<std::string> _iconThemes;

        //@}

        //! config path
        PathList _kdeConfigPathList;

        //! icon path
        PathList _kdeIconPathList;

        //! default icon path
        static const std::string _defaultKdeIconPath;

        //! palette
        Palette _palette;

        //!@name kde/oxygen style options
        //@{

        //! if true, inactive selection has different color from active
        bool _inactiveChangeSelectionColor;

        //! active icon effect
        bool _useIconEffect;

        //! background gradient
        bool _useBackgroundGradient;

        //! background pixmap
        std::string _backgroundPixmap;

        //! checkbox style
        CheckBoxStyle _checkBoxStyle;

        //! checkbox style
        TabStyle _tabStyle;

        //! 'add-line' buttons
        /*! corresponds to buttons located at the bottom (right) of vertical (horizontal) toolbars */
        int _scrollBarAddLineButtons;

        //! 'sub-line' buttons
        /*! corresponds to buttons located at the bottom (right) of vertical (horizontal) toolbars */
        int _scrollBarSubLineButtons;

        //! item separator
        bool _toolBarDrawItemSeparator;

        //! transparent tooltips
        bool _tooltipTransparent;

        //! framed tooltips
        bool _tooltipDrawStyledFrames;

        //! focus indicator in views
        bool _viewDrawFocusIndicator;

        //! true if tree lines should be drawn
        bool _viewDrawTreeBranchLines;

        //! true if arrows are used for expanders
        bool _viewDrawTriangularExpander;

        //! triangular expander size
        ArrowSize _viewTriangularExpanderSize;

        //! true if views sort order indicator arrow direction must be inverted
        bool _viewInvertSortIndicator;

        //! menu highlight mode
        MenuHighlightMode _menuHighlightMode;

        //! window drag mode
        bool _windowDragEnabled;

        //! window drag mode
        WindowDragMode _windowDragMode;

        //! use window manager to handle window grab
        bool _useWMMoveResize;

        //! drag distance
        int _startDragDist;

        //! drag delay
        int _startDragTime;

        //@}

        //!@name animation options
        //@{

        //! all animations
        bool _animationsEnabled;

        //! generic animations
        bool _genericAnimationsEnabled;

        //! menubar animations
        AnimationType _menuBarAnimationType;

        //! menu animations
        AnimationType _menuAnimationType;

        //! toolbar animation type
        AnimationType _toolBarAnimationType;

        //! generic animations
        int _genericAnimationsDuration;

        //! menubar animations
        int _menuBarAnimationsDuration;

        //! follow mouse animation
        int _menuBarFollowMouseAnimationsDuration;

        //! menu animation
        int _menuAnimationsDuration;

        //! follow mouse animation
        int _menuFollowMouseAnimationsDuration;

        //! toolbar (follow-mouse) animation
        int _toolBarAnimationsDuration;

        //@}

        //!@name window decoration options
        //@{

        //! button size
        ButtonSize _buttonSize;

        //! frame border
        FrameBorder _frameBorder;

        //! Window decoration blend type
        WindecoBlendType _windecoBlendType;

        //! active shadows
        ShadowConfiguration _activeShadowConfiguration;

        //! inactive shadows
        ShadowConfiguration _inactiveShadowConfiguration;

        //! oxygen-transparent background opacity
        int _backgroundOpacity;

        //! windeco font
        FontInfo _WMFont;

        //! title alignment
        PangoAlignment _titleAlignment;

        //@}

        //! true if argb is enabled
        bool _argbEnabled;

        //! widget explorer
        bool _widgetExplorerEnabled;

        //! initialization flags
        bool _initialized;
        bool _kdeColorsInitialized;
        bool _gtkColorsInitialized;

        //! KDE running flags
        bool _KDESession;

        //! gtk icons generator
        GtkIcons _icons;

        //! gtkrc
        Gtk::RC _rc;

        //! file monitors
        FileMap _monitoredFiles;

    };

}

#endif
