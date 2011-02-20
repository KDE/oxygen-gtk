#ifndef oxygenmenubarstateengine_h
#define oxygenmenubarstateengine_h
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
            _animationsEnabled( true )
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
                data().value( widget ).setDuration( duration() );
                data().value( widget ).setAnimationsEnabled( _animationsEnabled );
            }
            return registered;
        }

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

        //!@name accessors
        //@{

        //! true if locked
        bool isLocked( GtkWidget* widget )
        { return data().value( widget ).isLocked(); }

        //! true if animated
        bool isAnimated( GtkWidget* widget )
        { return data().value( widget ).isAnimated(); }

        //! true if given animation type is animated
        bool isAnimated( GtkWidget* widget, const AnimationType& type )
        { return data().value( widget ).isAnimated( type ); }

        //! animated rect for given widget and type
        const GdkRectangle& rectangle( GtkWidget* widget, const AnimationType& type )
        { return data().value( widget ).rectangle( type ); }

        //! animation data for given widget and type
        AnimationData animationData( GtkWidget* widget, const AnimationType& type )
        { return data().value( widget ).animationData( type ); }

        private:

        //! enable animations
        bool _animationsEnabled;

    };

}

#endif
