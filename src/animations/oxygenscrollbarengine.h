#ifndef oxygenscrollbarengine_h
#define oxygenscrollbarengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
