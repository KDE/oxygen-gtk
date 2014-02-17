/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenapplicationname.h"
#include "oxygengtkutils.h"
#include "config.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace Oxygen
{

    //__________________________________________________________________________
    void ApplicationName::initialize( void )
    {

        // get application name from gtk
        std::string gtkAppName( fromGtk() );

        // get application name from pid
        std::string pidAppName( fromPid( getpid() ) );

        #if OXYGEN_DEBUG
        std::cerr << "ApplicationName::initialize -"
            << " from pid: " << pidAppName
            << " from gtk: " << gtkAppName
            << std::endl;
        #endif

        // initialize to unknown
        _name = Unknown;

        // Way to override appname detection
        const char* envAppName(getenv("OXYGEN_APPLICATION_NAME_OVERRIDE"));
        if(envAppName)
        {
            gtkAppName=envAppName;
            pidAppName=envAppName;
        }

        // tag all mozilla-like applications (XUL)
        static const std::string XulAppNames[] =
        {
            "firefox",
            "thunderbird",
            "seamonkey",
            "iceweasel",
            "icecat",
            "icedove",
            "xulrunner",
            "komodo",
            "aurora",
            "zotero",
            ""
        };

        for( unsigned int index = 0; !XulAppNames[index].empty(); ++index )
        {
            if( gtkAppName.find( XulAppNames[index] ) == 0 || pidAppName.find( XulAppNames[index] ) == 0 )
            {
                _name = XUL;
                break;
            }
        }

        #if OXYGEN_DEBUG
        std::cerr << "ApplicationName::initialize -"
            << " from pid: " << pidAppName
            << " from gtk: " << gtkAppName
            << " internal: " << *this
            << std::endl;
        #endif


    }

    //__________________________________________________________________________
    bool ApplicationName::isGtkDialogWidget( GtkWidget* widget ) const
    {
        GtkWidget* parent( gtk_widget_get_toplevel( widget ) );

        // check parent
        return parent && GTK_IS_DIALOG( parent );
    }

    //__________________________________________________________________________
    bool ApplicationName::useFlatBackground( GtkWidget* widget ) const
    {

        // check application name
        if( !isXul() ) return false;

        // check for Gtk dialog type
        if( widget && isGtkDialogWidget( widget ) ) return false;

        // return true in all other cases
        return true;

    }

    //__________________________________________________________________________
    std::string ApplicationName::fromGtk( void ) const
    {
        if( const char* gtkAppName = g_get_prgname() ) return gtkAppName;
        else return "";
    }

    //__________________________________________________________________________
    std::string ApplicationName::fromPid( int pid ) const
    {

        // generate /proc filename
        std::ostringstream filename;
        filename << "/proc/" << pid << "/cmdline";

        // try read file
        std::ifstream in( filename.str().c_str() );
        if( !in ) return std::string();
        std::string line;
        std::getline( in, line, '\0' );
        const size_t pos( line.rfind( '/' ) );
        return ( pos == std::string::npos ) ? line:line.substr( pos+1 );

    }

    //__________________________________________________________________________
    std::ostream& operator << ( std::ostream& out, const ApplicationName& app )
    {
        switch( app._name )
        {
            default:
            case Unknown: out << "Unknown"; break;
            case XUL: out << "XUL (Mozilla)"; break;
        }

        return out;
    }
}
