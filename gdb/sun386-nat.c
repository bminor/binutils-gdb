/* OBSOLETE /* Native support for Sun 386i's for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1995, 1999, 2000, 2001 */
/* OBSOLETE    Free Software Foundation, Inc. */
/* OBSOLETE    Changes for sun386i by Jean Daniel Fekete (jdf@litp.univ-p6-7.fr), */
/* OBSOLETE    C2V Paris, April 89. */
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
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "inferior.h" */
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE #include <sys/param.h> */
/* OBSOLETE #include <sys/dir.h> */
/* OBSOLETE #include <sys/user.h> */
/* OBSOLETE #include <signal.h> */
/* OBSOLETE #include <sys/ioctl.h> */
/* OBSOLETE #include <fcntl.h> */
/* OBSOLETE  */
/* OBSOLETE #include <sys/ptrace.h> */
/* OBSOLETE #include <machine/reg.h> */
/* OBSOLETE  */
/* OBSOLETE #include <sys/file.h> */
/* OBSOLETE #include "gdb_stat.h" */
/* OBSOLETE #include <sys/core.h> */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Machine-dependent code which would otherwise be in corefile.c */ */
/* OBSOLETE /* Work with core files, for GDB. */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE core_file_command (char *filename, int from_tty) */
/* OBSOLETE { */
/* OBSOLETE   int val; */
/* OBSOLETE  */
/* OBSOLETE   /* Discard all vestiges of any previous core file */
/* OBSOLETE      and mark data and stack spaces as empty.  */ */
/* OBSOLETE  */
/* OBSOLETE   if (corefile) */
/* OBSOLETE     xfree (corefile); */
/* OBSOLETE   corefile = 0; */
/* OBSOLETE  */
/* OBSOLETE   if (corechan >= 0) */
/* OBSOLETE     close (corechan); */
/* OBSOLETE   corechan = -1; */
/* OBSOLETE  */
/* OBSOLETE   data_start = 0; */
/* OBSOLETE   data_end = 0; */
/* OBSOLETE   stack_start = STACK_END_ADDR; */
/* OBSOLETE   stack_end = STACK_END_ADDR; */
/* OBSOLETE  */
/* OBSOLETE   /* Now, if a new core file was specified, open it and digest it.  */ */
/* OBSOLETE  */
/* OBSOLETE   if (filename) */
/* OBSOLETE     { */
/* OBSOLETE       filename = tilde_expand (filename); */
/* OBSOLETE       make_cleanup (xfree, filename); */
/* OBSOLETE  */
/* OBSOLETE       if (have_inferior_p ()) */
/* OBSOLETE 	error ("To look at a core file, you must kill the program with \"kill\"."); */
/* OBSOLETE       corechan = open (filename, O_RDONLY, 0); */
/* OBSOLETE       if (corechan < 0) */
/* OBSOLETE 	perror_with_name (filename); */
/* OBSOLETE  */
/* OBSOLETE       { */
/* OBSOLETE 	struct core corestr; */
/* OBSOLETE  */
/* OBSOLETE 	val = myread (corechan, &corestr, sizeof corestr); */
/* OBSOLETE 	if (val < 0) */
/* OBSOLETE 	  perror_with_name (filename); */
/* OBSOLETE 	if (corestr.c_magic != CORE_MAGIC) */
/* OBSOLETE 	  error ("\"%s\" does not appear to be a core dump file (magic 0x%x, expected 0x%x)", */
/* OBSOLETE 		 filename, corestr.c_magic, (int) CORE_MAGIC); */
/* OBSOLETE 	else if (sizeof (struct core) != corestr.c_len) */
/* OBSOLETE 	    error ("\"%s\" has an invalid struct core length (%d, expected %d)", */
/* OBSOLETE 		   filename, corestr.c_len, (int) sizeof (struct core)); */
/* OBSOLETE  */
/* OBSOLETE 	data_start = exec_data_start; */
/* OBSOLETE 	data_end = data_start + corestr.c_dsize; */
/* OBSOLETE 	stack_start = stack_end - corestr.c_ssize; */
/* OBSOLETE 	data_offset = sizeof corestr; */
/* OBSOLETE 	stack_offset = sizeof corestr + corestr.c_dsize; */
/* OBSOLETE  */
/* OBSOLETE 	memcpy (registers, &corestr.c_regs, sizeof corestr.c_regs); */
/* OBSOLETE  */
/* OBSOLETE 	memcpy (&registers[REGISTER_BYTE (FP0_REGNUM)], */
/* OBSOLETE 		corestr.c_fpu.f_fpstatus.f_st, */
/* OBSOLETE 		sizeof corestr.c_fpu.f_fpstatus.f_st); */
/* OBSOLETE 	memcpy (&registers[REGISTER_BYTE (FPC_REGNUM)], */
/* OBSOLETE 		&corestr.c_fpu.f_fpstatus.f_ctrl, */
/* OBSOLETE 		sizeof corestr.c_fpu.f_fpstatus - */
/* OBSOLETE 		sizeof corestr.c_fpu.f_fpstatus.f_st); */
/* OBSOLETE  */
/* OBSOLETE 	/* the struct aouthdr of sun coff is not the struct exec stored */
/* OBSOLETE 	   in the core file. */ */
/* OBSOLETE 	memcpy (&core_aouthdr, &corestr.c_aouthdr, sizeof (struct exec)); */
/* OBSOLETE #ifndef COFF_ENCAPSULATE */
/* OBSOLETE 	core_aouthdr.magic = corestr.c_aouthdr.a_info; */
/* OBSOLETE 	core_aouthdr.vstamp = /*SUNVERSION */ 31252; */
/* OBSOLETE #endif */
/* OBSOLETE 	printf_unfiltered ("Core file is from \"%s\".\n", corestr.c_cmdname); */
/* OBSOLETE 	if (corestr.c_signo > 0) */
/* OBSOLETE 	  printf_unfiltered ("Program terminated with signal %d, %s.\n", */
/* OBSOLETE 			 corestr.c_signo, safe_strsignal (corestr.c_signo)); */
/* OBSOLETE       } */
/* OBSOLETE       if (filename[0] == '/') */
/* OBSOLETE 	corefile = savestring (filename, strlen (filename)); */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  corefile = concat (current_directory, "/", filename, NULL); */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE       flush_cached_frames (); */
/* OBSOLETE       select_frame (get_current_frame (), 0); */
/* OBSOLETE  */
/* OBSOLETE       validate_files (); */
/* OBSOLETE     } */
/* OBSOLETE   else if (from_tty) */
/* OBSOLETE     printf_unfiltered ("No core file now.\n"); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE i387_to_double (char *from, char *to) */
/* OBSOLETE { */
/* OBSOLETE   long *lp; */
/* OBSOLETE   /* push extended mode on 387 stack, then pop in double mode */
/* OBSOLETE  */
/* OBSOLETE    * first, set exception masks so no error is generated - */
/* OBSOLETE    * number will be rounded to inf or 0, if necessary  */
/* OBSOLETE    */ */
/* OBSOLETE   asm ("pushl %eax");		/* grab a stack slot */ */
/* OBSOLETE   asm ("fstcw (%esp)");		/* get 387 control word */ */
/* OBSOLETE   asm ("movl (%esp),%eax");	/* save old value */ */
/* OBSOLETE   asm ("orl $0x3f,%eax");	/* mask all exceptions */ */
/* OBSOLETE   asm ("pushl %eax"); */
/* OBSOLETE   asm ("fldcw (%esp)");		/* load new value into 387 */ */
/* OBSOLETE  */
/* OBSOLETE   asm ("movl 8(%ebp),%eax"); */
/* OBSOLETE   asm ("fldt (%eax)");		/* push extended number on 387 stack */ */
/* OBSOLETE   asm ("fwait"); */
/* OBSOLETE   asm ("movl 12(%ebp),%eax"); */
/* OBSOLETE   asm ("fstpl (%eax)");		/* pop double */ */
/* OBSOLETE   asm ("fwait"); */
/* OBSOLETE  */
/* OBSOLETE   asm ("popl %eax");		/* flush modified control word */ */
/* OBSOLETE   asm ("fnclex");		/* clear exceptions */ */
/* OBSOLETE   asm ("fldcw (%esp)");		/* restore original control word */ */
/* OBSOLETE   asm ("popl %eax");		/* flush saved copy */ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE double_to_i387 (char *from, char *to) */
/* OBSOLETE { */
/* OBSOLETE   /* push double mode on 387 stack, then pop in extended mode */
/* OBSOLETE    * no errors are possible because every 64-bit pattern */
/* OBSOLETE    * can be converted to an extended */
/* OBSOLETE    */ */
/* OBSOLETE   asm ("movl 8(%ebp),%eax"); */
/* OBSOLETE   asm ("fldl (%eax)"); */
/* OBSOLETE   asm ("fwait"); */
/* OBSOLETE   asm ("movl 12(%ebp),%eax"); */
/* OBSOLETE   asm ("fstpt (%eax)"); */
/* OBSOLETE   asm ("fwait"); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE fetch_inferior_registers (int regno) */
/* OBSOLETE { */
/* OBSOLETE   struct regs inferior_registers; */
/* OBSOLETE   struct fp_state inferior_fp_registers; */
/* OBSOLETE  */
/* OBSOLETE   registers_fetched (); */
/* OBSOLETE  */
/* OBSOLETE   ptrace (PTRACE_GETREGS, inferior_pid, */
/* OBSOLETE 	  (PTRACE_ARG3_TYPE) & inferior_registers); */
/* OBSOLETE   ptrace (PTRACE_GETFPREGS, inferior_pid, */
/* OBSOLETE 	  (PTRACE_ARG3_TYPE) & inferior_fp_registers); */
/* OBSOLETE  */
/* OBSOLETE   memcpy (registers, &inferior_registers, sizeof inferior_registers); */
/* OBSOLETE  */
/* OBSOLETE   memcpy (&registers[REGISTER_BYTE (FP0_REGNUM)], inferior_fp_registers.f_st, */
/* OBSOLETE 	  sizeof inferior_fp_registers.f_st); */
/* OBSOLETE   memcpy (&registers[REGISTER_BYTE (FPC_REGNUM)], */
/* OBSOLETE 	  &inferior_fp_registers.f_ctrl, */
/* OBSOLETE 	  sizeof inferior_fp_registers - sizeof inferior_fp_registers.f_st); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Store our register values back into the inferior. */
/* OBSOLETE    If REGNO is -1, do this for all registers. */
/* OBSOLETE    Otherwise, REGNO specifies which register (so we can save time).  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE store_inferior_registers (int regno) */
/* OBSOLETE { */
/* OBSOLETE   struct regs inferior_registers; */
/* OBSOLETE   struct fp_state inferior_fp_registers; */
/* OBSOLETE  */
/* OBSOLETE   memcpy (&inferior_registers, registers, 20 * 4); */
/* OBSOLETE  */
/* OBSOLETE   memcpy (inferior_fp_registers.f_st, */
/* OBSOLETE 	  &registers[REGISTER_BYTE (FP0_REGNUM)], */
/* OBSOLETE 	  sizeof inferior_fp_registers.f_st); */
/* OBSOLETE   memcpy (&inferior_fp_registers.f_ctrl, */
/* OBSOLETE 	  &registers[REGISTER_BYTE (FPC_REGNUM)], */
/* OBSOLETE 	  sizeof inferior_fp_registers - sizeof inferior_fp_registers.f_st); */
/* OBSOLETE  */
/* OBSOLETE #ifdef PTRACE_FP_BUG */
/* OBSOLETE   if (regno == FP_REGNUM || regno == -1) */
/* OBSOLETE     /* Storing the frame pointer requires a gross hack, in which an */
/* OBSOLETE        instruction that moves eax into ebp gets single-stepped.  */ */
/* OBSOLETE     { */
/* OBSOLETE       int stack = inferior_registers.r_reg[SP_REGNUM]; */
/* OBSOLETE       int stuff = ptrace (PTRACE_PEEKDATA, inferior_pid, */
/* OBSOLETE 			  (PTRACE_ARG3_TYPE) stack); */
/* OBSOLETE       int reg = inferior_registers.r_reg[EAX]; */
/* OBSOLETE       inferior_registers.r_reg[EAX] = */
/* OBSOLETE 	inferior_registers.r_reg[FP_REGNUM]; */
/* OBSOLETE       ptrace (PTRACE_SETREGS, inferior_pid, */
/* OBSOLETE 	      (PTRACE_ARG3_TYPE) & inferior_registers); */
/* OBSOLETE       ptrace (PTRACE_POKEDATA, inferior_pid, (PTRACE_ARG3_TYPE) stack, */
/* OBSOLETE 	      0xc589); */
/* OBSOLETE       ptrace (PTRACE_SINGLESTEP, inferior_pid, (PTRACE_ARG3_TYPE) stack, */
/* OBSOLETE 	      0); */
/* OBSOLETE       wait (0); */
/* OBSOLETE       ptrace (PTRACE_POKEDATA, inferior_pid, (PTRACE_ARG3_TYPE) stack, */
/* OBSOLETE 	      stuff); */
/* OBSOLETE       inferior_registers.r_reg[EAX] = reg; */
/* OBSOLETE     } */
/* OBSOLETE #endif */
/* OBSOLETE   ptrace (PTRACE_SETREGS, inferior_pid, */
/* OBSOLETE 	  (PTRACE_ARG3_TYPE) & inferior_registers); */
/* OBSOLETE   ptrace (PTRACE_SETFPREGS, inferior_pid, */
/* OBSOLETE 	  (PTRACE_ARG3_TYPE) & inferior_fp_registers); */
/* OBSOLETE } */
