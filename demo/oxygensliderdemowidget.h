#ifndef oxygensliderdemowidget_h
#define oxygensliderdemowidget_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#include "oxygendemowidget.h"
#include "oxygensignal.h"
#include "oxygentimer.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    class SliderDemoWidget: public DemoWidget
    {

        public:

        //! constructor
        SliderDemoWidget( void );

        //! destructor
        virtual ~SliderDemoWidget( void );

        //! pulse progress bar
        void startPulse( void )
        { if( !_timer.isRunning() ) _timer.start( 50, (GSourceFunc)pulseProgressBar, this ); }

        //! pulse progress bar
        void stopPulse( void )
        { if( _timer.isRunning() ) _timer.stop(); }

        protected:

        //! pulse progress bar
        static gboolean pulseProgressBar( gpointer );

        //! callback
        static void valueChanged( GtkRange*, gpointer );

        private:

        //! widget container
        class Sliders
        {
            public:

            //! constructor
            Sliders( void ):
                _scale( 0 ),
                _progressBar( 0 ),
                _scrollBar( 0 ),
                _progressEntry( 0 )
            {}

            //! change value
            void setValue( const double& value ) const;

            GtkWidget* _scale;
            GtkWidget* _progressBar;
            GtkWidget* _scrollBar;
            GtkWidget* _progressEntry;
        };

        //! horizontal sliders
        Sliders _horizontalSliders;

        //! vertical sliders
        Sliders _verticalSliders;

        //! pulse progressbar timer
        Timer _timer;

        //! pulse progress bar
        GtkWidget* _pulseProgressBar;
    };

}

#endif
