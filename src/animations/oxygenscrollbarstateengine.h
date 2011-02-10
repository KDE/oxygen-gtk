#ifndef oxygenscrollbarstateengine_h
#define oxygenscrollbarstateengine_h
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
#include "oxygenscrollbarstatedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable scrollbarstatees
    /*!
    ensures that the text entry and the button of editable scrollbarstatees
    gets hovered and focus flags at the same time
    */
    class ScrollBarStateEngine: public GenericEngine<ScrollBarStateData>, public AnimationEngine
    {

        public:

        //! constructor
        ScrollBarStateEngine( Animations* widget ):
            GenericEngine<ScrollBarStateData>( widget ),
            _duration( 150 )
            {}

        //! destructor
        virtual ~ScrollBarStateEngine( void )
        {}

        //! enabled state
        virtual void setEnabled( bool );

        //! transition duration
        virtual void setDuration( int );

        //! retrieve animation data matching a given widget for provided options
        /*! note: for convenience, this method also calls ::registerWidget and ::updateState */
        virtual AnimationData get( GtkWidget* widget, const GdkRectangle& rect, GtkArrowType type, const StyleOptions& options )
        {

            // check widget
            if( !( enabled() && widget ) ) return AnimationData();

            // register
            registerWidget( widget );

            ScrollBarStateData& data( this->data().value( widget ) );

            // update rect
            if( options&Hover ) data.setRect( type, rect );

            // check intersection and get relevant animation data
            if( gdk_rectangle_intersect( &rect, &data.rect( type ), 0L ) )
            {

                data.updateState( type, (options&Hover) && !(options&Disabled) );
                return data.timeLine( type ).isRunning() ? AnimationData( data.opacity( type ), AnimationHover ):AnimationData();

            } else return AnimationData();

        }

        //@}

        protected:

        //! register widget [overloaded]
        virtual bool registerWidget( GtkWidget* widget )
        {
            const bool registered( GenericEngine<ScrollBarStateData>::registerWidget( widget ) );
            if( registered )
            {
                data().value( widget ).timeLine( GTK_ARROW_UP ).setDuration( _duration );
                data().value( widget ).timeLine( GTK_ARROW_DOWN ).setDuration( _duration );
            }

            return registered;
        }

        private:

        //! transition duration
        int _duration;

    };

}

#endif
