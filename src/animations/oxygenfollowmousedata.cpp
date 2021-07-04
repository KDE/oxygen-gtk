/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
