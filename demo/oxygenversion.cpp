/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

#include "oxygenversion.h"
#include "../src/oxygendefines.h"
#include "config.h"

#include <iostream>
#include <glib-object.h>

namespace Oxygen
{

    //____________________________________________
    std::string Version::libVersion( void )
    {

        // load type for oxygen version container
        GType type( g_type_from_name( OXYGEN_VERSION_TYPE_NAME ) );
        if( !type ) return std::string();

        // load quark
        GQuark quark( g_quark_try_string( OXYGEN_VERSION_QUARK_NAME ) );
        if( !quark ) return std::string();

        // retrieve data from type
        gpointer data( g_type_get_qdata( type, quark ) );
        return data ? std::string( static_cast<const char*>( data ) ):std::string();

    }

    //____________________________________________
    bool Version::processCommandLine( int argc, char** argv )
    {

        // this is not robust at all
        if(argc>1)
        {
            if(std::string(argv[1])=="--version")
            {
                std::cout << "oxygen-gtk-demo version:    " << OXYGEN_VERSION << std::endl;

                const std::string libraryVersion( libVersion() );
                if( !libraryVersion.empty() )
                { std::cout << "oxygen-gtk library version: " << libraryVersion << std::endl; }

                return true;
            }
        }

        return false;
    }
}
