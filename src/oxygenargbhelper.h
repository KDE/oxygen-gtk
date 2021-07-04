#ifndef oxygenargbhelper_h
#define oxygenargbhelper_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
