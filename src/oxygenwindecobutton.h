#ifndef oxygenwindecobutton_h
#define oxygenwindecobutton_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygencairocontext.h"
namespace Oxygen
{

    // forward declarations
    class QtSettings;
    class StyleHelper;

    namespace WinDeco
    {

        //! button status
        enum ButtonStatus {
            Normal,
            Disabled, // this shouldn't be specified by WMs unless button is really insensitive
            Hovered,
            Pressed,
            ButtonStatusCount
        };

        //! buttons
        enum ButtonType {
            ButtonHelp=0,
            ButtonMax,
            ButtonMin,
            ButtonClose,
            ButtonMenu,
            ButtonStick,
            ButtonAbove,
            ButtonBelow,
            ButtonShade,
            ButtonUnmax,
            ButtonUnstick,
            ButtonUnshade,
            ButtonUndoAbove,
            ButtonUndoBelow,
            ButtonTypeCount
        };

        //! window decoration button
        class Button
        {

            public:

            //! constructor
            Button( const QtSettings& settings, StyleHelper& helper, const ButtonType& type ):
                _settings( settings ),
                _helper( helper ),
                _type( type ),
                _state( Normal )
            {}

            //! destructor
            virtual ~Button( void )
            {}

            //! state
            void setState( const ButtonStatus& state )
            { _state = state; }

            //! render
            void render( cairo_t*, gint x, gint y, gint w, gint h ) const;

            protected:

            //! icon
            void drawIcon( cairo_t*, gint w, gint h ) const;

            //! settings
            const QtSettings& settings( void ) const
            { return _settings; }

            //! helper
            StyleHelper& helper( void ) const
            { return _helper; }

            private:

            //! settings
            const QtSettings& _settings;

            //! helper
            StyleHelper& _helper;

            //! type
            ButtonType _type;

            //! status
            ButtonStatus _state;

        };


    }

}

#endif
