#ifndef oxygenarrowstateengine_h
#define oxygenarrowstateengine_h
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

#include "../oxygenanimationdata.h"
#include "../oxygenstyleoptions.h"
#include "oxygenanimationengine.h"
#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenarrowstatedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable arrowstatees
    /*!
    ensures that the text entry and the button of editable arrowstatees
    gets hovered and focus flags at the same time
    */
    class ArrowStateEngine: public GenericEngine<ArrowStateData>, public AnimationEngine
    {

        public:

        //! constructor
        ArrowStateEngine( Animations* widget ):
            GenericEngine<ArrowStateData>( widget ),
            _duration( 150 )
            {}

        //! destructor
        virtual ~ArrowStateEngine( void )
        {}

        //! enabled state
        virtual void setEnabled( bool );

        //! transition duration
        virtual void setDuration( int );

        //! retrieve animation data matching a given widget for provided options
        /*! note: for convenience, this method also calls ::registerWidget and ::updateState */
        virtual AnimationData get( GtkWidget* widget, GtkArrowType type, const StyleOptions& options )
        {

            // check widget
            if( !( enabled() && widget ) ) return AnimationData();

            // register
            registerWidget( widget );

            ArrowStateData& data( this->data().value( widget ) );
            data.updateState( type, (options&Hover) && !(options&Disabled) );

            return data.timeLine( type ).isRunning() ? AnimationData( data.opacity( type ), AnimationHover ):AnimationData();

        }

        //@}

        protected:

        //!@name protected modifiers
        //@{

        //! register widget [overloaded]
        virtual bool registerWidget( GtkWidget* widget )
        {
            const bool registered( GenericEngine<ArrowStateData>::registerWidget( widget ) );
            if( registered )
            {
                data().value( widget ).timeLine( GTK_ARROW_UP ).setDuration( _duration );
                data().value( widget ).timeLine( GTK_ARROW_DOWN ).setDuration( _duration );
            }

            return registered;
        }

        //! true if widget is animated
        virtual bool isAnimated( GtkWidget* widget, GtkArrowType type )
        { return data().value( widget ).timeLine( type ).isRunning(); }

        //! animation opacity
        virtual double opacity( GtkWidget* widget, GtkArrowType type )
        { return data().value( widget ).opacity( type ); }

        //! update state
        virtual bool updateState( GtkWidget* widget, GtkArrowType type, bool value )
        { return data().value( widget ).updateState( type, value ); }

        //@}

        private:

        //! transition duration
        int _duration;

    };

}

#endif
