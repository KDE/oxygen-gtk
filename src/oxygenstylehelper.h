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
        virtual ~StyleHelper( void )
        {}

        //! initialize ref surface
        void initializeRefSurface( void );

        //! separators
        void drawSeparator( Cairo::Context&, const ColorUtils::Rgba& color, int x, int y, int w, int h, bool vertical );

        //! clear caches
        void clearCaches( void )
        {
            m_separatorCache.clear();
            m_slabCache.clear();
            m_slabFocusedCache.clear();
            m_slabSunkenCache.clear();
            m_holeCache.clear();
            m_holeFocusedCache.clear();
            m_holeFlatCache.clear();
            m_scrollHoleCache.clear();
            m_slitFocusedCache.clear();
            m_dockFrameCache.clear();
            m_grooveCache.clear();
            m_selectionCache.clear();
            m_roundSlabCache.clear();
            m_roundSlabFocusedCache.clear();
            m_progressBarIndicatorCache.clear();
            m_windecoButtonCache.clear();
            m_windecoButtonGlowCache.clear();
            m_verticalGradientCache.clear();
            m_radialGradientCache.clear();
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

        const TileSet& slab( const ColorUtils::Rgba&, double shade, int size = 7 );
        const TileSet& slabFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba&, double shade, int size = 7 );
        const TileSet& slabSunken( const ColorUtils::Rgba&, double shade, int size = 7 );

        const Cairo::Surface& roundSlab( const ColorUtils::Rgba&, double shade, int size = 7 );
        const Cairo::Surface& roundSlabFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, double shade, int size = 7 );

        void fillSlab( Cairo::Context& context, int x, int y, int w, int h, const TileSet::Tiles& = TileSet::Ring ) const;

        //@}

        //! rounded mask
        /*! used for non-composited tooltips and menus */
        GdkPixmap* roundMask( int w, int h, int radius = 6 ) const;

        //!@name holes
        //@{

        const TileSet& hole( const ColorUtils::Rgba& base, const ColorUtils::Rgba& fill, double shade, int size = 7);
        const TileSet& hole( const ColorUtils::Rgba& base, double shade, int size = 7)
        { return hole( base, ColorUtils::Rgba(), shade, size ); }

        const TileSet& holeFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba& fill, const ColorUtils::Rgba &glow, double shade, int size = 7 );
        const TileSet& holeFocused( const ColorUtils::Rgba& base, const ColorUtils::Rgba &glow, double shade, int size = 7 )
        { return holeFocused( base, ColorUtils::Rgba(), glow, shade, size ); }

        const TileSet& holeFlat( const  ColorUtils::Rgba&, double shade, int size = 7 );
        const TileSet& scrollHole( const ColorUtils::Rgba&, bool vertical );

        //@}

        //! 'slit' (for hovered/focused flat buttons )
        const TileSet& slitFocused( const ColorUtils::Rgba& );

        //! dock frame
        const TileSet& dockFrame( const ColorUtils::Rgba&, int size );

        //! progressbar indicator
        const Cairo::Surface& progressBarIndicator( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, int w, int h );

        //! slider groove
        const TileSet& groove( const ColorUtils::Rgba&, double shade, int size = 7 );

        //! selection
        const TileSet& selection( const ColorUtils::Rgba&, int, bool custom );

        //! dots
        void renderDot( Cairo::Context&, const ColorUtils::Rgba&, int x, int y ) const;

        //@}

        protected:

        //! access reference surface
        const Cairo::Surface& refSurface( void ) const
        { return _refSurface; }

        // separator
        const Cairo::Surface& separator(const ColorUtils::Rgba &color, bool vertical, int size );

        //! slab rendering
        virtual void drawSlab( Cairo::Context&, const ColorUtils::Rgba&, double shade) const;

        //! draw hole
        void drawHole( Cairo::Context&, const ColorUtils::Rgba&, double shade, int r = 7 ) const;

        //! shadow rendering
        virtual void drawShadow( Cairo::Context&, const ColorUtils::Rgba&, int size) const;

        //! highlight glow
        virtual void drawOuterGlow( Cairo::Context&, const ColorUtils::Rgba&, int size) const;

        //! inverse shadow (for sunken slabs)
        void drawInverseShadow( Cairo::Context&, const ColorUtils::Rgba&, int pad, int size, double fuzz ) const;

        //! inverse glow (for sunken slabs)
        void drawInverseGlow( Cairo::Context&, const ColorUtils::Rgba&, int pad, int size, int rsize ) const;

        //! round slab
        void drawRoundSlab( Cairo::Context&, const ColorUtils::Rgba&, double ) const;

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
        CairoSurfaceCache<SeparatorKey> m_separatorCache;

        //! slabs
        TileSetCache<SlabKey> m_slabCache;

        //! slabs
        TileSetCache<SlabFocusedKey> m_slabFocusedCache;

        //! slabs
        TileSetCache<SlabKey> m_slabSunkenCache;

        //! holes
        TileSetCache<HoleKey> m_holeCache;

        //! holes
        TileSetCache<HoleFocusedKey> m_holeFocusedCache;

        //! holes
        TileSetCache<HoleFlatKey> m_holeFlatCache;

        //! scroll hole
        TileSetCache<ScrollHoleKey> m_scrollHoleCache;

        //! slit
        TileSetCache<SlitFocusedKey> m_slitFocusedCache;

        //! docks
        TileSetCache<DockFrameKey> m_dockFrameCache;

        //! groove
        TileSetCache<GrooveKey> m_grooveCache;

        //! selection
        TileSetCache<SelectionKey> m_selectionCache;

        //! round slabs
        CairoSurfaceCache<SlabKey> m_roundSlabCache;

        //! round slabs
        CairoSurfaceCache<SlabFocusedKey> m_roundSlabFocusedCache;

        //! progressbar indicators
        CairoSurfaceCache<ProgressBarIndicatorKey> m_progressBarIndicatorCache;

        //! decoration button
        CairoSurfaceCache<WindecoButtonKey> m_windecoButtonCache;

        //! decoration glow
        CairoSurfaceCache<WindecoButtonGlowKey> m_windecoButtonGlowCache;

        //! window backgound vertical gradient
        CairoSurfaceCache<VerticalGradientKey> m_verticalGradientCache;

        //! window backgound radial gradient
        CairoSurfaceCache<RadialGradientKey> m_radialGradientCache;

        //@}

    };

}

#endif
