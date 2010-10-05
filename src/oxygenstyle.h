#ifndef oxygenstyle_h
#define oxygenstyle_h
/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygencairocontext.h"
#include "oxygencomboboxdata.h"
#include "oxygengeometry.h"
#include "oxygengtkgap.h"
#include "oxygenqtsettings.h"
#include "oxygenstylehelper.h"
#include "oxygenstyleoptions.h"
#include "oxygenwidgetset.h"

#include <gdk/gdktypes.h>

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
        {}

        //! settings
        const QtSettings& settings( void ) const
        { return _settings; }

        //!@name primitives
        //@{

        //! fill given rectangle with flat color
        void fill( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const ColorUtils::Rgba& color ) const;

        //! separators
        void drawSeparator( GdkWindow*, GtkWidget*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;
        void drawSeparator( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o ) const
        { drawSeparator( window, 0L, r, x, y, w, h, o ); }

        //! window background
        /*! returns true if window gradient could be rendered */
        bool renderWindowBackground( GdkWindow*, GtkWidget*, GdkRectangle*, gint, gint, gint, gint ) const;
        bool renderWindowBackground( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h ) const
        { return renderWindowBackground( window, 0L, r, x, y, w, h ); }

        //! window background
        void renderMenuBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! tooltip background
        void renderTooltipBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! tree view header
        void renderHeaderBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint ) const;

        //! tree view header
        void renderHeaderLines( GdkWindow*, GdkRectangle*, gint, gint, gint, gint ) const;

        //! editors hole
        /*! returns true if window gradient could be rendered */
        bool renderHoleBackground( GdkWindow*, GdkRectangle*, gint, gint, gint, gint ) const;

        //! splitters
        void renderSplitter( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //!@name progressbar
        //@{
        void renderProgressBarHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;
        void renderProgressBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, GtkProgressBarOrientation ) const;
        //@}

        //!@name scrollbar
        //@{
        void renderScrollBarHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;
        void renderScrollBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;
        //@}

        //! toolbar handle
        void renderToolBarHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! frame
        void drawFloatFrame( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! button slab
        void renderButtonSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! checkbox
        void renderCheckBox( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! radio button
        void renderRadioButton( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //!@name generic slab
        //@{

        void renderSlab( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions ) const;
        void renderSlab( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o ) const
        { renderSlab( window, r, x, y, w, h, Gtk::Gap(), o ); }

        //@}

        //!@name hole
        //@{

        void renderHole( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions ) const;
        void renderHole( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o ) const
        { renderHole( window, r, x, y, w, h, Gtk::Gap(), o ); }
        //@}

        //!@name dock frame
        //@{
        void renderDockFrame( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, const Gtk::Gap&, StyleOptions ) const;
        void renderDockFrame( GdkWindow* window, GdkRectangle* r, gint x, gint y, gint w, gint h, StyleOptions o ) const
        { renderDockFrame( window, r, x, y, w, h, Gtk::Gap(), o ); }
        //@}

        //! menu item
        void renderMenuItemRect( GdkWindow*, GdkRectangle*, GtkWidget*, gint, gint, gint, gint ) const;

        //! selection
        void renderSelection( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, TileSet::Tiles tiles, StyleOptions ) const;

        //! arrow size
        enum ArrowSize
        {
            ArrowNormal,
            ArrowSmall,
            ArrowTiny
        };

        //! arrow
        void renderArrow( GdkWindow*, GdkRectangle*, GtkArrowType, gint, gint, gint, gint, ArrowSize = ArrowNormal, StyleOptions = Contrast ) const;

        //! slider groove
        void renderSliderGroove( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! slider handle
        void renderSliderHandle( GdkWindow*, GdkRectangle*, gint, gint, gint, gint, StyleOptions ) const;

        //! size grip
        void renderSizeGrip( GdkWindow*, GdkRectangle*, GdkWindowEdge, gint, gint, gint, gint ) const;

        //@}

        //! sanitize size
        void sanitizeSize( GdkWindow* window, gint& width, gint& height ) const;

        //! return map of registered comboboxes
        WidgetMap<ComboBoxData>& comboBoxes( void ) const
        { return *_comboBoxes; }

        protected:

        //! constructor
        explicit Style( void );

        //@name internal rendering
        //@{

        //! slab glowing color
        ColorUtils::Rgba slabShadowColor( StyleOptions ) const;

        //! slab
        void renderSlab( Cairo::Context&, gint, gint, gint, gint, const ColorUtils::Rgba&, StyleOptions, TileSet::Tiles tiles = TileSet::Ring ) const;

        //! progressbar hole (groove)
        /*! also used for progress bars */
        void renderScrollBarHole( Cairo::Context&, gint, gint, gint, gint, const ColorUtils::Rgba&, bool vertical, TileSet::Tiles tiles = TileSet::Full ) const;

        //! returns point position for generic arrows
        Polygon genericArrow( GtkArrowType, ArrowSize = ArrowNormal ) const;

        //! vertical window background gradient
        cairo_pattern_t* verticalGradient( const ColorUtils::Rgba&, int, int ) const;

        //! radial window background gradient
        cairo_pattern_t* radialGradient( const ColorUtils::Rgba&, int, int, int ) const;

        //@}

        //!@name utilities
        //@{

        // center rect
        void centerRect( GdkRectangle*, GdkRectangle* ) const;

        // generate map from gap
        void generateGapMask( Cairo::Context&, gint, gint, gint, gint, const Gtk::Gap& ) const;

        //@}

        private:

        //! metrics
        /*! these are copied from the old KStyle WidgetProperties */
        enum InternalMetrics
        {

            // checkbox. Do not change, unless
            // changing the actual cached pixmap size
            CheckBox_Size = 21

        };

        // Qt settings
        QtSettings _settings;

        // helper
        StyleHelper _helper;

        //! combobox data
        WidgetMap<ComboBoxData>* _comboBoxes;

    };

}

#endif
