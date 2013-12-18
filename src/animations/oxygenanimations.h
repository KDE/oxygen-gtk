#ifndef oxygenanimations_h
#define oxygenanimations_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "oxygenflatwidgetengine.h"
#include "oxygengroupboxengine.h"
#include "oxygengroupboxlabelengine.h"
#include "oxygenhook.h"
#include "oxygenhoverengine.h"
#include "oxygenmainwindowengine.h"
#include "oxygenmenubarstateengine.h"
#include "oxygenmenuitemengine.h"
#include "oxygenmenustateengine.h"
#include "oxygenpanedengine.h"
#include "oxygenscrollbarengine.h"
#include "oxygenscrollbarstateengine.h"
#include "oxygenscrolledwindowengine.h"
#include "oxygeninnershadowengine.h"
#include "oxygensignal.h"
#include "oxygentabwidgetengine.h"
#include "oxygentabwidgetstateengine.h"
#include "oxygentoolbarstateengine.h"
#include "oxygentreeviewengine.h"
#include "oxygentreeviewstateengine.h"
#include "oxygenwidgetsizeengine.h"
#include "oxygenwidgetstateengine.h"

#include <gtk/gtk.h>
#include <vector>
#include <map>

namespace Oxygen
{

    //! forward declaration
    class QtSettings;

    //! keeps track of all animation engines
    class Animations
    {

        public:

        //! constructor
        Animations( void );

        //! destructor
        virtual ~Animations( void );

        //! initialize settings
        void initialize( const QtSettings& );

        //! initialize hooks
        void initializeHooks( void );

        //! unregister widget
        bool registerWidget( GtkWidget* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! enabled state
        void setEnabled( bool value );

        //! inner shadow enabled state
        void setInnerShadowsEnabled( bool value )
        { _innerShadowsEnabled = value; }

        //! inner shadow enabled state
        bool innerShadowsEnabled( void ) const
        { return _innerShadowsEnabled; }

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

        //! flat widget
        FlatWidgetEngine& flatWidgetEngine( void ) const
        { return *_flatWidgetEngine; }

        //! groupbox engine
        GroupBoxEngine& groupBoxEngine( void ) const
        { return *_groupBoxEngine; }

        //! background hint
        BackgroundHintEngine& backgroundHintEngine( void ) const
        { return *_backgroundHintEngine; }

        //! main window engine
        MainWindowEngine& mainWindowEngine( void ) const
        { return *_mainWindowEngine; }

        //! menu item engine
        MenuItemEngine& menuItemEngine( void ) const
        { return *_menuItemEngine; }

        //! hover engine
        HoverEngine& hoverEngine( void ) const
        { return *_hoverEngine; }

        //! paned (splitter) engine
        PanedEngine& panedEngine( void ) const
        { return *_panedEngine; }

        //! scrollbar engine
        ScrollBarEngine& scrollBarEngine( void ) const
        { return *_scrollBarEngine; }

        //! scrolled window engine
        ScrolledWindowEngine& scrolledWindowEngine( void ) const
        { return *_scrolledWindowEngine; }

        //! inner shadow engine
        InnerShadowEngine& innerShadowEngine( void ) const
        { return *_innerShadowEngine; }

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

        //! tree view mouse-over animation engine
        TreeViewStateEngine& treeViewStateEngine( void ) const
        { return *_treeViewStateEngine; }

        //! menubar mouse-over animation engine
        MenuBarStateEngine& menuBarStateEngine( void ) const
        { return *_menuBarStateEngine; }

        //! menu mouse-over animation engine
        MenuStateEngine& menuStateEngine( void ) const
        { return *_menuStateEngine; }

        //! toolbar mouse-over animation engine
        ToolBarStateEngine& toolBarStateEngine( void ) const
        { return *_toolBarStateEngine; }

        //@}

        protected:

        //! register new engine
        void registerEngine( BaseEngine* engine )
        { _engines.push_back( engine ); }

        //! groupbox engine
        GroupBoxLabelEngine& groupBoxLabelEngine( void ) const
        { return *_groupBoxLabelEngine; }

        //! destruction callback
        static gboolean destroyNotifyEvent( GtkWidget*, gpointer );

        //! combobox list size adjustment hook
        static gboolean sizeAllocationHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //! inner shadow composited mode enabling hook
        static gboolean innerShadowHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //! widget realize hook
        static gboolean realizationHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        private:

        //! inner shadow enabled state
        bool _innerShadowsEnabled;

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

        //! flat widget
        FlatWidgetEngine* _flatWidgetEngine;

        //! groupbox engine
        GroupBoxEngine* _groupBoxEngine;

        //! groupbox engine
        GroupBoxLabelEngine* _groupBoxLabelEngine;

        //! hover engine
        HoverEngine* _hoverEngine;

        //! main window engine
        MainWindowEngine* _mainWindowEngine;

        //! menu item engine
        MenuItemEngine* _menuItemEngine;

        //! paned engine
        PanedEngine* _panedEngine;

        //! scrollbar engine
        ScrollBarEngine* _scrollBarEngine;

        //! scrolled window engine
        ScrolledWindowEngine* _scrolledWindowEngine;

        //! inner shadow engine
        InnerShadowEngine* _innerShadowEngine;

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

        //! tree view mouse-over animation engine
        TreeViewStateEngine* _treeViewStateEngine;

        //! menubar mouse-over animation engine
        MenuBarStateEngine* _menuBarStateEngine;

        //! menu mouse-over animation engine
        MenuStateEngine* _menuStateEngine;

        //! toolbar mouse-over animation engine
        ToolBarStateEngine* _toolBarStateEngine;

        //@}

        //!@name hooks
        //@{

        //! true when hooks are initialized
        bool _hooksInitialized;

        //! realization hook
        Hook _realizationHook;

        //! combobox hook
        Hook _sizeAllocationHook;

        //! inner shadows hook
        Hook _innerShadowHook;


        //@}

        //! keep track of all registered widgets, and associated destroy callback
        typedef std::map< GtkWidget*, Signal > WidgetMap;
        WidgetMap _allWidgets;

    };

}

#endif
