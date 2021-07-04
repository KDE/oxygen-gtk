#ifndef oxygendemowidget_h
#define oxygendemowidget_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignalhandler.h"

#include <gtk/gtk.h>
#include <cassert>
#include <string>

namespace Oxygen
{

    class DemoWidget: public SignalHandler
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
