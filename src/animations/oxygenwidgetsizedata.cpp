/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygenwidgetsizedata.h"
#include "../oxygenstyle.h"
#include "../oxygengtkutils.h"

namespace Oxygen
{
    bool WidgetSizeData::updateXShape()
    {
        GtkWidget* widget(_widget);
        GdkWindow* window(0);
        int verticalMaskOffset(0);

        if(GTK_IS_MENU(widget))
        {

            window = gtk_widget_get_parent_window(widget);
            verticalMaskOffset=Oxygen::Menu_VerticalOffset;

        } else if(Gtk::gtk_is_tooltip(widget)) {

            window = gtk_widget_get_window(widget);

        } else if(Gtk::gtk_combobox_is_popup(widget)) {

            window=gtk_widget_get_window(widget);

        } else if(Gtk::gtk_combo_is_popup(widget)) {

            window=gtk_widget_get_window(widget);

        } else {

            std::cerr << "FIXME: Oxygen::WidgetSizeData: unknown window type: \""<< Gtk::gtk_widget_path(widget)<<"\"\n";
            return false;

        }

        const bool wasAlpha(_alpha);
        const bool alpha(Gtk::gtk_widget_has_rgba( widget ));

        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
        const int &width(allocation.width), &height(allocation.height);

        const bool sizeChanged( width != _width || height != _height );
        const bool alphaChanged( alpha != wasAlpha );

        if( !sizeChanged && !alphaChanged ) return false;

        if(!alpha)
        {

            // make menus/tooltips/combo lists appear rounded using XShape extension if screen isn't composited
            GdkPixmap* mask( Style::instance().helper().roundMask( width, height - 2*verticalMaskOffset) );
            gdk_window_shape_combine_mask( window, mask, 0, verticalMaskOffset );
            gdk_pixmap_unref(mask);

        } else {

            // reset mask if compositing has appeared after we had set a mask
            gdk_window_shape_combine_mask( window, NULL, 0, 0);

            // TODO: move to size allocation hook
            if( Gtk::gtk_is_tooltip(widget) && ( sizeChanged || !wasAlpha ) )
            {
                Style::instance().setWindowBlur(window,true);
            }

            if( Style::instance().settings().backgroundOpacity() < 255 )
            {

                if(GTK_IS_MENU(widget)) Style::instance().setWindowBlur(window,true);

            }

        }

        // Update widget properties
        _width=width;
        _height=height;
        _alpha=alpha;

        return sizeChanged;
    }

}
