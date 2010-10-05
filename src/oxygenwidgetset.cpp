/*
* this file is part of the oxygen gtk engine
* Copyright(c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright(C ) 2008 Robert Staudinger
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

#include <iostream>
#include "oxygenwidgetset.h"

//! unregister widget on destruction
extern "C" void destroyRegisteredWidget( GtkWidget* widget, GdkEvent* event, gpointer data )
{ Oxygen::WidgetSetFactory::instance().unregisterWidget( widget ); }

namespace Oxygen
{


    //____________________________________________________________________________________________
    WidgetSetFactory& WidgetSetFactory::instance( void )
    {
        static WidgetSetFactory singleton;
        return singleton;
    }

    //____________________________________________________________________________________________
    WidgetSet* WidgetSetFactory::createWidgetSet( void )
    {
        WidgetSet* out = new WidgetSet();
        _containers.push_back( out );
        return out;
    }

    //____________________________________________________________________________________________
    void WidgetSetFactory::registerWidget( GtkWidget* widget )
    {
        if( _allWidgets.find( widget ) != _allWidgets.end() ) return;
        _allWidgets.insert( widget );
        g_signal_connect( widget, "destroy", G_CALLBACK( destroyRegisteredWidget ), 0L );

    }

    //____________________________________________________________________________________________
    void WidgetSetFactory::unregisterWidget( GtkWidget* widget )
    {

        _allWidgets.erase( widget );
        for( ContainerList::iterator iter = _containers.begin(); iter != _containers.end(); iter++ )
        { (*iter)->erase( widget ); }

    }

    //____________________________________________________________________________________________
    void WidgetSet::insert( GtkWidget* widget )
    {

        // insert in set and register to factory if new
        if( _widgets.insert( widget ).second )
        { WidgetSetFactory::instance().registerWidget( widget ); }

        return;
    }

}
