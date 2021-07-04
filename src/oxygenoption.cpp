/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2012 Ruslan Kabatsayev <b7.10110111@gmail.com>

    inspired notably from kdelibs/kdeui/color/kcolorutils.h
    SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
    SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
    SPDX-FileCopyrightText: 2007 Zack Rusin <zack@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenoption.h"

namespace Oxygen
{
    //_______________________________________________________________________
    template<> std::string Option::toVariant( std::string defaultValue ) const
    {

        std::string out;
        std::istringstream stream( _value );
        return ( std::getline(stream, out) ) ? out:defaultValue;
    }

}
