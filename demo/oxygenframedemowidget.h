#ifndef oxygenframedemowidget_h
#define oxygenframedemowidget_h

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
#include <map>

namespace Oxygen
{

    class FrameDemoWidget: public DemoWidget
    {

        public:

        //! constructor
        FrameDemoWidget( void );

        //! destructor
        virtual ~FrameDemoWidget( void );

        protected:

        //! orientation changed
        static void orientationChanged( GtkComboBox*, gpointer );

        //! shadow style changed
        static void shadowChanged( GtkToggleButton*, gpointer );

        private:

        //! frames box
        GtkWidget* _box;

        //! frame
        GtkWidget* _frame;

        //! map widget to frame shadow style
        typedef std::map<GtkWidget*, GtkShadowType> WidgetMap;
        WidgetMap _widgets;

    };

}

#endif
