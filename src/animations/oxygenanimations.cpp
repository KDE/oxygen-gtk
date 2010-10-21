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

#include "oxygenanimations.h"
#include "../config.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //____________________________________________________________________________________________
    Animations& Animations::instance( void )
    {
        static Animations singleton;
        return singleton;
    }

    //____________________________________________________________________________________________
    Animations::~Animations( void )
    {
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { delete *iter; }
    }

    //____________________________________________________________________________________________
    void Animations::registerWidget( GtkWidget* widget )
    {

        if( _allWidgets.find( widget ) != _allWidgets.end() ) return;

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::Animations::registerWidget - " << widget << std::endl;
        #endif

        WidgetData data;
        data._destroyId = g_signal_connect( widget, "destroy", G_CALLBACK( destroyNotifyEvent ), this );
        data._styleChangeId = g_signal_connect( widget, "style-set", G_CALLBACK( styleChangeNotifyEvent ), this );
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
        g_signal_handler_disconnect( G_OBJECT(widget), iter->second._destroyId );
        g_signal_handler_disconnect( G_OBJECT(widget), iter->second._styleChangeId );

        // erase from map
        _allWidgets.erase( widget );

        // erase from all maps
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { (*iter)->unregisterWidget( widget ); }

    }

    //____________________________________________________________________________________________
    gboolean Animations::destroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<Animations*>(data)->unregisterWidget( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void Animations::styleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<Animations*>(data)->unregisterWidget( widget ); }

    //_________________________________________
    Animations::Animations( void )
    {

        // create engines
        registerEngine( _comboBoxEngine = new ComboBoxEngine( this ) );
        registerEngine( _mainWindowEngine = new MainWindowEngine( this ) );
        registerEngine( _menuShellEngine = new MenuShellEngine( this ) );
        registerEngine( _lineEditEngine = new LineEditEngine( this ) );
        registerEngine( _scrollBarEngine = new ScrollBarEngine( this ) );
        registerEngine( _tabWidgetEngine = new TabWidgetEngine( this ) );
        registerEngine( _treeViewEngine = new TreeViewEngine( this ) );
        registerEngine( _widgetSizeEngine = new WidgetSizeEngine( this ) );

    }




}
