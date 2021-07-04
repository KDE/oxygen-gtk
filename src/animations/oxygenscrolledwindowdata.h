#ifndef oxygenscrolledwindowdata_h
#define oxygenscrolledwindowdata_h
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

    //! handle hover in scrolledwindows
    class ScrolledWindowData
    {

        public:

        //! constructor
        ScrolledWindowData( void ):
            _target(0)
        {}

        //! destructor
        virtual ~ScrolledWindowData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! true if any child is hovered
        bool hovered( void ) const
        { return std::find_if( _childrenData.begin(), _childrenData.end(), HoveredFTor() ) != _childrenData.end(); }

        //! true if any child has focus
        bool focused( void ) const
        { return std::find_if( _childrenData.begin(), _childrenData.end(), FocusedFTor() ) != _childrenData.end(); }

        //! register child
        /*! needed to track enter/leave events in treeview headers */
        void registerChild( GtkWidget* );

        protected:

        //! set mouse over state
        virtual void setHovered( GtkWidget* widget, bool value );

        //! set focus state
        virtual void setFocused( GtkWidget* widget, bool value );

        //! unregister child
        void unregisterChild( GtkWidget* );

        //!@ callbacks
        //@{
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );
        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static gboolean focusInNotifyEvent( GtkWidget*, GdkEvent*, gpointer );
        static gboolean focusOutNotifyEvent( GtkWidget*, GdkEvent*, gpointer );
        static void childAddedEvent( GtkContainer* parent, GtkWidget*, gpointer );
        //@}

        private:

        //! target widget
        GtkWidget* _target;

        //! child data
        class ChildData
        {
            public:

            //! constructor
            ChildData( void ):
                _hovered( false ),
                _focused( false )
            {}

            //! destructor
            virtual ~ChildData( void )
            {}

            //! disconnect all signals
            void disconnect( GtkWidget* );

            //! true if widget is hovered
            bool _hovered;
            bool _focused;

            Signal _destroyId;
            Signal _enterId;
            Signal _leaveId;
            Signal _focusInId;
            Signal _focusOutId;

        };

        //! need to detect hovered child
        class HoveredFTor
        {
            public:

            bool operator () ( const std::pair<GtkWidget*, ChildData>& dataPair )
            { return dataPair.second._hovered; }

        };

        //! need to detect hovered child
        class FocusedFTor
        {
            public:

            bool operator () ( const std::pair<GtkWidget*, ChildData>& dataPair )
            { return dataPair.second._focused; }

        };

        //! map registered children and corresponding data
        typedef std::map<GtkWidget*, ChildData> ChildDataMap;
        ChildDataMap _childrenData;

    };

}

#endif
