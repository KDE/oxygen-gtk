/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygencairoutils.h"
#include "oxygencairocontext.h"
#include "oxygenrgba.h"

#ifdef GDK_WINDOWING_X11
#include <cairo/cairo-xlib.h>
#endif

#include <cmath>
namespace Oxygen
{

    //__________________________________________________________________
    void cairo_arc_qt(cairo_t* context, double x, double y, double diam, double a, double alen)
    {

        const double xc( x+diam/2 );
        const double yc( y+diam/2 );
        const double radius( diam/2 );

        double angle1( -a );
        double angle2( -(alen+a) );

        if( angle1 > angle2 ) std::swap( angle1, angle2 );
        cairo_arc(context, xc,yc,radius,angle1,angle2);

    }

    //__________________________________________________________________
    void cairo_pattern_add_color_stop( cairo_pattern_t* pattern, double x, const ColorUtils::Rgba& color )
    {
        /*
        for some reason passing 1.0 for the upperbound on some gradients
        breaks the gradient definition internally with some compilers
        this is fixed by passing a value close to (but smaller than) unity.
        */
        cairo_pattern_add_color_stop_rgba( pattern, x, color.red(), color.green(), color.blue(), color.alpha() );
    }

    //__________________________________________________________________
    ColorStop::List cairo_pattern_get_color_stops( cairo_pattern_t* pattern )
    {

        ColorStop::List out;
        int count(0);
        if( cairo_pattern_get_color_stop_count( pattern, &count ) != CAIRO_STATUS_SUCCESS ) return out;

        for( int i = 0; i < count; ++i )
        {
            double x(0);
            double r(0), g(0), b(0), a(0);
            assert( cairo_pattern_get_color_stop_rgba( pattern, i, &x, &r, &g, &b, &a ) == CAIRO_STATUS_SUCCESS );
            out.push_back( ColorStop( x, ColorUtils::Rgba( r, g, b, a ) ) );
        }

        return out;

    }

    //__________________________________________________________________
    void cairo_set_source( cairo_t* context, const ColorUtils::Rgba& color )
    { cairo_set_source_rgba( context, color.red(), color.green(), color.blue(), color.alpha() ); }

    //__________________________________________________________________
    void cairo_rounded_rectangle( cairo_t* context, double x, double y, double w, double h, double r, Corners corners )
    {
        if(corners==CornersAll)
        {
            if(w<2*r)
            {
                double r0=r;
                r=0.5*w;
                y+=r0-r;
                h-=2*(r0-r);
            }
            if(h<2*r)
            {
                double r0=r;
                r=0.5*h;
                x+=r0-r;
                w-=2*(r0-r);
            }
        }

        if( corners == CornersNone )
        {
            cairo_rectangle( context, x, y, w, h );
            return;
        }

        if( corners & CornersTopLeft )
        {
            cairo_move_to( context, x, y+r );
            cairo_arc( context, x+r, y+r, r, M_PI, 3.0*M_PI/2 );
        } else cairo_move_to( context, x, y );

        if( corners & CornersTopRight )
        {
            cairo_line_to( context, x+w-r, y );
            cairo_arc( context, x+w-r, y+r, r, -M_PI/2, 0 );
        } else cairo_line_to( context, x+w, y );

        if( corners & CornersBottomRight )
        {
            cairo_line_to( context, x+w, y+h-r );
            cairo_arc( context, x+w-r, y+h-r, r, 0, M_PI/2 );
        } else cairo_line_to( context, x+w, y+h );

        if( corners & CornersBottomLeft )
        {
            cairo_line_to( context, x+r, y+h );
            cairo_arc( context, x+r, y+h-r, r, M_PI/2, M_PI );
        } else cairo_line_to( context, x, y+h );

        cairo_close_path( context );

    }

    //__________________________________________________________________
    void cairo_rounded_rectangle_negative( cairo_t* context, double x, double y, double w, double h, double r, Corners corners )
    {

        if( corners == CornersNone )
        {
            cairo_rectangle_negative( context, x, y, w, h );
            return;
        }

        if( corners & CornersTopRight )
        {
            cairo_move_to( context, x+w, y+r );
            cairo_arc_negative( context, x+w-r, y+r, r, 0, -M_PI/2 );
        } else cairo_move_to( context, x+w, y );

        if( corners & CornersTopLeft )
        {
            cairo_line_to( context, x+r, y );
            cairo_arc_negative( context, x+r, y+r, r, -M_PI/2, -M_PI );
        } else cairo_line_to( context, x, y );

        if( corners & CornersBottomLeft )
        {
            cairo_line_to( context, x, y+h-r );
            cairo_arc_negative( context, x+r, y+h-r, r, -M_PI, -3.0*M_PI/2 );
        } else cairo_line_to( context, x, y+h );

        if( corners & CornersBottomRight )
        {
            cairo_line_to( context, x+w-r, y+h );
            cairo_arc_negative( context, x+w-r, y+h-r, r, M_PI/2, 0 );
        } else cairo_line_to( context, x+w, y+h );

        cairo_close_path( context );

    }

    //__________________________________________________________________
    void cairo_rectangle_negative( cairo_t* context, double x, double y, double w, double h )
    {

        cairo_move_to( context, x+w, y );
        cairo_line_to( context, x, y );
        cairo_line_to( context, x, y+h );
        cairo_line_to( context, x+w, y+h );
        cairo_close_path( context );

    }

    //__________________________________________________________________
    void cairo_ellipse( cairo_t* context, double x, double y, double w, double h )
    {
        cairo_save( context );
        cairo_translate( context, x+w/2, y+h/2 );
        cairo_scale( context, w/2, h/2 );
        cairo_arc( context, 0, 0, 1, 0, 2.0*M_PI);
        cairo_restore( context );
    }

    //___________________________________________________________
    void cairo_ellipse_negative( cairo_t* context, double x, double y, double w, double h )
    {
        cairo_save( context );
        cairo_translate( context, x+w/2, y+h/2 );
        cairo_scale( context, w/2, h/2 );
        cairo_arc_negative( context, 0, 0, 1, 2.0*M_PI, 0);
        cairo_restore( context );
    }

    //__________________________________________________________________
    void cairo_polygon( cairo_t* context, const Polygon& polygon )
    {
        for( Polygon::const_iterator iter = polygon.begin(); iter != polygon.end(); ++iter )
        {
            if( iter == polygon.begin() ) cairo_move_to( context, iter->x(), iter->y() );
            else cairo_line_to( context, iter->x(), iter->y() );
        }
    }

    //_____________________________________________________
    int cairo_surface_get_width( cairo_surface_t* surface )
    {
        const cairo_surface_type_t type( cairo_surface_get_type( surface ) );
        switch( type )
        {

            case CAIRO_SURFACE_TYPE_IMAGE:
            return cairo_image_surface_get_width( surface );

            #if CAIRO_HAS_XLIB_SURFACE
            case CAIRO_SURFACE_TYPE_XLIB:
            return cairo_xlib_surface_get_width( surface );
            #endif

            default:
            {
                // This is less efficient, still makes rendering correct
                Cairo::Context context(surface);
                double x1,x2,dummy;
                cairo_clip_extents(context,&x1,&dummy,&x2,&dummy);
                return int(x2-x1);
            }
        }

    }

    //_____________________________________________________
    int cairo_surface_get_height( cairo_surface_t* surface )
    {
        const cairo_surface_type_t type( cairo_surface_get_type( surface ) );
        switch( type )
        {
            case CAIRO_SURFACE_TYPE_IMAGE:
            return cairo_image_surface_get_height( surface );

            #if CAIRO_HAS_XLIB_SURFACE
            case CAIRO_SURFACE_TYPE_XLIB:
            return cairo_xlib_surface_get_height( surface );
            #endif

            default:
            {
                // This is less efficient, still makes rendering correct
                Cairo::Context context(surface);
                double y1,y2,dummy;
                cairo_clip_extents(context,&dummy,&y1,&dummy,&y2);
                return int(y2-y1);
            }
        }

    }


    //_____________________________________________________
    void cairo_surface_get_size( cairo_surface_t* surface, int& width, int& height )
    {

        const cairo_surface_type_t type( cairo_surface_get_type( surface ) );
        switch( type )
        {
            case CAIRO_SURFACE_TYPE_IMAGE:
            width = cairo_image_surface_get_width( surface );
            height = cairo_image_surface_get_height( surface );
            return;

            #if CAIRO_HAS_XLIB_SURFACE
            case CAIRO_SURFACE_TYPE_XLIB:
            width = cairo_xlib_surface_get_width( surface );
            height = cairo_xlib_surface_get_height( surface );
            return;
            #endif

            default:
            {
                // This is less efficient, still makes rendering correct
                Cairo::Context context(surface);
                double x1, x2, y1, y2;
                cairo_clip_extents( context, &x1, &y1, &x2, &y2 );
                width = int(  x2 - x1 );
                height = int( y2 - y1 );
                return;
            }
        }

    }

    //__________________________________________________________________
    cairo_surface_t* cairo_surface_copy( cairo_surface_t* source )
    {

        const int width( cairo_surface_get_width( source ) );
        const int height( cairo_surface_get_height( source ) );
        cairo_surface_t* dest = cairo_surface_create_similar( source, CAIRO_CONTENT_COLOR_ALPHA, width, height );

        // create context on surface
        cairo_t* context( cairo_create( dest ) );
        cairo_set_source_surface( context, source, 0, 0 );
        cairo_rectangle( context, 0, 0, width, height );
        cairo_fill( context );

        cairo_destroy( context );
        return dest;

    }

    //__________________________________________________________________
    void cairo_surface_add_alpha( cairo_surface_t* surface, double alpha )
    {

        // create context on surface
        cairo_t* context( cairo_create( surface ) );
        cairo_set_operator( context, CAIRO_OPERATOR_DEST_IN );
        cairo_set_source_rgba( context, 1, 1, 1, alpha );
        cairo_rectangle( context, 0, 0, cairo_surface_get_width( surface ), cairo_surface_get_height( surface ) );
        cairo_fill( context );
        cairo_destroy( context );
        return;

    }

    //__________________________________________________________________
    void cairo_image_surface_saturate( cairo_surface_t* surface, double saturation )
    {

        // make sure right type is used
        assert( cairo_surface_get_type( surface ) == CAIRO_SURFACE_TYPE_IMAGE );
        assert( cairo_image_surface_get_format( surface ) == CAIRO_FORMAT_ARGB32 );


        // dimensions and stride
        const int width( cairo_image_surface_get_width( surface ) );
        const int height( cairo_image_surface_get_width( surface ) );
        const int stride( cairo_image_surface_get_stride( surface ) );
        const int bytesPerPixel(4);

        // data
        unsigned char* data( cairo_image_surface_get_data( surface ) );
        assert( data );

        int t;
        #define INTENSITY(r, g, b) ((unsigned char)((r) * 0.30 + (g) * 0.59 + (b) * 0.11))
        #define CLAMP_UCHAR(v) (t = (v), CLAMP (t, 0, 255))
        #define SATURATE(v) int(((1.0 - saturation) * intensity + saturation * (v)))

        unsigned char* line(data);
        unsigned char* pixel(data);

        for( int i = 0 ; i < height ; i++ )
        {

            pixel = line;
            line += stride;

            for( int j = 0 ; j < width ; j++ )
            {
                unsigned char intensity = INTENSITY( pixel[0], pixel[1], pixel[2] );
                pixel[0] = CLAMP_UCHAR( SATURATE( pixel[0] ) );
                pixel[1] = CLAMP_UCHAR( SATURATE( pixel[1] ) );
                pixel[2] = CLAMP_UCHAR( SATURATE( pixel[2] ) );
                pixel += bytesPerPixel;

            }

        }

    }

}
