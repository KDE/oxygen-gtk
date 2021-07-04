#ifndef oxygenstyleoptions_h
#define oxygenstyleoptions_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
        Active = 1<<2,
        Flat = 1<<3,
        Focus = 1<<4,
        Hover = 1<<5,
        NoFill = 1<<6,
        Vertical = 1<<7,
        Alpha = 1<<8,
        Round = 1<<9,
        Contrast = 1<<10,
        Selected = 1<<11,
        Disabled = 1<<12,
        Menu = 1<<13,
        DrawAlphaChannel = 1<<14
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
            else if( state == GTK_STATE_ACTIVE ) (*this) |= Active;

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
