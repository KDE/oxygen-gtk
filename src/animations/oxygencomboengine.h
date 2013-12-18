#ifndef oxygencomboengine_h
#define oxygencomboengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include <gtk/gtk.h>
#include <set>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboes
    /*!
    ensures that the text  and the button of editable comboes
    gets hovered and focus flags at the same time
    */
    class ComboEngine: public BaseEngine
    {

        public:

        //! constructor
        ComboEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~ComboEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget )
        {
            if( contains( widget ) ) return false;
            _data.insert( widget );
            return true;
        }

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* widget )
        { _data.erase( widget ); }

        //! true if widget is included
        virtual bool contains( GtkWidget* widget )
        { return _data.find( widget ) != _data.end(); }

        //@}

        //!@name accessors
        //@{

        //! find combo matching widget
        /*! the widget can be any of those in a visible list */
        inline GtkWidget* find( GtkWidget* ) const;

        //@}

        private:

        //! store registered widgets
        std::set<GtkWidget*> _data;

    };

    //_________________________________________________
    GtkWidget* ComboEngine::find( GtkWidget* value ) const
    {
        GtkWidget* topLevel( gtk_widget_get_toplevel( value ) );
        for( std::set<GtkWidget*>::const_iterator iter = _data.begin(); iter != _data.end(); iter++ )
        {

            if( GTK_IS_COMBO( *iter ) && topLevel == GTK_COMBO( *iter )->popwin )
            { return *iter; }

        }

        return 0L;

    }

}

#endif
