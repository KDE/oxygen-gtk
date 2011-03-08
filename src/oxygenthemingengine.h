#ifndef oxygenthemingengine_h
#define oxygenthemingengine_h
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

#include <gtk/gtk.h>

// oxygen theming engine struct
struct OxygenThemingEngine
{ GtkThemingEngine parent; };

// oxygen theming engine class struct
struct OxygenThemingEngineClass
{  GtkThemingEngineClass parent; };

namespace Oxygen
{
    class ThemingEngine
    {

        public:

        //! type registration
        static void registerType( GTypeModule* );

        //! registered type
        static GType type( void );

        //! parent class
        inline static GtkThemingEngineClass* parentClass( void )
        { return _parentClass; }

        protected:

        //! instance initialization
        static void instanceInit( OxygenThemingEngine* );

        //! class initialization
        static void classInit( OxygenThemingEngineClass* );

        private:

        //! parent class
        static GtkThemingEngineClass* _parentClass;

        //! registered type indo
        static GTypeInfo _typeInfo;

        //! registered type
        static GType _type;

    };
}

#endif
