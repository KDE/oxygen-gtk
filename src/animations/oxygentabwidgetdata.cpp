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
        _target = widget;
        _motionId = g_signal_connect( G_OBJECT(widget), "motion-notify-event", (GCallback)motionNotifyEvent, this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
        _pageAddedId = g_signal_connect( G_OBJECT(widget), "page-added", (GCallback)pageAddedEvent, this );

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
    void TabWidgetData::disconnect( GtkWidget* widget )
    {

        _target = 0L;
        g_signal_handler_disconnect(G_OBJECT(widget), _motionId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
        g_signal_handler_disconnect(G_OBJECT(widget), _pageAddedId );

        // disconnect all children
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); ++iter )
        { unregisterChild( iter->first ); }

    }

    //________________________________________________________________________________
    void TabWidgetData::updateHoveredTab(GtkWidget* widget )
    {

        if( !widget ) widget = _target;
        if( !widget ) return;

        // get pointer position
        int xPointer,yPointer;
        gdk_window_get_pointer(widget->window,&xPointer,&yPointer,NULL);

        // loop over tabs and check matching
        for( unsigned int i = 0; i < _tabRects.size(); i++ )
        {
            if( Gtk::gdk_rectangle_contains( &_tabRects[i], xPointer, yPointer ) )
            { setHoveredTab( widget, i ); return; }
        }

        // reset hovered tab
        setHoveredTab( widget, -1 );

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
    void TabWidgetData::pageAddedEvent( GtkNotebook*, GtkWidget* child, guint, gpointer data)
    { static_cast<TabWidgetData*>( data )->registerChild( child ); }

    //________________________________________________________________________________
    void TabWidgetData::registerChild( GtkWidget* widget )
    {
        // make sure widget is not already in map
        if( _childrenData.find( widget ) != _childrenData.end() ) return;

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::TabWidgetData::registerChild - " << widget << std::endl;
        #endif

        // allocate new ChildData
        ChildData data;
        data._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        data._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
        data._enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", (GCallback)childCrossingNotifyEvent, this );
        data._leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)childCrossingNotifyEvent, this );

        // and insert in map
        _childrenData.insert( std::make_pair( widget, data ) );

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
        std::cout << "Oxygen::TabWidgetData::unregisterChild - " << widget << std::endl;
        #endif

        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._destroyId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._styleChangeId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._enterId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._leaveId );
        _childrenData.erase( iter );
    }

    //____________________________________________________________________________________________
    gboolean TabWidgetData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<TabWidgetData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void TabWidgetData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<TabWidgetData*>(data)->unregisterChild( widget ); }

    //____________________________________________________________________________________________
    gboolean TabWidgetData::childCrossingNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        // retrieve widget's parent and check type
        static_cast<TabWidgetData*>(data)->updateHoveredTab();
        return FALSE;

    }

}
