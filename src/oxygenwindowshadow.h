#ifndef oxygenshadow_h
#define oxygenshadow_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygencairocontext.h"
#include "oxygenqtsettings.h"
#include "oxygenstylehelper.h"
#include "oxygenwindecooptions.h"
#include "oxygenshadowconfiguration.h"
#include <cmath>

namespace Oxygen
{
    class WindowShadow
    {

        // TODO: implement opacity
        public:

        //! constructor
        WindowShadow( const QtSettings& settings, StyleHelper& helper ):
            _settings( settings ),
            _helper(helper),
            inactiveShadowConfiguration_(settings.shadowConfiguration(Palette::Inactive)),
            activeShadowConfiguration_(settings.shadowConfiguration(Palette::Active))
        {}

        //! destructor
        virtual ~WindowShadow()
        {}

        void render(cairo_t*, gint x, gint y, gint w, gint h);

        // shadow tiles
        const TileSet& tileSet(const ColorUtils::Rgba& color, WindowShadowKey) const;

        void setWindowState(WinDeco::Options wopt)
        { _wopt=wopt; }

        //! shadow size
        double shadowSize() const
        {
            double activeSize( activeShadowConfiguration_.isEnabled() ? activeShadowConfiguration_.shadowSize() : 0 );
            double inactiveSize( inactiveShadowConfiguration_.isEnabled() ? inactiveShadowConfiguration_.shadowSize() : 0 );
            double size( std::max( activeSize, inactiveSize ) );

            // even if shadows are disabled,
            // you need a minimum size to allow corner rendering
            return std::max(size,5.0);
        }

        //! overlap between shadow pixmap and contents body
        enum { Overlap = 4 };

        protected:

        //! simple pixmap
        Cairo::Surface shadowPixmap(const ColorUtils::Rgba& color, const WindowShadowKey& ) const;

        //! settings
        const QtSettings& settings( void ) const
        { return _settings; }

        //! helper
        StyleHelper& helper( void ) const
        { return _helper; }

        //! square utility function
        static double square(double x)
        { return x*x; }

        //! functions used to draw shadows
        class Parabolic
        {
            public:

            //! constructor
            Parabolic( double amplitude, double width ):
                amplitude_( amplitude ),
                width_( width )
            {}

            //! destructor
            virtual ~Parabolic( void )
            {}

            //! value
            virtual double operator() ( double x ) const
            { return std::max( 0.0, amplitude_*(1.0 - square(x/width_) ) ); }

            private:

            double amplitude_;
            double width_;

        };

        //! functions used to draw shadows
        class Gaussian
        {
            public:

            //! constructor
            Gaussian( double amplitude, double width ):
                amplitude_( amplitude ),
                width_( width )
            {}

            //! destructor
            virtual ~Gaussian( void )
            {}

            //! value
            virtual double operator() ( double x ) const
            { return std::max( 0.0, amplitude_*(std::exp( -square(x/width_) -0.05 ) ) ); }

            private:

            double amplitude_;
            double width_;

        };

        private:

        //! draw gradient into rect
        /*! a separate method is used in order to properly account for corners */
        void renderGradient(cairo_t*,const GdkRectangle&,cairo_pattern_t*, bool hasTopBorder=true, bool hasBottomBorder = true ) const;

        //! settings
        const QtSettings& _settings;

        //! helper
        StyleHelper& _helper;

        //! window state
        WinDeco::Options _wopt;

        //! inactive shadow configuration
        ShadowConfiguration inactiveShadowConfiguration_;

        //! active shadow configuration
        ShadowConfiguration activeShadowConfiguration_;

    };

}

#endif
