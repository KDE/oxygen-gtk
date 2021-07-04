#ifndef oxygenxulinfo_h
#define oxygenxulinfo_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygengtkutils.h"

namespace Oxygen
{
    //! checkbox info, needed for proper focus rendering in Xul applications
    class XulInfo
    {

        public:

        enum Type
        {
            Invalid,
            CheckBox,
            RadioButton
        };

        //! constructor
        XulInfo( void ):
            _type( Invalid ),
            _rect( Gtk::gdk_rectangle() )
        {}

        //! destructor
        virtual ~XulInfo( void )
        {}

        //!@name accessors
        //@{

        Type type( void ) const
        { return _type; }

        //! rectangle
        const GdkRectangle& rect( void ) const
        { return _rect; }

        //! validity
        bool isValid( void )
        { return _type != Invalid && Gtk::gdk_rectangle_is_valid( &_rect ); }

        //@}

        //!@name modifiers
        //@{

        //! type
        void setType( Type value )
        { _type = value; }

        //! rectangle
        void setRect( const GdkRectangle& value )
        { _rect = value; }

        //! clear
        void clear( void )
        {
            _type = Invalid;
            _rect = Gtk::gdk_rectangle();
        }

        //@}

        private:

        //! type
        Type _type;

        //! rectangle
        GdkRectangle _rect;

    };

}
#endif
