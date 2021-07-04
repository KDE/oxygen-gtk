#ifndef oxygenwidgetexplorer_h
#define oxygenwidgetexplorer_h

/*
* oxygenwidgetexplorer.h
* printout widget information on button press, for debugging
* -------------------
*
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* Largely inspired from Qtcurve style
* SPDX-FileCopyrightText: 2003-2010 Craig Drummond <craig.p.drummond@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenhook.h"

#include <gtk/gtk.h>

// constructor
namespace Oxygen
{

    class WidgetExplorer
    {

        public:

        //! constructor
        WidgetExplorer( void );

        //! destructor
        virtual ~WidgetExplorer( void );

        //! initialize hooks
        void initializeHooks( void );

        //! enabled state
        void setEnabled( bool value );

        protected:

        //! style-set hook
        static gboolean buttonPressHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        private:

        //! enable set
        bool _enabled;

        //! true when hooks are initialized
        bool _hooksInitialized;

        //! mouse button press event hook
        Hook _buttonPressHook;

    };

}

#endif
