#ifndef oxygengtkicons_h
#define oxygengtkicons_h
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

#include "oxygengtkrc.h"
#include "oxygenpathlist.h"

#include <string>
#include <map>
#include <vector>

#include <gtk/gtk.h>

namespace Oxygen
{

    //! generate translations between kde icon names and gtk icons
    class GtkIcons
    {
        public:

        //! constructor
        GtkIcons( void );

        //! destructor
        virtual ~GtkIcons( void );

        //! load translations
        void loadTranslations( const std::string& filename );

        //! generate rc file
        Gtk::RC generate( const PathList& );

        //! set icon size
        void setIconSize( const std::string&, unsigned int value );

        //! true if dirty
        bool isDirty( void ) const
        { return _dirty; }

        protected:

        //! generate iconSet for given option
        GtkIconSet* generate( const std::string& gtkIconName, const std::string& kdeIconName, const PathList& pathList ) const;

        //! generate rc code for given option
        std::string generateString( const std::string& gtkIconName, const std::string& kdeIconName, const PathList& pathList ) const;

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

        //!@name local storage, to prevent unnecessary reloading
        //@{

        //! local translations filename
        std::string _filename;

        //! local path list
        PathList _pathList;

        //! icon factory
        GtkIconFactory* _factory;

        //! local GtkRC
        Gtk::RC _rc;

        //! dirty flag. Set to true when options needs update
        bool _dirty;

        //@}

    };


}

#endif
