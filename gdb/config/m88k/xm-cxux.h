/* OBSOLETE /* Host-machine dependent parameters for Motorola 88000, for GDB. */
/* OBSOLETE    Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1993, 1994, 1996, 1998 */
/* OBSOLETE    Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #include "m88k/xm-m88k.h" */
/* OBSOLETE  */
/* OBSOLETE #define HOST_BYTE_ORDER BIG_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE #if !defined (USG) */
/* OBSOLETE #define USG 1 */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #include <sys/param.h> */
/* OBSOLETE  */
/* OBSOLETE #define x_foff _x_x._x_offset */
/* OBSOLETE #define x_fname _x_name */
/* OBSOLETE #define USER ptrace_user */
/* OBSOLETE /* */
/* OBSOLETE    #define _BSD_WAIT_FLAVOR */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE #define HAVE_TERMIO */
/* OBSOLETE  */
/* OBSOLETE #ifndef USIZE */
/* OBSOLETE #define USIZE 2048 */
/* OBSOLETE #ifndef UPAGES */
/* OBSOLETE #define UPAGES USIZE */
/* OBSOLETE #endif */
/* OBSOLETE #endif */
/* OBSOLETE #define NBPG NBPC */
/* OBSOLETE  */
/* OBSOLETE /* Get rid of any system-imposed stack limit if possible.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SET_STACK_LIMIT_HUGE */
/* OBSOLETE  */
/* OBSOLETE /* This is the amount to subtract from u.u_ar0 */
/* OBSOLETE    to get the offset in the core file of the register values.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Since registers r0 through r31 are stored directly in the struct ptrace_user, */
/* OBSOLETE    (for m88k BCS) */
/* OBSOLETE    the ptrace_user offsets are sufficient and KERNEL_U_ADDRESS can be 0 */ */
/* OBSOLETE  */
/* OBSOLETE #define KERNEL_U_ADDR 0 */
/* OBSOLETE  */
/* OBSOLETE /* The CX/UX C compiler doesn't permit complex expressions as array bounds. */ */
/* OBSOLETE #define STRICT_ANSI_BOUNDS */
/* OBSOLETE  */
/* OBSOLETE #define CORE_REGISTER_ADDR(regno, reg_ptr) \ */
/* OBSOLETE    m88k_harris_core_register_addr(regno, reg_ptr) */
