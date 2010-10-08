/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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

#include "oxygenanimations.h"
#include "../config.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //! unregister widget on destruction
    void Animations::destroyRegisteredWidget( GtkWidget* widget, GdkEvent* event, gpointer data )
    { Oxygen::Animations::instance().unregisterWidget( widget ); }


    //____________________________________________________________________________________________
    Animations& Animations::instance( void )
    {
        static Animations singleton;
        return singleton;
    }

    //_________________________________________
    Animations::~Animations( void )
    {

        delete _comboBoxEngine;
        delete _menuShellEngine;
        delete _lineEditEngine;
        delete _spinBoxEngine;

    }

    //____________________________________________________________________________________________
    void Animations::registerWidget( GtkWidget* widget )
    {

        if( _allWidgets.find( widget ) != _allWidgets.end() ) return;

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::Animations::registerWidget - " << widget << std::endl;
        #endif

        WidgetData data;
        data._destroyId = g_signal_connect( widget, "destroy", G_CALLBACK( destroyRegisteredWidget ), 0L );
        data._styleChangeId = g_signal_connect( widget, "style-set", G_CALLBACK( destroyRegisteredWidget ), 0L );
        _allWidgets.insert( std::make_pair( widget, data ) );

    }

    //____________________________________________________________________________________________
    void Animations::unregisterWidget( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::Animations::unregisterWidget - " << widget << std::endl;
        #endif

        // find in map
        WidgetMap::iterator iter( _allWidgets.find( widget ) );
        assert( iter != _allWidgets.end() );

        // disconnect signal
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._destroyId );
        g_signal_handler_disconnect(G_OBJECT(widget), iter->second._styleChangeId );

        // erase from map
        _allWidgets.erase( widget );

        // erase from all maps
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); iter++ )
        { (*iter)->unregisterWidget( widget ); }

    }

    //_________________________________________
    Animations::Animations( void )
    {

        // create engines
        registerEngine( _comboBoxEngine = new ComboBoxEngine( this ) );
        registerEngine( _menuShellEngine = new MenuShellEngine( this ) );
        registerEngine( _lineEditEngine = new LineEditEngine( this ) );
        registerEngine( _spinBoxEngine = new SpinBoxEngine( this ) );

    }




}
