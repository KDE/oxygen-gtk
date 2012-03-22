#ifndef oxygenargbhelper_h
#define oxygenargbhelper_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

namespace Oxygen
{

    //! handles argb support on a per-application, per-widget basis
    class ArgbHelper
    {

        public:

        //! constructor
        explicit ArgbHelper( void );

        //! destructor
        virtual ~ArgbHelper( void );

        //! initialize hooks
        void initializeHooks( void );

        protected:

        //! argb hook
        static gboolean styleSetHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        private:

        //! true if hooks are initialized
        bool _hooksInitialized;

        //! colormap hook
        Hook _styleSetHook;

    };

}

#endif
