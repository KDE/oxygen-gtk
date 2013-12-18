#ifndef oxygenoptionmap_h
#define oxygenoptionmap_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenoption.h"

#include <map>
#include <set>
#include <string>

namespace Oxygen
{
    // all options from kde globals
    class OptionMap: public std::map<std::string, Option::Set>
    {
        public:

        //! constructor
        OptionMap( void )
        {}

        //! constructor from filename
        OptionMap( const std::string& );

        //! destructor
        virtual ~OptionMap( void )
        {}

        //! equal to operator
        bool operator == ( const OptionMap& ) const;

        //! differ from operator
        bool operator != (const OptionMap& other ) const
        { return !(*this == other ); }

        //! merge with other map
        /*! when options are duplicated between two maps, the second overwrite the first */
        OptionMap& merge( const OptionMap& );

        //! true if option is in map
        bool hasOption( const std::string& section, const std::string& tag ) const;

        //! find option in map
        Option getOption( const std::string& section, const std::string& tag ) const;

        //! find option in map
        std::string getValue( const std::string& section, const std::string& tag, const std::string& defaultValue = "" ) const
        {
            Option option( getOption( section, tag ) );
            return option == tag ? option.value():defaultValue;
        }

        //! streamer
        friend std::ostream& operator << (std::ostream& out, const OptionMap& options )
        {
            for( OptionMap::const_iterator iter = options.begin(); iter != options.end(); ++iter )
            {
                out << iter->first << std::endl;
                out << iter->second << std::endl;
            }
            return out;
        }

    };

}

#endif
