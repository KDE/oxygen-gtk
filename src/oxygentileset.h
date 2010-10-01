#ifndef oxygentileset_h
#define oxygentileset_h
/*
* this file is part of the oxygen gtk engine
* Copyright 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License version 2 as published by the Free Software Foundation.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include <cairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <vector>

namespace Cairo
{
    class Context;
}

namespace Oxygen
{

    class TileSet
    {

        public:

        //! empty constructor
        TileSet();

        /*!
        Create a TileSet from a pixmap. The size of the bottom/right chunks is
        whatever is left over from the other chunks, whose size is specified
        in the required parameters.

        @param w1 width of the left chunks
        @param h1 height of the top chunks
        @param w2 width of the not-left-or-right chunks
        @param h2 height of the not-top-or-bottom chunks
        */
        TileSet( GdkPixbuf*, int w1, int h1, int w2, int h2 );

        /*!
        Create a TileSet from a pixmap. The size of the top/left and bottom/right
        chunks is specified, with the middle chunks created from the specified
        portion of the pixmap. This allows the middle chunks to overlap the outer
        chunks (or to not use all pixels). The top/left and bottom/right chunks
        are carved out of the corners of the pixmap.

        @param w1 width of the left chunks
        @param h1 height of the top chunks
        @param w3 width of the right chunks
        @param h3 height of bottom chunks
        @param x2 x-coordinate of the top of the not-left-or-right chunks
        @param y2 y-coordinate of the left of the not-top-or-bottom chunks
        @param w2 width of the not-left-or-right chunks
        @param h2 height of the not-top-or-bottom chunks
        */
        TileSet( GdkPixbuf*, int w1, int h1, int w3, int h3, int x2, int y2, int w2, int h2 );

        //! copy constructor
        TileSet( const TileSet& );

        //! destructor
        virtual ~TileSet();

        //! assignment operator
        TileSet& operator = ( const TileSet& );

        /*!
        Flags specifying what sides to draw in ::render. Corners are drawn when
        the sides forming that corner are drawn, e.g. Top|Left draws the
        top-center, center-left, and top-left chunks. The center-center chunk is
        only drawn when Center is requested.
        */
        enum Tile {
            None = 0x0,
            Top = 0x1,
            Left = 0x2,
            Bottom = 0x4,
            Right = 0x8,
            Center = 0x10,
            TopLeft = Top|Left,
            TopRight = Top|Right,
            BottomLeft = Bottom|Left,
            BottomRight = Bottom|Right,
            Ring = Top|Left|Bottom|Right,
            Horizontal = Left|Right|Center,
            Vertical = Top|Bottom|Center,
            Full = Ring|Center
        };

        typedef unsigned long Tiles;

        //! return size associated to this tileset
        int width( void ) const
        { return _w1 + _w3; }

        //! return size associated to this tileset
        int height( void ) const
        { return _h1 + _h3; }

        //! is valid
        bool isValid( void ) const
        { return width() > 0  && height() > 0 && _pixmaps.size() == 9; }

        /*!
        Fills the specified rect with tiled chunks. Corners are never tiled,
        edges are tiled in one direction, and the center chunk is tiled in both
        directions. Partial tiles are used as needed so that the entire rect is
        perfectly filled. Filling is performed as if all chunks are being drawn.
        */
        void render( Cairo::Context&, int x, int y, int w, int h, unsigned int = Ring) const;

        private:

        //! shortcut to pixmap list
        typedef std::vector< GdkPixbuf* > PixbufList;

        //! initialize pixmap
        void initPixmap( PixbufList&, GdkPixbuf*, int x, int y, int w, int h );

        //! copy pixmap
        void copyPixmap( Cairo::Context&, int x, int y, GdkPixbuf*, int sx, int sy, int sw, int sh, cairo_extend_t ) const;

        private:

        //! pixmap arry
        PixbufList _pixmaps;

        // dimensions
        int _w1;
        int _h1;
        int _w3;
        int _h3;

    };

}

#endif
