#ifndef oxygencairocontext_h
#define oxygencairocontext_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
