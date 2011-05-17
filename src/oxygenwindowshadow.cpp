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

#include "oxygenwindowshadow.h"
#include "oxygencachekey.h"
#include "oxygencairoutils.h"
#include "oxygencolorutils.h"

namespace Oxygen
{

    //________________________________________________________________________________
    void WindowShadow::render(cairo_t* cr, gint x, gint y, gint w, gint h)
    {
        ColorUtils::Rgba background = settings().palette().color(Palette::Window);
        WindowShadowKey key;
        key.active=_wopt&WinDeco::Active;
        tileSet( background, key).render( cr, x, y, w, h, TileSet::Full );
    }

    //________________________________________________________________________________
    const TileSet& WindowShadow::tileSet(const ColorUtils::Rgba& color, WindowShadowKey key) const
    {

        // check if tileset already in cache
        const TileSet& tileSet( helper().windowShadowCache().value(key) );
        if( tileSet.isValid() ) return tileSet;

        const double size( shadowSize() );
        return helper().windowShadowCache().insert(key, TileSet( shadowPixmap( color, key ), int(size), int(size), 1, 1 ) );

    }

    //________________________________________________________________________________
    Cairo::Surface WindowShadow::shadowPixmap(const ColorUtils::Rgba& color, const WindowShadowKey& key ) const
    {

        const bool active( key.active );
        const ShadowConfiguration& shadowConfiguration( active ? activeShadowConfiguration_ : inactiveShadowConfiguration_ );

        static const double fixedSize=25.5;
        double size( shadowSize() );
        double shadowSize( shadowConfiguration.isEnabled() ? shadowConfiguration.shadowSize() : 0 );

        Cairo::Surface shadow( helper().createSurface( int(size*2), int(size*2) ) );
        Cairo::Context p(shadow);

        // some gradients rendering are different at bottom corners if client has no border
        const bool hasTopBorder( key.hasTopBorder );
        const bool hasBottomBorder( key.hasBottomBorder );

        if( shadowSize )
        {

            if( active )
            {
                {
                    // inner (shark) gradient
                    const double gradientSize = std::min( shadowSize, (shadowSize+fixedSize)/2 );
                    const double hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                    const double voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                    Cairo::Pattern rg( cairo_pattern_create_radial( size+12.0*hoffset, size+12.0*voffset, gradientSize ) );
                    cairo_pattern_add_color_stop( rg, 1, ColorUtils::Rgba::transparent() );

                    // gaussian shadow is used
                    int nPoints( int( (10*gradientSize)/fixedSize ) );
                    Gaussian f( 0.85, 0.17 );
                    ColorUtils::Rgba c(shadowConfiguration.innerColor());
                    for( double i=0; i < nPoints; i++ )
                    {
                        double x = i/nPoints;
                        c.setAlpha( f(x) );
                        cairo_pattern_add_color_stop( rg, x, c );
                    }

                    cairo_set_source(p, rg);
                    GdkRectangle rect={0,0,int(size*2),int(size*2)};
                    renderGradient(p, rect, rg, hasTopBorder, hasBottomBorder );
                }

                {
                    // outer (spread) gradient
                    const double gradientSize = shadowSize;
                    const double hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                    const double voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                    Cairo::Pattern rg( cairo_pattern_create_radial( size+12.0*hoffset, size+12.0*voffset, gradientSize ) );
                    cairo_pattern_add_color_stop( rg, 1, ColorUtils::Rgba::transparent() );

                    // gaussian shadow is used
                    int nPoints( int( (10*gradientSize)/fixedSize ) );
                    Gaussian f( 0.46, 0.34 );
                    ColorUtils::Rgba c(shadowConfiguration.outerColor());
                    for( double i=0; i < nPoints; i++ )
                    {
                        double x = i/nPoints;
                        c.setAlpha( f(x) );
                        cairo_pattern_add_color_stop( rg, x, c );
                    }

                    cairo_set_source(p, rg);
                    cairo_rectangle(p, 0,0,size*2,size*2);
                    cairo_fill(p);
                }
            } else {

                {
                    // inner (sharp) gradient
                    const double gradientSize = std::min( shadowSize, fixedSize );
                    const double hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                    const double voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                    Cairo::Pattern rg( cairo_pattern_create_radial( size+hoffset, size+voffset, gradientSize ) );
                    cairo_pattern_add_color_stop( rg, 1, ColorUtils::Rgba::transparent() );

                    // parabolic shadow is used
                    int nPoints( int( (10*gradientSize)/fixedSize ) );
                    Parabolic f( 1, 0.22 );
                    ColorUtils::Rgba c(shadowConfiguration.outerColor());
                    for( double i=0; i < nPoints; i++ )
                    {
                        double x = i/nPoints;
                        c.setAlpha( f(x) );
                        cairo_pattern_add_color_stop( rg, x, c );
                    }

                    cairo_set_source(p, rg);
                    GdkRectangle rect={0,0,int(size*2),int(size*2)};
                    renderGradient(p, rect, rg, hasTopBorder, hasBottomBorder );
                }

                {
                    // mid gradient
                    const double gradientSize = std::min( shadowSize, (shadowSize+2*fixedSize)/3 );
                    const double hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                    const double voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                    Cairo::Pattern rg( cairo_pattern_create_radial( size+8.0*hoffset, size+8.0*voffset, gradientSize ) );
                    cairo_pattern_add_color_stop( rg, 1, ColorUtils::Rgba::transparent() );

                    // gaussian shadow is used
                    int nPoints( int( (10*gradientSize)/fixedSize ) );
                    Gaussian f( 0.54, 0.21 );
                    ColorUtils::Rgba c(shadowConfiguration.outerColor());
                    for( double i=0; i < nPoints; i++ )
                    {
                        double x = i/nPoints;
                        c.setAlpha( f(x) );
                        cairo_pattern_add_color_stop( rg, x, c );
                    }

                    cairo_set_source(p, rg);
                    cairo_rectangle(p, 0,0,size*2,size*2);
                    cairo_fill(p);
                }

                {
                    // outer (spread) gradient
                    const double gradientSize = shadowSize;
                    const double hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                    const double voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                    Cairo::Pattern rg( cairo_pattern_create_radial( size+20.0*hoffset, size+20.0*voffset, gradientSize ) );
                    cairo_pattern_add_color_stop( rg, 1, ColorUtils::Rgba::transparent() );

                    // gaussian shadow is used
                    int nPoints( int( (20*gradientSize)/fixedSize ) );
                    Gaussian f( 0.155, 0.445 );
                    ColorUtils::Rgba c(shadowConfiguration.outerColor());
                    for( double i=0; i < nPoints; i++ )
                    {
                        double x = i/nPoints;
                        c.setAlpha( f(x) );
                        cairo_pattern_add_color_stop( rg, x, c );
                    }

                    cairo_set_source(p, rg);
                    cairo_rectangle(p, 0,0,size*2,size*2);
                    cairo_fill(p);
                }
            }

            cairo_set_source_rgb(p,0,0,0);
            cairo_set_operator(p,CAIRO_OPERATOR_CLEAR);
            cairo_ellipse(p, size-3,size-3,6,6);
            cairo_fill(p);

        }

        return shadow;
    }

    //________________________________________________________________________________
    void WindowShadow::renderGradient( cairo_t* p, const GdkRectangle& rect, cairo_pattern_t* rg, bool hasTopBorder, bool hasBottomBorder ) const
    {
        if( hasTopBorder && hasBottomBorder )
        {
            cairo_set_source(p,rg);
            gdk_cairo_rectangle(p, &rect);
            cairo_fill(p);
            return;
        }

        double size( rect.width/2.0 );

        // get pattern definition
        double x0(0), y0(0), r0(0);
        double x1(0), y1(0), r1(0);
        const cairo_status_t status( cairo_pattern_get_radial_circles( rg, &x0, &y0, &r0, &x1, &y1, &r1 ) );
        assert( status == CAIRO_STATUS_SUCCESS );

        const double hoffset( x0 - size );
        const double voffset( y0 - size );
        const double radius( r1 );

        ColorStop::List stops( cairo_pattern_get_color_stops( rg ) );

        // draw ellipse for the upper rect
        if( hasTopBorder ) {
            cairo_set_source( p, rg );
            cairo_rectangle( p, hoffset, voffset, 2*size-hoffset, size );
            cairo_fill( p );
        } else {

            // draw square gradients for the lower rect
            {
                // vertical lines
                Cairo::Pattern pattern( cairo_pattern_create_linear( hoffset, 0.0, 2*size+hoffset, 0.0 ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    const ColorUtils::Rgba c( stops[i]._color );
                    const double x( stops[i]._x * radius );
                    cairo_pattern_add_color_stop( pattern, (size-x)/(2.0*size), c );
                    cairo_pattern_add_color_stop( pattern, (size+x)/(2.0*size), c );
                }

                cairo_set_source( p, pattern );
                cairo_rectangle( p, hoffset, size+voffset-4, 2*size-hoffset, 4 );
                cairo_fill( p );
            }

            {

                // horizontal line
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, voffset, 0, 2*size+voffset ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    const ColorUtils::Rgba c( stops[i]._color );
                    const double x( stops[i]._x * radius );
                    cairo_pattern_add_color_stop( pattern, (size-x)/(2.0*size), c );
                }

                cairo_set_source( p, pattern );
                cairo_rectangle( p, size-4+hoffset, voffset, 8, size );
                cairo_fill( p );

            }

            {

                // bottom-left corner
                Cairo::Pattern pattern( cairo_pattern_create_radial( size+hoffset-4, size+voffset-4, radius ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    ColorUtils::Rgba c( stops[i]._color );
                    double x( stops[i]._x -4.0/radius );
                    if( x<0 )
                    {
                        if( i < stops.size()-1 )
                        {
                            const double x1( stops[i+1]._x - 4.0/radius );
                            c = ColorUtils::mix( c, stops[i+1]._color, -x/(x1-x) );
                        }

                        x = 0;

                    }

                    cairo_pattern_add_color_stop( pattern, x, c );

                }


                cairo_set_source( p, pattern );
                cairo_rectangle( p, hoffset, voffset-4, size-4, size );
                cairo_fill( p );

            }

            {

                // bottom-right corner
                Cairo::Pattern pattern( cairo_pattern_create_radial( size+hoffset+4, size+voffset-4, radius ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    ColorUtils::Rgba c( stops[i]._color );
                    double x( stops[i]._x -4.0/radius );
                    if( x<0 )
                    {
                        if( i < stops.size()-1 )
                        {
                            const double x1( stops[i+1]._x - 4.0/radius );
                            c = ColorUtils::mix( c, stops[i+1]._color, -x/(x1-x) );
                        }

                        x = 0;

                    }

                    cairo_pattern_add_color_stop( pattern, x, c );

                }


                cairo_set_source( p, pattern );
                cairo_rectangle( p, size+hoffset+4, voffset-4, size-4, size );
                cairo_fill( p );

            }

        }

        // Bottom part
        if( hasBottomBorder ) {

            cairo_set_source( p, rg );
            cairo_rectangle( p, hoffset, voffset+size, 2*size-hoffset, size );
            cairo_fill( p );

        } else {

            // draw square gradients for the lower rect
            {
                // vertical lines
                Cairo::Pattern pattern( cairo_pattern_create_linear( hoffset, 0.0, 2*size+hoffset, 0.0 ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    const ColorUtils::Rgba c( stops[i]._color );
                    const double x( stops[i]._x * radius );
                    cairo_pattern_add_color_stop( pattern, (size-x)/(2.0*size), c );
                    cairo_pattern_add_color_stop( pattern, (size+x)/(2.0*size), c );
                }

                cairo_set_source( p, pattern );
                cairo_rectangle( p, hoffset, size+voffset, 2*size-hoffset, 4 );
                cairo_fill( p );
            }

            {

                // horizontal line
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, voffset, 0, 2*size+voffset ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    const ColorUtils::Rgba c( stops[i]._color );
                    const double x( stops[i]._x * radius );
                    cairo_pattern_add_color_stop( pattern, (size+x)/(2.0*size), c );
                }

                cairo_set_source( p, pattern );
                cairo_rectangle( p, size-4+hoffset, size+voffset, 8, size );
                cairo_fill( p );

            }

            {

                // bottom-left corner
                Cairo::Pattern pattern( cairo_pattern_create_radial( size+hoffset-4, size+voffset+4, radius ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    ColorUtils::Rgba c( stops[i]._color );
                    double x( stops[i]._x -4.0/radius );
                    if( x<0 )
                    {
                        if( i < stops.size()-1 )
                        {
                            const double x1( stops[i+1]._x - 4.0/radius );
                            c = ColorUtils::mix( c, stops[i+1]._color, -x/(x1-x) );
                        }

                        x = 0;

                    }

                    cairo_pattern_add_color_stop( pattern, x, c );

                }


                cairo_set_source( p, pattern );
                cairo_rectangle( p, hoffset, size+voffset+4, size-4, size );
                cairo_fill( p );

            }

            {

                // bottom-right corner
                Cairo::Pattern pattern( cairo_pattern_create_radial( size+hoffset+4, size+voffset+4, radius ) );
                for( unsigned int i = 0; i < stops.size(); ++i )
                {
                    ColorUtils::Rgba c( stops[i]._color );
                    double x( stops[i]._x -4.0/radius );
                    if( x<0 )
                    {
                        if( i < stops.size()-1 )
                        {
                            const double x1( stops[i+1]._x - 4.0/radius );
                            c = ColorUtils::mix( c, stops[i+1]._color, -x/(x1-x) );
                        }

                        x = 0;

                    }

                    cairo_pattern_add_color_stop( pattern, x, c );

                }


                cairo_set_source( p, pattern );
                cairo_rectangle( p, size+hoffset+4, size+voffset+4, size-4, size );
                cairo_fill( p );

            }

        }

    }
}
