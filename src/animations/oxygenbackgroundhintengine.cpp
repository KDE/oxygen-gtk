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

#include <iostream>

#ifdef GDK_WINDOWING_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

namespace Oxygen
{

    //_________________________________________________________
    BackgroundHintEngine::BackgroundHintEngine( Animations* animations ):
        BaseEngine( animations )
    {

        // create background gradient atom
        #ifdef GDK_WINDOWING_X11
        GdkDisplay *display( gdk_display_get_default () );
        if( display )
        {

            _backgroundGradientAtom = XInternAtom( GDK_DISPLAY_XDISPLAY( display ), "_KDE_OXYGEN_BACKGROUND_GRADIENT", False);
            _backgroundPixmapAtom = XInternAtom( GDK_DISPLAY_XDISPLAY( display ), "_KDE_OXYGEN_BACKGROUND_PIXMAP", False);

        } else {

            _backgroundGradientAtom = None;
            _backgroundPixmapAtom = None;

        }
        #endif
    }

    //_________________________________________________________
    bool BackgroundHintEngine::registerWidget( GtkWidget* widget, BackgroundHints hints )
    {

        #ifdef GDK_WINDOWING_X11

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
        GdkDisplay *display( gtk_widget_get_display( topLevel ) );
        if( display && _backgroundGradientAtom && (hints&BackgroundGradient) )
        {
            unsigned long uLongValue( true );
            XChangeProperty(
                GDK_DISPLAY_XDISPLAY( display ), id, _backgroundGradientAtom, XA_CARDINAL, 32, PropModeReplace,
                reinterpret_cast<const unsigned char *>(&uLongValue), 1 );
        }

        if( display && _backgroundPixmapAtom && (hints&BackgroundPixmap) )
        {
            unsigned long uLongValue( true );
            XChangeProperty(
                GDK_DISPLAY_XDISPLAY( display ), id, _backgroundPixmapAtom, XA_CARDINAL, 32, PropModeReplace,
                reinterpret_cast<const unsigned char *>(&uLongValue), 1 );
        }

        // register
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::BackgroundHintEngine::registerWidget - (" << topLevel << "," << id << ")" << std::endl;
        #endif

        // insert in set
        _data.insert( data );

        // call base class
        BaseEngine::registerWidget( topLevel );
        return true;

        #else

        return false;

        #endif
    }

    //_________________________________________________________
    void BackgroundHintEngine::unregisterWidget( GtkWidget* widget )
    {

        #ifdef GDK_WINDOWING_X11
        SameWidgetFTor ftor( widget );
        for( std::set<Data>::iterator iter = _data.begin(); iter != _data.end(); )
        {

            if( ftor( *iter ) )
            {

                #if OXYGEN_DEBUG
                std::cerr << "Oxygen::BackgroundHintEngine::unregisterWidget - (" << iter->_widget << "," << iter->_id << ")" << std::endl;
                #endif

                _data.erase( iter++ );

            } else ++iter;

        }
        #endif

    }


    //____________________________________________________________________
    bool BackgroundHintEngine::contains( GtkWidget* widget ) const
    {

        #ifdef GDK_WINDOWING_X11
        return std::find_if( _data.begin(), _data.end(), SameWidgetFTor( widget ) ) != _data.end();
        #else
        return false;
        #endif

    }

}
