/* Copyright (C) 2017-2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef ARCH_ARC_H
#define ARCH_ARC_H

#include "gdbsupport/tdesc.h"

/* Supported ARC system hardware types.  */
enum arc_sys_type
{
  ARC_SYS_TYPE_ARCOMPACT = 0,	  /* ARC600 or ARC700 */
  ARC_SYS_TYPE_ARCV2,		  /* ARC EM or ARC HS */
  ARC_SYS_TYPE_NUM
};

static inline const char *
arc_sys_type_to_str (const arc_sys_type type)
{
  switch (type)
    {
    case ARC_SYS_TYPE_ARCOMPACT:
      return "ARC_SYS_TYPE_ARCOMPACT";
    case ARC_SYS_TYPE_ARCV2:
      return "ARC_SYS_TYPE_ARCV2";
    default:
      return "Invalid";
    }
}

/* Create target description for the specified system type.  */
target_desc *arc_create_target_description (arc_sys_type sys_type);

#endif /* ARCH_ARC_H */
