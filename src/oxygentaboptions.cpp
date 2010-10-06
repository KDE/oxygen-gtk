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

#include "oxygentaboptions.h"
#include "oxygenstyleoptions.h"

#include <gtk/gtk.h>
#include <cmath>
namespace Oxygen
{

    TabOptions tabOptions( GtkWidget* widget, GtkStateType state, int x, int y )
    {

        TabOptions out( None );
        if( state != GTK_STATE_ACTIVE ) out |= CurrentTab;

        // try detect if tab is first or last
        if( GTK_IS_NOTEBOOK( widget ) )
        {

            GtkNotebook* notebook = GTK_NOTEBOOK( widget );

            int tabIndex;
            int minDistance( -1 );
            for( int i = 0; i < gtk_notebook_get_n_pages( notebook ); i++ )
            {

                // retrieve page and tab label
                GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
                GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );

                // get allocted size
                GtkAllocation allocation;
                gtk_widget_get_allocation( tabLabel, &allocation );

                int distance = std::abs<int>( allocation.x - x ) + std::abs<int>( allocation.y - y );
                if( minDistance < 0 || distance < minDistance )
                {
                    minDistance = distance;
                    tabIndex = i;
                }
            }

            if( tabIndex == 0 ) out |= FirstTab;
            else if( tabIndex == gtk_notebook_get_n_pages( notebook ) - 1 ) out |= LastTab;

        }

        return out;
    }


}


