/* Definitions for General Magic target.
   Copyright (C) 1995 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "magic.h"

#define TARGET_SYMFILE_POSTREAD(OBJFILE) init_magic ()

#define TARGET_CREATE_INFERIOR_HOOK(PID) magic_create_inferior_hook ()

/* Magic Cap method stepping support.
   We use the shared library trampoline support to implement stepping over
   the method dispatcher.  We consider the entire dispatcher to be one big
   trampoline, and use SKIP_TRAMPOLINE_CODE() to skip from a dispatcher
   entry point to the dispatcher exit site, or from the exit site to the
   first instruction of the dispatched-to method.  */

#define DYNAMIC_TRAMPOLINE_NEXTPC(pc) \
	magic_skip_dispatcher(pc)

#if 0
#define IN_SOLIB_CALL_TRAMPOLINE(pc, name) \
	magic_in_dispatcher(pc, name)

#define IN_SOLIB_RETURN_TRAMPOLINE(pc, name) \
	magic_in_dispatcher(pc, name)
#endif
