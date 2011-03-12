#ifndef oxygencairoregion_h
#define oxygencairoregion_h
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

#include <cairo.h>
#include <cassert>

namespace Oxygen
{
    namespace Cairo
    {

        //! wrapper class around cairo_region_t structure
        class Region
        {
            public:

            //! empty constructor is private
            Region( void ):
                _region( 0L )
            {}

            //! constructor
            Region( cairo_region_t* region ):
                _region( region )
            {}

            //! destructor
            virtual ~Region( void )
            { free(); }


            //! copy constructor is private
            Region( const Region& other ):
                _region( other._region )
            { if( _region ) cairo_region_copy( _region ); }

            //! equal to operator is private
            Region& operator = (const Region& other )
            {
                cairo_region_t* old( _region );
                _region = other._region;
                if( _region ) cairo_region_copy( _region );
                if( old ) cairo_region_destroy( old );
                return *this;
            }

            //! set region
            void set( cairo_region_t* region )
            {
                assert( !_region );
                _region = region;
            }

            //! free the region
            /*!
            it should not be necessary to call this method
            since it is already handled in destructor
            */
            void free( void )
            {
                if( _region )
                {
                    cairo_region_destroy( _region );
                    _region = 0L;
                }
            }

            //! cast to cairo_region_t
            operator cairo_region_t* (void) const
            { return _region; }

            private:

            //! region
            cairo_region_t* _region;


        };

    }
}
#endif
