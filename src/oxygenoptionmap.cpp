/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* inspired notably from kdelibs/kdeui/color/kcolorutils.h
* Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
* Copyright (C) 2007 Thomas Zander <zander@kde.org>
* Copyright (C) 2007 Zack Rusin <zack@kde.org>
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

#include "oxygenoptionmap.h"
#include "config.h"

#include <iostream>

namespace Oxygen
{

    //_________________________________________________________
    OptionMap& OptionMap::merge( const OptionMap& other )
    {

        // loop over source maps
        for( const_iterator iter = other.begin(); iter != other.end(); iter++ )
        {

            iterator source_iter( find( iter->first ) );
            if( source_iter == end() )
            {

                // if section is not found in this map, insert it as a whole
                insert( std::make_pair( iter->first, iter->second ) );

            } else {

                // otherwise merge both sets
                for( Option::Set::const_iterator option_iter = iter->second.begin(); option_iter != iter->second.end(); option_iter++ )
                {

                    source_iter->second.erase( *option_iter );
                    source_iter->second.insert( *option_iter );

                }

            }
        }

        return *this;
    }

    //_________________________________________________________
    bool OptionMap::hasOption( const std::string& section, const std::string& tag ) const
    {
        const_iterator iter( find( section ) );
        if( iter == end() ) return false;

        Option::Set::const_iterator option_iter( iter->second.find( tag ) );
        return option_iter != iter->second.end();
    }

    //_________________________________________________________
    Option OptionMap::getOption( const std::string& section, const std::string& tag ) const
    {
        const_iterator iter( find( section ) );
        if( iter == end() )
        {
            #if OXYGEN_DEBUG
            std::cout << "Oxygen::OptionMap::getOption - could not find section " << section << std::endl;
            #endif

            return Option();

        }

        Option::Set::const_iterator option_iter( iter->second.find( tag ) );
        if( option_iter == iter->second.end() )
        {
            #if OXYGEN_DEBUG
            std::cout << "Oxygen::OptionMap::getOption - could not find tag " << tag << std::endl;
            #endif

            return Option();

        } else {

            return *option_iter;

        }
    }

}
