/*
* oxygenwidgetexplorer.h
* printout widget information on button press, for debugging
* -------------------
*
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* Largely inspired from Qtcurve style
* Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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

#include "oxygenwidgetexplorer.h"

#include "../oxygengtkutils.h"
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

        Gtk::gtk_widget_print_tree( widget );
        return TRUE;

    }

}
