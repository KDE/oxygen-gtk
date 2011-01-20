/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* the optionmenu data code is largely inspired from the gtk redmond engine
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

#include "oxygenoptionmenudata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>
#include <cassert>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void OptionMenuData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::OptionMenuData::connect - " << widget << std::endl;
        #endif

        _childAddedId.connect( G_OBJECT(widget), "add", G_CALLBACK( childAddedEvent ), this );

    }

    //________________________________________________________________________________
    void OptionMenuData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::OptionMenuData::disconnect - " << widget << std::endl;
        #endif

        _childAddedId.disconnect();

        // disconnect all children
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); ++iter )
        { iter->second.disconnect(); }
        _childrenData.clear();

    }

    //________________________________________________________________________________
    void OptionMenuData::registerChild( GtkWidget* widget )
    {

        // make sure widget is not already in map
        if( _childrenData.find( widget ) == _childrenData.end() )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::OptionMenuData::registerChild - " << widget << std::endl;
            #endif

            // update style
            updateChildStyle( widget );

            // allocate new ChildData
            ChildData data;
            data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._parentSetId.connect( G_OBJECT(widget), "parent-set", G_CALLBACK( parentSetEvent ), this );

            // and insert in map
            _childrenData.insert( std::make_pair( widget, data ) );

        }

    }

    //________________________________________________________________________________
    void OptionMenuData::unregisterChild( GtkWidget* widget )
    {

        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::OptionMenuData::unregisterChild - " << widget << std::endl;
        #endif

        iter->second.disconnect();
        _childrenData.erase( iter );

    }

    //____________________________________________________________________________________________
    void OptionMenuData::updateChildStyle( GtkWidget* widget ) const
    {

        // need to make sure the child's style is attached to the correct window
        GtkStyle* style( gtk_widget_get_style( widget ) );
        if( !style ) return;

        GdkWindow* window( gtk_widget_get_parent_window( widget ) );
        if( !window ) return;

        if( style->depth != gdk_drawable_get_depth( window ) )
        { widget->style = gtk_style_attach( style, window ); }

    }

    //____________________________________________________________________________________________
    void OptionMenuData::childAddedEvent( GtkContainer* parent, GtkWidget* child, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::OptionMenuData::childAddedEvent - child: " << child << std::endl;
        #endif

        static_cast<OptionMenuData*>(data)->registerChild( child );
        return;
    }

    //____________________________________________________________________________________________
    gboolean OptionMenuData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::OptionMenuData::childDestroyNotifyEvent - " << widget << std::endl;
        #endif

        static_cast<OptionMenuData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void OptionMenuData::parentSetEvent( GtkWidget* widget, GtkObject*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::OptionMenuData::parentSetEvent - " << widget << std::endl;
        #endif

        static_cast<OptionMenuData*>(data)->updateChildStyle( widget );

    }

    //____________________________________________________________________________________________
    void OptionMenuData::ChildData::disconnect( void )
    {

        _destroyId.disconnect();
        _parentSetId.disconnect();

    }

}
