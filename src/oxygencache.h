#ifndef oxygencache_h
#define oxygencache_h

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

#include <cassert>
#include <map>
#include <list>
#include <iostream>

namespace Oxygen
{

    //! simple "most recently used" stl based cache
    /*!
    an stl::map is used to keep the association between key and value.
    an stl::list is used to keep track of the most recently used values.
    the list contains pointers to the keys inserted in the map, to avoid
    unnecessary copy constructors.
    the 'erase' method is used to delete objects that are removed from the cache.
    By default, the erase method does nothing. It must be reimplemented to deal with
    non default Value deletion, when, for instance, the cache is used to store pointers
    */
    template <typename T, typename M>
    class Cache
    {

        public:

        //!@name convernience typenames
        typedef T Key;
        typedef M Value;
        typedef std::map<T,M> Map;
        typedef typename Map::iterator iterator;
        typedef typename Map::const_iterator const_iterator;

        //! creator
        Cache( size_t size = 512 ):
            _maxSize( size )
            {}

        //! copy constructor
        inline Cache( const Cache<T,M>& );

        //! destructor
        virtual ~Cache( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { erase( iter->second ); }
        }

        //! assignment
        inline Cache<T, M>& operator = (const Cache<T, M>& );

        //! clear cache
        virtual void clear( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { erase( iter->second ); }
            _map.clear();
            _keys.clear();
        }

        //! insert pair in cache
        inline void insert( const T&, const M& );

        //! returns true if cache contains key
        /*! cannot be const, because key gets promoted when contained */
        inline bool contains( const T& );

        //! find item in map
        /*! cannot be const, because key gets promoted when found */
        inline iterator find( const T& );

        //! end
        inline iterator end( void )
        { return _map.end(); }

        //! end
        inline const_iterator end( void ) const
        { return _map.end(); }

        protected:

        //! erase value
        /*!
        by default, does nothing. Must be re-implemented with proper deletion
        if for istance cache is used to store pointers
        */
        virtual void erase( M& )
        {}

        private:

        //! promote key to front of the list
        inline void promote( const Key& );

        //! adjust cache size
        inline void adjustSize( void );

        //! cache maximum size
        size_t _maxSize;

        //! map
        Map _map;

        //! keys
        typedef std::list<const T*> List;
        List _keys;

    };

    //______________________________________________________________________
    template <typename T, typename M>
    Cache<T,M>::Cache( const Cache<T,M>& other ):
        _maxSize( other._maxSize ),
        _map( other._map )
    {

        // loop over keys in other list
        for( typename List::const_iterator key_iter = other._keys.begin(); key_iter != other._keys.end(); key_iter++ )
        {
            // find corresponding iterator
            typename Map::const_iterator iter( _map.find( **key_iter ) );
            assert( iter != _map.end() );

            // insert address of corresponding key in key list
            _keys.push_back( &iter->first );
        }
    }

    //______________________________________________________________________
    template <typename T, typename M>
    Cache<T,M>& Cache<T,M>::operator = (const Cache<T,M>& other )
    {
        // clear
        clear();

        // copy max size and map
        _maxSize = other._maxSize;
        _map = other._map;

        // loop over keys in other list
        for( typename List::const_iterator key_iter = other._keys.begin(); key_iter != other._keys.end(); key_iter++ )
        {
            // find corresponding iterator
            typename Map::const_iterator iter( _map.find( **key_iter ) );
            assert( iter != _map.end() );

            // insert address of corresponding key in key list
            _keys.push_back( &iter->first );
        }

        return *this;
    }

    //______________________________________________________________________
    template <typename T, typename M>
    void Cache<T,M>::insert( const T& key, const M& value )
    {

        typename Map::iterator iter = _map.find( key );
        if( iter == _map.end() )
        {

            // insert in map, and push key in list
            const Key& internal_key( _map.insert( std::make_pair( key, value ) ).first->first );
            _keys.push_front( &internal_key );

        } else {

            // delete existing value
            erase( iter->second );

            // assign new value
            iter->second = value;

            // move key to front of the list
            promote( iter->first );

        }

        // adjust size
        adjustSize();

    }

    //______________________________________________________________________
    template <typename T, typename M>
    bool Cache<T,M>::contains( const T& key )
    {
        typename Map::const_iterator iter = _map.find( key );
        if( iter == _map.end() ) return false;
        else {

            promote( iter->first );
            return true;

        }
    }

    //______________________________________________________________________
    template <typename T, typename M>
    typename Cache<T,M>::iterator Cache<T,M>::find( const T& key )
    {
        typename Map::iterator iter = _map.find( key );
        if( iter != _map.end() ) {

            #if OXYGEN_DEBUG
            std::cout << "Oxygen::Cache - re-using key: " << key << std::endl;
            #endif
            promote( iter->first );

        }
        return iter;
    }

    //______________________________________________________________________
    template <typename T, typename M>
    void Cache<T,M>::promote( const T& key )
    {

        // erase key in list
        _keys.remove( &key );
        _keys.push_front( &key );

    }

    //______________________________________________________________________
    template <typename T, typename M>
    void Cache<T,M>::adjustSize( void )
    {

        while( _keys.size() > _maxSize )
        {

            // get last key in key list
            const Key* key( _keys.back() );

            #if OXYGEN_DEBUG
            std::cout << "Oxygen::Cache - removing key: " << *key << std::endl;
            #endif

            // find matching item in map
            typename Map::iterator iter( _map.find( *key ) );
            assert( iter != _map.end() );

            // delete value
            erase( iter->second );

            // remove item from map
            _map.erase( iter );

            // remove key from key list
            _keys.pop_back();

        }

    }

}

#endif
