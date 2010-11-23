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

#include "oxygencairocontext.h"
#include <gdk/gdk.h>
namespace Oxygen
{

    //_________________________________________-
    Cairo::Context::Context( GdkWindow* window, GdkRectangle* clipRect):
        _cr( 0L )
    {

        if( !GDK_IS_DRAWABLE(window) ) return;
        _cr= static_cast<cairo_t*>( gdk_cairo_create(window) );
        setClipping( clipRect );

    }

    //_________________________________________-
    Cairo::Context::Context( cairo_surface_t* surface, GdkRectangle* clipRect):
        _cr( 0L )
    {

        _cr= static_cast<cairo_t*>( cairo_create(surface) );
        setClipping( clipRect );

    }

    //__________________________________________________________________
    Cairo::Context::Context( GdkPixbuf *pixbuf, GdkRectangle* clipRect ):
        _cr( 0L )
    {

        /* Inspect input pixbuf and create compatible cairo surface */
        const gint width( gdk_pixbuf_get_width( pixbuf ) );
        gint height( gdk_pixbuf_get_height( pixbuf ) );

        const gint p_stride( gdk_pixbuf_get_rowstride( pixbuf ) );
        const gint p_n_channels( gdk_pixbuf_get_n_channels( pixbuf ) );
        guchar *p_pixels( gdk_pixbuf_get_pixels( pixbuf ) );

        // create surface and get data
        cairo_surface_t* surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, width, height );
        gint s_stride = cairo_image_surface_get_stride( surface );
        guchar* s_pixels = cairo_image_surface_get_data( surface );

        // copy pixmap data to surface
        while( height-- )
        {
            gint i;
            guchar *p_iter = p_pixels;
            guchar *s_iter = s_pixels;

            for( i = 0; i < width; i++ )
            {
                #if G_BYTE_ORDER == G_LITTLE_ENDIAN
                /* Pixbuf:  RGB(A)
                * Surface: BGRA */
                if( p_n_channels == 3 )
                {
                    s_iter[0] = p_iter[2];
                    s_iter[1] = p_iter[1];
                    s_iter[2] = p_iter[0];
                    s_iter[3] = 0xff;
                }
                else /* p_n_channels == 4 */
                {
                    gdouble alpha_factor = p_iter[3] / (gdouble)0xff;

                    s_iter[0] = (guchar)( p_iter[2] * alpha_factor + .5 );
                    s_iter[1] = (guchar)( p_iter[1] * alpha_factor + .5 );
                    s_iter[2] = (guchar)( p_iter[0] * alpha_factor + .5 );
                    s_iter[3] =           p_iter[3];
                }
                #elif G_BYTE_ORDER == G_BIG_ENDIAN
                /* Pixbuf:  RGB(A)
                * Surface: ARGB */
                if( p_n_channels == 3 )
                {
                    s_iter[3] = p_iter[2];
                    s_iter[2] = p_iter[1];
                    s_iter[1] = p_iter[0];
                    s_iter[0] = 0xff;
                }
                else /* p_n_channels == 4 */
                {
                    gdouble alpha_factor = p_iter[3] / (gdouble)0xff;

                    s_iter[3] = (guchar)( p_iter[2] * alpha_factor + .5 );
                    s_iter[2] = (guchar)( p_iter[1] * alpha_factor + .5 );
                    s_iter[1] = (guchar)( p_iter[0] * alpha_factor + .5 );
                    s_iter[0] =           p_iter[3];
                }
                #else /* PDP endianness */
                /* Pixbuf:  RGB(A)
                * Surface: RABG */
                if( p_n_channels == 3 )
                {
                    s_iter[0] = p_iter[0];
                    s_iter[1] = 0xff;
                    s_iter[2] = p_iter[2];
                    s_iter[3] = p_iter[1];
                }
                else /* p_n_channels == 4 */
                {
                    gdouble alpha_factor = p_iter[3] / (gdouble)0xff;

                    s_iter[0] = (guchar)( p_iter[0] * alpha_factor + .5 );
                    s_iter[1] =           p_iter[3];
                    s_iter[1] = (guchar)( p_iter[2] * alpha_factor + .5 );
                    s_iter[2] = (guchar)( p_iter[1] * alpha_factor + .5 );
                }
                #endif
                s_iter += 4;
                p_iter += p_n_channels;
            }
            s_pixels += s_stride;
            p_pixels += p_stride;
        }

        /* Create context and set user data */
        _cr = cairo_create( surface );
        cairo_surface_destroy( surface );

        // increase pixbuf reference
        // ensure deref at destruction
        g_object_ref( G_OBJECT( pixbuf ) );
        cairo_set_user_data( _cr, &_pixbuf_key, pixbuf, g_object_unref );

        // clipping
        setClipping( clipRect );

    }

    //_________________________________________________
    void Cairo::Context::free( void )
    {
        if( _cr ) {

            cairo_destroy( _cr );
            _cr = 0L;

        }
    }

    //_________________________________________________
    void Cairo::Context::setClipping( GdkRectangle* clipRect ) const
    {
        if( !clipRect ) return;
        cairo_rectangle( _cr, clipRect->x, clipRect->y, clipRect->width, clipRect->height );
        cairo_clip( _cr );
    }

    //_________________________________________________
    void Cairo::Context::setClipping( GdkRegion* clipRegion ) const
    {
        if( !clipRegion ) return;

        int numRects(0);
        GdkRectangle *rects;
        gdk_region_get_rectangles( clipRegion, &rects, &numRects );

        cairo_new_path( _cr );
        while( numRects-- )
        {
            const GdkRectangle& rect( rects[numRects] );
            cairo_rectangle( _cr, rect.x, rect.y, rect.width, rect.height );
        }

        cairo_clip( _cr );
        g_free( rects );
    }

    //__________________________________________________________________
    void Cairo::Context::updateGdkPixbuf( void ) const
    {

        /* Obtain pixbuf to be returned */
        GdkPixbuf* pixbuf = static_cast<GdkPixbuf*>( cairo_get_user_data( _cr, &_pixbuf_key ) );

        /* Obtain surface from where pixel values will be copied */
        cairo_surface_t* surface = cairo_get_target( _cr );

        /* Inspect input pixbuf and create compatible cairo surface */
        const gint width( gdk_pixbuf_get_width( pixbuf ) );
        gint height( gdk_pixbuf_get_height( pixbuf ) );
        const gint p_stride( gdk_pixbuf_get_rowstride( pixbuf ) );
        const gint p_n_channels( gdk_pixbuf_get_n_channels( pixbuf ) );
        guchar *p_pixels( gdk_pixbuf_get_pixels( pixbuf ) );

        gint s_stride = cairo_image_surface_get_stride( surface );
        guchar* s_pixels = cairo_image_surface_get_data( surface );

        /* Copy pixel data from surface to pixbuf */
        while( height-- )
        {
            gint i;
            guchar *p_iter = p_pixels;
            guchar *s_iter = s_pixels;

            for( i = 0; i < width; i++ )
            {
                #if G_BYTE_ORDER == G_LITTLE_ENDIAN

                // Pixbuf:  RGB(A); Surface: BGRA
                const gdouble alpha( s_iter[3] ? (gdouble)0xff / s_iter[3] : (gdouble)0xff );
                p_iter[0] = (guchar)( s_iter[2] * alpha + .5 );
                p_iter[1] = (guchar)( s_iter[1] * alpha + .5 );
                p_iter[2] = (guchar)( s_iter[0] * alpha + .5 );
                if( p_n_channels == 4 ) p_iter[3] = s_iter[3];

                #elif G_BYTE_ORDER == G_BIG_ENDIAN

                // Pixbuf:  RGB(A), Surface: ARGB
                const gdouble alpha( s_iter[0] ? (gdouble)0xff / s_iter[0] : (gdouble)0xff );
                p_iter[0] = (guchar)( s_iter[1] * alpha + .5 );
                p_iter[1] = (guchar)( s_iter[2] * alpha + .5 );
                p_iter[2] = (guchar)( s_iter[3] * alpha + .5 );
                if( p_n_channels == 4 ) p_iter[3] = s_iter[0];

                #else /* PDP endianness */

                // Pixbuf:  RGB(A) Surface: RABG
                const gdouble alpha( s_iter[1] ? (gdouble)0xff / s_iter[1] : (gdouble)0xff );
                p_iter[0] = (guchar)( s_iter[0] * alpha + .5 );
                p_iter[1] = (guchar)( s_iter[3] * alpha + .5 );
                p_iter[2] = (guchar)( s_iter[2] * alpha + .5 );
                if( p_n_channels == 4 ) p_iter[3] = s_iter[1];

                #endif
                s_iter += 4;
                p_iter += p_n_channels;
            }
            s_pixels += s_stride;
            p_pixels += p_stride;
        }

        return;

    }

}
