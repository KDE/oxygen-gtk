#ifndef oxygengroupboxengine_h
#define oxygengroupboxengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenbaseengine.h"

#include <gtk/gtk.h>
#include <set>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! associates widgets with some type of data
    class GroupBoxEngine: public BaseEngine
    {

        public:

        //! constructor
        GroupBoxEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~GroupBoxEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget )
        { return( _data.insert( widget ).second ); }

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* widget )
        { _data.erase( widget ); }

        //! true if widget is included
        virtual bool contains( GtkWidget* widget )
        { return _data.find( widget ) != _data.end(); }

        private:

        //! store registered widgets
        std::set<GtkWidget*> _data;

    };

}

#endif
