#ifndef oxygensliderdemowidget_h
#define oxygensliderdemowidget_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
