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
    StyleHelper::StyleHelper( void )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::StyleHelper::StyleHelper" << std::endl;
        #endif
    }

    //__________________________________________________________________
    StyleHelper::~StyleHelper( void )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::StyleHelper::~StyleHelper" << std::endl;
        #endif
    }

    //__________________________________________________________________
    void StyleHelper::initializeRefSurface( void )
    {

        // make sure that surface is not already initialized
        if( _refSurface.isValid() ) return;

        /*
        create dummy widget, check its associated screen;
        if found create surface for it and save as reference surface
        */
        GdkScreen* screen( gdk_screen_get_default() );
        GdkWindow* window( screen ? gdk_screen_get_root_window( screen ):0x0 );
        if( screen && window )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::StyleHelper::initializeRefSurface - "
                << " screen: " << screen
                << " window: " << window
                << std::endl;
            #endif

            Cairo::Context context( window );
            _refSurface = Cairo::Surface( cairo_surface_create_similar( cairo_get_target( context ), CAIRO_CONTENT_ALPHA, 1, 1 ) );

        } else {

            /*
            no screen found.
            Destroy widget (since gtk_widget_realize would otherwise crash)
            Fallback to ImageSurface
            */
            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::StyleHelper::initializeRefSurface - "
                << " No valid screen found to create X11 surface."
                << " Falling back to Cairo Image surface."
                << std::endl;
            #endif

            _refSurface.set( cairo_image_surface_create( CAIRO_FORMAT_ARGB32, 1, 1 ) );

        }

    }

    //__________________________________________________________________
    void StyleHelper::drawSeparator( Cairo::Context& context, const ColorUtils::Rgba& base, int x, int y, int w, int h, bool vertical )
    {

        // get surface
        const Cairo::Surface& surface( separator( base, vertical, vertical ? h:w ) );
        if(!surface) return;

        // translate
        cairo_save( context );
        if( vertical ) {

            cairo_translate( context, x+w/2-1, y );
            cairo_rectangle( context, 0, 0, 3, h );

        } else {

            cairo_translate( context, x, y+h/2 );
            cairo_rectangle( context, 0, 0, w, 2 );

        }

        cairo_set_source_surface( context, surface, 0, 0 );
        cairo_fill( context );
        cairo_restore( context );

    }

    //__________________________________________________________________
    const Cairo::Surface& StyleHelper::separator( const ColorUtils::Rgba& base, bool vertical, int size )
    {

        const SeparatorKey key( base, vertical, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = _separatorCache.value(key) )
        { return surface; }

        // for invalid sizes return a null surface
        if( size <= 0 )
        { return _separatorCache.insert( key, 0L ); }

        // cached not found, create new
        Cairo::Surface surface( vertical ? createSurface( 3, size ):createSurface( size, 2 ) );

        int xStart( 0 );
        int yStart( 0 );
        int xStop( vertical ? 0 : size );
        int yStop( vertical ? size : 0 );
        int xOffset( vertical ? 1:0 );
        int yOffset( vertical ? 0:1 );

        Cairo::Context context( surface );
        cairo_set_line_width( context, 1.0 );

        if( vertical ) cairo_translate( context, 0.5, 0 );
        else cairo_translate( context, 0, 0.5 );

        {
            ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( xStart, yStart, xStop, yStop ) );
            if( vertical ) light.setAlpha( 0.7 );

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
            ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

            Cairo::Pattern pattern( cairo_pattern_create_linear( xStart, yStart, xStop, yStop ) );
            cairo_pattern_add_color_stop( pattern, 0.3, dark );
            cairo_pattern_add_color_stop( pattern, 0.7, dark );
            dark.setAlpha(0);
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

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return _separatorCache.insert( key, surface );

    }

    //______________________________________________________________________________
    const Cairo::Surface& StyleHelper::windecoButton(const ColorUtils::Rgba &base, bool pressed, int size)
    {

        const WindecoButtonKey key( base, size, pressed );

        // try find in cache and return
        if( const Cairo::Surface& surface = _windecoButtonCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( size, size ) );

        // calculate colors
        ColorUtils::Rgba light = ColorUtils::lightColor(base);
        ColorUtils::Rgba dark = ColorUtils::darkColor(base);

        // create cairo context
        Cairo::Context context( surface );
        const double u = size/18.0;
        cairo_translate( context, 0.5*u, (0.5-0.668)*u );

        {

            // plain background
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, u*1.665, 0, u*(12.33+1.665) ) );
            if( pressed )
            {
                cairo_pattern_add_color_stop( pattern, 1, light );
                cairo_pattern_add_color_stop( pattern, 0, dark );
            } else {
                cairo_pattern_add_color_stop( pattern, 0, light );
                cairo_pattern_add_color_stop( pattern, 1, dark );
            }

            cairo_ellipse( context, u*0.5*(17-12.33), u*1.665, u*12.33, u*12.33 );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        {
            // outline circle
            const double penWidth( 0.7 );
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, u*1.665, 0, u*(2.0*12.33+1.665) ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, dark );

            cairo_ellipse( context, u*0.5*(17-12.33+penWidth), u*(1.665+penWidth), u*(12.33-penWidth), u*(12.33-penWidth) );
            cairo_set_source( context, pattern );
            cairo_set_line_width( context, penWidth );
            cairo_stroke( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return _windecoButtonCache.insert( key, surface );

    }

    //_______________________________________________________________________
    const Cairo::Surface& StyleHelper::windecoButtonGlow(const ColorUtils::Rgba &base, int size)
    {

        const WindecoButtonGlowKey key( base, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = _windecoButtonGlowCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( size, size ) );

        // right now the same color is used for the two shadows
        const ColorUtils::Rgba& light( base );
        const ColorUtils::Rgba& dark( base );

        Cairo::Context context( surface );
        const double u = size/18.0;
        cairo_translate( context, 0.5*u, (0.5-0.668)*u );

        {

            // outer shadow
            Cairo::Pattern pattern( cairo_pattern_create_radial( u*8.5, u*8.5, u*8.5 ) );

            static const int nPoints( 5 );
            double x[5] = { 0.61, 0.72, 0.81, 0.9, 1};
            double values[5] = { 255-172, 255-178, 255-210, 255-250, 0 };
            ColorUtils::Rgba c = dark;
            for( int i = 0; i<nPoints; i++ )
            { c.setAlpha( values[i]/255 ); cairo_pattern_add_color_stop( pattern, x[i], c ); }

            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, size, size );
            cairo_fill( context );
        }

        {
            // inner shadow
            Cairo::Pattern pattern( cairo_pattern_create_radial( u*8.5, u*8.5, u*8.5 ) );

            static const int nPoints(6);
            const double x[6] = { 0.61, 0.67, 0.7, 0.74, 0.78, 1 };
            const double values[6] = { 255-92, 255-100, 255-135, 255-205, 255-250, 0 };
            ColorUtils::Rgba c( light );
            for( int i = 0; i<nPoints; i++ )
            { c.setAlpha( values[i]/255 ); cairo_pattern_add_color_stop( pattern, x[i], c ); }

            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, size, size );
            cairo_fill( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return _windecoButtonGlowCache.insert( key, surface );

    }

    //_________________________________________________
    const Cairo::Surface& StyleHelper::verticalGradient( const ColorUtils::Rgba& base, int height )
    {

        const VerticalGradientKey key( base, height );

        // try find in cache and return
        if( const Cairo::Surface& surface = _verticalGradientCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( 32, height ) );

        {
            ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
            ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, height ) );
            cairo_pattern_add_color_stop( pattern, 0, top );
            cairo_pattern_add_color_stop( pattern, 0.5, base );
            cairo_pattern_add_color_stop( pattern, 1, bottom );

            Cairo::Context context( surface );
            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, 32, height );
            cairo_fill( context );
        }

        return _verticalGradientCache.insert( key, surface );
    }

    //_________________________________________________
    const Cairo::Surface& StyleHelper::radialGradient( const ColorUtils::Rgba& base, int radius )
    {

        const RadialGradientKey key( base, radius );

        // try find in cache and return
        if( const Cairo::Surface& surface = _radialGradientCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( 2*radius, radius ) );

        {
            // create radial pattern
            ColorUtils::Rgba radial( ColorUtils::backgroundRadialColor( base ) );
            Cairo::Pattern pattern( cairo_pattern_create_radial( radius, 0, radius ) );
            cairo_pattern_add_color_stop( pattern, 0, radial );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( radial, 101.0/255 ) );
            cairo_pattern_add_color_stop( pattern, 0.75, ColorUtils::alphaColor( radial, 37.0/255 ) );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( radial, 0 ) );

            Cairo::Context context( surface );
            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, 2*radius, radius );
            cairo_fill( context );
        }

        return _radialGradientCache.insert( key, surface );

    }

    //_________________________________________________
    const TileSet& StyleHelper::slab(const ColorUtils::Rgba& base, const ColorUtils::Rgba& glow, double shade, int size)
    {

        const SlabKey key( base, glow, shade, size );
        const TileSet& tileSet( _slabCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create surface and initialize
        const int w( 2*size );
        const int h( 2*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {
            // create cairo context
            Cairo::Context context( surface );
            cairo_scale( context, size/7.0, size/7.0 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::Rgba::transparent( base ) );
            cairo_fill( context );

            if( base.isValid() ) drawShadow( context, ColorUtils::shadowColor( base ), 14 );
            if( glow.isValid() ) drawOuterGlow( context, glow, 14 );
            if( base.isValid() ) drawSlab( context, base, shade );

        }

        // create tileSet
        return _slabCache.insert( key, TileSet( surface,  size, size, size, size, size-1, size, 2, 1) );

    }

    //__________________________________________________________________
    const TileSet& StyleHelper::slabSunken( const ColorUtils::Rgba& base, int size )
    {

        const SlabKey key( base, 0.0, size );
        const TileSet& tileSet( _slabSunkenCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create surface and initialize
        const int w( 2*size );
        const int h( 2*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {

            // create cairo context
            Cairo::Context context( surface );
            cairo_set_line_width( context, 1.0 );
            cairo_scale( context, size/7.0, size/7.0 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::Rgba::transparent( base ) );
            cairo_fill( context );

            if( base.isValid() )
            {
                drawInverseShadow( context, ColorUtils::shadowColor(base), 3, 8, 0.0);

                // contrast
                {
                    const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
                    Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 2, 0, 16 ) );
                    cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( light ) );
                    cairo_pattern_add_color_stop( pattern, 1.0, light );
                    cairo_set_source( context, pattern );
                    cairo_rounded_rectangle( context, 2.5, 2.5, 9, 9, 4.0 );
                    cairo_stroke( context );
                }

            }

        }

        // create tileSet
        return _slabSunkenCache.insert( key, TileSet( surface,  size, size, size, size, size-1, size, 2, 1) );

    }

    //______________________________________________________________________________
    const Cairo::Surface& StyleHelper::roundSlab(const ColorUtils::Rgba& base, const ColorUtils::Rgba& glow, double shade, int size)
    {

        SlabKey key( base, glow, shade, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = _roundSlabCache.value( key ) )
        { return surface; }

        // cached not found, create new
        const int w( 3*size );
        const int h( 3*size );
        Cairo::Surface surface( createSurface( w, h ) );

        // create cairo context
        Cairo::Context context( surface );
        cairo_scale( context, size/7.0, size/7.0 );

        // shadow
        if( base.isValid() ) drawShadow( context, ColorUtils::shadowColor(base), 21 );
        if( glow.isValid() ) drawOuterGlow( context, glow, 21 );
        if( base.isValid() ) drawRoundSlab( context, base, shade );

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return _roundSlabCache.insert( key, surface );

    }


    //__________________________________________________________________________________________________________
    const Cairo::Surface&  StyleHelper::sliderSlab(const ColorUtils::Rgba& base, const ColorUtils::Rgba& glow, bool sunken, double shade, int size)
    {

        SliderSlabKey key( base, glow, sunken, shade, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = _sliderSlabCache.value( key ) )
        { return surface; }

        // cached not found, create new
        const int w( 3*size );
        const int h( 3*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {
            Cairo::Context context( surface );

            cairo_save( context );
            cairo_scale( context, 3.0*size/23, 3.0*size/23 );
            cairo_translate( context, 1, 1 );

            if( base.isValid() ) drawShadow( context, ColorUtils::alphaColor( ColorUtils::shadowColor(base), 0.8 ), 21 );
            if( glow.isValid() ) drawOuterGlow( context, glow, 21 );
            cairo_restore( context );

            cairo_scale( context, 3.0*size/25, 3.0*size/25 );
            cairo_translate( context, 2, 2 );
            drawSliderSlab( context, base, sunken, shade );

        }

        return _sliderSlabCache.insert( key, surface );

    }

    //________________________________________________________________________________________________________
    const TileSet &StyleHelper::slope( const ColorUtils::Rgba& base, double shade, int size )
    {

        const SlabKey key( base, shade, size );
        const TileSet& tileSet( _slopeCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int w( 4*size );
        const int h( 4*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {
            Cairo::Context context( surface );
            const TileSet &slabTileSet = slab( base, shade, size );
            slabTileSet.render( context, 0, 0, size*4, size*5, TileSet::Left | TileSet::Right | TileSet::Top );

        }

        return _slopeCache.insert( key, TileSet( surface,  size, size, size, size, size-1, size, 2, 1) );

    }

    //__________________________________________________________________
    void StyleHelper::fillSlab( cairo_t* context, int x, int y, int w, int h, const TileSet::Tiles& tiles ) const
    {

        const double s( 3.6 + ( 0.5 * _slabThickness )  );
        const double r( s/2 );

        Corners corners( CornersNone );
        if( tiles & TileSet::Top )
        {
            if( tiles & TileSet::Left ) corners |= CornersTopLeft;
            if( tiles & TileSet::Right ) corners |= CornersTopRight;
        }

        if( tiles & TileSet::Bottom )
        {
            if( tiles & TileSet::Left ) corners |= CornersBottomLeft;
            if( tiles & TileSet::Right ) corners |= CornersBottomRight;
        }


        cairo_rounded_rectangle( context, x+s, y+s, w-2*s, h-2*s, r, corners );
        cairo_fill( context );
    }

    //__________________________________________________________________
    GdkPixmap* StyleHelper::roundMask( int w, int h, int radius ) const
    {

        GdkPixmap* mask( gdk_pixmap_new( 0L, w, h, 1 ) );

        {
            Cairo::Context context( GDK_DRAWABLE(mask) );

            // clear the window
            cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
            cairo_set_source( context, ColorUtils::Rgba::transparent() );
            cairo_paint( context );

            // now draw roundrect mask
            cairo_set_operator( context, CAIRO_OPERATOR_OVER );
            cairo_set_source( context, ColorUtils::Rgba::black() );

            // FIXME: radius found empirically
            cairo_rounded_rectangle( context, 0, 0, w, h, radius );
            cairo_fill( context );
        }

        return mask;

    }


    //______________________________________________________________________________
    const TileSet& StyleHelper::holeFocused(
        const ColorUtils::Rgba &base, const ColorUtils::Rgba &fill, const ColorUtils::Rgba &glow,
        int size, bool contrast )
    {

        const HoleFocusedKey key( base, fill, glow, size, contrast );
        const TileSet& tileSet( _holeFocusedCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // first create shadow
        const int shadowSize( (size*5)/7 );
        Cairo::Surface shadowSurface( createSurface( 2*shadowSize, 2*shadowSize ) );

        {
            Cairo::Context context( shadowSurface );
            cairo_scale( context, 5.0/shadowSize, 5.0/shadowSize );

            // get alpha channel
            double alpha( glow.isValid() ? glow.alpha() : 0 );
            if( alpha < 1 )
            {

                // shadow
                drawInverseShadow( context, ColorUtils::alphaColor( ColorUtils::shadowColor( base ), 1.0 - alpha ), 1, 8, 0.0);

            }

            if( alpha > 0 )
            {

                // glow
                drawInverseGlow( context, glow, 1, 8, shadowSize );

            }

        }

        // create surface
        Cairo::Surface surface( createSurface( 2*size, 2*size ) );
        {

            Cairo::Context context( surface );
            cairo_scale( context, 7.0/size, 7.0/size );
            cairo_set_line_width( context, 1 );

            // inside
            if( fill.isValid() )
            {
                cairo_rounded_rectangle( context, 1, 1, 12, 11, 2.5 );
                cairo_set_source( context, fill );
                cairo_fill( context );
            }

            // draw shadow
            TileSet(
                shadowSurface, shadowSize, shadowSize, shadowSize, shadowSize,
                shadowSize-1, shadowSize, 2, 1 ).
                render( context, 0, 0, size*2, size*2 );

            // contrast pixel
            if( contrast )
            {
                const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, 18 ) );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( light ) );
                cairo_pattern_add_color_stop( pattern, 1.0, light );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 0.5, 0.5, 13, 13, 4.0 );
                cairo_stroke( context );

            }

        }

        return _holeFocusedCache.insert( key, TileSet( surface, size, size, size, size, size-1, size, 2, 1 ) );

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::holeFlat( const ColorUtils::Rgba& base, double shade, bool fill, int size )
    {

        const HoleFlatKey key( base, shade, fill, size );
        const TileSet& tileSet( _holeFlatCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int w( 2*size );
        const int h( 2*size );

        Cairo::Surface surface( createSurface( w, h ) );

        if( fill )
        {

            Cairo::Context context( surface );
            cairo_set_line_width( context, 1.0 );

            cairo_scale( context, 14.0/w, 14.0/h );

            // hole inside
            cairo_set_source( context, base );
            cairo_rounded_rectangle( context, 1, 0, 12, 13, 3.0 );
            cairo_fill( context );

            {
                // shadow (top)
                const ColorUtils::Rgba dark( ColorUtils::shade( ColorUtils::darkColor( base ), shade ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, -2, 0, 14 ) );
                cairo_pattern_add_color_stop( pattern, 0, dark );
                cairo_pattern_add_color_stop( pattern, 0.4, ColorUtils::Rgba::transparent( dark ) );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 1.5, 0.5, 11, 12, 2.5 );
                cairo_stroke( context );
            }

            {
                // contrast (bottom)
                const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor( base ), shade ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, 18 ) );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( light ) );
                cairo_pattern_add_color_stop( pattern, 1.0, light );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 0.5, 0.5, 13, 13, 3.5 );
                cairo_stroke( context );
            }

        } else {

            Cairo::Context context( surface );
            cairo_set_line_width( context, 1.0 );

            cairo_scale( context, 14.0/w, 14.0/h );

            // hole inside
            cairo_set_source( context, base );
            cairo_rounded_rectangle( context, 1, 1, 12, 12, 3.0 );
            cairo_fill( context );

            {
                // shadow (top)
                const ColorUtils::Rgba dark( ColorUtils::shade( ColorUtils::darkColor( base ), shade ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 1, 0, 12 ) );
                cairo_pattern_add_color_stop( pattern, 0, dark );
                cairo_pattern_add_color_stop( pattern, 0.4, ColorUtils::Rgba::transparent( dark ) );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 1.5, 1.5, 11, 11, 2.5 );
                cairo_stroke( context );
            }

            {
                // contrast (bottom)
                const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor( base ), shade ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 1, 0, 12 ) );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( light ) );
                cairo_pattern_add_color_stop( pattern, 1.0, light );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 1.5, 1.5, 11, 11, 2.5 );
                cairo_stroke( context );
            }


        }

        return _holeFlatCache.insert( key, TileSet( surface, size, size, size, size, size-1, size, 2, 1 ) );

    }

    //______________________________________________________________________________
    const TileSet& StyleHelper::scrollHole( const ColorUtils::Rgba& base, bool vertical, bool smallShadow )
    {

        const ScrollHoleKey key( base, vertical, smallShadow );
        const TileSet& tileSet( _scrollHoleCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // define colors
        const ColorUtils::Rgba& dark( ColorUtils::darkColor( base ) );
        const ColorUtils::Rgba& light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba& shadow( ColorUtils::shadowColor( base ) );

        // create pixmap
        const int w( 15 );
        const int h( 15 );
        Cairo::Surface surface( createSurface( w, h ) );

        {
            Cairo::Context context( surface );

            GdkRectangle r = { 0, 0, w, h };
            GdkRectangle rect = { 1, 0, w-2, h-1 };

            // radius
            const double radius( smallShadow ? 2.5:3.0 );

            // base
            {
                cairo_set_source( context, dark );
                gdk_cairo_rounded_rectangle( context, &rect, radius );
                cairo_fill( context );
            }

            // light shadow across the whole hole
            {
                Cairo::Pattern pattern;
                if( vertical ) pattern.set( cairo_pattern_create_linear( rect.x, 0, rect.x + rect.width, 0 ) );
                else pattern.set( cairo_pattern_create_linear( 0, rect.y, 0, rect.y + rect.height ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.1 ) );
                cairo_pattern_add_color_stop( pattern, 0.6, ColorUtils::Rgba::transparent( shadow ) );

                cairo_set_source( context, pattern );
                gdk_cairo_rounded_rectangle( context, &rect, radius );
                cairo_fill( context );
            }

            // first create shadow
            const int shadowSize( 5 );
            Cairo::Surface shadowSurface( createSurface( 2*shadowSize, 2*shadowSize ) );
            {
                Cairo::Context context( shadowSurface );
                drawInverseShadow( context, ColorUtils::shadowColor( base ), 1, 8, 0.0);
            }

            // draw shadow
            TileSet(
                shadowSurface, shadowSize, shadowSize, shadowSize, shadowSize,
                shadowSize-1, shadowSize, 2, 1 ).
                render( context, 0, -1, w, h+1, TileSet::Full );

            // light bottom border
            {
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, r.y, 0, r.y + r.height ) );

                if( smallShadow && vertical )
                {

                    cairo_pattern_add_color_stop( pattern, 0.8, ColorUtils::Rgba::transparent( light ) );
                    cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( light, 0.5 ) );

                } else {

                    cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( light ) );
                    cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( light, 0.6 ) );

                }

                cairo_set_source( context, pattern );
                cairo_set_line_width( context, 1.0 );
                cairo_rounded_rectangle( context, 0.5+r.x, 0.5+r.y, r.width-1, r.height-1, radius+0.5 );
                cairo_stroke( context );
            }

        }

        return _scrollHoleCache.insert( key, TileSet( surface, 7, 7, 1, 1 ) );

    }

    //______________________________________________________________________________
    const TileSet& StyleHelper::scrollHandle( const ColorUtils::Rgba& base, const ColorUtils::Rgba& glow, int size )
    {

        const ScrollHandleKey key( base, glow, size );
        const TileSet& tileSet( _scrollHandleCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create pixmap
        const int w( 2*size );
        const int h( 2*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {
            Cairo::Context context( surface );
            cairo_scale( context, (2.0*size)/14, (2.0*size)/14 );

            // first create shadow
            Cairo::Surface shadowSurface( createSurface( 10, 10 ) );
            {
                Cairo::Context context( shadowSurface );

                // shadow/glow
                drawOuterGlow( context, glow, 10 );

            }

            // draw shadow
            TileSet( shadowSurface, 4, 4, 1, 1 ).
                render( context, 0, 0, 14, 14, TileSet::Full );

            // outline
            {
                const ColorUtils::Rgba mid( ColorUtils::midColor( base ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 3, 0, 11 ) );
                cairo_pattern_add_color_stop( pattern, 0, base );
                cairo_pattern_add_color_stop( pattern, 1, mid );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 3, 3, 8, 8, 2.5 );
                cairo_fill( context );
            }

            // contrast
            {
                const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 3, 0, 11 ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( light, 0.9 ) );
                cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( light, 0.44 ) );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 3, 3, 8, 8, 2.5 );
                cairo_fill( context );
            }

        }

        return _scrollHandleCache.insert( key, TileSet( surface, 7, 7, 1, 1 ) );

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::slitFocused( const ColorUtils::Rgba& glow )
    {

        const SlitFocusedKey key( glow );
        const TileSet& tileSet( _slitFocusedCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create pixmap
        const int w( 9 );
        const int h( 9 );
        Cairo::Surface surface( createSurface( w, h ) );
        {
            Cairo::Context context( surface );

            cairo_pattern_t* pattern( cairo_pattern_create_radial( 4.5, 4.5, 3.5 ) );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( glow, 180.0/255 ) );
            cairo_pattern_add_color_stop( pattern,  0.5, ColorUtils::Rgba::transparent( glow ) );

            cairo_set_source( context, pattern );
            cairo_ellipse( context, 1, 1, 7, 7 ) ;
            cairo_fill( context );

        }

        return _slitFocusedCache.insert( key, TileSet( surface, 4, 4, 1, 1 ) );

    }

    //____________________________________________________________________
    const TileSet& StyleHelper::dockFrame( const ColorUtils::Rgba &top, const ColorUtils::Rgba& bottom )
    {

        // key
        DockFrameKey key( top, bottom );
        const TileSet& tileSet( _dockFrameCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // fixed height
        const int size( 13 );

        Cairo::Surface surface( createSurface( size, size ) );
        {

            Cairo::Context context( surface );
            cairo_set_line_width( context, 1.0 );

            // local rectangle
            const double w( size );
            const double h( size );

            const ColorUtils::Rgba lightTop( ColorUtils::alphaColor( ColorUtils::lightColor( top ), 0.5 ) );
            const ColorUtils::Rgba lightBottom( ColorUtils::alphaColor( ColorUtils::lightColor( bottom ), 0.5 ) );
            const ColorUtils::Rgba darkTop( ColorUtils::alphaColor( ColorUtils::darkColor( top ), 0.5 ) );
            const ColorUtils::Rgba darkBottom( ColorUtils::alphaColor( ColorUtils::darkColor( bottom ), 0.5 ) );

            // dark frame
            {
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0.5, 0, h - 1.5 ) );
                cairo_pattern_add_color_stop( pattern, 0, darkTop );
                cairo_pattern_add_color_stop( pattern, 1, darkBottom );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 1.5, 0.5, w-3, h-2, 4 );
                cairo_stroke( context );
            }

            // bottom contrast
            {
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0.5, 0, h - 0.5 ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::Rgba::transparent( lightBottom ) );
                cairo_pattern_add_color_stop( pattern, 1, lightBottom );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, 0.5, 0.5, w-1, h-1, 4.5 );
                cairo_stroke( context );
            }

            // top contrast
            {
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 1.5, 0, h - 2.5 ) );
                cairo_pattern_add_color_stop( pattern, 0, lightTop );
                cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( lightTop ) );
                cairo_rounded_rectangle( context, 2.5, 1.5, w-5, h-4, 3.5 );
                cairo_set_source( context, pattern );
                cairo_stroke( context );
            }

        }

        return _dockFrameCache.insert( key, TileSet( surface, (size-1)/2, (size-1)/2, 1, 1 ) );
    }

    //____________________________________________________________________
    const Cairo::Surface& StyleHelper::progressBarIndicator(const ColorUtils::Rgba& base, const ColorUtils::Rgba& highlight, int w, int h )
    {

        ProgressBarIndicatorKey key( base, highlight, w, h );

        // try find in cache and return
        if( const Cairo::Surface& surface = _progressBarIndicatorCache.value( key ) )
        { return surface; }

        // cached not found, create new

        // local rect
        int xl = 0;
        int yl = 0;
        int wl = w;
        int hl = h;

        Cairo::Surface surface( createSurface( wl, hl ) );
        Cairo::Context context( surface );

        // colors
        const ColorUtils::Rgba lhighlight( ColorUtils::lightColor( highlight ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );
        const ColorUtils::Rgba shadow( ColorUtils::shadowColor( base ) );

        {
            // shadow
            cairo_rounded_rectangle( context, xl+0.5, yl+0.5, wl-1, hl, 3.0 );
            cairo_set_source( context, ColorUtils::alphaColor( shadow, 0.6 ) );
            cairo_set_line_width( context, 0.6 );
            cairo_stroke( context );
        }

        xl += 1;
        yl += 1;
        wl -= 2;
        hl -= 1;

        {
            // filling
            cairo_set_source( context, ColorUtils::mix( highlight, dark, 0.2 ) );
            cairo_rounded_rectangle( context, xl, yl, wl, hl, 2.5 );
            cairo_fill( context );
        }

        // create pattern pixbuf
        {

            Cairo::Pattern mask( cairo_pattern_create_linear( 0, 0, wl, 0 ) );
            cairo_pattern_add_color_stop( mask, 0, ColorUtils::Rgba::transparent() );
            cairo_pattern_add_color_stop( mask, 0.4, ColorUtils::Rgba::black() );
            cairo_pattern_add_color_stop( mask, 0.6, ColorUtils::Rgba::black() );
            cairo_pattern_add_color_stop( mask, 1.0, ColorUtils::Rgba::transparent() );

            const ColorUtils::Rgba mix( ColorUtils::mix( lhighlight, light, 0.3 ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, hl ) );
            cairo_pattern_add_color_stop( pattern, 0,  mix );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( mix ) );
            cairo_pattern_add_color_stop( pattern, 0.6, ColorUtils::Rgba::transparent( mix ) );
            cairo_pattern_add_color_stop( pattern, 1.0, mix );

            Cairo::Surface localSurface( createSurface( wl, hl ) );
            Cairo::Context localContext( localSurface );
            cairo_rectangle( localContext, 0, 0, wl, hl );
            cairo_set_source( localContext, pattern );
            cairo_mask( localContext, mask );
            localContext.free();

            cairo_save( context );
            cairo_translate( context, 1, 1 );
            cairo_rectangle( context, 0, 0, wl, hl );
            cairo_set_source_surface( context, localSurface, 0, 0 );
            cairo_fill( context );
            cairo_restore( context );

        }

        {
            // bevel
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, hl ) );
            cairo_pattern_add_color_stop( pattern, 0.0, lhighlight );
            cairo_pattern_add_color_stop( pattern, 0.5, highlight );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::darkColor(highlight) );
            cairo_set_line_width( context, 1.0 );
            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, xl+0.5, yl+0.5, wl-1, hl-1, 2.5 );
            cairo_stroke( context );
        }

        {
            // bright top edge
            Cairo::Pattern pattern( cairo_pattern_create_linear( xl, 0, xl+wl, 0 ) );
            const ColorUtils::Rgba mix( ColorUtils::mix( lhighlight, light, 0.8 ) );
            cairo_pattern_add_color_stop( pattern, 0.0, ColorUtils::Rgba::transparent( mix ) );
            cairo_pattern_add_color_stop( pattern, 0.5, mix );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::Rgba::transparent( mix ) );
            cairo_set_line_width( context, 1.0 );
            cairo_set_source( context, pattern );
            cairo_move_to( context, xl+0.5, yl+0.5 );
            cairo_line_to( context, xl+wl-0.5, yl+0.5 );
            cairo_stroke( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return _progressBarIndicatorCache.insert( key, surface );

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::groove( const ColorUtils::Rgba &base, int size )
    {

        const GrooveKey key( base, size );
        const TileSet& tileSet( _grooveCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int rsize( int( ceil( size * 3.0/7.0 ) ) );
        const int w( rsize*2 );
        const int h( rsize*2 );
        Cairo::Surface surface( createSurface( w, h ) );

        {

            Cairo::Context context( surface );
            cairo_scale( context, 6/w, 6/h );

            Cairo::Pattern pattern( inverseShadowGradient( ColorUtils::shadowColor( base ), 1, 4, 0.0 ) );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 1, 1, 4, 4 );
            cairo_ellipse_negative( context, 2, 2, 2, 2 );
            cairo_fill( context );

        }

        return _grooveCache.insert( key, TileSet( surface, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 ) );

    }

    //____________________________________________________________________
    const TileSet& StyleHelper::selection( const ColorUtils::Rgba& base, int h, bool custom )
    {

        const SelectionKey key( base, h, custom );
        const TileSet& tileSet( _selectionCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int w = 32+16;
        Cairo::Surface surface( createSurface( w, h ) );

        {

            // adjust height
            const int x = 0;
            const int y = 0;
            const double rounding( 2.5 );

            Cairo::Context context( surface );
            cairo_set_line_width( context, 1.0 );

            {
                // filling
                const int lightenAmount( custom ? 110 : 130 );
                const ColorUtils::Rgba light( base.light( lightenAmount ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, y, 0, y+h ) );
                cairo_pattern_add_color_stop( pattern, 0, light );
                cairo_pattern_add_color_stop( pattern, 1, base );

                cairo_rounded_rectangle( context, x, y, w, h, rounding );
                cairo_set_source( context, pattern );
                cairo_fill( context );
            }

            {
                // contrast
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, y, 0, y+h ) );
                cairo_pattern_add_color_stop( pattern, 0, base );
                cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent() );

                cairo_rounded_rectangle( context, x+0.5, y+0.5, w-1, h-1, rounding );
                cairo_set_source( context, pattern );
                cairo_stroke( context );
            }

        }

        return _selectionCache.insert( key, TileSet( surface, 8, 0, 32, h ) );

    }

    //____________________________________________________________________
    void StyleHelper::renderDot( cairo_t* context, const ColorUtils::Rgba& base, int x, int y ) const
    {
        // Reduce diameter to make dots look more like in Oxygen-Qt
        const double diameter( 1.8 - 0.35 );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ).dark(130) );

        cairo_ellipse( context, x + 1.0 - diameter/2, y + 1.0 - diameter/2.0, diameter, diameter );
        cairo_set_source( context, light );
        cairo_fill( context );

        cairo_ellipse( context, x + 0.5 - diameter/2, y + 0.5 - diameter/2.0, diameter, diameter );
        cairo_set_source( context, dark );
        cairo_fill( context );
    }

    //______________________________________________________________________________________
    void StyleHelper::drawSlab( Cairo::Context& context, const ColorUtils::Rgba& color, double shade) const
    {

        const ColorUtils::Rgba light( ColorUtils::shade( lightColor(color), shade ) );
        const ColorUtils::Rgba base( ColorUtils::alphaColor( light, 0.85 ) );
        const ColorUtils::Rgba dark( ColorUtils::shade( darkColor(color), shade ) );

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
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.9, base );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3.6, 3.6, 6.8, 6.8 );
            cairo_fill( context );

        }

        // inside mask
        cairo_save( context );
        cairo_set_operator( context, CAIRO_OPERATOR_DEST_OUT );
        cairo_set_source( context, ColorUtils::Rgba::black() );

        const double ic( 3.6 + 0.5*_slabThickness );
        const double is( 14.0 - 2.0*ic );
        cairo_ellipse( context, ic, ic, is, is );
        cairo_fill( context );
        cairo_restore( context );

    }

    //___________________________________________________________________________________________
    void StyleHelper::drawShadow( cairo_t* context, const ColorUtils::Rgba& base, int size ) const
    {

        const double m( 0.5*size-1 );
        const double offset( 0.8 );
        const double k0( (m-4.0) / m );

        const double x(m+1);
        const double y(m+offset+1);

        Cairo::Pattern pattern( cairo_pattern_create_radial(x, y, m ) );
        for (int i = 0; i < 8; i++)
        {
            // sinusoidal pattern
            const double k1( (k0 * (8 - i) + i) * 0.125 );
            const double a( (cos( M_PI * i * 0.125) + 1.0) * 0.30 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( base ) );
        cairo_set_source( context, pattern );
        cairo_ellipse( context, 0, 0, size, size );
        cairo_fill( context );

    }

    //_______________________________________________________________________
    void StyleHelper::drawOuterGlow( cairo_t* context, const ColorUtils::Rgba& base, int size ) const
    {

        const double m( size*0.5 );
        const double w( 3 );

        const double bias( _glowBias * 14.0/size );

        // k0 is located at w - bias from the outer edge
        const double gm( m + bias - 0.9 );
        const double k0( (m-w+bias) / gm );

        Cairo::Pattern pattern( cairo_pattern_create_radial( m, m, gm ) );
        for (int i = 0; i < 8; i++)
        {
            // parabolic pattern
            const double k1( k0 + i*(1.0-k0)/8.0 );
            const double a( 1.0 - sqrt(i/8.0) );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a ) );
        }

        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( base ) );

        cairo_set_source( context, pattern );
        cairo_ellipse( context, 0, 0, size, size );
        cairo_fill( context );

        cairo_save( context );
        cairo_set_operator( context, CAIRO_OPERATOR_DEST_OUT );
        cairo_set_source( context, ColorUtils::Rgba::black() );
        cairo_ellipse( context, w+0.5, w+0.5, size - 2*w-1, size - 2*w-1 );
        cairo_fill( context );
        cairo_restore( context );

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseShadow(
        Cairo::Context& context, const ColorUtils::Rgba& base,
        int pad, int size, double fuzz ) const
    {

        Cairo::Pattern pattern( inverseShadowGradient( base, pad, size, fuzz ) );

        cairo_set_source( context, pattern );
        cairo_ellipse( context, pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0 );
        cairo_fill( context );

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseGlow(
        Cairo::Context& context, const ColorUtils::Rgba& base,
        int pad, int size, int rsize ) const
    {

        const double m( size*0.5 );

        const double width( 3.5 );
        const double bias( _glowBias*7.0/rsize );
        const double k0( ( m-width )/( m-bias ) );
        Cairo::Pattern pattern( cairo_pattern_create_radial( pad+m, pad+m, m-bias ) );
        for ( int i = 0; i < 8; i++ )
        {
            // inverse parabolic gradient
            double k1 = ( k0 * i + 8 - i ) * 0.125;
            double a = 1.0 - sqrt( i * 0.125 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a ) );

        }

        cairo_pattern_add_color_stop( pattern, k0, ColorUtils::Rgba::transparent( base ) );
        cairo_set_source( context, pattern );
        cairo_ellipse( context, pad, pad, size, size );
        cairo_fill( context );
        return;
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

        return;

    }

    //__________________________________________________________________________________________________________
    void StyleHelper::drawSliderSlab( Cairo::Context& context, const ColorUtils::Rgba& color, bool sunken, double shade ) const
    {

        const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor(color), shade) );
        const ColorUtils::Rgba dark( ColorUtils::shade( ColorUtils::darkColor( color ), shade) );

        {
            //plain background
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 3, 0, 21 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, dark );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3, 3, 15, 15 );
            cairo_fill( context );

        }

        if( sunken )
        {

            //plain background
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 3, 0, 21 ) );
            cairo_pattern_add_color_stop( pattern, 0, dark );
            cairo_pattern_add_color_stop( pattern, 1, light );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 5, 5, 11, 11 );
            cairo_fill( context );


        }

        {
            // outline circle
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 3, 0, 30 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, dark );
            cairo_set_source( context, pattern );

            cairo_ellipse( context, 3.5, 3.5, 14, 14 );
            cairo_set_line_width( context, 1.0 );
            cairo_stroke( context );
        }

        return;

    }

    //________________________________________________________________________________________________________
    cairo_pattern_t* StyleHelper::inverseShadowGradient(
        const ColorUtils::Rgba& base,
        int pad, int size, double fuzz ) const
    {

        const double m( size*0.5 );
        const double offset( 0.8 );
        const double k0( (m-2) / (m+2.0) );

        const double x(pad+m);
        const double y(pad+m+offset);

        cairo_pattern_t* pattern( cairo_pattern_create_radial(x, y, m+2 ) );
        for (int i = 0; i < 8; i++)
        {
            // sinusoidal pattern
            const double k1( (8 - i + k0 * i) * 0.125 );
            const double a( (cos(3.14159 * i * 0.125) + 1.0) * 0.25 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, k0, ColorUtils::Rgba::transparent( base ) );
        return pattern;

    }


    //________________________________________________________________________________________________________
    const Cairo::Surface& StyleHelper::dockWidgetButton(const ColorUtils::Rgba& base, bool pressed, int size)
    {
        const DockWidgetButtonKey key( base, pressed, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = _dockWidgetButtonCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( size, size ) );

        Cairo::Context context( surface );
        cairo_set_source( context, ColorUtils::Rgba::transparent( base ) );
        cairo_paint( context );

        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

        const double u( size/18.0 );
        cairo_translate( context, 0.5*u, ( 0.5-0.668 )*u );

        {
            // outline circle
            double penWidth = 1.2;
            Cairo::Pattern lg( cairo_pattern_create_linear( 0, u*( 1.665-penWidth ), 0, u*( 12.33+1.665-penWidth ) ) );

            cairo_pattern_add_color_stop( lg, 0, dark );
            cairo_pattern_add_color_stop( lg, 1, light );
            cairo_set_source( context, lg );
            cairo_set_line_width( context, penWidth*u );
            cairo_ellipse( context, u*0.5*( 17-12.33+penWidth ), u*( 1.665+penWidth ), u*( 12.33-penWidth ), u*( 12.33-penWidth ) );
            cairo_stroke( context );
        }

        return _dockWidgetButtonCache.insert(key, surface);
    }


}
