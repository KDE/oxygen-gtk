/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenscrollbarstatedata.h"
#include "../config.h"

#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    void ScrollBarStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrollBarStateData::connect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _target = widget;
        _upArrowData._timeLine.connect( (GSourceFunc)delayedUpdate, this );
        _downArrowData._timeLine.connect( (GSourceFunc)delayedUpdate, this );
    }

    //_____________________________________________
    void ScrollBarStateData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrollBarStateData::disconnect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _upArrowData._timeLine.disconnect();
        _upArrowData._rect = Gtk::gdk_rectangle();
        _upArrowData._state = false;

        _downArrowData._timeLine.disconnect();
        _downArrowData._rect = Gtk::gdk_rectangle();
        _downArrowData._state = false;

        _target = 0L;

    }

    //_____________________________________________
    bool ScrollBarStateData::Data::updateState( bool state )
    {
        if( state == _state ) return false;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ScrollBarStateData::Data::updateState - "
            << " state: " << state
            << std::endl;
        #endif

        _state = state;

        // change direction
        _timeLine.setDirection( state ? TimeLine::Forward:TimeLine::Backward );

        // restart timeLine if needed
        if( _timeLine.isConnected() && !_timeLine.isRunning() ) _timeLine.start();

        return true;
    }

    //_____________________________________________
    gboolean ScrollBarStateData::delayedUpdate( gpointer pointer )
    {

        ScrollBarStateData& data( *static_cast<ScrollBarStateData*>( pointer ) );

        if( data._target )
        { gtk_widget_queue_draw( data._target ); }

        return FALSE;

    }

}
