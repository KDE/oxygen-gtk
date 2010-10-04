#ifndef oxygenwidgetset_h
#define oxygenwidgetset_h
/*
* this file is part of the oxygen gtk engine
* Copyright(c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright(C ) 2008 Robert Staudinger
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

#include <cassert>
#include <set>
#include <vector>
#include <gtk/gtk.h>

namespace Oxygen
{

    class WidgetContainer
    {

        public:

        //! constructor
        explicit WidgetContainer( void )
        {}

        //! destructor
        virtual ~WidgetContainer( void )
        {}

        //! erase widget from container
        virtual void erase( GtkWidget* ) = 0;

    };

    //! stores widgets into a std::set
    /*! stores widgets into a std::set. Automatically remove them from the set when they are destroyed */
    class WidgetSet: public WidgetContainer
    {

        public:

        //! destructor
        virtual ~WidgetSet( void )
        {}

        //! insert new widget
        void insert( GtkWidget* );

        //! true if widget is in list
        bool contains( GtkWidget* widget )
        { return _widgets.find( widget ) != _widgets.end(); }

        //! erase
        void erase( GtkWidget* widget )
        { _widgets.erase( widget ); }

        private:

        //! constructor is private
        WidgetSet( void )
        {}

        //! registered widgets
        std::set<GtkWidget*> _widgets;

        friend class WidgetSetFactory;

    };

    //! stores all widget set
    class WidgetSetFactory
    {
        public:

        //! singleton
        static WidgetSetFactory& instance( void );

        //! destructor
        virtual ~WidgetSetFactory( void )
        {
            for( ContainerList::iterator iter = _containers.begin(); iter != _containers.end(); iter++ )
            { delete *iter; }
        }

        //! create new widget set
        WidgetSet* createWidgetSet( void );

        //! register new widget
        void registerWidget( GtkWidget* );

        //! register new widget
        void unregisterWidget( GtkWidget* );

        private:

        //! constructor is private
        WidgetSetFactory( void )
        {}

        //! list of registered widget sets
        typedef std::vector< WidgetContainer* > ContainerList;
        ContainerList _containers;

        //! keep track of all registered widgets
        std::set< GtkWidget* > _allWidgets;

    };

}

#endif
