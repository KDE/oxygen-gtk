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

#include "oxygengtktypenames.h"

namespace Oxygen
{

    //_______________________________________________
    typedef Gtk::TypeNames::Entry<GtkStateType> StateMap;
    static const StateMap stateMap[] =
    {

        { GTK_STATE_NORMAL, "normal" },
        { GTK_STATE_ACTIVE, "active" },
        { GTK_STATE_PRELIGHT, "prelight" },
        { GTK_STATE_SELECTED, "selected" },
        { GTK_STATE_INSENSITIVE, "insensitive" }

    };

    //___________________________________________________________
    GtkStateType Gtk::TypeNames::matchState( const char*  cssState )
    { return Gtk::TypeNames::Finder<GtkStateType>( stateMap, 5 ).findGtk( cssState, GTK_STATE_NORMAL ); }

    //___________________________________________________________
    const char*  Gtk::TypeNames::state( GtkStateType gtkState )
    { return Gtk::TypeNames::Finder<GtkStateType>( stateMap, 5 ).findCss( gtkState ); }

    //___________________________________________________________
    // Map between shadow types in gtk and css format.
    typedef Gtk::TypeNames::Entry<GtkShadowType> ShadowMap;
    static const ShadowMap shadowMap[] =
    {
        { GTK_SHADOW_NONE, "none" },
        { GTK_SHADOW_IN, "in" },
        { GTK_SHADOW_OUT, "out" },
        { GTK_SHADOW_ETCHED_IN, "etched-in" },
        { GTK_SHADOW_ETCHED_OUT, "etched-out" }
    };

    //___________________________________________________________
    GtkShadowType Gtk::TypeNames::matchShadow( const char*  cssShadow )
    { return Gtk::TypeNames::Finder<GtkShadowType>( shadowMap, 5 ).findGtk( cssShadow, GTK_SHADOW_NONE ); }

    //___________________________________________________________
    const char* Gtk::TypeNames::shadow( GtkShadowType gtkShadow )
    { return Gtk::TypeNames::Finder<GtkShadowType>( shadowMap, 5 ).findCss( gtkShadow ); }

    //___________________________________________________________
    typedef Gtk::TypeNames::Entry<GtkArrowType> ArrowMap;
    static const ArrowMap arrowMap[] =
    {
        { GTK_ARROW_UP, "up" },
        { GTK_ARROW_DOWN, "down" },
        { GTK_ARROW_LEFT, "left" },
        { GTK_ARROW_RIGHT, "right" },
        { GTK_ARROW_NONE, "none" }
    };

    //___________________________________________________________
    GtkArrowType Gtk::TypeNames::matchArrow( const char*  cssArrow )
    { return Gtk::TypeNames::Finder<GtkArrowType>( arrowMap, 5 ).findGtk( cssArrow, GTK_ARROW_NONE ); }

    //___________________________________________________________
    const char* Gtk::TypeNames::arrow( GtkArrowType gtkArrow )
    { return Gtk::TypeNames::Finder<GtkArrowType>( arrowMap, 5 ).findCss( gtkArrow ); }

    //___________________________________________________________
    typedef Gtk::TypeNames::Entry<GtkPositionType> PositionMap;
    static const PositionMap positionMap[] =
    {
        { GTK_POS_LEFT, "left" },
        { GTK_POS_RIGHT, "right" },
        { GTK_POS_TOP, "top" },
        { GTK_POS_BOTTOM, "bottom" },
    };

    //___________________________________________________________
    GtkPositionType Gtk::TypeNames::matchPosition( const char* cssPosition )
    { return Gtk::TypeNames::Finder<GtkPositionType>( positionMap, 4 ).findGtk( cssPosition, GTK_POS_LEFT ); }

    //__________________________________________________________________
    const char* Gtk::TypeNames::position( GtkPositionType gtkPosition )
    { return Gtk::TypeNames::Finder<GtkPositionType>( positionMap, 4 ).findCss( gtkPosition ); }

    //__________________________________________________________________
    typedef Gtk::TypeNames::Entry<GdkWindowEdge> WindowEdgeMap;
    static const WindowEdgeMap windowEdgeMap[] =
    {
        { GDK_WINDOW_EDGE_NORTH, "north" },
        { GDK_WINDOW_EDGE_SOUTH, "south" },
        { GDK_WINDOW_EDGE_WEST, "west" },
        { GDK_WINDOW_EDGE_EAST, "east" },
        { GDK_WINDOW_EDGE_NORTH_WEST, "north-west" },
        { GDK_WINDOW_EDGE_NORTH_EAST, "north-east" },
        { GDK_WINDOW_EDGE_SOUTH_WEST, "south-west" },
        { GDK_WINDOW_EDGE_SOUTH_EAST, "south-east" },
    };

    //__________________________________________________________________
    GdkWindowEdge Gtk::TypeNames::matchWindowEdge( const char*  cssWindowEdge )
    { return Gtk::TypeNames::Finder<GdkWindowEdge>( windowEdgeMap, 8 ).findGtk( cssWindowEdge, GDK_WINDOW_EDGE_SOUTH_EAST ); }

    //__________________________________________________________________
    const char*  Gtk::TypeNames::windowEdge( GdkWindowEdge gdkWindowEdge )
    { return Gtk::TypeNames::Finder<GdkWindowEdge>( windowEdgeMap, 8 ).findCss( gdkWindowEdge ); }

    //__________________________________________________________________
    typedef Gtk::TypeNames::Entry<GdkWindowTypeHint> WindowTypeHintMap;
    static const WindowTypeHintMap windowTypeHintMap[] =
    {
        { GDK_WINDOW_TYPE_HINT_COMBO, "combobox list" },
        { GDK_WINDOW_TYPE_HINT_DESKTOP, "desktop" },
        { GDK_WINDOW_TYPE_HINT_DIALOG, "dialog" },
        { GDK_WINDOW_TYPE_HINT_DND, "drag-and-drop" },
        { GDK_WINDOW_TYPE_HINT_DOCK, "dock" },
        { GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU, "dropdown menu" },
        { GDK_WINDOW_TYPE_HINT_MENU, "menu" },
        { GDK_WINDOW_TYPE_HINT_NORMAL, "normal" },
        { GDK_WINDOW_TYPE_HINT_NOTIFICATION, "notification" },
        { GDK_WINDOW_TYPE_HINT_POPUP_MENU, "popup menu" },
        { GDK_WINDOW_TYPE_HINT_SPLASHSCREEN, "splashscreen" },
        { GDK_WINDOW_TYPE_HINT_TOOLBAR, "toolbar" },
        { GDK_WINDOW_TYPE_HINT_TOOLTIP, "tooltip" },
        { GDK_WINDOW_TYPE_HINT_UTILITY, "utility" }
    };

    //__________________________________________________________________
    GdkWindowTypeHint Gtk::TypeNames::matchWindowTypeHint( const char*  cssWindowTypeHint )
    { return Gtk::TypeNames::Finder<GdkWindowTypeHint>( windowTypeHintMap, 14 ).findGtk( cssWindowTypeHint, GDK_WINDOW_TYPE_HINT_NORMAL ); }

    //__________________________________________________________________
    const char*  Gtk::TypeNames::windowTypeHint( GdkWindowTypeHint gdkWindowTypeHint )
    { return Gtk::TypeNames::Finder<GdkWindowTypeHint>( windowTypeHintMap, 14 ).findCss( gdkWindowTypeHint ); }

    //__________________________________________________________________
    typedef Gtk::TypeNames::Entry<GtkOrientation> OrientationMap;
    static const OrientationMap orientationMap[] =
    {
        { GTK_ORIENTATION_HORIZONTAL, "horizontal" },
        { GTK_ORIENTATION_VERTICAL, "vertical" }
    };

    //__________________________________________________________________
    GtkOrientation Gtk::TypeNames::matchOrientation( const char*  cssOrientation )
    { return Gtk::TypeNames::Finder<GtkOrientation>( orientationMap, 2 ).findGtk( cssOrientation, GTK_ORIENTATION_HORIZONTAL ); }

    //__________________________________________________________________
    const char* Gtk::TypeNames::orientation( GtkOrientation gtkOrientation )
    { return Gtk::TypeNames::Finder<GtkOrientation>( orientationMap, 2 ).findCss( gtkOrientation ); }

    //__________________________________________________________________
    typedef Gtk::TypeNames::Entry<GtkExpanderStyle> EpanderStyleMap;
    static const EpanderStyleMap expanderStyleMap[] =
    {
        { GTK_EXPANDER_COLLAPSED, "collapsed" },
        { GTK_EXPANDER_SEMI_COLLAPSED, "semi-collapsed" },
        { GTK_EXPANDER_SEMI_EXPANDED, "semi-expanded" },
        { GTK_EXPANDER_EXPANDED, "expanded" }
    };

    //__________________________________________________________________
    GtkExpanderStyle Gtk::TypeNames::matchExpanderStyle( const char*  cssExpanderStyle )
    { return Gtk::TypeNames::Finder<GtkExpanderStyle>( expanderStyleMap, 4 ).findGtk( cssExpanderStyle, GTK_EXPANDER_COLLAPSED ); }

    //__________________________________________________________________
    const char*  Gtk::TypeNames::expanderStyle( GtkExpanderStyle gtkExpanderStyle )
    { return Gtk::TypeNames::Finder<GtkExpanderStyle>( expanderStyleMap, 4 ).findCss( gtkExpanderStyle ); }

    //__________________________________________________________________
    typedef Gtk::TypeNames::Entry<GtkResponseType> ResponseTypeMap;
    static const ResponseTypeMap responseTypeMap[] =
    {
        { GTK_RESPONSE_NONE, "none" },
        { GTK_RESPONSE_REJECT, "reject" },
        { GTK_RESPONSE_ACCEPT, "accept" },
        { GTK_RESPONSE_DELETE_EVENT, "delete" },
        { GTK_RESPONSE_OK, "ok" },
        { GTK_RESPONSE_CANCEL, "cancel" },
        { GTK_RESPONSE_CLOSE, "close" },
        { GTK_RESPONSE_YES, "yes" },
        { GTK_RESPONSE_NO, "no" },
        { GTK_RESPONSE_APPLY, "apply" },
        { GTK_RESPONSE_HELP, "help" },
        { (GtkResponseType)1, "id 1" }
    };
    const int responseTypeMapSize=sizeof(responseTypeMap)/sizeof(responseTypeMap[0]);

    //__________________________________________________________________
    GtkResponseType Gtk::TypeNames::matchResponse( const char*  cssResponseType )
    { return Gtk::TypeNames::Finder<GtkResponseType>( responseTypeMap, responseTypeMapSize ).findGtk( cssResponseType, GTK_RESPONSE_NONE ); }

    //__________________________________________________________________
    const char*  Gtk::TypeNames::response( GtkResponseType gtkResponseType )
    { return Gtk::TypeNames::Finder<GtkResponseType>( responseTypeMap, responseTypeMapSize ).findCss( gtkResponseType ); }

    //__________________________________________________________________
    typedef Gtk::TypeNames::Entry<GtkIconSize> IconSizeMap;
    static const IconSizeMap iconSizeMap[] =
    {
        { GTK_ICON_SIZE_INVALID, "invalid" },
        { GTK_ICON_SIZE_MENU, "menu" },
        { GTK_ICON_SIZE_SMALL_TOOLBAR, "small toolbar" },
        { GTK_ICON_SIZE_LARGE_TOOLBAR, "large toolbar" },
        { GTK_ICON_SIZE_BUTTON, "button" },
        { GTK_ICON_SIZE_DND, "drag and drop" },
        { GTK_ICON_SIZE_DIALOG, "dialog" }
     };
    const int iconSizeMapSize=sizeof(iconSizeMap)/sizeof(iconSizeMap[0]);

    //__________________________________________________________________
    GtkIconSize Gtk::TypeNames::matchIconSize( const char*  cssIconSize )
    { return Gtk::TypeNames::Finder<GtkIconSize>( iconSizeMap, iconSizeMapSize ).findGtk( cssIconSize, GTK_ICON_SIZE_INVALID ); }

    //__________________________________________________________________
    const char*  Gtk::TypeNames::iconSize( GtkIconSize gtkIconSize )
    { return Gtk::TypeNames::Finder<GtkIconSize>( iconSizeMap, iconSizeMapSize ).findCss( gtkIconSize ); }

}
