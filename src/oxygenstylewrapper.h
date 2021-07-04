#ifndef oxygenstylewrapper_h
#define oxygenstylewrapper_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <gtk/gtk.h>
#include "oxygenxulinfo.h"

// oxygen style struct
struct OxygenStyle
{ GtkStyle parent; };

// oxygen style class struct
struct OxygenStyleClass
{  GtkStyleClass parent; };

namespace Oxygen
{
    class StyleWrapper
    {

        public:

        //! type registration
        static void registerType( GTypeModule* );

        //! version type registration
        /*!
        it is used to let an external program retrieve
        the oxygen-gtk version that it uses, if any
        */
        static void registerVersionType( void );

        //! registered type
        static GType type( void );

        //! parent class
        inline static GtkStyleClass* parentClass( void )
        { return _parentClass; }

        //! xul info
        inline static XulInfo& xulInfo( void )
        { return _xulInfo; }

        protected:

        //! instance initialization
        static void instanceInit( OxygenStyle* );

        //! class initialization
        static void classInit( OxygenStyleClass* );

        private:

        //! parent class
        static GtkStyleClass* _parentClass;

        //! registered type indo
        static GTypeInfo _typeInfo;

        //! registered type
        static GType _type;

        //! xul info, needed to properly render focus in checkboxes and radio buttons
        static XulInfo _xulInfo;

    };
}

#endif
