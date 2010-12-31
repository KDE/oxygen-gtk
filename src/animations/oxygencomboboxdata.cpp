/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygencomboboxdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"
#include "../oxygenstyle.h"

#include <gtk/gtk.h>
#include <iostream>
#include <cassert>
#include <algorithm>
namespace Oxygen
{

    //________________________________________________________________________________
    void ComboBoxData::connect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ComboBoxData::connect - widget: " << widget << std::endl;
        #endif

        _target = widget;
        _stateChangeId.connect( G_OBJECT(widget), "state-changed", G_CALLBACK( stateChangeEvent ), this );
        initializeCellView( widget );

    }

    //________________________________________________________________________________
    void ComboBoxData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ComboBoxData::disconnect - widget: " << widget << std::endl;
        #endif

        _stateChangeId.disconnect();

        _target = 0L;
        _button.disconnect();
        _cell.disconnect();

        // disconnect all children
        for( HoverDataMap::iterator iter = _hoverData.begin(); iter != _hoverData.end(); ++iter )
        { iter->second.disconnect(); }

        _hoverData.clear();

    }

    //________________________________________________________________________________
    void ComboBoxData::setButton( GtkWidget* widget )
    {
        if( _button._widget == widget ) return;
        assert( !_button._widget );

        _button._toggledId.connect( G_OBJECT(widget), "toggled", G_CALLBACK( childToggledEvent ), this );
        _button._widget = widget;

        registerChild( widget, false );

    }

    //________________________________________________________________________________
    void ComboBoxData::initializeCellView( GtkWidget* widget )
    {

        GList* children( gtk_container_get_children( GTK_CONTAINER( widget ) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !GTK_IS_CELL_VIEW( child->data ) ) continue;

            // convert to widget and store
            GtkWidget* widget( GTK_WIDGET( child->data ) );
            if( _cell._widget == widget ) return;
            assert( !_cell._widget );

            _cell._widget = GTK_WIDGET( child->data );
            _cell._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            _cell._styleChangeId.connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );

            updateCellViewColor();

        }

        if( children ) g_list_free( children );
        return;

    }
    //________________________________________________________________________________
    void ComboBoxData::updateCellViewColor( void )
    {
        // change background color
        if( _cell._widget )
        { gtk_cell_view_set_background_color( GTK_CELL_VIEW( _cell._widget ), 0L ); }
    }

    //________________________________________________________________________________
    void ComboBoxData::initializeCellLayout( void )
    {

        if( _cellLayoutInitialized ) return;
        if( !_cell._widget ) return;

        GtkTreePath* path( gtk_cell_view_get_displayed_row( GTK_CELL_VIEW( _cell._widget ) ) );
        if( !path ) return;

        gtk_cell_view_set_background_color( GTK_CELL_VIEW( _cell._widget ), 0L );

        GtkCellLayout* layout( GTK_CELL_LAYOUT( _cell._widget ) );
        GList* renderers( gtk_cell_layout_get_cells( layout ) );

        // get maximum padding
        unsigned short ypadmax(0);
        for( GList* renderer = g_list_first(renderers); renderer; renderer = g_list_next(renderer) )
        { ypadmax = std::max<unsigned short>( ypadmax, GTK_CELL_RENDERER( renderer->data )->ypad ); }

        // assign to all renderers
        for( GList* renderer = g_list_first(renderers); renderer; renderer = g_list_next(renderer) )
        {
            GtkCellRenderer* r( GTK_CELL_RENDERER( renderer->data ) );

            int xpad, ypad;
            gtk_cell_renderer_get_padding( r, &xpad, &ypad );

            int xsize, ysize;
            gtk_cell_renderer_get_size( r, _cell._widget, 0L, 0L, 0L, &xsize, &ysize );

            gtk_cell_renderer_set_padding( r, std::max( 6, xpad ), ypadmax );
            gtk_cell_renderer_set_fixed_size( r, xsize+6, ysize + ypadmax );
        }

        if( renderers ) g_list_free( renderers );

        // need to trigger model changed
        GtkTreeModel* model = gtk_combo_box_get_model( GTK_COMBO_BOX( _target ) );
        GtkTreeIter iter;
        gtk_combo_box_get_active_iter( GTK_COMBO_BOX( _target ), &iter );
        gtk_tree_model_row_changed( model, path, &iter );
        gtk_tree_path_free( path );

        // mark as initialized
        _cellLayoutInitialized = true;

    }

    //________________________________________________________________________________
    void ComboBoxData::setPressed( GtkWidget* widget, bool value )
    {
        const bool oldPressed( pressed() );
        if( widget == _button._widget ) _button._pressed = value;
        else return;

        if( oldPressed != pressed() && _target ) gtk_widget_queue_draw( _target );

        // save the combobox as currently popped up if pressed, and none as popped up if not
        if( pressed() )
            Style::instance().animations().comboBoxEngine().setPoppedUpWidget(widget);
        else
            Style::instance().animations().comboBoxEngine().setPoppedUpWidget(NULL);

    }

    //________________________________________________________________________________
    void ComboBoxData::setHovered( GtkWidget* widget, bool value )
    {

        bool oldHover( hovered() );
        HoverDataMap::iterator iter( _hoverData.find( widget ) );
        if( iter != _hoverData.end() ) iter->second._hovered = value;
        else return;

        // need to schedule repaint of the whole widget
        if( oldHover != hovered() && _target ) gtk_widget_queue_draw( _target );

    }

    //________________________________________________________________________________
    void ComboBoxData::registerChild( GtkWidget* widget, bool recursive )
    {

        // make sure widget is not already in map
        if( _hoverData.find( widget ) == _hoverData.end() )
        {

            #if OXYGEN_DEBUG
            std::cout
                << "Oxygen::ComboBoxData::registerChild -"
                << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
            #endif

            // allocate new Hover data
            HoverData data;
            data._widget = widget;
            data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._styleChangeId.connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
            data._enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
            data._leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );

            // and insert in map
            _hoverData.insert( std::make_pair( widget, data ) );

        }

        /*
        also insert widget's children, recursively.
        that should take care of buttons in tabs and other fancy stuff that applications mght do
        */
        if( recursive && GTK_IS_CONTAINER( widget ) )
        {

            GList *children( gtk_container_get_children( GTK_CONTAINER(widget) ) );
            for( GList* child = g_list_first(children); child; child = g_list_next(child) )
            { registerChild( GTK_WIDGET( child->data ) ); }

            if( children ) g_list_free( children );
        }

    }

    //________________________________________________________________________________
    void ComboBoxData::unregisterChild( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cout
            << "Oxygen::ComboBoxData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        // see if widget is button or cell
        if( widget == _button._widget ) _button.disconnect();
        if( widget == _cell._widget ) _cell.disconnect();

        // loopup in hover map
        HoverDataMap::iterator iter( _hoverData.find( widget ) );
        if( iter == _hoverData.end() ) return;

        iter->second.disconnect();
        _hoverData.erase( iter );

    }

    //________________________________________________________________________________
    void ComboBoxData::ChildData::disconnect( void )
    {

        if( !_widget ) return;

        #if OXYGEN_DEBUG
        std::cout
            << "Oxygen::ComboBoxData::ChildData::disconnect -"
            << " " << _widget << " (" << G_OBJECT_TYPE_NAME( _widget ) << ")"
            << std::endl;
        #endif

        _destroyId.disconnect();
        _styleChangeId.disconnect();
        _widget = 0L;
    }

    //________________________________________________________________________________
    void ComboBoxData::ButtonData::disconnect( void )
    {
        if( !_widget ) return;
        _toggledId.disconnect();
        _pressed = false;
        _focus = false;

        // base class
        ChildData::disconnect();
    }

    //________________________________________________________________________________
    void ComboBoxData::HoverData::disconnect( void )
    {
        if( !_widget ) return;

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ComboBoxData::HoverData::disconnect -"
            << " " << _widget << " (" << G_OBJECT_TYPE_NAME( _widget ) << ")"
            << std::endl;
        #endif

        _enterId.disconnect();
        _leaveId.disconnect();
        _hovered = false;

        // base class
        ChildData::disconnect();
    }

    //____________________________________________________________________________________________
    gboolean ComboBoxData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cout
            << "Oxygen::ComboBoxData::childDestroyNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<ComboBoxData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void ComboBoxData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<ComboBoxData*>(data)->unregisterChild( widget ); }

    //____________________________________________________________________________________________
    void ComboBoxData::childToggledEvent( GtkWidget* widget, gpointer data)
    {
        if( GTK_IS_TOGGLE_BUTTON( widget ) )
        { static_cast<ComboBoxData*>(data)->setPressed( widget, gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) ); }
        return;
    }

    //________________________________________________________________________________
    gboolean ComboBoxData::enterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ComboBoxData::enterNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ComboBoxData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ComboBoxData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ComboBoxData::leaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ComboBoxData*>( data )->setHovered( widget, false );
        return FALSE;
    }

    //________________________________________________________________________________
    void ComboBoxData::stateChangeEvent( GtkWidget*, GtkStateType, gpointer data )
    { static_cast<ComboBoxData*>( data )->updateCellViewColor(); }

}
