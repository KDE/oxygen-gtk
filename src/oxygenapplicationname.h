#ifndef oxygenapplicationname_h
#define oxygenapplicationname_h
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

#include <gtk/gtk.h>
#include <string>

namespace Oxygen
{

    //! application name enumeration
    enum AppName
    {
        Unknown,
        Acrobat,
        Firefox,
        Seamonkey,
        Thunderbird,
        Xul,
        Gimp,
        OpenOffice,
        GoogleChrome
    };

    //! stores application name and provides some utility functions
    class ApplicationName
    {

        public:

        //! constructor
        ApplicationName( AppName name = Unknown ):
            _name( name )
        {}

        //! constructor
        ApplicationName( const std::string& appName ):
            _name( Unknown )
        { parse( appName ); }

        //! assignment
        ApplicationName& operator = ( const AppName& name )
        {
            _name = name;
            return *this;
        }

        //! cast
        operator const AppName& ( void ) const
        { return _name; }

        //! get type from string
        void parse( const std::string& );

        //!@name utilities
        //@{

        bool isAcrobat( void ) const { return _name == Acrobat; }
        bool isFirefox( void ) const { return _name == Firefox; }
        bool isThunderbird( void ) const { return _name == Thunderbird; }
        bool isSeamonkey( void ) const { return _name == Seamonkey; }
        bool isXul( void ) const { return _name == Xul; }
        bool isMozilla( void ) const { return isFirefox() || isThunderbird() || isSeamonkey() || isXul(); }
        bool isGimp( void ) const { return _name == Gimp; }
        bool isOpenOffice( void ) const { return _name == OpenOffice; }
        bool isGoogleChrome( void ) const { return _name == GoogleChrome; }
        bool isUnknown( void ) const { return _name == Unknown; }

        //! special case for mozilla and acrobat that also check the type of the top level widget
        /*! this allows to prevent false positive for open and print dialogs */
        bool isAcrobat( GtkWidget* ) const;
        bool isMozilla( GtkWidget* ) const;

        //@}

        private:

        //! name
        AppName _name;

    };


}

#endif
