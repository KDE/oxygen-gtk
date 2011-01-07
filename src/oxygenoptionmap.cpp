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
#include <fstream>

namespace Oxygen
{

    //_________________________________________________________
    OptionMap::OptionMap( const std::string& filename )
    {

        std::ifstream in( filename.c_str() );
        if( !in ) return;

        std::string currentSection;
        std::string currentLine;
        while( std::getline( in, currentLine, '\n' ) )
        {

            if( currentLine.empty() ) continue;

            // check if line is a section
            if( currentLine[0] == '[' )
            {

                size_t end( currentLine.rfind( ']' ) );
                if( end == std::string::npos ) continue;
                currentSection = currentLine.substr( 0, end+1 );

            } else if( currentSection.empty() ) {

                continue;

            }

            // check if line is a valid option
            size_t mid( currentLine.find( '=' ) );
            if( mid == std::string::npos ) continue;

            // insert new option in map
            Option option( currentLine.substr( 0, mid ), currentLine.substr( mid+1 ) );

            #if OXYGEN_DEBUG
            option.setFile( filename );
            #endif

            (*this)[currentSection].insert( option );

        }

    }

    //_________________________________________________________
    OptionMap& OptionMap::merge( const OptionMap& other )
    {

        // loop over source maps
        for( const_iterator iter = other.begin(); iter != other.end(); ++iter )
        {

            iterator source_iter( find( iter->first ) );
            if( source_iter == end() )
            {

                // if section is not found in this map, insert it as a whole
                insert( std::make_pair( iter->first, iter->second ) );

            } else {

                // otherwise merge both sets
                for( Option::Set::const_iterator optionIter = iter->second.begin(); optionIter != iter->second.end(); ++optionIter )
                {

                    source_iter->second.erase( *optionIter );
                    source_iter->second.insert( *optionIter );

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

        Option::Set::const_iterator optionIter( iter->second.find( Option(tag) ) );
        return optionIter != iter->second.end();
    }

    //_________________________________________________________
    Option OptionMap::getOption( const std::string& section, const std::string& tag ) const
    {
        const_iterator iter( find( section ) );
        if( iter == end() )
        {
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::OptionMap::getOption - could not find section " << section << std::endl;
            #endif

            return Option();

        }

        Option::Set::const_iterator optionIter( iter->second.find( Option(tag) ) );
        if( optionIter == iter->second.end() )
        {
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::OptionMap::getOption - could not find tag " << tag << std::endl;
            #endif

            return Option();

        } else {

            return *optionIter;

        }
    }

}
