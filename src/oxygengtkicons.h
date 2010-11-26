#ifndef oxygengtkicons_h
#define oxygengtkicons_h
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

#include "oxygengtkrc.h"

#include <string>
#include <map>
#include <vector>

namespace Oxygen
{

    //! generate translations between kde icon names and gtk icons
    class GtkIcons
    {
        public:

        //! constructor
        GtkIcons( void );

        //! destructor
        virtual ~GtkIcons( void )
        {}

        //! load translations
        void loadTranslations( const std::string& filename );

        //! generate rc file
        Gtk::RC generate( const std::vector<std::string>& pathList ) const;

        //! set icon size
        void setIconSize( const std::string&, int value );

        protected:

        //! generate rc code for given option
        std::string generate( const std::string& gtkIconName, const std::string& kdeIconName, const std::vector<std::string>& pathList ) const;

        private:

        //!@name icon map
        //@{
        typedef std::pair<std::string, std::string> IconPair;
        typedef std::map<std::string, std::string> IconMap;

        IconMap _icons;
        //@}

        //!@name icon sizes
        //@{
        typedef std::pair<std::string, unsigned int> SizePair;
        typedef std::vector<SizePair> SizeMap;
        SizeMap _sizes;

        //! used to find size pair matching given tag
        class SameTagFTor
        {
            public:

            //! constructor
            SameTagFTor( const std::string& tag ):
                _tag( tag )
                {}

            //! predicate
            bool operator() (const SizePair& pair ) const
            { return pair.first == _tag; }

            private:

            //! prediction
            std::string _tag;
        };

        //@}

    };


}

#endif
