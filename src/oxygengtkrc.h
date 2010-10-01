#ifndef oxygengtkrc_h
#define oxygengtkrc_h
/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include <iostream>
#include <list>
#include <string>

namespace Gtk
{

    //! handle gtkrc option generation
    class RC
    {

        public:

        //! constructor
        RC( void ):
            _currentSection( _rootSectionName )
        { _sections.push_back( Section( _rootSectionName ) ); }

        //! destructor
        virtual ~RC( void )
        {}

        //! clear
        void clear( void )
        { _sections.clear(); }

        //! add section
        void addSection( const std::string& name, const std::string& parent = std::string() );

        //! set current section
        void setCurrentSection( const std::string& name );

        //! add line to section
        void addToSection( const std::string& name, const std::string& content );

        //! add line to current section
        void addToCurrentSection( const std::string& content )
        { addToSection( _currentSection, content ); }

        //! add to root
        void addToRoot( const std::string& content )
        { addToSection( _rootSectionName, content ); }

        protected:

        class Section
        {

            public:

            //! list
            typedef std::list<Section> List;

            //! empty constructor
            explicit Section( void )
            {}

            //! constructor
            Section( const std::string& name, const std::string& parent = std::string() ):
                _name( name ),
                _parent( parent )
            {}

            //! clear
            void clear( void )
            { _content = std::string(); }

            //! add to content
            void add( const std::string& content )
            { _content += content; }

            //! equal operator. Based on name only
            bool operator == (const Section& other ) const
            { return other._name == _name; }

            //! name
            std::string _name;

            //! parent
            std::string _parent;

            //! content
            std::string _content;

        };

        private:

        //! root section name
        static const std::string _rootSectionName;

        //! list of sections
        Section::List _sections;

        //! current section
        std::string _currentSection;

        //! streamer
        friend std::ostream& operator << ( std::ostream&, const Section& );

        //! streamer
        friend std::ostream& operator << ( std::ostream&, const RC& );

    };
}

#endif
