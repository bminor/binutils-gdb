// OBSOLETE /* Target-machine dependent code for Zilog Z8000, for GDB.
// OBSOLETE 
// OBSOLETE    Copyright 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2001,
// OBSOLETE    2002, 2003 Free Software Foundation, Inc.
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
// OBSOLETE #include "gdbcmd.h"
// OBSOLETE #include "gdbtypes.h"
// OBSOLETE #include "dis-asm.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #include "value.h" /* For read_register() */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE static int read_memory_pointer (CORE_ADDR x);
// OBSOLETE 
// OBSOLETE /* Return the saved PC from this frame.
// OBSOLETE 
// OBSOLETE    If the frame has a memory copy of SRP_REGNUM, use that.  If not,
// OBSOLETE    just use the register SRP_REGNUM itself.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE z8k_frame_saved_pc (struct frame_info *frame)
// OBSOLETE {
// OBSOLETE   return read_memory_pointer (frame->frame + (BIG ? 4 : 2));
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #define IS_PUSHL(x) (BIG ? ((x & 0xfff0) == 0x91e0):((x & 0xfff0) == 0x91F0))
// OBSOLETE #define IS_PUSHW(x) (BIG ? ((x & 0xfff0) == 0x93e0):((x & 0xfff0)==0x93f0))
// OBSOLETE #define IS_MOVE_FP(x) (BIG ? x == 0xa1ea : x == 0xa1fa)
// OBSOLETE #define IS_MOV_SP_FP(x) (BIG ? x == 0x94ea : x == 0x0d76)
// OBSOLETE #define IS_SUB2_SP(x) (x==0x1b87)
// OBSOLETE #define IS_MOVK_R5(x) (x==0x7905)
// OBSOLETE #define IS_SUB_SP(x) ((x & 0xffff) == 0x020f)
// OBSOLETE #define IS_PUSH_FP(x) (BIG ? (x == 0x93ea) : (x == 0x93fa))
// OBSOLETE 
// OBSOLETE /* work out how much local space is on the stack and
// OBSOLETE    return the pc pointing to the first push */
// OBSOLETE 
// OBSOLETE static CORE_ADDR
// OBSOLETE skip_adjust (CORE_ADDR pc, int *size)
// OBSOLETE {
// OBSOLETE   *size = 0;
// OBSOLETE 
// OBSOLETE   if (IS_PUSH_FP (read_memory_short (pc))
// OBSOLETE       && IS_MOV_SP_FP (read_memory_short (pc + 2)))
// OBSOLETE     {
// OBSOLETE       /* This is a function with an explict frame pointer */
// OBSOLETE       pc += 4;
// OBSOLETE       *size += 2;		/* remember the frame pointer */
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* remember any stack adjustment */
// OBSOLETE   if (IS_SUB_SP (read_memory_short (pc)))
// OBSOLETE     {
// OBSOLETE       *size += read_memory_short (pc + 2);
// OBSOLETE       pc += 4;
// OBSOLETE     }
// OBSOLETE   return pc;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static CORE_ADDR examine_frame (CORE_ADDR, CORE_ADDR * regs, CORE_ADDR);
// OBSOLETE static CORE_ADDR
// OBSOLETE examine_frame (CORE_ADDR pc, CORE_ADDR *regs, CORE_ADDR sp)
// OBSOLETE {
// OBSOLETE   int w = read_memory_short (pc);
// OBSOLETE   int offset = 0;
// OBSOLETE   int regno;
// OBSOLETE 
// OBSOLETE   for (regno = 0; regno < NUM_REGS; regno++)
// OBSOLETE     regs[regno] = 0;
// OBSOLETE 
// OBSOLETE   while (IS_PUSHW (w) || IS_PUSHL (w))
// OBSOLETE     {
// OBSOLETE       /* work out which register is being pushed to where */
// OBSOLETE       if (IS_PUSHL (w))
// OBSOLETE 	{
// OBSOLETE 	  regs[w & 0xf] = offset;
// OBSOLETE 	  regs[(w & 0xf) + 1] = offset + 2;
// OBSOLETE 	  offset += 4;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  regs[w & 0xf] = offset;
// OBSOLETE 	  offset += 2;
// OBSOLETE 	}
// OBSOLETE       pc += 2;
// OBSOLETE       w = read_memory_short (pc);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (IS_MOVE_FP (w))
// OBSOLETE     {
// OBSOLETE       /* We know the fp */
// OBSOLETE 
// OBSOLETE     }
// OBSOLETE   else if (IS_SUB_SP (w))
// OBSOLETE     {
// OBSOLETE       /* Subtracting a value from the sp, so were in a function
// OBSOLETE          which needs stack space for locals, but has no fp.  We fake up
// OBSOLETE          the values as if we had an fp */
// OBSOLETE       regs[FP_REGNUM] = sp;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       /* This one didn't have an fp, we'll fake it up */
// OBSOLETE       regs[SP_REGNUM] = sp;
// OBSOLETE     }
// OBSOLETE   /* stack pointer contains address of next frame */
// OBSOLETE   /*  regs[fp_regnum()] = fp; */
// OBSOLETE   regs[SP_REGNUM] = sp;
// OBSOLETE   return pc;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE z8k_skip_prologue (CORE_ADDR start_pc)
// OBSOLETE {
// OBSOLETE   CORE_ADDR dummy[NUM_REGS];
// OBSOLETE 
// OBSOLETE   return examine_frame (start_pc, dummy, 0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE z8k_addr_bits_remove (CORE_ADDR addr)
// OBSOLETE {
// OBSOLETE   return (addr & PTR_MASK);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE read_memory_pointer (CORE_ADDR x)
// OBSOLETE {
// OBSOLETE   return read_memory_integer (ADDR_BITS_REMOVE (x), BIG ? 4 : 2);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE z8k_frame_chain (struct frame_info *thisframe)
// OBSOLETE {
// OBSOLETE   if (!inside_entry_file (get_frame_pc (thisframe)))
// OBSOLETE     {
// OBSOLETE       return read_memory_pointer (thisframe->frame);
// OBSOLETE     }
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Put here the code to store, into a struct frame_saved_regs,
// OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO.
// OBSOLETE    This includes special registers such as pc and fp saved in special
// OBSOLETE    ways in the stack frame.  sp is even more special:
// OBSOLETE    the address we return for it IS the sp for the next frame.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE z8k_frame_init_saved_regs (struct frame_info *frame_info)
// OBSOLETE {
// OBSOLETE   CORE_ADDR pc;
// OBSOLETE   int w;
// OBSOLETE 
// OBSOLETE   frame_saved_regs_zalloc (frame_info);
// OBSOLETE   pc = get_pc_function_start (get_frame_pc (frame_info));
// OBSOLETE 
// OBSOLETE   /* wander down the instruction stream */
// OBSOLETE   examine_frame (pc, frame_info->saved_regs, frame_info->frame);
// OBSOLETE 
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE z8k_push_dummy_frame (void)
// OBSOLETE {
// OBSOLETE   internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE gdb_print_insn_z8k (bfd_vma memaddr, disassemble_info *info)
// OBSOLETE {
// OBSOLETE   if (BIG)
// OBSOLETE     return print_insn_z8001 (memaddr, info);
// OBSOLETE   else
// OBSOLETE     return print_insn_z8002 (memaddr, info);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Fetch the instruction at ADDR, returning 0 if ADDR is beyond LIM or
// OBSOLETE    is not the address of a valid instruction, the address of the next
// OBSOLETE    instruction beyond ADDR otherwise.  *PWORD1 receives the first word
// OBSOLETE    of the instruction. */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE NEXT_PROLOGUE_INSN (CORE_ADDR addr, CORE_ADDR lim, short *pword1)
// OBSOLETE {
// OBSOLETE   char buf[2];
// OBSOLETE   if (addr < lim + 8)
// OBSOLETE     {
// OBSOLETE       read_memory (addr, buf, 2);
// OBSOLETE       *pword1 = extract_signed_integer (buf, 2);
// OBSOLETE 
// OBSOLETE       return addr + 2;
// OBSOLETE     }
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE /* Put here the code to store, into a struct frame_saved_regs,
// OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO.
// OBSOLETE    This includes special registers such as pc and fp saved in special
// OBSOLETE    ways in the stack frame.  sp is even more special:
// OBSOLETE    the address we return for it IS the sp for the next frame.
// OBSOLETE 
// OBSOLETE    We cache the result of doing this in the frame_cache_obstack, since
// OBSOLETE    it is fairly expensive.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE frame_find_saved_regs (struct frame_info *fip, struct frame_saved_regs *fsrp)
// OBSOLETE {
// OBSOLETE   int locals;
// OBSOLETE   CORE_ADDR pc;
// OBSOLETE   CORE_ADDR adr;
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   memset (fsrp, 0, sizeof *fsrp);
// OBSOLETE 
// OBSOLETE   pc = skip_adjust (get_pc_function_start (get_frame_pc (fip)), &locals);
// OBSOLETE 
// OBSOLETE   {
// OBSOLETE     adr = get_frame_base (fip) - locals;
// OBSOLETE     for (i = 0; i < 8; i++)
// OBSOLETE       {
// OBSOLETE 	int word = read_memory_short (pc);
// OBSOLETE 
// OBSOLETE 	pc += 2;
// OBSOLETE 	if (IS_PUSHL (word))
// OBSOLETE 	  {
// OBSOLETE 	    fsrp->regs[word & 0xf] = adr;
// OBSOLETE 	    fsrp->regs[(word & 0xf) + 1] = adr - 2;
// OBSOLETE 	    adr -= 4;
// OBSOLETE 	  }
// OBSOLETE 	else if (IS_PUSHW (word))
// OBSOLETE 	  {
// OBSOLETE 	    fsrp->regs[word & 0xf] = adr;
// OBSOLETE 	    adr -= 2;
// OBSOLETE 	  }
// OBSOLETE 	else
// OBSOLETE 	  break;
// OBSOLETE       }
// OBSOLETE 
// OBSOLETE   }
// OBSOLETE 
// OBSOLETE   fsrp->regs[PC_REGNUM] = fip->frame + 4;
// OBSOLETE   fsrp->regs[FP_REGNUM] = fip->frame;
// OBSOLETE 
// OBSOLETE }
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE int
// OBSOLETE z8k_saved_pc_after_call (struct frame_info *frame)
// OBSOLETE {
// OBSOLETE   return ADDR_BITS_REMOVE
// OBSOLETE     (read_memory_integer (read_register (SP_REGNUM), PTR_SIZE));
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE void
// OBSOLETE extract_return_value (struct type *type, char *regbuf, char *valbuf)
// OBSOLETE {
// OBSOLETE   int b;
// OBSOLETE   int len = TYPE_LENGTH (type);
// OBSOLETE 
// OBSOLETE   for (b = 0; b < len; b += 2)
// OBSOLETE     {
// OBSOLETE       int todo = len - b;
// OBSOLETE 
// OBSOLETE       if (todo > 2)
// OBSOLETE 	todo = 2;
// OBSOLETE       memcpy (valbuf + b, regbuf + b, todo);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE write_return_value (struct type *type, char *valbuf)
// OBSOLETE {
// OBSOLETE   int reg;
// OBSOLETE   int len;
// OBSOLETE 
// OBSOLETE   for (len = 0; len < TYPE_LENGTH (type); len += 2)
// OBSOLETE     deprecated_write_register_bytes (REGISTER_BYTE (len / 2 + 2),
// OBSOLETE 				     valbuf + len, 2);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
// OBSOLETE {
// OBSOLETE   write_register (2, addr);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE z8k_print_register_hook (int regno)
// OBSOLETE {
// OBSOLETE   if ((regno & 1) == 0 && regno < 16)
// OBSOLETE     {
// OBSOLETE       unsigned char l[4];
// OBSOLETE 
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno, l + 0);
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno + 1, l + 2);
// OBSOLETE       printf_unfiltered ("\t");
// OBSOLETE       printf_unfiltered ("0x%02x%02x%02x%02x", l[0], l[1], l[2], l[3]);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if ((regno & 3) == 0 && regno < 16)
// OBSOLETE     {
// OBSOLETE       unsigned char l[8];
// OBSOLETE 
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno, l + 0);
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno + 1, l + 2);
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno + 2, l + 4);
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno + 3, l + 6);
// OBSOLETE 
// OBSOLETE       printf_unfiltered ("\t");
// OBSOLETE       printf_unfiltered ("0x%02x%02x%02x%02x%02x%02x%02x%02x",
// OBSOLETE                          l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7]);
// OBSOLETE     }
// OBSOLETE   if (regno == 15)
// OBSOLETE     {
// OBSOLETE       unsigned short rval;
// OBSOLETE       int i;
// OBSOLETE 
// OBSOLETE       frame_register_read (deprecated_selected_frame, regno, (char *) (&rval));
// OBSOLETE 
// OBSOLETE       printf_unfiltered ("\n");
// OBSOLETE       for (i = 0; i < 10; i += 2)
// OBSOLETE 	{
// OBSOLETE 	  printf_unfiltered ("(sp+%d=%04x)", i,
// OBSOLETE 			     (unsigned int)read_memory_short (rval + i));
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE z8k_print_registers_info (struct gdbarch *gdbarch,
// OBSOLETE 			  struct ui_file *file,
// OBSOLETE 			  struct frame_info *frame,
// OBSOLETE 			  int regnum, int print_all)
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
// OBSOLETE       /* Some z8k specific info.  */
// OBSOLETE       z8k_print_register_hook (i);
// OBSOLETE 
// OBSOLETE       fprintf_filtered (file, "\n");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE z8k_do_registers_info (int regnum, int all)
// OBSOLETE {
// OBSOLETE   z8k_print_registers_info (current_gdbarch, gdb_stdout,
// OBSOLETE 			    deprecated_selected_frame, regnum, all);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE z8k_pop_frame (void)
// OBSOLETE {
// OBSOLETE }
// OBSOLETE 
// OBSOLETE struct cmd_list_element *setmemorylist;
// OBSOLETE 
// OBSOLETE void
// OBSOLETE z8k_set_pointer_size (int newsize)
// OBSOLETE {
// OBSOLETE   static int oldsize = 0;
// OBSOLETE 
// OBSOLETE   if (oldsize != newsize)
// OBSOLETE     {
// OBSOLETE       printf_unfiltered ("pointer size set to %d bits\n", newsize);
// OBSOLETE       oldsize = newsize;
// OBSOLETE       if (newsize == 32)
// OBSOLETE 	{
// OBSOLETE 	  BIG = 1;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  BIG = 0;
// OBSOLETE 	}
// OBSOLETE       /* FIXME: This code should be using the GDBARCH framework to
// OBSOLETE          handle changed type sizes.  If this problem is ever fixed
// OBSOLETE          (the direct reference to _initialize_gdbtypes() below
// OBSOLETE          eliminated) then Makefile.in should be updated so that
// OBSOLETE          z8k-tdep.c is again compiled with -Werror. */
// OBSOLETE       _initialize_gdbtypes ();
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE segmented_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   z8k_set_pointer_size (32);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE unsegmented_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   z8k_set_pointer_size (16);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE set_memory (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   printf_unfiltered ("\"set memory\" must be followed by the name of a memory subcommand.\n");
// OBSOLETE   help_list (setmemorylist, "set memory ", -1, gdb_stdout);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_z8ktdep (void)
// OBSOLETE {
// OBSOLETE   tm_print_insn = gdb_print_insn_z8k;
// OBSOLETE 
// OBSOLETE   add_prefix_cmd ("memory", no_class, set_memory,
// OBSOLETE 		  "set the memory model", &setmemorylist, "set memory ", 0,
// OBSOLETE 		  &setlist);
// OBSOLETE   add_cmd ("segmented", class_support, segmented_command,
// OBSOLETE 	   "Set segmented memory model.", &setmemorylist);
// OBSOLETE   add_cmd ("unsegmented", class_support, unsegmented_command,
// OBSOLETE 	   "Set unsegmented memory model.", &setmemorylist);
// OBSOLETE 
// OBSOLETE }
