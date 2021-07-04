#ifndef oxygenscrollbarengine_h
#define oxygenscrollbarengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenscrollbardata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable scrollbares
    /*!
    ensures that the text entry and the button of editable scrollbares
    gets hovered and focus flags at the same time
    */
    class ScrollBarEngine: public GenericEngine<ScrollBarData>
    {

        public:

        //! constructor
        ScrollBarEngine( Animations* widget ):
            GenericEngine<ScrollBarData>( widget )
            {}

        //! destructor
        virtual ~ScrollBarEngine( void )
        {}

        //! register scrollbar from scrolled window
        inline void registerScrolledWindow( GtkWidget* );

    };

    //________________________________________________________________
    void ScrollBarEngine::registerScrolledWindow( GtkWidget* widget )
    {
        if( !GTK_IS_SCROLLED_WINDOW( widget ) ) return;
        GtkScrolledWindow* scrolledWindow( GTK_SCROLLED_WINDOW( widget ) );

        if( GtkWidget* hScrollBar = gtk_scrolled_window_get_hscrollbar( scrolledWindow ) )
        { registerWidget( hScrollBar ); }

        if( GtkWidget* vScrollBar = gtk_scrolled_window_get_vscrollbar( scrolledWindow ) )
        { registerWidget( vScrollBar ); }

        return;
    }

}

#endif
