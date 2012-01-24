#ifndef ObjectCounterMap_h
#define ObjectCounterMap_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2012 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
