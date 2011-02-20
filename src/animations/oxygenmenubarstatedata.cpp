/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* MenuBarState prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc. <@redhat.com>
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
*
* the menushell data code is largely inspired from the gtk redmond engine
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

#include "oxygenmenubarstatedata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void MenuBarStateData::connect( GtkWidget* widget )
    {

        _target = widget;
        _motionId.connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), this );
        _leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );

        // connect timeLines
        _current._timeLine.connect( (GSourceFunc)delayedUpdate, this );
        _previous._timeLine.connect( (GSourceFunc)delayedUpdate, this );

        // set directions
        _current._timeLine.setDirection( TimeLine::Forward );
        _previous._timeLine.setDirection( TimeLine::Backward );

    }

    //________________________________________________________________________________
    void MenuBarStateData::disconnect( GtkWidget* widget )
    {

        _target = 0L;

        // stop timer
        _timer.stop();

        // disconnect signal
        _motionId.disconnect();
        _leaveId.disconnect();

        // disconnect timelines
        _current._timeLine.disconnect();
        _previous._timeLine.disconnect();

    }

    //________________________________________________________________________________
    bool MenuBarStateData::updateState( GtkWidget* widget, const GdkRectangle& rect, bool state )
    {

        // do nothing if animations are disabled
        if( !_animationsEnabled ) return true;

        if( state && widget != _current._widget )
        {

            // stop timer. This blocks fade-out animation
            const bool locked( _timer.isRunning() );
            if( _timer.isRunning() ) _timer.stop();

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _current._widget )
            {
                if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

                // move current to previous
                _previous._widget = _current._widget;
                _previous._rect = _current._rect;
                if( !locked ) _previous._timeLine.start();
            }

            // assign new widget to current and start animation
            _current._widget = widget;
            _current._rect = rect;
            if( _current._widget )
            {
                if( !locked ) _current._timeLine.start();
                else delayedUpdate( this );
            }

            return true;

        } else if( (!state) && widget == _current._widget ) {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

            // move current to previous
            _previous._widget = _current._widget;
            _previous._rect = _current._rect;
            if( _previous._widget )
            {
                //_previous._timeLine.start();
                if( _timer.isRunning() ) _timer.stop();
                _timer.start( 100, (GSourceFunc)delayedAnimate, this );
            }

            // assign invalid widget to current
            _current._widget = 0L;
            _current._rect = Gtk::gdk_rectangle();
            return true;

        } else return false;

    }

    //________________________________________________________________________________
    gboolean MenuBarStateData::motionNotifyEvent(GtkWidget* widget, GdkEventMotion*, gpointer pointer )
    {
        if( !GTK_IS_MENU_SHELL( widget ) ) return FALSE;

        // cast pointer
        MenuBarStateData& data( *static_cast<MenuBarStateData*>( pointer ) );

        // get pointer position
        gint xPointer, yPointer;
        gdk_window_get_pointer( gtk_widget_get_window( widget ), &xPointer, &yPointer, 0L );

        GList *children( gtk_container_get_children( GTK_CONTAINER(widget) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;

            GtkWidget* childWidget( GTK_WIDGET( child->data ) );
            if( gtk_widget_get_state( childWidget ) == GTK_STATE_INSENSITIVE ) continue;

            const GtkAllocation& allocation( childWidget->allocation );
            if( Gtk::gdk_rectangle_contains( &allocation, xPointer, yPointer ) )
            {

                // this triggers widget update
                data.updateState( childWidget, allocation, true );
                gtk_widget_set_state( childWidget, GTK_STATE_PRELIGHT );

            } else {

                // this triggers widget update
                data.updateState( childWidget, allocation, false );
                gtk_widget_set_state( childWidget, GTK_STATE_NORMAL );

            }
        }

        if( children ) g_list_free( children );

        return FALSE;

    }

    //________________________________________________________________________________
    gboolean MenuBarStateData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer pointer )
    {

        if( !GTK_IS_MENU_SHELL( widget ) ) return FALSE;
        GList* children( gtk_container_get_children( GTK_CONTAINER( widget ) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;
            if( gtk_widget_get_state( GTK_WIDGET( child->data ) ) == GTK_STATE_INSENSITIVE ) continue;

            // this is terrible code. I hate gtk. (hugo)
            if( (!GTK_IS_MENU(GTK_MENU_ITEM(child->data)->submenu)) ||
                (!(GTK_WIDGET_REALIZED(GTK_MENU_ITEM(child->data)->submenu) &&
                GTK_WIDGET_VISIBLE(GTK_MENU_ITEM(child->data)->submenu) &&
                GTK_WIDGET_REALIZED(GTK_MENU(GTK_MENU_ITEM(child->data)->submenu)->toplevel) &&
                GTK_WIDGET_VISIBLE(GTK_MENU(GTK_MENU_ITEM(child->data)->submenu)->toplevel))))
            { gtk_widget_set_state( GTK_WIDGET(child->data), GTK_STATE_NORMAL ); }
        }

        if( children ) g_list_free( children );

        // also triggers animation of current widget
        MenuBarStateData& data( *static_cast<MenuBarStateData*>( pointer ) );
        if( data._current._widget )
        { data.updateState( data._current._widget, data._current._widget->allocation, false ); }

        return FALSE;

    }

    //_____________________________________________
    gboolean MenuBarStateData::delayedUpdate( gpointer pointer )
    {

        MenuBarStateData& data( *static_cast<MenuBarStateData*>( pointer ) );

        if( data._target )
        { Gtk::gtk_widget_queue_draw( data._target ); }

        return FALSE;

    }

    //________________________________________________________________________________
    gboolean MenuBarStateData::delayedAnimate( gpointer pointer )
    {

        MenuBarStateData& data( *static_cast<MenuBarStateData*>( pointer ) );
        if( data._previous._widget )
        {
            assert( !data._previous._timeLine.isRunning() );
            data._previous._timeLine.start();
        }

        return FALSE;
    }

}
