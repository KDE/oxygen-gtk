#ifndef oxygencachekey_h
#define oxygencachekey_h

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

namespace Oxygen
{

    //! key for separators
    /*! keys are used to store tilesets into cache */
    class SeparatorKey
    {
        public:

        //! constructor
        SeparatorKey( const ColorUtils::Rgba& color, bool vertical, int size ):
            _color( color.toInt() ),
            _vertical( vertical ),
            _size( size )
        {}

        //! equal to operator
        bool operator == (const SeparatorKey& other ) const
        {
            return
                _color == other._color &&
                _vertical == other._vertical &&
                _size == other._size;
        }

        //! less than operator
        bool operator < (const SeparatorKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else if( _vertical != other._vertical ) return _vertical < other._vertical;
            else return _size < other._size;
        }

        private:

        guint32 _color;
        bool _vertical;
        int _size;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const SeparatorKey& key )
        {
            out << "SeparatorKey - color: " << key._color << " shade: " << key._vertical << " size: " << key._size;
            return out;
        }

    };

    //! key for slabs
    /*! keys are used to store tilesets into cache */
    class SlabKey
    {
        public:

        //! constructor
        SlabKey( const ColorUtils::Rgba& color, double shade, int size ):
            _color( color.toInt() ),
            _shade( shade ),
            _size( size )
        {}

        //! equal to operator
        bool operator == (const SlabKey& other ) const
        {
            return
                _color == other._color &&
                _shade == other._shade &&
                _size == other._size;
        }

        //! less than operator
        bool operator < (const SlabKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else if( _shade != other._shade ) return _shade < other._shade;
            else return _size < other._size;
        }

        private:

        guint32 _color;
        double _shade;
        int _size;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const SlabKey& key )
        {
            out << "SlabKey - color: " << key._color << " shade: " << key._shade << " size: " << key._size;
            return out;
        }

    };

    //! key for focused slabs
    class SlabFocusedKey
    {
        public:

        //! constructor
        SlabFocusedKey( const ColorUtils::Rgba& color, const ColorUtils::Rgba& glow, double shade, int size ):
            _color( color.toInt() ),
            _glow( glow.toInt() ),
            _shade( shade ),
            _size( size )
        {}

        //! equal to operator
        bool operator == (const SlabFocusedKey& other ) const
        {
            return
                _color == other._color &&
                _glow == other._glow &&
                _shade == other._shade &&
                _size == other._size;
        }

        //! less than operator
        bool operator < (const SlabFocusedKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else if( _glow != other._glow ) return _glow < other._glow;
            else if( _shade != other._shade ) return _shade < other._shade;
            else return _size < other._size;
        }

        private:

        guint32 _color;
        guint32 _glow;
        double _shade;
        int _size;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const SlabFocusedKey& key )
        {
            out << "SlabFocusedKey - color: " << key._color << " glow: " << key._glow << " shade: " << key._shade << " size: " << key._size;
            return out;
        }
    };

    //! key for holes
    class HoleKey
    {
        public:

        //! constructor
        HoleKey( const ColorUtils::Rgba& color, const ColorUtils::Rgba& fill, double shade, int size ):
            _color( color.toInt() ),
            _fill( fill.toInt() ),
            _shade( shade ),
            _size( size ),
            _filled( fill.isValid() )
        {}

        //! equal to operator
        bool operator == (const HoleKey& other ) const
        {
            return
                _color == other._color &&
                _shade == other._shade &&
                _size == other._size &&
                _filled == other._filled &&
                (_fill == other._fill || !_filled );
        }

        //! less than operator
        bool operator < (const HoleKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else if( _shade != other._shade ) return _shade < other._shade;
            else if( _size != other._size ) return _size < other._size;
            else if( _filled != other._filled ) return !_filled;
            else if( _filled && _fill != other._fill ) return _fill < other._fill;
            else return false;
        }

        private:

        guint32 _color;
        guint32 _fill;
        double _shade;
        int _size;
        bool _filled;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const HoleKey& key )
        {
            out << "HoleKey - color: " << key._color << " fill: " << key._fill << " shade: " << key._shade << " size: " << key._size << " filled: " << key._filled;
            return out;
        }

    };

    //! key for holes
    class HoleFocusedKey
    {
        public:

        //! constructor
        HoleFocusedKey( const ColorUtils::Rgba& color, const ColorUtils::Rgba& fill, const ColorUtils::Rgba& glow, double shade, int size ):
            _color( color.toInt() ),
            _fill( fill.toInt() ),
            _glow( glow.toInt() ),
            _shade( shade ),
            _size( size ),
            _filled( fill.isValid() )
        {}

        //! equal to operator
        bool operator == (const HoleFocusedKey& other ) const
        {
            return
                _color == other._color &&
                _glow == other._glow &&
                _shade == other._shade &&
                _size == other._size &&
                _filled == other._filled &&
                (_fill == other._fill || !_filled );
        }

        //! less than operator
        bool operator < (const HoleFocusedKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            if( _glow != other._glow ) return _glow < other._glow;
            else if( _shade != other._shade ) return _shade < other._shade;
            else if( _size != other._size ) return _size < other._size;
            else if( _filled != other._filled ) return !_filled;
            else if( _filled && _fill != other._fill ) return _fill < other._fill;
            else return false;
        }

        private:

        guint32 _color;
        guint32 _fill;
        guint32 _glow;
        double _shade;
        int _size;
        bool _filled;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const HoleFocusedKey& key )
        {
            out << "HoleFocusedKey - color: " << key._color << " glow: " << key._glow << " fill: " << key._fill << " shade: " << key._shade << " size: " << key._size << " filled: " << key._filled;
            return out;
        }

    };

    //! key for flat holes
    typedef SlabKey HoleFlatKey;

    //! key for scroll holes
    class ScrollHoleKey
    {
       public:

        //! constructor
        ScrollHoleKey( const ColorUtils::Rgba& color, bool vertical ):
            _color( color.toInt() ),
            _vertical( vertical )
        {}

        //! equal to operator
        bool operator == (const ScrollHoleKey& other ) const
        {
            return
                _color == other._color &&
                _vertical == other._vertical;
        }

        //! less than operator
        bool operator < (const ScrollHoleKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else return _vertical < other._vertical;
        }

        private:

        guint32 _color;
        bool _vertical;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const ScrollHoleKey& key )
        {
            out << "ScrollHoleKey - color: " << key._color << "vertical: " << (key._vertical ? "true":"false");
            return out;
        }

    };

    //@}

    //! key for focused slits
    class SlitFocusedKey
    {
       public:

        //! constructor
        SlitFocusedKey( const ColorUtils::Rgba& color ):
            _color( color.toInt() )
        {}

        //! equal to operator
        bool operator == (const SlitFocusedKey& other ) const
        { return _color == other._color; }

        //! less than operator
        bool operator < (const SlitFocusedKey& other ) const
        { return _color < other._color; }

        private:

        guint32 _color;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const SlitFocusedKey& key )
        {
            out << "SlitFocusedKey - color: " << key._color;
            return out;
        }

    };

    //@}

    //! key for docks
    class DockFrameKey
    {
       public:

        //! constructor
        DockFrameKey( const ColorUtils::Rgba& color, int size ):
            _color( color.toInt() ),
            _size( size )
        {}

        //! equal to operator
        bool operator == (const DockFrameKey& other ) const
        {
            return
                _color == other._color &&
                _size == other._size;
        }

        //! less than operator
        bool operator < (const DockFrameKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else return _size < other._size;
        }

        private:

        guint32 _color;
        int _size;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const DockFrameKey& key )
        {
            out << "DockFrameKey - color: " << key._color << " size: " << key._size;
            return out;
        }

    };

    //! key for groove
    typedef SlabKey GrooveKey;


    //! key for selection rects
    class SelectionKey
    {
       public:

        //! constructor
        SelectionKey( const ColorUtils::Rgba& color, int size, bool custom ):
            _color( color.toInt() ),
            _size( size ),
            _custom( custom )
        {}

        //! equal to operator
        bool operator == (const SelectionKey& other ) const
        {
            return
                _color == other._color &&
                _size == other._size &&
                _custom == other._custom;
        }

        //! less than operator
        bool operator < (const SelectionKey& other ) const
        {

            if( _color != other._color ) return _color < other._color;
            else if( _size != other._size ) return _size < other._size;
            else return _custom < other._custom;

        }

        private:

        guint32 _color;
        int _size;
        bool _custom;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const SelectionKey& key )
        {
            out << "SelectionKey - color: " << key._color << " size: " << key._size << " custom: " << ( key._custom ? "true":"false" );
            return out;
        }

    };

    //! key for progressbar indicator
    class ProgressBarIndicatorKey
    {
        public:

        //! constructor
        ProgressBarIndicatorKey( const ColorUtils::Rgba& color, const ColorUtils::Rgba& glow, int width, int height ):
            _color( color.toInt() ),
            _glow( glow.toInt() ),
            _width( width ),
            _height( height )
        {}

        //! equal to operator
        bool operator == (const ProgressBarIndicatorKey& other ) const
        {
            return
                _color == other._color &&
                _glow == other._glow &&
                _width == other._width &&
                _height == other._height;
        }

        //! less than operator
        bool operator < (const ProgressBarIndicatorKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else if( _glow != other._glow ) return _glow < other._glow;
            else if( _width != other._width ) return _width < other._width;
            else return _height < other._height;
        }

        private:

        guint32 _color;
        guint32 _glow;
        int _width;
        int _height;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const ProgressBarIndicatorKey& key )
        {
            out << "ProgressBarIndicatorKey - color: " << key._color << " glow: " << key._glow << " width: " << key._width << " height: " << key._height;
            return out;
        }
    };

    //! key for windeco buttons
    /*! keys are used to store tilesets into cache */
    class WindecoButtonKey
    {
        public:

        //! constructor
        WindecoButtonKey( const ColorUtils::Rgba& color, int size, bool pressed ):
            _color( color.toInt() ),
            _size( size ),
            _pressed( pressed )
        {}

        //! equal to operator
        bool operator == (const WindecoButtonKey& other ) const
        {
            return
                _color == other._color &&
                _size == other._size &&
                _pressed == other._pressed;
        }

        //! less than operator
        bool operator < (const WindecoButtonKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else if( _size != other._size ) return _size < other._size;
            else return _pressed < other._pressed;
        }

        private:

        guint32 _color;
        int _size;
        bool _pressed;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const WindecoButtonKey& key )
        {
            out << "WindecoButtonKey - color: " << key._color << " size: " << key._size << " pressed: " << key._pressed;
            return out;
        }

    };

    //! key for windeco buttons
    /*! keys are used to store tilesets into cache */
    class WindecoButtonGlowKey
    {
        public:

        //! constructor
        WindecoButtonGlowKey( const ColorUtils::Rgba& color, int size ):
            _color( color.toInt() ),
            _size( size )
        {}

        //! equal to operator
        bool operator == (const WindecoButtonGlowKey& other ) const
        {
            return
                _color == other._color &&
                _size == other._size;
        }

        //! less than operator
        bool operator < (const WindecoButtonGlowKey& other ) const
        {
            if( _color != other._color ) return _color < other._color;
            else return _size < other._size;
        }

        private:

        guint32 _color;
        int _size;

        //! streamer
        friend std::ostream& operator << ( std::ostream& out, const WindecoButtonGlowKey& key )
        {
            out << "WindecoButtonGlowKey - color: " << key._color << " size: " << key._size;
            return out;
        }

    };


}

#endif
