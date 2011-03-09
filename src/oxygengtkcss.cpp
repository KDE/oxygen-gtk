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

#include "oxygengtkcss.h"

#include <gtk/gtk.h>
#include <algorithm>
#include <cassert>

namespace Oxygen
{

    //_________________________________________________
    const std::string Gtk::CSS::_defaultSectionName = "*";

    //_________________________________________________
    void Gtk::CSS::commit( GtkCssProvider* provider )
    {

        // do nothing if empty
        if( empty() ) return;

        assert( provider );
        GError *error( 0L );
        const std::string contents( toString() );
        gtk_css_provider_load_from_data( provider, contents.c_str(), contents.size(), &error );
        if( error )
        {
            std::cerr << "Oxygen::CSS::commit - error reported while parsing: " << std::endl;
            std::cerr << error->message << std::endl;
            g_error_free( error );
        }

        clear();
    }

    //_________________________________________________
    void Gtk::CSS::merge( const Gtk::CSS& other )
    {

        // loop over sections in other
        for( Section::List::const_iterator iter = other._sections.begin(); iter != other._sections.end(); ++iter )
        {
            Section::List::iterator sectionIter = std::find_if( _sections.begin(), _sections.end(), Section::SameNameFTor( *iter ) );
            if( sectionIter == _sections.end() ) _sections.push_back( *iter );
            else {

                sectionIter->add( iter->_content );

            }

        }

        return;
    }

    //_________________________________________________
    void Gtk::CSS::addSection( const std::string& name )
    {

        if( std::find( _sections.begin(), _sections.end(), name ) != _sections.end() )
        {

            std::cerr << "Gtk::CSS::addSection - section named " << name << " already exists" << std::endl;

        } else {

            _sections.push_back( Section( name ) );

        }

        setCurrentSection( name );

    }

    //_________________________________________________
    void Gtk::CSS::addToSection( const std::string& name, const std::string& content )
    {
        Section::List::iterator iter( std::find( _sections.begin(), _sections.end(), name ) );
        if( iter == _sections.end() )
        {
            std::cerr << "Gtk::CSS::addToSection - unable to find section named " << name << std::endl;
            return;
        }

        iter->add( content );
    }

    //_________________________________________________
    void Gtk::CSS::setCurrentSection( const std::string& name )
    {

        if( std::find( _sections.begin(), _sections.end(), name ) == _sections.end() )
        {

            std::cerr << "Gtk::CSS::setCurrentSection - unable to find section named " << name << std::endl;
            return;

        } else {

            _currentSection = name;

        }
    }

    //_________________________________________________
    void Gtk::CSS::Section::add( const Gtk::CSS::Section::ContentList& content )
    {
        for( ContentList::const_iterator iter = content.begin(); iter != content.end(); ++iter )
        {
            if( std::find( _content.begin(), _content.end(), *iter ) == _content.end() )
            { _content.push_back( *iter ); }
        }
    }

    namespace Gtk
    {
        //_______________________________________________________________________
        std::ostream& operator << (std::ostream& out, const CSS::Section& section )
        {

            out << section._name << " {";
            out << std::endl;

            // add contents
            for( CSS::Section::ContentList::const_iterator iter = section._content.begin(); iter != section._content.end(); ++iter )
            { out << *iter << std::endl; }

            out << "}" << std::endl;
            return out;

        }

        //_______________________________________________________________________
        std::ostream& operator << (std::ostream& out, const CSS& rc )
        {

            // dump all sections
            for( CSS::Section::List::const_iterator iter = rc._sections.begin(); iter != rc._sections.end(); ++iter )
            { out << *iter << std::endl; }

            return out;
        }

    }
}
