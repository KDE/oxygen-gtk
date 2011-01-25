#ifndef oxygenpalette_h
#define oxygenpalette_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenrgba.h"

#include <vector>
#include <map>

namespace Oxygen
{

    // forward declarations
    namespace ColorUtils
    { class Effect; }

    //! store colors for all Color groups and roles
    class Palette
    {

        public:

        //! color roles
        enum Role
        {
            Base,
            BaseAlternate,
            Button,
            Selected,
            Window,
            Tooltip,
            Text,
            NegativeText,
            ButtonText,
            SelectedText,
            WindowText,
            TooltipText,
            Focus,
            Hover,

            // these two are use for drawFloatFrame
            ActiveWindowBackground,
            InactiveWindowBackground,

            NumColors
        };

        //! color groups
        enum Group
        {
            Active,
            Inactive,
            Disabled
        };

        //! color list
        typedef std::vector<ColorUtils::Rgba> ColorList;

        //! color set
        class ColorSet: public std::map<Role, ColorUtils::Rgba>
        {

            public:

            //! insert
            void insert( Role role, const ColorUtils::Rgba& color )
            { std::map<Role, ColorUtils::Rgba>::insert( std::make_pair( role, color ) ); }

            //! returns true if color set contains given Role
            bool contains( Role role ) const
            { return find( role ) != end(); }

        };

        //! constructor
        Palette( void ):
            _activeColors( NumColors, ColorUtils::Rgba() ),
            _inactiveColors( NumColors, ColorUtils::Rgba() ),
            _disabledColors( NumColors, ColorUtils::Rgba() ),
            _group( Active )
        {}

        //! clear
        void clear( void )
        {
            _activeColors = ColorList( NumColors, ColorUtils::Rgba() );
            _inactiveColors = ColorList( NumColors, ColorUtils::Rgba() );
            _disabledColors = ColorList( NumColors, ColorUtils::Rgba() );
        }

        //! get color
        const ColorUtils::Rgba& color( Role role ) const
        { return colorList( _group )[role]; }

        //! get color
        const ColorUtils::Rgba& color( Group group, Role role ) const
        { return colorList( group )[role]; }

        //! set current group
        void setGroup( Group value )
        { _group = value; }

        //! set current
        void setColor( Group group, Role role, const ColorUtils::Rgba& value )
        { colorList( group )[role] = value; }

        //! copy on group onto the other
        void copy( Group from, Group to )
        { colorList(to) = colorList(from); }

        //! generate group from input, using provided effect
        void generate( Group from, Group to, const ColorUtils::Effect&, bool changeSelectionColor = false );

        //! get string for role
        static std::string groupName( const Group& group )
        {
            switch( group )
            {
                case Active: return "Active";
                case Inactive: return "Inactive";
                case Disabled: return "Disabled";
                default: return "unknown";
            }
        }

        //! get string for role
        static std::string roleName( const Role& role )
        {
            switch( role )
            {
                case Base: return "Base";
                case BaseAlternate: return "BaseAlternate";
                case Button: return "Button";
                case Selected: return "Selected";
                case Window: return "Window";
                case Tooltip: return "Tooltip";
                case Text: return "Text";
                case NegativeText: return "NegativeText";
                case ButtonText: return "ButtonText";
                case SelectedText: return "SelectedText";
                case WindowText: return "WindowText";
                case TooltipText: return "TooltipText";
                case Focus: return "Focus";
                case Hover: return "Hover";
                case ActiveWindowBackground: return "ActiveWindowBackground";
                case InactiveWindowBackground: return "InactiveWindowBackground";
                default: return "unknown";
            }
        }

        protected:

        //! get color list from group
        const ColorList& colorList( Group group ) const
        {
            switch( group )
            {
                default:
                case Active: return _activeColors;
                case Inactive: return _inactiveColors;
                case Disabled: return _disabledColors;
            }
        }

        //! get color list from group
        ColorList& colorList( Group group )
        {
            switch( group )
            {
                default:
                case Active: return _activeColors;
                case Inactive: return _inactiveColors;
                case Disabled: return _disabledColors;
            }
        }

        private:

        ColorList _activeColors;
        ColorList _inactiveColors;
        ColorList _disabledColors;

        //! current group
        Group _group;

        //! streamer for color list
        friend std::ostream& operator << ( std::ostream& out, const ColorList& colors );
        friend std::ostream& operator << ( std::ostream& out, const ColorSet& colors );
        friend std::ostream& operator << ( std::ostream& out, const Palette& palette );

    };

}

#endif
