/*
    oxygenwidgetexplorer.h
    printout widget information on button press, for debugging
    -------------------

    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    Largely inspired from Qtcurve style
    SPDX-FileCopyrightText: Craig Drummond, 2003 - 2010 <craig.p.drummond@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenwidgetexplorer.h"
#include "config.h"

#include <iostream>

namespace Oxygen
{
    //___________________________________________________________
    WidgetExplorer::WidgetExplorer( void ):
        _enabled( false ),
        _hooksInitialized( false )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WidgetExplorer::WidgetExplorer" << std::endl;
        #endif

    }


    //_________________________________________________
    WidgetExplorer::~WidgetExplorer( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WidgetExplorer::~WidgetExplorer" << std::endl;
        #endif

        _buttonPressHook.disconnect();

    }

    //_________________________________________________
    void WidgetExplorer::initializeHooks( void )
    {
        if( _hooksInitialized ) return;

        _buttonPressHook.connect( "button-press-event", (GSignalEmissionHook)buttonPressHook, this );
        _hooksInitialized = true;
    }

    //_________________________________________________
    void WidgetExplorer::setEnabled( bool value )
    { _enabled = value; }

    //_________________________________________________________________
    gboolean WidgetExplorer::buttonPressHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // cast data to window manager
        WidgetExplorer &explorer( *static_cast<WidgetExplorer*>(data) );
        if( !explorer._enabled ) return TRUE;

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );
        if( !GTK_IS_WIDGET( widget ) ) return TRUE;

        std::cerr << "Oxygen::WidgetExplorer::buttonPressHook -";
        int row( 0 );
        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        {

            if( row ) std::cerr << " parent:";
            std::cerr << " " << parent<< " (" << G_OBJECT_TYPE_NAME( parent ) << ")" << std::endl;
            ++row;
        }
        if( row > 1 ) std::cerr << std::endl;
        return TRUE;

    }

}
