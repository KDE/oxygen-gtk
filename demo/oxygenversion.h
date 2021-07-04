#ifndef oxygenversion_h
#define oxygenversion_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <string>

namespace Oxygen
{

    class Version
    {
        public:

        //! gtk version
        static std::string gtkVersion( void );

        //! lib version
        static std::string libVersion( void );

        //! print options
        static void print( void );

    };

}


#endif
