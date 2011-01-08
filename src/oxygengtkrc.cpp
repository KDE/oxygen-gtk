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

#include "oxygengtkrc.h"

#include <gtk/gtk.h>
#include <algorithm>
#include <cassert>

namespace Oxygen
{

    //_________________________________________________
    const std::string Gtk::RC::_headerSectionName = "__head__";
    const std::string Gtk::RC::_rootSectionName = "__root__";
    const std::string Gtk::RC::_defaultSectionName = "oxygen-default-internal";

    //_________________________________________________
    void Gtk::RC::commit( void )
    {
        gtk_rc_parse_string( toString().c_str() );
        clear();
    }

    //_________________________________________________
    void Gtk::RC::merge( const Gtk::RC& other )
    {

        // loop over sections in other
        for( Section::List::const_iterator iter = other._sections.begin(); iter != other._sections.end(); ++iter )
        {
            Section::List::iterator sectionIter = std::find_if( _sections.begin(), _sections.end(), Section::SameNameFTor( *iter ) );
            if( sectionIter == _sections.end() ) _sections.push_back( *iter );
            else {

                assert( sectionIter->_parent == iter->_parent );
                sectionIter->add( iter->_content );

            }

        }

        return;
    }

    //_________________________________________________
    void Gtk::RC::addSection( const std::string& name, const std::string& parent )
    {
        if( std::find( _sections.begin(), _sections.end(), name ) != _sections.end() )
        {

            std::cerr << "Gtk::RC::addSection - section named " << name << " already exists" << std::endl;

        } else {

            _sections.push_back( Section( name, parent ) );

        }

        setCurrentSection( name );

    }

    //_________________________________________________
    void Gtk::RC::addToSection( const std::string& name, const std::string& content )
    {
        Section::List::iterator iter( std::find( _sections.begin(), _sections.end(), name ) );
        if( iter == _sections.end() )
        {
            std::cerr << "Gtk::RC::addToSection - unable to find section named " << name << std::endl;
            return;
        }

        iter->add( content );
    }

    //_________________________________________________
    void Gtk::RC::setCurrentSection( const std::string& name )
    {
        if( std::find( _sections.begin(), _sections.end(), name ) == _sections.end() )
        {

            std::cerr << "Gtk::RC::setCurrentSection - unable to find section named " << name << std::endl;
            return;

        } else {

            _currentSection = name;

        }
    }

    //_________________________________________________
    void Gtk::RC::Section::add( const Gtk::RC::Section::ContentList& content )
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
        std::ostream& operator << (std::ostream& out, const RC::Section& section )
        {
            if( section._name == RC::_rootSectionName || section._name == RC::_headerSectionName )
            {

                // add contents
                for( RC::Section::ContentList::const_iterator iter = section._content.begin(); iter != section._content.end(); ++iter )
                { out << *iter << std::endl; }

            } else {

                out << "style \"" << section._name << "\"";
                if( !section._parent.empty() ) out << " = \"" << section._parent << "\"";
                out << std::endl;
                out << "{" << std::endl;

                // add contents
                for( RC::Section::ContentList::const_iterator iter = section._content.begin(); iter != section._content.end(); ++iter )
                { out << *iter << std::endl; }

                out << "}" << std::endl;

            }

            return out;
        }

        //_______________________________________________________________________
        std::ostream& operator << (std::ostream& out, const RC& rc )
        {
            // dump header section
            RC::Section::List::const_iterator iter( std::find( rc._sections.begin(), rc._sections.end(), RC::_headerSectionName ) );
            assert( iter != rc._sections.end() );
            out << *iter << std::endl;

            // dump all section except root
            for( RC::Section::List::const_iterator iter = rc._sections.begin(); iter != rc._sections.end(); ++iter )
            { if( !(*iter == RC::_rootSectionName || *iter == RC::_headerSectionName ) ) out << *iter << std::endl; }

            // dump root section
            iter = std::find( rc._sections.begin(), rc._sections.end(), RC::_rootSectionName );
            assert( iter != rc._sections.end() );
            out << *iter << std::endl;

            return out;
        }

    }
}
