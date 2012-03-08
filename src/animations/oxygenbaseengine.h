#ifndef oxygenbaseengine_h
#define oxygenbaseengine_h
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
