/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* the treeview data code is largely inspired from the gtk redmond engine
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

#include "oxygentreeviewdata.h"
#include "../config.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void TreeViewData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewData::connect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        // store target
        _target = widget;

        // base class
        HoverData::connect( widget );

        // get full-width flag
        if( GTK_IS_TREE_VIEW( widget ) )
        {
            gtk_widget_style_get( widget, "row_ending_details", &_fullWidth, NULL );

            if( hovered() )
            {
                // on connection, needs to check whether mouse pointer is in widget or not
                // to have the proper initial value of the hover flag
                GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                gint xPointer,yPointer;
                gdk_window_get_pointer( gtk_widget_get_window( widget ), &xPointer, &yPointer, 0L );
                gtk_tree_view_convert_widget_to_bin_window_coords( treeView, xPointer, yPointer, &xPointer, &yPointer );
                updatePosition( widget, xPointer, yPointer );
            }

            // columns changed signal connection
            _columnsChangedId.connect( G_OBJECT(widget), "columns-changed", G_CALLBACK( columnsChanged ), this );
        }

        // motion notify signal connection
        _motionId.connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), this );

        // also register scrollbars from parent scrollWindow
        registerScrollBars( widget );

    }

    //________________________________________________________________________________
    void TreeViewData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewData::disconnect - " << widget << " (" << (widget ? G_OBJECT_TYPE_NAME( widget ):"null") << ")" << std::endl;
        #endif

        // reset target
        _target = 0L;

        _columnsChangedId.disconnect();
        _motionId.disconnect();

        // also free path if valid
        _cellInfo.clear();

        // disconnect scrollbars
        _vScrollBar.disconnect();
        _hScrollBar.disconnect();

        // base class
        HoverData::disconnect( widget );

    }

    //________________________________________________________________________________
    void TreeViewData::updateColumnsCursor( void ) const
    {
        // check tree view and target
        if( !_cursor ) return;
        if( !GTK_IS_TREE_VIEW( _target ) ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewData::updateColumnsCursor - " << _target << " (" << G_OBJECT_TYPE_NAME( _target ) << ")" << std::endl;
        #endif

        GList* children( gtk_tree_view_get_columns( GTK_TREE_VIEW( _target ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {
            if( GTK_IS_TREE_VIEW_COLUMN( child->data ) )
            {
                GdkWindow* window( GTK_TREE_VIEW_COLUMN( child->data )->window );
                gdk_window_set_cursor( window, _cursor );
            }
        }

        if( children ) g_list_free( children );
    }

    //________________________________________________________________________________
    void TreeViewData::updateHoveredCell( void )
    {
        if( !( isDirty() && GTK_IS_TREE_VIEW( _target ) ) ) return;
        _cellInfo = Gtk::CellInfo( GTK_TREE_VIEW( _target ), _x, _y );
        setDirty( false );
    }

    //________________________________________________________________________________
    bool TreeViewData::setHovered( GtkWidget* widget, bool value )
    {
        if( !HoverData::setHovered( widget, value ) ) return false;
        if( !value ) clearPosition();
        return true;
    }

    //________________________________________________________________________________
    void TreeViewData::updatePosition( GtkWidget* widget, int x, int y )
    {

        // check type and cast to treeview
        if( !GTK_IS_TREE_VIEW( widget ) ) return;
        GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );

        // store position
        _x = x;
        _y = y;

        // get cellInfo at x and y
        Gtk::CellInfo cellInfo( treeView, x, y );

        // do nothing if unchanged
        if( cellInfo == _cellInfo ) return;

        // prepare update area
        // get old rectangle
        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
        GdkRectangle oldRect( _cellInfo.backgroundRect( treeView ) );
        if( _fullWidth )
        {
            oldRect.x = 0;
            oldRect.width = allocation.width;
        }

        // get new rectangle and update position
        GdkRectangle newRect( cellInfo.backgroundRect( treeView ) );
        if( cellInfo.isValid() && _fullWidth )
        {
            newRect.x = 0;
            newRect.width = allocation.width;
        }

        // take the union of both rectangles
        GdkRectangle updateRect( Gtk::gdk_rectangle() );
        Gtk::gdk_rectangle_union( &oldRect, &newRect, &updateRect );

        // store new cell info
        _cellInfo = cellInfo;

        // convert to widget coordinates and schedule redraw
        gtk_tree_view_convert_bin_window_to_widget_coords( treeView, updateRect.x, updateRect.y, &updateRect.x, &updateRect.y );
        Gtk::gtk_widget_queue_draw( widget, &updateRect );

    }

    //________________________________________________________________________________
    void TreeViewData::clearPosition( GtkWidget* widget )
    {

        // check widget
        if( !widget ) widget = _target;
        if( !widget ) return;

        // check path and widget
        if( !( _cellInfo.isValid() && GTK_IS_TREE_VIEW( widget ) ) ) return;
        GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );

        // prepare update area
        GdkRectangle updateRect( _cellInfo.backgroundRect( treeView ) );
        updateRect.x = 0;
        updateRect.width = Gtk::gtk_widget_get_allocation( widget ).width;

        // clear path and column
        _cellInfo.clear();

        // schedule redraw
        gtk_tree_view_convert_bin_window_to_widget_coords( treeView, updateRect.x, updateRect.y, &updateRect.x, &updateRect.y );
        Gtk::gtk_widget_queue_draw( widget, &updateRect );

    }

    //________________________________________________________________________________
    void TreeViewData::triggerRepaint( void )
    {
        if( !( _target && hovered() ) ) return;
        setDirty( true );
    }

    //________________________________________________________________________________
    void TreeViewData::registerScrollBars( GtkWidget* widget )
    {

        // find parent scrolled window
        GtkWidget* parent( Gtk::gtk_parent_scrolled_window( widget ) );
        if( !parent ) return;

        // cast and register scrollbars
        GtkScrolledWindow *scrolledWindow( GTK_SCROLLED_WINDOW( parent ) );

        if( GtkWidget* hScrollBar = gtk_scrolled_window_get_hscrollbar( scrolledWindow ) )
        { registerChild( hScrollBar, _hScrollBar ); }

        if( GtkWidget* vScrollBar = gtk_scrolled_window_get_vscrollbar( scrolledWindow ) )
        { registerChild( vScrollBar, _vScrollBar ); }

    }

    //________________________________________________________________________________
    void TreeViewData::registerChild( GtkWidget* widget, ScrollBarData& data )
    {

        if( data._widget ) data.disconnect();

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewData::registerChild - " << widget << std::endl;
        #endif

        // make sure widget is not already in map
        data._widget = widget;
        data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        data._valueChangedId.connect( G_OBJECT(widget), "value-changed", G_CALLBACK( childValueChanged ), this );

    }

    //________________________________________________________________________________
    void TreeViewData::unregisterChild( GtkWidget* widget )
    {
        if( widget == _vScrollBar._widget ) _vScrollBar.disconnect();
        else if( widget == _hScrollBar._widget ) _hScrollBar.disconnect();
    }

    //____________________________________________________________________________________________
    gboolean TreeViewData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<TreeViewData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //________________________________________________________________________________
    void TreeViewData::childValueChanged( GtkRange* widget, gpointer data )
    {
        static_cast<TreeViewData*>(data)->triggerRepaint();
        return;
    }

    //________________________________________________________________________________
    void TreeViewData::columnsChanged( GtkTreeView*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewData::columnsChanged" << std::endl;
        #endif

        static_cast<TreeViewData*>(data)->updateColumnsCursor();
        return;
    }

    //________________________________________________________________________________
    gboolean TreeViewData::motionNotifyEvent(GtkWidget* widget, GdkEventMotion* event, gpointer data )
    {

        // check event
        if( !( event && event->window ) ) return FALSE;

        // make sure event window is treeview's bin window
        if( GTK_IS_TREE_VIEW( widget ) && gtk_tree_view_get_bin_window( GTK_TREE_VIEW( widget ) ) == event->window )
        { static_cast<TreeViewData*>( data )->updatePosition( widget, (int)event->x, (int)event->y ); }

        return FALSE;
    }

    //____________________________________________________________________________________________
    void TreeViewData::ScrollBarData::disconnect( void )
    {

        if( !_widget ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewData::ScrollBarData::disconnect - " << _widget << std::endl;
        #endif

        _destroyId.disconnect();
        _valueChangedId.disconnect();
        _widget = 0L;

    }

}
