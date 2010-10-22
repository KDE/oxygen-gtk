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

#include <algorithm>
#include <cassert>

namespace Gtk
{

    //_________________________________________________
    const std::string RC::_headerSectionName = "__head__";
    const std::string RC::_rootSectionName = "__root__";
    const std::string RC::_defaultSectionName = "oxygen-default-internal";

    //_________________________________________________
    void RC::addSection( const std::string& name, const std::string& parent )
    {
        if( std::find( _sections.begin(), _sections.end(), name ) != _sections.end() )
        {

            std::cout << "Gtk::RC::addSection - section named " << name << " already exists" << std::endl;

        } else {

            _sections.push_back( Section( name, parent ) );

        }

        setCurrentSection( name );

    }

    //_________________________________________________
    void RC::addToSection( const std::string& name, const std::string& content )
    {
        Section::List::iterator iter( std::find( _sections.begin(), _sections.end(), name ) );
        if( iter == _sections.end() )
        {
            std::cout << "Gtk::RC::addToSection - unable to find section named " << name << std::endl;
            return;
        }

        iter->add( content );
    }

    //_________________________________________________
    void RC::setCurrentSection( const std::string& name )
    {
        if( std::find( _sections.begin(), _sections.end(), name ) == _sections.end() )
        {

            std::cout << "Gtk::RC::setCurrentSection - unable to find section named " << name << std::endl;
            return;

        } else {

            _currentSection = name;

        }
    }

    //_______________________________________________________________________
    std::ostream& operator << (std::ostream& out, const RC::Section& section )
    {
        if( section._name == RC::_rootSectionName || section._name == RC::_headerSectionName )
        {

            out << section._content;

        } else {

            out << "style \"" << section._name << "\"";
            if( !section._parent.empty() ) out << " = \"" << section._parent << "\"";
            out << std::endl;
            out << "{" << std::endl;
            out << section._content;
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
