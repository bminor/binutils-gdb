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


#ifndef _OS_EMUL_C_
#define _OS_EMUL_C_

#include "cpu.h"
#include "idecode.h"
#include "os_emul.h"

#include "emul_generic.h"
#include "emul_netbsd.h"

#ifndef STATIC_INLINE_OS_EMUL
#define STATIC_INLINE_OS_EMUL STATIC_INLINE
#endif


INLINE_OS_EMUL void
os_emul_call(cpu *processor,
	     unsigned_word cia)
{
  emulation *emul = &emul_netbsd;
  emul_do_call(emul,
	       cpu_registers(processor)->gpr[0],
	       3, /*r3 contains arg0*/
	       processor,
	       cia);
}

#endif /* _OS_EMUL_C_ */
