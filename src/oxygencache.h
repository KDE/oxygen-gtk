#ifndef oxygencache_h
#define oxygencache_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config.h"

#include <algorithm>
#include <map>
#include <deque>

namespace Oxygen
{

    //! simple firt-in first-out stl based cache
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
    class SimpleCache
    {

        public:

        //!@name convernience typenames
        typedef T Key;
        typedef M Value;
        typedef std::map<T,M> Map;
        typedef typename Map::iterator iterator;
        typedef typename Map::const_iterator const_iterator;

        //! creator
        SimpleCache( size_t size = 100, M defaultValue = M() ):
            _maxSize( size ),
            _defaultValue( defaultValue )
            {}

        //! copy constructor
        inline SimpleCache( const SimpleCache<T,M>& );

        //! destructor
        virtual ~SimpleCache( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { erase( iter->second ); }
        }

        //! assignment
        inline SimpleCache<T, M>& operator = (const SimpleCache<T, M>& );

        //! clear cache
        virtual void clear( void )
        {
            for( typename Map::iterator iter = _map.begin(); iter != _map.end(); iter++ )
            { erase( iter->second ); }
            _map.clear();
            _keys.clear();
        }

        //! insert pair in cache
        inline const M& insert( const T&, const M& );

        //! returns true if cache contains key
        /*! cannot be const, because key gets promoted when contained */
        inline bool contains( const T& );

        //! find item in map
        /*! cannot be const, because key gets promoted when found */
        inline iterator find( const T& );

        //! return value for given key, or defaultValue if not found
        inline const M& value( const T& );

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

        //! promote key to front of the list
        virtual inline void promote( const T& );

        //! adjust cache size
        inline void adjustSize( void );

        //! give access to key list to derived classes
        typedef std::deque<const T*> List;
        List& keys( void )
        { return _keys; }

        private:

        //! cache maximum size
        size_t _maxSize;

        //! map
        Map _map;

        //! keys
        List _keys;

        M _defaultValue;

    };

    //______________________________________________________________________
    template <typename T, typename M>
    SimpleCache<T,M>::SimpleCache( const SimpleCache<T,M>& other ):
        _maxSize( other._maxSize ),
        _map( other._map )
    {

        // loop over keys in other list
        for( typename List::const_iterator key_iter = other._keys.begin(); key_iter != other._keys.end(); key_iter++ )
        {
            // find corresponding iterator
            typename Map::const_iterator iter( _map.find( **key_iter ) );

            // insert address of corresponding key in key list
            _keys.push_back( &iter->first );
        }
    }

    //______________________________________________________________________
    template <typename T, typename M>
    SimpleCache<T,M>& SimpleCache<T,M>::operator = (const SimpleCache<T,M>& other )
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

            // insert address of corresponding key in key list
            _keys.push_back( &iter->first );
        }

        return *this;
    }

    //______________________________________________________________________
    template <typename T, typename M>
    const M& SimpleCache<T,M>::insert( const T& key, const M& value )
    {

        typename Map::iterator iter = _map.find( key );
        if( iter == _map.end() )
        {

            // insert in map, and push key in list
            iter = _map.insert( std::make_pair( key, value ) ).first;
            const Key& internal_key( iter->first );
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

        // return newly inserted value
        return iter->second;

    }

    //______________________________________________________________________
    template <typename T, typename M>
    bool SimpleCache<T,M>::contains( const T& key )
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
    typename SimpleCache<T,M>::iterator SimpleCache<T,M>::find( const T& key )
    {
        typename Map::iterator iter = _map.find( key );
        if( iter != _map.end() ) promote( iter->first );
        return iter;
    }

    //______________________________________________________________________
    template <typename T, typename M>
    const M& SimpleCache<T,M>::value( const T& key )
    {
        typename Map::iterator iter = _map.find( key );
        if( iter == _map.end() ) return _defaultValue;
        else {
            promote( iter->first );
            return iter->second;
        }
    }

    //______________________________________________________________________
    template <typename T, typename M>
    void SimpleCache<T,M>::promote( const T& key )
    {}

    //______________________________________________________________________
    template <typename T, typename M>
    void SimpleCache<T,M>::adjustSize( void )
    {

        while( _keys.size() > _maxSize )
        {

            // get last key in key list
            const Key* key( _keys.back() );

            // find matching item in map
            typename Map::iterator iter( _map.find( *key ) );

            // delete value
            erase( iter->second );

            // remove item from map
            _map.erase( iter );

            // remove key from key list
            _keys.pop_back();

        }

    }


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
    class Cache: public SimpleCache<T,M>
    {

        public:


        //! creator
        Cache( size_t size = 100, M defaultValue = M() ):
            SimpleCache<T,M>( size, defaultValue )
            {}

        protected:

        //! promote key to front of the list
        virtual inline void promote( const T& );

    };

    //______________________________________________________________________
    template <typename T, typename M>
    void Cache<T,M>::promote( const T& key )
    {

        if( (!SimpleCache<T,M>::keys().empty()) )
        {

            // do nothing if key is already up front
            if( SimpleCache<T,M>::keys().front() == &key ) return;

            // erase key in list
            typename SimpleCache<T,M>::List::iterator iter( std::find( SimpleCache<T,M>::keys().begin(), SimpleCache<T,M>::keys().end(), &key ) );
            SimpleCache<T,M>::keys().erase( iter );

        }

        // (re) add key up front
        SimpleCache<T,M>::keys().push_front( &key );

    }

}

#endif
