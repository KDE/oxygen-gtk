#ifndef oxygenanimations_h
#define oxygenanimations_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenarrowstateengine.h"
#include "oxygenbackgroundhintengine.h"
#include "oxygenbaseengine.h"
#include "oxygencomboengine.h"
#include "oxygencomboboxengine.h"
#include "oxygencomboboxentryengine.h"
#include "oxygendialogengine.h"
#include "oxygenhook.h"
#include "oxygenhoverengine.h"
#include "oxygenmainwindowengine.h"
#include "oxygenmenushellengine.h"
#include "oxygenscrollbarengine.h"
#include "oxygenscrollbarstateengine.h"
#include "oxygenscrolledwindowengine.h"
#include "oxygensignal.h"
#include "oxygentabwidgetengine.h"
#include "oxygentabwidgetstateengine.h"
#include "oxygentreeviewengine.h"
#include "oxygenwidgetsizeengine.h"
#include "oxygenwidgetstateengine.h"

#include <gtk/gtk.h>
#include <vector>
#include <map>

namespace Oxygen
{

    class Animations
    {

        public:

        //! constructor
        Animations( void );

        //! destructor
        virtual ~Animations( void );

        //! initialize hooks
        void initializeHooks( void );

        //! unregister widget
        bool registerWidget( GtkWidget* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! enabled state
        void setEnabled( bool value );

        //! enabled state
        bool enabled( void ) const
        { return _enabled; }

        //! combo engine
        ComboEngine& comboEngine( void ) const
        { return *_comboEngine; }

        //! combobox engine
        ComboBoxEngine& comboBoxEngine( void ) const
        { return *_comboBoxEngine; }

        //! comboboxentry engine
        ComboBoxEntryEngine& comboBoxEntryEngine( void ) const
        { return *_comboBoxEntryEngine; }

        //! dialog engine
        DialogEngine& dialogEngine( void ) const
        { return *_dialogEngine; }

        //! background hint
        BackgroundHintEngine& backgroundHintEngine( void ) const
        { return *_backgroundHintEngine; }

        //! main window engine
        MainWindowEngine& mainWindowEngine( void ) const
        { return *_mainWindowEngine; }

        //! menushell engine
        MenuShellEngine& menuShellEngine( void ) const
        { return *_menuShellEngine; }

        //! hover engine
        HoverEngine& hoverEngine( void ) const
        { return *_hoverEngine; }

        //! scrollbar engine
        ScrollBarEngine& scrollBarEngine( void ) const
        { return *_scrollBarEngine; }

        //! scrolled window engine
        ScrolledWindowEngine& scrolledWindowEngine( void ) const
        { return *_scrolledWindowEngine; }

        //! tab widget engine
        TabWidgetEngine& tabWidgetEngine( void ) const
        { return *_tabWidgetEngine; }

        //! tree view engine
        TreeViewEngine& treeViewEngine( void ) const
        { return *_treeViewEngine; }

        //! window size engine
        WidgetSizeEngine& widgetSizeEngine( void ) const
        { return *_widgetSizeEngine; }

        //!@name animations specific engines
        //@{

        //! widget mouse-over and focus animations engine
        WidgetStateEngine& widgetStateEngine( void ) const
        { return *_widgetStateEngine; }

        //! arrow mouse-over animations engine
        ArrowStateEngine& arrowStateEngine( void ) const
        { return *_arrowStateEngine; }

        //! scrollbar arrow mouse-over animations engine
        ScrollBarStateEngine& scrollBarStateEngine( void ) const
        { return *_scrollBarStateEngine; }

        //! notebook tabs mouse-over animations engine
        TabWidgetStateEngine& tabWidgetStateEngine( void ) const
        { return *_tabWidgetStateEngine; }

        //@}

        protected:

        //! register new engine
        void registerEngine( BaseEngine* engine )
        { _engines.push_back( engine ); }

        //! destruction callback
        static gboolean destroyNotifyEvent( GtkWidget*, gpointer );

        //! destruction callback
        static void styleChangeNotifyEvent( GtkWidget*, GtkStyle*, gpointer );

        //! combobox list size adjustment hook
        static gboolean comboBoxHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        private:

        //! enabled state
        bool _enabled;

        //! list of engines
        BaseEngine::List _engines;

        //! background hint engine
        BackgroundHintEngine* _backgroundHintEngine;

        //! combobox engine
        ComboEngine* _comboEngine;

        //! combobox engine
        ComboBoxEngine* _comboBoxEngine;

        //! combobox entry engine
        ComboBoxEntryEngine* _comboBoxEntryEngine;

        //! dialog engine
        DialogEngine* _dialogEngine;

        //! hover engine
        HoverEngine* _hoverEngine;

        //! main window engine
        MainWindowEngine* _mainWindowEngine;

        //! menushell engine
        MenuShellEngine* _menuShellEngine;

        //! scrollbar engine
        ScrollBarEngine* _scrollBarEngine;

        //! scrolled window engine
        ScrolledWindowEngine* _scrolledWindowEngine;

        //! tab widget engine
        TabWidgetEngine* _tabWidgetEngine;

        //! tree view engine
        TreeViewEngine* _treeViewEngine;

        //! widget size engine
        WidgetSizeEngine* _widgetSizeEngine;

        //!@name Animation specific engines
        //@{

        //! widget mouse-over and focus animations engine
        WidgetStateEngine* _widgetStateEngine;

        //! arrow mouse-over animations engine
        ArrowStateEngine* _arrowStateEngine;

        //! scrollbar arrow mouse-over animations engine
        ScrollBarStateEngine* _scrollBarStateEngine;

        //! notebook tabs mouse-over animations engine
        TabWidgetStateEngine* _tabWidgetStateEngine;

        //@}

        //!@name hooks
        //@{

        //! true when hooks are initialized
        bool _hooksInitialized;

        //! combobox hook
        Hook _comboBoxHook;


        //@}

        //! keep track of destruction and style change signals
        /*!
        this is needed so that signals are disconnected when either
        the style change or the widget is destroyed
        */
        class WidgetData
        {

            public:

            //! constructor
            WidgetData( void )
            {}

            Signal _destroyId;
            Signal _styleChangeId;
        };

        //! keep track of all registered widgets, and associated destroy callback
        typedef std::map< GtkWidget*, WidgetData > WidgetMap;
        WidgetMap _allWidgets;

    };

}

#endif
