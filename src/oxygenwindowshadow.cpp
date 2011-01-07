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
#include "oxygencairoutils.h"
#include "oxygencolorutils.h"

namespace Oxygen
{

    //________________________________________________________________________________
    void WindowShadow::render(cairo_t* cr, gint x, gint y, gint w, gint h)
    {
        // FIXME: is this correct color
        ColorUtils::Rgba background = settings().palette().color(Palette::Window);
        TileSet* tileset=tileSet(background, _wopt&WinDeco::Active);
        tileset->render(cr,x,y,w,h,TileSet::Full);
    }

    //________________________________________________________________________________
    TileSet* WindowShadow::tileSet(const ColorUtils::Rgba& color, double opacity)
    {
        // TODO: implement real opacity to get animated transition from active to inactive
        // For now, opacity==0 means inactive, otherwise active

        double size( shadowSize() );

        GdkPixbuf* shadow = shadowPixmap(color,opacity);
        TileSet* tileSet = new TileSet(shadow,int(size),int(size),1,1);
        return tileSet;
    }

    //________________________________________________________________________________
    GdkPixbuf* WindowShadow::shadowPixmap(const ColorUtils::Rgba& color, bool active)
    {
        ShadowConfiguration& shadowConfiguration( active ? activeShadowConfiguration_ : inactiveShadowConfiguration_ );

        static const double fixedSize=25.5;
        double size( shadowSize() );
        double shadowSize( shadowConfiguration.isEnabled() ? shadowConfiguration.shadowSize() : 0 );

        GdkPixbuf* shadow( gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, int(size*2), int(size*2) ) );
        gdk_pixbuf_fill( shadow, ColorUtils::Rgba::transparent().toInt() );

        Cairo::Context p(shadow);

        // some gradients rendering are different at bottom corners if client has no border
        bool hasBorder( true || _wopt&WinDeco::Shaded ); // TODO: true -> hasBorder

        if( shadowSize )
        {
            if( active && true ) // TODO: true -> useOxygenShadows
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
                    renderGradient(p, rect, rg, hasBorder);
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
                    renderGradient(p, rect, rg, hasBorder);
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
        }

        // draw the corner of the window - actually all 4 corners as one circle
        // this is all fixedSize. Does not scale with shadow size
        Cairo::Pattern lg( cairo_pattern_create_linear(0,size-4.5,0,size-4.5) );
        cairo_pattern_add_color_stop(lg, 0.0,  ColorUtils::lightColor( ColorUtils::backgroundTopColor( color ) ) );
        cairo_pattern_add_color_stop(lg, 0.51, ColorUtils::backgroundBottomColor(color));
        cairo_pattern_add_color_stop(lg, 1.0,  ColorUtils::backgroundBottomColor(color));

        cairo_set_source(p, lg);
        cairo_ellipse(p, size-4,size-4,8,8);
        cairo_fill(p);

        p.updateGdkPixbuf();

        return shadow;
    }
    //________________________________________________________________________________
    void WindowShadow::renderGradient(cairo_t* p,const GdkRectangle& rect, cairo_pattern_t* rg, bool hasBorder) const
    {
        if( hasBorder || true ) // TODO: implement remaining part of this method and remove '|| true '
        {
            cairo_set_source(p,rg);
            gdk_cairo_rectangle(p, &rect);
            cairo_fill(p);
            return;
        }
    }
}
