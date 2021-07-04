
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignalhandler.h"

namespace Oxygen
{

    //________________________________________________________
    SignalHandler::~SignalHandler( void )
    {

        // disconnect all signals
        for( std::vector<Signal>::iterator iter = _signals.begin(); iter != _signals.end(); ++iter )
        { iter->disconnect(); }

    }

    //________________________________________________________
    void SignalHandler::connect( GObject* object, const std::string& signal, GCallback callback, gpointer data )
    {
        // check if object is already connected
        if( _objects.find( object ) == _objects.end() )
        {

            // insert object
            _objects.insert( object );

            // insert destroy signal
            Signal destroyId;
            destroyId.connect( object, "destroy", (GCallback) destroyNotifyEvent, this );
            _signals.push_back( destroyId );

        }

        // insert signal
        Signal signalId;
        signalId.connect( object, signal, callback, data );
        _signals.push_back( signalId );

    }

    //________________________________________________________
    gboolean SignalHandler::destroyNotifyEvent( GtkWidget* widget, gpointer pointer )
    {
        SignalHandler& data( *static_cast<SignalHandler*>( pointer ) );
        for( std::vector<Signal>::iterator iter = data._signals.begin(); iter != data._signals.end(); ++iter )
        { if( iter->belongsTo( G_OBJECT( widget ) ) ) iter->disconnect(); }

        // remove from objects
        data._objects.erase( G_OBJECT( widget ) );
        return FALSE;

    }

}
