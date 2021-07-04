#ifndef oxygenfontinfo_h
#define oxygenfontinfo_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
