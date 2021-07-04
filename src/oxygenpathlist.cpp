/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenpathlist.h"

#include <sstream>

namespace Oxygen
{

    //_________________________________________________________
    void PathList::split( const std::string& path, const std::string& separator )
    {

        clear();
        std::string local( path );
        if( local.empty() ) return;
        if( local[local.size()-1] == '\n' ) local = local.substr( 0, local.size()-1 );

        size_t position( std::string::npos );
        while( ( position = local.find( separator ) ) != std::string::npos )
        {
            push_back( local.substr(0, position ) );
            local = local.substr( position + separator.length() );
        }

        if( !local.empty() ) push_back( local );
        return;

    }

    //_________________________________________________________
    std::string PathList::join( const std::string& separator ) const
    {
        std::ostringstream out;
        for( const_iterator iter = begin(); iter != end(); ++iter )
        {
            if( iter != begin() ) out << separator;
            out << *iter;
        }

        return out.str();
    }

}
