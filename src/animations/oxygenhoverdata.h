#ifndef oxygenhoverdata_h
#define oxygenhoverdata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignal.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    class HoverData
    {

        public:

        //! constructor
        HoverData( void ):
            _hovered( false ),
            _updateOnHover( false )
        {}

        //! destructor
        virtual ~HoverData( void )
        { disconnect( 0L ); }

        //! setup connections
        virtual void connect( GtkWidget* );

        //! disconnect
        virtual void disconnect( GtkWidget* );

        //! true if hovered
        virtual bool hovered( void ) const
        { return _hovered; }

        //! true if hover state change should trigger widget repaint
        void setUpdateOnHover( bool value )
        { _updateOnHover = value; }

        //! set mouse over state
        virtual bool setHovered( GtkWidget* widget, bool value )
        {
            if( _hovered == value ) return false;
            _hovered = value;

            // schedule repaint
            if( _updateOnHover )
            { gtk_widget_queue_draw( widget ); }

            return true;
        }

        protected:

        //!@name static callbacks
        //@{
        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        //@}

        private:

        //! enter signal id
        Signal _enterId;

        //! leave signal id
        Signal _leaveId;

        //! true if hovered
        bool _hovered;

        //! also update
        bool _updateOnHover;

    };

}

#endif
