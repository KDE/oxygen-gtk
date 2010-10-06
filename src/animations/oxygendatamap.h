#ifndef oxygendatamap_h
#define oxygendatamap_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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

#include <cassert>
#include <map>
#include <gtk/gtkwidget.h>

namespace Oxygen
{

    template <typename T>
    class DataMap: public std::map<GtkWidget*, T>
    {

        public:

        //! insert new widget
        inline virtual void insert( GtkWidget* widget, const T& value = T())
        { Map::insert( std::make_pair( widget, value ) ); }

        //! true if widget is in list
        virtual bool contains( GtkWidget* widget ) const
        { return Map::find( widget ) != Map::end(); }

        //! return value
        virtual const T& value( GtkWidget* widget ) const
        {
            typename Map::const_iterator iter( Map::find( widget ) );
            assert( iter != Map::end() );
            return iter->second;
        }

        //! return value
        virtual T& value( GtkWidget* widget )
        {
            typename Map::iterator iter(  Map::find( widget ) );
            assert( iter != Map::end() );
            return iter->second;
        }

        //! erase
        virtual void erase( GtkWidget* widget )
        { Map::erase( widget ); }

        private:

        //! shortcut
        typedef std::map<GtkWidget*, T> Map;


    };

}

#endif
