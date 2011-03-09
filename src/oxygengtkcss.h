#ifndef oxygengtkrc_h
#define oxygengtkrc_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include <gtk/gtk.h>

#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

namespace Oxygen
{
    namespace Gtk
    {

        //! convenience class to generate GtkCSS option
        template< typename T>
            class CSSOption
        {
            public:

            //! constructor
            CSSOption( std::string name, const T& value )
            {
                std::ostringstream out;
                out << "  "<< name << ": " << value << ";";
                _value = out.str();
            }

            //! cast to cairo_t
            operator const std::string& (void) const
            { return _value; }

            private:

            std::string _value;

        };

        //! handle Gtk CSS style sheet generation
        class CSS
        {

            public:

            //! constructor
            explicit CSS( void )
            { init(); }

            //! destructor
            virtual ~CSS( void )
            {}

            //! clear
            void clear( void )
            {
                _sections.clear();
                init();
            }

            //! returns true if empty
            bool empty( void ) const
            { return _sections.empty(); }

            //! commit to gtk and clear
            void commit( GtkCssProvider* );

            //! merge
            void merge( const CSS& );

            //! create new section and set as current
            void addSection( const std::string& name );

            //! set current section
            void setCurrentSection( const std::string& name );

            //! add line to section
            void addToSection( const std::string& name, const std::string& content );

            //! add line to current section
            void addToCurrentSection( const std::string& content )
            { addToSection( _currentSection, content ); }

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

            protected:

            //! initialize default sections
            void init( void )
            { addSection( _defaultSectionName ); }

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
                explicit Section( const std::string& name ):
                    _name( name )
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

            //! default section
            static const std::string _defaultSectionName;

            //! list of sections
            Section::List _sections;

            //! current section
            std::string _currentSection;

            //! streamer
            friend std::ostream& operator << ( std::ostream&, const Section& );

            //! streamer
            friend std::ostream& operator << ( std::ostream&, const CSS& );

        };
    }

}
#endif
