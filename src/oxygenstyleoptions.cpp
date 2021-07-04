/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenstyleoptions.h"

#include <vector>
#include <string>

namespace Oxygen
{

    //______________________________________________________________
    std::ostream& operator << (std::ostream& out, const StyleOptions& options )
    {
        std::vector<std::string> flags;
        if( options & Blend ) flags.push_back( "Blend" );
        if( options & Sunken ) flags.push_back( "Sunken" );
        if( options & Flat ) flags.push_back( "Flat" );
        if( options & Focus ) flags.push_back( "Focus" );
        if( options & Hover ) flags.push_back( "Hover" );
        if( options & NoFill ) flags.push_back( "NoFill" );
        if( options & Vertical ) flags.push_back( "Vertical" );
        if( options & Alpha ) flags.push_back( "Alpha" );
        if( options & Round ) flags.push_back( "Round" );
        if( options & Contrast ) flags.push_back( "Contrast" );
        if( options & Selected ) flags.push_back( "Selected" );
        if( options & Disabled ) flags.push_back( "Disabled" );
        if( options & Menu ) flags.push_back( "Menu" );

        if( flags.empty() ) out << "None";
        else {

            for( std::vector<std::string>::const_iterator iter = flags.begin(); iter != flags.end(); ++iter )
            {
                if( iter == flags.begin() ) out << *iter;
                else out << "|" << *iter;
            }

        }

        return out;

    }

}
