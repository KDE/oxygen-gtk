/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenflatwidgetengine.h"
#include "../oxygengtktypenames.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <string>

namespace Oxygen
{
    //_________________________________________________________
    bool FlatWidgetEngine::registerFlatWidget( GtkWidget* widget )
    {
        if( containsFlat( widget ) ) return false;
        _flatData.insert( widget );
        BaseEngine::registerWidget( widget );
        return true;
    }

    //_________________________________________________________
    bool FlatWidgetEngine::registerPaintWidget( GtkWidget* widget )
    {
        if( containsPaint( widget ) ) return false;
        _paintData.insert( widget );
        BaseEngine::registerWidget( widget );
        return true;
    }

    //_________________________________________________________
    GtkWidget* FlatWidgetEngine::flatParent( GtkWidget* widget )
    {

        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        {
            if( containsPaint( parent ) ) return 0x0;
            else if( containsFlat( parent ) ) return parent;
        }

        return 0L;
    }

}
