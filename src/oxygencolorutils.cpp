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
#include "oxygencache.h"
#include "oxygenrgba.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>

namespace Oxygen
{

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

        // caches
        typedef SimpleCache<guint32,Rgba> ColorCache;

        static ColorCache m_decoColorCache;
        static ColorCache m_lightColorCache;
        static ColorCache m_darkColorCache;
        static ColorCache m_midColorCache;
        static ColorCache m_shadowColorCache;
        static ColorCache m_backgroundTopColorCache;
        static ColorCache m_backgroundBottomColorCache;
        static ColorCache m_backgroundRadialColorCache;
        static ColorCache m_backgroundColorCache;

        typedef SimpleCache<guint32,bool> ColorFlags;
        ColorFlags m_highThreshold;
        ColorFlags m_lowThreshold;

        // clear caches
        void clearCaches( void )
        {
            m_decoColorCache.clear();
            m_lightColorCache.clear();
            m_darkColorCache.clear();
            m_midColorCache.clear();
            m_shadowColorCache.clear();
            m_backgroundTopColorCache.clear();
            m_backgroundBottomColorCache.clear();
            m_backgroundRadialColorCache.clear();
            m_backgroundColorCache.clear();

            m_highThreshold.clear();
            m_lowThreshold.clear();

        }

    }

    //____________________________________________________________________
    bool ColorUtils::lowThreshold(const Rgba &color)
    {

        ColorFlags::const_iterator iter( m_lowThreshold.find( color.toInt() ) );
        if( iter != m_lowThreshold.end() ) return iter->second;
        else {

            const Rgba darker( shade(color, MidShade, 0.5 ) );
            const bool out( luma(darker) > luma(color) );
            m_lowThreshold.insert( color.toInt(), out );
            return out;
        }

    }

     //____________________________________________________________________
    bool ColorUtils::highThreshold(const Rgba &color)
    {

        ColorFlags::const_iterator iter( m_highThreshold.find( color.toInt() ) );
        if( iter != m_highThreshold.end() ) return iter->second;
        else {

            const Rgba lighter( shade(color, LightShade, 0.5 ) );
            const bool out( luma(lighter) < luma(color) );
            m_highThreshold.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::backgroundTopColor(const Rgba &color)
    {

        ColorCache::const_iterator iter( m_backgroundTopColorCache.find( color.toInt() ) );
        if( iter != m_backgroundTopColorCache.end() ) return iter->second;
        else {
            Rgba out;
            if( lowThreshold(color) ) out = shade(color, MidlightShade, 0.0);
            else {
                const double my( luma( shade(color, LightShade, 0.0) ) );
                const double by( luma(color) );
                out = shade(color, (my - by) * backgroundContrast());
            }

            m_backgroundTopColorCache.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::backgroundBottomColor(const Rgba &color)
    {

        ColorCache::const_iterator iter( m_backgroundBottomColorCache.find( color.toInt() ) );
        if( iter != m_backgroundBottomColorCache.end() ) return iter->second;
        else {
            Rgba out( shade(color, MidShade, 0.0) );
            if( !lowThreshold(color) ) {

                const double by( luma(color) );
                const double my( luma(out) );
                out = shade(color, (my - by) * backgroundContrast());
            }

            m_backgroundBottomColorCache.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::backgroundRadialColor(const Rgba &color)
    {

        ColorCache::const_iterator iter( m_backgroundRadialColorCache.find( color.toInt() ) );
        if( iter != m_backgroundRadialColorCache.end() ) return iter->second;
        else {
            Rgba out;
            if( lowThreshold(color) ) out = shade(color, LightShade, 0.0);
            else if( highThreshold( color ) ) out = color;
            else out = shade(color, LightShade, backgroundContrast() );
            m_backgroundRadialColorCache.insert( color.toInt(), out );
            return out;
        }

    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::lightColor(const ColorUtils::Rgba &color)
    {

        ColorCache::const_iterator iter( m_lightColorCache.find( color.toInt() ) );
        if( iter != m_lightColorCache.end() ) return iter->second;
        else {
            const Rgba out( highThreshold( color ) ? color: shade( color, LightShade, contrast() ) );
            m_lightColorCache.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::darkColor( const ColorUtils::Rgba& color )
    {
        ColorCache::const_iterator iter( m_darkColorCache.find( color.toInt() ) );
        if( iter != m_darkColorCache.end() ) return iter->second;
        else {
            const Rgba out( lowThreshold(color) ?
                mix( lightColor(color), color, 0.3 + 0.7 * contrast() ):
                shade(color, MidShade, contrast() ) );
            m_darkColorCache.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::midColor( const ColorUtils::Rgba& color )
    {
        ColorCache::const_iterator iter( m_midColorCache.find( color.toInt() ) );
        if( iter != m_midColorCache.end() ) return iter->second;
        else {
            const Rgba out( shade( color, MidShade, contrast() - 1.0 ) );
            m_midColorCache.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::shadowColor( const ColorUtils::Rgba& color )
    {
        ColorCache::const_iterator iter( m_shadowColorCache.find( color.toInt() ) );
        if( iter != m_shadowColorCache.end() ) return iter->second;
        else {

            Rgba out( mix( Rgba::black(), color, color.alpha() ) );
            if( !lowThreshold(color) ) out = shade( out, ShadowShade, contrast() );
            m_shadowColorCache.insert( color.toInt(), out );
            return out;
        }
    }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::decoColor( const ColorUtils::Rgba& background, const ColorUtils::Rgba& color )
    { return mix( background, color, 0.4 + 0.8*contrast() ); }

    //_________________________________________________________________________
    ColorUtils::Rgba ColorUtils::alphaColor( const ColorUtils::Rgba& color, double alpha )
    { return Rgba( color.red(), color.green(), color.blue(), normalize(alpha)*color.alpha() ); }

    //____________________________________________________________________
    ColorUtils::Rgba ColorUtils::backgroundColor(const ColorUtils::Rgba &color, double ratio )
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

    namespace ColorUtils
    {

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
        static inline Rgba tintHelper( const Rgba &base, const Rgba &color, double amount )
        {
            HCY result( mix( base, color, pow( amount, 0.3 ) ) );
            result.y = mixdouble( luma( base ), result.y, amount );

            return result.rgba();
        }

    }

    //___________________________________________________________________
    double ColorUtils::luma( const ColorUtils::Rgba &color )
    {

        // RGB ratios
        return
            gamma( color.red() )*yc[0] +
            gamma( color.green() )*yc[1] +
            gamma( color.blue() )*yc[2];
    }

    //___________________________________________________________________
    double ColorUtils::contrastRatio( const ColorUtils::Rgba &c1, const ColorUtils::Rgba &c2 )
    {
        double y1 = luma( c1 ), y2 = luma( c2 );
        if( y1 > y2 ) return ( y1 + 0.05 ) / ( y2 + 0.05 );
        else return ( y2 + 0.05 ) / ( y1 + 0.05 );
    }

    //___________________________________________________________________
    ColorUtils::Rgba ColorUtils::lighten( const ColorUtils::Rgba &color, double ky, double kc )
    {
        HCY c( color );
        c.y = 1.0 - normalize( ( 1.0 - c.y ) * ( 1.0 - ky ) );
        c.c = 1.0 - normalize( ( 1.0 - c.c ) * kc );
        return c.rgba();
    }

    //___________________________________________________________________
    ColorUtils::Rgba ColorUtils::darken( const ColorUtils::Rgba &color, double ky, double kc )
    {
        HCY c( color );
        c.y = normalize( c.y * ( 1.0 - ky ) );
        c.c = normalize( c.c * kc );
        return c.rgba();
    }

    //___________________________________________________________________
    ColorUtils::Rgba ColorUtils::tint( const ColorUtils::Rgba &base, const ColorUtils::Rgba &color, double amount )
    {
        if( amount <= 0.0 ) return base;
        if( amount >= 1.0 ) return color;
        if( isnan( amount ) ) return base;

        double ri = contrastRatio( base, color );
        double rg = 1.0 + ( ( ri + 1.0 ) * amount * amount * amount );
        double u = 1.0;
        double l = 0.0;
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
    ColorUtils::Rgba ColorUtils::mix( const ColorUtils::Rgba &c1, const ColorUtils::Rgba &c2, double bias )
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
    ColorUtils::Rgba ColorUtils::shade(const ColorUtils::Rgba &color, ColorUtils::ShadeRole role)
    { return shade(color, role, _contrast ); }

    //___________________________________________________________________
    ColorUtils::Rgba ColorUtils::shade(const ColorUtils::Rgba &color, ColorUtils::ShadeRole role, double contrast, double chromaAdjust)
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
    ColorUtils::Rgba ColorUtils::shade( const ColorUtils::Rgba &color, double ky, double kc )
    {
        HCY c( color );
        c.y = normalize( c.y + ky );
        c.c = normalize( c.c + kc );
        return c.rgba();
    }


}
