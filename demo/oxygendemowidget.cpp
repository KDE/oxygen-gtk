
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

#include "oxygendemowidget.h"

namespace Oxygen
{

    //_______________________________________________________
    void DemoWidget::realize( void )
    {
        assert( !_mainWidget );
        assert( _widget );

        // create main widget (vbox)
        _mainWidget = gtk_vbox_new( false, 0 );
        gtk_box_set_spacing( GTK_BOX( _mainWidget ), 10 );
        gtk_widget_show( _mainWidget );

        // horizontal box
        GtkWidget* hbox( gtk_hbox_new( false, 0 ) );
        gtk_box_pack_start( GTK_BOX( _mainWidget ), hbox, false, true, 0 );
        gtk_widget_show( hbox );

        // label
        std::string comments( _comments.empty() ? _name:_comments );
        GtkWidget* label( gtk_label_new( comments.c_str() ) );

        PangoAttrList* attributes( pango_attr_list_new() );
        pango_attr_list_insert( attributes, pango_attr_weight_new( PANGO_WEIGHT_BOLD ) );
        gtk_label_set_attributes( GTK_LABEL( label ), attributes );
        pango_attr_list_unref( attributes );

        gtk_box_pack_start( GTK_BOX( hbox ), label, false, true, 0 );

        gtk_widget_show( label );

        // icon
        if( !_iconName.empty() )
        {
            GtkIconTheme* theme( gtk_icon_theme_get_default() );
            GdkPixbuf* icon( gtk_icon_theme_load_icon( theme, _iconName.c_str(), 22, (GtkIconLookupFlags) 0, 0L ) );
            GtkWidget* image( gtk_image_new_from_pixbuf( icon ) );
            gtk_box_pack_end( GTK_BOX( hbox ), image, false, false, 0 );
            gtk_widget_show( image );

        }

        // main content
        gtk_box_pack_start( GTK_BOX( _mainWidget ), _widget, true, true, 0 );
        gtk_widget_show( _widget );
    }
}
