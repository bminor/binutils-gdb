/* OBSOLETE /* Target-machine dependent code for WDC-65816, for GDB. */
/* OBSOLETE    Copyright 1995, 1996, 2000, 2001 Free Software Foundation, Inc. */
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
/* OBSOLETE /* */
/* OBSOLETE    Contributed by Steve Chamberlain */
/* OBSOLETE    sac@cygnus.com */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "obstack.h" */
/* OBSOLETE #include "symtab.h" */
/* OBSOLETE #include "gdbcmd.h" */
/* OBSOLETE #include "gdbtypes.h" */
/* OBSOLETE #include "dis-asm.h" */
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* Return the saved PC from this frame. */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE w65_frame_saved_pc (struct frame_info *frame) */
/* OBSOLETE { */
/* OBSOLETE   return (read_memory_integer (frame->frame + 2, 4) & 0xffffff); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE w65_addr_bits_remove (CORE_ADDR addr) */
/* OBSOLETE { */
/* OBSOLETE   return ((addr) & 0xffffff); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE read_memory_pointer (CORE_ADDR x) */
/* OBSOLETE { */
/* OBSOLETE   return read_memory_integer (ADDR_BITS_REMOVE (x), 4); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE init_frame_pc (void) */
/* OBSOLETE { */
/* OBSOLETE   internal_error (__FILE__, __LINE__, "failed internal consistency check"); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE w65_push_dummy_frame (void) */
/* OBSOLETE { */
/* OBSOLETE   internal_error (__FILE__, __LINE__, "failed internal consistency check"); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame. */
/* OBSOLETE  */
/* OBSOLETE    We cache the result of doing this in the frame_cache_obstack, since */
/* OBSOLETE    it is fairly expensive.  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE frame_find_saved_regs (struct frame_info *fip, struct frame_saved_regs *fsrp) */
/* OBSOLETE { */
/* OBSOLETE   int locals; */
/* OBSOLETE   CORE_ADDR pc; */
/* OBSOLETE   CORE_ADDR adr; */
/* OBSOLETE   int i; */
/* OBSOLETE  */
/* OBSOLETE   memset (fsrp, 0, sizeof *fsrp); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE saved_pc_after_call (void) */
/* OBSOLETE { */
/* OBSOLETE   int sp = read_register (SP_REGNUM); */
/* OBSOLETE   int val = read_memory_integer (sp + 1, 4); */
/* OBSOLETE   return ADDR_BITS_REMOVE (val); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE extract_return_value (struct type *type, char *regbuf, char *valbuf) */
/* OBSOLETE { */
/* OBSOLETE   int b; */
/* OBSOLETE   int len = TYPE_LENGTH (type); */
/* OBSOLETE  */
/* OBSOLETE   for (b = 0; b < len; b += 2) */
/* OBSOLETE     { */
/* OBSOLETE       int todo = len - b; */
/* OBSOLETE       if (todo > 2) */
/* OBSOLETE 	todo = 2; */
/* OBSOLETE       memcpy (valbuf + b, regbuf + b, todo); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE write_return_value (struct type *type, char *valbuf) */
/* OBSOLETE { */
/* OBSOLETE   int reg; */
/* OBSOLETE   int len; */
/* OBSOLETE   for (len = 0; len < TYPE_LENGTH (type); len += 2) */
/* OBSOLETE     { */
/* OBSOLETE       write_register_bytes (REGISTER_BYTE (len / 2 + 2), valbuf + len, 2); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE store_struct_return (CORE_ADDR addr, CORE_ADDR sp) */
/* OBSOLETE { */
/* OBSOLETE   write_register (2, addr); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE w65_pop_frame (void) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE init_extra_frame_info (void) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE pop_frame (void) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE w65_frame_chain (struct frame_info *thisframe) */
/* OBSOLETE { */
/* OBSOLETE   return 0xffff & read_memory_integer ((thisframe)->frame, 2); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE gb (int x) */
/* OBSOLETE { */
/* OBSOLETE   return read_memory_integer (x, 1) & 0xff; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR */
/* OBSOLETE w65_skip_prologue (CORE_ADDR pc) */
/* OBSOLETE { */
/* OBSOLETE   CORE_ADDR too_far = pc + 20; */
/* OBSOLETE  */
/* OBSOLETE   /* looking for bits of the prologue, we can expect to */
/* OBSOLETE      see this in a frameful function: */
/* OBSOLETE  */
/* OBSOLETE      stack adjust: */
/* OBSOLETE  */
/* OBSOLETE      3B                 tsc */
/* OBSOLETE      1A                 inc a */
/* OBSOLETE      18                 clc */
/* OBSOLETE      69E2FF             adc     #0xffe2 */
/* OBSOLETE      3A                 dec a */
/* OBSOLETE      1B                 tcs */
/* OBSOLETE      1A                 inc a */
/* OBSOLETE  */
/* OBSOLETE      link: */
/* OBSOLETE  */
/* OBSOLETE      A500               lda     <r15 */
/* OBSOLETE      48                 pha */
/* OBSOLETE      3B                 tsc */
/* OBSOLETE      1a                 inc     a */
/* OBSOLETE      8500               sta     <r15 */
/* OBSOLETE  */
/* OBSOLETE    */ */
/* OBSOLETE  */
/* OBSOLETE #define TSC  0x3b */
/* OBSOLETE #define TCS  0x1b */
/* OBSOLETE #define INCA 0x1a */
/* OBSOLETE #define PHA  0x48 */
/* OBSOLETE #define LDADIR 0xa5 */
/* OBSOLETE #define STADIR 0x85 */
/* OBSOLETE  */
/* OBSOLETE   /* Skip a stack adjust - any area between a tsc and tcs */ */
/* OBSOLETE   if (gb (pc) == TSC) */
/* OBSOLETE     { */
/* OBSOLETE       while (pc < too_far && gb (pc) != TCS) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  pc++; */
/* OBSOLETE 	} */
/* OBSOLETE       pc++; */
/* OBSOLETE       /* Skip a stupid inc a */ */
/* OBSOLETE       if (gb (pc) == INCA) */
/* OBSOLETE 	pc++; */
/* OBSOLETE  */
/* OBSOLETE     } */
/* OBSOLETE   /* Stack adjust can also be done with n pha's */ */
/* OBSOLETE   while (gb (pc) == PHA) */
/* OBSOLETE     pc++; */
/* OBSOLETE  */
/* OBSOLETE   /* Skip a link - that's a ld/ph/tsc/inc/sta */ */
/* OBSOLETE  */
/* OBSOLETE   if (gb (pc) == LDADIR */
/* OBSOLETE       && gb (pc + 5) == STADIR */
/* OBSOLETE       && gb (pc + 1) == gb (pc + 6) */
/* OBSOLETE       && gb (pc + 2) == PHA */
/* OBSOLETE       && gb (pc + 3) == TSC */
/* OBSOLETE       && gb (pc + 4) == INCA) */
/* OBSOLETE     { */
/* OBSOLETE       pc += 7; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   return pc; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE register_raw_size (int n) */
/* OBSOLETE { */
/* OBSOLETE   return sim_reg_size (n); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE print_register_hook (int regno) */
/* OBSOLETE { */
/* OBSOLETE   if (regno == P_REGNUM) */
/* OBSOLETE     { */
/* OBSOLETE       /* CCR register */ */
/* OBSOLETE  */
/* OBSOLETE       int C, Z, N, V, I, D, X, M; */
/* OBSOLETE       unsigned char b[1]; */
/* OBSOLETE       unsigned char l; */
/* OBSOLETE  */
/* OBSOLETE       read_relative_register_raw_bytes (regno, b); */
/* OBSOLETE       l = b[0]; */
/* OBSOLETE       printf_unfiltered ("\t"); */
/* OBSOLETE       C = (l & 0x1) != 0; */
/* OBSOLETE       Z = (l & 0x2) != 0; */
/* OBSOLETE       I = (l & 0x4) != 0; */
/* OBSOLETE       D = (l & 0x8) != 0; */
/* OBSOLETE       X = (l & 0x10) != 0; */
/* OBSOLETE       M = (l & 0x20) != 0; */
/* OBSOLETE       V = (l & 0x40) != 0; */
/* OBSOLETE       N = (l & 0x80) != 0; */
/* OBSOLETE  */
/* OBSOLETE       printf_unfiltered ("N-%d ", N); */
/* OBSOLETE       printf_unfiltered ("V-%d ", V); */
/* OBSOLETE       printf_unfiltered ("M-%d ", M); */
/* OBSOLETE       printf_unfiltered ("X-%d ", X); */
/* OBSOLETE       printf_unfiltered ("D-%d ", D); */
/* OBSOLETE       printf_unfiltered ("I-%d ", I); */
/* OBSOLETE       printf_unfiltered ("Z-%d ", Z); */
/* OBSOLETE       printf_unfiltered ("C-%d ", C); */
/* OBSOLETE       if ((C | Z) == 0) */
/* OBSOLETE 	printf_unfiltered ("u> "); */
/* OBSOLETE       if ((C | Z) == 1) */
/* OBSOLETE 	printf_unfiltered ("u<= "); */
/* OBSOLETE       if ((C == 0)) */
/* OBSOLETE 	printf_unfiltered ("u>= "); */
/* OBSOLETE       if (C == 1) */
/* OBSOLETE 	printf_unfiltered ("u< "); */
/* OBSOLETE       if (Z == 0) */
/* OBSOLETE 	printf_unfiltered ("!= "); */
/* OBSOLETE       if (Z == 1) */
/* OBSOLETE 	printf_unfiltered ("== "); */
/* OBSOLETE       if ((N ^ V) == 0) */
/* OBSOLETE 	printf_unfiltered (">= "); */
/* OBSOLETE       if ((N ^ V) == 1) */
/* OBSOLETE 	printf_unfiltered ("< "); */
/* OBSOLETE       if ((Z | (N ^ V)) == 0) */
/* OBSOLETE 	printf_unfiltered ("> "); */
/* OBSOLETE       if ((Z | (N ^ V)) == 1) */
/* OBSOLETE 	printf_unfiltered ("<= "); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE _initialize_w65_tdep (void) */
/* OBSOLETE { */
/* OBSOLETE   tm_print_insn = print_insn_w65; */
/* OBSOLETE } */
