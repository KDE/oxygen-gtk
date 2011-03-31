#ifndef oxygentabdemowidget_h
#define oxygentabdemowidget_h

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

#include "oxygendemowidget.h"
#include "oxygensignal.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    class TabDemoWidget: public DemoWidget
    {

        public:

        //! constructor
        TabDemoWidget( void );

        //! destructor
        virtual ~TabDemoWidget( void );

        protected:

        //! add page to notebook
        void addPage( const std::string&, GtkWidget* ) const;

        //! orientation changed
        static void orientationChanged( GtkComboBox*, gpointer );

        private:

        //! tab widget
        GtkWidget *_notebook;

        //! selection change
        Signal _changedId;

    };

}

#endif
