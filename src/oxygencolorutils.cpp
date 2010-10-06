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

#include "oxygencolorutils.h"
#include "oxygenrgba.h"

#include <cmath>
#include <algorithm>
#include <sstream>

namespace ColorUtils
{

    //___________________________________________________________
    // contrast
    static double _contrast = 0.5;
    static double _bgcontrast = 0.5;

    //___________________________________________________________
    void setContrast( double value )
    {
        _contrast = value;
        _bgcontrast = std::min( 1.0, 0.9*_contrast/0.7 );
    }

    //___________________________________________________________
    const double& contrast( void )
    { return _contrast; }

    //___________________________________________________________
    const double& backgroundContrast( void )
    { return _bgcontrast; }

    //___________________________________________________________________
    static inline double normalize( double a )
    { return ( a < 1.0 ? ( a > 0.0 ? a : 0.0 ) : 1.0 ); }

    //____________________________________________________________________
    bool lowThreshold(const Rgba &color)
    {

        const Rgba darker( shade(color, MidShade, 0.5 ) );
        return luma(darker) > luma(color);

    }

     //____________________________________________________________________
    bool highThreshold(const Rgba &color)
    {
        const Rgba lighter( shade(color, LightShade, 0.5 ) );
        return luma(lighter) < luma(color);
    }

    //_________________________________________________________________________
    Rgba backgroundTopColor(const Rgba &color)
    {
        if( lowThreshold(color) ) return shade(color, MidlightShade, 0.0);
        else {
            const double my( luma( shade(color, LightShade, 0.0) ) );
            const double by( luma(color) );
            return shade(color, (my - by) * backgroundContrast());
        }

    }

    //_________________________________________________________________________
    Rgba backgroundBottomColor(const Rgba &color)
    {

        const Rgba midColor( shade(color, MidShade, 0.0) );
        if( lowThreshold(color) ) return midColor;
        else {

            const double by( luma(color) );
            const double my( luma(midColor) );
            return shade(color, (my - by) * backgroundContrast());

        }
    }

    //_________________________________________________________________________
    Rgba backgroundRadialColor(const Rgba &color)
    {

        if( lowThreshold(color) ) return shade(color, LightShade, 0.0);
        else if( highThreshold( color ) ) return color;
        else return shade(color, LightShade, backgroundContrast() );
    }

    //_________________________________________________________________________
    Rgba lightColor(const Rgba &color)
    { return highThreshold( color ) ? color: shade( color, LightShade, contrast() ); }

    //_________________________________________________________________________
    Rgba darkColor( const Rgba& color )
    {
        return lowThreshold(color) ?
            mix( lightColor(color), color, 0.3 + 0.7 * contrast() ):
            shade(color, MidShade, contrast() );
    }

    //_________________________________________________________________________
    Rgba midColor( const Rgba& color )
    { return shade( color, MidShade, contrast() - 1.0 ); }


    //_________________________________________________________________________
    Rgba shadowColor( const Rgba& color )
    {
        Rgba mixed( mix( Rgba::black(), color, color.alpha() ) );
        return lowThreshold(color) ? mixed: shade( mixed, ShadowShade, contrast() );
    }

    //_________________________________________________________________________
    Rgba decoColor( const Rgba& background, const Rgba& color )
    { return mix( background, color, 0.4 + 0.8*contrast() ); }

    //_________________________________________________________________________
    Rgba alphaColor( const Rgba& color, double alpha )
    { return Rgba( color.red(), color.green(), color.blue(), normalize(alpha)*color.alpha() ); }

    //____________________________________________________________________
    Rgba backgroundColor(const Rgba &color, double ratio )
    {

        if( ratio < 0 ) return color;

        if( ratio < 0.5 )
        {

            const double a( 2.0*ratio );
            return mix(backgroundTopColor(color), color, a );

        } else {

            const double a( 2.0*ratio-1 );
            return mix(color, backgroundBottomColor(color), a );

        }

    }

    //___________________________________________________________________
    // luma coefficient
    static const double yc[3] = { 0.2126, 0.7152, 0.0722 };

    //___________________________________________________________________
    static inline double mixdouble( double a, double b, double bias )
    { return a + ( b - a ) * bias; }

    //___________________________________________________________________
    static inline double gamma( double n )
    { return pow( normalize( n ), 2.2 ); }

    //___________________________________________________________________
    static inline double igamma(double n)
    { return pow( normalize(n), 1.0/2.2); }

    //___________________________________________________________________
    static inline double wrap( double a, double d = 1.0 )
    {
        double r = fmod( a, d );
        return ( r < 0.0 ? d + r : ( r > 0.0 ? r : 0.0 ) );
    }

    //___________________________________________________________________
    // hcy representation of a colors
    class HCY
    {

        public:

        //! constructor
        HCY( const Rgba& color )
        {

            // transparency
            a = color.alpha();

            // luma component
            y = luma( color );

            double r = gamma( color.red() );
            double g = gamma( color.green() );
            double b = gamma( color.blue() );

            // hue component
            double p = std::max( std::max( r, g ), b );
            double n = std::min( std::min( r, g ), b );
            double d = 6.0 * ( p - n );
            if( n == p ) h = 0.0;
            else if( r == p ) h = ( ( g - b ) / d );
            else if( g == p ) h = ( ( b - r ) / d ) + ( 1.0 / 3.0 );
            else h = ( ( r - g ) / d ) + ( 2.0 / 3.0 );

            // chroma component
            if( r == g && g == b ) c = 0.0;
            else c = std::max( ( y - n ) / y, ( p - y ) / ( 1 - y ) );

        }

        //! convert back to color
        Rgba rgba() const
        {
            // start with sane component values
            double _h = wrap( h );
            double _c = normalize( c );
            double _y = normalize( y );

            // calculate some needed variables
            double _hs = _h * 6.0, th, tm;
            if( _hs < 1.0 )
            {

                th = _hs;
                tm = yc[0] + yc[1] * th;

            } else if( _hs < 2.0 ) {

                th = 2.0 - _hs;
                tm = yc[1] + yc[0] * th;

            } else if( _hs < 3.0 ) {

                th = _hs - 2.0;
                tm = yc[1] + yc[2] * th;

            } else if( _hs < 4.0 ) {

                th = 4.0 - _hs;
                tm = yc[2] + yc[1] * th;

            } else if( _hs < 5.0 ) {

                th = _hs - 4.0;
                tm = yc[2] + yc[0] * th;

            } else {

                th = 6.0 - _hs;
                tm = yc[0] + yc[2] * th;

            }

            // calculate RGB channels in sorted order
            double tn, to, tp;
            if( tm >= _y )
            {

                tp = _y + _y * _c * ( 1.0 - tm ) / tm;
                to = _y + _y * _c * ( th - tm ) / tm;
                tn = _y - ( _y * _c );

            } else {

                tp = _y + ( 1.0 - _y ) * _c;
                to = _y + ( 1.0 - _y ) * _c * ( th - tm ) / ( 1.0 - tm );
                tn = _y - ( 1.0 - _y ) * _c * tm / ( 1.0 - tm );

            }

            // return RGB channels in appropriate order
            if( _hs < 1.0 ) return Rgba( igamma( tp ), igamma( to ), igamma( tn ), a );
            else if( _hs < 2.0 ) return Rgba( igamma( to ), igamma( tp ), igamma( tn ), a );
            else if( _hs < 3.0 ) return Rgba( igamma( tn ), igamma( tp ), igamma( to ), a );
            else if( _hs < 4.0 ) return Rgba( igamma( tn ), igamma( to ), igamma( tp ), a );
            else if( _hs < 5.0 ) return Rgba( igamma( to ), igamma( tn ), igamma( tp ), a );
            else return Rgba( igamma( tp ), igamma( tn ), igamma( to ), a );
        }

        double h;
        double c;
        double y;
        double a;

    };

    //___________________________________________________________________
    double luma( const Rgba &color )
    {

        // RGB ratios
        return
            gamma( color.red() )*yc[0] +
            gamma( color.green() )*yc[1] +
            gamma( color.blue() )*yc[2];
    }

    //___________________________________________________________________
    double contrastRatio( const Rgba &c1, const Rgba &c2 )
    {
        double y1 = luma( c1 ), y2 = luma( c2 );
        if( y1 > y2 ) return ( y1 + 0.05 ) / ( y2 + 0.05 );
        else return ( y2 + 0.05 ) / ( y1 + 0.05 );
    }

    //___________________________________________________________________
    Rgba lighten( const Rgba &color, double ky, double kc )
    {
        HCY c( color );
        c.y = 1.0 - normalize( ( 1.0 - c.y ) * ( 1.0 - ky ) );
        c.c = 1.0 - normalize( ( 1.0 - c.c ) * kc );
        return c.rgba();
    }

    //___________________________________________________________________
    Rgba darken( const Rgba &color, double ky, double kc )
    {
        HCY c( color );
        c.y = normalize( c.y * ( 1.0 - ky ) );
        c.c = normalize( c.c * kc );
        return c.rgba();
    }

    //___________________________________________________________________
    static inline Rgba tintHelper( const Rgba &base, const Rgba &color, double amount )
    {
        HCY result( mix( base, color, pow( amount, 0.3 ) ) );
        result.y = mixdouble( luma( base ), result.y, amount );

        return result.rgba();
    }

    //___________________________________________________________________
    Rgba tint( const Rgba &base, const Rgba &color, double amount )
    {
        if( amount <= 0.0 ) return base;
        if( amount >= 1.0 ) return color;
        if( isnan( amount ) ) return base;

        double ri = contrastRatio( base, color );
        double rg = 1.0 + ( ( ri + 1.0 ) * amount * amount * amount );
        double u = 1.0, l = 0.0;
        Rgba result;
        for ( int i = 12 ; i ; --i )
        {

            double a = 0.5 * ( l+u );
            result = tintHelper( base, color, a );
            double ra = contrastRatio( base, result );
            if( ra > rg ) u = a;
            else l = a;

        }

        return result;

    }

    //___________________________________________________________________
    Rgba mix( const Rgba &c1, const Rgba &c2, double bias )
    {
        if( bias <= 0.0 ) return c1;
        if( bias >= 1.0 ) return c2;
        if( isnan( bias ) ) return c1;

        double r = mixdouble( c1.red(),   c2.red(),   bias );
        double g = mixdouble( c1.green(), c2.green(), bias );
        double b = mixdouble( c1.blue(),  c2.blue(),  bias );
        double a = mixdouble( c1.alpha(), c2.alpha(), bias );

        return Rgba( r, g, b, a );
    }


    //___________________________________________________________________
    Rgba shade(const Rgba &color, ShadeRole role)
    { return shade(color, role, _contrast ); }

    //___________________________________________________________________
    Rgba shade(const Rgba &color, ShadeRole role, double contrast, double chromaAdjust)
    {

        // nan -> 1.0
        contrast = (1.0 > contrast ? (-1.0 < contrast ? contrast : -1.0) : 1.0);
        double y = luma(color), yi = 1.0 - y;

        if (y < 0.006)
        {

            // handle very dark colors (base, mid, dark, shadow == midlight, light)
            switch (role)
            {

                case LightShade: return shade(color, 0.05 + 0.95 * contrast, chromaAdjust);
                case MidShade: return shade(color, 0.01 + 0.20 * contrast, chromaAdjust);
                case DarkShade: return shade(color, 0.02 + 0.40 * contrast, chromaAdjust);
                default: return shade(color, 0.03 + 0.60 * contrast, chromaAdjust);

            }

        } else if (y > 0.93) {

            // handle very light colors (base, midlight, light == mid, dark, shadow)
            switch (role)
            {

                case MidlightShade: return shade(color, -0.02 - 0.20 * contrast, chromaAdjust);
                case DarkShade: return shade(color, -0.06 - 0.60 * contrast, chromaAdjust);
                case ShadowShade: return shade(color, -0.10 - 0.90 * contrast, chromaAdjust);
                default: return shade(color, -0.04 - 0.40 * contrast, chromaAdjust);

            }

        } else {

            // handle everything else
            double lightAmount = (0.05 + y * 0.55) * (0.25 + contrast * 0.75);
            double darkAmount =  (     - y       ) * (0.55 + contrast * 0.35);
            switch (role)
            {
                case LightShade: return shade(color, lightAmount, chromaAdjust);
                case MidlightShade: return shade(color, (0.15 + 0.35 * yi) * lightAmount, chromaAdjust);
                case MidShade: return shade(color, (0.35 + 0.15 * y) * darkAmount, chromaAdjust);
                case DarkShade: return shade(color, darkAmount, chromaAdjust);
                default: return darken(shade(color, darkAmount, chromaAdjust), 0.5 + 0.3 * y);
            }

        }

    }

    //___________________________________________________________________
    Rgba shade( const Rgba &color, double ky, double kc )
    {
        HCY c( color );
        c.y = normalize( c.y + ky );
        c.c = normalize( c.c + kc );
        return c.rgba();
    }


}
