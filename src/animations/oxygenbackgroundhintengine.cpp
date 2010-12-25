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

#include "oxygenbackgroundhintengine.h"
#include "config.h"
#include "../oxygenstylehelper.h"
#include "../oxygengtkutils.h"

#include <iostream>

namespace Oxygen
{

    //_________________________________________________________
    bool BackgroundHintEngine::registerWidget( GtkWidget* widget, StyleHelper* helper )
    {

        // get associated top level widget
        GtkWidget* topLevel( gtk_widget_get_toplevel( widget ) );
        if( !topLevel ) return false;

        // check associated window
        GdkWindow* window( gtk_widget_get_window( topLevel ) );
        if( !window ) return false;

        const XID id( GDK_WINDOW_XID( window ) );
        if( !id ) return false;

        Data data( topLevel, id );
        if( contains( data ) ) return false;

        // set hint
        helper->setHasBackgroundGradient( id, true );

        // register
        #if OXYGEN_DEBUG
        std::cout << "Oxygen::BackgroundHintEngine::registerWidget - (" << topLevel << "," << id << ")" << std::endl;
        #endif

        // insert in set
        _data.insert( data );

        // call base class
        BaseEngine::registerWidget( topLevel );
        return true;
    }

    //_________________________________________________________
    void BackgroundHintEngine::unregisterWidget( GtkWidget* widget )
    {
        SameWidgetFTor ftor( widget );
        for( std::set<Data>::iterator iter = _data.begin(); iter != _data.end(); )
        {

            if( ftor( *iter ) )
            {

                #if OXYGEN_DEBUG
                std::cout << "Oxygen::BackgroundHintEngine::unregisterWidget - (" << iter->_widget << "," << iter->_id << ")" << std::endl;
                #endif

                _data.erase( iter++ );

            } else ++iter;

        }
    }

}
