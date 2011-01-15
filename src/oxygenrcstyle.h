#ifndef oxygenrcstyle_h
#define oxygenrcstyle_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

// Oxygen RcStyle struct
struct OxygenRcStyle
{ GtkRcStyle parent; };

// oxygen RcStyleClass struct
struct OxygenRcStyleClass
{ GtkRcStyleClass parent; };

namespace Oxygen
{
    class RCStyle
    {

        public:

        //! type registration
        static void registerType( GTypeModule* );

        //! registered type
        static GType type( void );

        protected:

        //! class initialization method
        static void classInit( OxygenRcStyleClass* );

        //! style creation
        static GtkStyle* createStyle( GtkRcStyle* );

        //! parse RC options
        static guint parse( GtkRcStyle*, GtkSettings*, GScanner* );

        //! merge styles
        static void merge( GtkRcStyle*, GtkRcStyle* );

        private:

        //! parent class
        static GtkRcStyleClass* _parentClass;

        //! registered type indo
        static GTypeInfo _typeInfo;

        //! registered type
        static GType _type;

    };
}


#endif /* OXYGEN_RC_STYLE_H */
