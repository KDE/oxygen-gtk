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
#include <string>

namespace Oxygen
{

    class DemoWidget
    {

        public:

        //! constructor
        DemoWidget( void ):
            _mainWidget( 0L ),
            _widget(0L),
            _enabled( true )
        {}

        //! destructor
        virtual ~DemoWidget( void )
        {}

        //! main widget
        virtual GtkWidget* mainWidget( void )
        { return _mainWidget; }

        //! enable state
        virtual void setEnabled( bool value )
        {
            _enabled = value;
            if( _widget )
            { gtk_widget_set_sensitive( _widget, _enabled ); }
        }

        //! name
        const std::string& name( void ) const
        { return _name; }

        //! icon name
        const std::string& iconName( void ) const
        { return _iconName; }

        protected:

        //! name
        void setName( const std::string name )
        { _name = name; }

        //! comments
        void setComments( const std::string comments )
        { _comments = comments; }

        //! icon name
        void setIconName( const std::string iconName )
        { _iconName = iconName; }

        //! assign main widget
        virtual void setWidget( GtkWidget *widget )
        { _widget = widget; }

        //! realize widget
        void realize( void );

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

        //! page name
        std::string _name;

        //! page comments
        std::string _comments;

        //! icon name
        std::string _iconName;

        //! main widget
        GtkWidget* _mainWidget;

        //! contents widget
        GtkWidget* _widget;

        //! enable state
        bool _enabled;

    };

}

#endif
