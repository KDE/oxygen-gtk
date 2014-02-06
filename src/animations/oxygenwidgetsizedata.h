#ifndef oxygenwidgetsizedata_h
#define oxygenwidgetsizedata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "../oxygenflags.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    // track main window resize events
    class WidgetSizeData
    {

        public:

        //! constructor
        WidgetSizeData( void ):
            _target(NULL),
            _width(-1),
            _height(-1),
            _alpha(false)
        {}

        //! destructor
        virtual ~WidgetSizeData( void )
        {}

        //! setup connections
        void connect( GtkWidget* widget )
        { _target = widget; }

        //! disconnect
        void disconnect( GtkWidget* )
        { _target = 0L; }

        //! changed flags
        enum ChangedFlag
        {
            SizeChanged = 1<<0,
            AlphaChanged = 1<<1
        };

        OX_DECLARE_FLAGS( ChangedFlags, ChangedFlag )

        //! update widget size and alpha
        /*! return bitmask on what was changed */
        ChangedFlags update();

        //! width
        int width( void ) const
        { return _width; }

        //! height
        int height( void ) const
        { return _height; }

        //! alpha
        bool alpha( void ) const
        { return _alpha; }

        private:

        //! target widget
        GtkWidget* _target;

        //! old width
        int _width;

        //! old height
        int _height;

        //! whether the widget has alpha channel
        bool _alpha;

    };

    OX_DECLARE_OPERATORS_FOR_FLAGS( WidgetSizeData::ChangedFlags )

}

#endif
