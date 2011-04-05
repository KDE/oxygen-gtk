/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* the tabwidget data code is largely inspired from the gtk redmond engine
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

#include "oxygentabwidgetdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>
#include <cassert>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void TabWidgetData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TabWidgetData::connect - " << widget << std::endl;
        #endif

        _target = widget;
        _motionId.connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), this );
        _leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );
        _pageAddedId.connect( G_OBJECT(widget), "page-added", G_CALLBACK( pageAddedEvent ), this );

        updateRegisteredChildren( widget );

    }

    //________________________________________________________________________________
    void TabWidgetData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TabWidgetData::disconnect - " << widget << std::endl;
        #endif

        _target = 0L;
        _motionId.disconnect();
        _leaveId.disconnect();
        _pageAddedId.disconnect();

        // disconnect all children
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); ++iter )
        { iter->second.disconnect(); }
        _childrenData.clear();

    }

    //________________________________________________________________________________
    void TabWidgetData::updateHoveredTab(GtkWidget* widget )
    {

        if( !widget ) widget = _target;
        if( !widget ) return;

        // get pointer position
        int xPointer,yPointer;
        gdk_window_get_pointer( gtk_widget_get_window( widget ), &xPointer, &yPointer, 0L );

        // loop over tabs and check matching
        for( unsigned int i = (unsigned int)Gtk::gtk_notebook_find_first_tab( widget ); i < _tabRects.size(); i++ )
        {
            if( Gtk::gdk_rectangle_contains( &_tabRects[i], xPointer, yPointer ) )
            { setHoveredTab( widget, i ); return; }
        }

        // reset hovered tab
        setHoveredTab( widget, -1 );
        return;

    }

    //________________________________________________________________________________
    void TabWidgetData::updateTabRect( GtkWidget* widget, int index, const GdkRectangle& r )
    {
        // make sure the vector has the right size
        if( !GTK_IS_NOTEBOOK( widget ) ) return;
        GtkNotebook* notebook = GTK_NOTEBOOK( widget );
        _tabRects.resize( gtk_notebook_get_n_pages( notebook ), defaultRect() );

        // check index against number of tabs
        if( index < 0 || index >= (int)_tabRects.size() )
        { return; }

        // store rectangle
        _tabRects[index]=r;
    }

    //________________________________________________________________________________
    void TabWidgetData::setDirty( bool value )
    {
        if( _dirty == value ) return;
        _dirty = value;
        if( _dirty && _target )
        {

            // we should only update the tabbar rect here
            GdkRectangle updateRect;
            Gtk::gtk_notebook_get_tabbar_rect( GTK_NOTEBOOK( _target ), &updateRect );
            Gtk::gtk_widget_queue_draw( _target, &updateRect );

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::TabWidgetData::setDirty - update: " << updateRect << std::endl;
            #endif

        }

    }

    //________________________________________________________________________________
    void TabWidgetData::setHoveredTab( GtkWidget* widget, int index )
    {

        if( _hoveredTab == index ) return;

        _hoveredTab = index;

        GdkRectangle updateRect( Gtk::gdk_rectangle() );
        for( RectangleList::const_iterator iter = _tabRects.begin(); iter != _tabRects.end(); ++iter )
        { gdk_rectangle_union( &(*iter), &updateRect, &updateRect ); }

        gtk_widget_queue_draw_area( widget, updateRect.x-4, updateRect.y-4, updateRect.width+8, updateRect.height+8 );

        return;
    }

    //________________________________________________________________________________
    gboolean TabWidgetData::motionNotifyEvent(GtkWidget* widget, GdkEventMotion*, gpointer data )
    {

        static_cast<TabWidgetData*>( data )->updateHoveredTab( widget );
        return FALSE;

    }

    //________________________________________________________________________________
    gboolean TabWidgetData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        // reset hovered tab
        static_cast<TabWidgetData*>( data )->setHoveredTab( widget, -1 );
        return FALSE;
    }

    //________________________________________________________________________________
    void TabWidgetData::pageAddedEvent( GtkNotebook* parent, GtkWidget* child, guint, gpointer data)
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TabWidgetData::pageAddedEvent - " << child << std::endl;
        #endif
        static_cast<TabWidgetData*>(data)->updateRegisteredChildren( GTK_WIDGET( parent ) );
    }

    //________________________________________________________________________________
    void TabWidgetData::updateRegisteredChildren( GtkWidget* widget )
    {

        if( !widget ) widget = _target;
        if( !widget ) return;

        // cast to notebook and check against number of pages
        if( GTK_IS_NOTEBOOK( widget ) )
        {
            GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
            for( int i = 0; i <  gtk_notebook_get_n_pages( notebook ); ++i )
            {

                // retrieve page and tab label
                GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
                registerChild( gtk_notebook_get_tab_label( notebook, page ) );
            }
        }
    }

    //________________________________________________________________________________
    void TabWidgetData::registerChild( GtkWidget* widget )
    {

        // do nothing if child is invalid (might happen: not checked at calling stage)
        if( !widget ) return;

        // make sure widget is not already in map
        if( _childrenData.find( widget ) == _childrenData.end() )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::TabWidgetData::registerChild - " << widget << std::endl;
            #endif

            // allocate new ChildData
            ChildData data;
            data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( childCrossingNotifyEvent ), this );
            data._leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( childCrossingNotifyEvent ), this );

            if( GTK_IS_CONTAINER( widget ) )
            { data._addId.connect( G_OBJECT(widget), "add", G_CALLBACK( childAddedEvent ), this ); }

            // and insert in map
            _childrenData.insert( std::make_pair( widget, data ) );

        }

        /*
        also insert widget's children, recursively.
        that should take care of buttons in tabs and other fancy stuff that applications mght do
        */
        if( GTK_IS_CONTAINER( widget ) )
        {

            GList *children( gtk_container_get_children( GTK_CONTAINER(widget) ) );
            for( GList* child = g_list_first(children); child; child = g_list_next(child) )
            { registerChild( GTK_WIDGET( child->data ) ); }

            if( children ) g_list_free( children );
        }

    }

    //________________________________________________________________________________
    void TabWidgetData::unregisterChild( GtkWidget* widget )
    {

        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TabWidgetData::unregisterChild - " << widget << std::endl;
        #endif

        iter->second.disconnect();
        _childrenData.erase( iter );

    }

    //____________________________________________________________________________________________
    gboolean TabWidgetData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<TabWidgetData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void TabWidgetData::childAddedEvent( GtkContainer* parent, GtkWidget*, gpointer data )
    {
        static_cast<TabWidgetData*>(data)->updateRegisteredChildren();
        return;
    }


    //____________________________________________________________________________________________
    gboolean TabWidgetData::childCrossingNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        // retrieve widget's parent and check type
        static_cast<TabWidgetData*>(data)->updateHoveredTab();
        return FALSE;

    }

    //____________________________________________________________________________________________
    void TabWidgetData::ChildData::disconnect( void )
    {

        _destroyId.disconnect();
        _enterId.disconnect();
        _leaveId.disconnect();
        _addId.disconnect();

    }

}
