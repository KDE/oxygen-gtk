#ifndef oxygendemodialog_h
#define oxygendemodialog_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignalhandler.h"
#include "oxygensliderdemowidget.h"

#include <gtk/gtk.h>
#include <map>

namespace Oxygen
{

    // forward declaration
    class DemoWidget;

    // main demo dialog
    class DemoDialog: public SignalHandler
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

        //!@name callbacks
        //@{

        static void selectionChanged( GtkIconView*, gpointer );
        static void toggleEnableState( GtkToggleButton*, gpointer );
        static void toggleWidgetDirection( GtkToggleButton*, gpointer );
        static gboolean keyPress( GtkWidget*, GdkEventKey*, gpointer );
        //@}

        private:

        //! main widget
        GtkWidget* _mainWidget;

        //! tab widget
        GtkWidget* _notebook;

        //! model
        GtkListStore* _model;

        //! map page index and page widget
        typedef std::map<int, DemoWidget*> PageMap;
        PageMap _pages;
        SliderDemoWidget* _sliderDemoWidget;

        //! enable state button
        GtkWidget* _stateButton;

    };

}

#endif
