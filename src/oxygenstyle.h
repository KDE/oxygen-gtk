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
#include "oxygencairocontext.h"
#include "oxygengeometry.h"
#include "oxygengtkcellinfo.h"
#include "oxygengtkgap.h"
#include "oxygenqtsettings.h"
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

        //! return singleton
        static Style& instance( void );

        //! destructor
        virtual ~Style( void )
        { _instance = 0L; }

        //! initialize
        void initialize( void );

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

        //! window manager
        WindowManager& windowManager( void )
        { return _windowManager; }

        //!@name primitives
        //@{

        //! fill given rectangle with flat color
        void fill( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const ColorUtils::Rgba& color ) const;

        //! separators
        void drawSeparator( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions );

        //! window background
        /*! returns true if window gradient could be rendered */
        bool renderWindowBackground( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h ) const
        { return renderWindowBackground( window, 0L, r, x, y, w, h ); }
        bool renderWindowBackground( cairo_t*, GdkWindow*, GtkWidget*, GdkRectangle*, gint, gint, gint, gint ) const;
        bool renderWindowBackground( GdkWindow* window, GtkWidget* widget, GdkRectangle* r, gint x, gint y, gint w, gint h ) const
        { return renderWindowBackground( 0L,window,widget,r,x,y,w,h ); }

        //! window background
        void renderMenuBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! tooltip background
        void renderTooltipBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! tree view header
        void renderHeaderBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint ) const;

        //! tree view header
        void renderHeaderLines( GdkWindow*, GdkRectangle*, gint, gint, gint, gint ) const;

        //! tree view lines
        void renderTreeLines( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::CellInfoFlags&, StyleOptions ) const;

        //! editors hole
        /*! returns true if window gradient could be rendered */
        bool renderHoleBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, TileSet::Tiles = TileSet::Ring ) const;

        //! splitters
        void renderSplitter( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //!@name progressbar
        //@{
        void renderProgressBarHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions );
        void renderProgressBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions );
        //@}

        //!@name scrollbar
        //@{
        void renderScrollBarHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions );
        void renderScrollBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;
        //@}

        //! toolbar handle
        void renderToolBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! frame
        void drawFloatFrame( cairo_t* context, GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;
        void drawFloatFrame( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions opt) const
        { drawFloatFrame( 0L, window, r, x, y, w, h, opt ); }

        //! button slab
        // void renderButtonSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions, TileSet::Tiles = TileSet::Ring );
        void renderButtonSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions, TileSet::Tiles = TileSet::Ring, const ColorUtils::Rgba& = ColorUtils::Rgba() );

        //! checkbox
        /*! shadow type is used to decide whether check is on/off or tristate */
        void renderCheckBox( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkShadowType, StyleOptions );

        //! radio button
        void renderRadioButton( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkShadowType, StyleOptions );

        //!@name generic slab
        //@{

        void renderSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions );
        void renderSlab( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o )
        { renderSlab( window, r, x, y, w, h, Gtk::Gap(), o ); }

        //@}

        //! info bar
        void renderInfoBar( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const ColorUtils::Rgba& );

        //!@name hole
        //@{

        void renderHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions, TileSet::Tiles = TileSet::Ring );
        void renderHole( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( window, r, x, y, w, h, Gtk::Gap(), o, tiles ); }

        //@}

        //!@name dock frame
        //@{
        void renderDockFrame( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions );
        void renderDockFrame( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o )
        { renderDockFrame( window, r, x, y, w, h, Gtk::Gap(), o ); }
        //@}

        //! menu item
        void renderMenuItemRect( GdkWindow*, GdkRectangle*, GtkWidget*, gint, gint, gint, gint, StyleOptions );

        //! selection
        void renderSelection( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, TileSet::Tiles tiles, StyleOptions );

        //! arrow
        void renderArrow( GdkWindow*, GdkRectangle*, GtkArrowType, gint, gint, gint, gint, QtSettings::ArrowSize = QtSettings::ArrowNormal, StyleOptions = Contrast ) const;

        //! slider groove
        void renderSliderGroove( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions );

        //! slider handle
        void renderSliderHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions );

        //! size grip
        void renderSizeGrip( GdkWindow*, GdkRectangle*, GdkWindowEdge, gint, gint, gint, gint ) const;

        //! tab
        void renderTab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkPositionType, StyleOptions, TabOptions );

        //! tabbar base
        void renderTabBarBase( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkPositionType, Gtk::Gap, StyleOptions, TabOptions );

        //! tabwidget frame
        void renderTabBarFrame( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions );

        //! tree 'standard' expanders (that is: +!-)
        void renderTreeExpander( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkExpanderStyle, StyleOptions ) const;

        //@}

        //! draw window decorations
        void drawWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint);

        //! render XShape window decoration mask
        void drawWindecoShapeMask( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h );

    	//! draw windeco button
	    void drawWindecoButton(cairo_t*,WinDeco::ButtonType,WinDeco::ButtonStatus,WinDeco::Options, gint,gint,gint,gint);

        // adjust scrollbar hole, depending on orientation and buttons settings
        void adjustScrollBarHole( int& x, int& y, int& w, int& h, StyleOptions ) const;

        //! sanitize size
        void sanitizeSize( GdkWindow* window, gint& width, gint& height ) const;

        //! metrics
        /*! these are copied from the old KStyle WidgetProperties */
        enum Metrics
        {

            // checkbox. Do not change, unless
            // changing the actual cached pixmap size
            CheckBox_Size = 21,

            // slider groove height
            Slider_GrooveWidth = 5,

            // menu vertical offset
            Menu_VerticalOffset = 1

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
        explicit Style( void );

        //@name internal rendering
        //@{

        //! tab
        void renderActiveTab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkPositionType, StyleOptions, TabOptions );

        //! tab
        void renderInactiveTab_Plain( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkPositionType, StyleOptions, TabOptions );

        //! tab
        void renderInactiveTab_Single( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkPositionType, StyleOptions, TabOptions );

        //! slab glowing color
        ColorUtils::Rgba slabShadowColor( StyleOptions ) const;

        //! slab
        void renderSlab( Cairo::Context&, gint, gint, gint, gint, const ColorUtils::Rgba&, StyleOptions, TileSet::Tiles tiles = TileSet::Ring );

        //! progressbar hole (groove)
        /*! also used for progress bars */
        void renderScrollBarHole( Cairo::Context&, gint, gint, gint, gint, const ColorUtils::Rgba&, bool vertical, TileSet::Tiles tiles = TileSet::Full );

        //! returns point position for generic arrows
        Polygon genericArrow( GtkArrowType, QtSettings::ArrowSize = QtSettings::ArrowNormal ) const;

        //! vertical window background gradient
        cairo_pattern_t* verticalGradient( const ColorUtils::Rgba&, int, int ) const;

        //! radial window background gradient
        cairo_pattern_t* radialGradient( const ColorUtils::Rgba&, int, int, int ) const;

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
            explicit SlabRect( int x, int y, int w, int h, const TileSet::Tiles& tiles, StyleOptions options = StyleOptions() ):
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

        //! window manager
        WindowManager _windowManager;

        //! singleton
        static Style* _instance;

    };

}

#endif
