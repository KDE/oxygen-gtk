#ifndef oxygenmenubarstateengine_h
#define oxygenmenubarstateengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenanimationengine.h"
#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenmenubarstatedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboboxes
    /*!
    ensures that the text entry and the button of editable comboboxes
    gets hovered and focus flags at the same time
    */
    class MenuBarStateEngine: public GenericEngine<MenuBarStateData>, public AnimationEngine
    {

        public:

        //! constructor
        MenuBarStateEngine( Animations* parent ):
            GenericEngine<MenuBarStateData>( parent ),
            _animationsEnabled( true ),
            _followMouse( false ),
            _followMouseAnimationsDuration( 80 )
            {}

        //! destructor
        virtual ~MenuBarStateEngine( void )
        {}

        //! register widget [overloaded]
        virtual bool registerWidget( GtkWidget* widget )
        {
            const bool registered( GenericEngine<MenuBarStateData>::registerWidget( widget ) );
            if( registered )
            {
                MenuBarStateData& data( this->data().value( widget ) );
                data.setDuration( duration() );
                data.setAnimationsEnabled( _animationsEnabled );
                data.setFollowMouse( _followMouse );
                data.setFollowMouseAnimationsDuration( _followMouseAnimationsDuration );
            }
            return registered;
        }

        //!@name modifiers
        //@{

        //! enable animations
        bool setAnimationsEnabled( bool value )
        {
            if( _animationsEnabled == value ) return false;
            _animationsEnabled = value;

            for( DataMap<MenuBarStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setAnimationsEnabled( value && !widgetIsBlackListed( iter->first ) ); }
            return true;
        }

        //! transition duration
        virtual bool setDuration( int value )
        {
            if( !AnimationEngine::setDuration( value ) ) return false;
            for( DataMap<MenuBarStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setDuration( value ); }
            return false;
        }

        //! enable follow-mouse animation
        bool setFollowMouse( bool value )
        {
            if( _followMouse == value ) return false;
            _followMouse = value;

            for( DataMap<MenuBarStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setFollowMouse( value && !widgetIsBlackListed( iter->first ) ); }
            return true;
        }

        //! follow-mouse animations duration
        bool setFollowMouseAnimationsDuration( int value )
        {
            if( _followMouseAnimationsDuration == value ) return false;
            _followMouseAnimationsDuration = value;

            for( DataMap<MenuBarStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setFollowMouseAnimationsDuration( value ); }
            return true;
        }

        //@}

        //!@name accessors
        //@{

        //! true if animated
        bool isAnimated( GtkWidget* widget )
        { return data().value( widget ).isAnimated(); }

        //! true if given animation type is animated
        bool isAnimated( GtkWidget* widget, const WidgetType& type )
        { return data().value( widget ).isAnimated( type ); }

        //! animated widget for given parent and type
        GtkWidget* widget( GtkWidget* widget, const WidgetType& type )
        { return data().value( widget ).widget( type ); }

        //! animated rect for given widget and type
        const GdkRectangle& rectangle( GtkWidget* widget, const WidgetType& type )
        { return data().value( widget ).rectangle( type ); }

        //! animation data for given widget and type
        AnimationData animationData( GtkWidget* widget, const WidgetType& type )
        { return data().value( widget ).animationData( type ); }

        //! returns true if animated rectangle is valid
        bool animatedRectangleIsValid( GtkWidget* widget )
        { return data().value( widget ).animatedRectangleIsValid(); }

        //! animated rectangle
        const GdkRectangle& animatedRectangle( GtkWidget* widget )
        { return data().value( widget ).animatedRectangle(); }

        //@}

        private:

        //! enable animations
        bool _animationsEnabled;

        //! follow-mouse enabled
        bool _followMouse;

        //! follow-mouse animation duration
        int _followMouseAnimationsDuration;

    };

}

#endif
