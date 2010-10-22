/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#define OXYGEN_DEBUG 1

namespace Oxygen
{

    //________________________________________________________________________________
    void TreeViewData::connect( GtkWidget* widget )
    {

        // store target
        _target = widget;

        // base class
        HoverData::connect( widget );

        // get full-width flag
        if( GTK_IS_TREE_VIEW( widget ) )
        {
            gtk_widget_style_get( widget, "row_ending_details", &_fullWidth, NULL );
            GtkTreeModel* model( gtk_tree_view_get_model( GTK_TREE_VIEW( widget ) ) );
            _rowDeletedId = g_signal_connect( G_OBJECT( model ), "row-deleted", G_CALLBACK( rowDeletedEvent ), this );
        }

        _motionId = g_signal_connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), this );

        // also register scrollbars from parent scrollWindow
        registerScrollBars( widget );

    }

    //________________________________________________________________________________
    void TreeViewData::disconnect( GtkWidget* widget )
    {

        // reset target
        _target = 0L;

        // motion handler
        g_signal_handler_disconnect( G_OBJECT(widget), _motionId );

        // TODO: this should really get handled at model deletion
        if( _rowDeletedId >= 0 && GTK_IS_TREE_VIEW( widget ) )
        {
            GtkTreeModel* model( gtk_tree_view_get_model( GTK_TREE_VIEW( widget ) ) );
            g_signal_handler_disconnect( G_OBJECT(model), _rowDeletedId );
        }

        // also free path if valid
        _cellInfo.clear();

        // disconnect all children
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); ++iter )
        { iter->second.disconnect( iter->first ); }
        _childrenData.clear();

        // base class
        HoverData::disconnect( widget );

    }

    //________________________________________________________________________________
    void TreeViewData::updatePosition( GtkWidget* widget, int x, int y )
    {

        if( !GTK_IS_TREE_VIEW( widget ) ) return;
        GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );

        // get cellInfo at x and y
        Gtk::CellInfo cellInfo( treeView, x, y );

        // do nothing if unchanged
        if( cellInfo == _cellInfo ) return;

        // prepare update area
        // get old rectangle
        GdkRectangle oldRect( _cellInfo.backgroundRect( treeView ) );
        if( _fullWidth ) { oldRect.x = 0; oldRect.width = widget->allocation.width; }

        // get new rectangle and update position
        GdkRectangle newRect( cellInfo.backgroundRect( treeView ) );
        if( cellInfo.isValid() )
        {

            _x = newRect.x + newRect.width/2;
            _y = newRect.y + newRect.height/2;

            if( _fullWidth ) { newRect.x = 0; newRect.width = widget->allocation.width; }

        } else {

            _x = -1;
            _y = -1;
        }

        // take the union of both rectangles
        GdkRectangle updateRect;
        if( Gtk::gdk_rectangle_is_valid( &oldRect ) )
        {

            if( Gtk::gdk_rectangle_is_valid( &newRect ) ) gdk_rectangle_union( &oldRect, &newRect, &updateRect );
            else updateRect = oldRect;

        } else updateRect = newRect;

        // store new cell info
        _cellInfo = cellInfo;

        // convert to widget coordinates and schedule redraw
        gtk_tree_view_convert_bin_window_to_widget_coords( treeView, updateRect.x, updateRect.y, &updateRect.x, &updateRect.y );
        Gtk::gtk_widget_queue_draw( widget, &updateRect );

    }

    //________________________________________________________________________________
    void TreeViewData::clearPosition( GtkWidget* widget )
    {

        // clear stored position
        _x = -1;
        _y = -1;

        // check widget
        if( !widget ) widget = _target;
        if( !widget ) return;

        // check path and widget
        if( !( _cellInfo.isValid() && GTK_IS_TREE_VIEW( widget ) ) ) return;
        GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );

        // prepare update area
        GdkRectangle updateRect( _cellInfo.backgroundRect( treeView ) );
        updateRect.x = 0;
        updateRect.width = widget->allocation.width;

        // clear path and column
        _cellInfo.clear();

        // schedule redraw
        gtk_tree_view_convert_bin_window_to_widget_coords( treeView, updateRect.x, updateRect.y, &updateRect.x, &updateRect.y );
        Gtk::gtk_widget_queue_draw( widget, &updateRect );

    }

    //________________________________________________________________________________
    void TreeViewData::repaintSelection( GtkWidget* widget )
    {

        // check widget
        if( !widget ) widget = _target;
        if( !widget ) return;

        // check path and widget
        if( !( hovered() && _cellInfo.isValid() && GTK_IS_TREE_VIEW( widget ) ) ) return;
        GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );

        // prepare update area
        GdkRectangle updateRect( _cellInfo.backgroundRect( treeView ) );
        updateRect.x = 0;
        updateRect.width = widget->allocation.width;

        // schedule redraw
        gtk_tree_view_convert_bin_window_to_widget_coords( treeView, updateRect.x, updateRect.y, &updateRect.x, &updateRect.y );
        Gtk::gtk_widget_queue_draw( widget, &updateRect );

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
        { registerChild( hScrollBar ); }

        if( GtkWidget* vScrollBar = gtk_scrolled_window_get_vscrollbar( scrolledWindow ) )
        { registerChild( vScrollBar ); }

    }

    //________________________________________________________________________________
    void TreeViewData::registerChild( GtkWidget* widget )
    {

        // make sure widget is not already in map
        if( _childrenData.find( widget ) == _childrenData.end() )
        {

            #if OXYGEN_DEBUG
            std::cout << "Oxygen::TreeViewData::registerChild - " << widget << std::endl;
            #endif

            // allocate new ChildData
            ChildData data;
            data._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
            data._valueChangedId = g_signal_connect( G_OBJECT(widget), "value-changed", G_CALLBACK( childValueChanged ), this );

            // and insert in map
            _childrenData.insert( std::make_pair( widget, data ) );

        }

    }

    //________________________________________________________________________________
    void TreeViewData::unregisterChild( GtkWidget* widget )
    {

        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;
        iter->second.disconnect( widget );
        _childrenData.erase( iter );

    }

    //____________________________________________________________________________________________
    gboolean TreeViewData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<TreeViewData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void TreeViewData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<TreeViewData*>(data)->unregisterChild( widget ); }

    //________________________________________________________________________________
    void TreeViewData::childValueChanged( GtkRange* widget, gpointer data )
    {
        static_cast<TreeViewData*>(data)->repaintSelection();
        return;
    }

    //________________________________________________________________________________
    gboolean TreeViewData::motionNotifyEvent(GtkWidget* widget, GdkEventMotion* event, gpointer data )
    {

        static_cast<TreeViewData*>( data )->updatePosition( widget, (int)event->x, (int)event->y );
        return FALSE;

    }

    //________________________________________________________________________________
    void TreeViewData::rowDeletedEvent( GtkTreeModel*, GtkTreePath* path, gpointer data )
    { static_cast<TreeViewData*>( data )->clearPosition(); }

    //____________________________________________________________________________________________
    void TreeViewData::ChildData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::TreeViewData::ChildData::disconnect - " << widget << std::endl;
        #endif

        g_signal_handler_disconnect(G_OBJECT(widget), _destroyId );
        g_signal_handler_disconnect(G_OBJECT(widget), _styleChangeId );
        g_signal_handler_disconnect(G_OBJECT(widget), _valueChangedId );

    }

}
