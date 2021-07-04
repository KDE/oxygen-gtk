#ifndef oxygeninnershadowdata_h
#define oxygeninnershadowdata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <algorithm>
#include <map>

namespace Oxygen
{
    //! handle inner shadow drawing using composited GdkWindow API
    class InnerShadowData
    {

        public:

        //! constructor
        InnerShadowData( void ):
            _target(0)
        {}

        //! destructor
        virtual ~InnerShadowData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! register child
        /*! needed to restore composited state on disconnect */
        void registerChild( GtkWidget* );

        protected:

        //! unregister child
        void unregisterChild( GtkWidget* );

        //!@ callbacks
        //@{
        static gboolean childUnrealizeNotifyEvent( GtkWidget*, gpointer );
        static gboolean targetExposeEvent( GtkWidget*, GdkEventExpose*, gpointer );
        //@}

        private:

        //! target widget
        GtkWidget* _target;

        //! expose
        Signal _exposeId;

        //! child data
        class ChildData
        {
            public:

            //! constructor
            ChildData( void ):
                _initiallyComposited( false )
            {}

            //! destructor
            virtual ~ChildData( void )
            {}

            //! disconnect all signals
            void disconnect( GtkWidget* );

            //! signal
            Signal _unrealizeId;

            //! keep track of composited state
            bool _initiallyComposited;
        };

        //! map registered children and corresponding data
        typedef std::map<GtkWidget*, ChildData> ChildDataMap;
        ChildDataMap _childrenData;

    };

}

#endif
