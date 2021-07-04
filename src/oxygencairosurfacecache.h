#ifndef oxygencairosurfacecache_h
#define oxygencairosurfacecache_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygencache.h"
#include "oxygencairosurface.h"

#include <cairo.h>

namespace Oxygen
{


    template< typename T>
    class CairoSurfaceCache: public Cache<T, Cairo::Surface>
    {

        public:

        //! constructor
        CairoSurfaceCache( size_t size = 100 ):
            Cache<T, Cairo::Surface>( size )
        {}

        //! destructor
        virtual ~CairoSurfaceCache( void )
        {}

    };

}

#endif
