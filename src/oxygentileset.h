#ifndef oxygentileset_h
#define oxygentileset_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenflags.h"
#include "oxygencairosurface.h"

#include <cairo.h>
#include <vector>

namespace Oxygen
{

    // forward declaration
    namespace Cairo
    { class Context; }

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
        TileSet( const Cairo::Surface& surface, int w1, int h1, int w2, int h2 );

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
        TileSet( const Cairo::Surface& surface, int w1, int h1, int w3, int h3, int x2, int y2, int w2, int h2 );

        //! destructor
        virtual ~TileSet();

        /*!
        Flags specifying what sides to draw in ::render. Corners are drawn when
        the sides forming that corner are drawn, e.g. Top|Left draws the
        top-center, center-left, and top-left chunks. The center-center chunk is
        only drawn when Center is requested.
        */
        enum Tile
        {
            Top = 1<<0,
            Left = 1<<1,
            Bottom = 1<<2,
            Right = 1<<3,
            Center = 1<<4,
            TopLeft = Top|Left,
            TopRight = Top|Right,
            BottomLeft = Bottom|Left,
            BottomRight = Bottom|Right,
            Ring = Top|Left|Bottom|Right,
            Horizontal = Left|Right|Center,
            Vertical = Top|Bottom|Center,
            Full = Ring|Center
        };

        OX_DECLARE_FLAGS( Tiles, Tile );

        //! return size associated to this tileset
        int width( void ) const
        { return _w1 + _w3; }

        //! return size associated to this tileset
        int height( void ) const
        { return _h1 + _h3; }

        //! is valid
        bool isValid( void ) const
        { return _surfaces.size() == 9; }

        /*!
        Fills the specified rect with tiled chunks. Corners are never tiled,
        edges are tiled in one direction, and the center chunk is tiled in both
        directions. Partial tiles are used as needed so that the entire rect is
        perfectly filled. Filling is performed as if all chunks are being drawn.
        */
        void render( cairo_t*, int x, int y, int w, int h, unsigned int = Ring) const;

        //! returns surface for given index
        const Cairo::Surface& surface( unsigned int index ) const
        {
            assert( index < _surfaces.size() );
            return _surfaces[index];
        }

        protected:

        //!@name internal constructors
        //@{
        void init( const Cairo::Surface&, int w1, int h1, int w2, int h2 );
        void init( const Cairo::Surface&, int w1, int h1, int w3, int h3, int x2, int y2, int w2, int h2 );
        //@}

        //! shortcut to pixmap list
        typedef std::vector< Cairo::Surface > SurfaceList;

        //! initialize pixmap
        void initSurface( SurfaceList&, const Cairo::Surface&, int w, int h, int sx, int sy, int sw, int sh );

        //! copy pixmap
        void copySurface( cairo_t*, int x, int y, const Cairo::Surface&, int sx, int sy, int sw, int sh, cairo_extend_t ) const;

        private:

        //! pixmap arry
        SurfaceList _surfaces;

        // dimensions
        int _w1;
        int _h1;
        int _w3;
        int _h3;

    };

    OX_DECLARE_OPERATORS_FOR_FLAGS( TileSet::Tiles );

}

#endif
