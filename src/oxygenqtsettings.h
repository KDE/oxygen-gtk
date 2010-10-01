#ifndef oxygenqtsettings_h
#define oxygenqtsettings_h
/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
#include "oxygengtkrc.h"
#include "oxygenoption.h"
#include "oxygenoptionmap.h"
#include "oxygenpalette.h"

#include <gtk/gtk.h>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Oxygen
{

    class QtSettings
    {

        public:

        //! constructor
        QtSettings( void );

        //! destructor
        virtual ~QtSettings( void )
        {}

        //! initialize
        void init( void );

        //! palette
        const Palette& palette( void ) const
        { return _palette; }

        //! application name
        const ApplicationName& applicationName( void ) const
        { return _applicationName; }

        //! checkbox style
        enum CheckBoxStyle
        {
           CS_CHECK,
           CS_X
        };

        //! checkbox style
        CheckBoxStyle checkBoxStyle( void ) const
        { return _checkBoxStyle; }

        protected:

        // get home directory
        std::string home( void ) const;

        // get kde home
        std::string kdeHome( void ) const;

        // get kde icon prefix
        std::string kdeIconPrefix( void ) const;

        //! icon path
        std::vector<std::string> kdeIconPath( void ) const;

        //! init application name
        void initApplicationName( void );

        //! load kde icons
        void loadKdeIcons( void );

        //! load palette from kdeglobals
        void loadKdePalette( void );

        //! load fonts from kdeglobals and pass to gtk
        void loadKdeFonts( void );

        //! update gtk colors
        /*! generates an RC string and pass it to gtk */
        void generateGtkColors( void );

        // sanitize path
        std::string sanitizePath( const std::string& ) const;

        // read all options from kdeglobals
        OptionMap readOptions( const std::string& ) const;

        private:

        //! application
        /*! this is needed to deal with some application hacks */
        ApplicationName _applicationName;

        //! kde global options
        OptionMap _kdeGlobals;

        //! kde oxygen options
        OptionMap _oxygen;

        //! kde home path
        std::string _kdeHome;

        //!@name icons
        //@{

        //! icon themes
        std::string _kdeIconTheme;
        std::string _kdeFallbackIconTheme;

        //! icon prefix
        std::string _kdeIconPrefix;

        //! default icon prefix
        static const std::string _defaultKdeIconPrefix;

        //! icon path
        std::vector<std::string> _kdeIconPath;

        //@}

        //! palette
        Palette _palette;

        //! checkbox style
        CheckBoxStyle _checkBoxStyle;

        //! rc options (passed to gtk at the end of init
        Gtk::RC _rc;

    };

}

#endif
