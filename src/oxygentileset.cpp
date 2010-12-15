/*
* this file is part of the oxygen gtk engine
* Copyright 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

#include "oxygentileset.h"
#include "oxygencairocontext.h"
#include "oxygencolorutils.h"

#include <algorithm>
#include <iostream>
#include <gdk/gdk.h>

namespace Oxygen
{

    //______________________________________________________________
    TileSet::TileSet( void ):
        _w1(0),
        _h1(0),
        _w3(0),
        _h3(0)
    {}

    //______________________________________________________________
    TileSet::TileSet( GdkPixbuf *pix, int w1, int h1, int w2, int h2):
        _w1(w1), _h1(h1), _w3(0), _h3(0)
    {

        if( !pix ) return;

        _w3 = gdk_pixbuf_get_width( pix ) - (w1 + w2);
        _h3 = gdk_pixbuf_get_height( pix ) - (h1 + h2);
        int w = w2; while (w < 32 && w2 > 0) w += w2;
        int h = h2; while (h < 32 && h2 > 0) h += h2;

        // initialise pixmap array
        // top
        initPixmap( _pixmaps, pix, _w1, _h1, 0, 0, _w1, _h1 );
        initPixmap( _pixmaps, pix, w, _h1, _w1, 0, w2, _h1 );
        initPixmap( _pixmaps, pix, _w3, _h1, _w1+w2, 0, _w3, _h1 );

        // center
        initPixmap( _pixmaps, pix, _w1, h, 0, _h1, _w1, h2 );
        initPixmap( _pixmaps, pix, w, h, w1, _h1, w2, h2 );
        initPixmap( _pixmaps, pix, _w3, h, _w1+w2, _h1, _w3, h2 );

        // bottom
        initPixmap( _pixmaps, pix, _w1, _h3, 0, _h1+h2, _w1, _h3 );
        initPixmap( _pixmaps, pix, w, _h3, _w1, _h1+h2, w2, _h3 );
        initPixmap( _pixmaps, pix, _w3, _h3, _w1+w2, _h1+h2, _w3, _h3 );
    }

    //______________________________________________________________
    TileSet::TileSet( GdkPixbuf* pix, int w1, int h1, int w3, int h3, int x1, int y1, int w2, int h2):
        _w1(w1), _h1(h1), _w3(w3), _h3(h3)
    {

        if( !pix ) return;

        int x2 = gdk_pixbuf_get_width( pix ) - _w3;
        int y2 = gdk_pixbuf_get_height( pix ) - _h3;
        int w = w2; while (w < 32 && w2 > 0) w += w2;
        int h = h2; while (h < 32 && h2 > 0) h += h2;

        // initialise pixmap array
        // top
        initPixmap( _pixmaps, pix, _w1, _h1, 0, 0, _w1, _h1 );
        initPixmap( _pixmaps, pix, w, _h1, x1, 0, w2, _h1 );
        initPixmap( _pixmaps, pix, _w3, h1, x2, 0, _w3, _h1 );

        // center
        initPixmap( _pixmaps, pix, _w1, h, 0, y1, _w1, h2 );
        initPixmap( _pixmaps, pix, w, h, x1, y1, w2, h2 );
        initPixmap( _pixmaps, pix, _w3, h, x2, y1, _w3, h2 );

        // bottom
        initPixmap( _pixmaps, pix, _w1, _h3, 0, y2, _w1, _h3 );
        initPixmap( _pixmaps, pix, w, _h3, x1, y2, w2, _h3 );
        initPixmap( _pixmaps, pix, _w3, _h3, x2, y2, _w3, _h3 );

    }

    //______________________________________________________________
    TileSet::TileSet( const TileSet& other ):
        _pixmaps( other._pixmaps ),
        _w1( other._w1 ),
        _h1( other._h1 ),
        _w3( other._w3 ),
        _h3( other._h3 )
    {

        // increase ref counter
        for( PixbufList::iterator iter = _pixmaps.begin(); iter != _pixmaps.end(); ++iter )
        { g_object_ref( *iter ); }

    }

    //______________________________________________________________
    TileSet& TileSet::operator = ( const TileSet& other )
    {

        _w1 = other._w1;
        _h1 = other._h1;
        _w3 = other._w3;
        _h3 = other._h3;

        // unref existing pixmaps
        for( PixbufList::iterator iter = _pixmaps.begin(); iter != _pixmaps.end(); ++iter )
        { g_object_unref( *iter ); }

        // copy pixmap list
        _pixmaps = other._pixmaps;

        // increase ref counter
        for( PixbufList::iterator iter = _pixmaps.begin(); iter != _pixmaps.end(); ++iter )
        { g_object_ref( *iter ); }

        return *this;

    }

    //______________________________________________________________
    TileSet::~TileSet( void )
    {
        // unref existing pixmaps
        for( PixbufList::iterator iter = _pixmaps.begin(); iter != _pixmaps.end(); ++iter )
        { if( *iter ) g_object_unref( *iter ); }
    }

    //___________________________________________________________
    inline bool bits( unsigned int flags, unsigned int testFlags)
    { return (flags & testFlags) == testFlags; }

    //______________________________________________________________
    void TileSet::render( Cairo::Context& context, int x0, int y0, int w, int h, unsigned int t ) const
    {

        // check initialization
        if( _pixmaps.size() < 9 ) return;

        // calculate pixmaps widths
        int wLeft(0);
        int wRight(0);
        if( _w1+_w3 > 0 )
        {
            double wRatio( double( _w1 )/double( _w1 + _w3 ) );
            wLeft = (t&Right) ? std::min( _w1, int(w*wRatio) ):_w1;
            wRight = (t&Left) ? std::min( _w3, int(w*(1.0-wRatio)) ):_w3;
        }

        // calculate pixmap heights
        int hTop(0);
        int hBottom(0);
        if( _h1+_h3 > 0 )
        {
            double hRatio( double( _h1 )/double( _h1 + _h3 ) );
            hTop = (t&Bottom) ? std::min( _h1, int(h*hRatio) ):_h1;
            hBottom = (t&Top) ? std::min( _h3, int(h*(1.0-hRatio)) ):_h3;
        }

        // calculate corner locations
        w -= wLeft + wRight;
        h -= hTop + hBottom;
        int x1 = x0 + wLeft;
        int x2 = x1 + w;
        int y1 = y0 + hTop;
        int y2 = y1 + h;

        // corners
        if( bits(t, Top|Left) )  copyPixmap( context, x0, y0, _pixmaps.at(0), 0, 0, wLeft, hTop, CAIRO_EXTEND_NONE );
        if( bits(t, Top|Right) ) copyPixmap( context, x2, y0, _pixmaps.at(2), _w3-wRight, 0, wRight, hTop, CAIRO_EXTEND_NONE);
        if( bits(t, Bottom|Left) )  copyPixmap( context, x0, y2, _pixmaps.at(6), 0, _h3-hBottom, wLeft, hBottom, CAIRO_EXTEND_NONE);
        if( bits(t, Bottom|Right) ) copyPixmap( context, x2, y2, _pixmaps.at(8), _w3-wRight, _h3-hBottom, wRight, hBottom, CAIRO_EXTEND_NONE );

        // top and bottom
        if( w > 0 )
        {
            if( t & Top ) copyPixmap( context, x1, y0, _pixmaps.at(1), 0, 0, w, hTop, CAIRO_EXTEND_REPEAT );
            if( t & Bottom ) copyPixmap( context, x1, y2, _pixmaps.at(7), 0, _h3-hBottom, w, hBottom, CAIRO_EXTEND_REPEAT );
        }

        // left and right
        if( h > 0 )
        {
            if( t & Left ) copyPixmap( context, x0, y1, _pixmaps.at(3), 0, 0, wLeft, h, CAIRO_EXTEND_REPEAT );
            if( t & Right ) copyPixmap( context, x2, y1, _pixmaps.at(5), _w3-wRight, 0, wRight, h, CAIRO_EXTEND_REPEAT );
        }

        // center
        if ( (t & Center) && h > 0 && w > 0 ) copyPixmap( context, x1, y1, _pixmaps.at(4), 0, 0, w, h, CAIRO_EXTEND_REPEAT );

   }


    //______________________________________________________________
    void TileSet::initPixmap( PixbufList& pixmaps, GdkPixbuf *pix, int w, int h, int sx, int sy, int sw, int sh )
    {
        if( sw <= 0 || sh<= 0 || w <=0 || h <= 0 ) pixmaps.push_back( 0L );
        else if( sw == w && sh == h ) pixmaps.push_back( gdk_pixbuf_new_subpixbuf( pix, sx, sy, sw, sh ) );
        else {

            GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
            gdk_pixbuf_fill( pixbuf, ColorUtils::Rgba::transparent().toInt() );
            Cairo::Context context( pixbuf );

            GdkPixbuf* tile( gdk_pixbuf_new_subpixbuf( pix, sx, sy, sw, sh ) );
            gdk_cairo_set_source_pixbuf( context, tile, 0, 0 );
            cairo_pattern_set_extend( cairo_get_source( context ), CAIRO_EXTEND_REPEAT );
            cairo_rectangle( context, 0, 0, w, h );
            cairo_fill( context );
            context.updateGdkPixbuf();
            pixmaps.push_back( pixbuf );
            g_object_unref( tile );

        }
    }

    //______________________________________________________________
    void TileSet::copyPixmap( Cairo::Context& context, int x, int y, GdkPixbuf* pixbuf, int sx, int sy, int sw, int sh, cairo_extend_t extend ) const
    {
        if( !pixbuf ) return;
        cairo_translate( context, x, y );
        cairo_rectangle( context, 0, 0, sw, sh );

        gdk_cairo_set_source_pixbuf( context, pixbuf, -sx, -sy );
        cairo_pattern_set_extend( cairo_get_source( context ), extend );
        cairo_fill( context );
        cairo_translate( context, -x, -y );
    }

}
