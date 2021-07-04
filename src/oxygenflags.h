#ifndef oxygenflags_h
#define oxygenflags_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* largely inspired from Flags
* SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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

    //! bitwise or
    template<typename Enum>
    Flags<Enum> operator|(Enum f1, Enum f2)
    { return Flags<Enum>(f1) | f2; }

    //! used for bitwise or between incompatible flags
    class IncompatibleFlag
    {
        public:

        //! constructor
        inline explicit IncompatibleFlag(int ai):
            i(ai)
            {}

        //! cast to int
        inline operator int() const
        { return i; }

        private:
        int i;

    };

}

#define OX_DECLARE_FLAGS(F, Enum)\
typedef Flags<Enum> F;

#define OX_DECLARE_INCOMPATIBLE_FLAGS(F) \
inline Oxygen::IncompatibleFlag operator|(F::enum_type f1, int f2) \
{ return Oxygen::IncompatibleFlag(int(f1) | f2); }


#define OX_DECLARE_OPERATORS_FOR_FLAGS(F) \
inline Oxygen::Flags<F::enum_type> operator|(F::enum_type f1, F::enum_type f2) \
{ return Oxygen::Flags<F::enum_type>(f1) | f2; } \
inline Oxygen::Flags<F::enum_type> operator|(F::enum_type f1, Oxygen::Flags<F::enum_type> f2) \
{ return f2 | f1; } \
OX_DECLARE_INCOMPATIBLE_FLAGS(F)

#endif
