/* OBSOLETE /* Parameters for execution on a Sony/NEWS, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1987, 1989, 1991, 1993, 1999, 2000 */
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
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* See following cpu type determination macro to get the machine type. */
/* OBSOLETE  */
/* OBSOLETE    Here is an m-news.h file for gdb.  It supports the 68881 registers. */
/* OBSOLETE    by hikichi@srava.sra.junet */
/* OBSOLETE  */
/* OBSOLETE    * Ptrace for handling floating register has a bug(before NEWS OS version 2.2), */
/* OBSOLETE    * After NEWS OS version 3.2, some of ptrace's bug is fixed. */
/* OBSOLETE    But we cannot change the floating register(see adb(1) in OS 3.2) yet.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF.  */ */
/* OBSOLETE  */
/* OBSOLETE /* when it return the floating value, use the FP0 in NEWS.  */ */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE   { if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) \ */
/* OBSOLETE       { \ */
/* OBSOLETE 	REGISTER_CONVERT_TO_VIRTUAL (FP0_REGNUM, TYPE, \ */
/* OBSOLETE 			             &REGBUF[REGISTER_BYTE (FP0_REGNUM)], \ */
/* OBSOLETE 				     VALBUF); \ */
/* OBSOLETE       } \ */
/* OBSOLETE     else \ */
/* OBSOLETE       memcpy (VALBUF, REGBUF, TYPE_LENGTH (TYPE)); } */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  */ */
/* OBSOLETE  */
/* OBSOLETE /* when it return the floating value, use the FP0 in NEWS.  */ */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE   { if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) \ */
/* OBSOLETE       { \ */
/* OBSOLETE 	char raw_buf[REGISTER_RAW_SIZE (FP0_REGNUM)]; \ */
/* OBSOLETE 	REGISTER_CONVERT_TO_RAW (TYPE, FP0_REGNUM, VALBUF, raw_buf); \ */
/* OBSOLETE 	write_register_bytes (REGISTER_BYTE (FP0_REGNUM), \ */
/* OBSOLETE 			      raw_buf, REGISTER_RAW_SIZE (FP0_REGNUM)); \ */
/* OBSOLETE       } \ */
/* OBSOLETE     else \ */
/* OBSOLETE       write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE)); } */
/* OBSOLETE  */
/* OBSOLETE /* Return number of args passed to a frame. */
/* OBSOLETE    Can return -1, meaning no way to tell.  */ */
/* OBSOLETE  */
/* OBSOLETE extern int news_frame_num_args (struct frame_info *fi); */
/* OBSOLETE #define FRAME_NUM_ARGS(fi) (news_frame_num_args ((fi))) */
/* OBSOLETE  */
/* OBSOLETE #include "m68k/tm-m68k.h" */
