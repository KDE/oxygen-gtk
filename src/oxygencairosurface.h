#ifndef oxygencairosurface_h
#define oxygencairosurface_h
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

        //! wrapper class around cairo_surface_t structure
        class Surface
        {
            public:

            //! empty constructor is private
            Surface( void ):
                _surface( 0L )
            {}

            //! constructor
            Surface( cairo_surface_t* surface ):
                _surface( surface )
            {}

            //! destructor
            virtual ~Surface( void )
            { free(); }


            //! copy constructor is private
            Surface( const Surface& other ):
                _surface( other._surface )
            { if( _surface ) cairo_surface_reference( _surface ); }

            //! equal to operator is private
            Surface& operator = (const Surface& other )
            {
                cairo_surface_t* old( _surface );
                _surface = other._surface;
                if( _surface ) cairo_surface_reference( _surface );
                if( old ) cairo_surface_destroy( old );
                return *this;
            }

            //! set surface
            void set( cairo_surface_t* surface )
            {
                assert( !_surface );
                _surface = surface;
            }

            //! free the surface
            /*!
            it should not be necessary to call this method
            since it is already handled in destructor
            */
            void free( void )
            {
                if( _surface )
                {
                    cairo_surface_destroy( _surface );
                    _surface = 0L;
                }
            }

            //! cast to cairo_surface_t
            operator cairo_surface_t* (void) const
            { return _surface; }

            private:

            //! equal to operator is private
            Surface& operator = (cairo_surface_t* other )
            {
                assert( false );
                return *this;
            }

            //! surface
            cairo_surface_t* _surface;


        };

    }
}
#endif
