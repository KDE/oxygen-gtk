#ifndef oxygenwidgetexplorer_h
#define oxygenwidgetexplorer_h

/*
* oxygenwidgetexplorer.h
* printout widget information on button press, for debugging
* -------------------
*
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* Largely inspired from Qtcurve style
* Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
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
