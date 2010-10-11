/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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

#include <gtk/gtk.h>
#include <cassert>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void TabWidgetData::connect( GtkWidget* widget )
    {
        _motionId = g_signal_connect( G_OBJECT(widget), "motion-notify-event", (GCallback)motionNotifyEvent, this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
    }

    //________________________________________________________________________________
    void TabWidgetData::disconnect( GtkWidget* widget )
    {
        g_signal_handler_disconnect(G_OBJECT(widget), _motionId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
    }

    //________________________________________________________________________________
    void TabWidgetData::updateHoveredTab(GtkWidget* widget )
    {

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

}
