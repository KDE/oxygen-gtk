#ifndef ObjectCounterMap_h
#define ObjectCounterMap_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <map>
#include <string>

//! thread-safe Object counter storage map
namespace Oxygen
{
    class ObjectCounterMap: public std::map<std::string, int>
    {

        public:

        //! singleton
        static ObjectCounterMap& get( void );

        //!  get counter for a given name
        /*!
        if the name is found, returns adress of the existing counter
        creates new counter otherwise and returns adress
        */
        int* counter( const std::string& name )
        {
            iterator iter = find( name );
            if( iter == end() ) return &(insert( std::make_pair( name, 0 ) ).first->second);
            else return &(iter->second);
        }

        //! increment
        void increment( int& counter )
        { counter++; }

        //! increment
        void decrement( int& counter )
        { counter--; }

        private:

        //! constructor
        ObjectCounterMap( void )
        {}

    };

}

#endif
