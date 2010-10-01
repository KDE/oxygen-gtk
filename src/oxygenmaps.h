#ifndef oxygenmaps_h
#define oxygenmaps_h
/*
* this file is part of the oxygen gtk engine
* Copyright(c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright(C ) 2008 Robert Staudinger
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include <glib.h>
#include <gtk/gtk.h>
#include <string>
#include <iostream>

namespace Oxygen
{

    //! css to gtk mapping
    class Maps
    {

        public:
        //@{

        static GtkStateType matchState( const char* );
        static const char* getState( GtkStateType );

        static GtkShadowType matchShadow( const char* );
        static const char* getShadow( GtkShadowType );

        static GtkArrowType matchArrow( const char* );
        static const char* getArrow( GtkArrowType );

        static GtkPositionType matchPosition( const char* );
        static const char* getPosition( GtkPositionType );

        static GdkWindowEdge matchWindowEdge	( const char* );
        static const char* getWindowEdge( GdkWindowEdge );

        static GtkOrientation matchOrientation( const char* );
        static const char* getOrientation( GtkOrientation );

        static GtkExpanderStyle	matchExpanderStyle( const char* );
        static const char* getExpanderStyle( GtkExpanderStyle );

        //@}

        //__________________________________________
        //! template class to handle css to gtk maps
        template< typename T> struct Entry
        {
            public:

            T gtk_value;
            std::string css_value;
        };

        //__________________________________________
        //! template class to handle css to gtk maps
        template< typename T> class Finder
        {
            public:

            typedef const Entry<T>* ValueList;

            //! constructor
            Finder( ValueList values, unsigned int size ):
                _values( values ),
                _size( size )
            {}

            //! return gtk value matching css
            T findGtk( const char* css_value, const T& default_value )
            {
                g_return_val_if_fail( css_value, default_value );
                for( unsigned int i = 0; i < _size; i++ )
                {
                    if( _values[i].css_value == css_value )
                    { return _values[i].gtk_value; }
                }

                return default_value;
            }

            //! return css value matching gtk
            const char* findCss( const T& gtk_value )
            {
                for( unsigned int i = 0; i < _size; i++ )
                {
                    if( _values[i].gtk_value == gtk_value )
                    { return _values[i].css_value.c_str(); }
                }

                return 0L;

            }

            private:

            unsigned int _size;
            ValueList _values;

        };

    };
}

#endif
