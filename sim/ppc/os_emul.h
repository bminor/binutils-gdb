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


#ifndef _OS_EMUL_H_
#define _OS_EMUL_H_

#ifndef INLINE_OS_EMUL
#define INLINE_OS_EMUL
#endif

typedef struct _os_emul *os_emul;

INLINE_OS_EMUL os_emul *os_emul_create
(char *emulation_name);

INLINE_OS_EMUL void os_emul_init
(os_emul *emulation);

INLINE_OS_EMUL void os_emul_call
(cpu *processor,
 unsigned_word cia);

#endif
