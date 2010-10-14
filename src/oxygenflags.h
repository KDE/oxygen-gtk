#ifndef oxygenflags_h
#define oxygenflags_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* largely inspired from Flags
* Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

namespace Oxygen
{

    template<typename Enum>
    class Flags
    {

        public:

        typedef Enum enum_type;
        typedef unsigned long ValueType;

        //! copy constructor
        Flags( const Flags &f ):
            i(f.i)
        {}

        //! constructor
        Flags(Enum f):
            i(f)
        {}

        //! constructor
        Flags():
            i(0)
        {}

        //! destructor
        virtual ~Flags( void )
        {}

        //! assignment
        Flags &operator=(const Flags &f)
        {
            i = f.i;
            return *this;
        }

        //!@name binary operators and assignment
        //@{
        Flags &operator&=( ValueType mask)
        {
            i &= mask;
            return *this;
        }

        Flags &operator|=(Flags f)
        {
            i |= f.i;
            return *this;
        }

        Flags &operator|=(Enum f)
        {
            i |= f;
            return *this;
        }

        Flags &operator^=(Flags f)
        {
            i ^= f.i;
            return *this;
        }

        Flags &operator^=(Enum f)
        {
            i ^= f;
            return *this;
        }

        //@}

        //! conversion to int
        operator ValueType() const
        { return i; }

        //!@name binary operations (non assigned)
        //@{
        Flags operator|(Flags f) const
        {
            Flags g;
            g.i = i | f.i;
            return g;
        }

        Flags operator|(Enum f) const
        {
            Flags g;
            g.i = i | f;
            return g;
        }

        Flags operator^(Flags f) const
        {
            Flags g;
            g.i = i ^ f.i;
            return g;
        }

        Flags operator^(Enum f) const
        {
            Flags g;
            g.i = i ^ f;
            return g;
        }

        Flags operator&(ValueType mask) const
        {
            Flags g;
            g.i = i & mask;
            return g;
        }

        Flags operator&(Enum f) const
        {
            Flags g;
            g.i = i & f;
            return g;
        }

        Flags operator~() const
        {
            Flags g;
            g.i = ~i;
            return g;
        }
        //@}

        bool operator!() const
        { return !i; }

        bool testFlag(Enum f) const
        { return (i & f) == f && (f != 0 || i == ValueType(f) ); }

        private:
        ValueType i;

    };

    template<typename Enum>
    Flags<Enum> operator|(Enum f1, Enum f2)
    { return Flags<Enum>(f1) | f2; }

}
#endif
