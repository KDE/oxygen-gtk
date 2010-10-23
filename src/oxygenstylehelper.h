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

#include "oxygentileset.h"

#include <gdk/gdk.h>

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
        StyleHelper( void )
        {}

        //! destructor
        virtual ~StyleHelper( void )
        {}

        //! separators
        virtual void drawSeparator( Cairo::Context&, const ColorUtils::Rgba& color, int x, int y, int w, int h, bool vertical ) const;

        //!@name slabs
        //@{

        TileSet slab( const ColorUtils::Rgba&, double shade, int size = 7 ) const;
        TileSet slabFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba&, double shade, int size = 7 ) const;
        TileSet slabSunken( const ColorUtils::Rgba&, double shade, int size = 7 ) const;

        GdkPixbuf* roundSlab( const ColorUtils::Rgba&, double shade, int size = 7 ) const;
        GdkPixbuf* roundSlabFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, double shade, int size = 7 ) const;

        void fillSlab( Cairo::Context& context, int x, int y, int w, int h, const TileSet::Tiles& = TileSet::Ring ) const;

        //@}

        //! rounded mask
        /*! used for non-composited tooltips and menus */
        GdkPixmap* roundMask( int w, int h, int radius = 6 ) const;

        //!@name holes
        //@{

        TileSet hole( const ColorUtils::Rgba&, double shade, int size = 7, bool fill = false ) const;
        TileSet holeFlat( const  ColorUtils::Rgba&, double shade, int size = 7 ) const;
        TileSet holeFocused( const ColorUtils::Rgba&, const ColorUtils::Rgba &glowColor, double shade, int size = 7, bool fill = false ) const;
        TileSet scrollHole( const ColorUtils::Rgba&, bool vertical ) const;

        //@}

        //! 'slit' (for hovered/focused flat buttons )
        TileSet slitFocused( const ColorUtils::Rgba& ) const;

        //! dock frame
        TileSet dockFrame( const ColorUtils::Rgba&, int size ) const;

        //! progressbar indicator
        GdkPixbuf* progressBarIndicator( const ColorUtils::Rgba&, const ColorUtils::Rgba& glow, int w, int h ) const;

        //! slider groove
        TileSet groove( const ColorUtils::Rgba&, double shade, int size = 7 ) const;

        //! selection
        TileSet selection( const ColorUtils::Rgba&, int, bool custom ) const;

        //! dots
        void renderDot( Cairo::Context&, const ColorUtils::Rgba&, int x, int y ) const;

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

    };

}

#endif
