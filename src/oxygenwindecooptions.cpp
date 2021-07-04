/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2011 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenwindecooptions.h"
#include "oxygenstyle.h"
#include "oxygenwindowshadow.h"

namespace Oxygen
{
    namespace WinDeco
    {

        gint getMetric(Metric wm)
        {
            const int frameBorder(Style::instance().settings().frameBorder());
            const bool narrowSpacing(false); // TODO: false -> read configuration
            const bool compositingActive(false); // TODO: implement check for this
            const bool hideTitleBar(false); // TODO: implement

            switch (wm)
            {
                case BorderLeft:
                case BorderRight:
                case BorderBottom:
                {
                    gint border(0);
                    // for now we think that respectWindowState is always true,
                    // and WM will remove borders for maximized windows by default
                    // TODO: impelement respectWindowState
                    if( wm==BorderBottom && frameBorder >= QtSettings::BorderNoSide ) {

                        // for tiny border, the convention is to have a larger bottom area in order to
                        // make resizing easier
                        border = std::max(frameBorder,4);
                    }
                    else if( frameBorder < QtSettings::BorderTiny ) {

                        border = 0;
                    }
                    else if( compositingActive && frameBorder == QtSettings::BorderTiny ) {

                        border = std::max( frameBorder, 3 );
                    }
                    else {

                        border = frameBorder;
                    }

                    return border;
                }

                case BorderTop:
                {
                    gint topEdge(0);
                    if( frameBorder == QtSettings::BorderNone && hideTitleBar )
                    {
                        topEdge=0;
                    }
                    else
                    {
                        topEdge=3; // TFRAMESIZE
                    }

                    return Style::instance().settings().buttonSize()+topEdge;
                }

                case ButtonSpacing:
                {
                    return ( narrowSpacing ? 1 : 3 );
                }

                case ButtonMarginTop:
                case ButtonMarginBottom:
                {
                    return 0;
                }

                case ShadowLeft:
                case ShadowRight:
                case ShadowTop:
                case ShadowBottom:
                {
                    WindowShadow shadow(Style::instance().settings(), Style::instance().helper());
                    return int(shadow.shadowSize()-4);
                }

                default:
                {
                    // -1 means we don't understand the query
                    return -1;
                }
            }
        }

        int getButtonSize()
        {
            // in oxygen all buttons have the same size
            return Style::instance().settings().buttonSize();
        }
    }
}
