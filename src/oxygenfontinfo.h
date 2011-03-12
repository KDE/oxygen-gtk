#ifndef oxygenfontinfo_h
#define oxygenfontinfo_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include <string>
#include <map>

namespace Oxygen
{

    class FontInfo
    {

        public:

        //! font types
        enum FontType
        {
            Default,
            Desktop,
            Fixed,
            Menu,
            Small,
            Taskbar,
            ToolBar
        };

        typedef std::map<FontType, FontInfo> Map;

        //! font weight
        enum FontWeight
        {
            Light = 0,
            Normal = 38,
            DemiBold = 57,
            Bold = 69,
            Black = 81
        };

        //! empty constructor
        FontInfo( void ):
            _weight( Normal ),
            _italic( false ),
            _fixed( false ),
            _size( 0 )
        {}

        //! true if font is valid
        bool isValid( void ) const
        { return _size > 0 && !_family.empty(); }

        //! weight
        FontWeight weight( void ) const
        { return _weight; }

        //! italic
        bool italic( void ) const
        { return _italic; }

        //! fixed width
        bool fixed( void ) const
        { return _fixed; }

        //! size
        double size( void ) const
        { return _size; }

        //! family
        const std::string& family( void ) const
        { return _family; }

        //! convert to gtk string
        std::string toString( bool addQuotes = true ) const;

        //! utilities
        static FontInfo fromKdeOption( std::string );

        std::string weightString( void ) const;
        std::string italicString( void ) const;

        private:

        //! font weight
        FontWeight _weight;

        //! true if italic
        bool _italic;

        //! true if fixed width
        bool _fixed;

        //! font size
        double _size;

        //! family
        std::string _family;

    };



}

#endif
