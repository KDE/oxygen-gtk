/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenscrollbardata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void ScrollBarData::connect( GtkWidget* widget )
    {
        _target = widget;
        _locked = false;
        _valueChangedId.connect( G_OBJECT(widget), "value-changed", G_CALLBACK( valueChanged ), this );
    }

    //________________________________________________________________________________
    void ScrollBarData::disconnect( GtkWidget* )
    {
        _target = 0L;

        // reset timeout and locked flag
        _timer.stop();
        _locked = false;

        _valueChangedId.disconnect();
    }

    //________________________________________________________________________________
    void ScrollBarData::valueChanged( GtkRange* widget, gpointer pointer )
    {

        ScrollBarData& data( *static_cast<ScrollBarData*>( pointer ) );
        if( data._updatesDelayed )
        {

            // schedule delayed timeOut
            if( !data._timer.isRunning() )
            {

                data._timer.start( data._delay, (GSourceFunc)delayedUpdate, &data );
                data._locked = false;

            } else data._locked = true;


        } else if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( GTK_WIDGET( widget ) ) ) {

            gtk_widget_queue_draw( parent );

        }

        return;
    }

    //________________________________________________________________________________
    gboolean ScrollBarData::delayedUpdate( gpointer pointer )
    {

        ScrollBarData& data( *static_cast<ScrollBarData*>(pointer) );

        if( !data._target )
        {

            // if target is invalid, reset timeout and return
            data._locked = false;
            return FALSE;

        } else if( data._locked ) {

            // if locked, reset the flag and re-run timer
            data._locked = false;
            return TRUE;

        } else if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( GTK_WIDGET( data._target ) ) ) {

            // otherwise, trigger update
            gtk_widget_queue_draw( parent );
            return FALSE;

        }

        // if everything fails, unlock and do nothing
        data._locked = false;
        return FALSE;

    }

}
