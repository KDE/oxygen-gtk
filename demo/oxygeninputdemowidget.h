#ifndef oxygeninputdemowidget_h
#define oxygeninputdemowidget_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
