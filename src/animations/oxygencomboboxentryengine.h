#ifndef oxygencomboboxentryengine_h
#define oxygencomboboxentryengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygencomboboxentrydata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboboxentryes
    /*!
    ensures that the text entry and the button of editable comboboxentryes
    gets hovered and focus flags at the same time
    */
    class ComboBoxEntryEngine: public GenericEngine<ComboBoxEntryData>
    {

        public:

        //! constructor
        ComboBoxEntryEngine( Animations* widget ):
            GenericEngine<ComboBoxEntryData>( widget )
            {}

        //! destructor
        virtual ~ComboBoxEntryEngine( void )
        {}

        //!@name modifiers
        //@{

        //! assign button to data matching widget
        void setButton( GtkWidget* widget, GtkWidget* value )
        { data().value( widget ).setButton( value ); }

        //! assign entry to data matching widget
        void setEntry( GtkWidget* widget, GtkWidget* value )
        { data().value( widget ).setEntry( value ); }

        //! button focus
        void setButtonFocus( GtkWidget* widget, bool value )
        { data().value( widget ).setButtonFocus( value ); }

         //! button focus
        void setButtonHovered( GtkWidget* widget, bool value )
        { data().value( widget ).setButtonHovered( value ); }

        //! entry focus
        void setEntryFocus( GtkWidget* widget, bool value )
        { data().value( widget ).setEntryFocus( value ); }

        //@}

        //!@name accessors
        //@{

        //! returns pressed combobox if any
        inline GtkWidget* find( GtkWidget* );

        //! true if either button or entry has focus
        bool hasFocus( GtkWidget* widget )
        { return data().value( widget ).hasFocus(); }

        //! true if comboBox is hovered
        bool hovered( GtkWidget* widget )
        { return data().value( widget ).hovered(); }

        //@}

    };

    //_________________________________________________
    GtkWidget* ComboBoxEntryEngine::find( GtkWidget* value )
    {
        GtkWidget* topLevel( gtk_widget_get_toplevel( value ) );
        DataMap<ComboBoxEntryData>::Map& dataMap( data().map() );
        for( DataMap<ComboBoxEntryData>::Map::iterator iter = dataMap.begin(); iter != dataMap.end(); iter++ )
        {
            if( iter->second.pressed() )
            {
                iter->second.setList( topLevel );
                return iter->first;
            }
        }

        for( DataMap<ComboBoxEntryData>::Map::iterator iter = dataMap.begin(); iter != dataMap.end(); iter++ )
        { if( iter->second.list() == topLevel ) return iter->first; }

        return 0L;

    }

}

#endif
