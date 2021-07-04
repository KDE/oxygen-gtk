/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygendialogengine.h"
#include "../oxygengtktypenames.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <string>

namespace Oxygen
{
    //_________________________________________________________
    bool DialogEngine::registerWidget( GtkWidget* widget )
    {
        if( contains( widget ) ) return false;

        GtkDialog* dialog(GTK_DIALOG(widget));

        GtkResponseType responses[]={
            GTK_RESPONSE_HELP,
            (GtkResponseType)1, // FIXME: This id is commonly used, but doesn't seem to be documented anywhere...
            GTK_RESPONSE_OK,
            GTK_RESPONSE_YES,
            GTK_RESPONSE_ACCEPT,
            GTK_RESPONSE_APPLY,
            GTK_RESPONSE_REJECT,
            GTK_RESPONSE_CLOSE,
            GTK_RESPONSE_NO,
            GTK_RESPONSE_CANCEL
        };

        const int numOfResponseIDs=sizeof(responses)/sizeof(responses[0]);

        #if OXYGEN_DEBUG
        std::cerr<<"DialogEngine::registerWidget - Initial response table: ";
        for(int i=0; i<numOfResponseIDs; i++)
        { std::cerr << Gtk::TypeNames::response( responses[i] ) << ", "; }
        std::cerr<<std::endl;
        #endif

        int numOfResponsesFound=0;
        for(int i=0; i<numOfResponseIDs; i++)
        {
            GtkWidget* button( Gtk::gtk_dialog_find_button(dialog,responses[i]) );
            if( !button ) continue;

            #if OXYGEN_DEBUG
            std::cerr << "responseID found: " << Gtk::TypeNames::response( responses[i] ) << "; button \"" << gtk_button_get_label(GTK_BUTTON(button)) << "\""  << std::endl;
            #endif
            // i is always >= numOfResponsesFound, so this will copy response id nearer to start, but never to end
            responses[numOfResponsesFound]=responses[i];
            numOfResponsesFound++;

        }

        #if OXYGEN_DEBUG
        std::cerr << "DialogEngine::registerWidget - numOfResponsesFound: " << numOfResponsesFound << std::endl;
        std::cerr << "List of responses found: { ";

        for(int i=0; i<numOfResponsesFound; i++)
        { std::cerr << Gtk::TypeNames::response( responses[i] ) << (i==numOfResponsesFound-1 ? " " :", "); }

        std::cerr << "}; remaining in the list: { ";

        for(int i=numOfResponsesFound; i<numOfResponseIDs; i++)
        { std::cerr << Gtk::TypeNames::response( responses[i] ) << (i==numOfResponseIDs-1 ? " " :", "); }

        std::cerr << "}\n";
        #endif

        // change order
        gtk_dialog_set_alternative_button_order_from_array( dialog, numOfResponsesFound, (gint*) responses );

        // insert in set
        _data.insert( widget );

        // call base class
        BaseEngine::registerWidget( widget );
        return true;

    }
}
