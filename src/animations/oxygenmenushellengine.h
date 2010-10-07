#ifndef oxygenmenushellengine_h
#define oxygenmenushellengine_h
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
#include "oxygenmenushelldata.h"

#include <gtk/gtkwidget.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboboxes
    /*!
    ensures that the text entry and the button of editable comboboxes
    gets hovered and focus flags at the same time
    */
    class MenuShellEngine: public BaseEngine
    {

        public:

        //! constructor
        MenuShellEngine( Animations* parent ):
            BaseEngine( parent )
            {}

        //! destructor
        virtual ~MenuShellEngine( void )
        {}

        //! register widget
        void registerWidget( GtkWidget* widget )
        {

            if( _data.contains( widget ) ) return;
            MenuShellData& data( _data.registerWidget( widget ) );
            data.connect( widget );
            BaseEngine::registerWidget( widget );

        }

        //! unregister widget
        void unregisterWidget( GtkWidget* widget )
        {
            if( !_data.contains( widget ) ) return;
            _data.value( widget ).disconnect( widget );
            _data.erase( widget );
        }

        private:

        //! map widgets to MenuShellData
        DataMap<MenuShellData> _data;

    };

}

#endif
