// OBSOLETE /* Sequent Symmetry host interface, for GDB when running under Unix.
// OBSOLETE 
// OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 1999,
// OBSOLETE    2000, 2001, 2003 Free Software Foundation, Inc.
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
// OBSOLETE /* FIXME, some 387-specific items of use taken from i387-tdep.c -- ought to be
// OBSOLETE    merged back in. */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "frame.h"
// OBSOLETE #include "inferior.h"
// OBSOLETE #include "symtab.h"
// OBSOLETE #include "target.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE /* FIXME: What is the _INKERNEL define for?  */
// OBSOLETE #define _INKERNEL
// OBSOLETE #include <signal.h>
// OBSOLETE #undef _INKERNEL
// OBSOLETE #include "gdb_wait.h"
// OBSOLETE #include <sys/param.h>
// OBSOLETE #include <sys/user.h>
// OBSOLETE #include <sys/proc.h>
// OBSOLETE #include <sys/dir.h>
// OBSOLETE #include <sys/ioctl.h>
// OBSOLETE #include "gdb_stat.h"
// OBSOLETE #ifdef _SEQUENT_
// OBSOLETE #include <sys/ptrace.h>
// OBSOLETE #else
// OBSOLETE /* Dynix has only machine/ptrace.h, which is already included by sys/user.h  */
// OBSOLETE /* Dynix has no mptrace call */
// OBSOLETE #define mptrace ptrace
// OBSOLETE #endif
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include <fcntl.h>
// OBSOLETE #include <sgtty.h>
// OBSOLETE #define TERMINAL struct sgttyb
// OBSOLETE 
// OBSOLETE #include "gdbcore.h"
// OBSOLETE 
// OBSOLETE void
// OBSOLETE store_inferior_registers (int regno)
// OBSOLETE {
// OBSOLETE   struct pt_regset regs;
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   /* FIXME: Fetching the registers is a kludge to initialize all elements
// OBSOLETE      in the fpu and fpa status. This works for normal debugging, but
// OBSOLETE      might cause problems when calling functions in the inferior.
// OBSOLETE      At least fpu_control and fpa_pcr (probably more) should be added 
// OBSOLETE      to the registers array to solve this properly.  */
// OBSOLETE   mptrace (XPT_RREGS, PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) & regs, 0);
// OBSOLETE 
// OBSOLETE   regs.pr_eax = *(int *) &deprecated_registers[REGISTER_BYTE (0)];
// OBSOLETE   regs.pr_ebx = *(int *) &deprecated_registers[REGISTER_BYTE (5)];
// OBSOLETE   regs.pr_ecx = *(int *) &deprecated_registers[REGISTER_BYTE (2)];
// OBSOLETE   regs.pr_edx = *(int *) &deprecated_registers[REGISTER_BYTE (1)];
// OBSOLETE   regs.pr_esi = *(int *) &deprecated_registers[REGISTER_BYTE (6)];
// OBSOLETE   regs.pr_edi = *(int *) &deprecated_registers[REGISTER_BYTE (7)];
// OBSOLETE   regs.pr_esp = *(int *) &deprecated_registers[REGISTER_BYTE (14)];
// OBSOLETE   regs.pr_ebp = *(int *) &deprecated_registers[REGISTER_BYTE (15)];
// OBSOLETE   regs.pr_eip = *(int *) &deprecated_registers[REGISTER_BYTE (16)];
// OBSOLETE   regs.pr_flags = *(int *) &deprecated_registers[REGISTER_BYTE (17)];
// OBSOLETE   for (i = 0; i < 31; i++)
// OBSOLETE     {
// OBSOLETE       regs.pr_fpa.fpa_regs[i] =
// OBSOLETE 	*(int *) &deprecated_registers[REGISTER_BYTE (FP1_REGNUM + i)];
// OBSOLETE     }
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[0], &deprecated_registers[REGISTER_BYTE (ST0_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[1], &deprecated_registers[REGISTER_BYTE (ST1_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[2], &deprecated_registers[REGISTER_BYTE (ST2_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[3], &deprecated_registers[REGISTER_BYTE (ST3_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[4], &deprecated_registers[REGISTER_BYTE (ST4_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[5], &deprecated_registers[REGISTER_BYTE (ST5_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[6], &deprecated_registers[REGISTER_BYTE (ST6_REGNUM)], 10);
// OBSOLETE   memcpy (regs.pr_fpu.fpu_stack[7], &deprecated_registers[REGISTER_BYTE (ST7_REGNUM)], 10);
// OBSOLETE   mptrace (XPT_WREGS, PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) & regs, 0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE fetch_inferior_registers (int regno)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   struct pt_regset regs;
// OBSOLETE 
// OBSOLETE   deprecated_registers_fetched ();
// OBSOLETE 
// OBSOLETE   mptrace (XPT_RREGS, PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) & regs, 0);
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (EAX_REGNUM)] = regs.pr_eax;
// OBSOLETE   *(int *) &rdeprecated_egisters[REGISTER_BYTE (EBX_REGNUM)] = regs.pr_ebx;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (ECX_REGNUM)] = regs.pr_ecx;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (EDX_REGNUM)] = regs.pr_edx;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (ESI_REGNUM)] = regs.pr_esi;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (EDI_REGNUM)] = regs.pr_edi;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (EBP_REGNUM)] = regs.pr_ebp;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (ESP_REGNUM)] = regs.pr_esp;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (EIP_REGNUM)] = regs.pr_eip;
// OBSOLETE   *(int *) &deprecated_registers[REGISTER_BYTE (EFLAGS_REGNUM)] = regs.pr_flags;
// OBSOLETE   for (i = 0; i < FPA_NREGS; i++)
// OBSOLETE     {
// OBSOLETE       *(int *) &deprecated_registers[REGISTER_BYTE (FP1_REGNUM + i)] =
// OBSOLETE 	regs.pr_fpa.fpa_regs[i];
// OBSOLETE     }
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST0_REGNUM)], regs.pr_fpu.fpu_stack[0], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST1_REGNUM)], regs.pr_fpu.fpu_stack[1], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST2_REGNUM)], regs.pr_fpu.fpu_stack[2], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST3_REGNUM)], regs.pr_fpu.fpu_stack[3], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST4_REGNUM)], regs.pr_fpu.fpu_stack[4], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST5_REGNUM)], regs.pr_fpu.fpu_stack[5], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST6_REGNUM)], regs.pr_fpu.fpu_stack[6], 10);
// OBSOLETE   memcpy (&deprecated_registers[REGISTER_BYTE (ST7_REGNUM)], regs.pr_fpu.fpu_stack[7], 10);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* FIXME:  This should be merged with i387-tdep.c as well. */
// OBSOLETE static
// OBSOLETE print_fpu_status (struct pt_regset ep)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   int bothstatus;
// OBSOLETE   int top;
// OBSOLETE   int fpreg;
// OBSOLETE   unsigned char *p;
// OBSOLETE 
// OBSOLETE   printf_unfiltered ("80387:");
// OBSOLETE   if (ep.pr_fpu.fpu_ip == 0)
// OBSOLETE     {
// OBSOLETE       printf_unfiltered (" not in use.\n");
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       printf_unfiltered ("\n");
// OBSOLETE     }
// OBSOLETE   if (ep.pr_fpu.fpu_status != 0)
// OBSOLETE     {
// OBSOLETE       print_387_status_word (ep.pr_fpu.fpu_status);
// OBSOLETE     }
// OBSOLETE   print_387_control_word (ep.pr_fpu.fpu_control);
// OBSOLETE   printf_unfiltered ("last exception: ");
// OBSOLETE   printf_unfiltered ("opcode 0x%x; ", ep.pr_fpu.fpu_rsvd4);
// OBSOLETE   printf_unfiltered ("pc 0x%x:0x%x; ", ep.pr_fpu.fpu_cs, ep.pr_fpu.fpu_ip);
// OBSOLETE   printf_unfiltered ("operand 0x%x:0x%x\n", ep.pr_fpu.fpu_data_offset, ep.pr_fpu.fpu_op_sel);
// OBSOLETE 
// OBSOLETE   top = (ep.pr_fpu.fpu_status >> 11) & 7;
// OBSOLETE 
// OBSOLETE   printf_unfiltered ("regno  tag  msb              lsb  value\n");
// OBSOLETE   for (fpreg = 7; fpreg >= 0; fpreg--)
// OBSOLETE     {
// OBSOLETE       double val;
// OBSOLETE 
// OBSOLETE       printf_unfiltered ("%s %d: ", fpreg == top ? "=>" : "  ", fpreg);
// OBSOLETE 
// OBSOLETE       switch ((ep.pr_fpu.fpu_tag >> (fpreg * 2)) & 3)
// OBSOLETE 	{
// OBSOLETE 	case 0:
// OBSOLETE 	  printf_unfiltered ("valid ");
// OBSOLETE 	  break;
// OBSOLETE 	case 1:
// OBSOLETE 	  printf_unfiltered ("zero  ");
// OBSOLETE 	  break;
// OBSOLETE 	case 2:
// OBSOLETE 	  printf_unfiltered ("trap  ");
// OBSOLETE 	  break;
// OBSOLETE 	case 3:
// OBSOLETE 	  printf_unfiltered ("empty ");
// OBSOLETE 	  break;
// OBSOLETE 	}
// OBSOLETE       for (i = 9; i >= 0; i--)
// OBSOLETE 	printf_unfiltered ("%02x", ep.pr_fpu.fpu_stack[fpreg][i]);
// OBSOLETE 
// OBSOLETE       i387_to_double ((char *) ep.pr_fpu.fpu_stack[fpreg], (char *) &val);
// OBSOLETE       printf_unfiltered ("  %g\n", val);
// OBSOLETE     }
// OBSOLETE   if (ep.pr_fpu.fpu_rsvd1)
// OBSOLETE     warning ("rsvd1 is 0x%x\n", ep.pr_fpu.fpu_rsvd1);
// OBSOLETE   if (ep.pr_fpu.fpu_rsvd2)
// OBSOLETE     warning ("rsvd2 is 0x%x\n", ep.pr_fpu.fpu_rsvd2);
// OBSOLETE   if (ep.pr_fpu.fpu_rsvd3)
// OBSOLETE     warning ("rsvd3 is 0x%x\n", ep.pr_fpu.fpu_rsvd3);
// OBSOLETE   if (ep.pr_fpu.fpu_rsvd5)
// OBSOLETE     warning ("rsvd5 is 0x%x\n", ep.pr_fpu.fpu_rsvd5);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE print_1167_control_word (unsigned int pcr)
// OBSOLETE {
// OBSOLETE   int pcr_tmp;
// OBSOLETE 
// OBSOLETE   pcr_tmp = pcr & FPA_PCR_MODE;
// OBSOLETE   printf_unfiltered ("\tMODE= %#x; RND= %#x ", pcr_tmp, pcr_tmp & 12);
// OBSOLETE   switch (pcr_tmp & 12)
// OBSOLETE     {
// OBSOLETE     case 0:
// OBSOLETE       printf_unfiltered ("RN (Nearest Value)");
// OBSOLETE       break;
// OBSOLETE     case 1:
// OBSOLETE       printf_unfiltered ("RZ (Zero)");
// OBSOLETE       break;
// OBSOLETE     case 2:
// OBSOLETE       printf_unfiltered ("RP (Positive Infinity)");
// OBSOLETE       break;
// OBSOLETE     case 3:
// OBSOLETE       printf_unfiltered ("RM (Negative Infinity)");
// OBSOLETE       break;
// OBSOLETE     }
// OBSOLETE   printf_unfiltered ("; IRND= %d ", pcr_tmp & 2);
// OBSOLETE   if (0 == pcr_tmp & 2)
// OBSOLETE     {
// OBSOLETE       printf_unfiltered ("(same as RND)\n");
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       printf_unfiltered ("(toward zero)\n");
// OBSOLETE     }
// OBSOLETE   pcr_tmp = pcr & FPA_PCR_EM;
// OBSOLETE   printf_unfiltered ("\tEM= %#x", pcr_tmp);
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_DM)
// OBSOLETE     printf_unfiltered (" DM");
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_UOM)
// OBSOLETE     printf_unfiltered (" UOM");
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_PM)
// OBSOLETE     printf_unfiltered (" PM");
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_UM)
// OBSOLETE     printf_unfiltered (" UM");
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_OM)
// OBSOLETE     printf_unfiltered (" OM");
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_ZM)
// OBSOLETE     printf_unfiltered (" ZM");
// OBSOLETE   if (pcr_tmp & FPA_PCR_EM_IM)
// OBSOLETE     printf_unfiltered (" IM");
// OBSOLETE   printf_unfiltered ("\n");
// OBSOLETE   pcr_tmp = FPA_PCR_CC;
// OBSOLETE   printf_unfiltered ("\tCC= %#x", pcr_tmp);
// OBSOLETE   if (pcr_tmp & FPA_PCR_20MHZ)
// OBSOLETE     printf_unfiltered (" 20MHZ");
// OBSOLETE   if (pcr_tmp & FPA_PCR_CC_Z)
// OBSOLETE     printf_unfiltered (" Z");
// OBSOLETE   if (pcr_tmp & FPA_PCR_CC_C2)
// OBSOLETE     printf_unfiltered (" C2");
// OBSOLETE 
// OBSOLETE   /* Dynix defines FPA_PCR_CC_C0 to 0x100 and ptx defines
// OBSOLETE      FPA_PCR_CC_C1 to 0x100.  Use whichever is defined and assume
// OBSOLETE      the OS knows what it is doing.  */
// OBSOLETE #ifdef FPA_PCR_CC_C1
// OBSOLETE   if (pcr_tmp & FPA_PCR_CC_C1)
// OBSOLETE     printf_unfiltered (" C1");
// OBSOLETE #else
// OBSOLETE   if (pcr_tmp & FPA_PCR_CC_C0)
// OBSOLETE     printf_unfiltered (" C0");
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE   switch (pcr_tmp)
// OBSOLETE     {
// OBSOLETE     case FPA_PCR_CC_Z:
// OBSOLETE       printf_unfiltered (" (Equal)");
// OBSOLETE       break;
// OBSOLETE #ifdef FPA_PCR_CC_C1
// OBSOLETE     case FPA_PCR_CC_C1:
// OBSOLETE #else
// OBSOLETE     case FPA_PCR_CC_C0:
// OBSOLETE #endif
// OBSOLETE       printf_unfiltered (" (Less than)");
// OBSOLETE       break;
// OBSOLETE     case 0:
// OBSOLETE       printf_unfiltered (" (Greater than)");
// OBSOLETE       break;
// OBSOLETE       case FPA_PCR_CC_Z |
// OBSOLETE #ifdef FPA_PCR_CC_C1
// OBSOLETE 	FPA_PCR_CC_C1
// OBSOLETE #else
// OBSOLETE 	FPA_PCR_CC_C0
// OBSOLETE #endif
// OBSOLETE     | FPA_PCR_CC_C2:
// OBSOLETE       printf_unfiltered (" (Unordered)");
// OBSOLETE       break;
// OBSOLETE     default:
// OBSOLETE       printf_unfiltered (" (Undefined)");
// OBSOLETE       break;
// OBSOLETE     }
// OBSOLETE   printf_unfiltered ("\n");
// OBSOLETE   pcr_tmp = pcr & FPA_PCR_AE;
// OBSOLETE   printf_unfiltered ("\tAE= %#x", pcr_tmp);
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_DE)
// OBSOLETE     printf_unfiltered (" DE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_UOE)
// OBSOLETE     printf_unfiltered (" UOE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_PE)
// OBSOLETE     printf_unfiltered (" PE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_UE)
// OBSOLETE     printf_unfiltered (" UE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_OE)
// OBSOLETE     printf_unfiltered (" OE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_ZE)
// OBSOLETE     printf_unfiltered (" ZE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_EE)
// OBSOLETE     printf_unfiltered (" EE");
// OBSOLETE   if (pcr_tmp & FPA_PCR_AE_IE)
// OBSOLETE     printf_unfiltered (" IE");
// OBSOLETE   printf_unfiltered ("\n");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE print_1167_regs (long regs[FPA_NREGS])
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   union
// OBSOLETE     {
// OBSOLETE       double d;
// OBSOLETE       long l[2];
// OBSOLETE     }
// OBSOLETE   xd;
// OBSOLETE   union
// OBSOLETE     {
// OBSOLETE       float f;
// OBSOLETE       long l;
// OBSOLETE     }
// OBSOLETE   xf;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   for (i = 0; i < FPA_NREGS; i++)
// OBSOLETE     {
// OBSOLETE       xf.l = regs[i];
// OBSOLETE       printf_unfiltered ("%%fp%d: raw= %#x, single= %f", i + 1, regs[i], xf.f);
// OBSOLETE       if (!(i & 1))
// OBSOLETE 	{
// OBSOLETE 	  printf_unfiltered ("\n");
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  xd.l[1] = regs[i];
// OBSOLETE 	  xd.l[0] = regs[i + 1];
// OBSOLETE 	  printf_unfiltered (", double= %f\n", xd.d);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE print_fpa_status (struct pt_regset ep)
// OBSOLETE {
// OBSOLETE 
// OBSOLETE   printf_unfiltered ("WTL 1167:");
// OBSOLETE   if (ep.pr_fpa.fpa_pcr != 0)
// OBSOLETE     {
// OBSOLETE       printf_unfiltered ("\n");
// OBSOLETE       print_1167_control_word (ep.pr_fpa.fpa_pcr);
// OBSOLETE       print_1167_regs (ep.pr_fpa.fpa_regs);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       printf_unfiltered (" not in use.\n");
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #if 0				/* disabled because it doesn't go through the target vector.  */
// OBSOLETE i386_float_info (void)
// OBSOLETE {
// OBSOLETE   char ubuf[UPAGES * NBPG];
// OBSOLETE   struct pt_regset regset;
// OBSOLETE 
// OBSOLETE   if (have_inferior_p ())
// OBSOLETE     {
// OBSOLETE       PTRACE_READ_REGS (PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) & regset);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       int corechan = bfd_cache_lookup (core_bfd);
// OBSOLETE       if (lseek (corechan, 0, 0) < 0)
// OBSOLETE 	{
// OBSOLETE 	  perror ("seek on core file");
// OBSOLETE 	}
// OBSOLETE       if (myread (corechan, ubuf, UPAGES * NBPG) < 0)
// OBSOLETE 	{
// OBSOLETE 	  perror ("read on core file");
// OBSOLETE 	}
// OBSOLETE       /* only interested in the floating point registers */
// OBSOLETE       regset.pr_fpu = ((struct user *) ubuf)->u_fpusave;
// OBSOLETE       regset.pr_fpa = ((struct user *) ubuf)->u_fpasave;
// OBSOLETE     }
// OBSOLETE   print_fpu_status (regset);
// OBSOLETE   print_fpa_status (regset);
// OBSOLETE }
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE static volatile int got_sigchld;
// OBSOLETE 
// OBSOLETE /*ARGSUSED */
// OBSOLETE /* This will eventually be more interesting. */
// OBSOLETE void
// OBSOLETE sigchld_handler (int signo)
// OBSOLETE {
// OBSOLETE   got_sigchld++;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Signals for which the default action does not cause the process
// OBSOLETE  * to die.  See <sys/signal.h> for where this came from (alas, we
// OBSOLETE  * can't use those macros directly)
// OBSOLETE  */
// OBSOLETE #ifndef sigmask
// OBSOLETE #define sigmask(s) (1 << ((s) - 1))
// OBSOLETE #endif
// OBSOLETE #define SIGNALS_DFL_SAFE sigmask(SIGSTOP) | sigmask(SIGTSTP) | \
// OBSOLETE 	sigmask(SIGTTIN) | sigmask(SIGTTOU) | sigmask(SIGCHLD) | \
// OBSOLETE 	sigmask(SIGCONT) | sigmask(SIGWINCH) | sigmask(SIGPWR) | \
// OBSOLETE 	sigmask(SIGURG) | sigmask(SIGPOLL)
// OBSOLETE 
// OBSOLETE #ifdef ATTACH_DETACH
// OBSOLETE /*
// OBSOLETE  * Thanks to XPT_MPDEBUGGER, we have to mange child_wait().
// OBSOLETE  */
// OBSOLETE ptid_t
// OBSOLETE child_wait (ptid_t ptid, struct target_waitstatus *status)
// OBSOLETE {
// OBSOLETE   int save_errno, rv, xvaloff, saoff, sa_hand;
// OBSOLETE   struct pt_stop pt;
// OBSOLETE   struct user u;
// OBSOLETE   sigset_t set;
// OBSOLETE   /* Host signal number for a signal which the inferior terminates with, or
// OBSOLETE      0 if it hasn't terminated due to a signal.  */
// OBSOLETE   static int death_by_signal = 0;
// OBSOLETE #ifdef SVR4_SHARED_LIBS		/* use this to distinguish ptx 2 vs ptx 4 */
// OBSOLETE   prstatus_t pstatus;
// OBSOLETE #endif
// OBSOLETE   int pid = PIDGET (ptid);
// OBSOLETE 
// OBSOLETE   do
// OBSOLETE     {
// OBSOLETE       set_sigint_trap ();	/* Causes SIGINT to be passed on to the
// OBSOLETE 				   attached process. */
// OBSOLETE       save_errno = errno;
// OBSOLETE 
// OBSOLETE       got_sigchld = 0;
// OBSOLETE 
// OBSOLETE       sigemptyset (&set);
// OBSOLETE 
// OBSOLETE       while (got_sigchld == 0)
// OBSOLETE 	{
// OBSOLETE 	  sigsuspend (&set);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       clear_sigint_trap ();
// OBSOLETE 
// OBSOLETE       rv = mptrace (XPT_STOPSTAT, 0, (char *) &pt, 0);
// OBSOLETE       if (-1 == rv)
// OBSOLETE 	{
// OBSOLETE 	  printf ("XPT_STOPSTAT: errno %d\n", errno);	/* DEBUG */
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       pid = pt.ps_pid;
// OBSOLETE 
// OBSOLETE       if (pid != PIDGET (inferior_ptid))
// OBSOLETE 	{
// OBSOLETE 	  /* NOTE: the mystery fork in csh/tcsh needs to be ignored.
// OBSOLETE 	   * We should not return new children for the initial run
// OBSOLETE 	   * of a process until it has done the exec.
// OBSOLETE 	   */
// OBSOLETE 	  /* inferior probably forked; send it on its way */
// OBSOLETE 	  rv = mptrace (XPT_UNDEBUG, pid, 0, 0);
// OBSOLETE 	  if (-1 == rv)
// OBSOLETE 	    {
// OBSOLETE 	      printf ("child_wait: XPT_UNDEBUG: pid %d: %s\n", pid,
// OBSOLETE 		      safe_strerror (errno));
// OBSOLETE 	    }
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE       /* FIXME: Do we deal with fork notification correctly?  */
// OBSOLETE       switch (pt.ps_reason)
// OBSOLETE 	{
// OBSOLETE 	case PTS_FORK:
// OBSOLETE 	  /* multi proc: treat like PTS_EXEC */
// OBSOLETE 	  /*
// OBSOLETE 	   * Pretend this didn't happen, since gdb isn't set up
// OBSOLETE 	   * to deal with stops on fork.
// OBSOLETE 	   */
// OBSOLETE 	  rv = ptrace (PT_CONTSIG, pid, 1, 0);
// OBSOLETE 	  if (-1 == rv)
// OBSOLETE 	    {
// OBSOLETE 	      printf ("PTS_FORK: PT_CONTSIG: error %d\n", errno);
// OBSOLETE 	    }
// OBSOLETE 	  continue;
// OBSOLETE 	case PTS_EXEC:
// OBSOLETE 	  /*
// OBSOLETE 	   * Pretend this is a SIGTRAP.
// OBSOLETE 	   */
// OBSOLETE 	  status->kind = TARGET_WAITKIND_STOPPED;
// OBSOLETE 	  status->value.sig = TARGET_SIGNAL_TRAP;
// OBSOLETE 	  break;
// OBSOLETE 	case PTS_EXIT:
// OBSOLETE 	  /*
// OBSOLETE 	   * Note: we stop before the exit actually occurs.  Extract
// OBSOLETE 	   * the exit code from the uarea.  If we're stopped in the
// OBSOLETE 	   * exit() system call, the exit code will be in
// OBSOLETE 	   * u.u_ap[0].  An exit due to an uncaught signal will have
// OBSOLETE 	   * something else in here, see the comment in the default:
// OBSOLETE 	   * case, below.  Finally,let the process exit.
// OBSOLETE 	   */
// OBSOLETE 	  if (death_by_signal)
// OBSOLETE 	    {
// OBSOLETE 	      status->kind = TARGET_WAITKIND_SIGNALED;
// OBSOLETE 	      status->value.sig = target_signal_from_host (death_by_signal);
// OBSOLETE 	      death_by_signal = 0;
// OBSOLETE 	      break;
// OBSOLETE 	    }
// OBSOLETE 	  xvaloff = (unsigned long) &u.u_ap[0] - (unsigned long) &u;
// OBSOLETE 	  errno = 0;
// OBSOLETE 	  rv = ptrace (PT_RUSER, pid, (char *) xvaloff, 0);
// OBSOLETE 	  status->kind = TARGET_WAITKIND_EXITED;
// OBSOLETE 	  status->value.integer = rv;
// OBSOLETE 	  /*
// OBSOLETE 	   * addr & data to mptrace() don't matter here, since
// OBSOLETE 	   * the process is already dead.
// OBSOLETE 	   */
// OBSOLETE 	  rv = mptrace (XPT_UNDEBUG, pid, 0, 0);
// OBSOLETE 	  if (-1 == rv)
// OBSOLETE 	    {
// OBSOLETE 	      printf ("child_wait: PTS_EXIT: XPT_UNDEBUG: pid %d error %d\n", pid,
// OBSOLETE 		      errno);
// OBSOLETE 	    }
// OBSOLETE 	  break;
// OBSOLETE 	case PTS_WATCHPT_HIT:
// OBSOLETE 	  internal_error (__FILE__, __LINE__,
// OBSOLETE 			  "PTS_WATCHPT_HIT\n");
// OBSOLETE 	  break;
// OBSOLETE 	default:
// OBSOLETE 	  /* stopped by signal */
// OBSOLETE 	  status->kind = TARGET_WAITKIND_STOPPED;
// OBSOLETE 	  status->value.sig = target_signal_from_host (pt.ps_reason);
// OBSOLETE 	  death_by_signal = 0;
// OBSOLETE 
// OBSOLETE 	  if (0 == (SIGNALS_DFL_SAFE & sigmask (pt.ps_reason)))
// OBSOLETE 	    {
// OBSOLETE 	      break;
// OBSOLETE 	    }
// OBSOLETE 	  /* else default action of signal is to die */
// OBSOLETE #ifdef SVR4_SHARED_LIBS
// OBSOLETE 	  rv = ptrace (PT_GET_PRSTATUS, pid, (char *) &pstatus, 0);
// OBSOLETE 	  if (-1 == rv)
// OBSOLETE 	    error ("child_wait: signal %d PT_GET_PRSTATUS: %s\n",
// OBSOLETE 		   pt.ps_reason, safe_strerror (errno));
// OBSOLETE 	  if (pstatus.pr_cursig != pt.ps_reason)
// OBSOLETE 	    {
// OBSOLETE 	      printf ("pstatus signal %d, pt signal %d\n",
// OBSOLETE 		      pstatus.pr_cursig, pt.ps_reason);
// OBSOLETE 	    }
// OBSOLETE 	  sa_hand = (int) pstatus.pr_action.sa_handler;
// OBSOLETE #else
// OBSOLETE 	  saoff = (unsigned long) &u.u_sa[0] - (unsigned long) &u;
// OBSOLETE 	  saoff += sizeof (struct sigaction) * (pt.ps_reason - 1);
// OBSOLETE 	  errno = 0;
// OBSOLETE 	  sa_hand = ptrace (PT_RUSER, pid, (char *) saoff, 0);
// OBSOLETE 	  if (errno)
// OBSOLETE 	    error ("child_wait: signal %d: RUSER: %s\n",
// OBSOLETE 		   pt.ps_reason, safe_strerror (errno));
// OBSOLETE #endif
// OBSOLETE 	  if ((int) SIG_DFL == sa_hand)
// OBSOLETE 	    {
// OBSOLETE 	      /* we will be dying */
// OBSOLETE 	      death_by_signal = pt.ps_reason;
// OBSOLETE 	    }
// OBSOLETE 	  break;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE     }
// OBSOLETE   while (pid != PIDGET (inferior_ptid));	/* Some other child died or stopped */
// OBSOLETE 
// OBSOLETE   return pid_to_ptid (pid);
// OBSOLETE }
// OBSOLETE #else /* !ATTACH_DETACH */
// OBSOLETE /*
// OBSOLETE  * Simple child_wait() based on inftarg.c child_wait() for use until
// OBSOLETE  * the MPDEBUGGER child_wait() works properly.  This will go away when
// OBSOLETE  * that is fixed.
// OBSOLETE  */
// OBSOLETE ptid_t
// OBSOLETE child_wait (ptid_t ptid, struct target_waitstatus *ourstatus)
// OBSOLETE {
// OBSOLETE   int save_errno;
// OBSOLETE   int status;
// OBSOLETE   int pid = PIDGET (ptid);
// OBSOLETE 
// OBSOLETE   do
// OBSOLETE     {
// OBSOLETE       pid = wait (&status);
// OBSOLETE       save_errno = errno;
// OBSOLETE 
// OBSOLETE       if (pid == -1)
// OBSOLETE 	{
// OBSOLETE 	  if (save_errno == EINTR)
// OBSOLETE 	    continue;
// OBSOLETE 	  fprintf (stderr, "Child process unexpectedly missing: %s.\n",
// OBSOLETE 		   safe_strerror (save_errno));
// OBSOLETE 	  ourstatus->kind = TARGET_WAITKIND_SIGNALLED;
// OBSOLETE 	  ourstatus->value.sig = TARGET_SIGNAL_UNKNOWN;
// OBSOLETE 	  return pid_to_ptid (-1);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   while (pid != PIDGET (inferior_ptid));	/* Some other child died or stopped */
// OBSOLETE   store_waitstatus (ourstatus, status);
// OBSOLETE   return pid_to_ptid (pid);
// OBSOLETE }
// OBSOLETE #endif /* ATTACH_DETACH */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* This function simply calls ptrace with the given arguments.  
// OBSOLETE    It exists so that all calls to ptrace are isolated in this 
// OBSOLETE    machine-dependent file. */
// OBSOLETE int
// OBSOLETE call_ptrace (int request, int pid, PTRACE_ARG3_TYPE addr, int data)
// OBSOLETE {
// OBSOLETE   return ptrace (request, pid, addr, data);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE call_mptrace (int request, int pid, PTRACE_ARG3_TYPE addr, int data)
// OBSOLETE {
// OBSOLETE   return mptrace (request, pid, addr, data);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #if defined (DEBUG_PTRACE)
// OBSOLETE /* For the rest of the file, use an extra level of indirection */
// OBSOLETE /* This lets us breakpoint usefully on call_ptrace. */
// OBSOLETE #define ptrace call_ptrace
// OBSOLETE #define mptrace call_mptrace
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE void
// OBSOLETE kill_inferior (void)
// OBSOLETE {
// OBSOLETE   if (ptid_equal (inferior_ptid, null_ptid))
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   /* For MPDEBUGGER, don't use PT_KILL, since the child will stop
// OBSOLETE      again with a PTS_EXIT.  Just hit him with SIGKILL (so he stops)
// OBSOLETE      and detach. */
// OBSOLETE 
// OBSOLETE   kill (PIDGET (inferior_ptid), SIGKILL);
// OBSOLETE #ifdef ATTACH_DETACH
// OBSOLETE   detach (SIGKILL);
// OBSOLETE #else /* ATTACH_DETACH */
// OBSOLETE   ptrace (PT_KILL, PIDGET (inferior_ptid), 0, 0);
// OBSOLETE   wait ((int *) NULL);
// OBSOLETE #endif /* ATTACH_DETACH */
// OBSOLETE   target_mourn_inferior ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Resume execution of the inferior process.
// OBSOLETE    If STEP is nonzero, single-step it.
// OBSOLETE    If SIGNAL is nonzero, give it that signal.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE child_resume (ptid_t ptid, int step, enum target_signal signal)
// OBSOLETE {
// OBSOLETE   int pid = PIDGET (ptid);
// OBSOLETE 
// OBSOLETE   errno = 0;
// OBSOLETE 
// OBSOLETE   if (pid == -1)
// OBSOLETE     pid = PIDGET (inferior_ptid);
// OBSOLETE 
// OBSOLETE   /* An address of (PTRACE_ARG3_TYPE)1 tells ptrace to continue from where
// OBSOLETE      it was.  (If GDB wanted it to start some other way, we have already
// OBSOLETE      written a new PC value to the child.)
// OBSOLETE 
// OBSOLETE      If this system does not support PT_SSTEP, a higher level function will
// OBSOLETE      have called single_step() to transmute the step request into a
// OBSOLETE      continue request (by setting breakpoints on all possible successor
// OBSOLETE      instructions), so we don't have to worry about that here.  */
// OBSOLETE 
// OBSOLETE   if (step)
// OBSOLETE     ptrace (PT_SSTEP, pid, (PTRACE_ARG3_TYPE) 1, signal);
// OBSOLETE   else
// OBSOLETE     ptrace (PT_CONTSIG, pid, (PTRACE_ARG3_TYPE) 1, signal);
// OBSOLETE 
// OBSOLETE   if (errno)
// OBSOLETE     perror_with_name ("ptrace");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef ATTACH_DETACH
// OBSOLETE /* Start debugging the process whose number is PID.  */
// OBSOLETE int
// OBSOLETE attach (int pid)
// OBSOLETE {
// OBSOLETE   sigset_t set;
// OBSOLETE   int rv;
// OBSOLETE 
// OBSOLETE   rv = mptrace (XPT_DEBUG, pid, 0, 0);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       error ("mptrace(XPT_DEBUG): %s", safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE   rv = mptrace (XPT_SIGNAL, pid, 0, SIGSTOP);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       error ("mptrace(XPT_SIGNAL): %s", safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE   attach_flag = 1;
// OBSOLETE   return pid;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE detach (int signo)
// OBSOLETE {
// OBSOLETE   int rv;
// OBSOLETE 
// OBSOLETE   rv = mptrace (XPT_UNDEBUG, PIDGET (inferior_ptid), 1, signo);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       error ("mptrace(XPT_UNDEBUG): %s", safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE   attach_flag = 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #endif /* ATTACH_DETACH */
// OBSOLETE 
// OBSOLETE /* Default the type of the ptrace transfer to int.  */
// OBSOLETE #ifndef PTRACE_XFER_TYPE
// OBSOLETE #define PTRACE_XFER_TYPE int
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* NOTE! I tried using PTRACE_READDATA, etc., to read and write memory
// OBSOLETE    in the NEW_SUN_PTRACE case.
// OBSOLETE    It ought to be straightforward.  But it appears that writing did
// OBSOLETE    not write the data that I specified.  I cannot understand where
// OBSOLETE    it got the data that it actually did write.  */
// OBSOLETE 
// OBSOLETE /* Copy LEN bytes to or from inferior's memory starting at MEMADDR
// OBSOLETE    to debugger memory starting at MYADDR.   Copy to inferior if
// OBSOLETE    WRITE is nonzero.  TARGET is ignored.
// OBSOLETE 
// OBSOLETE    Returns the length copied, which is either the LEN argument or zero.
// OBSOLETE    This xfer function does not do partial moves, since child_ops
// OBSOLETE    doesn't allow memory operations to cross below us in the target stack
// OBSOLETE    anyway.  */
// OBSOLETE 
// OBSOLETE int
// OBSOLETE child_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
// OBSOLETE 		   struct mem_attrib *attrib,
// OBSOLETE 		   struct target_ops *target)
// OBSOLETE {
// OBSOLETE   register int i;
// OBSOLETE   /* Round starting address down to longword boundary.  */
// OBSOLETE   register CORE_ADDR addr = memaddr & -(CORE_ADDR) sizeof (PTRACE_XFER_TYPE);
// OBSOLETE   /* Round ending address up; get number of longwords that makes.  */
// OBSOLETE   register int count
// OBSOLETE   = (((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1)
// OBSOLETE   / sizeof (PTRACE_XFER_TYPE);
// OBSOLETE   /* Allocate buffer of that many longwords.  */
// OBSOLETE   /* FIXME (alloca): This code, cloned from infptrace.c, is unsafe
// OBSOLETE      because it uses alloca to allocate a buffer of arbitrary size.
// OBSOLETE      For very large xfers, this could crash GDB's stack.  */
// OBSOLETE   register PTRACE_XFER_TYPE *buffer
// OBSOLETE     = (PTRACE_XFER_TYPE *) alloca (count * sizeof (PTRACE_XFER_TYPE));
// OBSOLETE 
// OBSOLETE   if (write)
// OBSOLETE     {
// OBSOLETE       /* Fill start and end extra bytes of buffer with existing memory data.  */
// OBSOLETE 
// OBSOLETE       if (addr != memaddr || len < (int) sizeof (PTRACE_XFER_TYPE))
// OBSOLETE 	{
// OBSOLETE 	  /* Need part of initial word -- fetch it.  */
// OBSOLETE 	  buffer[0] = ptrace (PT_RTEXT, PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) addr,
// OBSOLETE 			      0);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       if (count > 1)		/* FIXME, avoid if even boundary */
// OBSOLETE 	{
// OBSOLETE 	  buffer[count - 1]
// OBSOLETE 	    = ptrace (PT_RTEXT, PIDGET (inferior_ptid),
// OBSOLETE 		      ((PTRACE_ARG3_TYPE)
// OBSOLETE 		       (addr + (count - 1) * sizeof (PTRACE_XFER_TYPE))),
// OBSOLETE 		      0);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Copy data to be written over corresponding part of buffer */
// OBSOLETE 
// OBSOLETE       memcpy ((char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
// OBSOLETE 	      myaddr,
// OBSOLETE 	      len);
// OBSOLETE 
// OBSOLETE       /* Write the entire buffer.  */
// OBSOLETE 
// OBSOLETE       for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
// OBSOLETE 	{
// OBSOLETE 	  errno = 0;
// OBSOLETE 	  ptrace (PT_WDATA, PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) addr,
// OBSOLETE 		  buffer[i]);
// OBSOLETE 	  if (errno)
// OBSOLETE 	    {
// OBSOLETE 	      /* Using the appropriate one (I or D) is necessary for
// OBSOLETE 	         Gould NP1, at least.  */
// OBSOLETE 	      errno = 0;
// OBSOLETE 	      ptrace (PT_WTEXT, PIDGET (inferior_ptid), (PTRACE_ARG3_TYPE) addr,
// OBSOLETE 		      buffer[i]);
// OBSOLETE 	    }
// OBSOLETE 	  if (errno)
// OBSOLETE 	    return 0;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       /* Read all the longwords */
// OBSOLETE       for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
// OBSOLETE 	{
// OBSOLETE 	  errno = 0;
// OBSOLETE 	  buffer[i] = ptrace (PT_RTEXT, PIDGET (inferior_ptid),
// OBSOLETE 			      (PTRACE_ARG3_TYPE) addr, 0);
// OBSOLETE 	  if (errno)
// OBSOLETE 	    return 0;
// OBSOLETE 	  QUIT;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Copy appropriate bytes out of the buffer.  */
// OBSOLETE       memcpy (myaddr,
// OBSOLETE 	      (char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
// OBSOLETE 	      len);
// OBSOLETE     }
// OBSOLETE   return len;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_symm_nat (void)
// OBSOLETE {
// OBSOLETE #ifdef ATTACH_DETACH
// OBSOLETE /*
// OBSOLETE  * the MPDEBUGGER is necessary for process tree debugging and attach
// OBSOLETE  * to work, but it alters the behavior of debugged processes, so other
// OBSOLETE  * things (at least child_wait()) will have to change to accomodate
// OBSOLETE  * that.
// OBSOLETE  *
// OBSOLETE  * Note that attach is not implemented in dynix 3, and not in ptx
// OBSOLETE  * until version 2.1 of the OS.
// OBSOLETE  */
// OBSOLETE   int rv;
// OBSOLETE   sigset_t set;
// OBSOLETE   struct sigaction sact;
// OBSOLETE 
// OBSOLETE   rv = mptrace (XPT_MPDEBUGGER, 0, 0, 0);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       internal_error (__FILE__, __LINE__,
// OBSOLETE 		      "_initialize_symm_nat(): mptrace(XPT_MPDEBUGGER): %s",
// OBSOLETE 		      safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /*
// OBSOLETE    * Under MPDEBUGGER, we get SIGCLHD when a traced process does
// OBSOLETE    * anything of interest.
// OBSOLETE    */
// OBSOLETE 
// OBSOLETE   /*
// OBSOLETE    * Block SIGCHLD.  We leave it blocked all the time, and then
// OBSOLETE    * call sigsuspend() in child_wait() to wait for the child
// OBSOLETE    * to do something.  None of these ought to fail, but check anyway.
// OBSOLETE    */
// OBSOLETE   sigemptyset (&set);
// OBSOLETE   rv = sigaddset (&set, SIGCHLD);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       internal_error (__FILE__, __LINE__,
// OBSOLETE 		      "_initialize_symm_nat(): sigaddset(SIGCHLD): %s",
// OBSOLETE 		      safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE   rv = sigprocmask (SIG_BLOCK, &set, (sigset_t *) NULL);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       internal_error (__FILE__, __LINE__,
// OBSOLETE 		      "_initialize_symm_nat(): sigprocmask(SIG_BLOCK): %s",
// OBSOLETE 		      safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   sact.sa_handler = sigchld_handler;
// OBSOLETE   sigemptyset (&sact.sa_mask);
// OBSOLETE   sact.sa_flags = SA_NOCLDWAIT;	/* keep the zombies away */
// OBSOLETE   rv = sigaction (SIGCHLD, &sact, (struct sigaction *) NULL);
// OBSOLETE   if (-1 == rv)
// OBSOLETE     {
// OBSOLETE       internal_error (__FILE__, __LINE__,
// OBSOLETE 		      "_initialize_symm_nat(): sigaction(SIGCHLD): %s",
// OBSOLETE 		      safe_strerror (errno));
// OBSOLETE     }
// OBSOLETE #endif
// OBSOLETE }
