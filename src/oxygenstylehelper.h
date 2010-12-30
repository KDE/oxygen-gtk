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
#include "oxygengdkpixbufcache.h"
#include "oxygentileset.h"
#include "oxygentilesetcache.h"

#include <gdk/gdk.h>
#include <X11/Xdefs.h>

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

        //! set background gradient hint to widget
        virtual void setHasBackgroundGradient( XID, bool ) const;

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
        }

        // separator
        virtual GdkPixbuf* separator(const ColorUtils::Rgba &color, bool vertical, int size );

        //!@name decoration specific helper functions
        //!
        //@{
        virtual GdkPixbuf* windecoButton(const ColorUtils::Rgba &color, bool pressed, int size = 21);
        virtual GdkPixbuf* windecoButtonGlow(const ColorUtils::Rgba &color, int size = 21);
        //@}

        //!@name slabs
        //@{

        const TileSet& slab( const ColorUtils::Rgba&, double shade, int size = 7 );
        const TileSet& slabFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba&, double shade, int size = 7 );
        const TileSet& slabSunken( const ColorUtils::Rgba&, double shade, int size = 7 );

        GdkPixbuf* roundSlab( const ColorUtils::Rgba&, double shade, int size = 7 );
        GdkPixbuf* roundSlabFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, double shade, int size = 7 );

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
        GdkPixbuf* progressBarIndicator( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, int w, int h );

        //! slider groove
        const TileSet& groove( const ColorUtils::Rgba&, double shade, int size = 7 );

        //! selection
        const TileSet& selection( const ColorUtils::Rgba&, int, bool custom );

        //! dots
        void renderDot( cairo_t*, const ColorUtils::Rgba&, int x, int y ) const;

        protected:

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

        //!@name caches
        //@{

        //! round slabs
        GdkPixbufCache<SeparatorKey> m_separatorCache;

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
        GdkPixbufCache<SlabKey> m_roundSlabCache;

        //! round slabs
        GdkPixbufCache<SlabFocusedKey> m_roundSlabFocusedCache;

        //! progressbar indicators
        GdkPixbufCache<ProgressBarIndicatorKey> m_progressBarIndicatorCache;

        //! decoration button
        GdkPixbufCache<WindecoButtonKey> m_windecoButtonCache;

        //! decoration glow
        GdkPixbufCache<WindecoButtonGlowKey> m_windecoButtonGlowCache;

        //@}

        //! argb hint atom
        Atom _backgroundGradientAtom;

    };

}

#endif
