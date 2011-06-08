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

#include "oxygenpanedengine.h"

#include <cassert>

namespace Oxygen
{

    //_____________________________________________________________
    PanedEngine::PanedEngine( Animations* parent ):
        BaseEngine( parent ),
        _verticalCursorLoaded( false ),
        _horizontalCursorLoaded( false ),
        _verticalCursor( 0L ),
        _horizontalCursor( 0L )
     {}

    //_____________________________________________________________
    PanedEngine::~PanedEngine( void )
    {
        if( _verticalCursor ) gdk_cursor_unref( _verticalCursor );
        if( _horizontalCursor ) gdk_cursor_unref( _horizontalCursor );
    }

    //_____________________________________________________________
    bool PanedEngine::registerWidget( GtkWidget* widget )
    {

        // make sure it is not already registered
        if( _data.find( widget ) != _data.end() ) return false;

        GdkCursor* cursor(0L);
        if( GTK_IS_VPANED( widget ) )
        {
            if( !_verticalCursorLoaded )
            {

                assert( !_verticalCursor );

                // get display
                GdkDisplay *display( gdk_display_get_default () );
                _verticalCursor = gdk_cursor_new_from_name( display, "col-resize" );
                _verticalCursorLoaded = true;

            }

            cursor = _verticalCursor;
            _data.insert( widget );

        } else if( GTK_IS_HPANED( widget ) ) {

            if( !_horizontalCursorLoaded )
            {

                assert( !_horizontalCursor );

                // get display
                GdkDisplay *display( gdk_display_get_default () );
                _horizontalCursor = gdk_cursor_new_from_name( display, "row-resize" );
                _horizontalCursorLoaded = true;

            }

            cursor = _horizontalCursor;
            _data.insert( widget );

        }

        if( cursor )
        {

            // load handle window
            // assign cursor
            GdkWindow* window(  gtk_paned_get_handle_window( GTK_PANED( widget ) ) );
            gdk_window_set_cursor( window, cursor );

            return true;

        } else return false;

    }
}
