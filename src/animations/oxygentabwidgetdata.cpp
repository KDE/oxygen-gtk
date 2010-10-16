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
        std::cout << "Oxygen::TabWidgetData::connect - " << widget << std::endl;
        #endif

        _target = widget;
        _motionId = g_signal_connect( G_OBJECT(widget), "motion-notify-event", (GCallback)motionNotifyEvent, this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
        _pageAddedId = g_signal_connect( G_OBJECT(widget), "page-added", (GCallback)pageAddedEvent, this );
        _pageSwitchId = g_signal_connect( G_OBJECT(widget), "switch-page", (GCallback)pageSwitchEvent, this );

        updateRegisteredChildren( widget );

    }

    //________________________________________________________________________________
    void TabWidgetData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::TabWidgetData::disconnect - " << widget << std::endl;
        #endif

        _target = 0L;
        g_signal_handler_disconnect(G_OBJECT(widget), _motionId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
        g_signal_handler_disconnect(G_OBJECT(widget), _pageAddedId );
        g_signal_handler_disconnect(G_OBJECT(widget), _pageSwitchId );

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
        std::cout << "Oxygen::TabWidgetData::pageAddedEvent - " << child << std::endl;
        #endif
        static_cast<TabWidgetData*>(data)->updateRegisteredChildren( GTK_WIDGET( parent ) );
    }

    void setButtonsNormal(GtkContainer* container,gpointer data=NULL)
    {
        if(GTK_IS_BUTTON(container))
        {
            gtk_button_set_relief(GTK_BUTTON(container),GTK_RELIEF_NORMAL);
            gtk_widget_set_state(GTK_WIDGET(container),GTK_STATE_NORMAL);
            return;
        }
        if(GTK_IS_CONTAINER(container))
            gtk_container_foreach(container,(GtkCallback)setButtonsNormal,NULL);
    }

    gboolean updateCloseButtons(GtkNotebook* notebook)
    {
        // cast to notebook and check against number of pages
        if( GTK_IS_NOTEBOOK( notebook ) )
        {
            GtkWidget* tabLabel=0;
            int numPages=gtk_notebook_get_n_pages( notebook );
            for( int i = 0; i < numPages; ++i )
            {

                // retrieve page and tab label
                GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
                if(page)
                    tabLabel=gtk_notebook_get_tab_label( notebook, page );

                if(page && tabLabel && GTK_IS_CONTAINER(tabLabel))
                    setButtonsNormal(GTK_CONTAINER(tabLabel));

            }
        }
        return FALSE;
    }

    //________________________________________________________________________________
    void TabWidgetData::pageSwitchEvent( GtkNotebook* notebook, GtkNotebookPage* /*page*/, guint /*page_num*/, gpointer /*data*/)
    {
        // make all the close buttons normal instead of active (pressed)
        // this is a dirty hack, but i dunno why buttons become active, so for now just force inactivate them
        // FIXME: this may not work if the system is too loaded so that the buttons haven't activated yet, but the timer activates
        g_timeout_add(10,(GSourceFunc)updateCloseButtons,(gpointer)notebook);
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

        // make sure widget is not already in map
        if( _childrenData.find( widget ) == _childrenData.end() )
        {

            #if OXYGEN_DEBUG
            std::cout << "Oxygen::TabWidgetData::registerChild - " << widget << std::endl;
            #endif

            // allocate new ChildData
            ChildData data;
            data._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
            data._enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", (GCallback)childCrossingNotifyEvent, this );
            data._leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)childCrossingNotifyEvent, this );

            if( GTK_IS_CONTAINER( widget ) )
            { data._addId = g_signal_connect( G_OBJECT(widget), "add", G_CALLBACK( childAddedEvent ), this ); }

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
        std::cout << "Oxygen::TabWidgetData::unregisterChild - " << widget << std::endl;
        #endif

        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._destroyId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._styleChangeId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._enterId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._leaveId );

        if( iter->second._addId >= 0 ) g_signal_handler_disconnect(G_OBJECT(widget), iter->second._addId );

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

}
