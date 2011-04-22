#ifndef oxygenstyleoptions_h
#define oxygenstyleoptions_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenflags.h"
#include "oxygengtkutils.h"
#include "oxygenpalette.h"

#include <iostream>
#include <gtk/gtk.h>

namespace Oxygen
{

    //! internal option flags to pass arguments around
    enum StyleOption
    {
        Blend = 1<<0,
        Sunken = 1<<1,
        Flat = 1<<2,
        Focus = 1<<3,
        Hover = 1<<4,
        NoFill = 1<<5,
        Vertical = 1<<6,
        Alpha = 1<<7,
        Round = 1<<8,
        Contrast = 1<<9,
        Selected = 1<<10,
        Disabled = 1<<11,
        Menu = 1<<12
    };

    class StyleOptions: public Flags<StyleOption>
    {

        public:

        //! constructor
        StyleOptions( void )
        {}

        //! constructor
        StyleOptions(StyleOption f):
            Flags<StyleOption>( f )
        {}

        //! constructor
        StyleOptions( Flags<StyleOption> f):
            Flags<StyleOption>( f )
        {}

        //! constructor from widget
        StyleOptions( GtkWidget* widget, GtkStateType state, GtkShadowType shadow = GTK_SHADOW_NONE )
        {
            if( state == GTK_STATE_INSENSITIVE ) (*this) |= Disabled;
            else if( state == GTK_STATE_PRELIGHT ) (*this) |= Hover;
            else if( state == GTK_STATE_SELECTED ) (*this) |= Selected;

            if( shadow == GTK_SHADOW_IN ) (*this) |= Sunken;
            if( widget && gtk_widget_has_focus(widget) ) (*this)|=Focus;
        }

        //! destructor
        virtual ~StyleOptions( void )
        {}


        //! color set
        /*! it is used to pass custom colors to painting routines */
        Palette::ColorSet _customColors;

        //! streamer
        friend std::ostream& operator << (std::ostream& out, const StyleOptions& options );

    };

}

OX_DECLARE_OPERATORS_FOR_FLAGS( Oxygen::StyleOptions )

#endif
