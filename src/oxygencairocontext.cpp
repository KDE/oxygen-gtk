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

}
