#ifndef oxygenoption_h
#define oxygenoption_h
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

#include "config.h"

#include <iostream>
#include <set>
#include <sstream>
#include <string>

namespace Oxygen
{

    //! used to store values from kde-like rc file
    class Option
    {
        public:

        typedef std::set<Option> Set;

        //! constructor
        explicit Option( const std::string& tag = std::string(), const std::string& value = std::string() ):
            _tag(tag),
            _value(value)
        {}

        //! destructor
        virtual ~Option( void )
        {}

        //! equal to operator
        bool operator == (const Option& other ) const
        { return tag() == other.tag(); }

        //! equal to operator
        bool operator == (const std::string& other ) const
        { return tag() == other; }

        //! less than operator
        bool operator < (const Option& other ) const
        { return tag() < other.tag(); }

        //! tag
        const std::string& tag( void ) const
        { return _tag; }

        //! value
        const std::string& value( void ) const
        { return _value; }

        #if OXYGEN_DEBUG
        //! set filename from where option was read
        void setFile( const std::string& file )
        { _file = file; }
        #endif

        //! convert to integer
        int toInt( int defaultValue ) const
        { return toVariant<int>( defaultValue ); }

        template< typename T>  T toVariant( T = T() ) const;

        #if OXYGEN_DEBUG
        protected:
        const std::string& file( void ) const
        { return _file; }
        #endif

        private:

        //! tag
        std::string _tag;

        //! value
        std::string _value;

        //! file (used for debugging)
        std::string _file;

        //! streamer
        friend std::ostream& operator << (std::ostream& out, const Option& option )
        {
            out << option.tag() << "=" << option.value();

            #if OXYGEN_DEBUG
            if( !option.file().empty() )
            { out << " (" << option.file() << ")"; }
            #endif

            return out;
        }

        //! streamer
        friend std::ostream& operator << (std::ostream& out, const Option::Set& options )
        {
            for( Option::Set::const_iterator iter = options.begin(); iter != options.end(); ++iter )
            { out << *iter << std::endl; }

            return out;
        }

    };

    //_______________________________________________________________________
    template<typename T> T Option::toVariant( T defaultValue ) const
    {

        T out;
        std::istringstream stream( _value );
        return ( stream >> out ) ? out:defaultValue;
    }

}

#endif
