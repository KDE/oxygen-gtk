/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygeninfowidget.h"
#include "oxygenversion.h"
#include "config.h"

#include <iostream>
#include <string>
#include <sstream>

namespace Oxygen
{

    //____________________________________________________
    InfoWidget::InfoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 5 );
        setWidget( mainWidget );

        // setup
        setName( "About" );
        setComments( "Shows information about this application" );
        setIconName( "dialog-information" );
        realize();

        // about information
        {
            GtkWidget* frame( gtk_frame_new( "About" ) );
            gtk_box_pack_start( GTK_BOX( mainWidget ), frame, false, true, 0 );
            gtk_widget_show( frame );

            // vbox
            GtkWidget* box( gtk_vbox_new( false, 0 ) );
            gtk_box_set_spacing( GTK_BOX( box ), 5 );
            gtk_container_add( GTK_CONTAINER( frame ), box );
            gtk_container_set_border_width( GTK_CONTAINER( box ), 10 );
            gtk_widget_show( box );

            // application name
            GtkWidget* label;
            label = boldLabel();
            gtk_label_set_text( GTK_LABEL( label ), "oxygen-gtk-demo" );
            gtk_label_set_selectable( GTK_LABEL( label ), true );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            // version
            {
                std::ostringstream stream;
                stream << "version " << OXYGEN_VERSION;
                label = boldLabel();
                gtk_label_set_selectable( GTK_LABEL( label ), true );
                gtk_label_set_text( GTK_LABEL( label ), stream.str().c_str() );
                gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
                gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
                gtk_widget_show( label );
            }

            // gtk version
            {
                std::ostringstream stream;
                stream << "using gtk version " << Version::gtkVersion();
                gtk_label_set_selectable( GTK_LABEL( label ), true );
                label = gtk_label_new( stream.str().c_str() );
                gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
                gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
                gtk_widget_show( label );
            }

            // library version
            const std::string libVersion( Version::libVersion() );
            if( !libVersion.empty() )
            {
                std::ostringstream stream;
                stream << "using oxygen-gtk version " << libVersion;
                gtk_label_set_selectable( GTK_LABEL( label ), true );
                label = gtk_label_new( stream.str().c_str() );
                gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
                gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
                gtk_widget_show( label );
            }

            label = gtk_label_new( "" );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            // description
            label = gtk_label_new( "Gtk+ widgets preview for Oxygen" );
            gtk_label_set_selectable( GTK_LABEL( label ), true );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( "" );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( "(C) 2010 Hugo Pereira Da Costa, Ruslan Kabatsayev, Cédric Bellegarde" );
            gtk_label_set_selectable( GTK_LABEL( label ), true );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( "" );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( 0L );
            gtk_label_set_markup( GTK_LABEL( label ), "<a href=\"https://projects.kde.org/projects/playground/artwork/oxygen-gtk\">https://projects.kde.org/projects/playground/artwork/oxygen-gtk</a>" );
            gtk_label_set_selectable( GTK_LABEL( label ), true );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( 0L );
            gtk_label_set_markup( GTK_LABEL( label ), "<a href=\"http://kde-look.org/content/show.php/?content=136216\">http://kde-look.org/content/show.php/?content=136216</a>" );
            gtk_label_set_selectable( GTK_LABEL( label ), true );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( "" );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( 0L );
            gtk_label_set_selectable( GTK_LABEL( label ), true );
            gtk_label_set_markup( GTK_LABEL( label ), "License: <a href=\"http://www.gnu.org/licenses/lgpl-2.1.html\">GNU Lesser General Public License Version 2</a>" );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );


        }

        // authors information
        {
            GtkWidget* frame( gtk_frame_new( "Authors" ) );
            gtk_box_pack_start( GTK_BOX( mainWidget ), frame, false, true, 0 );
            gtk_widget_show( frame );

            // vbox
            GtkWidget* box( gtk_vbox_new( false, 0 ) );
            gtk_box_set_spacing( GTK_BOX( box ), 5 );
            gtk_container_add( GTK_CONTAINER( frame ), box );
            gtk_container_set_border_width( GTK_CONTAINER( box ), 10 );
            gtk_widget_show( box );

            GtkWidget* label = gtk_label_new( 0L );
            gtk_label_set_markup( GTK_LABEL( label ), "Please use <a href=\"https://bugs.kde.org\">bugs.kde.org</a> to report bugs" );
            gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            label = gtk_label_new( "" );
            gtk_box_pack_start( GTK_BOX( box ), label, true, true, 0 );
            gtk_widget_show( label );

            // table
            GtkWidget* table( gtk_table_new( 3, 3, false ) );
            gtk_box_pack_start( GTK_BOX( box ), table, false, false, 0 );
            gtk_container_set_border_width( GTK_CONTAINER( table ), 0 );
            gtk_table_set_row_spacings( GTK_TABLE( table ), 5 );
            gtk_table_set_col_spacings( GTK_TABLE( table ), 5 );
            gtk_widget_show( table );

            // spacing
            GtkWidget* spacing( gtk_label_new( "" ) );
            gtk_table_attach( GTK_TABLE( table ), spacing, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 2, 0  );
            gtk_widget_show( spacing );

            addAuthor( table, 0, "<a href=\"mailto:hugo.pereira@free.fr\">Hugo Pereira Da Costa</a>", "Maintainer and developer" );
            addAuthor( table, 1, "<a href=\"mailto:b7.10110111@gmail.com\">Ruslan Kabatsayev</a>", "Maintainer and developer" );
            addAuthor( table, 2, "<a href=\"mailto:cedric.bellegarde@ensat.fr\">Cédric Bellegarde</a>", "Developer" );

        }

    }

    //____________________________________________________
    InfoWidget::~InfoWidget( void )
    {}

    //____________________________________________________
    GtkWidget* InfoWidget::boldLabel( void ) const
    {
        GtkWidget* label = gtk_label_new( 0L );

        // create pango attributes list
        PangoAttrList* attributes( pango_attr_list_new() );
        pango_attr_list_insert( attributes, pango_attr_weight_new( PANGO_WEIGHT_BOLD ) );

        GValue val = { 0, };
        g_value_init(&val, PANGO_TYPE_ATTR_LIST );
        g_value_set_boxed( &val, attributes );
        g_object_set_property( G_OBJECT( label ), "attributes", &val );

        pango_attr_list_unref( attributes );

        return label;
    }

    //____________________________________________________
    void InfoWidget::addAuthor( GtkWidget* table, int row, const std::string& author, const std::string& comment ) const
    {

        GtkWidget* label;
        label = gtk_label_new( 0L );
        gtk_label_set_markup( GTK_LABEL( label ), author.c_str() );
        gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
        gtk_table_attach( GTK_TABLE( table ), label, 0, 1, row, row+1, GTK_FILL, GTK_FILL, 0, 0 );
        gtk_widget_show( label );

        label = gtk_label_new( 0L );
        gtk_label_set_markup( GTK_LABEL( label ), comment.c_str() );
        gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
        gtk_table_attach( GTK_TABLE( table ), label, 1, 2, row, row+1, GTK_FILL, GTK_FILL, 0, 0 );
        gtk_widget_show( label );

        return;

    }

}
