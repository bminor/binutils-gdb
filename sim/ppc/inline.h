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


#ifndef _INLINE_H_
#define _INLINE_H_

#if SIM_ENDIAN_INLINE
#if SIM_ENDIAN_INLINE == 2
#define INLINE_SIM_ENDIAN static INLINE
#else
#define INLINE_SIM_ENDIAN static
#endif
#endif

#if ICACHE_INLINE
#if ICACHE_INLINE == 2
#define INLINE_ICACHE static INLINE
#else
#define INLINE_ICACHE static
#endif
#endif

#if CORE_INLINE
#if CORE_INLINE == 2
#define INLINE_CORE static INLINE
#else
#define INLINE_CORE static
#endif
#endif

#if VM_INLINE
#if VM_INLINE == 2
#define INLINE_VM static INLINE
#else
#define INLINE_VM static
#endif
#endif

#if CPU_INLINE
#if CPU_INLINE == 2
#define INLINE_CPU static INLINE
#else
#define INLINE_CPU static
#endif
#endif

#if MODEL_INLINE
#if MODEL_INLINE == 2
#define INLINE_MODEL static INLINE
#else
#define INLINE_MODEL static
#endif
#define STATIC_MODEL static
#endif

#if BITS_INLINE
#if BITS_INLINE == 2
#define INLINE_BITS static INLINE
#else
#define INLINE_BITS static
#endif
#endif

#if EVENTS_INLINE
#if EVENTS_INLINE == 2
#define INLINE_EVENTS static INLINE
#else
#define INLINE_EVENTS static
#endif
#endif

#if MON_INLINE
#if MON_INLINE == 2
#define INLINE_MON static INLINE
#else
#define INLINE_MON static
#endif
#endif

#if REGISTERS_INLINE
#if REGISTERS_INLINE == 2
#define INLINE_REGISTERS static INLINE
#else
#define INLINE_REGISTERS static
#endif
#endif

#if INTERRUPTS_INLINE
#if INTERRUPTS_INLINE == 2
#define INLINE_INTERRUPTS static INLINE
#else
#define INLINE_INTERRUPTS static
#endif
#endif

#if DEVICE_TREE_INLINE
#if DEVICE_TREE_INLINE == 2
#define INLINE_DEVICE_TREE static INLINE
#else
#define INLINE_DEVICE_TREE static
#endif
#endif

#if DEVICES_INLINE
#if DEVICES_INLINE == 2
#define INLINE_DEVICES static INLINE
#else
#define INLINE_DEVICES static
#endif
#define STATIC_DEVICES static
#endif

#if SPREG_INLINE
#if SPREG_INLINE == 2
#define INLINE_SPREG static INLINE
#else
#define INLINE_SPREG static
#endif
#endif

#if SEMANTICS_INLINE && !defined(_SEMANTICS_C_)
#if SEMANTICS_INLINE == 2
#define INLINE_SEMANTICS static INLINE
#else
#define INLINE_SEMANTICS static
#endif
#define STATIC_SEMANTICS static
#endif

#if IDECODE_INLINE
#if IDECODE_INLINE == 2
#define INLINE_IDECODE static INLINE
#else
#define INLINE_IDECODE static
#endif
#define STATIC_IDECODE static
#endif

#if FUNCTION_UNIT_INLINE
#if FUNCTION_UNIT_INLINE == 2
#define INLINE_FUNCTION_UNIT static INLINE
#else
#define INLINE_FUNCTION_UNIT static
#endif
#endif


#endif
