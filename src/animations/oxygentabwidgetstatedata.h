#ifndef oxygentabwidgetstatedata_h
#define oxygentabwidgetstatedata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygentimeline.h"
#include "../oxygenanimationdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! track arrow state changes events,
    /*! used to deal with spinbox and Notebook arrows */
    class TabWidgetStateData
    {

        public:

        //! constructor
        explicit TabWidgetStateData( void ):
            _target( 0L )
        {}

        //! destructor
        virtual ~TabWidgetStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! update state
        bool updateState( int, bool );

        //! enable state
        void setEnabled( bool value )
        {
            _current._timeLine.setEnabled( value );
            _previous._timeLine.setEnabled( value );
        }

        //! duration
        void setDuration( int value )
        {
            _current._timeLine.setDuration( value );
            _previous._timeLine.setDuration( value );
        }

        //! true if tab index is animated
        bool isAnimated( int index ) const
        {
            if( index == _current._index ) return _current._timeLine.isRunning();
            else if( index == _previous._index ) return _previous._timeLine.isRunning();
            else return false;
        }

        //! opacity
        double opacity( int index ) const
        {
            if( index == _current._index ) return _current._timeLine.value();
            else if( index == _previous._index ) return _previous._timeLine.value();
            else return OpacityInvalid;
        }

        protected:

        //! return dirty rect (for update)
        GdkRectangle dirtyRect( void ) const;

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! invalid index
        static const int IndexInvalid;

        //! tab data
        class Data
        {

            public:

            //! constructor
            explicit Data( void ):
                _index( IndexInvalid )
            {}

            //! timeline
            TimeLine _timeLine;

            //! tab index
            int _index;

        };

        //! target
        GtkWidget* _target;

        //! current tab data
        Data _current;

        //! previous tab data
        Data _previous;

    };

}

#endif
