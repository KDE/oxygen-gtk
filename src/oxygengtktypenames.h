#ifndef oxygengtktypenames_h
#define oxygengtktypenames_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <string>
#include <iostream>

namespace Oxygen
{

    namespace Gtk
    {
        //! css to gtk mapping
        class TypeNames
        {

            public:
            //@{

            //! widget state
            static const char* state( GtkStateType );

            //! shadow type
            static const char* shadow( GtkShadowType );

            //! arrow type
            static const char* arrow( GtkArrowType );

            //! position
            static const char* position( GtkPositionType );

            //! edge
            static const char* windowEdge( GdkWindowEdge );

            //! window type
            static const char* windowTypeHint( GdkWindowTypeHint );

            //! widget orientation
            static const char* orientation( GtkOrientation );

            //! tree expander style
            static const char* expanderStyle( GtkExpanderStyle );

            //! Gtk dialogs response IDs
            static const char* response( GtkResponseType );

            //! icon size
            static const char* iconSize( GtkIconSize );

            //! file monitor event
            static const char* fileMonitorEvent( GFileMonitorEvent );

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
                    _size( size ),
                    _values( values )
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

                    return "";

                }

                private:

                unsigned int _size;
                ValueList _values;

            };

            protected:

            static GtkStateType matchState( const char* );
            static GtkShadowType matchShadow( const char* );
            static GtkArrowType matchArrow( const char* );
            static GtkPositionType matchPosition( const char* );
            static GdkWindowEdge matchWindowEdge	( const char* );
            static GdkWindowTypeHint matchWindowTypeHint( const char* );
            static GtkOrientation matchOrientation( const char* );
            static GtkExpanderStyle	matchExpanderStyle( const char* );
            static GtkResponseType matchResponse( const char* );
            static GtkIconSize matchIconSize( const char* );
            static GFileMonitorEvent matchFileMonitorEvent( const char* );

        };
    }

}

#endif
