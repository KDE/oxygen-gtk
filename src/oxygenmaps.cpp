/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright( C ) 2008 Robert Staudinger
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

#include <string.h>
#include "oxygenmaps.h"

namespace Oxygen
{

    //_______________________________________________
    typedef Maps::Entry<GtkStateType> StateMap;
    static const StateMap State_map[] =
    {

        { GTK_STATE_NORMAL, "normal" },
        { GTK_STATE_ACTIVE, "active" },
        { GTK_STATE_PRELIGHT, "prelight" },
        { GTK_STATE_SELECTED, "selected" },
        { GTK_STATE_INSENSITIVE, "insensitive" }

    };

    //___________________________________________________________
    GtkStateType Maps::matchState( const char*  cssState )
    { return Maps::Finder<GtkStateType>( State_map, 5 ).findGtk( cssState, GTK_STATE_NORMAL ); }

    //___________________________________________________________
    const char*  Maps::getState( GtkStateType gtkState )
    { return Maps::Finder<GtkStateType>( State_map, 5 ).findCss( gtkState ); }

    //___________________________________________________________
    // Map between shadow types in gtk and css format.
    typedef Maps::Entry<GtkShadowType> ShadowMap;
    static const ShadowMap Shadow_map[] =
    {
        { GTK_SHADOW_NONE, "none" },
        { GTK_SHADOW_IN, "in" },
        { GTK_SHADOW_OUT, "out" },
        { GTK_SHADOW_ETCHED_IN, "etched-in" },
        { GTK_SHADOW_ETCHED_OUT, "etched-out" }
    };

    //___________________________________________________________
    GtkShadowType Maps::matchShadow( const char*  cssShadow )
    { return Maps::Finder<GtkShadowType>( Shadow_map, 5 ).findGtk( cssShadow, GTK_SHADOW_NONE ); }

    //___________________________________________________________
    const char* Maps::getShadow( GtkShadowType gtkShadow )
    { return Maps::Finder<GtkShadowType>( Shadow_map, 5 ).findCss( gtkShadow ); }

    //___________________________________________________________
    typedef Maps::Entry<GtkArrowType> ArrowMap;
    static const ArrowMap Arrow_map[] =
    {
        { GTK_ARROW_UP, "up" },
        { GTK_ARROW_DOWN, "down" },
        { GTK_ARROW_LEFT, "left" },
        { GTK_ARROW_RIGHT, "right" },
        { GTK_ARROW_NONE, "none" }
    };

    //___________________________________________________________
    GtkArrowType Maps::matchArrow( const char*  cssArrow )
    { return Maps::Finder<GtkArrowType>( Arrow_map, 5 ).findGtk( cssArrow, GTK_ARROW_NONE ); }

    //___________________________________________________________
    const char* Maps::getArrow( GtkArrowType gtkArrow )
    { return Maps::Finder<GtkArrowType>( Arrow_map, 5 ).findCss( gtkArrow ); }

    //___________________________________________________________
    typedef Maps::Entry<GtkPositionType> PositionMap;
    static const PositionMap Position_map[] =
    {
        { GTK_POS_LEFT, "left" },
        { GTK_POS_RIGHT, "right" },
        { GTK_POS_TOP, "top" },
        { GTK_POS_BOTTOM, "bottom" },
    };

    //___________________________________________________________
    GtkPositionType Maps::matchPosition( const char* cssPosition )
    { return Maps::Finder<GtkPositionType>( Position_map, 4 ).findGtk( cssPosition, GTK_POS_LEFT ); }

    //__________________________________________________________________
    const char* Maps::getPosition( GtkPositionType gtkPosition )
    { return Maps::Finder<GtkPositionType>( Position_map, 4 ).findCss( gtkPosition ); }

    //__________________________________________________________________
    typedef Maps::Entry<GdkWindowEdge> WindowEdgeMap;
    static const WindowEdgeMap WindowEdge_map[] =
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
    GdkWindowEdge Maps::matchWindowEdge( const char*  cssWindowEdge )
    { return Maps::Finder<GdkWindowEdge>( WindowEdge_map, 8 ).findGtk( cssWindowEdge, GDK_WINDOW_EDGE_SOUTH_EAST ); }

    //__________________________________________________________________
    const char*  Maps::getWindowEdge( GdkWindowEdge gdkWindowEdge )
    { return Maps::Finder<GdkWindowEdge>( WindowEdge_map, 8 ).findCss( gdkWindowEdge ); }

    //__________________________________________________________________
    typedef Maps::Entry<GtkOrientation> OrientationMap;
    static const OrientationMap Orientation_map[] =
    {
        { GTK_ORIENTATION_HORIZONTAL, "horizontal" },
        { GTK_ORIENTATION_VERTICAL, "vertical" }
    };

    //__________________________________________________________________
    GtkOrientation Maps::matchOrientation( const char*  cssOrientation )
    { return Maps::Finder<GtkOrientation>( Orientation_map, 2 ).findGtk( cssOrientation, GTK_ORIENTATION_HORIZONTAL ); }

    //__________________________________________________________________
    const char* Maps::getOrientation( GtkOrientation gtkOrientation )
    { return Maps::Finder<GtkOrientation>( Orientation_map, 2 ).findCss( gtkOrientation ); }

    //__________________________________________________________________
    typedef Maps::Entry<GtkExpanderStyle> EpanderStyleMap;
    static const EpanderStyleMap ExpanderStyle_map[] =
    {
        { GTK_EXPANDER_COLLAPSED, "collapsed" },
        { GTK_EXPANDER_SEMI_COLLAPSED, "semi-collapsed" },
        { GTK_EXPANDER_SEMI_EXPANDED, "semi-expanded" },
        { GTK_EXPANDER_EXPANDED, "expanded" }
    };

    //__________________________________________________________________
    GtkExpanderStyle Maps::matchExpanderStyle( const char*  cssExpanderStyle )
    { return Maps::Finder<GtkExpanderStyle>( ExpanderStyle_map, 4 ).findGtk( cssExpanderStyle, GTK_EXPANDER_COLLAPSED ); }

    //__________________________________________________________________
    const char*  Maps::getExpanderStyle( GtkExpanderStyle gtkExpanderStyle )
    { return Maps::Finder<GtkExpanderStyle>( ExpanderStyle_map, 4 ).findCss( gtkExpanderStyle ); }

}
