#ifndef oxygenoptionmenudata_h
#define oxygenoptionmenudata_h
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

#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <map>

namespace Oxygen
{

    //! detects and stores tab widget hovered tab
    class OptionMenuData
    {

        public:

        //! constructor
        OptionMenuData( void )
        {}

        //! destructor
        virtual ~OptionMenuData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        //! register child
        void registerChild( GtkWidget* );

        //! unregister child
        void unregisterChild( GtkWidget* );

        //! update child style to adjust colormap depth
        void updateChildStyle( GtkWidget* ) const;

        //!@name callbacks
        //@{

        //! child added callback
        static void childAddedEvent( GtkContainer*, GtkWidget*, gpointer );

        //! child parent has changed
        static void parentSetEvent( GtkWidget*, GtkObject*, gpointer );

        //! child destroyed
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );

        //@}

        private:

        //! callbacks IDs
        Signal _childAddedId;

        //! handles children data
        class ChildData
        {
            public:

            //! constructor
            ChildData( void )
            {}

            //! destructor
            virtual ~ChildData( void )
            {}

            //! disconnect all signals
            void disconnect( void );

            Signal _destroyId;
            Signal _parentSetId;
        };

        //! map registered children and corresponding data
        typedef std::map<GtkWidget*, ChildData> ChildDataMap;
        ChildDataMap _childrenData;


    };

}

#endif
