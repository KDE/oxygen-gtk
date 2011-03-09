#ifndef oxygenstyle_h
#define oxygenstyle_h
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

#include "oxygenanimations.h"
#include "oxygenanimationdata.h"
#include "oxygenanimationmodes.h"
#include "oxygenargbhelper.h"
#include "oxygencairocontext.h"
#include "oxygengeometry.h"
#include "oxygengtkcellinfo.h"
#include "oxygengtkgap.h"
#include "oxygenqtsettings.h"
#include "oxygenstylehelper.h"
#include "oxygenstyleoptions.h"
#include "oxygentaboptions.h"
#include "oxygentileset.h"
#include "oxygenwidgetlookup.h"
#include "oxygenwindecooptions.h"
#include "oxygenwindecobutton.h"
#include "oxygenwindowmanager.h"
#include <gdk/gdk.h>

namespace Oxygen
{

    //! handle all plainting
    class Style
    {

        public:

        //! singleton
        static Style& instance( void );

        //! destructor
        virtual ~Style( void )
        {}

        //! initialize
        void initialize( unsigned int flags = QtSettings::All );

        //! settings
        const QtSettings& settings( void ) const
        { return _settings; }

        //! settings
        QtSettings& settings( void )
        { return _settings; }

        //! helper
        StyleHelper& helper( void )
        { return _helper; }

        //! helper
        const StyleHelper& helper( void ) const
        { return _helper; }

        //! animations
        const Animations& animations( void ) const
        { return _animations; }

        //! animations
        Animations& animations( void )
        { return _animations; }

        //! argb helper
        ArgbHelper& argbHelper( void )
        { return _argbHelper; }

        //! window manager
        WindowManager& windowManager( void )
        { return _windowManager; }

        //! widget lookup
        WidgetLookup& widgetLookup( void )
        { return _widgetLookup; }

        //! return tabCloseButton for given set of options
        Cairo::Surface tabCloseButton( const StyleOptions& );

        //!@name primitives
        //@{

        //! fill given rectangle with flat color
        void fill( cairo_t* context, gint x, gint y, gint w, gint h, Palette::Role role ) const
        { fill( context, x, y, w, h, settings().palette().color( Palette::Active, role ) ); }

        //! fill given rectangle with flat color
        void fill( cairo_t* context, gint x, gint y, gint w, gint h, Palette::Group group, Palette::Role role ) const
        { fill( context, x, y, w, h, settings().palette().color( group, role ) ); }

        //! fill given rectangle with flat color
        void fill( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba& color ) const;

        //! outline given rectangle with flat color
        void outline( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba& color = ColorUtils::Rgba( 1, 0, 0 ) ) const;

        //! separators
        void drawSeparator( GtkWidget*, cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        void drawSeparator( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options )
        { drawSeparator( 0L, context, x, y, w, h, options ); }

        //! window background
        /*! returns true if window gradient could be rendered */
        void renderWindowBackground( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, const StyleOptions& = StyleOptions(), TileSet::Tiles = TileSet::Center );

        void renderWindowBackground( cairo_t* context, GdkWindow* window, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions(), TileSet::Tiles tiles= TileSet::Center)
        { renderWindowBackground( context, window, 0L, x, y, w, h, o, tiles ); }

//         void renderWindowBackground( GdkWindow* window, GtkWidget* widget, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions(), TileSet::Tiles tiles= TileSet::Center )
//         { renderWindowBackground( 0L, window, widget, r, x, y, w, h, o, tiles ); }

//         void renderWindowBackground( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions(), TileSet::Tiles tiles= TileSet::Center)
//         { renderWindowBackground( window, 0L, r, x, y, w, h, o, tiles ); }

        //! window background
        bool renderMenuBackground( cairo_t*, GdkWindow*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! tooltip background
        void renderTooltipBackground( cairo_t*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! tree view header
        void renderHeaderBackground( cairo_t*, GdkWindow*, gint, gint, gint, gint );

        //! tree view header
        void renderHeaderLines( cairo_t*, gint, gint, gint, gint ) const;

        //! tree view lines
        void renderTreeLines( cairo_t*, gint, gint, gint, gint, const Gtk::CellInfoFlags&, const StyleOptions& ) const;

        //! editors hole
        /*! returns true if window gradient could be rendered */
        void renderHoleBackground( cairo_t*, GdkWindow*, gint, gint, gint, gint, TileSet::Tiles = TileSet::Ring );

        //! splitters
        void renderSplitter( cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //!@name progressbar
        //@{
        void renderProgressBarHole( cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        void renderProgressBarHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        //@}

        //!@name scrollbar
        //@{
        void renderScrollBarHole( cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        void renderScrollBarHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() ) const;
        //@}

        //! toolbar handle
        void renderToolBarHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! frame
        void drawFloatFrame( cairo_t*, gint, gint, gint, gint, const StyleOptions&, Palette::Role = Palette::Window ) const;

        //!@name button slab
        //@{
        void renderButtonSlab( GtkWidget*, cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData&, TileSet::Tiles = TileSet::Ring );

        void renderButtonSlab( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options, const AnimationData& data, TileSet::Tiles tiles = TileSet::Ring )
        { renderButtonSlab( 0L, context, x, y, w, h, options, data, tiles ); }

        void renderButtonSlab( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options, TileSet::Tiles tiles = TileSet::Ring )
        { renderButtonSlab( 0L, context, x, y, w, h, options, AnimationData(), tiles ); }
        //@}

        //! checkbox
        /*! shadow type is used to decide whether check is on/off or tristate */
        void renderCheckBox( cairo_t*, gint, gint, gint, gint, GtkShadowType, const StyleOptions&, const AnimationData& = AnimationData() );

        //! radio button
        void renderRadioButton( cairo_t*, gint, gint, gint, gint, GtkShadowType, const StyleOptions&, const AnimationData& = AnimationData() );

        //!@name generic slab
        //@{

        void renderSlab( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions&, const AnimationData& = AnimationData() );
        void renderSlab( cairo_t* context, int x, gint y, gint w, gint h, const StyleOptions& options, const AnimationData& animationData = AnimationData() )
        { renderSlab( context, x, y, w, h, Gtk::Gap(), options, animationData ); }

        //@}

        //! info bar
        void renderInfoBar( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba& );

        //!@name hole
        //@{

        void renderHole( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions&, const AnimationData& = AnimationData(), TileSet::Tiles = TileSet::Ring );

        void renderHole( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& o, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( context, x, y, w, h, Gtk::Gap(), o, AnimationData(), tiles ); }

        void renderHole( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& o, const AnimationData& animationData, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( context, x, y, w, h, Gtk::Gap(), o, animationData, tiles ); }

        //@}

        //!@name dock frame
        //@{

        void renderDockFrame( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions& );
        void renderDockFrame( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& o )
        { renderDockFrame( context, x, y, w, h, Gtk::Gap(), o ); }

        //@}

        //! menu item
        void renderMenuItemRect( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );

        //! selection
        void renderSelection( cairo_t*, gint, gint, gint, gint, TileSet::Tiles tiles, const StyleOptions& );

        //! arrow
        void renderArrow( cairo_t*, GtkArrowType, gint, gint, gint, gint, QtSettings::ArrowSize, const StyleOptions&, const AnimationData&, Palette::Role ) const;
        void renderArrow( cairo_t* context, GtkArrowType type, gint x, gint y, gint w, gint h, QtSettings::ArrowSize size = QtSettings::ArrowNormal, const StyleOptions& options = Contrast, Palette::Role role = Palette::ButtonText ) const
        { renderArrow( context, type, x, y, w, h, size, options, AnimationData(), role ); }

        //! slider groove
        void renderSliderGroove( cairo_t*, gint, gint, gint, gint, const StyleOptions& );

        //! slider handle
        void renderSliderHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );

        //! size grip
        void renderSizeGrip( cairo_t*, GdkWindowEdge, gint, gint, gint, gint ) const;

        //! tab
        void renderTab(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& = AnimationData() );

        //! tabbar base
        void renderTabBarBase(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType, Gtk::Gap,
            const StyleOptions&,
            const TabOptions& );

        //! tabwidget frame
        void renderTabBarFrame( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions& );

        //! tree 'standard' expanders (that is: +!-)
        void renderTreeExpander( cairo_t*, gint, gint, gint, gint, GtkExpanderStyle, const StyleOptions&, const AnimationData&, Palette::Role ) const;
        void renderTreeExpander( cairo_t* context, gint x, gint y, gint w, gint h, GtkExpanderStyle style, const StyleOptions& options, Palette::Role role ) const
        { renderTreeExpander( context, x, y, w, h, style, options, AnimationData(), role ); }

        //@}

        //! draw window decorations
        void drawWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint, gchar**, gint, gint);

        //! internal windeco renderer
        void renderWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint, gchar**, gint, gint, bool = true);

        //! draw window shadow
        void drawWindowShadow( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h );

        //! render XShape window decoration mask
        void drawWindecoShapeMask( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h );

        //! draw windeco button
        void drawWindecoButton(cairo_t*,WinDeco::ButtonType,WinDeco::ButtonStatus,WinDeco::Options, gint,gint,gint,gint);

        // adjust scrollbar hole, depending on orientation and buttons settings
        void adjustScrollBarHole( int& x, int& y, int& w, int& h, const StyleOptions& ) const;

        //! sanitize size
        void sanitizeSize( GdkWindow* window, gint& width, gint& height ) const;

        //! metrics
        /*! these are copied from the old KStyle WidgetProperties */
        enum Metrics
        {

            /*
            checkbox. Do not change, unless
            changing the actual cached pixmap size
            */
            CheckBox_Size = 21,

            // slider groove height
            Slider_GrooveWidth = 5,

            // menu vertical offset
            Menu_VerticalOffset = 1,

            /*
            entries size margins. Do not change, unless
            changing the corresponding oxygen-entry-margins style
            in gtkrc
            */
            Entry_SideMargin = 3

        };

        // get tiles for given tab orientation
        TileSet::Tiles tabTiles( GtkPositionType position ) const
        {

            TileSet::Tiles out( TileSet::Ring );
            switch( position )
            {
                case GTK_POS_BOTTOM: out &= ~TileSet::Bottom; break;
                case GTK_POS_TOP: out &= ~TileSet::Top; break;
                case GTK_POS_LEFT: out &= ~TileSet::Left; break;
                case GTK_POS_RIGHT: out &= ~TileSet::Right; break;
                default: break;
            }

            return out;
        }

        protected:

        //! constructor
        explicit Style( void )
        {}

        //! get color matching role from either style option or default palette
        const ColorUtils::Rgba& color( Palette::Role role, const StyleOptions& option ) const
        { return color( Palette::Active, role, option ); }

        //! get color matching group and role from either style option or default palette
        const ColorUtils::Rgba& color( Palette::Group group, Palette::Role role, const StyleOptions& option ) const
        {
            Palette::ColorSet::const_iterator iter( option._customColors.find( role ) );
            return iter == option._customColors.end() ? settings().palette().color( group, role ) : iter->second;
        }

        //@name internal rendering
        //@{

        //! tab
        void renderActiveTab(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions& );

        //! tab
        void renderInactiveTab_Plain(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& );

        //! tab
        void renderInactiveTab_Single(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& );

        //! slab glowing color
        ColorUtils::Rgba slabShadowColor( const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //! hole glowing color
        ColorUtils::Rgba holeShadowColor( const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //! slab
        void renderSlab( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba&, const StyleOptions&, const AnimationData& = AnimationData(), TileSet::Tiles tiles = TileSet::Ring );

        //! progressbar hole (groove)
        /*! also used for progress bars */
        void renderScrollBarHole( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba&, bool vertical, TileSet::Tiles tiles = TileSet::Full );

        //! returns point position for generic arrows
        Polygon genericArrow( GtkArrowType, QtSettings::ArrowSize = QtSettings::ArrowNormal ) const;

        //@}

        //!@name window decoration rendering
        //@{

        //! draw resize handles for window decoration
        void renderWindowDots(cairo_t*, gint x, gint y, gint w, gint h, const ColorUtils::Rgba&, WinDeco::Options);

        //@}

        //!@name utilities
        //@{

        // center rect
        void centerRect( GdkRectangle*, GdkRectangle* ) const;

        // generate map from gap
        void generateGapMask( cairo_t*, gint, gint, gint, gint, const Gtk::Gap& ) const;

        //@}

        //! used to store slab characteristics
        class SlabRect
        {
            public:

            //! constructor
            explicit SlabRect(void):
                _x(0),
                _y(0),
                _w(-1),
                _h(-1),
                _tiles( TileSet::Ring )
            {}

            //! constructor
            explicit SlabRect( int x, int y, int w, int h, const TileSet::Tiles& tiles, const StyleOptions& options = StyleOptions() ):
                _x(x),
                _y(y),
                _w(w),
                _h(h),
                _tiles( TileSet::Tiles( tiles ) ),
                _options( options )
            {}

            int _x;
            int _y;
            int _w;
            int _h;
            TileSet::Tiles _tiles;
            StyleOptions _options;

            typedef std::vector<SlabRect> List;

        };

        private:

        // Qt settings
        QtSettings _settings;

        // helper
        StyleHelper _helper;

        // animations
        Animations _animations;

        //! argb helper
        ArgbHelper _argbHelper;

        //! window manager
        WindowManager _windowManager;

        //! widget lookup
        WidgetLookup _widgetLookup;

        //! Tab close buttons
        class TabCloseButtons
        {
            public:

            //! constructor
            TabCloseButtons( void ):
                normal(0L),
                active(0L),
                inactive(0L),
                prelight(0L)
            {}

            //! destructor
            virtual ~TabCloseButtons( void )
            {}

            Cairo::Surface normal;
            Cairo::Surface active;
            Cairo::Surface inactive;
            Cairo::Surface prelight;
        };

        //! Tab close buttons
        TabCloseButtons _tabCloseButtons;

        //! singleton
        static Style* _instance;

    };

}

#endif
