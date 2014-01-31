#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#define GTK_THEME_DIR "@GTK_THEME_DIR@/gtk-2.0"
#ifndef OXYGEN_DEBUG
#define OXYGEN_DEBUG @OXYGEN_DEBUG@
#endif

#ifndef OXYGEN_DEBUG_INNERSHADOWS
#define OXYGEN_DEBUG_INNERSHADOWS @OXYGEN_DEBUG_INNERSHADOWS@
#endif

#define OXYGEN_ICON_HACK @OXYGEN_ICON_HACK@
#define OXYGEN_FORCE_KDE_ICONS_AND_FONTS @OXYGEN_FORCE_KDE_ICONS_AND_FONTS@
#define HAVE_DBUS @HAVE_DBUS@
#define HAVE_DBUS_GLIB @HAVE_DBUS_GLIB@
#define ENABLE_COMBOBOX_LIST_RESIZE @ENABLE_COMBOBOX_LIST_RESIZE@

#define ENABLE_INNER_SHADOWS_HACK @ENABLE_INNER_SHADOWS_HACK@
#define ENABLE_GROUPBOX_HACK @ENABLE_GROUPBOX_HACK@
#define DISABLE_SIGNAL_HOOKS @DISABLE_SIGNAL_HOOKS@

#define OXYGEN_VERSION "@OXYGEN_VERSION@"

#endif
