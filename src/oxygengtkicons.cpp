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

#include "oxygengtkicons.h"
#include "config.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <gtk/gtk.h>

namespace Oxygen
{

    //_________________________________________
    GtkIcons::GtkIcons( void ):
        _factory( 0L ),
        _dirty( true )
    {

        // initialize sizes
        _sizes.push_back( std::make_pair( "panel-menu", 16 ) );
        _sizes.push_back( std::make_pair( "panel", 32 ) );
        _sizes.push_back( std::make_pair( "gtk-small-toolbar", 22 ) );
        _sizes.push_back( std::make_pair( "gtk-large-toolbar", 22 ) );
        _sizes.push_back( std::make_pair( "gtk-dnd", 48 ) );
        _sizes.push_back( std::make_pair( "gtk-button", 16 ) );
        _sizes.push_back( std::make_pair( "gtk-menu", 16 ) );
        _sizes.push_back( std::make_pair( "gtk-dialog", 32 ) );
        _sizes.push_back( std::make_pair( "", 16 ) );

    }

    //_________________________________________
    GtkIcons::~GtkIcons( void )
    {
        if( _factory )
        { gtk_icon_factory_remove_default( _factory ); }
    }

    //_________________________________________
    void GtkIcons::setIconSize( const std::string& tag, unsigned int value )
    {
        SizeMap::iterator iter( std::find_if( _sizes.begin(), _sizes.end(), SameTagFTor( tag ) ) );
        if( iter == _sizes.end() ) {

            std::cerr << "GtkIcons::setIconSize - no match for" << tag << "," << value << std::endl;

        } else if( iter->second != value ) {

            iter->second = value;
            _dirty = true;

        }

    }

    //_________________________________________
    void GtkIcons::loadTranslations( const std::string& filename )
    {

        if( filename == _filename )
        { return; }

        _filename = filename;
        _dirty = true;
        _icons.clear();

        std::ifstream in( filename.c_str() );
        if( !in )
        {
            std::cerr << "Oxygen::GtkIcons::loadTranslations - could not open " << filename << std::endl;
            return;
        }

        std::string line;
        while( std::getline( in, line, '\n' ) )
        {

            if( line.empty() ) continue;

            IconPair iconPair;
            std::istringstream stream( line.c_str() );
            stream >> iconPair.first >> iconPair.second;
            if( ( stream.rdstate() & std::ios::failbit ) != 0 )
            { continue; }

            _icons.insert( iconPair );

        }

    }

    //_________________________________________
    Gtk::RC GtkIcons::generate( const PathList& pathList )
    {

        if( (!_dirty) && _pathList == pathList ) return _rc;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::GtkIcons::generate - regenerating translation tables" << std::endl;
        #endif

        _pathList = pathList;
        _rc.clear();

        // reset factory
        if( _factory )
        {
            gtk_icon_factory_remove_default( _factory );
            g_object_unref( G_OBJECT( _factory ) );
        }

        // create new
        _factory = gtk_icon_factory_new();

        // generate icon size string
        std::ostringstream iconSizesStr;
        for( SizeMap::const_iterator iter = _sizes.begin(); iter != _sizes.end(); ++iter )
        {
            if( iter->first.empty() ) continue;
            if( iter != _sizes.begin() ) iconSizesStr << ": ";
            iconSizesStr << iter->first << " = " << iter->second << "," << iter->second;
        }

        // pass to settings
        GtkSettings* settings( gtk_settings_get_default() );
        gtk_settings_set_string_property( settings, "gtk-icon-sizes", iconSizesStr.str().c_str(), "oxygen-gtk" );

        // generate pixmap path
        // this must be passed to gtk before any icon settings, otherwise
        // other icons are not recognized
        std::ostringstream pixmapPathStr;
        pixmapPathStr << "pixmap_path \"";
        for( PathList::const_iterator iter = pathList.begin(); iter != pathList.end(); ++iter )
        {
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::GtkIcons::generate - adding path: " << *iter << std::endl;
            #endif

            if( iter != pathList.begin() ) pixmapPathStr << ":";
            pixmapPathStr << *iter;
        }
        pixmapPathStr << "\"";
        _rc.addToHeaderSection( pixmapPathStr.str() );

        // loop over icons
        bool empty( true );
        for( IconMap::const_iterator iconIter = _icons.begin(); iconIter != _icons.end(); ++iconIter )
        {

            GtkIconSet* iconSet( generate( iconIter->first, iconIter->second, pathList ) );
            if( iconSet )
            {
                gtk_icon_factory_add( _factory, iconIter->first.c_str(), iconSet );
                gtk_icon_set_unref( iconSet );
                empty = false;
            }

        }

        if( empty )
        {

            g_object_unref( G_OBJECT( _factory ) );
            _factory = 0L;

        } else gtk_icon_factory_add_default( _factory );

        // extra settings for entries
        std::string stock( generateString( "gtk-clear", "actions/edit-clear-locationbar-rtl.png", pathList ) );
        if( !stock.empty() )
        {
            _rc.addSection( "oxygen-icons-editor", Gtk::RC::defaultSection() );
            _rc.addToCurrentSection( stock );
            _rc.addToRootSection( "class \"*Entry*\" style \"oxygen-icons-editor\"" );
        }

        _dirty = false;
        return _rc;

    }

    //__________________________________________________________________
    GtkIconSet* GtkIcons::generate(
        const std::string& gtkIconName,
        const std::string& kdeIconName,
        const PathList& pathList ) const
    {


        if( kdeIconName == "NONE" ) return 0L;

        bool empty( true );

        // create iconSet
        GtkIconSet* iconSet = gtk_icon_set_new();

        // loop over iconSizes
        for( SizeMap::const_iterator sizeIter = _sizes.begin(); sizeIter != _sizes.end(); ++sizeIter )
        {

            // generate full icon name
            std::ostringstream iconFileStream;
            iconFileStream << sizeIter->second << "x" << sizeIter->second << "/" << kdeIconName;

            // loop over provided path to see if at least one icon is found
            for( PathList::const_iterator pathIter = pathList.begin(); pathIter != pathList.end(); ++pathIter )
            {
                std::string filename( *pathIter + '/' + iconFileStream.str() );
                if( !std::ifstream( filename.c_str() ) ) continue;

                empty = false;
                GtkIconSource* iconSource( gtk_icon_source_new() );

                // set name
                gtk_icon_source_set_filename( iconSource, filename.c_str() );

                // set direction and state wildcarded
                gtk_icon_source_set_direction_wildcarded( iconSource, TRUE );
                gtk_icon_source_set_state_wildcarded( iconSource, TRUE );

                // set size
                if( sizeIter->first.empty() ) gtk_icon_source_set_size_wildcarded( iconSource, TRUE );
                else {

                    GtkIconSize size = gtk_icon_size_from_name( sizeIter->first.c_str() );
                    if (size != GTK_ICON_SIZE_INVALID)
                    {
                        gtk_icon_source_set_size_wildcarded( iconSource, FALSE );
                        gtk_icon_source_set_size( iconSource, size );
                    }
                }

                // add source to iconSet
                gtk_icon_set_add_source( iconSet, iconSource );
                break;

            }

        }

        // if nothing found, return;
        if( empty )
        {

            gtk_icon_set_unref( iconSet );
            return 0L;

        } else return iconSet;

    }

    //__________________________________________________________________
    std::string GtkIcons::generateString(
        const std::string& gtkIconName,
        const std::string& kdeIconName,
        const PathList& pathList ) const
    {


        if( kdeIconName == "NONE" ) return std::string();

        bool empty( true );
        std::ostringstream stockOut;

        // new stock
        stockOut << "  stock[\"" << gtkIconName << "\"]={" << std::endl;

        // loop over iconSizes
        for( SizeMap::const_iterator sizeIter = _sizes.begin(); sizeIter != _sizes.end(); ++sizeIter )
        {

            // generate full icon name
            std::ostringstream iconFileStream;
            iconFileStream << sizeIter->second << "x" << sizeIter->second << "/" << kdeIconName;

            // loop over provided path to see if at least one icon is found
            bool found( false );
            for( PathList::const_iterator pathIter = pathList.begin(); pathIter != pathList.end(); ++pathIter )
            {
                std::string filename( *pathIter + '/' + iconFileStream.str() );
                if( !std::ifstream( filename.c_str() ) ) continue;
                found = true;
                break;
            }

            if( !found ) continue;
            empty = false;
            if( sizeIter->first.empty() ) stockOut << "    { \"" << iconFileStream.str() << "\" }" << std::endl;
            else stockOut << "    { \"" << iconFileStream.str() << "\", *, *, \"" << sizeIter->first << "\" }," << std::endl;


        }

        stockOut << "  }" << std::endl;

        return empty ? std::string() : stockOut.str();

    }



}
