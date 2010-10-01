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

#include "oxygenstylehelper.h"
#include "oxygencairocontext.h"
#include "oxygencairoutils.h"
#include "oxygencolorutils.h"
#include "oxygenrgba.h"

#include <cmath>
#include <gdk/gdk.h>

namespace Oxygen
{

    //__________________________________________________________________
    const double StyleHelper::_slabThickness = 0.45;
    const double StyleHelper::_shadowGain = 1.5;
    const double StyleHelper::_glowBias = 0.6;

    //__________________________________________________________________
    void StyleHelper::drawSeparator( Cairo::Context& context, const ColorUtils::Rgba& color, int x, int y, int w, int h, bool vertical ) const
    {

        cairo_save( context );
        cairo_set_line_width( context, 1.0 );

        // compute limits
        int xStart( vertical ? x+w/2-1 : x );
        int yStart( vertical ? y : y+h/2-1 );
        int xStop( vertical ? x+w/2-1 : x+w );
        int yStop( vertical ? y+h : y+h/2-1 );
        int xOffset( vertical ? 1:0 );
        int yOffset( vertical ? 0:1 );

        if( vertical ) cairo_translate( context, 0.5, 0 );
        else cairo_translate( context, 0, 1.5 );


        {
            ColorUtils::Rgba light( ColorUtils::lightColor( color ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( xStart, yStart, xStop, yStop ) );
            cairo_pattern_add_color_stop( pattern, 0.3, light );
            cairo_pattern_add_color_stop( pattern, 0.7, light );
            light.setAlpha( 0 );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, light );
            cairo_set_source( context, pattern );

            if( vertical )
            {
                cairo_move_to( context, xStart, yStart );
                cairo_line_to( context, xStop, yStop );
                cairo_move_to( context, xStart+2*xOffset, yStart+2*yOffset );
                cairo_line_to( context, xStop+2*xOffset, yStop+2*yOffset );
            } else {
                cairo_move_to( context, xStart+xOffset, yStart+yOffset );
                cairo_line_to( context, xStop+xOffset, yStop+yOffset );
            }

            cairo_stroke( context );
        }

        {
            ColorUtils::Rgba dark( ColorUtils::darkColor( color ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( xStart, yStart, xStop, yStop ) );
            cairo_pattern_add_color_stop( pattern, 0.3, dark );
            cairo_pattern_add_color_stop( pattern, 0.7, dark );
            dark.setAlpha( 0 );
            cairo_pattern_add_color_stop( pattern, 0, dark );
            cairo_pattern_add_color_stop( pattern, 1, dark );
            cairo_set_source( context, pattern );

            if( vertical )
            {
                cairo_move_to( context, xStart+xOffset, yStart+yOffset );
                cairo_line_to( context, xStop+xOffset, yStop+yOffset );
            } else {
                cairo_move_to( context, xStart, yStart );
                cairo_line_to( context, xStop, yStop );
            }

            cairo_stroke( context );
        }

        cairo_restore( context );

    }

    //_________________________________________________
    TileSet StyleHelper::slab(const ColorUtils::Rgba& color, double shade, int size) const
    {

        // create pixbuf and initialize
        const int w( 2*size );
        const int h( 2*size );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            // create cairo context
            Cairo::Context context( pixbuf );
            cairo_scale( context, double(size)/7, double(size)/7 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::alphaColor( color, 0 ) );
            cairo_fill( context );

            if( color.isValid() )
            {
                drawShadow( context, ColorUtils::shadowColor( color ), 14 );
                drawSlab( context, color, shade );
            }

            context.updateGdkPixbuf();
       }

        // create tileSet
        TileSet out( pixbuf,  size, size, size, size, size-1, size, 2, 1);
        g_object_unref( pixbuf );
        return out;
    }

    //_________________________________________________
    TileSet StyleHelper::slabFocused(const ColorUtils::Rgba& color, const ColorUtils::Rgba& glow, double shade, int size) const
    {

        // create pixbuf and initialize
        const int w( 2*size );
        const int h( 2*size );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {
            // create cairo context
            Cairo::Context context( pixbuf );
            cairo_scale( context, double(size)/7, double(size)/7 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::alphaColor( color, 0 ) );
            cairo_fill( context );

            if( color.isValid() ) drawShadow( context, ColorUtils::shadowColor( color ), 14 );
            if( glow.isValid() ) drawOuterGlow( context, glow, 14 );
            if( color.isValid() ) drawSlab( context, color, shade );
            context.updateGdkPixbuf();

        }

        // create tileSet
        TileSet out( pixbuf,  size, size, size, size, size-1, size, 2, 1);
        g_object_unref( pixbuf );
        return out;
    }

    //__________________________________________________________________
    TileSet StyleHelper::slabSunken( const ColorUtils::Rgba& color, double shade, int size ) const
    {


        // create pixbuf and initialize
        const int w( 2*size );
        const int h( 2*size );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            // create cairo context
            Cairo::Context context( pixbuf );
            cairo_scale( context, double(size)/7, double(size)/7 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::alphaColor( color, 0 ) );
            cairo_fill( context );

            if( color.isValid() )
            {
                drawSlab( context, color, shade );
                drawInverseShadow( context, ColorUtils::shadowColor(color), 3, 8, 0.0);
            }
            context.updateGdkPixbuf();

        }

        // create tileSet
        TileSet out( pixbuf,  size, size, size, size, size-1, size, 2, 1);
        g_object_unref( pixbuf );
        return out;

    }

    //______________________________________________________________________________
    GdkPixbuf* StyleHelper::roundSlab( const ColorUtils::Rgba& color, double shade, int size ) const
    {

        const int w( 3*size );
        const int h( 3*size );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            // create cairo context
            Cairo::Context context( pixbuf );
            cairo_scale( context, double(size)/7, double(size)/7 );

            // shadow
            if( color.isValid() )
            {
                drawShadow( context, ColorUtils::shadowColor(color), 21 );
                drawRoundSlab( context, color, shade );
            }

            context.updateGdkPixbuf();

        }

        return pixbuf;
    }

    //__________________________________________________________________________________________________________
    GdkPixbuf* StyleHelper::roundSlabFocused(const ColorUtils::Rgba& color, const ColorUtils::Rgba& glow, double shade, int size) const
    {

        const int w( 3*size );
        const int h( 3*size );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            // create cairo context
            Cairo::Context context( pixbuf );
            cairo_scale( context, double(size)/7, double(size)/7 );

            // shadow
            if( color.isValid() ) drawShadow( context, ColorUtils::shadowColor(color), 21 );
            if( glow.isValid() ) drawOuterGlow( context, glow, 21 );
            if( color.isValid() ) drawRoundSlab( context, color, shade );

            context.updateGdkPixbuf();

        }

        return pixbuf;
    }

    //__________________________________________________________________
    void StyleHelper::fillSlab( Cairo::Context& context, int x, int y, int w, int h ) const
    {
        const double s( 3.6 + (0.5 * _slabThickness) );
        const double r( s/2 );
        cairo_rounded_rectangle( context, x+s, y+s, w-2*s, h-2*s, r );
        cairo_fill( context );
    }

    //________________________________________________________________________________________________________
    //TileSet StyleHelper::hole( const ColorUtils::Rgba& color, double shade, int size ) const
    TileSet StyleHelper::hole( const ColorUtils::Rgba &color, double shade, int size, bool fill ) const
    {

        // create pixbuf and initialize
        const int rsize( (int)ceil(double(size) * 5.0/7.0 ) );
        const int w( 2*rsize );
        const int h( 2*rsize );

        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            Cairo::Context context( pixbuf );
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 10.0/w, 10.0/h );

            // inside
            if( fill )
            {
                cairo_ellipse( context, 3, 3, 8, 8 );
                cairo_set_source( context, ColorUtils::Rgba::white() );
                cairo_fill( context );
            }

            // shadow
            drawInverseShadow( context, ColorUtils::shadowColor( color ), 3, 8, 0.0);
            context.updateGdkPixbuf();

        }

        TileSet out(pixbuf, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1);
        g_object_unref( pixbuf );
        return out;

    }

    //________________________________________________________________________________________________________
    TileSet StyleHelper::holeFlat( const ColorUtils::Rgba& color, double shade, int size ) const
    {

        const int rsize( ( int )ceil( double( size ) * 5.0/7.0 ) );
        const int w( 2*rsize );
        const int h( 2*rsize );

        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            Cairo::Context context( pixbuf );
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 10.0/w, 10.0/h );

            // hole
            drawHole( context, color, shade, 7 );

            // hole inside
            cairo_set_source( context, color );
            cairo_ellipse( context, 3.4, 3.4, 7.2, 7.2 );
            cairo_fill( context );
            context.updateGdkPixbuf();

        }


        TileSet out( pixbuf, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 );
        g_object_unref( pixbuf );
        return out;

    }

    //______________________________________________________________________________
    TileSet StyleHelper::scrollHole( const ColorUtils::Rgba& color, bool vertical ) const
    {

        // define colors
        const ColorUtils::Rgba& dark( ColorUtils::darkColor( color ) );
        const ColorUtils::Rgba& light( ColorUtils::lightColor( color ) );
        const ColorUtils::Rgba& shadow( ColorUtils::shadowColor( color ) );

        // create pixmap
        const int w( 15 );
        const int h( 15 );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::Rgba::transparent().toInt() );
        {
            Cairo::Context context( pixbuf );

            GdkRectangle r = { 0, 0, w, h };
            GdkRectangle rect = { 1, 0, w-2, h-1 };

            const int shadowWidth( vertical ? 2:3 );

            // base
            {
                cairo_set_source( context, dark );
                gdk_cairo_rounded_rectangle( context, &rect, 4.5 );
                cairo_fill( context );
            }

            // light shadow across the whole hole
            {
                Cairo::Pattern pattern;
                if( vertical ) pattern.set( cairo_pattern_create_linear( rect.x, 0, rect.x + rect.width, 0 ) );
                else pattern.set( cairo_pattern_create_linear( 0, rect.y, 0, rect.y + rect.height ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.1 ) );
                cairo_pattern_add_color_stop( pattern, 0.6, ColorUtils::alphaColor( shadow, 0 ) );

                cairo_set_source( context, pattern );
                gdk_cairo_rounded_rectangle( context, &rect, 4.5 );
                cairo_fill( context );
            }


            // strong shadow
            {
                // left
                Cairo::Pattern pattern( cairo_pattern_create_linear( rect.x, 0, rect.x + shadowWidth, 0 ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, vertical ? 0.2 : 0.3 ) );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( shadow, 0.1 ) );
                cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( shadow, 0 ) );

                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, rect.x, rect.y, shadowWidth, rect.height, shadowWidth, CornersLeft );
                cairo_fill( context );
            }

            {
                // right
                Cairo::Pattern pattern( cairo_pattern_create_linear( rect.x + rect.width - shadowWidth, 0, rect.x + rect.width, 0 ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::Rgba::transparent() );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( shadow, 0.2 ) );
                cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( shadow, vertical ? 0.2 : 0.3 ) );

                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, rect.x + rect.width - shadowWidth, rect.y, shadowWidth, rect.height, shadowWidth, CornersRight );
                cairo_fill( context );
            }

            {
                // top
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, rect.y, 0, rect.y+3 ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.3 ) );
                cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent() );

                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, rect.x, rect.y, rect.width, 3, 3, CornersTop );
                cairo_fill( context );
            }


            // light bottom border
            {
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, r.y + r.height/2 - 1, 0, r.y + r.height ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( light, 0 ) );
                cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( light, 0.8 ) );

                cairo_set_source( context, pattern );
                cairo_set_line_width( context, 1.0 );
                cairo_rounded_rectangle( context, r.x+0.5, r.y, r.width-1, r.height, 5.0 );
                cairo_stroke( context );
            }

            context.updateGdkPixbuf();
        }

        TileSet out( pixbuf, 7, 7, 1, 1 );
        g_object_unref( pixbuf );
        return out;
    }

    //____________________________________________________________________
    GdkPixbuf* StyleHelper::progressBarIndicator(const ColorUtils::Rgba& base, const ColorUtils::Rgba& highlight, int w, int h ) const
    {

        // local rect
        int xl = 0;
        int yl = 0;
        int wl = w+2;
        int hl = h+3;

        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, wl, hl ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( base, 0 ).toInt() );

        {

            // context
            Cairo::Context context( pixbuf );

            // adjust rect
            xl += 1;
            yl += 1;
            wl -= 2;
            hl -= 2;

            // colors
            const ColorUtils::Rgba lhighlight( ColorUtils::lightColor( highlight ) );
            const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
            const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );
            const ColorUtils::Rgba shadow( ColorUtils::shadowColor( base ) );

            {
                // shadow
                cairo_rounded_rectangle( context, double(xl)+0.5, double(yl)-0.5, wl, hl+2, 2 );
                cairo_set_source( context, ColorUtils::alphaColor( shadow, 0.6 ) );
                cairo_set_line_width( context, 0.6 );
                cairo_stroke( context );
            }

            {
                // filling
                cairo_set_source( context, ColorUtils::mix( highlight, dark, 0.2 ) );
                cairo_rectangle( context, xl+1, yl, wl-2, hl );
                cairo_fill( context );
            }

            // create pattern pixbuf
            wl--;
            {
                GdkPixbuf* localbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, wl, hl ) );
                gdk_pixbuf_fill( localbuf, ColorUtils::Rgba::transparent().toInt() );

                Cairo::Pattern mask( cairo_pattern_create_linear( 0, 0, wl, 0 ) );
                cairo_pattern_add_color_stop( mask, 0, ColorUtils::Rgba::transparent() );
                cairo_pattern_add_color_stop( mask, 0.4, ColorUtils::Rgba::black() );
                cairo_pattern_add_color_stop( mask, 0.6, ColorUtils::Rgba::black() );
                cairo_pattern_add_color_stop( mask, 1.0, ColorUtils::Rgba::transparent() );

                const ColorUtils::Rgba mix( ColorUtils::mix( lhighlight, light, 0.3 ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, hl ) );
                cairo_pattern_add_color_stop( pattern, 0,  mix );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( mix, 0 ) );
                cairo_pattern_add_color_stop( pattern, 0.6, ColorUtils::alphaColor( mix, 0 ) );
                cairo_pattern_add_color_stop( pattern, 1.0, mix );

                Cairo::Context localContext( localbuf );
                cairo_rectangle( localContext, 0, 0, wl, hl );
                cairo_set_source( localContext, pattern );
                cairo_mask( localContext, mask );
                localContext.updateGdkPixbuf();
                localContext.free();

                cairo_save( context );
                cairo_translate( context, 1, 1 );
                cairo_rectangle( context, 0, 0, wl, hl );
                gdk_cairo_set_source_pixbuf( context, localbuf, 0, 0 );
                cairo_fill( context );
                cairo_restore( context );
                g_object_unref( localbuf );

            }

            {
                // bevel
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, hl ) );
                cairo_pattern_add_color_stop( pattern, 0.0, lhighlight );
                cairo_pattern_add_color_stop( pattern, 0.5, highlight );
                cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::darkColor(highlight) );
                cairo_set_line_width( context, 1.0 );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, xl+0.5, yl+0.5, wl, hl, 1.5 );
                cairo_stroke( context );
            }

            {
                // bright top edge
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, wl, 0 ) );
                const ColorUtils::Rgba mix( ColorUtils::mix( lhighlight, light, 0.8 ) );
                cairo_pattern_add_color_stop( pattern, 0.0, ColorUtils::alphaColor( mix, 0 ) );
                cairo_pattern_add_color_stop( pattern, 0.5, mix );
                cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( mix, 0 ) );
                cairo_set_line_width( context, 1.0 );
                cairo_set_source( context, pattern );
                cairo_move_to( context, xl+0.5, yl+0.5 );
                cairo_line_to( context, xl+wl-0.5, yl+0.5 );
                cairo_stroke( context );
            }

            context.updateGdkPixbuf();
        }

        return pixbuf;

    }

    //________________________________________________________________________________________________________
    TileSet StyleHelper::groove( const ColorUtils::Rgba &base, double shade, int size ) const
    {

        const int rsize( ( int )ceil( double( size ) * 3.0/7.0 ) );
        const int w( rsize*2 );
        const int h( rsize*2 );
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::Rgba::transparent().toInt() );

       {

            Cairo::Context context( pixbuf );
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 6/w, 6/h );

            Cairo::Pattern pattern( inverseShadowGradient( ColorUtils::shadowColor( base ), 3, 4, 0.0 ) );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3, 3, 4, 4 );
            cairo_ellipse_negative( context, 4, 4, 2, 2 );
            cairo_fill( context );

            context.updateGdkPixbuf();

        }

        TileSet out( pixbuf, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 );
        g_object_unref( pixbuf );
        return out;

    }

    //____________________________________________________________________
    TileSet StyleHelper::selection( const ColorUtils::Rgba& color, int h, bool custom ) const
    {

        TileSet tileSet;

        const int w = 32+16;
        GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
        gdk_pixbuf_fill( pixbuf, ColorUtils::alphaColor( color, 0 ).toInt() );

        {

            // adjust height
            const int x = 0;
            const int y = 0;
            const double rounding( 2.5 );

            Cairo::Context context( pixbuf );
            cairo_set_line_width( context, 1.0 );

            {
                // filling
                const int lightenAmount( custom ? 110 : 130 );
                const ColorUtils::Rgba light( color.light( lightenAmount ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, y, 0, y+h ) );
                cairo_pattern_add_color_stop( pattern, 0, light );
                cairo_pattern_add_color_stop( pattern, 1, color );

                cairo_rounded_rectangle( context, x, y, w, h, rounding );
                cairo_set_source( context, pattern );
                cairo_fill( context );

                cairo_rounded_rectangle( context, double(x)+0.5, double(y)+0.5, w-1, h-1, rounding );
                cairo_set_source( context, color );
                cairo_stroke( context );

            }

            {
                // contrast
                cairo_rounded_rectangle( context, double(x)+1.5, double(y)+1.5, w-3, h-3, rounding-1 );
                cairo_set_source( context, ColorUtils::alphaColor( ColorUtils::Rgba::white(), 64.0/255 ) );
                cairo_stroke( context );
            }

            context.updateGdkPixbuf();

        }

        // create tileSet
        TileSet out( pixbuf, 8, 0, 32, h );
        g_object_unref( pixbuf );

        return out;

    }

    //____________________________________________________________________
    void StyleHelper::renderDot( Cairo::Context& context, const ColorUtils::Rgba& color, int x, int y ) const
    {

        const double diameter( 1.8 );
        const ColorUtils::Rgba light( ColorUtils::lightColor( color ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( color ).dark(130).setAlpha( 0.65 ) );

        cairo_ellipse( context, double(x) + 1.0 - diameter/2, double(y) + 1.0 - diameter/2.0, diameter, diameter );
        cairo_set_source( context, light );
        cairo_fill( context );

        cairo_ellipse( context, double(x) + 0.5 - diameter/2, double(y) + 0.5 - diameter/2.0, diameter, diameter );
        cairo_set_source( context, dark );
        cairo_fill( context );
    }

    //______________________________________________________________________________________
    void StyleHelper::drawSlab( Cairo::Context& context, const ColorUtils::Rgba& color, double shade) const
    {

        const ColorUtils::Rgba light( ColorUtils::shade( lightColor(color), shade ) );
        const ColorUtils::Rgba base( ColorUtils::alphaColor( light, 0.85 ) );
        const ColorUtils::Rgba dark( ColorUtils::shade( darkColor(color), shade ) );

        // mask dimensions
        const double ic = 3.6 + 0.5*_slabThickness;
        const double is = 14 - 2.0*ic;

        // bevel, part 1
        {

            const double y( ColorUtils::luma(base) );
            const double yl( ColorUtils::luma(light) );
            const double yd( ColorUtils::luma(dark) );

            // create pattern
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 7, 0, 11 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            if (y < yl && y > yd)
            {
                // no middle when color is very light/dark
                cairo_pattern_add_color_stop( pattern, 0.5, base );
            }

            cairo_pattern_add_color_stop( pattern, 0.9, base );
            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, 3.0, 3.0, 8, 8, 3.5 );
            cairo_ellipse_negative( context, ic, ic, is, is );
            cairo_fill( context );

        }

        // bevel, part 2
        if( _slabThickness > 0.0 )
        {

            /*
            for some reason need to make ic a bit thicker
            this is likely due to differences between cairo and qpainter
            as a result, need to make the second pattern brighter
            */
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 6, 0, 19 ) );
            cairo_pattern_add_color_stop( pattern, 0, light.light(200) );
            cairo_pattern_add_color_stop( pattern, 0.9, base.light(200) );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3.6, 3.6, 6.8, 6.8 );

            cairo_ellipse_negative( context, ic, ic, is, is );
            cairo_fill( context );

        }

    }

    //___________________________________________________________________________________________
    void StyleHelper::drawHole( Cairo::Context& context, const ColorUtils::Rgba& color, double shade, int r ) const
    {

        const int r2( 2*r );
        const ColorUtils::Rgba base( ColorUtils::shade( color, shade ) );
        const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor( color ), shade ) );
        const ColorUtils::Rgba dark( ColorUtils::shade( ColorUtils::darkColor( color ), shade ) );
        const ColorUtils::Rgba mid( ColorUtils::shade( ColorUtils::midColor( color ), shade ) );

        // bevel
        const double y( ColorUtils::luma( base ) );
        const double yl( ColorUtils::luma( light ) );
        const double yd( ColorUtils::luma( dark ) );

        Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 2, 0, r2-2 ) );
        cairo_pattern_add_color_stop( pattern, 0.2, dark );
        cairo_pattern_add_color_stop( pattern, 0.5, mid );
        cairo_pattern_add_color_stop( pattern, 1.0, light );
        if( y < yl && y > yd )
        {
            // no middle when color is very light/dark
            cairo_pattern_add_color_stop( pattern, 0.6, base );
        }

        cairo_set_source( context, pattern );
        cairo_ellipse( context, 3, 3, r2-6, r2-6 );
        cairo_fill( context );

    }

    //___________________________________________________________________________________________
    void StyleHelper::drawShadow( Cairo::Context& context, const ColorUtils::Rgba& color, int size) const
    {
        //const double m( double(size-1.5)*0.5 );
        const double m( double(size-2)*0.5 );
        const double offset( 0.8 );
        const double k0( (m-4.0) / m );

        const double x(m+1);
        const double y(m+offset+1);

        // mask dimensions
        const double ic = 3.6 + 0.5*_slabThickness;
        const double is = size - 2.0*ic;

        Cairo::Pattern pattern( cairo_pattern_create_radial(x, y, m ) );
        for (int i = 0; i < 8; i++)
        {
            // sinusoidal pattern
            const double k1( (k0 * double(8 - i) + double(i)) * 0.125 );
            const double a( (cos( M_PI * i * 0.125) + 1.0) * 0.30 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( color, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( color, 0 ) );
        cairo_set_source( context, pattern );
        cairo_ellipse( context, 0, 0, size, size );
        cairo_ellipse_negative( context, ic, ic, is, is );
        cairo_fill( context );

    }

    //_______________________________________________________________________
    void StyleHelper::drawOuterGlow( Cairo::Context& context, const ColorUtils::Rgba& color, int size ) const
    {

        const double m( double(size)*0.5 );

        // mask dimensions
        const double ic = 3.6 + 0.5*_slabThickness;
        const double is = size - 2.0*ic;

        const double w( 4 );

        const double bias( _glowBias * double(14)/size );

        // k0 is located at w - bias from the outer edge
        const double gm( m + bias - 0.9 );
        const double k0( (m-w+bias) / gm );


        const double x(m);
        const double y(m);

        Cairo::Pattern pattern( cairo_pattern_create_radial(x, y, m ) );
        for (int i = 0; i < 8; i++)
        {
            // parabolic pattern
            const double k1( k0 + double(i)*(1.0-k0)/8.0 );
            const double a( 1.0 - sqrt(double(i)/8) );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( color, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( color, 0 ) );

        cairo_set_source( context, pattern );
        cairo_ellipse( context, 0, 0, size, size );
        cairo_ellipse_negative( context, ic, ic, is, is );
        cairo_fill( context );

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseShadow(
        Cairo::Context& context, const ColorUtils::Rgba& color,
        int pad, int size, double fuzz ) const
    {

        Cairo::Pattern pattern( inverseShadowGradient( color, pad, size, fuzz ) );

        cairo_set_source( context, pattern );
        cairo_ellipse( context, pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0 );
        cairo_fill( context );

    }

    //__________________________________________________________________________________________________________
    void StyleHelper::drawRoundSlab( Cairo::Context& context, const ColorUtils::Rgba& color, double shade ) const
    {

        // colors
        const ColorUtils::Rgba base( ColorUtils::shade(color, shade) );
        const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor(color), shade) );

        // bevel, part 1
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 10, 0, 18 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.9, ColorUtils::alphaColor( light, 0.85 ) );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3, 3, 15, 15 );
            cairo_fill( context );
        }

        // bevel, part 2
        if( _slabThickness > 0 )
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 7, 0, 28 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.9, base );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3.6, 3.6, 13.8, 13.8 );
            cairo_fill( context );
        }

        // inside
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, -17, 0, 20 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, base );
            cairo_set_source( context, pattern );

            const double ic( 3.6 + _slabThickness );
            const double is( 21 - 2.0*ic );
            cairo_ellipse( context, ic, ic, is, is );
            cairo_fill( context );
        }

    }

    //________________________________________________________________________________________________________
    cairo_pattern_t* StyleHelper::inverseShadowGradient(
        const ColorUtils::Rgba& color,
        int pad, int size, double fuzz ) const
    {

        const double m( double(size)*0.5 );
        const double offset( 0.8 );
        const double k0( (m-2) / double(m+2.0) );

        const double x(pad+m);
        const double y(pad+m+offset);

        cairo_pattern_t* pattern( cairo_pattern_create_radial(x, y, m+2 ) );
        for (int i = 0; i < 8; i++)
        {
            // sinusoidal pattern
            const double k1( (double(8 - i) + k0 * double(i)) * 0.125 );
            const double a( (cos(3.14159 * i * 0.125) + 1.0) * 0.25 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( color, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, k0, ColorUtils::alphaColor( color, 0 ) );
        return pattern;

    }


}
