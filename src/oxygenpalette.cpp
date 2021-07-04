/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    inspired notably from kdelibs/kdeui/color/kcolorscheme.h
    SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenpalette.h"
#include "oxygencoloreffect.h"
#include "oxygencolorutils.h"

namespace Oxygen
{

    //____________________________________________________________________________________
    std::ostream& operator << ( std::ostream& out, const Palette::ColorList& colors )
    {
        for( unsigned int i=0; i<colors.size(); i++ )
        { out << Palette::roleName( Palette::Role(i) ) << "=" << colors[i] << std::endl; }

        return out;

    }

    //____________________________________________________________________________________
    std::ostream& operator << ( std::ostream& out, const Palette::ColorSet& colors )
    {
        for( Palette::ColorSet::const_iterator iter = colors.begin(); iter != colors.end(); ++iter )
        { out << Palette::roleName( iter->first ) << "=" << iter->second << std::endl; }

        return out;

    }

    //____________________________________________________________________________________
    std::ostream& operator << ( std::ostream& out, const Palette& palette )
    {
        out << "[Colors:" << Palette::groupName( Palette::Active ) << "]" << std::endl;
        out << palette._activeColors << std::endl;

        out << "[Colors:" << Palette::groupName( Palette::Inactive ) << "]" << std::endl;
        out << palette._inactiveColors << std::endl;

        out << "[Colors:" << Palette::groupName( Palette::Disabled ) << "]" << std::endl;
        out << palette._disabledColors << std::endl;

        return out;
    }

    //_______________________________________________________
    void Palette::generate( Group from, Group to, const ColorUtils::Effect& effect, bool changeSelectionColor )
    {
        // start by copying palette
        colorList(to) = colorList(from);

        const ColorList& source = colorList(from);
        ColorList &destination = colorList(to);

        // apply effects to background
        destination[Window] = effect.color(source[Window]);
        destination[Button] = effect.color(source[Button]);
        destination[Base] = effect.color(source[Base]);
        destination[BaseAlternate] = effect.color(source[BaseAlternate]);

        if( changeSelectionColor ) destination[Selected] = effect.color( ColorUtils::tint( source[Window], source[Selected], 0.4 ) );
        else destination[Selected] = effect.color( source[Selected] );

        // apply effects to text
        destination[WindowText] = effect.color( effect.color( source[WindowText], source[Window] ) );
        destination[ButtonText] = effect.color( effect.color( source[ButtonText], source[Button] ) );
        destination[Text] = effect.color( effect.color(source[Text], source[Base] ) );

        // apply effects to focus
        destination[Focus] = effect.color(source[Focus], source[Base] );
        destination[Hover] = effect.color(source[Hover], source[Base] );

    }

}
