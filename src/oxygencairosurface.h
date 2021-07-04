#ifndef oxygencairosurface_h
#define oxygencairosurface_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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

            //! returns true if valid
            bool isValid( void ) const
            { return (bool) _surface; }

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
