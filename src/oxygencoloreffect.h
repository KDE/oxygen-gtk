#ifndef oxygencoloreffect_h
#define oxygencoloreffect_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* inspired notably from kdelibs/kdeui/colors/kcolorscheme.h
* SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
