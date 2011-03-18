#ifndef oxygendemowidget_h
#define oxygendemowidget_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#include <gtk/gtk.h>
#include <cassert>

namespace Oxygen
{

    class DemoWidget
    {

        public:

        //! constructor
        DemoWidget( void ):
            _mainWidget( 0L ),
            _enabled( true )
        {}

        //! destructor
        virtual ~DemoWidget( void )
        {}

        //! main widget
        virtual GtkWidget* mainWidget( void )
        {
            assert( _mainWidget );
            return _mainWidget;
        }

        //! enable state
        virtual void setEnabled( bool value )
        {
            _enabled = value;
            if( _mainWidget )
            { gtk_widget_set_sensitive( _mainWidget, _enabled ); }
        }

        protected:

        //! assign main widget
        virtual void setMainWidget( GtkWidget *widget )
        {
            assert( !_mainWidget );
            _mainWidget = widget;
            setEnabled( _enabled );
        }

        private:

        //! copy constructor
        DemoWidget( const DemoWidget& )
        { assert( false ); }

        //! assignment operator
        DemoWidget& operator = (const DemoWidget& )
        {
            assert( false );
            return *this;
        }

        //! main widget
        GtkWidget* _mainWidget;

        //! enable state
        bool _enabled;

    };

}

#endif
