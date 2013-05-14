#ifndef oxygeninnershadowdata_h
#define oxygeninnershadowdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
