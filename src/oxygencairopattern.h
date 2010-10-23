#ifndef oxygencairopattern_h
#define oxygencairopattern_h
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
        
        //! wrapper class around cairo_pattern_t structure
        class Pattern
        {
            public:
            
            //! empty constructor is private
            Pattern( void ):
                _pattern( 0L )
            {}
                
            //! constructor
            Pattern( cairo_pattern_t* pattern ):
                _pattern( pattern )
            {}
            
            //! destructor
            virtual ~Pattern( void )
            { free(); }
            
            //! set pattern
            void set( cairo_pattern_t* pattern )
            {
                assert( !_pattern );
                _pattern = pattern;
            }
            
            //! free the pattern
            /*!
            it should not be necessary to call this method
            since it is already handled in destructor
            */
            void free( void );
            
            //! cast to cairo_pattern_t
            operator cairo_pattern_t* (void) const
            { return _pattern; }
            
            private:
            
            //! copy constructor is private
            Pattern( const Pattern& other ):
                _pattern( 0L )
            { assert( false ); }
                
            //! equal to operator is private
            Pattern& operator = (const Pattern& other )
            {
                _pattern = other._pattern;
                assert( false );
                return *this;
            }

            //! equal to operator is private
            Pattern& operator = (cairo_pattern_t* other )
            {
                assert( false );
                return *this;
            }

            //! pattern
            cairo_pattern_t* _pattern;


        };

    }
}
#endif
