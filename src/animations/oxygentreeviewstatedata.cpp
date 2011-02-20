/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygentreeviewstatedata.h"
#include "../config.h"

#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    void TreeViewStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewStateData::connect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _target = widget;

        // connect timeLines
        _current._timeLine.connect( (GSourceFunc)delayedUpdate, this );
        _previous._timeLine.connect( (GSourceFunc)delayedUpdate, this );

        // set directions
        _current._timeLine.setDirection( TimeLine::Forward );
        _previous._timeLine.setDirection( TimeLine::Backward );

    }

    //_____________________________________________
    void TreeViewStateData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TreeViewStateData::disconnect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _current._timeLine.disconnect();
        _current._info.clear();

        _previous._timeLine.disconnect();
        _previous._info.clear();

        _target = 0L;

    }

    //_____________________________________________
    bool TreeViewStateData::updateState( const Gtk::CellInfo& info, bool state )
    {
        if( state && info != _current._info )
        {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _current._info.isValid() )
            {
                if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

                // update dirty rect, to make sure de-allocated cellInfo gets repainted
                if( _previous._info.isValid() && GTK_IS_TREE_VIEW( _target ) )
                { _dirtyRect = _previous._info.backgroundRect( GTK_TREE_VIEW( _target ) ); }

                // move current tab info to previous
                _previous._info = _current._info;
                _previous._timeLine.start();
            }

            // assign new info to current and start animation
            _current._info = info;
            if( _current._info.isValid() ) _current._timeLine.start();

            return true;

        } else if( (!state) && info == _current._info ) {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

            // update dirty rect, to make sure de-allocated cellInfo gets repainted
            if( _previous._info.isValid() && GTK_IS_TREE_VIEW( _target ) )
            { _dirtyRect = _previous._info.backgroundRect( GTK_TREE_VIEW( _target ) ); }

            // move current tab info to previous
            _previous._info = _current._info;
            if( _previous._info.isValid() ) _previous._timeLine.start();

            // assign invalid info to current
            _current._info.clear();

            return true;

        } else return false;

    }

    //_____________________________________________
    GdkRectangle TreeViewStateData::dirtyRect( void )
    {

        GdkRectangle rect( Gtk::gdk_rectangle() );
        if( GTK_IS_TREE_VIEW( _target ) )
        {
            GtkTreeView* treeView( GTK_TREE_VIEW( _target ) );

            const GdkRectangle previousRect( _previous._info.backgroundRect( treeView ) );
            const GdkRectangle currentRect( _current._info.backgroundRect( treeView ) );
            if( Gtk::gdk_rectangle_is_valid( &previousRect ) && Gtk::gdk_rectangle_is_valid( &currentRect ) )
            {

                gdk_rectangle_union( &previousRect, &currentRect, &rect );

            } else if( Gtk::gdk_rectangle_is_valid( &previousRect ) ) {

                rect = previousRect;

            } else if( Gtk::gdk_rectangle_is_valid( &currentRect ) ) {

                rect = currentRect;

            }

            // also union with dirty rect
            if( Gtk::gdk_rectangle_is_valid( &_dirtyRect ) )
            {
                if( Gtk::gdk_rectangle_is_valid( &rect ) ) gdk_rectangle_union( &_dirtyRect, &rect, &rect );
                else rect = _dirtyRect;

                _dirtyRect = Gtk::gdk_rectangle();

            }

            // finally convert to widget coordinated
            gtk_tree_view_convert_bin_window_to_widget_coords(
                treeView, rect.x, rect.y,
                &rect.x, &rect.y );


        }

        return rect;

    }

    //_____________________________________________
    gboolean TreeViewStateData::delayedUpdate( gpointer pointer )
    {

        TreeViewStateData& data( *static_cast<TreeViewStateData*>( pointer ) );

        if( data._target )
        {
            const GdkRectangle rect( data.dirtyRect() );
            Gtk::gtk_widget_queue_draw( data._target, &rect );
        }

        return FALSE;

    }

}
