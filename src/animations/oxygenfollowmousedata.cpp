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

#include "oxygenfollowmousedata.h"

namespace Oxygen
{

    //________________________________________________________________________________
    void FollowMouseData::startAnimation( const GdkRectangle& startRect, const GdkRectangle& endRect )
    {

        // copy end rect
        _endRect = endRect;

        // check timeLine status
        if( _timeLine.isRunning() &&
            _timeLine.value() < 1.0 &&
            Gtk::gdk_rectangle_is_valid( &_endRect ) &&
            Gtk::gdk_rectangle_is_valid( &_animatedRect ) )
        {

            // mark old start rect as part of dirtyRect
            _dirtyRect = _startRect;

            // do some math so that the animation finishes at new endRect without discontinuity
            const double ratio( _timeLine.value()/(1.0-_timeLine.value() ) );
            _startRect.x += double( _animatedRect.x - _endRect.x )*ratio;
            _startRect.y += double( _animatedRect.y - _endRect.y )*ratio;
            _startRect.width += double( _animatedRect.width - _endRect.width )*ratio;
            _startRect.height += double( _animatedRect.height - _endRect.height )*ratio;


        } else {

            if( _timeLine.isRunning() ) _timeLine.stop();
            _startRect = startRect;
            _timeLine.start();

        }

        return;

    }

    //________________________________________________________________________________
    void FollowMouseData::updateAnimatedRect( void )
    {
        if( _timeLine.isRunning() &&
            Gtk::gdk_rectangle_is_valid( &_startRect ) &&
            Gtk::gdk_rectangle_is_valid( &_endRect ) )
        {

            _animatedRect.x = _startRect.x + double( _endRect.x - _startRect.x )*_timeLine.value();
            _animatedRect.y = _startRect.y + double( _endRect.y - _startRect.y )*_timeLine.value();
            _animatedRect.width = _startRect.width + double( _endRect.width - _startRect.width )*_timeLine.value();
            _animatedRect.height = _startRect.height + double( _endRect.height - _startRect.height )*_timeLine.value();

        } else {

            _animatedRect = Gtk::gdk_rectangle();

        }

        return;

    }

    //________________________________________________________________________________
    GdkRectangle FollowMouseData::dirtyRect( void )
    {

        GdkRectangle rect( Gtk::gdk_rectangle() );
        Gtk::gdk_rectangle_union( &_startRect, &_animatedRect, &rect );

        // also union with dirty rect
        if( Gtk::gdk_rectangle_is_valid( &_dirtyRect ) )
        {
            Gtk::gdk_rectangle_union( &_dirtyRect, &rect, &rect );
            _dirtyRect = Gtk::gdk_rectangle();
        }

        return rect;

    }


}
