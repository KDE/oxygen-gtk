#ifndef oxygentabwidgetstateengine_h
#define oxygentabwidgetstateengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../oxygenanimationdata.h"
#include "../oxygenstyleoptions.h"
#include "oxygenanimationengine.h"
#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygentabwidgetstatedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable tabwidgetstatees
    /*!
    ensures that the text entry and the button of editable tabwidgetstatees
    gets hovered and focus flags at the same time
    */
    class TabWidgetStateEngine: public GenericEngine<TabWidgetStateData>, public AnimationEngine
    {

        public:

        //! constructor
        TabWidgetStateEngine( Animations* widget ):
            GenericEngine<TabWidgetStateData>( widget )
            {}

        //! destructor
        virtual ~TabWidgetStateEngine( void )
        {}

        //! enabled state
        virtual bool setEnabled( bool value )
        {
            if( !BaseEngine::setEnabled( value ) ) return false;
            for( DataMap<TabWidgetStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
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
            for( DataMap<TabWidgetStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setDuration( value ); }
            return false;
        }

        //! retrieve animation data matching a given widget for provided options
        /*! note: for convenience, this method also calls ::registerWidget and ::updateState */
        virtual AnimationData get( GtkWidget* widget, int index, const StyleOptions& options )
        {

            // check widget
            if( !( enabled() && widget ) ) return AnimationData();

            // register
            registerWidget( widget );

            TabWidgetStateData& data( this->data().value( widget ) );
            data.updateState( index, (options&Hover) && !(options&Disabled) );

            return data.isAnimated( index ) ? AnimationData( data.opacity( index ), AnimationHover ):AnimationData();

        }

        //@}

        protected:

        //!@name protected modifiers
        //@{

        //! register widget [overloaded]
        virtual bool registerWidget( GtkWidget* widget )
        {
            const bool registered( GenericEngine<TabWidgetStateData>::registerWidget( widget ) );
            if( registered ) {
                data().value( widget ).setEnabled( enabled() );
                data().value( widget ).setDuration( duration() );
            }
            return registered;
        }

        //@}

    };

}

#endif
