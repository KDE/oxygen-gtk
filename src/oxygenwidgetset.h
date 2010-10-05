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
#include <map>
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
        bool contains( GtkWidget* widget ) const
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


    //! stores map of pointer to widget and user data
    template< typename T>
    class WidgetMap: public WidgetContainer
    {

        public:


        //! insert new widget
        inline void insert( GtkWidget* widget, const T& value = T());

        //! true if widget is in list
        bool contains( GtkWidget* widget ) const
        { return _widgets.find( widget ) != _widgets.end(); }

        //! return value
        T& value( GtkWidget* widget )
        {
            typename std::map<GtkWidget*, T>::iterator iter( _widgets.find( widget ) );
            assert( iter != _widgets.end() );
            return iter->second;
        }

        //! erase
        void erase( GtkWidget* widget )
        { _widgets.erase( widget ); }

        private:

        //! constructor
        WidgetMap( void )
        {}

        //! map
        std::map<GtkWidget*, T> _widgets;

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

        //! create new set of type T and insert in map
        /*! T must derive from the WidgetContainer class, and have an empty constructor */
        template< typename T >
        T* createNew( void );
//         //! create new widget set
//         WidgetSet* createWidgetSet( void );
//
//         //! create new map
//         template<typename T>
//         WidgetMap<T>* createWidgetMap( void );

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

        //! keep track of all registered widgets, and associated destroy callback
        typedef std::map< GtkWidget*, int > SignalIdMap;
        SignalIdMap _allWidgets;

    };

    //___________________________________________________
    template< typename T> void WidgetMap<T>::insert( GtkWidget* widget, const T& value )
    {
        _widgets.insert( std::make_pair( widget, value ) );
        WidgetSetFactory::instance().registerWidget( widget );
    }

//     //___________________________________________________
//     template<typename T>
//         WidgetMap<T>* WidgetSetFactory::createWidgetMap( void )
//     {
//         WidgetMap<T>* out = new WidgetMap<T>();
//         _containers.push_back( out );
//         return out;
//     }

    //___________________________________________________
    template<typename T>
        T* WidgetSetFactory::createNew( void )
    {
        T* out = new T();
        _containers.push_back( out );
        return out;
    }

}

#endif
