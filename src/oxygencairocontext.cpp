/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
