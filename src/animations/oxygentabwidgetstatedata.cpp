/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygentabwidgetstatedata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    const int TabWidgetStateData::IndexInvalid = -1;

    //_____________________________________________
    void TabWidgetStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TabWidgetStateData::connect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
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
    void TabWidgetStateData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::TabWidgetStateData::disconnect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _current._timeLine.disconnect();
        _current._index = IndexInvalid;

        _previous._timeLine.disconnect();
        _previous._index = IndexInvalid;

        _target = 0L;

    }

    //_____________________________________________
    bool TabWidgetStateData::updateState( int index, bool state )
    {
        if( state && index != _current._index )
        {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _current._index != IndexInvalid )
            {
                if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

                // move current tab index to previous
                _previous._index = _current._index;
                _previous._timeLine.start();
            }

            // assign new index to current and start animation
            _current._index = index;
            if( _current._index != IndexInvalid ) _current._timeLine.start();

            return true;

        } else if( (!state) && index == _current._index ) {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

            // move current tab index to previous
            _previous._index = _current._index;
            if( _previous._index != IndexInvalid ) _previous._timeLine.start();

            // assign invalid index to current
            _current._index = IndexInvalid;

            return true;

        } else return false;

    }

    //_____________________________________________
    GdkRectangle TabWidgetStateData::dirtyRect( void ) const
    {

        // FIXME: it might be enough to union the rects of the current and previous tab
        if( GTK_IS_NOTEBOOK( _target ) )
        {

            GdkRectangle rect( Gtk::gdk_rectangle() );
            Gtk::gtk_notebook_get_tabbar_rect( GTK_NOTEBOOK( _target ), &rect );

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::TabWidgetData::dirtyRect - " << rect << std::endl;
            #endif

            return rect;

        } else {

            return Gtk::gtk_widget_get_allocation( _target );

        }

    }

    //_____________________________________________
    gboolean TabWidgetStateData::delayedUpdate( gpointer pointer )
    {

        TabWidgetStateData& data( *static_cast<TabWidgetStateData*>( pointer ) );

        if( !data._target ) return FALSE;

        const GdkRectangle rect( data.dirtyRect() );
        Gtk::gtk_widget_queue_draw( data._target, &rect );
        return FALSE;

    }

}
