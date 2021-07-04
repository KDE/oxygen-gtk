#ifndef oxygentabdemowidget_h
#define oxygentabdemowidget_h

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

    };

}

#endif
