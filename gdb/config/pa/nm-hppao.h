// OBSOLETE /* HPPA PA-RISC machine native support for Lites, for GDB.
// OBSOLETE    Copyright 1995, 2002 Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #include "config/nm-m3.h"
// OBSOLETE #define U_REGS_OFFSET 0
// OBSOLETE 
// OBSOLETE #define KERNEL_U_ADDR 0
// OBSOLETE 
// OBSOLETE /* What a coincidence! */
// OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno)				\
// OBSOLETE { addr = (int)(blockend) + REGISTER_BYTE (regno);}
// OBSOLETE 
// OBSOLETE /* fetch_inferior_registers is in hppab-nat.c.  */
// OBSOLETE #define FETCH_INFERIOR_REGISTERS
// OBSOLETE 
// OBSOLETE /* attach/detach works to some extent under BSD and HPUX.  So long
// OBSOLETE    as the process you're attaching to isn't blocked waiting on io,
// OBSOLETE    blocked waiting on a signal, or in a system call things work 
// OBSOLETE    fine.  (The problems in those cases are related to the fact that
// OBSOLETE    the kernel can't provide complete register information for the
// OBSOLETE    target process...  Which really pisses off GDB.)  */
// OBSOLETE 
// OBSOLETE #define ATTACH_DETACH
// OBSOLETE 
// OBSOLETE #define EMULATOR_BASE 0x90100000
// OBSOLETE #define EMULATOR_END  0x90200000
