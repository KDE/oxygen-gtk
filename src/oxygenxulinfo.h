#ifndef oxygenxulinfo_h
#define oxygenxulinfo_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
