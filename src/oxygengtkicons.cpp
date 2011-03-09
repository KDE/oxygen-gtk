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

#include "oxygengtkicons.h"
#include "config.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Oxygen
{

    //_________________________________________
    GtkIcons::GtkIcons( void ):
        _dirty( true )
    {

        // initialize sizes
        _sizes.push_back( std::make_pair( "panel-menu", 16 ) );
        _sizes.push_back( std::make_pair( "panel", 32 ) );
        _sizes.push_back( std::make_pair( "gtk-small-toolbar", 22 ) );
        _sizes.push_back( std::make_pair( "gtk-large-toolbar", 22 ) );
        _sizes.push_back( std::make_pair( "gtk-dnd", 48 ) );
        _sizes.push_back( std::make_pair( "gtk-button", 16 ) );
        _sizes.push_back( std::make_pair( "gtk-menu", 16 ) );
        _sizes.push_back( std::make_pair( "gtk-dialog", 32 ) );
        _sizes.push_back( std::make_pair( "", 16 ) );

    }

    //_________________________________________
    void GtkIcons::setIconSize( const std::string& tag, unsigned int value )
    {
        SizeMap::iterator iter( std::find_if( _sizes.begin(), _sizes.end(), SameTagFTor( tag ) ) );
        if( iter == _sizes.end() ) {

            std::cerr << "GtkIcons::setIconSize - no match for" << tag << "," << value << std::endl;

        } else if( iter->second != value ) {

            iter->second = value;
            _dirty = true;

        }

    }

    //_________________________________________
    std::string GtkIcons::generate( void )
    {

        std::ostringstream out;
        for( SizeMap::const_iterator iter = _sizes.begin(); iter != _sizes.end(); ++iter )
        {
            if( iter->first.empty() ) continue;
            if( iter != _sizes.begin() ) out << ": ";
            out << iter->first << " = " << iter->second << "," << iter->second;
        }

        _dirty = false;

        return out.str();

    }

}
