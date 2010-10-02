#ifndef oxygengtkdetails_h
#define oxygengtkdetails_h
/*
* this file is part of the oxygen gtk engine
* Copyright(c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright(C ) 2008 Robert Staudinger
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include<string>
namespace Gtk
{

    class Detail
    {
        public:

        //! constructor
        Detail( const char* value )
        { if( value ) _value = value; }

        bool isBar( void ) const { return _value == "bar"; }
        bool isBase( void ) const { return _value == "base"; }
        bool isButton( void ) const { return _value == "button"; }

        //!@name treeview cells
        //@{

        bool isCellEven( void ) const { return _value == "cell_even"; }
        bool isCellEvenSorted( void ) const { return _value == "cell_even_sorted"; }
        bool isCellEvenRuled( void ) const { return _value == "cell_even_ruled"; }
        bool isCellOdd( void ) const { return _value == "cell_odd"; }
        bool isCellOddSorted( void ) const { return _value == "cell_odd_sorted"; }
        bool isCellOddRuled( void ) const { return _value == "cell_odd_ruled"; }
        bool isCell( void ) const { return _value.find( "cell_" ) == 0; }

        bool isCellRuled( void ) const { return isCell() && _value.find( "_ruled" ) != std::string::npos; }
        bool isCellStart( void ) const { return isCell() && _value.find( "_start" ) != std::string::npos; }
        bool isCellEnd( void ) const { return isCell() && _value.find( "_end" ) != std::string::npos; }
        bool isCellMiddle( void ) const { return isCell() && _value.find( "_middle" ) != std::string::npos; }

        //@}

        //!@name scrollbars
        //@{
        bool isVScrollBar( void ) const { return _value == "vscrollbar"; }
        bool isHScrollBar( void ) const { return _value == "hscrollbar"; }
        bool isScrollBar( void ) const { return isVScrollBar() || isHScrollBar(); }
        //@}

        //!@name scales
        //@{
        bool isHScale( void ) const { return _value == "hscale"; }
        bool isVScale( void ) const { return _value == "vscale"; }
        bool isScale( void ) const { return isHScale() || isVScale(); }
        //@}

        bool isCellCheck( void ) const { return _value == "cellcheck"; }
        bool isCheck( void ) const { return _value == "check"; }
        bool isCheckButton( void ) const { return _value == "checkbutton"; }
        bool isDefaultButton( void ) const { return _value == "buttondefault"; }
        bool isEntry( void ) const { return _value == "entry"; }
        bool isEntryBg( void ) const { return _value == "entry_bg"; }
        bool isEventBox( void ) const { return _value == "eventbox"; }
        bool isExpander( void ) const { return _value == "expander"; }
        bool isFrame( void ) const { return _value == "frame"; }
        bool isMenu( void ) const { return _value == "menu"; }
        bool isMenuBar( void ) const { return _value == "menubar"; }
        bool isMenuItem( void ) const { return _value == "menuitem"; }
        bool isNotebook( void ) const { return _value == "notebook"; }
        bool isOptionMenu( void ) const { return _value == "optionmenu"; }
        bool isOptionMenuTab( void ) const { return _value == "optionmenutab"; }
        bool isPaned( void ) const { return _value == "paned"; }
        bool isRadioButton( void ) const { return _value == "radiobutton"; }
        bool isScrolledWindow( void ) const { return _value == "scrolled_window"; }

        //!@name spinbuttons
        //@{
        bool isSpinButton( void ) const { return _value == "spinbutton"; }
        bool isSpinButtonUp( void ) const { return _value=="spinbutton_up"; }
        bool isSpinButtonDown( void ) const { return _value=="spinbutton_down"; }
        bool isSpinButtonArrow( void ) const { return isSpinButtonUp() || isSpinButtonDown(); }
        //@}

        bool isToolbar( void ) const { return _value == "toolbar"; }
        bool isTooltip( void ) const { return _value == "tooltip"; }
        bool isTreeView( void ) const { return _value == "treeview"; }
        bool isTrough( void ) const { return _value == "trough"; }
        bool isViewport( void ) const { return _value == "viewport"; }
        bool isViewportBin( void ) const { return _value == "viewportbin"; }



        private:

        //! value
        std::string _value;

    };

}

#endif
