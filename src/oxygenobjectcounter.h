#ifndef Counter_h
#define Counter_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
