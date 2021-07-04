#ifndef oxygenshadowhelper_h
#define oxygenshadowhelper_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2011 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenapplicationname.h"
#include "oxygencairosurface.h"
#include "oxygenhook.h"
#include "oxygensignal.h"
#include "oxygentileset.h"
#include "oxygenwindowshadow.h"

#include <vector>
#include <map>

#ifdef GDK_WINDOWING_X11
#include <X11/Xdefs.h>
#include <X11/Xlib.h>
#endif

namespace Oxygen
{

    //! handles X11 property passed to menu windows for window manager to draw proper shadows
    class ShadowHelper
    {

        public:

        //!@name property names
        //@{
        static const char* const netWMShadowAtomName;
        //@}

        //! constructor
        ShadowHelper( void );

        //! destructor
        virtual ~ShadowHelper( void );

        //! true is supported
        void setSupported( bool value )
        { _supported = value; }

        //! true if supported
        bool isSupported( void ) const
        { return _supported; }

        //! reset
        void reset( void );

        //! initialize hooks
        void initializeHooks( void );

        //! application name
        void setApplicationName( const ApplicationName& applicationName )
        { _applicationName = applicationName; }

        //! initialize
        void initialize( const ColorUtils::Rgba&, const WindowShadow& );

        //! register widget
        bool registerWidget( GtkWidget* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! true if widget is menu
        bool isMenu( GtkWidget* ) const;

        //! true if widget is menu
        bool isToolTip( GtkWidget* ) const;

        //! true if shadow should be installed on widget
        bool acceptWidget( GtkWidget* ) const;

        //! create pixmaps
        void createPixmapHandles( void );

        //! create Pixmap for given surface
        #ifdef GDK_WINDOWING_X11
        Pixmap createPixmap( const Cairo::Surface&, int opacity = 255 ) const;
        #endif

        //! install shadow X11 property on given widget
        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */
        void installX11Shadows( GtkWidget* );

        //! uninstall shadow X11 property on given widget
        void uninstallX11Shadows( GtkWidget* ) const;

        //! map event hook
        static gboolean realizeHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //! destruction callback
        static gboolean destroyNotifyEvent( GtkWidget*, gpointer );

        private:

        //! true if shadows are supported
        bool _supported;

        //! shadow size
        int _size;

        //! shadow tileset
        TileSet _roundTiles;

        //! shadow tileset
        TileSet _squareTiles;

        //! application name
        ApplicationName _applicationName;

        #ifdef GDK_WINDOWING_X11
        //! shadow atom
        Atom _atom;
        #endif

        //! number of pixmaps
        enum { numPixmaps = 8 };

        //! round shadows pixmap handles
        typedef std::vector<unsigned long> PixmapList;
        PixmapList _roundPixmaps;

        //! square shadows pixmap handles
        PixmapList _squarePixmaps;

        //! widget data
        class WidgetData
        {

            public:

            //! constructor
            WidgetData( void )
            {}

            //! destroy signal
            Signal _destroyId;

        };

        //! map widgets and window id
        typedef std::map<GtkWidget*, WidgetData> WidgetMap;
        WidgetMap _widgets;

        //! true if hooks are initialized
        bool _hooksInitialized;

        //! map-event hook
        Hook _realizeHook;

    };

}

#endif
