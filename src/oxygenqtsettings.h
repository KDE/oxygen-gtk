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

#include "oxygenapplicationname.h"
#include "oxygengtkicons.h"
#include "oxygengtkrc.h"
#include "oxygenoption.h"
#include "oxygenoptionmap.h"
#include "oxygenpalette.h"
#include "oxygenpathlist.h"

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

        //! returns user config dir
        std::string userConfigDir( void ) const
        { return _userConfigDir; }

        //! initialize
        void initialize( void );

        //! palette
        const Palette& palette( void ) const
        { return _palette; }

        //! application name
        const ApplicationName& applicationName( void ) const
        { return _applicationName; }

        //!@name oxygen style options
        //@{

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

        //! true if hovered scrollbars must be colored
        bool scrollBarColored( void ) const
        { return _scrollBarColored; }

        //! true if hovered scrollbars must be colored
        bool scrollBarBevel( void ) const
        { return _scrollBarBevel; }

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

        //@}

        //! true if argb is enabled
        bool argbEnabled( void ) const
        { return _argbEnabled; }

        protected:

        // get home directory
        std::string home( void ) const;

        //! icon path
        PathList kdeConfigPathList( void ) const;

        //! icon path
        PathList kdeIconPathList( void ) const;

        //! add icon theme to path list, accounting for theme inheritance (recursively)
        void addIconTheme( PathList&, const std::string& );

        //! init user config dir
        void initUserConfigDir( void );

        //! init application name
        void initApplicationName( void );

        //! init argb support
        void initArgb( void );

        //! load kde icons
        void loadKdeIcons( void );

        //! load palette from kdeglobals
        void loadKdePalette( void );

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

        // read all options from kdeglobals
        OptionMap readOptions( const std::string& ) const;

        private:

        //! application
        /*! this is needed to deal with some application hacks */
        ApplicationName _applicationName;

        //! kde global options
        OptionMap _kdeGlobals;

        //! kde oxygen options
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

        //! checkbox style
        CheckBoxStyle _checkBoxStyle;

        //! checkbox style
        TabStyle _tabStyle;

        //! colored scrollbar (on hover)
        bool _scrollBarColored;

        //! scrollbar bevel
        bool _scrollBarBevel;

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

        //!@name window decoration options
        //@{

        //! button size
        ButtonSize _buttonSize;

        //! frame border
        FrameBorder _frameBorder;

        //@}

        //! true if argb is enabled
        bool _argbEnabled;

        //! initialization flags
        bool _initialized;
        bool _kdeColorsInitialized;
        bool _gtkColorsInitialized;

        //! gtk icons generator
        GtkIcons _icons;

        //! rc options (passed to gtk at the end of init
        Gtk::RC _rc;

    };

}

#endif
