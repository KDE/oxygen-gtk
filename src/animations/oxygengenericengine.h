#ifndef oxygengenericengine_h
#define oxygengenericengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
        /*! returns true if changed */
        inline virtual bool setEnabled( bool value );

        //! true if widget is included
        virtual bool contains( GtkWidget* widget )
        { return _data.contains( widget ); }

        protected:

        //! return refence to data
        virtual DataMap<T>& data( void )
        { return _data; }

        //! return refence to data
        virtual const DataMap<T>& data( void ) const
        { return _data; }

        private:

        //! map widgets to data
        DataMap<T> _data;

    };

    //________________________________________________________________________
    template< typename T> bool GenericEngine<T>::setEnabled( bool value )
    {
        if( enabled() == value ) return false;

        BaseEngine::setEnabled( value );
        if( enabled() ) _data.connectAll();
        else _data.disconnectAll();

        return true;

    }

}

#endif
