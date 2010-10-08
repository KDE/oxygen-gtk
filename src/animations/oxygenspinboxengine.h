#ifndef oxygenspinboxengine_h
#define oxygenspinboxengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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


#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenspinboxdata.h"

#include <gtk/gtkwidget.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable spinboxes
    /*!
    ensures that the text entry and the button of editable spinboxes
    gets hovered and focus flags at the same time
    */
    class SpinBoxEngine: public BaseEngine
    {

        public:

        //! constructor
        SpinBoxEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~SpinBoxEngine( void )
        {}

        //! register widget
        void registerWidget( GtkWidget* widget )
        {
            if( _data.contains( widget ) ) return;
            _data.registerWidget( widget );
            BaseEngine::registerWidget( widget );
        }

        //! unregister widget
        void unregisterWidget( GtkWidget* widget )
        { _data.erase( widget ); }

        //! true if widget is included
        bool contains( GtkWidget* widget )
        { return _data.contains( widget ); }

        //! update mouseoverstate
        void updateMouseOver( GtkWidget* widget )
        { _data.value( widget ).updateMouseOver( widget ); }

        //! true if comboBox is hovered
        bool hovered( GtkWidget* widget )
        { return _data.value( widget ).hovered(); }

        private:

        //! map widgets to SpinBoxData
        DataMap<SpinBoxData> _data;

    };

}

#endif
