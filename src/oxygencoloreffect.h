#ifndef oxygencoloreffect_h
#define oxygencoloreffect_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* inspired notably from kdelibs/kdeui/colors/kcolorscheme.h
* Copyright (c) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
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

#include "oxygenpalette.h"
#include "oxygenrgba.h"

#include <iostream>

namespace Oxygen
{

    // forward declaration
    class OptionMap;

    namespace ColorUtils
    {

        //! used to handle color modification effects
        /*! used notably to generate the correct disabled palette from kde options */
        class Effect
        {

            public:

            //!constructor
            explicit Effect( void ):
                _intensityEffect( IntensityNoEffect ),
                _intensityEffectAmount(0),
                _colorEffect( ColorNoEffect ),
                _colorEffectAmount(0),
                _contrastEffect( ContrastNoEffect ),
                _contrastEffectAmount(0),
                _enabled( false )
            {}

            //! constructor from option map
            explicit Effect( Palette::Group, const OptionMap& );

            //! destructor
            virtual ~Effect( void )
            {}

            //! apply effect to input background
            Rgba color( const Rgba& background ) const;

            //! apply effect to input foreground
            Rgba color( const Rgba& foreground, const Rgba& background ) const;

            private:

            //!@name intensity
            //@{

            enum IntensityEffect
            {
                IntensityNoEffect,
                IntensityShade,
                IntensityDarken,
                IntensityLighten,
            };

            IntensityEffect _intensityEffect;
            double _intensityEffectAmount;

            //@}

            //!@name color
            //@{

            enum ColorEffect
            {
                ColorNoEffect,
                ColorDesaturate,
                ColorFade,
                ColorTint
            };

            Rgba _color;
            ColorEffect _colorEffect;
            double _colorEffectAmount;

            //@}

            //!@name contrast
            //@{

            enum ContrastEffect
            {
                ContrastNoEffect,
                ContrastFade,
                ContrastTint
            };

            ContrastEffect _contrastEffect;
            double _contrastEffectAmount;

            //@}

            //! enable state
            bool _enabled;

            // streamer
            friend std::ostream& operator << (std::ostream&, const Effect& );

        };

    }
}
#endif
