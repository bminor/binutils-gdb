/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _PPC_INLINE_C_
#define _PPC_INLINE_C_

#if ENDIAN_INLINE
#include "ppc-endian.c"
#endif

#if ICACHE_INLINE
#include "icache.c"
#endif

#if CORE_INLINE
#include "corefile.c"
#endif

#if VM_INLINE
#include "vm.c"
#endif

#if CPU_INLINE
#include "cpu.c"
#endif

#if EVENTS_INLINE
#include "events.c"
#endif

#if MON_INLINE
#include "mon.c"
#endif

#if REGISTERS_INLINE
#include "registers.c"
#endif

#if INTERRUPTS_INLINE
#include "interrupts.c"
#endif

#if DEVICE_TREE_INLINE
#include "device_tree.c"
#endif

#if DEVICES_INLINE
#include "devices.c"
#endif

#if SPREG_INLINE
#include "spreg.c"
#endif

#if SEMANTICS_INLINE
#include "semantics.c"
#endif

#if IDECODE_INLINE
#include "idecode.c"
#endif

#endif
