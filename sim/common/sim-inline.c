/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation, Inc.

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


#ifndef _SIM_INLINE_C_
#define _SIM_INLINE_C_

#include "sim-inline.h"


#if defined(SIM_BITS_INLINE)
#if ((SIM_BITS_INLINE & INCLUDE_MODULE) \
     && !(SIM_BITS_INLINE & INCLUDED_BY_MODULE))
#include "sim-bits.c"
#endif
#endif


#if defined(SIM_CORE_INLINE)
#if ((SIM_CORE_INLINE & INCLUDE_MODULE) \
     && !(SIM_CORE_INLINE & INCLUDED_BY_MODULE))
#include "sim-core.c"
#endif
#endif


#if defined(SIM_ENDIAN_INLINE)
#if ((SIM_ENDIAN_INLINE & INCLUDE_MODULE) \
     && !(SIM_ENDIAN_INLINE & INCLUDED_BY_MODULE))
#include "sim-endian.c"
#endif
#endif


#if defined(SIM_EVENTS_INLINE)
#if ((SIM_EVENTS_INLINE & INCLUDE_MODULE) \
     && !(SIM_EVENTS_INLINE & INCLUDED_BY_MODULE))
#include "sim-events.c"
#endif
#endif


#if defined(SIM_FPU_INLINE)
#if ((SIM_FPU_INLINE & INCLUDE_MODULE) \
     && !(SIM_FPU_INLINE & INCLUDED_BY_MODULE))
#include "sim-fpu.c"
#endif
#endif


#if defined(SIM_IO_INLINE)
#if ((SIM_IO_INLINE & INCLUDE_MODULE) \
     && !(SIM_IO_INLINE & INCLUDED_BY_MODULE))
#include "sim-io.c"
#endif
#endif



#endif
