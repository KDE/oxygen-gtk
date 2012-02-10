#ifndef oxygenqtsettings_h
#define oxygenqtsettings_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenanimationmodes.h"
#include "oxygenapplicationname.h"
#include "oxygengtkicons.h"
#include "oxygengtkrc.h"
#include "oxygenoption.h"
#include "oxygenoptionmap.h"
#include "oxygenpalette.h"
#include "oxygenshadowconfiguration.h"
#include "oxygenpathlist.h"
#include "oxygenfontinfo.h"
#include "pango/pango.h"

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
        {}

        //! load kdeglobals settings into optionMap
        void loadKdeGlobals( void );

        //! initialization flags
        enum Flags
        {
            AppName = 1<<0,
            Icons = 1<<1,
            Fonts = 1<<2,
            KdeGlobals = 1<<3,
            Oxygen = 1<<4,
            Colors = 1<<5,
            All = AppName|Icons|Fonts|KdeGlobals|Oxygen|Colors,
            Forced = 1<<6
        };

        //! returns user config dir
        std::string userConfigDir( void ) const
        { return _userConfigDir; }

        //! initialize
        void initialize( unsigned int flags = All );

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

        protected:

        //! icon path
        PathList kdeConfigPathList( void ) const;

        //! icon path
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

        //! load fonts from kdeglobals and pass to gtk
        void loadKdeFonts( void );

        //! extra kde globals options
        void loadKdeGlobalsOptions( void );

        //! oxygen options (from oxygenrc)
        void loadOxygenOptions( void );

        // sanitize path
        std::string sanitizePath( const std::string& ) const;

        private:

        //! application
        /*! this is needed to deal with some application hacks */
        ApplicationName _applicationName;

        //! kde global options
        OptionMap _kdeGlobals;

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

        //! menu highlight mode
        MenuHighlightMode _menuHighlightMode;

        //! window drag mode
        bool _windowDragEnabled;

        //! window drag mode
        WindowDragMode _windowDragMode;

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

        //! windeco font
        FontInfo _WMFont;

        //! title alignment
        PangoAlignment _titleAlignment;

        //@}

        //! true if argb is enabled
        bool _argbEnabled;

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

    };

}

#endif
