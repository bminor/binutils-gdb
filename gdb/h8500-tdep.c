// OBSOLETE /* Target-dependent code for Hitachi H8/500, for GDB.
// OBSOLETE 
// OBSOLETE    Copyright 1993, 1994, 1995, 1998, 2000, 2001, 2002 Free Software
// OBSOLETE    Foundation, Inc.
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
// OBSOLETE /*
// OBSOLETE    Contributed by Steve Chamberlain
// OBSOLETE    sac@cygnus.com
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "frame.h"
// OBSOLETE #include "symtab.h"
// OBSOLETE #include "gdbtypes.h"
// OBSOLETE #include "gdbcmd.h"
// OBSOLETE #include "value.h"
// OBSOLETE #include "dis-asm.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #define UNSIGNED_SHORT(X) ((X) & 0xffff)
// OBSOLETE 
// OBSOLETE static int code_size = 2;
// OBSOLETE 
// OBSOLETE static int data_size = 2;
// OBSOLETE 
// OBSOLETE /* Shape of an H8/500 frame :
// OBSOLETE 
// OBSOLETE    arg-n
// OBSOLETE    ..
// OBSOLETE    arg-2
// OBSOLETE    arg-1
// OBSOLETE    return address <2 or 4 bytes>
// OBSOLETE    old fp         <2 bytes>
// OBSOLETE    auto-n
// OBSOLETE    ..
// OBSOLETE    auto-1
// OBSOLETE    saved registers
// OBSOLETE 
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE /* an easy to debug H8 stack frame looks like:
// OBSOLETE    0x6df6               push    r6
// OBSOLETE    0x0d76       mov.w   r7,r6
// OBSOLETE    0x6dfn          push    reg
// OBSOLETE    0x7905 nnnn          mov.w  #n,r5    or   0x1b87  subs #2,sp
// OBSOLETE    0x1957               sub.w  r5,sp
// OBSOLETE 
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #define IS_PUSH(x) (((x) & 0xff00)==0x6d00)
// OBSOLETE #define IS_LINK_8(x) ((x) == 0x17)
// OBSOLETE #define IS_LINK_16(x) ((x) == 0x1f)
// OBSOLETE #define IS_MOVE_FP(x) ((x) == 0x0d76)
// OBSOLETE #define IS_MOV_SP_FP(x) ((x) == 0x0d76)
// OBSOLETE #define IS_SUB2_SP(x) ((x) == 0x1b87)
// OBSOLETE #define IS_MOVK_R5(x) ((x) == 0x7905)
// OBSOLETE #define IS_SUB_R5SP(x) ((x) == 0x1957)
// OBSOLETE 
// OBSOLETE #define LINK_8 0x17
// OBSOLETE #define LINK_16 0x1f
// OBSOLETE 
// OBSOLETE int minimum_mode = 1;
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE h8500_skip_prologue (CORE_ADDR start_pc)
// OBSOLETE {
// OBSOLETE   short int w;
// OBSOLETE 
// OBSOLETE   w = read_memory_integer (start_pc, 1);
// OBSOLETE   if (w == LINK_8)
// OBSOLETE     {
// OBSOLETE       start_pc += 2;
// OBSOLETE       w = read_memory_integer (start_pc, 1);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (w == LINK_16)
// OBSOLETE     {
// OBSOLETE       start_pc += 3;
// OBSOLETE       w = read_memory_integer (start_pc, 2);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return start_pc;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE h8500_addr_bits_remove (CORE_ADDR addr)
// OBSOLETE {
// OBSOLETE   return ((addr) & 0xffffff);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Given a GDB frame, determine the address of the calling function's
// OBSOLETE    frame.  This will be used to create a new GDB frame struct, and
// OBSOLETE    then INIT_EXTRA_FRAME_INFO and DEPRECATED_INIT_FRAME_PC will be
// OBSOLETE    called for the new frame.
// OBSOLETE 
// OBSOLETE    For us, the frame address is its stack pointer value, so we look up
// OBSOLETE    the function prologue to determine the caller's sp value, and return it.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE h8500_frame_chain (struct frame_info *thisframe)
// OBSOLETE {
// OBSOLETE   if (!inside_entry_file (thisframe->pc))
// OBSOLETE     return (read_memory_integer (get_frame_base (thisframe), PTR_SIZE));
// OBSOLETE   else
// OBSOLETE     return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Fetch the instruction at ADDR, returning 0 if ADDR is beyond LIM or
// OBSOLETE    is not the address of a valid instruction, the address of the next
// OBSOLETE    instruction beyond ADDR otherwise.  *PWORD1 receives the first word
// OBSOLETE    of the instruction. */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE NEXT_PROLOGUE_INSN (CORE_ADDR addr, CORE_ADDR lim, char *pword1)
// OBSOLETE {
// OBSOLETE   if (addr < lim + 8)
// OBSOLETE     {
// OBSOLETE       read_memory (addr, pword1, 1);
// OBSOLETE       read_memory (addr, pword1 + 1, 1);
// OBSOLETE       return 1;
// OBSOLETE     }
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Examine the prologue of a function.  `ip' points to the first
// OBSOLETE    instruction.  `limit' is the limit of the prologue (e.g. the addr
// OBSOLETE    of the first linenumber, or perhaps the program counter if we're
// OBSOLETE    stepping through).  `frame_sp' is the stack pointer value in use in
// OBSOLETE    this frame.  `fsr' is a pointer to a frame_saved_regs structure
// OBSOLETE    into which we put info about the registers saved by this frame.
// OBSOLETE    `fi' is a struct frame_info pointer; we fill in various fields in
// OBSOLETE    it to reflect the offsets of the arg pointer and the locals
// OBSOLETE    pointer.  */
// OBSOLETE 
// OBSOLETE /* Return the saved PC from this frame. */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE frame_saved_pc (struct frame_info *frame)
// OBSOLETE {
// OBSOLETE   return read_memory_integer (get_frame_base (frame) + 2, PTR_SIZE);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE h8500_pop_frame (void)
// OBSOLETE {
// OBSOLETE   unsigned regnum;
// OBSOLETE   struct frame_saved_regs fsr;
// OBSOLETE   struct frame_info *frame = get_current_frame ();
// OBSOLETE 
// OBSOLETE   deprecated_get_frame_saved_regs (frame, &fsr);
// OBSOLETE 
// OBSOLETE   for (regnum = 0; regnum < 8; regnum++)
// OBSOLETE     {
// OBSOLETE       if (fsr.regs[regnum])
// OBSOLETE 	write_register (regnum, read_memory_short (fsr.regs[regnum]));
// OBSOLETE 
// OBSOLETE       flush_cached_frames ();
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE h8500_print_register_hook (int regno)
// OBSOLETE {
// OBSOLETE   if (regno == CCR_REGNUM)
// OBSOLETE     {
// OBSOLETE       /* CCR register */
// OBSOLETE 
// OBSOLETE       int C, Z, N, V;
// OBSOLETE       unsigned char b[2];
// OBSOLETE       unsigned char l;
// OBSOLETE 
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno, b);
// OBSOLETE       l = b[1];
// OBSOLETE       printf_unfiltered ("\t");
// OBSOLETE       printf_unfiltered ("I-%d - ", (l & 0x80) != 0);
// OBSOLETE       N = (l & 0x8) != 0;
// OBSOLETE       Z = (l & 0x4) != 0;
// OBSOLETE       V = (l & 0x2) != 0;
// OBSOLETE       C = (l & 0x1) != 0;
// OBSOLETE       printf_unfiltered ("N-%d ", N);
// OBSOLETE       printf_unfiltered ("Z-%d ", Z);
// OBSOLETE       printf_unfiltered ("V-%d ", V);
// OBSOLETE       printf_unfiltered ("C-%d ", C);
// OBSOLETE       if ((C | Z) == 0)
// OBSOLETE 	printf_unfiltered ("u> ");
// OBSOLETE       if ((C | Z) == 1)
// OBSOLETE 	printf_unfiltered ("u<= ");
// OBSOLETE       if ((C == 0))
// OBSOLETE 	printf_unfiltered ("u>= ");
// OBSOLETE       if (C == 1)
// OBSOLETE 	printf_unfiltered ("u< ");
// OBSOLETE       if (Z == 0)
// OBSOLETE 	printf_unfiltered ("!= ");
// OBSOLETE       if (Z == 1)
// OBSOLETE 	printf_unfiltered ("== ");
// OBSOLETE       if ((N ^ V) == 0)
// OBSOLETE 	printf_unfiltered (">= ");
// OBSOLETE       if ((N ^ V) == 1)
// OBSOLETE 	printf_unfiltered ("< ");
// OBSOLETE       if ((Z | (N ^ V)) == 0)
// OBSOLETE 	printf_unfiltered ("> ");
// OBSOLETE       if ((Z | (N ^ V)) == 1)
// OBSOLETE 	printf_unfiltered ("<= ");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE h8500_print_registers_info (struct gdbarch *gdbarch,
// OBSOLETE 			    struct ui_file *file,
// OBSOLETE 			    struct frame_info *frame,
// OBSOLETE 			    int regnum, int print_all)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   const int numregs = NUM_REGS + NUM_PSEUDO_REGS;
// OBSOLETE   char *raw_buffer = alloca (MAX_REGISTER_RAW_SIZE);
// OBSOLETE   char *virtual_buffer = alloca (MAX_REGISTER_VIRTUAL_SIZE);
// OBSOLETE 
// OBSOLETE   for (i = 0; i < numregs; i++)
// OBSOLETE     {
// OBSOLETE       /* Decide between printing all regs, non-float / vector regs, or
// OBSOLETE          specific reg.  */
// OBSOLETE       if (regnum == -1)
// OBSOLETE 	{
// OBSOLETE 	  if (!print_all)
// OBSOLETE 	    {
// OBSOLETE 	      if (TYPE_CODE (REGISTER_VIRTUAL_TYPE (i)) == TYPE_CODE_FLT)
// OBSOLETE 		continue;
// OBSOLETE 	      if (TYPE_VECTOR (REGISTER_VIRTUAL_TYPE (i)))
// OBSOLETE 		continue;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  if (i != regnum)
// OBSOLETE 	    continue;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* If the register name is empty, it is undefined for this
// OBSOLETE          processor, so don't display anything.  */
// OBSOLETE       if (REGISTER_NAME (i) == NULL || *(REGISTER_NAME (i)) == '\0')
// OBSOLETE 	continue;
// OBSOLETE 
// OBSOLETE       fputs_filtered (REGISTER_NAME (i), file);
// OBSOLETE       print_spaces_filtered (15 - strlen (REGISTER_NAME (i)), file);
// OBSOLETE 
// OBSOLETE       /* Get the data in raw format.  */
// OBSOLETE       if (! frame_register_read (frame, i, raw_buffer))
// OBSOLETE 	{
// OBSOLETE 	  fprintf_filtered (file, "*value not available*\n");
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* FIXME: cagney/2002-08-03: This code shouldn't be necessary.
// OBSOLETE          The function frame_register_read() should have returned the
// OBSOLETE          pre-cooked register so no conversion is necessary.  */
// OBSOLETE       /* Convert raw data to virtual format if necessary.  */
// OBSOLETE       if (REGISTER_CONVERTIBLE (i))
// OBSOLETE 	{
// OBSOLETE 	  REGISTER_CONVERT_TO_VIRTUAL (i, REGISTER_VIRTUAL_TYPE (i),
// OBSOLETE 				       raw_buffer, virtual_buffer);
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  memcpy (virtual_buffer, raw_buffer,
// OBSOLETE 		  REGISTER_VIRTUAL_SIZE (i));
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* If virtual format is floating, print it that way, and in raw
// OBSOLETE          hex.  */
// OBSOLETE       if (TYPE_CODE (REGISTER_VIRTUAL_TYPE (i)) == TYPE_CODE_FLT)
// OBSOLETE 	{
// OBSOLETE 	  int j;
// OBSOLETE 
// OBSOLETE 	  val_print (REGISTER_VIRTUAL_TYPE (i), virtual_buffer, 0, 0,
// OBSOLETE 		     file, 0, 1, 0, Val_pretty_default);
// OBSOLETE 
// OBSOLETE 	  fprintf_filtered (file, "\t(raw 0x");
// OBSOLETE 	  for (j = 0; j < REGISTER_RAW_SIZE (i); j++)
// OBSOLETE 	    {
// OBSOLETE 	      int idx;
// OBSOLETE 	      if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
// OBSOLETE 		idx = j;
// OBSOLETE 	      else
// OBSOLETE 		idx = REGISTER_RAW_SIZE (i) - 1 - j;
// OBSOLETE 	      fprintf_filtered (file, "%02x", (unsigned char) raw_buffer[idx]);
// OBSOLETE 	    }
// OBSOLETE 	  fprintf_filtered (file, ")");
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  /* Print the register in hex.  */
// OBSOLETE 	  val_print (REGISTER_VIRTUAL_TYPE (i), virtual_buffer, 0, 0,
// OBSOLETE 		     file, 'x', 1, 0, Val_pretty_default);
// OBSOLETE           /* If not a vector register, print it also according to its
// OBSOLETE              natural format.  */
// OBSOLETE 	  if (TYPE_VECTOR (REGISTER_VIRTUAL_TYPE (i)) == 0)
// OBSOLETE 	    {
// OBSOLETE 	      fprintf_filtered (file, "\t");
// OBSOLETE 	      val_print (REGISTER_VIRTUAL_TYPE (i), virtual_buffer, 0, 0,
// OBSOLETE 			 file, 0, 1, 0, Val_pretty_default);
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Some h8500 specific info.  */
// OBSOLETE       h8500_print_register_hook (i);
// OBSOLETE 
// OBSOLETE       fprintf_filtered (file, "\n");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE h8500_do_registers_info (int regnum, int all)
// OBSOLETE {
// OBSOLETE   h8500_print_registers_info (current_gdbarch, gdb_stdout, deprecated_selected_frame,
// OBSOLETE 			      regnum, all);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE h8500_register_size (int regno)
// OBSOLETE {
// OBSOLETE   switch (regno)
// OBSOLETE     {
// OBSOLETE     case SEG_C_REGNUM:
// OBSOLETE     case SEG_D_REGNUM:
// OBSOLETE     case SEG_E_REGNUM:
// OBSOLETE     case SEG_T_REGNUM:
// OBSOLETE       return 1;
// OBSOLETE     case R0_REGNUM:
// OBSOLETE     case R1_REGNUM:
// OBSOLETE     case R2_REGNUM:
// OBSOLETE     case R3_REGNUM:
// OBSOLETE     case R4_REGNUM:
// OBSOLETE     case R5_REGNUM:
// OBSOLETE     case R6_REGNUM:
// OBSOLETE     case R7_REGNUM:
// OBSOLETE     case CCR_REGNUM:
// OBSOLETE       return 2;
// OBSOLETE 
// OBSOLETE     case PR0_REGNUM:
// OBSOLETE     case PR1_REGNUM:
// OBSOLETE     case PR2_REGNUM:
// OBSOLETE     case PR3_REGNUM:
// OBSOLETE     case PR4_REGNUM:
// OBSOLETE     case PR5_REGNUM:
// OBSOLETE     case PR6_REGNUM:
// OBSOLETE     case PR7_REGNUM:
// OBSOLETE     case PC_REGNUM:
// OBSOLETE       return 4;
// OBSOLETE     default:
// OBSOLETE       internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE struct type *
// OBSOLETE h8500_register_virtual_type (int regno)
// OBSOLETE {
// OBSOLETE   switch (regno)
// OBSOLETE     {
// OBSOLETE     case SEG_C_REGNUM:
// OBSOLETE     case SEG_E_REGNUM:
// OBSOLETE     case SEG_D_REGNUM:
// OBSOLETE     case SEG_T_REGNUM:
// OBSOLETE       return builtin_type_unsigned_char;
// OBSOLETE     case R0_REGNUM:
// OBSOLETE     case R1_REGNUM:
// OBSOLETE     case R2_REGNUM:
// OBSOLETE     case R3_REGNUM:
// OBSOLETE     case R4_REGNUM:
// OBSOLETE     case R5_REGNUM:
// OBSOLETE     case R6_REGNUM:
// OBSOLETE     case R7_REGNUM:
// OBSOLETE     case CCR_REGNUM:
// OBSOLETE       return builtin_type_unsigned_short;
// OBSOLETE     case PR0_REGNUM:
// OBSOLETE     case PR1_REGNUM:
// OBSOLETE     case PR2_REGNUM:
// OBSOLETE     case PR3_REGNUM:
// OBSOLETE     case PR4_REGNUM:
// OBSOLETE     case PR5_REGNUM:
// OBSOLETE     case PR6_REGNUM:
// OBSOLETE     case PR7_REGNUM:
// OBSOLETE     case PC_REGNUM:
// OBSOLETE       return builtin_type_unsigned_long;
// OBSOLETE     default:
// OBSOLETE       internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Put here the code to store, into a struct frame_saved_regs,
// OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO.
// OBSOLETE    This includes special registers such as pc and fp saved in special
// OBSOLETE    ways in the stack frame.  sp is even more special:
// OBSOLETE    the address we return for it IS the sp for the next frame.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE frame_find_saved_regs (struct frame_info *frame_info,
// OBSOLETE 		       struct frame_saved_regs *frame_saved_regs)
// OBSOLETE {
// OBSOLETE   register int regnum;
// OBSOLETE   register int regmask;
// OBSOLETE   register CORE_ADDR next_addr;
// OBSOLETE   register CORE_ADDR pc;
// OBSOLETE   unsigned char thebyte;
// OBSOLETE 
// OBSOLETE   memset (frame_saved_regs, '\0', sizeof *frame_saved_regs);
// OBSOLETE 
// OBSOLETE   if ((frame_info)->pc >= (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 4
// OBSOLETE       && (frame_info)->pc <= (frame_info)->frame)
// OBSOLETE     {
// OBSOLETE       next_addr = (frame_info)->frame;
// OBSOLETE       pc = (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 4;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       pc = get_pc_function_start ((frame_info)->pc);
// OBSOLETE       /* Verify we have a link a6 instruction next;
// OBSOLETE          if not we lose.  If we win, find the address above the saved
// OBSOLETE          regs using the amount of storage from the link instruction.
// OBSOLETE        */
// OBSOLETE 
// OBSOLETE       thebyte = read_memory_integer (pc, 1);
// OBSOLETE       if (0x1f == thebyte)
// OBSOLETE 	next_addr = (frame_info)->frame + read_memory_integer (pc += 1, 2), pc += 2;
// OBSOLETE       else if (0x17 == thebyte)
// OBSOLETE 	next_addr = (frame_info)->frame + read_memory_integer (pc += 1, 1), pc += 1;
// OBSOLETE       else
// OBSOLETE 	goto lose;
// OBSOLETE #if 0
// OBSOLETE       /* FIXME steve */
// OBSOLETE       /* If have an add:g.waddal #-n, sp next, adjust next_addr.  */
// OBSOLETE       if ((0x0c0177777 & read_memory_integer (pc, 2)) == 0157774)
// OBSOLETE 	next_addr += read_memory_integer (pc += 2, 4), pc += 4;
// OBSOLETE #endif
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   thebyte = read_memory_integer (pc, 1);
// OBSOLETE   if (thebyte == 0x12)
// OBSOLETE     {
// OBSOLETE       /* Got stm */
// OBSOLETE       pc++;
// OBSOLETE       regmask = read_memory_integer (pc, 1);
// OBSOLETE       pc++;
// OBSOLETE       for (regnum = 0; regnum < 8; regnum++, regmask >>= 1)
// OBSOLETE 	{
// OBSOLETE 	  if (regmask & 1)
// OBSOLETE 	    {
// OBSOLETE 	      (frame_saved_regs)->regs[regnum] = (next_addr += 2) - 2;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE       thebyte = read_memory_integer (pc, 1);
// OBSOLETE     }
// OBSOLETE   /* Maybe got a load of pushes */
// OBSOLETE   while (thebyte == 0xbf)
// OBSOLETE     {
// OBSOLETE       pc++;
// OBSOLETE       regnum = read_memory_integer (pc, 1) & 0x7;
// OBSOLETE       pc++;
// OBSOLETE       (frame_saved_regs)->regs[regnum] = (next_addr += 2) - 2;
// OBSOLETE       thebyte = read_memory_integer (pc, 1);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE lose:;
// OBSOLETE 
// OBSOLETE   /* Remember the address of the frame pointer */
// OBSOLETE   (frame_saved_regs)->regs[FP_REGNUM] = (frame_info)->frame;
// OBSOLETE 
// OBSOLETE   /* This is where the old sp is hidden */
// OBSOLETE   (frame_saved_regs)->regs[SP_REGNUM] = (frame_info)->frame;
// OBSOLETE 
// OBSOLETE   /* And the PC - remember the pushed FP is always two bytes long */
// OBSOLETE   (frame_saved_regs)->regs[PC_REGNUM] = (frame_info)->frame + 2;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE saved_pc_after_call (void)
// OBSOLETE {
// OBSOLETE   int x;
// OBSOLETE   int a = read_register (SP_REGNUM);
// OBSOLETE 
// OBSOLETE   x = read_memory_integer (a, code_size);
// OBSOLETE   if (code_size == 2)
// OBSOLETE     {
// OBSOLETE       /* Stick current code segement onto top */
// OBSOLETE       x &= 0xffff;
// OBSOLETE       x |= read_register (SEG_C_REGNUM) << 16;
// OBSOLETE     }
// OBSOLETE   x &= 0xffffff;
// OBSOLETE   return x;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE h8500_set_pointer_size (int newsize)
// OBSOLETE {
// OBSOLETE   static int oldsize = 0;
// OBSOLETE 
// OBSOLETE   if (oldsize != newsize)
// OBSOLETE     {
// OBSOLETE       printf_unfiltered ("pointer size set to %d bits\n", newsize);
// OBSOLETE       oldsize = newsize;
// OBSOLETE       if (newsize == 32)
// OBSOLETE 	{
// OBSOLETE 	  minimum_mode = 0;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  minimum_mode = 1;
// OBSOLETE 	}
// OBSOLETE       _initialize_gdbtypes ();
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE big_command (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   h8500_set_pointer_size (32);
// OBSOLETE   code_size = 4;
// OBSOLETE   data_size = 4;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE medium_command (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   h8500_set_pointer_size (32);
// OBSOLETE   code_size = 4;
// OBSOLETE   data_size = 2;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE compact_command (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   h8500_set_pointer_size (32);
// OBSOLETE   code_size = 2;
// OBSOLETE   data_size = 4;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE small_command (char *arg, int from_tty)
// OBSOLETE {
// OBSOLETE   h8500_set_pointer_size (16);
// OBSOLETE   code_size = 2;
// OBSOLETE   data_size = 2;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static struct cmd_list_element *setmemorylist;
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE set_memory (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   printf_unfiltered ("\"set memory\" must be followed by the name of a memory subcommand.\n");
// OBSOLETE   help_list (setmemorylist, "set memory ", -1, gdb_stdout);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* See if variable name is ppc or pr[0-7] */
// OBSOLETE 
// OBSOLETE int
// OBSOLETE h8500_is_trapped_internalvar (char *name)
// OBSOLETE {
// OBSOLETE   if (name[0] != 'p')
// OBSOLETE     return 0;
// OBSOLETE 
// OBSOLETE   if (strcmp (name + 1, "pc") == 0)
// OBSOLETE     return 1;
// OBSOLETE 
// OBSOLETE   if (name[1] == 'r'
// OBSOLETE       && name[2] >= '0'
// OBSOLETE       && name[2] <= '7'
// OBSOLETE       && name[3] == '\000')
// OBSOLETE     return 1;
// OBSOLETE   else
// OBSOLETE     return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE struct value *
// OBSOLETE h8500_value_of_trapped_internalvar (struct internalvar *var)
// OBSOLETE {
// OBSOLETE   LONGEST regval;
// OBSOLETE   unsigned char regbuf[4];
// OBSOLETE   int page_regnum, regnum;
// OBSOLETE 
// OBSOLETE   regnum = var->name[2] == 'c' ? PC_REGNUM : var->name[2] - '0';
// OBSOLETE 
// OBSOLETE   switch (var->name[2])
// OBSOLETE     {
// OBSOLETE     case 'c':
// OBSOLETE       page_regnum = SEG_C_REGNUM;
// OBSOLETE       break;
// OBSOLETE     case '0':
// OBSOLETE     case '1':
// OBSOLETE     case '2':
// OBSOLETE     case '3':
// OBSOLETE       page_regnum = SEG_D_REGNUM;
// OBSOLETE       break;
// OBSOLETE     case '4':
// OBSOLETE     case '5':
// OBSOLETE       page_regnum = SEG_E_REGNUM;
// OBSOLETE       break;
// OBSOLETE     case '6':
// OBSOLETE     case '7':
// OBSOLETE       page_regnum = SEG_T_REGNUM;
// OBSOLETE       break;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   get_saved_register (regbuf, NULL, NULL, deprecated_selected_frame, page_regnum, NULL);
// OBSOLETE   regval = regbuf[0] << 16;
// OBSOLETE 
// OBSOLETE   get_saved_register (regbuf, NULL, NULL, deprecated_selected_frame, regnum, NULL);
// OBSOLETE   regval |= regbuf[0] << 8 | regbuf[1];		/* XXX host/target byte order */
// OBSOLETE 
// OBSOLETE   xfree (var->value);		/* Free up old value */
// OBSOLETE 
// OBSOLETE   var->value = value_from_longest (builtin_type_unsigned_long, regval);
// OBSOLETE   release_value (var->value);	/* Unchain new value */
// OBSOLETE 
// OBSOLETE   VALUE_LVAL (var->value) = lval_internalvar;
// OBSOLETE   VALUE_INTERNALVAR (var->value) = var;
// OBSOLETE   return var->value;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE h8500_set_trapped_internalvar (struct internalvar *var, struct value *newval,
// OBSOLETE 			       int bitpos, int bitsize, int offset)
// OBSOLETE {
// OBSOLETE   char *page_regnum, *regnum;
// OBSOLETE   char expression[100];
// OBSOLETE   unsigned new_regval;
// OBSOLETE   struct type *type;
// OBSOLETE   enum type_code newval_type_code;
// OBSOLETE 
// OBSOLETE   type = check_typedef (VALUE_TYPE (newval));
// OBSOLETE   newval_type_code = TYPE_CODE (type);
// OBSOLETE 
// OBSOLETE   if ((newval_type_code != TYPE_CODE_INT
// OBSOLETE        && newval_type_code != TYPE_CODE_PTR)
// OBSOLETE       || TYPE_LENGTH (type) != sizeof (new_regval))
// OBSOLETE     error ("Illegal type (%s) for assignment to $%s\n",
// OBSOLETE 	   TYPE_NAME (VALUE_TYPE (newval)), var->name);
// OBSOLETE 
// OBSOLETE   new_regval = *(long *) VALUE_CONTENTS_RAW (newval);
// OBSOLETE 
// OBSOLETE   regnum = var->name + 1;
// OBSOLETE 
// OBSOLETE   switch (var->name[2])
// OBSOLETE     {
// OBSOLETE     case 'c':
// OBSOLETE       page_regnum = "cp";
// OBSOLETE       break;
// OBSOLETE     case '0':
// OBSOLETE     case '1':
// OBSOLETE     case '2':
// OBSOLETE     case '3':
// OBSOLETE       page_regnum = "dp";
// OBSOLETE       break;
// OBSOLETE     case '4':
// OBSOLETE     case '5':
// OBSOLETE       page_regnum = "ep";
// OBSOLETE       break;
// OBSOLETE     case '6':
// OBSOLETE     case '7':
// OBSOLETE       page_regnum = "tp";
// OBSOLETE       break;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   sprintf (expression, "$%s=%d", page_regnum, new_regval >> 16);
// OBSOLETE   parse_and_eval (expression);
// OBSOLETE 
// OBSOLETE   sprintf (expression, "$%s=%d", regnum, new_regval & 0xffff);
// OBSOLETE   parse_and_eval (expression);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE h8500_read_sp (void)
// OBSOLETE {
// OBSOLETE   return read_register (PR7_REGNUM);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE h8500_write_sp (CORE_ADDR v)
// OBSOLETE {
// OBSOLETE   write_register (PR7_REGNUM, v);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE h8500_read_pc (ptid_t ptid)
// OBSOLETE {
// OBSOLETE   return read_register (PC_REGNUM);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE h8500_write_pc (CORE_ADDR v, ptid_t ptid)
// OBSOLETE {
// OBSOLETE   write_register (PC_REGNUM, v);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE h8500_read_fp (void)
// OBSOLETE {
// OBSOLETE   return read_register (PR6_REGNUM);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_h8500_tdep (void)
// OBSOLETE {
// OBSOLETE   tm_print_insn = print_insn_h8500;
// OBSOLETE 
// OBSOLETE   add_prefix_cmd ("memory", no_class, set_memory,
// OBSOLETE 		  "set the memory model", &setmemorylist, "set memory ", 0,
// OBSOLETE 		  &setlist);
// OBSOLETE 
// OBSOLETE   add_cmd ("small", class_support, small_command,
// OBSOLETE       "Set small memory model. (16 bit code, 16 bit data)", &setmemorylist);
// OBSOLETE 
// OBSOLETE   add_cmd ("big", class_support, big_command,
// OBSOLETE 	"Set big memory model. (32 bit code, 32 bit data)", &setmemorylist);
// OBSOLETE 
// OBSOLETE   add_cmd ("medium", class_support, medium_command,
// OBSOLETE      "Set medium memory model. (32 bit code, 16 bit data)", &setmemorylist);
// OBSOLETE 
// OBSOLETE   add_cmd ("compact", class_support, compact_command,
// OBSOLETE     "Set compact memory model. (16 bit code, 32 bit data)", &setmemorylist);
// OBSOLETE 
// OBSOLETE }
