#ifndef oxygengenericengine_h
#define oxygengenericengine_h
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


#include "oxygenbaseengine.h"
#include "oxygendatamap.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! associates widgets with some type of data
    template< typename T>
    class GenericEngine: public BaseEngine
    {

        public:

        //! constructor
        GenericEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~GenericEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget )
        {
            if( _data.contains( widget ) ) return false;
            if( enabled() )
            {

                T& data( _data.registerWidget( widget ) );
                data.connect( widget );

            } else _data.registerWidget( widget );

            BaseEngine::registerWidget( widget );
            return true;

        }

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* widget )
        {
            if( !_data.contains( widget ) ) return;
            _data.value( widget ).disconnect( widget );
            _data.erase( widget );
        }

        //! enabled state
        inline virtual void setEnabled( bool value );

        //! true if widget is included
        virtual bool contains( GtkWidget* widget )
        { return _data.contains( widget ); }

        protected:

        //! return refence to data
        virtual DataMap<T>& data( void )
        { return _data; }

        private:

        //! map widgets to data
        DataMap<T> _data;

    };

    //________________________________________________________________________
    template< typename T> void GenericEngine<T>::setEnabled( bool value )
    {
        if( enabled() == value ) return;
        BaseEngine::setEnabled( value );
        if( enabled() ) _data.connectAll();
        else _data.disconnectAll();
    }

}

#endif
