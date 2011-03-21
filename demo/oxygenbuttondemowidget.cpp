/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#include "oxygenbuttondemowidget.h"

#include <string>

namespace Oxygen
{

    //____________________________________________________
    ButtonDemoWidget::ButtonDemoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 5 );
        setWidget( mainWidget );

        // setup
        setName( "Buttons" );
        setComments( "Shows the appearance of buttons" );
        setIconName( "go-jump-locationbar" );
        realize();

        // pushbuttons
        {
            GtkWidget* frame( gtk_frame_new( "Pushbutton" ) );
            gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );
            gtk_box_pack_start( GTK_BOX( mainWidget ), frame, false, true, 0 );
            gtk_widget_show( frame );

            // inner table
            GtkWidget* table = gtk_table_new( 2, 4, false );
            gtk_table_set_row_spacings( GTK_TABLE( table ), 5 );
            gtk_container_add( GTK_CONTAINER( frame ), table );
            gtk_widget_show( table );

            // spacing
            GtkWidget* spacing( gtk_label_new( "" ) );
            gtk_table_attach( GTK_TABLE( table ), spacing, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 2, 0  );
            gtk_widget_show( spacing );

            // generic label
            GtkWidget* label( 0L );

            {
                // text only
                gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Text only: " ), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
                gtk_widget_show( label );

                // button
                GtkWidget* button( gtk_button_new_with_label( "Normal" ) );
                gtk_table_attach( GTK_TABLE( table ), button, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_widget_show( button );

                // combobox model
                GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
                const char* columns[] = {
                    "Small",
                    "Normal",
                    "Large"
                };

                for( unsigned int i=0; i<3; i++ )
                {
                    GtkTreeIter iter;
                    gtk_list_store_append( model, &iter );
                    gtk_list_store_set( model, &iter, 0, columns[i], -1 );
                }

                GtkWidget* comboBox(0L);
                gtk_table_attach( GTK_TABLE( table ), comboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL( model ) ), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 2, 0 );
                gtk_combo_box_set_active( GTK_COMBO_BOX( comboBox ), 0 );
                gtk_widget_show( comboBox );

            }

            {
                // text and icons
                gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Text and icon: " ), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
                gtk_widget_show( label );

                // button
                GtkWidget* button( gtk_button_new_with_label( "Normal" ) );

                GtkIconTheme* theme( gtk_icon_theme_get_default() );
                GdkPixbuf* icon = gtk_icon_theme_load_icon( theme, "oxygen", 16, (GtkIconLookupFlags) 0, 0L );
                GtkWidget* image( gtk_image_new_from_pixbuf( icon ) );
                gtk_button_set_image( GTK_BUTTON( button ), image );
                gtk_widget_show( button );
            }

        }

    }

    //____________________________________________________
    ButtonDemoWidget::~ButtonDemoWidget( void )
    {}

}
