#ifndef oxygenwidgetlookup_h
#define oxygenwidgetlookup_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygenhook.h"
#include "oxygensignal.h"

#include <cairo/cairo.h>
#include <gtk/gtk.h>

#include <list>
#include <map>

namespace Oxygen
{

    //! uses the widget's "draw" signal to bind GtkWidget and cairo context
    /*! this allows us to retrieve painted widgets from ThemingEngine rendering primitives */
    class WidgetLookup
    {

        public:

        //! constructor
        explicit WidgetLookup( void );

        //! destructor
        virtual ~WidgetLookup( void );

        //! initialize hooks
        void initializeHooks( void );

        //! find widget matching given context and type
        GtkWidget* find( cairo_t*, const GtkWidgetPath* ) const;

        //! find widget matching given context and type
        GtkWidget* find( cairo_t*, GType ) const;

        protected:

        //! bind widget to context
        void bind( GtkWidget*, cairo_t* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! hook connected to widget's "draw" signal
        static gboolean drawHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //! destruction callback
        static gboolean destroyNotifyEvent( GtkWidget*, gpointer );

        private:

        //! true if hooks are initialized
        bool _hooksInitialized;

        //! hook connected to widget's "draw" signal
        Hook _drawHook;

        //! store current context
        cairo_t* _context;

        //! store list of associated widgets, to keep track of destruction
        typedef std::list<GtkWidget*> WidgetList;
        WidgetList _widgets;

        //! keep track of all registered widgets, and associated destroy callback
        typedef std::map< GtkWidget*, Signal > WidgetMap;
        WidgetMap _allWidgets;

    };

}

#endif
