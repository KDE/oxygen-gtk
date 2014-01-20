#ifndef oxygeninputdemowidget_h
#define oxygeninputdemowidget_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include <vector>

namespace Oxygen
{

    class InputDemoWidget: public DemoWidget
    {

        public:

        //! constructor
        InputDemoWidget( void );

        //! destructor
        virtual ~InputDemoWidget( void );

        protected:

        //! wrap mode changed
        static void flatModeChanged( GtkToggleButton*, gpointer );

        //! wrap mode changed
        static void wrapModeChanged( GtkToggleButton*, gpointer );

        private:

        //! list of gtk entries
        typedef std::vector<GtkWidget*> WidgetList;
        WidgetList _entries;

        //! text editor
        GtkWidget* _textView;

    };

}

#endif
