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
#include "oxygenmetrics.h"
#include "oxygenqtsettings.h"
#include "oxygenshadowhelper.h"
#include "oxygenstylehelper.h"
#include "oxygenstyleoptions.h"
#include "oxygentaboptions.h"
#include "oxygentileset.h"
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
        {
            if( _instance == this )
            { _instance = 0L; }
        }

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

        //! shadow helper
        ShadowHelper& shadowHelper( void )
        { return _shadowHelper; }

        //! window manager
        WindowManager& windowManager( void )
        { return _windowManager; }

        //! return tabCloseButton for given set of options
        Cairo::Surface tabCloseButton( const StyleOptions& );

        //! background surface
        bool hasBackgroundSurface( void ) const;

        //!@name primitives
        //@{

        //! fill given rectangle with flat color
        void fill( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, Palette::Role role ) const
        { fill( window, r, x, y, w, h, settings().palette().color( Palette::Active, role ) ); }

        //! fill given rectangle with flat color
        void fill( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, Palette::Group group, Palette::Role role ) const
        { fill( window, r, x, y, w, h, settings().palette().color( group, role ) ); }

        //! fill given rectangle with flat color
        void fill( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const ColorUtils::Rgba& color ) const;

        //! outline given rectangle with flat color
        void outline( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const ColorUtils::Rgba& color = ColorUtils::Rgba( 1, 0, 0 ) ) const;

        //! separators
        void drawSeparator( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& );

        //! window background
        /*! returns true if window gradient could be rendered */
        bool renderWindowBackground( cairo_t*, GdkWindow*, GtkWidget*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& = StyleOptions(), TileSet::Tiles = TileSet::Center );

        bool renderWindowBackground( GdkWindow* window, GtkWidget* widget, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions(), TileSet::Tiles tiles= TileSet::Center )
        { return renderWindowBackground( 0L, window, widget, r, x, y, w, h, o, tiles ); }

        bool renderWindowBackground( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions(), TileSet::Tiles tiles = TileSet::Center)
        { return renderWindowBackground( window, 0L, r, x, y, w, h, o, tiles ); }

        //! groupbox background
        bool renderGroupBoxBackground( GdkWindow* window, GtkWidget* widget, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o, TileSet::Tiles tiles = TileSet::Center )
        { return renderGroupBoxBackground( 0L, window, widget, r, x, y, w, h, o, tiles ); }

        bool renderGroupBoxBackground( cairo_t*, GdkWindow*, GtkWidget*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, TileSet::Tiles = TileSet::Center );

        //! menu background
        bool renderMenuBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! tooltip background
        void renderTooltipBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! tree view header
        void renderHeaderBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint );

        //! tree view header
        void renderHeaderLines( GdkWindow*, GdkRectangle*, gint, gint, gint, gint ) const;

        //! tree view lines
        void renderTreeLines( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::CellInfoFlags&, const StyleOptions& ) const;

        //! editors hole
        /*! returns true if window gradient could be rendered */
        void renderHoleBackground( GdkWindow*, GtkWidget*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, TileSet::Tiles = TileSet::Ring, gint = Entry_SideMargin );
        void renderHoleBackground( GdkWindow* window, GtkWidget* widget, GdkRectangle* clip, gint x, gint y, gint w, gint h, TileSet::Tiles tiles = TileSet::Ring, gint margin = Entry_SideMargin )
        { renderHoleBackground( window, widget, clip, x, y, w, h, StyleOptions(), tiles, margin ); }

        //! splitters
        void renderSplitter( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //!@name progressbar
        //@{
        void renderProgressBarHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& );
        void renderProgressBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& );
        //@}

        //!@name scrollbar
        //@{
        void renderScrollBarHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& );
        void renderScrollBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );
        //@}

        //! toolbar handle
        void renderToolBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! frame
        void drawFloatFrame( cairo_t* context, GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, Palette::Role = Palette::Window ) const;
        void drawFloatFrame( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& opt, Palette::Role role = Palette::Window) const
        { drawFloatFrame( 0L, window, r, x, y, w, h, opt, role ); }

        //! button slab
        void renderButtonSlab( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const StyleOptions& options, TileSet::Tiles tiles = TileSet::Ring )
        { renderButtonSlab( window, clipRect, x, y, w, h, options, AnimationData(), tiles ); }

        void renderButtonSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, const AnimationData&, TileSet::Tiles = TileSet::Ring );

        //! checkbox
        /*! shadow type is used to decide whether check is on/off or tristate */
        void renderCheckBox( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkShadowType, const StyleOptions&, const AnimationData& = AnimationData() );

        //! radio button
        void renderRadioButton( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkShadowType, const StyleOptions&, const AnimationData& = AnimationData() );

        //!@name generic slab
        //@{

        void renderSlab( GdkWindow* window, GdkRectangle* clipRect, int x, gint y, gint w, gint h, const StyleOptions& options, const AnimationData& animationData = AnimationData() )
        { renderSlab( window, clipRect, x, y, w, h, Gtk::Gap(), options, animationData ); }

        void renderSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions&, const AnimationData& = AnimationData() );

        //@}

        //! info bar
        void renderInfoBar( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const ColorUtils::Rgba& );

        //!@name hole
        //@{

        void renderHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions&, const AnimationData& = AnimationData(), TileSet::Tiles = TileSet::Ring );

        void renderHole( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( window, r, x, y, w, h, Gtk::Gap(), o, AnimationData(), tiles ); }

        void renderHole( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o, const AnimationData& animationData, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( window, r, x, y, w, h, Gtk::Gap(), o, animationData, tiles ); }

        //@}

        //!@name dock frame
        //@{
        void renderDockFrame( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions& );
        void renderDockFrame( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions& o )
        { renderDockFrame( window, r, x, y, w, h, Gtk::Gap(), o ); }
        //@}

        //! groupbox frame
        void renderGroupBoxFrame( GdkWindow* window, GtkWidget*, GdkRectangle* r, gint x, gint y, gint w, gint h, const StyleOptions&  );

        //! menu item
        void renderMenuItemRect( GdkWindow*, GdkRectangle*, GtkWidget*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );

        //! selection
        void renderSelection( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, TileSet::Tiles tiles, const StyleOptions& );

        //! arrow
        void renderArrow( GdkWindow*, GdkRectangle*, GtkArrowType, gint, gint, gint, gint, QtSettings::ArrowSize, const StyleOptions&, const AnimationData&, Palette::Role ) const;
        void renderArrow( GdkWindow* window, GdkRectangle* clipRect, GtkArrowType type, gint x, gint y, gint w, gint h, QtSettings::ArrowSize size = QtSettings::ArrowNormal, const StyleOptions& options = Contrast, Palette::Role role = Palette::ButtonText ) const
        { renderArrow( window, clipRect, type, x, y, w, h, size, options, AnimationData(), role ); }

        //! slider groove
        void renderSliderGroove( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions& );

        //! slider handle
        void renderSliderHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );

        //! size grip
        void renderSizeGrip( GdkWindow*, GdkRectangle*, GdkWindowEdge, gint, gint, gint, gint ) const;

        //! tab
        void renderTab(
            GdkWindow*, GdkRectangle*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& = AnimationData() );

        //! tabbar base
        void renderTabBarBase(
            GdkWindow*, GdkRectangle*,
            gint, gint, gint, gint,
            GtkPositionType, Gtk::Gap,
            const StyleOptions&,
            const TabOptions& );

        //! tabwidget frame
        void renderTabBarFrame( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions& );

        //! tree 'standard' expanders (that is: +!-)
        void renderTreeExpander( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkExpanderStyle, const StyleOptions&, const AnimationData&, Palette::Role ) const;
        void renderTreeExpander( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, GtkExpanderStyle style, const StyleOptions& options, Palette::Role role ) const
        { renderTreeExpander( window, clipRect, x, y, w, h, style, options, AnimationData(), role ); }

        //@}

        //! draw window decorations
        void drawWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint, const gchar**, gint, gint);

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

        //! set background surface
        void setBackgroundSurface( const std::string& );

        //@name internal rendering
        //@{

        //! tab
        void renderActiveTab(
            GdkWindow*, GdkRectangle*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions& );

        //! tab
        void renderInactiveTab_Plain(
            GdkWindow*, GdkRectangle*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& );

        //! tab
        void renderInactiveTab_Single(
            GdkWindow*, GdkRectangle*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& );

        //! slab glowing color
        ColorUtils::Rgba slabShadowColor( const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //! hole glowing color
        ColorUtils::Rgba holeShadowColor( const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //! groupbox
        void renderGroupBox( cairo_t*, const ColorUtils::Rgba&, gint, gint, gint, gint, const StyleOptions& = StyleOptions() );

        //! slab
        void renderSlab( Cairo::Context&, gint, gint, gint, gint, const ColorUtils::Rgba&, const StyleOptions&, const AnimationData& = AnimationData(), TileSet::Tiles tiles = TileSet::Ring );

        //! progressbar hole (groove)
        /*! also used for progress bars */
        void renderScrollBarHole( Cairo::Context&, gint, gint, gint, gint, const ColorUtils::Rgba&, bool vertical, TileSet::Tiles = TileSet::Full );

        //! add hole mask to context
        void renderHoleMask( cairo_t*, gint, gint, gint, gint, TileSet::Tiles, gint );

        //! returns point position for generic arrows
        Polygon genericArrow( GtkArrowType, QtSettings::ArrowSize = QtSettings::ArrowNormal ) const;

        //@}

        //!@name window decoration rendering
        //@{

        //! draw resize handles for window decoration
        void renderWindowDots(cairo_t*, gint x, gint y, gint w, gint h, const ColorUtils::Rgba&, WinDeco::Options);

        //! internal windeco renderer
        void renderWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint, const gchar**, gint, gint, bool = true);

        //@}

        //!@name utilities
        //@{

        // center rect
        void centerRect( GdkRectangle*, GdkRectangle* ) const;

        // generate map from gap
        void generateGapMask( Cairo::Context&, gint, gint, gint, gint, const Gtk::Gap& ) const;

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

        //! shadow helper
        ShadowHelper _shadowHelper;

        //! window manager
        WindowManager _windowManager;

        //! background surface
        Cairo::Surface _backgroundSurface;

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
