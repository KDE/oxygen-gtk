#ifndef oxygencairocontext_h
#define oxygencairocontext_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include <gdk/gdk.h>

#include <cassert>

namespace Oxygen
{
    namespace Cairo
    {

        //! wrapper class around cairo_t structure
        class Context
        {
            public:

            //! constructor
            explicit Context( GdkWindow*, GdkRectangle* = 0L );

            //! constructor
            explicit Context( cairo_surface_t*, GdkRectangle* = 0L );

            //! destructor
            virtual ~Context( void )
            { free(); }

            //! free the context
            /*!
            it should not be necessary to call this method
            since it is already handled in destructor
            */
            void free( void );

            //! cast to cairo_t
            operator cairo_t* (void) const
            { return _cr; }

            //! context accessor
            cairo_t* cr( void ) const
            { return _cr; }

            //! set clipping
            void setClipping( GdkRectangle* ) const;

            private:

            //! empty constructor is private
            explicit Context( void ):
                _cr( 0L )
            { assert( false ); }

            //! copy constructor is private
            Context( const Context& other ):
                _cr( 0L )
            { assert( false ); }

            //! equal to operator is private
            Context& operator = (const Context& other )
            {
                _cr = other._cr;
                assert( false );
                return *this;
            }

            //! equal to operator is private
            Context& operator = (cairo_t* other )
            {
                assert( false );
                return *this;
            }

            //! cairo contect
            cairo_t* _cr;

        };

    }
}
#endif
