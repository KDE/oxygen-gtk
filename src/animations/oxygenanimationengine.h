#ifndef oxygenanimationengine_h
#define oxygenanimationengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../oxygenapplicationname.h"

namespace Oxygen
{

    class AnimationEngine
    {

        public:

        //! constructor
        explicit AnimationEngine( void ):
            _duration( 150 )
        {}

        //! destructor
        virtual ~AnimationEngine( void )
        {}

        //! application name
        virtual void setApplicationName( const ApplicationName& applicationName )
        { _applicationName = applicationName; }

        //! duration
        const int& duration( void ) const
        { return _duration; }

        //! duration
        /*! returns true if changed */
        bool setDuration( int value )
        {
            if( _duration == value ) return false;
            _duration = value;
            return true;
        }

        protected:

        //! returs true if widget is black listed (based notably on application name)
        bool widgetIsBlackListed( GtkWidget* widget ) const
        {

            // for now, only Mozilla applications need blacklisting
            return _applicationName.isXul( widget );

        }

        private:

        //! application name
        ApplicationName _applicationName;

        //! animation duration
        int _duration;

    };
}

#endif
