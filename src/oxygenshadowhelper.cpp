/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2011 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygencairocontext.h"
#include "oxygencairoutils.h"
#include "config.h"
#include "oxygengtkutils.h"
#include "oxygenmetrics.h"
#include "oxygenrgba.h"
#include "oxygenshadowhelper.h"

#include "config.h"

#include <iostream>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

namespace Oxygen
{

    //______________________________________________
    ShadowHelper::ShadowHelper( void ):
        _size(0),
        _atom(0),
        _hooksInitialized( false )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::ShadowHelper" << std::endl;
        #endif
    }

    //______________________________________________
    ShadowHelper::~ShadowHelper( void )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::~ShadowHelper" << std::endl;
        #endif

        reset();
    }

    //______________________________________________
    void ShadowHelper::reset( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::reset" << std::endl;
        #endif

        GdkScreen* screen = gdk_screen_get_default();
        if( !screen ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::~ShadowHelper - reset" << std::endl;
        #endif

        Display* display( GDK_DISPLAY_XDISPLAY( gdk_screen_get_display( screen ) ) );

        // round pixmaps
        for( PixmapList::const_iterator iter = _roundPixmaps.begin(); iter != _roundPixmaps.end(); ++iter )
        { XFreePixmap(display, *iter); }
        _roundPixmaps.clear();

        // square pixmaps
        for( PixmapList::const_iterator iter = _squarePixmaps.begin(); iter != _squarePixmaps.end(); ++iter )
        { XFreePixmap(display, *iter); }
        _squarePixmaps.clear();

        // reset size
        _size = 0;

    }

    //______________________________________________
    void ShadowHelper::initializeHooks( void )
    {
       if( _hooksInitialized ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::initializeHooks" << std::endl;
        #endif

        // install hooks
        _realizeHook.connect( "realize", (GSignalEmissionHook)realizeHook, this );
        _hooksInitialized = true;

    }

    //______________________________________________
    void ShadowHelper::initialize( const ColorUtils::Rgba& color, const WindowShadow& shadow )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::initialize" << std::endl;
        #endif

        reset();
        _size = int(shadow.shadowSize()) - WindowShadow::Overlap;

        // round tiles
        WindowShadowKey key;
        key.hasTopBorder = true;
        key.hasBottomBorder = true;
        _roundTiles = shadow.tileSet( color, key );

        // square tiles
        key.hasTopBorder = false;
        key.hasBottomBorder = false;
        _squareTiles = shadow.tileSet( color, key );

        // re-install shadows for all windowId
        for( WidgetMap::const_iterator iter = _widgets.begin(); iter != _widgets.end(); ++iter )
        { installX11Shadows( iter->first ); }

    }

    //______________________________________________
    bool ShadowHelper::registerWidget( GtkWidget* widget )
    {

        // check widget
        if( !( widget && GTK_IS_WINDOW( widget ) ) ) return false;

        // make sure that widget is not already registered
        if( _widgets.find( widget ) != _widgets.end() ) return false;

        // check if window is accepted
        if( !acceptWidget( widget ) ) return false;

        // try install shadows
        installX11Shadows( widget );

        // register in map and returns success
        WidgetData data;
        data._destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( destroyNotifyEvent ), this );
        _widgets.insert( std::make_pair( widget, data ) );

        return true;

    }

    //______________________________________________
    void ShadowHelper::unregisterWidget( GtkWidget* widget )
    {
        // find matching data in map
        WidgetMap::iterator iter( _widgets.find( widget ) );
        if( iter == _widgets.end() ) return;

        // disconnect
        iter->second._destroyId.disconnect();

        // remove from map
        _widgets.erase( iter );
    }

    //______________________________________________
    bool ShadowHelper::isMenu( GtkWidget* widget ) const
    {
        if( !( widget && GTK_IS_WINDOW( widget ) ) ) return false;
        const GdkWindowTypeHint hint( gtk_window_get_type_hint( GTK_WINDOW( widget ) ) );
        return
            hint == GDK_WINDOW_TYPE_HINT_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_POPUP_MENU;
    }

    //______________________________________________
    bool ShadowHelper::isToolTip( GtkWidget* widget ) const
    {
        if( !( widget && GTK_IS_WINDOW( widget ) ) ) return false;
        const GdkWindowTypeHint hint( gtk_window_get_type_hint( GTK_WINDOW( widget ) ) );
        return hint == GDK_WINDOW_TYPE_HINT_TOOLTIP;
    }

    //______________________________________________
    bool ShadowHelper::acceptWidget( GtkWidget* widget ) const
    {

        // check widget and type
        if( !( widget && GTK_IS_WINDOW( widget ) ) ) return false;

        // for openoffice, accept all non decorated windows
        if( _applicationName.isOpenOffice() ) return true;

        // otherwise check window hint
        const GdkWindowTypeHint hint( gtk_window_get_type_hint( GTK_WINDOW( widget ) ) );
        return
            hint == GDK_WINDOW_TYPE_HINT_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_POPUP_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_COMBO ||
            hint == GDK_WINDOW_TYPE_HINT_TOOLTIP;
    }

    //______________________________________________
    void ShadowHelper::createPixmapHandles( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowHelper::createPixmapHandles" << std::endl;
        #endif

        // create atom
        if( !_atom )
        {

            // get screen and check
            GdkScreen* screen = gdk_screen_get_default();
            if( !screen )
            {

                #if OXYGEN_DEBUG
                std::cerr << "ShadowHelper::createPixmapHandles - screen is NULL" << std::endl;
                #endif

                return;
            }

            // get display and check
            Display* display( GDK_DISPLAY_XDISPLAY( gdk_screen_get_display( screen ) ) );
            if( !display )
            {

                #if OXYGEN_DEBUG
                std::cerr << "ShadowHelper::createPixmapHandles - display is NULL" << std::endl;
                #endif

                return;
            }

           _atom = XInternAtom( display, "_KDE_NET_WM_SHADOW", False);
        }

        // make sure size is valid
        if( _size <= 0 ) return;

        // opacity
        const int shadowOpacity = 150;

        if( _roundPixmaps.empty() || _squarePixmaps.empty() )
        {
            // get screen, display, visual and check
            // no need to check screen and display, since was already done for ATOM
            GdkScreen* screen = gdk_screen_get_default();
            if( !gdk_screen_get_rgba_visual( screen ) )
            {

                #if OXYGEN_DEBUG
                std::cerr << "ShadowHelper::createPixmapHandles - no valid RGBA visual found." << std::endl;
                #endif

                return;

            }
        }

        // make sure pixmaps are not already initialized
        if( _roundPixmaps.empty() )
        {

            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 1 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 2 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 5 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 8 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 7 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 6 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 3 ), shadowOpacity ) );
            _roundPixmaps.push_back( createPixmap( _roundTiles.surface( 0 ), shadowOpacity ) );

        }

        if( _squarePixmaps.empty() )
        {

            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 1 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 2 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 5 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 8 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 7 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 6 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 3 ), shadowOpacity ) );
            _squarePixmaps.push_back( createPixmap( _squareTiles.surface( 0 ), shadowOpacity ) );

        }

    }

    //______________________________________________
    Pixmap ShadowHelper::createPixmap( const Cairo::Surface& surface, int opacity ) const
    {
        assert( surface.isValid() );
        const int width( cairo_surface_get_width( surface ) );
        const int height( cairo_surface_get_height( surface ) );

        GdkScreen* screen = gdk_screen_get_default();
        Display* display( GDK_DISPLAY_XDISPLAY( gdk_screen_get_display( screen ) ) );
        Window root( GDK_WINDOW_XID( gdk_screen_get_root_window( screen ) ) );
        Pixmap pixmap = XCreatePixmap( display, root, width, height, 32 );

        // create surface for pixmap
        {
            Cairo::Surface dest( cairo_xlib_surface_create( display, pixmap, GDK_VISUAL_XVISUAL( gdk_screen_get_rgba_visual( screen ) ), width, height ) );
            Cairo::Context context( dest );
            cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );

            cairo_rectangle( context, 0, 0, width, height );
            cairo_set_source_surface( context, surface, 0, 0 );
            cairo_fill( context );

            if( opacity < 255 )
            {

                cairo_set_operator( context, CAIRO_OPERATOR_DEST_IN );
                cairo_set_source( context, ColorUtils::Rgba( 0, 0, 0, double(opacity)/255 ) );
                cairo_rectangle( context, 0, 0, width, height );
                cairo_fill( context );

            }

        }

        return pixmap;

    }

    //______________________________________________
    void ShadowHelper::installX11Shadows( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ShadowHelper::installX11Shadows - "
            << " widget: " << widget
            << " wid: " << GDK_WINDOW_XID( gtk_widget_get_window( widget ) )
            << std::endl;
        #endif

        // make sure handles and atom are defined
        createPixmapHandles();

        GdkWindow  *window = gtk_widget_get_window( widget );
        GdkDisplay *display = gtk_widget_get_display( widget );

        std::vector<unsigned long> data;
        const bool isMenu( this->isMenu( widget ) );
        const bool isToolTip( this->isToolTip( widget ) );
        if( _applicationName.isOpenOffice() || ( (isMenu||isToolTip) && _applicationName.isXul( widget ) ) )
        {

            data = _squarePixmaps;
            data.push_back( _size );
            data.push_back( _size );
            data.push_back( _size );
            data.push_back( _size );

        } else {

            data = _roundPixmaps;
            if( isMenu )
            {

                /*
                for menus, need to shrink top and bottom shadow size, since body is done likely with respect to real size
                in painting method (Oxygen::Style::renderMenuBackground)
                */
                data.push_back( _size - Menu_VerticalOffset );
                data.push_back( _size );
                data.push_back( _size - Menu_VerticalOffset );
                data.push_back( _size );

            } else {

                // all sides have same sizz
                data.push_back( _size );
                data.push_back( _size );
                data.push_back( _size );
                data.push_back( _size );

            }

        }

        // change property
        XChangeProperty(
            GDK_DISPLAY_XDISPLAY( display ), GDK_WINDOW_XID(window), _atom, XA_CARDINAL, 32, PropModeReplace,
            reinterpret_cast<const unsigned char *>(&data[0]), data.size() );

    }

    //_______________________________________________________
    void ShadowHelper::uninstallX11Shadows( GtkWidget* widget ) const
    {

        if( !widget ) return;

        GdkWindow  *window = gtk_widget_get_window( widget );
        GdkDisplay *display = gtk_widget_get_display( widget );
        XDeleteProperty( GDK_DISPLAY_XDISPLAY( display ), GDK_WINDOW_XID(window), _atom);

    }

    //_______________________________________________________
    gboolean ShadowHelper::realizeHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;
        static_cast<ShadowHelper*>(data)->registerWidget( widget );
        return TRUE;
    }

    //____________________________________________________________________________________________
    gboolean ShadowHelper::destroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<ShadowHelper*>(data)->unregisterWidget( widget );
        return FALSE;
    }

}
