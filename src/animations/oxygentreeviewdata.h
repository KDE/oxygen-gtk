#ifndef oxygentreeviewdata_h
#define oxygentreeviewdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
#include "../oxygengtkcellinfo.h"
#include "../oxygengtkutils.h"
#include "oxygenhoverdata.h"
#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <algorithm>

namespace Oxygen
{
    class TreeViewData: public HoverData
    {

        public:

        //! constructor
        TreeViewData( void ):
            _fullWidth( false )
        {}

        //! destructor
        virtual ~TreeViewData( void )
        {}

        //! full width flag
        void setFullWidth( bool value )
        { _fullWidth = value; }

        //! update hovered cell using stored position
        void setHoveredCell( const Gtk::CellInfo& cellInfo )
        { _cellInfo = cellInfo; }

        //! true if cell info is hovered
        bool isCellHovered( const Gtk::CellInfo& cellInfo ) const
        { return isCellHovered( cellInfo, _fullWidth ); }

        //! true if cell info is hovered
        bool isCellHovered( const Gtk::CellInfo& cellInfo, bool fullWidth ) const
        { return hovered() && (fullWidth || cellInfo.sameColumn( _cellInfo ) ) && cellInfo.samePath( _cellInfo ); }

        private:

        //! true if hover works on full width
        bool _fullWidth;

        //! keep track of the hovered path and column
        Gtk::CellInfo _cellInfo;

    };

}

#endif
