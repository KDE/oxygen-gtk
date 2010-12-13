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
#include "config.h"

#include <iostream>

namespace Oxygen
{

    //__________________________________________________________________________
    void ApplicationName::parse( const std::string& appName )
    {

        #if OXYGEN_DEBUG
        std::cout << "ApplicationName::parse - " << appName << std::endl;
        #endif

        if( appName.find("firefox") == 0 ) _name = Firefox;
        else if( appName.find("thunderbird") == 0 ) _name = Thunderbird;
        else if( appName.find("seamonkey" ) == 0 ) _name = Seamonkey;
        else if( appName == "soffice" ) _name = OpenOffice;
        else if( appName == "gimp" ) _name = Gimp;
        else if( appName == "chromium" || appName == "chromium-browser" || appName == "google-chrome" ) _name = GoogleChrome;
        else _name = Unknown;
    }

}
