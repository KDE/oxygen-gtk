#ifndef oxygengtkrc_h
#define oxygengtkrc_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include <sstream>
#include <string>
#include <vector>

namespace Oxygen
{
    namespace Gtk
    {

        //! convenience class to generate GtkRC option
        template< typename T>
            class RCOption
        {
            public:

            //! constructor
            RCOption( std::string name, const T& value )
            {
                std::ostringstream stream;
                stream << name << " = " << value;
                _value = stream.str();
            }

            //! cast to cairo_t
            operator const std::string& (void) const
            { return _value; }

            private:

            std::string _value;

        };

        //! handle gtkrc option generation
        class RC
        {

            public:

            //! constructor
            explicit RC( void )
            { init(); }

            //! destructor
            virtual ~RC( void )
            {}

            //! clear
            void clear( void )
            {
                _sections.clear();
                init();
            }


            //! merge
            void merge( const RC& );

            //! create new section and set as current
            void addSection( const std::string& name, const std::string& parent = std::string() );

            //! set current section
            void setCurrentSection( const std::string& name );

            //! add line to section
            void addToSection( const std::string& name, const std::string& content );

            //! add line to current section
            void addToCurrentSection( const std::string& content )
            { addToSection( _currentSection, content ); }

            //! add to header
            void addToHeaderSection( const std::string& content )
            { addToSection( _headerSectionName, content ); }

            //! add to root
            void addToRootSection( const std::string& content )
            { addToSection( _rootSectionName, content ); }

            //! match given widget selection to given section
            void matchClassToSection( const std::string& content, const std::string& name );
            void matchWidgetToSection( const std::string& content, const std::string& name );
            void matchWidgetClassToSection( const std::string& content, const std::string& name );

            //! default section name
            static const std::string& defaultSection( void )
            { return _defaultSectionName; }

            //! convert to string
            std::string toString( void ) const
            {
                std::ostringstream out;
                out << *this << std::endl;
                return out.str();
            }

            //! commit to gtk and clear
            void commit( void );

            protected:

            //! initialize default sections
            void init( void )
            {
                addSection( _headerSectionName );
                addSection( _rootSectionName );
                addSection( _defaultSectionName, "oxygen-default" );
                addToRootSection( std::string("class \"*\" style \"")+_defaultSectionName+"\"" );
            }

            //! describes each style section in resource list
            class Section
            {

                public:

                //! list
                typedef std::list<Section> List;

                //! empty constructor
                explicit Section( void )
                {}

                //! constructor
                explicit Section( const std::string& name, const std::string& parent = std::string() ):
                    _name( name ),
                    _parent( parent )
                {}

                //! clear
                void clear( void )
                { _content.clear(); }

                //! add to content
                void add( const std::string& content )
                {
                    if( !content.empty() )
                    _content.push_back( content );
                }

                //! contents list shortcut
                typedef std::vector<std::string> ContentList;

                //! add to content
                void add( const ContentList& );

                //! equal operator. Based on name only
                bool operator == (const Section& other ) const
                { return other._name == _name; }

                //! equal operator. Based on name only
                bool operator == (const std::string& other ) const
                { return other == _name; }

                //! name
                std::string _name;

                //! parent
                std::string _parent;

                //! content
                ContentList _content;

                //! used to find section with matching name
                class SameNameFTor
                {
                    public:

                    //! constructor
                    SameNameFTor( const Section& section ):
                        _name( section._name )
                        {}

                    //! predicate
                    bool operator() ( const Section& other ) const
                    { return _name == other._name; }

                    private:

                    //! prediction
                    std::string _name;

                };


            };

            private:

            //! root section name
            static const std::string _headerSectionName;
            static const std::string _rootSectionName;
            static const std::string _defaultSectionName;

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

}
#endif
