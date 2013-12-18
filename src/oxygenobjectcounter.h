#ifndef Counter_h
#define Counter_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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


#include <string>
#include <map>


namespace Oxygen
{

    class ObjectCounter
    {

        public:

        //! counter name and counts pair
        typedef std::pair<std::string, int> Pair;

        //! constructor
        ObjectCounter( const std::string& name );

        //! constructor
        ObjectCounter( const ObjectCounter& counter );

        //! destructor
        virtual ~ObjectCounter( void );

        //! retrieves counter count
        int count( void ) const;

        private:

        //! pointer to integer counter
        int* count_;

    };

}

#endif
