#ifndef oxygenmenustateengine_h
#define oxygenmenustateengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenanimationengine.h"
#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenmenustatedata.h"

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
    class MenuStateEngine: public GenericEngine<MenuStateData>, public AnimationEngine
    {

        public:

        //! constructor
        MenuStateEngine( Animations* parent ):
            GenericEngine<MenuStateData>( parent ),
            _followMouse( false ),
            _followMouseAnimationsDuration( 40 )
            {}

        //! destructor
        virtual ~MenuStateEngine( void )
        {}

        //! register widget [overloaded]
        virtual bool registerWidget( GtkWidget* widget )
        {
            const bool registered( GenericEngine<MenuStateData>::registerWidget( widget ) );
            if( registered )
            {
                MenuStateData& data( this->data().value( widget ) );
                data.setDuration( duration() );
                data.setEnabled( enabled() );
                data.setFollowMouse( _followMouse );
                data.setFollowMouseAnimationsDuration( _followMouseAnimationsDuration );
            }
            return registered;
        }

        //!@name modifiers
        //@{

        //! enable state
        virtual bool setEnabled( bool value )
        {
            if( !BaseEngine::setEnabled( value ) ) return false;
            for( DataMap<MenuStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            {
                iter->second.setEnabled( value );
                if( enabled() && !widgetIsBlackListed( iter->first ) ) iter->second.connect( iter->first );
                else iter->second.disconnect( iter->first );
            }

            return true;
        }

        //! transition duration
        virtual bool setDuration( int value )
        {
            if( !AnimationEngine::setDuration( value ) ) return false;
            for( DataMap<MenuStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setDuration( value ); }
            return false;
        }

        //! enable follow-mouse animation
        bool setFollowMouse( bool value )
        {
            if( _followMouse == value ) return false;
            _followMouse = value;

            for( DataMap<MenuStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setFollowMouse( value && !widgetIsBlackListed( iter->first ) ); }
            return true;
        }

        //! follow-mouse animations duration
        bool setFollowMouseAnimationsDuration( int value )
        {
            if( _followMouseAnimationsDuration == value ) return false;
            _followMouseAnimationsDuration = value;

            for( DataMap<MenuStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
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

        //! true when fade out animation is locked (delayed)
        bool isLocked( GtkWidget* widget )
        { return data().value( widget ).isLocked(); }

        //@}

        private:

        //! follow-mouse enabled
        bool _followMouse;

        //! follow-mouse animation duration
        int _followMouseAnimationsDuration;

    };

}

#endif
