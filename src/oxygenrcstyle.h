#ifndef oxygenrcstyle_h
#define oxygenrcstyle_h
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
