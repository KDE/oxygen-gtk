#ifndef oxygenanimations_h
#define oxygenanimations_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include "oxygenbaseengine.h"
#include "oxygencomboboxengine.h"

#include <gtk/gtkwidget.h>
#include <vector>
#include <map>

namespace Oxygen
{

    class Animations
    {

        public:

        //! singleton
        static Animations& instance( void );

        //! destructor
        virtual ~Animations( void );

        //! unregister widget
        void registerWidget( GtkWidget* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! combobox engine
        ComboBoxEngine& comboBoxEngine( void ) const
        { return *_comboBoxEngine; }

        protected:

        //! register new engine
        void registerEngine( BaseEngine* engine )
        { _engines.push_back( engine ); }

        //! destruction callback
        static void destroyRegisteredWidget( GtkWidget* widget, GdkEvent* event, gpointer data );

        private:

        //! constructor
        Animations( void );

        //! list of engines
        BaseEngine::List _engines;

        //! combobox engine
        ComboBoxEngine* _comboBoxEngine;

        //! keep track of destruction and style change signals
        /*!
        this is needed so that signals are disconnected when either
        the style change or the widget is destroyed
        */
        class WidgetData
        {

            public:

            //! constructor
            WidgetData( void ):
                _destroyId(-1),
                _styleChangeId(-1)
            {}

            int _destroyId;
            int _styleChangeId;
        };

        //! keep track of all registered widgets, and associated destroy callback
        typedef std::map< GtkWidget*, WidgetData > WidgetMap;
        WidgetMap _allWidgets;

    };

}

#endif
