#ifndef oxygenflatwidgetengine_h
#define oxygenflatwidgetengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenbaseengine.h"

#include <gtk/gtk.h>
#include <cassert>
#include <set>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! associates widgets with some type of data
    class FlatWidgetEngine: public BaseEngine
    {

        public:

        //! constructor
        FlatWidgetEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~FlatWidgetEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget )
        { return registerFlatWidget( widget ); }

        //! register widget
        virtual bool registerFlatWidget( GtkWidget* );

        //! register widget
        virtual bool registerPaintWidget( GtkWidget* );

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* widget )
        {
            _flatData.erase( widget );
            _paintData.erase( widget );
        }

        //! true if one of widgets parent is included
        virtual GtkWidget* flatParent( GtkWidget* );

        protected:

        bool containsFlat( GtkWidget* widget ) const
        { return _flatData.find( widget ) != _flatData.end(); }

        bool containsPaint( GtkWidget* widget ) const
        { return _paintData.find( widget ) != _paintData.end(); }

        private:

        //! store registered widgets
        std::set<GtkWidget*> _flatData;
        std::set<GtkWidget*> _paintData;

    };

}

#endif
