#ifndef oxygentilesetcache_h
#define oxygentilesetcache_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygencache.h"

namespace Oxygen
{

    // forward declaration
    class TileSet;

    template< typename T>
    class TileSetCache: public Cache<T, TileSet>
    {

        public:

        //! constructor
        TileSetCache( size_t size = 100 ):
            Cache<T, TileSet>( size )
        {}

        //! destructor
        virtual ~TileSetCache( void )
        {}

    };

}

#endif
