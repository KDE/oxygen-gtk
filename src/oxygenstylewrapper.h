#ifndef oxygenstylewrapper_h
#define oxygenstylewrapper_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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
