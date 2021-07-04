#ifndef oxygenbaseengine_h
#define oxygenbaseengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <gtk/gtk.h>
#include <vector>

namespace Oxygen
{

    //! forward declaration
    class Animations;

    //! base class for all engines
    /*! engines map pointers to GtkWidget to some data, needed for animations */
    class BaseEngine
    {
        public:

        //! constructor
        BaseEngine( Animations* parent ):
            _parent( parent ),
            _enabled( true )
        {}

        virtual ~BaseEngine()
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget );

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* ) = 0;

        //! engine list
        typedef std::vector< BaseEngine* > List;

        //! enable state
        /*! returns true if changed */
        virtual bool setEnabled( bool value )
        {
            if( _enabled == value ) return false;
            _enabled = value;
            return true;
        }

        //! enable state
        bool enabled( void ) const
        { return _enabled; }

        protected:

        //! returns parent
        virtual Animations& parent( void ) const
        { return *_parent; }

        private:

        //! parent
        Animations* _parent;

        //! enable flag
        bool _enabled;

    };

}

#endif
