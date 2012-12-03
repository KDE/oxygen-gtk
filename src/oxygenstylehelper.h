#ifndef oxygenstylehelper_h
#define oxygenstylehelper_h
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

#include "oxygencachekey.h"
#include "oxygencairosurface.h"
#include "oxygencairosurfacecache.h"
#include "oxygentileset.h"
#include "oxygentilesetcache.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    // forward declaration
    namespace Cairo
    { class Context; }

    namespace ColorUtils
    { class Rgba; }

    class StyleHelper
    {

        public:

        //! constructor
        StyleHelper( void );

        //! destructor
        virtual ~StyleHelper( void );

        //! initialize ref surface
        void initializeRefSurface( void );

        //! separators
        void drawSeparator( Cairo::Context&, const ColorUtils::Rgba& color, int x, int y, int w, int h, bool vertical );

        //! clear caches
        void clearCaches( void )
        {
            _separatorCache.clear();
            _slabCache.clear();
            _slopeCache.clear();
            _slabSunkenCache.clear();
            _holeFocusedCache.clear();
            _holeFlatCache.clear();
            _scrollHoleCache.clear();
            _scrollHandleCache.clear();
            _slitFocusedCache.clear();
            _dockFrameCache.clear();
            _grooveCache.clear();
            _selectionCache.clear();
            _roundSlabCache.clear();
            _sliderSlabCache.clear();
            _progressBarIndicatorCache.clear();
            _windecoButtonCache.clear();
            _windecoButtonGlowCache.clear();
            _windowShadowCache.clear();
            _verticalGradientCache.clear();
            _radialGradientCache.clear();
            _windecoLeftBorderCache.clear();
            _windecoRightBorderCache.clear();
            _windecoTopBorderCache.clear();
            _windecoBottomBorderCache.clear();
        }

        //! create surface from reference for given width and height
        Cairo::Surface createSurface( int w, int h ) const
        {
            if( w <= 0 || h <= 0 ) return 0L;
            else return cairo_surface_create_similar( _refSurface, CAIRO_CONTENT_COLOR_ALPHA, w, h );
        }

        //!@name decoration specific helper functions
        //@{
        const Cairo::Surface& windecoButton(const ColorUtils::Rgba &color, bool pressed, int size = 21);
        const Cairo::Surface& windecoButtonGlow(const ColorUtils::Rgba &color, int size = 21);
        //@}

        //!@name window background cache
        //@{
        const Cairo::Surface& verticalGradient( const ColorUtils::Rgba&, int );
        const Cairo::Surface& radialGradient( const ColorUtils::Rgba&, int );
        //@}

        //!@name slabs
        //@{

        const TileSet& slab( const ColorUtils::Rgba& color, double shade = 0, int size = 7 )
        { return slab( color, ColorUtils::Rgba(), shade, size ); }

        const TileSet& slab( const ColorUtils::Rgba&, const ColorUtils::Rgba&, double shade = 0, int size = 7 );

        const TileSet& slabSunken( const ColorUtils::Rgba&, int size = 7 );

        const Cairo::Surface& roundSlab( const ColorUtils::Rgba& color, double shade, int size = 7 )
        { return roundSlab( color, ColorUtils::Rgba(), shade, size ); }

        const Cairo::Surface& roundSlab( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, double shade, int size = 7 );

        // slider slab
        const Cairo::Surface& sliderSlab( const ColorUtils::Rgba&, const ColorUtils::Rgba& glowColor, bool sunken, double shade, int size = 7 );

        //! groupbox background
        const TileSet& slope( const ColorUtils::Rgba&, double shade, int size = 7 );

        void fillSlab( cairo_t* context, int x, int y, int w, int h, const TileSet::Tiles& = TileSet::Ring ) const;

        //@}

        //! rounded mask
        /*! used for non-composited tooltips and menus */
        GdkPixmap* roundMask( int w, int h, int radius = 6 ) const;

        //!@name holes
        //@{

        const TileSet& hole( const ColorUtils::Rgba& base, const ColorUtils::Rgba& fill, int size = 7, bool contrast = false )
        { return holeFocused( base, fill, ColorUtils::Rgba(), size, contrast ); }

        const TileSet& hole( const ColorUtils::Rgba& base, int size = 7, bool contrast = false)
        { return holeFocused( base, ColorUtils::Rgba(), ColorUtils::Rgba(), size, contrast ); }

        const TileSet& holeFocused( const ColorUtils::Rgba& base, const ColorUtils::Rgba &glow, int size = 7, bool contrast = false )
        { return holeFocused( base, ColorUtils::Rgba(), glow, size, contrast ); }

        const TileSet& holeFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba& fill, const ColorUtils::Rgba &glow, int size = 7, bool contrast = false );

        const TileSet& holeFlat( const  ColorUtils::Rgba&, double, bool = true, int size = 7 );

        //@}

        //!@name scrollbar elements
        //@{
        const TileSet& scrollHole( const ColorUtils::Rgba&, bool vertical, bool smallShadow = false );
        const TileSet& scrollHandle( const ColorUtils::Rgba&, const ColorUtils::Rgba&, int size = 7 );

        //! 'slit' (for hovered/focused flat buttons )
        const TileSet& slitFocused( const ColorUtils::Rgba& );

        //! dock frame
        const TileSet& dockFrame( const ColorUtils::Rgba&, const ColorUtils::Rgba& );

        //! progressbar indicator
        const Cairo::Surface& progressBarIndicator( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, int w, int h );

        //! slider groove
        const TileSet& groove( const ColorUtils::Rgba&, int size = 7 );

        //! selection
        const TileSet& selection( const ColorUtils::Rgba&, int, bool custom );

        //! dots
        void renderDot( cairo_t*, const ColorUtils::Rgba&, int x, int y ) const;

        //!@name windeco caches
        //@{

        /*! needed to oxygenwindowshadow and drawWindowDecoration() */
        TileSetCache<WindowShadowKey>& windowShadowCache( void )
        { return _windowShadowCache; }

        CairoSurfaceCache<WindecoLeftBorderKey>& windecoLeftBorderCache( void )
        { return _windecoLeftBorderCache; }

        CairoSurfaceCache<WindecoRightBorderKey>& windecoRightBorderCache( void )
        { return _windecoRightBorderCache; }

        CairoSurfaceCache<WindecoTopBorderKey>& windecoTopBorderCache( void )
        { return _windecoTopBorderCache; }

        CairoSurfaceCache<WindecoBottomBorderKey>& windecoBottomBorderCache( void )
        { return _windecoBottomBorderCache; }

        //! shadow rendering
        virtual void drawShadow( cairo_t*, const ColorUtils::Rgba&, int size ) const;

        //! highlight glow
        virtual void drawOuterGlow( cairo_t*, const ColorUtils::Rgba&, int size) const;

        //@}

        //@}

        //! window decorations button for MDI windows from oxygen-qt, used as tab close button in oxygen-gtk
        const Cairo::Surface& dockWidgetButton(const ColorUtils::Rgba& base, bool pressed, int size);

        protected:

        //! access reference surface
        const Cairo::Surface& refSurface( void ) const
        { return _refSurface; }

        // separator
        const Cairo::Surface& separator(const ColorUtils::Rgba &color, bool vertical, int size );

        //! slab rendering
        virtual void drawSlab( Cairo::Context&, const ColorUtils::Rgba&, double shade) const;

        //! inverse shadow (for sunken slabs)
        void drawInverseShadow( Cairo::Context&, const ColorUtils::Rgba&, int pad, int size, double fuzz ) const;

        //! inverse glow (for sunken slabs)
        void drawInverseGlow( Cairo::Context&, const ColorUtils::Rgba&, int pad, int size, int rsize ) const;

        //! round slab
        void drawRoundSlab( Cairo::Context&, const ColorUtils::Rgba&, double ) const;

        //! slider slab
        void drawSliderSlab( Cairo::Context&, const ColorUtils::Rgba&, bool sunken, double ) const;

        //! inverse shadow gradient
        cairo_pattern_t* inverseShadowGradient( const ColorUtils::Rgba&, int pad, int size, double fuzz ) const;

        private:

        //!@name some constants for drawing
        //@{
        static const double _slabThickness;
        static const double _shadowGain;
        static const double _glowBias;
        //@}

        //! reference surface for all later surface creations
        Cairo::Surface _refSurface;

        //!@name caches
        //@{

        //! round slabs
        CairoSurfaceCache<SeparatorKey> _separatorCache;

        //! slabs
        TileSetCache<SlabKey> _slabCache;

        //! slabs
        TileSetCache<SlabKey> _slopeCache;

        //! slabs
        TileSetCache<SlabKey> _slabSunkenCache;

        //! holes
        TileSetCache<HoleFocusedKey> _holeFocusedCache;

        //! holes
        TileSetCache<HoleFlatKey> _holeFlatCache;

        //! scroll hole
        TileSetCache<ScrollHoleKey> _scrollHoleCache;

        //! scrollbar handle
        TileSetCache<ScrollHandleKey> _scrollHandleCache;

        //! slit
        TileSetCache<SlitFocusedKey> _slitFocusedCache;

        //! docks
        TileSetCache<DockFrameKey> _dockFrameCache;

        //! groove
        TileSetCache<GrooveKey> _grooveCache;

        //! selection
        TileSetCache<SelectionKey> _selectionCache;

        //! round slabs
        CairoSurfaceCache<SlabKey> _roundSlabCache;

        //! slider slabs
        CairoSurfaceCache<SliderSlabKey> _sliderSlabCache;

        //! progressbar indicators
        CairoSurfaceCache<ProgressBarIndicatorKey> _progressBarIndicatorCache;

        //! decoration button
        CairoSurfaceCache<WindecoButtonKey> _windecoButtonCache;

        //! decoration glow
        CairoSurfaceCache<WindecoButtonGlowKey> _windecoButtonGlowCache;

        //! decoration shadow
        TileSetCache<WindowShadowKey> _windowShadowCache;

        //! window backgound vertical gradient
        CairoSurfaceCache<VerticalGradientKey> _verticalGradientCache;

        //! window backgound radial gradient
        CairoSurfaceCache<RadialGradientKey> _radialGradientCache;

        //! dock widget button
        CairoSurfaceCache<DockWidgetButtonKey> _dockWidgetButtonCache;

        //@}

        //!@name windeco borders
        //@{

        CairoSurfaceCache<WindecoLeftBorderKey> _windecoLeftBorderCache;
        CairoSurfaceCache<WindecoRightBorderKey> _windecoRightBorderCache;
        CairoSurfaceCache<WindecoTopBorderKey> _windecoTopBorderCache;
        CairoSurfaceCache<WindecoBottomBorderKey> _windecoBottomBorderCache;

        //@}

    };

}

#endif
