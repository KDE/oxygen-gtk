#ifndef oxygencolorutils_h
#define oxygencolorutils_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* inspired notably from kdelibs/kdeui/color/kcolorutils.h
* Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
* Copyright (C) 2007 Thomas Zander <zander@kde.org>
* Copyright (C) 2007 Zack Rusin <zack@kde.org>
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

#include "oxygenrgba.h"

namespace Oxygen
{
    namespace ColorUtils
    {

        //! set contrast values
        void setContrast( double );

        //! clear caches
        void clearCaches( void );

        //! contrast
        const double& contrast( void );

        //! background contrast
        const double& backgroundContrast( void );

        //!@name color utilities
        //@{
        bool lowThreshold( const Rgba& );
        bool highThreshold( const Rgba& );
        Rgba backgroundTopColor( const Rgba& );
        Rgba backgroundBottomColor( const Rgba& );
        Rgba backgroundRadialColor( const Rgba& );
        Rgba lightColor( const Rgba& );
        Rgba darkColor( const Rgba& );
        Rgba midColor( const Rgba& );
        Rgba shadowColor( const Rgba& );

        //! decoration color. Merges foreground and background
        Rgba decoColor( const Rgba& background, const Rgba& color );

        //! change alpha chanel (multiplicative)
        Rgba alphaColor( const Rgba& color, double alpha);

        //! returns background color matching position in a top level widget of given height
        Rgba backgroundColor(const Rgba &color, double ratio);

        //! returns background color matching position in a top level widget of given height
        inline Rgba backgroundColor(const Rgba &color, int height, int y)
        { return (height > 0) ? backgroundColor( color, std::min( 1.0 , double( y )/std::min( 300, 3*height/4 ) ) ) : color; }

        //! returns menu background color matching position in a top level widget of given height
        inline Rgba menuBackgroundColor(const Rgba &color, int height, int y)
        { return (height > 0) ? backgroundColor( color, std::min( 1.0, double( y )/std::min( 200, 3*height/4 ) ) ) : color; }

        //@}

        /*!
        Calculate the luma of a color. Luma is weighted sum of gamma-adjusted
        R'G'B' components of a color. The result is similar to qGray. The range
        is from 0.0 (black) to 1.0 (white).
        */
        double luma(const Rgba&);

        /*!
        Calculate the contrast ratio between two colors, according to the
        W3C/WCAG2.0 algorithm, (Lmax + 0.05)/(Lmin + 0.05), where Lmax and Lmin
        are the luma values of the lighter color and the darker color,
        respectively.
        */
        double contrastRatio(const Rgba&, const Rgba&);

        //! Adjust the luma of a color by changing its distance from white.
        Rgba lighten(const Rgba&, double amount = 0.5, double chromaInverseGain = 1.0);

        //! Adjust the luma of a color by changing its distance from black.
        Rgba darken(const Rgba&, double amount = 0.5, double chromaGain = 1.0);

        //! Create a new color by tinting one color with another
        Rgba tint(const Rgba &base, const Rgba &color, double amount = 0.3);

        //! mix two colors
        Rgba mix(const Rgba &c1, const Rgba &c2, double bias = 0.5);

        enum ShadeRole
        {

            // The light color is lighter than dark() or shadow() and contrasts with the base color.
            LightShade,

            // The midlight color is in between base() and light().
            MidlightShade,

            // The mid color is in between base() and dark().
            MidShade,

            // The dark color is in between mid() and shadow().
            DarkShade,

            // The shadow color is darker than light() or midlight() and contrasts the base color.
            ShadowShade

        };

        Rgba shade(const Rgba&, ShadeRole);
        Rgba shade(const Rgba&, ShadeRole, double contrast, double chromaAdjust = 0.0);
        Rgba shade(const Rgba&, double lumaAmount, double chromaAmount = 0.0);

    }
}

#endif
