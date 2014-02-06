/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

    //___________________________________________________________________________________________________________
    bool WidgetSizeData::updateMask()
    {

        /* check widget type, store window and offset */
        GdkWindow* window(0);
        int verticalMaskOffset(0);
        if( GTK_IS_MENU( _target ) )
        {

            window = gtk_widget_get_parent_window( _target );
            verticalMaskOffset=Oxygen::Menu_VerticalOffset;

        } else if(
            Gtk::gtk_is_tooltip( _target ) ||
            Gtk::gtk_combobox_is_popup( _target ) ||
            Gtk::gtk_combo_is_popup( _target ) ) {

            window=gtk_widget_get_window( _target );

        } else {

            std::cerr << "FIXME: Oxygen::WidgetSizeData: unknown window type: \""<< Gtk::gtk_widget_path( _target )<<"\"\n";
            return false;

        }

        const bool alpha( Gtk::gtk_widget_has_rgba( _target ) );
        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( _target ) );
        const int &width(allocation.width);
        const int &height(allocation.height);

        // check modifications
        const bool sizeChanged( width != _width || height != _height );
        const bool alphaChanged( alpha != _alpha );
        if( !( sizeChanged || alphaChanged ) ) return false;

        if(!alpha)
        {

            // make menus/tooltips/combo lists appear rounded using XShape extension if screen isn't composited
            GdkPixmap* mask( Style::instance().helper().roundMask( width, height - 2*verticalMaskOffset) );
            gdk_window_shape_combine_mask( window, mask, 0, verticalMaskOffset );
            gdk_pixmap_unref(mask);

        } else {

            // reset mask if compositing has appeared after we had set a mask
            gdk_window_shape_combine_mask( window, NULL, 0, 0);

        }

        // adjust blur region
        if( sizeChanged && alpha &&
            ( Gtk::gtk_is_tooltip( _target ) ||
            ( Style::instance().settings().backgroundOpacity() < 255 && GTK_IS_MENU( _target ) ) ) )
        { Style::instance().setWindowBlur(window,true); }

        // Update widget properties
        _width=width;
        _height=height;
        _alpha=alpha;

        return sizeChanged;
    }

}
