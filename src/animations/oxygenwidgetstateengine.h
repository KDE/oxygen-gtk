#ifndef oxygenwidgetstateengine_h
#define oxygenwidgetstateengine_h
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


#include "../oxygenanimationmodes.h"
#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenwidgetstatedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! associates widgets with some type of data
    class WidgetStateEngine: public BaseEngine
    {

        public:

        //! constructor
        WidgetStateEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~WidgetStateEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget, AnimationModes modes )
        {
            bool registered( false );
            if( (modes&AnimationHover) && registerWidget( widget, _hoverData ) ) registered = true;
            if( (modes&AnimationFocus) && registerWidget( widget, _focusData ) ) registered = true;

            if( registered )
            { BaseEngine::registerWidget( widget ); }

            return registered;

        }

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* widget )
        {
            unregisterWidget( widget, _hoverData );
            unregisterWidget( widget, _focusData );
        }

        //! enabled state
        inline virtual void setEnabled( bool value );

        //!@name accessors
        //@{

        //! true if widget is included
        virtual bool contains( GtkWidget* widget, AnimationMode mode )
        {
            switch( mode )
            {
                case AnimationHover: return _hoverData.contains( widget );
                case AnimationFocus: return _focusData.contains( widget );
                default: return false;
            }
        }

        //! true if widget is animated
        virtual bool isAnimated( GtkWidget* widget, AnimationMode mode )
        {
            switch( mode )
            {
                case AnimationHover:
                return _hoverData.value( widget ).timeLine().isRunning();

                case AnimationFocus:
                return _focusData.value( widget ).timeLine().isRunning();

                default: return false;
            }
        }

        //! animation opacity
        virtual double opacity( GtkWidget* widget, AnimationMode mode )
        {
            if( !isAnimated( widget, mode ) ) return WidgetStateData::OpacityInvalid;
            switch( mode )
            {
                case AnimationHover:
                return _hoverData.value( widget ).opacity();

                case AnimationFocus:
                return _focusData.value( widget ).opacity();

                default: return WidgetStateData::OpacityInvalid;

            }

        }

        //@}

        //!@name modifiers
        //@{

        //! update state
        virtual bool updateState( GtkWidget* widget, AnimationMode mode, bool value )
        {
            switch( mode )
            {
                case AnimationHover:
                return _hoverData.value( widget ).updateState( value );

                case AnimationFocus:
                return _focusData.value( widget ).updateState( value );

                default: return false;
            }

        }

        //@}

        protected:

        //! register widget in given map
        bool registerWidget( GtkWidget* widget, DataMap<WidgetStateData>& dataMap ) const
        {

            if( dataMap.contains( widget ) ) return false;
            if( enabled() )
            {

                WidgetStateData& data( dataMap.registerWidget( widget ) );
                data.connect( widget );

            } else dataMap.registerWidget( widget );

            return true;

        }

        //! register widget in given map
        void unregisterWidget( GtkWidget* widget, DataMap<WidgetStateData>& data ) const
        {
            if( !data.contains( widget ) ) return;
            data.value( widget ).disconnect( widget );
            data.erase( widget );
        }

        private:

        //!@name maps
        //@{
        DataMap<WidgetStateData> _hoverData;
        DataMap<WidgetStateData> _focusData;
        //@}

    };

    //________________________________________________________________________
    void WidgetStateEngine::setEnabled( bool value )
    {

        if( enabled() == value ) return;
        BaseEngine::setEnabled( value );
        if( enabled() )
        {

            _hoverData.connectAll();
            _focusData.connectAll();

        } else {

            _hoverData.disconnectAll();
            _focusData.disconnectAll();

        }

    }

}

#endif
