/* OBSOLETE /* Native-dependent code for GDB, for NYU Ultra3 running Sym1 OS. */
/* OBSOLETE    Copyright 1988, 1989, 1991, 1992, 1995, 1996, 1999, 2000, 2001 */
/* OBSOLETE    Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by David Wood (wood@nyu.edu) at New York University. */
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
/* OBSOLETE #define DEBUG */
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "inferior.h" */
/* OBSOLETE #include "symtab.h" */
/* OBSOLETE #include "value.h" */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE #include <sys/types.h> */
/* OBSOLETE #include <sys/param.h> */
/* OBSOLETE #include <signal.h> */
/* OBSOLETE #include <sys/ioctl.h> */
/* OBSOLETE #include <fcntl.h> */
/* OBSOLETE  */
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE  */
/* OBSOLETE #include <sys/file.h> */
/* OBSOLETE #include "gdb_stat.h" */
/* OBSOLETE  */
/* OBSOLETE static void fetch_core_registers (char *, unsigned, int, CORE_ADDR); */
/* OBSOLETE  */
/* OBSOLETE /* Assumes support for AMD's Binary Compatibility Standard */
/* OBSOLETE    for ptrace().  If you define ULTRA3, the ultra3 extensions to */
/* OBSOLETE    ptrace() are used allowing the reading of more than one register */
/* OBSOLETE    at a time.  */
/* OBSOLETE  */
/* OBSOLETE    This file assumes KERNEL_DEBUGGING is turned off.  This means */
/* OBSOLETE    that if the user/gdb tries to read gr64-gr95 or any of the  */
/* OBSOLETE    protected special registers we silently return -1 (see the */
/* OBSOLETE    CANNOT_STORE/FETCH_REGISTER macros).  */ */
/* OBSOLETE #define	ULTRA3 */
/* OBSOLETE  */
/* OBSOLETE #if !defined (offsetof) */
/* OBSOLETE #define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER) */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE extern int errno; */
/* OBSOLETE struct ptrace_user pt_struct; */
/* OBSOLETE  */
/* OBSOLETE /* Get all available registers from the inferior.  Registers that are */
/* OBSOLETE  * defined in REGISTER_NAMES, but not available to the user/gdb are */
/* OBSOLETE  * supplied as -1.  This may include gr64-gr95 and the protected special */
/* OBSOLETE  * purpose registers. */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE fetch_inferior_registers (int regno) */
/* OBSOLETE { */
/* OBSOLETE   register int i, j, ret_val = 0; */
/* OBSOLETE   char buf[128]; */
/* OBSOLETE  */
/* OBSOLETE   if (regno != -1) */
/* OBSOLETE     { */
/* OBSOLETE       fetch_register (regno); */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE /* Global Registers */ */
/* OBSOLETE #ifdef ULTRA3 */
/* OBSOLETE   errno = 0; */
/* OBSOLETE   ptrace (PT_READ_STRUCT, PIDGET (inferior_ptid), */
/* OBSOLETE 	  (PTRACE_ARG3_TYPE) register_addr (GR96_REGNUM, 0), */
/* OBSOLETE 	  (int) &pt_struct.pt_gr[0], 32 * 4); */
/* OBSOLETE   if (errno != 0) */
/* OBSOLETE     { */
/* OBSOLETE       perror_with_name ("reading global registers"); */
/* OBSOLETE       ret_val = -1; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     for (regno = GR96_REGNUM, j = 0; j < 32; regno++, j++) */
/* OBSOLETE       { */
/* OBSOLETE 	supply_register (regno, &pt_struct.pt_gr[j]); */
/* OBSOLETE       } */
/* OBSOLETE #else */
/* OBSOLETE   for (regno = GR96_REGNUM; !ret_val && regno < GR96_REGNUM + 32; regno++) */
/* OBSOLETE     fetch_register (regno); */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Local Registers */ */
/* OBSOLETE #ifdef ULTRA3 */
/* OBSOLETE   errno = 0; */
/* OBSOLETE   ptrace (PT_READ_STRUCT, PIDGET (inferior_ptid), */
/* OBSOLETE 	  (PTRACE_ARG3_TYPE) register_addr (LR0_REGNUM, 0), */
/* OBSOLETE 	  (int) &pt_struct.pt_lr[0], 128 * 4); */
/* OBSOLETE   if (errno != 0) */
/* OBSOLETE     { */
/* OBSOLETE       perror_with_name ("reading local registers"); */
/* OBSOLETE       ret_val = -1; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     for (regno = LR0_REGNUM, j = 0; j < 128; regno++, j++) */
/* OBSOLETE       { */
/* OBSOLETE 	supply_register (regno, &pt_struct.pt_lr[j]); */
/* OBSOLETE       } */
/* OBSOLETE #else */
/* OBSOLETE   for (regno = LR0_REGNUM; !ret_val && regno < LR0_REGNUM + 128; regno++) */
/* OBSOLETE     fetch_register (regno); */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Special Registers */ */
/* OBSOLETE   fetch_register (GR1_REGNUM); */
/* OBSOLETE   fetch_register (CPS_REGNUM); */
/* OBSOLETE   fetch_register (PC_REGNUM); */
/* OBSOLETE   fetch_register (NPC_REGNUM); */
/* OBSOLETE   fetch_register (PC2_REGNUM); */
/* OBSOLETE   fetch_register (IPC_REGNUM); */
/* OBSOLETE   fetch_register (IPA_REGNUM); */
/* OBSOLETE   fetch_register (IPB_REGNUM); */
/* OBSOLETE   fetch_register (Q_REGNUM); */
/* OBSOLETE   fetch_register (BP_REGNUM); */
/* OBSOLETE   fetch_register (FC_REGNUM); */
/* OBSOLETE  */
/* OBSOLETE /* Fake any registers that are in REGISTER_NAMES, but not available to gdb */ */
/* OBSOLETE   registers_fetched (); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Store our register values back into the inferior. */
/* OBSOLETE  * If REGNO is -1, do this for all registers. */
/* OBSOLETE  * Otherwise, REGNO specifies which register (so we can save time).   */
/* OBSOLETE  * NOTE: Assumes AMD's binary compatibility standard.  */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE store_inferior_registers (int regno) */
/* OBSOLETE { */
/* OBSOLETE   register unsigned int regaddr; */
/* OBSOLETE   char buf[80]; */
/* OBSOLETE  */
/* OBSOLETE   if (regno >= 0) */
/* OBSOLETE     { */
/* OBSOLETE       if (CANNOT_STORE_REGISTER (regno)) */
/* OBSOLETE 	return; */
/* OBSOLETE       regaddr = register_addr (regno, 0); */
/* OBSOLETE       errno = 0; */
/* OBSOLETE       ptrace (PT_WRITE_U, PIDGET (inferior_ptid), */
/* OBSOLETE 	      (PTRACE_ARG3_TYPE) regaddr, read_register (regno)); */
/* OBSOLETE       if (errno != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  sprintf (buf, "writing register %s (#%d)", REGISTER_NAME (regno), regno); */
/* OBSOLETE 	  perror_with_name (buf); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE #ifdef ULTRA3 */
/* OBSOLETE       pt_struct.pt_gr1 = read_register (GR1_REGNUM); */
/* OBSOLETE       for (regno = GR96_REGNUM; regno < GR96_REGNUM + 32; regno++) */
/* OBSOLETE 	pt_struct.pt_gr[regno] = read_register (regno); */
/* OBSOLETE       for (regno = LR0_REGNUM; regno < LR0_REGNUM + 128; regno++) */
/* OBSOLETE 	pt_struct.pt_gr[regno] = read_register (regno); */
/* OBSOLETE       errno = 0; */
/* OBSOLETE       ptrace (PT_WRITE_STRUCT, PIDGET (inferior_ptid), */
/* OBSOLETE 	      (PTRACE_ARG3_TYPE) register_addr (GR1_REGNUM, 0), */
/* OBSOLETE 	      (int) &pt_struct.pt_gr1, (1 * 32 * 128) * 4); */
/* OBSOLETE       if (errno != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  sprintf (buf, "writing all local/global registers"); */
/* OBSOLETE 	  perror_with_name (buf); */
/* OBSOLETE 	} */
/* OBSOLETE       pt_struct.pt_psr = read_register (CPS_REGNUM); */
/* OBSOLETE       pt_struct.pt_pc0 = read_register (NPC_REGNUM); */
/* OBSOLETE       pt_struct.pt_pc1 = read_register (PC_REGNUM); */
/* OBSOLETE       pt_struct.pt_pc2 = read_register (PC2_REGNUM); */
/* OBSOLETE       pt_struct.pt_ipc = read_register (IPC_REGNUM); */
/* OBSOLETE       pt_struct.pt_ipa = read_register (IPA_REGNUM); */
/* OBSOLETE       pt_struct.pt_ipb = read_register (IPB_REGNUM); */
/* OBSOLETE       pt_struct.pt_q = read_register (Q_REGNUM); */
/* OBSOLETE       pt_struct.pt_bp = read_register (BP_REGNUM); */
/* OBSOLETE       pt_struct.pt_fc = read_register (FC_REGNUM); */
/* OBSOLETE       errno = 0; */
/* OBSOLETE       ptrace (PT_WRITE_STRUCT, PIDGET (inferior_ptid), */
/* OBSOLETE 	      (PTRACE_ARG3_TYPE) register_addr (CPS_REGNUM, 0), */
/* OBSOLETE 	      (int) &pt_struct.pt_psr, (10) * 4); */
/* OBSOLETE       if (errno != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  sprintf (buf, "writing all special registers"); */
/* OBSOLETE 	  perror_with_name (buf); */
/* OBSOLETE 	  return; */
/* OBSOLETE 	} */
/* OBSOLETE #else */
/* OBSOLETE       store_inferior_registers (GR1_REGNUM); */
/* OBSOLETE       for (regno = GR96_REGNUM; regno < GR96_REGNUM + 32; regno++) */
/* OBSOLETE 	store_inferior_registers (regno); */
/* OBSOLETE       for (regno = LR0_REGNUM; regno < LR0_REGNUM + 128; regno++) */
/* OBSOLETE 	store_inferior_registers (regno); */
/* OBSOLETE       store_inferior_registers (CPS_REGNUM); */
/* OBSOLETE       store_inferior_registers (PC_REGNUM); */
/* OBSOLETE       store_inferior_registers (NPC_REGNUM); */
/* OBSOLETE       store_inferior_registers (PC2_REGNUM); */
/* OBSOLETE       store_inferior_registers (IPC_REGNUM); */
/* OBSOLETE       store_inferior_registers (IPA_REGNUM); */
/* OBSOLETE       store_inferior_registers (IPB_REGNUM); */
/* OBSOLETE       store_inferior_registers (Q_REGNUM); */
/* OBSOLETE       store_inferior_registers (BP_REGNUM); */
/* OBSOLETE       store_inferior_registers (FC_REGNUM); */
/* OBSOLETE #endif /* ULTRA3 */ */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /*  */
/* OBSOLETE  * Fetch an individual register (and supply it). */
/* OBSOLETE  * return 0 on success, -1 on failure. */
/* OBSOLETE  * NOTE: Assumes AMD's Binary Compatibility Standard for ptrace(). */
/* OBSOLETE  */ */
/* OBSOLETE static void */
/* OBSOLETE fetch_register (int regno) */
/* OBSOLETE { */
/* OBSOLETE   char buf[128]; */
/* OBSOLETE   int val; */
/* OBSOLETE  */
/* OBSOLETE   if (CANNOT_FETCH_REGISTER (regno)) */
/* OBSOLETE     { */
/* OBSOLETE       val = -1; */
/* OBSOLETE       supply_register (regno, &val); */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       errno = 0; */
/* OBSOLETE       val = ptrace (PT_READ_U, PIDGET (inferior_ptid), */
/* OBSOLETE 		    (PTRACE_ARG3_TYPE) register_addr (regno, 0), 0); */
/* OBSOLETE       if (errno != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  sprintf (buf, "reading register %s (#%d)", REGISTER_NAME (regno), regno); */
/* OBSOLETE 	  perror_with_name (buf); */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  supply_register (regno, &val); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /*  */
/* OBSOLETE  * Read AMD's Binary Compatibilty Standard conforming core file. */
/* OBSOLETE  * struct ptrace_user is the first thing in the core file */
/* OBSOLETE  * */
/* OBSOLETE  * CORE_REG_SECT, CORE_REG_SIZE, WHICH, and REG_ADDR are all ignored. */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE fetch_core_registers (char *core_reg_sect, unsigned core_reg_size, */
/* OBSOLETE 		      int which, CORE_ADDR reg_addr) */
/* OBSOLETE { */
/* OBSOLETE   register int regno; */
/* OBSOLETE   int val; */
/* OBSOLETE   char buf[4]; */
/* OBSOLETE  */
/* OBSOLETE   for (regno = 0; regno < NUM_REGS; regno++) */
/* OBSOLETE     { */
/* OBSOLETE       if (!CANNOT_FETCH_REGISTER (regno)) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  val = bfd_seek (core_bfd, (file_ptr) register_addr (regno, 0), SEEK_SET); */
/* OBSOLETE 	  if (val < 0 || (val = bfd_read (buf, sizeof buf, 1, core_bfd)) < 0) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      char *buffer = (char *) alloca (strlen (REGISTER_NAME (regno)) + 35); */
/* OBSOLETE 	      strcpy (buffer, "Reading core register "); */
/* OBSOLETE 	      strcat (buffer, REGISTER_NAME (regno)); */
/* OBSOLETE 	      perror_with_name (buffer); */
/* OBSOLETE 	    } */
/* OBSOLETE 	  supply_register (regno, buf); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Fake any registers that are in REGISTER_NAMES, but not available to gdb */ */
/* OBSOLETE   registers_fetched (); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /*   */
/* OBSOLETE  * Takes a register number as defined in tm.h via REGISTER_NAMES, and maps */
/* OBSOLETE  * it to an offset in a struct ptrace_user defined by AMD's BCS. */
/* OBSOLETE  * That is, it defines the mapping between gdb register numbers and items in */
/* OBSOLETE  * a struct ptrace_user. */
/* OBSOLETE  * A register protection scheme is set up here.  If a register not */
/* OBSOLETE  * available to the user is specified in 'regno', then an address that */
/* OBSOLETE  * will cause ptrace() to fail is returned. */
/* OBSOLETE  */ */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE register_addr (int regno, CORE_ADDR blockend) */
/* OBSOLETE { */
/* OBSOLETE   if ((regno >= LR0_REGNUM) && (regno < LR0_REGNUM + 128)) */
/* OBSOLETE     { */
/* OBSOLETE       return (offsetof (struct ptrace_user, pt_lr[regno - LR0_REGNUM])); */
/* OBSOLETE     } */
/* OBSOLETE   else if ((regno >= GR96_REGNUM) && (regno < GR96_REGNUM + 32)) */
/* OBSOLETE     { */
/* OBSOLETE       return (offsetof (struct ptrace_user, pt_gr[regno - GR96_REGNUM])); */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       switch (regno) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case GR1_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_gr1)); */
/* OBSOLETE 	case CPS_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_psr)); */
/* OBSOLETE 	case NPC_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_pc0)); */
/* OBSOLETE 	case PC_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_pc1)); */
/* OBSOLETE 	case PC2_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_pc2)); */
/* OBSOLETE 	case IPC_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_ipc)); */
/* OBSOLETE 	case IPA_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_ipa)); */
/* OBSOLETE 	case IPB_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_ipb)); */
/* OBSOLETE 	case Q_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_q)); */
/* OBSOLETE 	case BP_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_bp)); */
/* OBSOLETE 	case FC_REGNUM: */
/* OBSOLETE 	  return (offsetof (struct ptrace_user, pt_fc)); */
/* OBSOLETE 	default: */
/* OBSOLETE 	  fprintf_filtered (gdb_stderr, "register_addr():Bad register %s (%d)\n", */
/* OBSOLETE 			    REGISTER_NAME (regno), regno); */
/* OBSOLETE 	  return (0xffffffff);	/* Should make ptrace() fail */ */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Register that we are able to handle ultra3 core file formats. */
/* OBSOLETE    FIXME: is this really bfd_target_unknown_flavour? */ */
/* OBSOLETE  */
/* OBSOLETE static struct core_fns ultra3_core_fns = */
/* OBSOLETE { */
/* OBSOLETE   bfd_target_unknown_flavour,		/* core_flavour */ */
/* OBSOLETE   default_check_format,			/* check_format */ */
/* OBSOLETE   default_core_sniffer,			/* core_sniffer */ */
/* OBSOLETE   fetch_core_registers,			/* core_read_registers */ */
/* OBSOLETE   NULL					/* next */ */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE _initialize_core_ultra3 (void) */
/* OBSOLETE { */
/* OBSOLETE   add_core_fns (&ultra3_core_fns); */
/* OBSOLETE } */
