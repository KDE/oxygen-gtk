#ifndef oxygentreeviewstateengine_h
#define oxygentreeviewstateengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../oxygenanimationdata.h"
#include "../oxygengtkcellinfo.h"
#include "../oxygenstyleoptions.h"
#include "oxygenanimationengine.h"
#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygentreeviewstatedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable treeviewstatees
    /*!
    ensures that the text entry and the button of editable treeviewstatees
    gets hovered and focus flags at the same time
    */
    class TreeViewStateEngine: public GenericEngine<TreeViewStateData>, public AnimationEngine
    {

        public:

        //! constructor
        TreeViewStateEngine( Animations* widget ):
            GenericEngine<TreeViewStateData>( widget ),
            _duration( 150 )
            {}

        //! destructor
        virtual ~TreeViewStateEngine( void )
        {}

        //! enabled state
        virtual bool setEnabled( bool value )
        {
            if( !BaseEngine::setEnabled( value ) ) return false;
            for( DataMap<TreeViewStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
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
            for( DataMap<TreeViewStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
            { iter->second.setDuration( value ); }
            return true;
        }

        //! retrieve animation data matching a given widget for provided options
        /*! note: for convenience, this method also calls ::registerWidget and ::updateState */
        virtual AnimationData get( GtkWidget* widget, const Gtk::CellInfo& info, const StyleOptions& options )
        {

            // check widget
            if( !( enabled() && widget && info.isValid() ) ) return AnimationData();

            // register
            registerWidget( widget );

            TreeViewStateData& data( this->data().value( widget ) );
            data.updateState( info, (options&Hover) && !(options&Disabled) );

            return data.isAnimated( info ) ? AnimationData( data.opacity( info ), AnimationHover ):AnimationData();

        }

        //@}

        protected:

        //!@name protected modifiers
        //@{

        //! register widget [overloaded]
        virtual bool registerWidget( GtkWidget* widget )
        {
            const bool registered( GenericEngine<TreeViewStateData>::registerWidget( widget ) );
            if( registered ) {
                data().value( widget ).setEnabled( enabled() );
                data().value( widget ).setDuration( duration() );
            }
            return registered;
        }

        //@}

        private:

        //! transition duration
        int _duration;

    };

}

#endif
