#ifndef oxygengtkdetails_h
#define oxygengtkdetails_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
namespace Oxygen
{
    namespace Gtk
    {

        class Detail
        {
            public:

            //! constructor
            explicit Detail( const char* value = 0L )
            { if( value ) _value = value; }

            //! true when no valid value was passed
            bool isNull( void ) const { return _value.empty(); }
            bool is( const std::string value ) const
            { return _value == value; }

            bool isArrow( void ) const { return _value == "arrow"; }
            bool isBar( void ) const { return _value == "bar"; }
            bool isBase( void ) const { return _value == "base"; }
            bool isButton( void ) const { return _value == "button"; }

            //!@name treeview cells
            //@{

            bool isCellEvenSorted( void ) const { return _value == "cell_even_sorted"; }
            bool isCellEvenRuled( void ) const { return _value == "cell_even_ruled"; }
            bool isCellOddSorted( void ) const { return _value == "cell_odd_sorted"; }
            bool isCellOddRuled( void ) const { return _value == "cell_odd_ruled"; }
            bool isCell( void ) const { return _value.find( "cell_" ) == 0; }

            bool isCellOdd( void ) const { return isCell() && _value.find( "_odd" ) != std::string::npos; }
            bool isCellEven( void ) const { return isCell() && _value.find( "_even" ) != std::string::npos; }

            bool isCellRuled( void ) const { return isCell() && _value.find( "_ruled" ) != std::string::npos; }
            bool isCellStart( void ) const { return isCell() && _value.find( "_start" ) != std::string::npos; }
            bool isCellEnd( void ) const { return isCell() && _value.find( "_end" ) != std::string::npos; }
            bool isCellMiddle( void ) const { return isCell() && _value.find( "_middle" ) != std::string::npos; }

            bool isCellRadio( void ) const { return _value == "cellradio"; }
            //@}

            bool isDragAndDrop( void ) const { return _value == "dnd"; }

            //!@name scrollbars
            //@{
            bool isHScrollBar( void ) const { return _value == "hscrollbar"; }
            bool isVScrollBar( void ) const { return _value == "vscrollbar"; }
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
            bool isDockItem( void ) const { return _value == "dockitem"; }
            bool isInfoBar( void ) const { return _value.find( "infobar" ) == 0; }
            bool isEntry( void ) const { return _value == "entry"; }
            bool isEntryBg( void ) const { return _value == "entry_bg"; }
            bool isEntryProgress( void ) const { return _value == "entry-progress"; }
            bool isEventBox( void ) const { return _value == "eventbox"; }
            bool isExpander( void ) const { return _value == "expander"; }
            bool isFrame( void ) const { return _value == "frame"; }
            bool isHandleBox( void ) const { return _value == "handlebox" || _value == "handlebox_bin"; }
            bool isMenu( void ) const { return _value == "menu"; }
            bool isMenuBar( void ) const { return _value == "menubar"; }
            bool isMenuItem( void ) const { return _value == "menuitem"; }
            bool isMenuScrollArrow( void ) const { return _value == "menu_scroll_arrow_down" || _value == "menu_scroll_arrow_up"; }
            bool isNotebook( void ) const { return _value == "notebook"; }
            bool isOption( void ) const { return _value == "option"; }
            bool isOptionMenu( void ) const { return _value == "optionmenu"; }
            bool isOptionMenuTab( void ) const { return _value == "optionmenutab"; }
            bool isPaned( void ) const { return _value == "paned"; }
            bool isRadioButton( void ) const { return _value == "radiobutton"; }
            bool isProgressBar( void) const { return _value == "progressbar"; }

            //!@name rulers
            //@{
            bool isRuler( void ) const { return _value == "vruler" || _value == "hruler"; }
            //@}

            bool isScrolledWindow( void ) const { return _value == "scrolled_window"; }
            bool isSlider( void ) const { return _value == "slider"; }

            //!@name spinbuttons
            //@{
            bool isSpinButton( void ) const { return _value == "spinbutton"; }
            bool isSpinButtonUp( void ) const { return _value=="spinbutton_up"; }
            bool isSpinButtonDown( void ) const { return _value=="spinbutton_down"; }
            bool isSpinButtonArrow( void ) const { return isSpinButtonUp() || isSpinButtonDown(); }
            //@}

            bool isStepper( void ) const { return _value == "stepper"; }

            bool isTab( void ) const { return _value == "tab"; }
            bool isTearOffMenuItem( void ) const { return _value == "tearoffmenuitem"; }
            bool isToggleButton( void ) const { return _value == "togglebutton"; }
            bool isToolBar( void ) const { return _value == "toolbar"; }
            bool isTooltip( void ) const { return _value == "tooltip"; }
            bool isTreeView( void ) const { return _value == "treeview"; }

            //!@name through
            //@{
            bool isTroughFillLevel( void ) const { return _value == "trough-fill-level"; }
            bool isTroughLower( void ) const { return _value == "trough-lower"; }
            bool isTroughUpper( void ) const { return _value == "trough-upper"; }
            bool isTrough( void ) const { return _value == "trough"; }
            bool isTroughAny( void ) const { return isTrough()||isTroughLower()||isTroughUpper(); }
            //@}

            bool isViewport( void ) const { return _value == "viewport"; }
            bool isViewportBin( void ) const { return _value == "viewportbin"; }

            private:

            //! value
            std::string _value;

        };

    }

}
#endif
