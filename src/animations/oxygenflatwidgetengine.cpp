/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include "oxygenflatwidgetengine.h"
#include "../oxygengtktypenames.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <string>

namespace Oxygen
{
    //_________________________________________________________
    bool FlatWidgetEngine::registerWidget( GtkWidget* widget )
    {
        if( contains( widget ) ) return false;
        _data.insert( widget );
        BaseEngine::registerWidget( widget );
        return true;
    }

    //_________________________________________________________
    GtkWidget* FlatWidgetEngine::flatParent( GtkWidget* widget )
    {

        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        { if( contains( parent ) ) return parent; }

        return 0L;
    }

}
