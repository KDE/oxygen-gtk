#ifndef oxygenbackgroundhintengine_h
#define oxygenbackgroundhintengine_h
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

#include "../oxygenflags.h"
#include "oxygenbaseengine.h"

#include <gtk/gtk.h>
#include <set>
#include <algorithm>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#include <X11/Xdefs.h>
#endif

namespace Oxygen
{

    //! forward declaration
    class Animations;
    class StyleHelper;

    enum BackgroundHint
    {

        BackgroundGradient = 1<<0,
        BackgroundPixmap = 1<<1
    };

    OX_DECLARE_FLAGS( BackgroundHints, BackgroundHint )
    OX_DECLARE_OPERATORS_FOR_FLAGS( BackgroundHints )

    //! associates widgets with some type of data
    class BackgroundHintEngine: public BaseEngine
    {

        public:

        //! constructor
        BackgroundHintEngine( Animations* );

        //! destructor
        virtual ~BackgroundHintEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget )
        { return registerWidget( widget, BackgroundGradient|BackgroundPixmap ); }

        //! register widget
        virtual bool registerWidget( GtkWidget*, BackgroundHints );

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* );

        //! returns true if widget is registered
        bool contains( GtkWidget* widget ) const;

        #ifdef GDK_WINDOWING_X11
        Atom backgroundGradientAtom( void ) const
        { return _backgroundGradientAtom; }
        #endif

        protected:

        #ifdef GDK_WINDOWING_X11
        //! argb hint atom
        Atom _backgroundGradientAtom;

        //! background gradient hint atom
        Atom _backgroundPixmapAtom;

        //! map widget and window id
        class Data
        {

            public:

            //! constructor
            Data( GtkWidget* widget, XID id ):
                _widget( widget ),
                _id( id )
            {}

            //! equal to operator
            bool operator == (const Data& other ) const
            {
                return
                    other._widget == _widget &&
                    other._id == _id;
            }

            //! less than operator
            bool operator < ( const Data& other ) const
            {
                if( _widget != other._widget ) return _widget < other._widget;
                else return _id < other._id;
            }

            //! widget
            GtkWidget* _widget;

            //! window id
            XID _id;

        };

        //! true if widget is included
        virtual bool contains( const Data& data ) const
        { return _data.find( data ) != _data.end(); }

        //! used to find Data matching given widget
        class SameWidgetFTor
        {

            public:

            //! constructor
            SameWidgetFTor( GtkWidget* widget ):
                _widget( widget )
                {}

            //! predicate
            bool operator () (const Data& data ) const
            { return data._widget == _widget; }

            private:

            //! prediction
            GtkWidget* _widget;

        };

        //! store registered widgets
        std::set<Data> _data;

        #endif

    };

}

#endif
