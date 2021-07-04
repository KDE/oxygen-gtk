#ifndef oxygenbuttondemowidget_h
#define oxygenbuttondemowidget_h

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

namespace Oxygen
{

    class ButtonDemoWidget: public DemoWidget
    {

        public:

        //! constructor
        ButtonDemoWidget( void );

        //! destructor
        virtual ~ButtonDemoWidget( void );

        protected:

        //! toolbar styla changed
        static void toolBarStyleChanged( GtkComboBox*, gpointer );

        //! icon size changed
        static void iconSizeChanged( GtkComboBox*, gpointer );

        private:

        //! toolbar
        GtkWidget* _toolbar;

    };

}

#endif
