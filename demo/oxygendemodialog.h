#ifndef oxygendemodialog_h
#define oxygendemodialog_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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
#include <map>

namespace Oxygen
{

    // forward declaration
    class DemoWidget;

    // main demo dialog
    class DemoDialog
    {

        public:

        //! constructor
        DemoDialog( void );

        //! destructor
        virtual ~DemoDialog( void );

         //! main widget
        virtual GtkWidget* mainWidget( void )
        { return _mainWidget; }

        protected:

        //! add page
        void addPage( DemoWidget* );

        private:

        //! main widget
        GtkWidget* _mainWidget;

        //! tab widget
        GtkWidget* _notebook;

        //! map page index and page widget
        std::map<int, DemoWidget*> _pages;

    };

}

#endif
