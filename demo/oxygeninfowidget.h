#ifndef oxygeninfowidget_h
#define oxygeninfowidget_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygendemowidget.h"
#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <string>

namespace Oxygen
{

    class InfoWidget: public DemoWidget
    {

        public:

        //! constructor
        InfoWidget( void );

        //! destructor
        virtual ~InfoWidget( void );

        protected:

        // bold label
        GtkWidget* boldLabel( void ) const;

        //! add author
        void addAuthor( GtkWidget*, int, const std::string&, const std::string& ) const;

    };

}

#endif
