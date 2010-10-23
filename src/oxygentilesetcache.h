#ifndef oxygentilesetcache_h
#define oxygentilesetcache_h

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

#include <map>

namespace Oxygen
{

    // forward declaration
    class TileSet;

    template< typename T>
    class TileSetCache
    {

        public:

        //! key type
        typedef T Key;

        private:

        //! internal map
        typedef std::map<Key, TileSet*> Map;

        public:

        //! constructor
        TileSetCache( void )
        {}

        //! destructor
        ~TileSetCache( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { delete iter->second; }
        }

        //! return tileset matching key, or NULL if not found
        TileSet* find( const Key& key ) const
        {
            typename Map::const_iterator iter( _map.find( key ) );
            if( iter == _map.end() ) return 0L;
            else return iter->second;
        }

        //! insert in cache
        void insert( const Key& key, TileSet* tileSet )
        {
            typename Map::iterator iter( _map.find( key ) );
            if( iter != _map.end() ) delete iter->second;
            _map.insert( std::make_pair( key, tileSet ) );
        }

        //! clear
        void clear( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { delete iter->second; }

            _map.clear();
        }

        private:

        Map _map;

    };

}

#endif
