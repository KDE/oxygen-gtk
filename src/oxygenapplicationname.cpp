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

#include "oxygenapplicationname.h"
#include "oxygengtkutils.h"
#include "config.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace Oxygen
{

    //__________________________________________________________________________
    void ApplicationName::initialize( void )
    {

        // get application name from gtk
        const std::string gtkAppName( fromGtk() );

        // get application name from pid
        const std::string pidAppName( fromPid( getpid() ) );

        #if OXYGEN_DEBUG
        std::cerr << "ApplicationName::initialize -"
            << " from pid: " << pidAppName
            << " from gtk: " << gtkAppName
            << std::endl;
        #endif

        if( pidAppName == "opera" ) _name = Opera;
        else if( pidAppName == "java" ) {

            if( !gtkAppName.empty() ) _name = JavaSwt;
            else _name = Java;

        } else if( gtkAppName == "acroread" ) _name = Acrobat;
        else if( gtkAppName.find("firefox") == 0 ) _name = Firefox;
        else if( gtkAppName.find("xulrunner") == 0 ) _name = Xul;
        else if( gtkAppName.find("thunderbird") == 0 ) _name = Thunderbird;
        else if( gtkAppName.find("seamonkey" ) == 0 ) _name = Seamonkey;
        else if( gtkAppName == "soffice" ) _name = OpenOffice;
        else if( gtkAppName == "gimp" ) _name = Gimp;
        else if(
            gtkAppName == "chromium" ||
            gtkAppName == "chromium-browser" ||
            gtkAppName == "google-chrome" ) _name = GoogleChrome;
        else if( gtkAppName == "eclipse" || gtkAppName == "Eclipse" ) _name = Eclipse;
        else _name = Unknown;

    }

    //__________________________________________________________________________
    bool ApplicationName::isMozilla( GtkWidget* widget ) const
    {
        if( !isMozilla() ) return false;

        GtkWidget* parent( gtk_widget_get_toplevel( widget ) );

        // check parent
        if( parent && GTK_IS_DIALOG( parent ) ) return false;
        else return true;

    }

    //__________________________________________________________________________
    bool ApplicationName::isAcrobat( GtkWidget* widget ) const
    {
        if( !isAcrobat() ) return false;

        GtkWidget* parent( gtk_widget_get_toplevel( widget ) );

        // check parent
        if( parent && GTK_IS_DIALOG( parent ) ) return false;
        else return true;

    }
    //__________________________________________________________________________
    bool ApplicationName::isJavaSwt( GtkWidget* widget ) const
    {
        if( !isJavaSwt() ) return false;

        GtkWidget* parent( gtk_widget_get_toplevel( widget ) );

        // check parent
        if( parent && GTK_IS_DIALOG( parent ) ) return false;
        else return true;

    }

    //__________________________________________________________________________
    bool ApplicationName::useFlatBackground( GtkWidget* widget ) const
    {

        // first check parent
        GtkWidget* parent( gtk_widget_get_toplevel( widget ) );

        // check parent
        if( parent && GTK_IS_DIALOG( parent ) ) return false;
        else return
            isMozilla() ||
            isAcrobat() ||
            isJavaSwt() ||
            isOpenOffice() ||
            isEclipse();

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

        /*
        somehow std::getline gets some extra crap (non char) from the procfile
        one has to use ifstream::getline, and pass it a fixed size line
        */
        char lineC[1024];
        in.getline( lineC, 1024, '\n' );
        std::string line( lineC );

        // get position of last "/" character, and truncate accordingly
        const size_t pos = line.rfind( '/' );
        if( pos == std::string::npos ) return line;
        else return line.substr( pos+1 );

    }

}
