#ifndef oxygentreeviewstateengine_h
#define oxygentreeviewstateengine_h
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
