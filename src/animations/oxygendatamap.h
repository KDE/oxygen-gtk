#ifndef oxygendatamap_h
#define oxygendatamap_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
#include <gtk/gtk.h>

namespace Oxygen
{

    //! generic class to map data to widgets
    /*
    Note: I'm not sure about memory management. At some point one might need to allocate the registered
    data on the heap rather than on the stack, to be able to safely pass the data pointer around via callbacks.
    The current implementation should make that possible without external code change, provided that the map
    content is properly deleted (as opposed to erased) in destructor and 'unregister' method.
    */
    template <typename T>
    class DataMap
    {

        public:

        //! constructor
        DataMap( void ):
            _lastWidget( 0L ),
            _lastData( 0L )
        {}

        virtual ~DataMap(){}

        //! insert new widget
        inline T& registerWidget( GtkWidget* widget )
        {
            T& data( _map.insert( std::make_pair( widget, T() ) ).first->second );
            _lastWidget = widget;
            _lastData = &data;
            return data;
        }

        //! true if widget is in list
        virtual bool contains( GtkWidget* widget )
        {

            // check against last widget
            if( widget == _lastWidget ) return true;

            // find in map, returns false if not found
            typename Map::iterator iter = _map.find( widget );
            if( iter == _map.end() ) return false;

            // store as last widget/last data, to speed up lookup.
            _lastWidget = widget;
            _lastData = &iter->second;
            return true;

        }

        //! return value
        virtual T& value( GtkWidget* widget )
        {

            // check against last widget
            if( widget == _lastWidget ) return *_lastData;

            // find in map, abort if not found
            typename Map::iterator iter(  _map.find( widget ) );
            assert( iter != _map.end() );

            // store as last widget/last data, to speed up lookup.
            _lastWidget = widget;
            _lastData = &iter->second;
            return iter->second;

        }

        //! erase
        virtual void erase( GtkWidget* widget )
        {

            // clear last widget and data, if match
            if( _lastWidget == widget )
            {
                _lastWidget = 0L;
                _lastData = 0L;
            }

            // erase from map
            _map.erase( widget );

        }

        //! connect all widgets in map
        void connectAll( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { iter->second.connect( iter->first ); }
        }


        //! connect all widgets in map
        void disconnectAll( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { iter->second.disconnect( iter->first ); }
        }

        //! erase
        virtual void clear( void )
        {

            _lastWidget = 0L;
            _lastData = 0L;
            _map.clear();

        }

        //! retrieve internal map
        typedef std::map<GtkWidget*, T> Map;
        Map& map( void )
        { return _map; }

        protected:

        //! copy constructor is private
        DataMap( const DataMap& )
        { assert( false ); }

        //! assignment operator
        DataMap& operator = ( const DataMap& )
        {
            assert( false );
            return *this;
        }

        private:

        //! pointer to last inquired widget
        GtkWidget* _lastWidget;

        //! pointer to last retrieved data
        T* _lastData;

        //! internal map between widget and data
        Map _map;

    };

}

#endif
