// OBSOLETE /* Remote debugging interface to m32r and mon2000 ROM monitors for GDB, 
// OBSOLETE    the GNU debugger.
// OBSOLETE    Copyright 1996, 1997, 1998, 1999, 2000, 2001
// OBSOLETE    Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    Adapted by Michael Snyder of Cygnus Support.
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
// OBSOLETE /* This module defines communication with the Mitsubishi m32r monitor */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "target.h"
// OBSOLETE #include "monitor.h"
// OBSOLETE #include "serial.h"
// OBSOLETE #include "symtab.h"
// OBSOLETE #include "command.h"
// OBSOLETE #include "gdbcmd.h"
// OBSOLETE #include "symfile.h"		/* for generic load */
// OBSOLETE #include <time.h>		/* for time_t */
// OBSOLETE #include "gdb_string.h"
// OBSOLETE #include "objfiles.h"		/* for ALL_OBJFILES etc. */
// OBSOLETE #include "inferior.h"		/* for write_pc() */
// OBSOLETE #include <ctype.h>
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE extern void report_transfer_performance (unsigned long, time_t, time_t);
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * All this stuff just to get my host computer's IP address!
// OBSOLETE  */
// OBSOLETE #include <sys/types.h>
// OBSOLETE #include <netdb.h>		/* for hostent */
// OBSOLETE #include <netinet/in.h>		/* for struct in_addr */
// OBSOLETE #if 1
// OBSOLETE #include <arpa/inet.h>		/* for inet_ntoa */
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE static char *board_addr;	/* user-settable IP address for M32R-EVA */
// OBSOLETE static char *server_addr;	/* user-settable IP address for gdb host */
// OBSOLETE static char *download_path;	/* user-settable path for SREC files     */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* 
// OBSOLETE  * Function: m32r_load_1 (helper function)
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_load_section (bfd *abfd, asection *s, void *obj)
// OBSOLETE {
// OBSOLETE   unsigned int *data_count = obj;
// OBSOLETE   if (s->flags & SEC_LOAD)
// OBSOLETE     {
// OBSOLETE       bfd_size_type section_size = bfd_section_size (abfd, s);
// OBSOLETE       bfd_vma section_base = bfd_section_lma (abfd, s);
// OBSOLETE       unsigned int buffer, i;
// OBSOLETE 
// OBSOLETE       *data_count += section_size;
// OBSOLETE 
// OBSOLETE       printf_filtered ("Loading section %s, size 0x%lx lma ",
// OBSOLETE 		       bfd_section_name (abfd, s), section_size);
// OBSOLETE       print_address_numeric (section_base, 1, gdb_stdout);
// OBSOLETE       printf_filtered ("\n");
// OBSOLETE       gdb_flush (gdb_stdout);
// OBSOLETE       monitor_printf ("%s mw\r", paddr_nz (section_base));
// OBSOLETE       for (i = 0; i < section_size; i += 4)
// OBSOLETE 	{
// OBSOLETE 	  QUIT;
// OBSOLETE 	  monitor_expect (" -> ", NULL, 0);
// OBSOLETE 	  bfd_get_section_contents (abfd, s, (char *) &buffer, i, 4);
// OBSOLETE 	  monitor_printf ("%x\n", buffer);
// OBSOLETE 	}
// OBSOLETE       monitor_expect (" -> ", NULL, 0);
// OBSOLETE       monitor_printf ("q\n");
// OBSOLETE       monitor_expect_prompt (NULL, 0);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE m32r_load_1 (void *dummy)
// OBSOLETE {
// OBSOLETE   int data_count = 0;
// OBSOLETE 
// OBSOLETE   bfd_map_over_sections ((bfd *) dummy, m32r_load_section, &data_count);
// OBSOLETE   return data_count;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* 
// OBSOLETE  * Function: m32r_load (an alternate way to load) 
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_load (char *filename, int from_tty)
// OBSOLETE {
// OBSOLETE   bfd *abfd;
// OBSOLETE   asection *s;
// OBSOLETE   unsigned int i, data_count = 0;
// OBSOLETE   time_t start_time, end_time;	/* for timing of download */
// OBSOLETE 
// OBSOLETE   if (filename == NULL || filename[0] == 0)
// OBSOLETE     filename = get_exec_file (1);
// OBSOLETE 
// OBSOLETE   abfd = bfd_openr (filename, 0);
// OBSOLETE   if (!abfd)
// OBSOLETE     error ("Unable to open file %s\n", filename);
// OBSOLETE   if (bfd_check_format (abfd, bfd_object) == 0)
// OBSOLETE     error ("File is not an object file\n");
// OBSOLETE   start_time = time (NULL);
// OBSOLETE #if 0
// OBSOLETE   for (s = abfd->sections; s; s = s->next)
// OBSOLETE     if (s->flags & SEC_LOAD)
// OBSOLETE       {
// OBSOLETE 	bfd_size_type section_size = bfd_section_size (abfd, s);
// OBSOLETE 	bfd_vma section_base = bfd_section_vma (abfd, s);
// OBSOLETE 	unsigned int buffer;
// OBSOLETE 
// OBSOLETE 	data_count += section_size;
// OBSOLETE 
// OBSOLETE 	printf_filtered ("Loading section %s, size 0x%lx vma ",
// OBSOLETE 			 bfd_section_name (abfd, s), section_size);
// OBSOLETE 	print_address_numeric (section_base, 1, gdb_stdout);
// OBSOLETE 	printf_filtered ("\n");
// OBSOLETE 	gdb_flush (gdb_stdout);
// OBSOLETE 	monitor_printf ("%x mw\r", section_base);
// OBSOLETE 	for (i = 0; i < section_size; i += 4)
// OBSOLETE 	  {
// OBSOLETE 	    monitor_expect (" -> ", NULL, 0);
// OBSOLETE 	    bfd_get_section_contents (abfd, s, (char *) &buffer, i, 4);
// OBSOLETE 	    monitor_printf ("%x\n", buffer);
// OBSOLETE 	  }
// OBSOLETE 	monitor_expect (" -> ", NULL, 0);
// OBSOLETE 	monitor_printf ("q\n");
// OBSOLETE 	monitor_expect_prompt (NULL, 0);
// OBSOLETE       }
// OBSOLETE #else
// OBSOLETE   if (!(catch_errors (m32r_load_1, abfd, "Load aborted!\n", RETURN_MASK_ALL)))
// OBSOLETE     {
// OBSOLETE       monitor_printf ("q\n");
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE #endif
// OBSOLETE   end_time = time (NULL);
// OBSOLETE   printf_filtered ("Start address 0x%lx\n", bfd_get_start_address (abfd));
// OBSOLETE   report_transfer_performance (data_count, start_time, end_time);
// OBSOLETE 
// OBSOLETE   /* Finally, make the PC point at the start address */
// OBSOLETE   if (exec_bfd)
// OBSOLETE     write_pc (bfd_get_start_address (exec_bfd));
// OBSOLETE 
// OBSOLETE   inferior_ptid = null_ptid;	/* No process now */
// OBSOLETE 
// OBSOLETE   /* This is necessary because many things were based on the PC at the
// OBSOLETE      time that we attached to the monitor, which is no longer valid
// OBSOLETE      now that we have loaded new code (and just changed the PC).
// OBSOLETE      Another way to do this might be to call normal_stop, except that
// OBSOLETE      the stack may not be valid, and things would get horribly
// OBSOLETE      confused... */
// OBSOLETE 
// OBSOLETE   clear_symtab_users ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_load_gen (char *filename, int from_tty)
// OBSOLETE {
// OBSOLETE   generic_load (filename, from_tty);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void m32r_open (char *args, int from_tty);
// OBSOLETE static void mon2000_open (char *args, int from_tty);
// OBSOLETE 
// OBSOLETE /* This array of registers needs to match the indexes used by GDB. The
// OBSOLETE    whole reason this exists is because the various ROM monitors use
// OBSOLETE    different names than GDB does, and don't support all the registers
// OBSOLETE    either. So, typing "info reg sp" becomes an "A7". */
// OBSOLETE 
// OBSOLETE static char *m32r_regnames[] =
// OBSOLETE {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
// OBSOLETE  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
// OBSOLETE  "psw", "cbr", "spi", "spu", "bpc", "pc", "accl", "acch",
// OBSOLETE };
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_supply_register (char *regname, int regnamelen, char *val, int vallen)
// OBSOLETE {
// OBSOLETE   int regno;
// OBSOLETE   int num_regs = sizeof (m32r_regnames) / sizeof (m32r_regnames[0]);
// OBSOLETE 
// OBSOLETE   for (regno = 0; regno < num_regs; regno++)
// OBSOLETE     if (strncmp (regname, m32r_regnames[regno], regnamelen) == 0)
// OBSOLETE       break;
// OBSOLETE 
// OBSOLETE   if (regno >= num_regs)
// OBSOLETE     return;			/* no match */
// OBSOLETE 
// OBSOLETE   if (regno == ACCL_REGNUM)
// OBSOLETE     {				/* special handling for 64-bit acc reg */
// OBSOLETE       monitor_supply_register (ACCH_REGNUM, val);
// OBSOLETE       val = strchr (val, ':');	/* skip past ':' to get 2nd word */
// OBSOLETE       if (val != NULL)
// OBSOLETE 	monitor_supply_register (ACCL_REGNUM, val + 1);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       monitor_supply_register (regno, val);
// OBSOLETE       if (regno == PSW_REGNUM)
// OBSOLETE 	{
// OBSOLETE 	  unsigned long psw = strtoul (val, NULL, 16);
// OBSOLETE 	  char *zero = "00000000", *one = "00000001";
// OBSOLETE 
// OBSOLETE #ifdef SM_REGNUM
// OBSOLETE 	  /* Stack mode bit */
// OBSOLETE 	  monitor_supply_register (SM_REGNUM, (psw & 0x80) ? one : zero);
// OBSOLETE #endif
// OBSOLETE #ifdef BSM_REGNUM
// OBSOLETE 	  /* Backup stack mode bit */
// OBSOLETE 	  monitor_supply_register (BSM_REGNUM, (psw & 0x8000) ? one : zero);
// OBSOLETE #endif
// OBSOLETE #ifdef IE_REGNUM
// OBSOLETE 	  /* Interrupt enable bit */
// OBSOLETE 	  monitor_supply_register (IE_REGNUM, (psw & 0x40) ? one : zero);
// OBSOLETE #endif
// OBSOLETE #ifdef BIE_REGNUM
// OBSOLETE 	  /* Backup interrupt enable bit */
// OBSOLETE 	  monitor_supply_register (BIE_REGNUM, (psw & 0x4000) ? one : zero);
// OBSOLETE #endif
// OBSOLETE #ifdef COND_REGNUM
// OBSOLETE 	  /* Condition bit (carry etc.) */
// OBSOLETE 	  monitor_supply_register (COND_REGNUM, (psw & 0x1) ? one : zero);
// OBSOLETE #endif
// OBSOLETE #ifdef CBR_REGNUM
// OBSOLETE 	  monitor_supply_register (CBR_REGNUM, (psw & 0x1) ? one : zero);
// OBSOLETE #endif
// OBSOLETE #ifdef BPC_REGNUM
// OBSOLETE 	  monitor_supply_register (BPC_REGNUM, zero);	/* KLUDGE:   (???????) */
// OBSOLETE #endif
// OBSOLETE #ifdef BCARRY_REGNUM
// OBSOLETE 	  monitor_supply_register (BCARRY_REGNUM, zero);	/* KLUDGE: (??????) */
// OBSOLETE #endif
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       if (regno == SPI_REGNUM || regno == SPU_REGNUM)
// OBSOLETE 	{			/* special handling for stack pointer (spu or spi) */
// OBSOLETE 	  unsigned long stackmode = read_register (PSW_REGNUM) & 0x80;
// OBSOLETE 
// OBSOLETE 	  if (regno == SPI_REGNUM && !stackmode)	/* SP == SPI */
// OBSOLETE 	    monitor_supply_register (SP_REGNUM, val);
// OBSOLETE 	  else if (regno == SPU_REGNUM && stackmode)	/* SP == SPU */
// OBSOLETE 	    monitor_supply_register (SP_REGNUM, val);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* m32r RevC board monitor */
// OBSOLETE 
// OBSOLETE static struct target_ops m32r_ops;
// OBSOLETE 
// OBSOLETE static char *m32r_inits[] =
// OBSOLETE {"\r", NULL};
// OBSOLETE 
// OBSOLETE static struct monitor_ops m32r_cmds;
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE init_m32r_cmds (void)
// OBSOLETE {
// OBSOLETE   m32r_cmds.flags = MO_CLR_BREAK_USES_ADDR | MO_REGISTER_VALUE_FIRST;
// OBSOLETE   m32r_cmds.init = m32r_inits;	/* Init strings */
// OBSOLETE   m32r_cmds.cont = "go\r";	/* continue command */
// OBSOLETE   m32r_cmds.step = "step\r";	/* single step */
// OBSOLETE   m32r_cmds.stop = NULL;	/* interrupt command */
// OBSOLETE   m32r_cmds.set_break = "%x +bp\r";	/* set a breakpoint */
// OBSOLETE   m32r_cmds.clr_break = "%x -bp\r";	/* clear a breakpoint */
// OBSOLETE   m32r_cmds.clr_all_break = "bpoff\r";	/* clear all breakpoints */
// OBSOLETE   m32r_cmds.fill = "%x %x %x fill\r";	/* fill (start length val) */
// OBSOLETE   m32r_cmds.setmem.cmdb = "%x 1 %x fill\r";	/* setmem.cmdb (addr, value) */
// OBSOLETE   m32r_cmds.setmem.cmdw = "%x 1 %x fillh\r";	/* setmem.cmdw (addr, value) */
// OBSOLETE   m32r_cmds.setmem.cmdl = "%x 1 %x fillw\r";	/* setmem.cmdl (addr, value) */
// OBSOLETE   m32r_cmds.setmem.cmdll = NULL;	/* setmem.cmdll (addr, value) */
// OBSOLETE   m32r_cmds.setmem.resp_delim = NULL;	/* setmem.resp_delim */
// OBSOLETE   m32r_cmds.setmem.term = NULL;	/* setmem.term */
// OBSOLETE   m32r_cmds.setmem.term_cmd = NULL;	/* setmem.term_cmd */
// OBSOLETE   m32r_cmds.getmem.cmdb = "%x %x dump\r";	/* getmem.cmdb (addr, len) */
// OBSOLETE   m32r_cmds.getmem.cmdw = NULL;	/* getmem.cmdw (addr, len) */
// OBSOLETE   m32r_cmds.getmem.cmdl = NULL;	/* getmem.cmdl (addr, len) */
// OBSOLETE   m32r_cmds.getmem.cmdll = NULL;	/* getmem.cmdll (addr, len) */
// OBSOLETE   m32r_cmds.getmem.resp_delim = ": ";	/* getmem.resp_delim */
// OBSOLETE   m32r_cmds.getmem.term = NULL;	/* getmem.term */
// OBSOLETE   m32r_cmds.getmem.term_cmd = NULL;	/* getmem.term_cmd */
// OBSOLETE   m32r_cmds.setreg.cmd = "%x to %%%s\r";	/* setreg.cmd (name, value) */
// OBSOLETE   m32r_cmds.setreg.resp_delim = NULL;	/* setreg.resp_delim */
// OBSOLETE   m32r_cmds.setreg.term = NULL;	/* setreg.term */
// OBSOLETE   m32r_cmds.setreg.term_cmd = NULL;	/* setreg.term_cmd */
// OBSOLETE   m32r_cmds.getreg.cmd = NULL;	/* getreg.cmd (name) */
// OBSOLETE   m32r_cmds.getreg.resp_delim = NULL;	/* getreg.resp_delim */
// OBSOLETE   m32r_cmds.getreg.term = NULL;	/* getreg.term */
// OBSOLETE   m32r_cmds.getreg.term_cmd = NULL;	/* getreg.term_cmd */
// OBSOLETE   m32r_cmds.dump_registers = ".reg\r";	/* dump_registers */
// OBSOLETE   m32r_cmds.register_pattern = "\\(\\w+\\) += \\([0-9a-fA-F]+\\b\\)";	/* register_pattern */
// OBSOLETE   m32r_cmds.supply_register = m32r_supply_register;	/* supply_register */
// OBSOLETE   m32r_cmds.load_routine = NULL;	/* load_routine (defaults to SRECs) */
// OBSOLETE   m32r_cmds.load = NULL;	/* download command */
// OBSOLETE   m32r_cmds.loadresp = NULL;	/* load response */
// OBSOLETE   m32r_cmds.prompt = "ok ";	/* monitor command prompt */
// OBSOLETE   m32r_cmds.line_term = "\r";	/* end-of-line terminator */
// OBSOLETE   m32r_cmds.cmd_end = NULL;	/* optional command terminator */
// OBSOLETE   m32r_cmds.target = &m32r_ops;	/* target operations */
// OBSOLETE   m32r_cmds.stopbits = SERIAL_1_STOPBITS;	/* number of stop bits */
// OBSOLETE   m32r_cmds.regnames = m32r_regnames;	/* registers names */
// OBSOLETE   m32r_cmds.magic = MONITOR_OPS_MAGIC;	/* magic */
// OBSOLETE }				/* init_m32r_cmds */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_open (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   monitor_open (args, &m32r_cmds, from_tty);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Mon2000 monitor (MSA2000 board) */
// OBSOLETE 
// OBSOLETE static struct target_ops mon2000_ops;
// OBSOLETE static struct monitor_ops mon2000_cmds;
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE init_mon2000_cmds (void)
// OBSOLETE {
// OBSOLETE   mon2000_cmds.flags = MO_CLR_BREAK_USES_ADDR | MO_REGISTER_VALUE_FIRST;
// OBSOLETE   mon2000_cmds.init = m32r_inits;	/* Init strings */
// OBSOLETE   mon2000_cmds.cont = "go\r";	/* continue command */
// OBSOLETE   mon2000_cmds.step = "step\r";	/* single step */
// OBSOLETE   mon2000_cmds.stop = NULL;	/* interrupt command */
// OBSOLETE   mon2000_cmds.set_break = "%x +bp\r";	/* set a breakpoint */
// OBSOLETE   mon2000_cmds.clr_break = "%x -bp\r";	/* clear a breakpoint */
// OBSOLETE   mon2000_cmds.clr_all_break = "bpoff\r";	/* clear all breakpoints */
// OBSOLETE   mon2000_cmds.fill = "%x %x %x fill\r";	/* fill (start length val) */
// OBSOLETE   mon2000_cmds.setmem.cmdb = "%x 1 %x fill\r";	/* setmem.cmdb (addr, value) */
// OBSOLETE   mon2000_cmds.setmem.cmdw = "%x 1 %x fillh\r";		/* setmem.cmdw (addr, value) */
// OBSOLETE   mon2000_cmds.setmem.cmdl = "%x 1 %x fillw\r";		/* setmem.cmdl (addr, value) */
// OBSOLETE   mon2000_cmds.setmem.cmdll = NULL;	/* setmem.cmdll (addr, value) */
// OBSOLETE   mon2000_cmds.setmem.resp_delim = NULL;	/* setmem.resp_delim */
// OBSOLETE   mon2000_cmds.setmem.term = NULL;	/* setmem.term */
// OBSOLETE   mon2000_cmds.setmem.term_cmd = NULL;	/* setmem.term_cmd */
// OBSOLETE   mon2000_cmds.getmem.cmdb = "%x %x dump\r";	/* getmem.cmdb (addr, len) */
// OBSOLETE   mon2000_cmds.getmem.cmdw = NULL;	/* getmem.cmdw (addr, len) */
// OBSOLETE   mon2000_cmds.getmem.cmdl = NULL;	/* getmem.cmdl (addr, len) */
// OBSOLETE   mon2000_cmds.getmem.cmdll = NULL;	/* getmem.cmdll (addr, len) */
// OBSOLETE   mon2000_cmds.getmem.resp_delim = ": ";	/* getmem.resp_delim */
// OBSOLETE   mon2000_cmds.getmem.term = NULL;	/* getmem.term */
// OBSOLETE   mon2000_cmds.getmem.term_cmd = NULL;	/* getmem.term_cmd */
// OBSOLETE   mon2000_cmds.setreg.cmd = "%x to %%%s\r";	/* setreg.cmd (name, value) */
// OBSOLETE   mon2000_cmds.setreg.resp_delim = NULL;	/* setreg.resp_delim */
// OBSOLETE   mon2000_cmds.setreg.term = NULL;	/* setreg.term */
// OBSOLETE   mon2000_cmds.setreg.term_cmd = NULL;	/* setreg.term_cmd */
// OBSOLETE   mon2000_cmds.getreg.cmd = NULL;	/* getreg.cmd (name) */
// OBSOLETE   mon2000_cmds.getreg.resp_delim = NULL;	/* getreg.resp_delim */
// OBSOLETE   mon2000_cmds.getreg.term = NULL;	/* getreg.term */
// OBSOLETE   mon2000_cmds.getreg.term_cmd = NULL;	/* getreg.term_cmd */
// OBSOLETE   mon2000_cmds.dump_registers = ".reg\r";	/* dump_registers */
// OBSOLETE   mon2000_cmds.register_pattern = "\\(\\w+\\) += \\([0-9a-fA-F]+\\b\\)";	/* register_pattern */
// OBSOLETE   mon2000_cmds.supply_register = m32r_supply_register;	/* supply_register */
// OBSOLETE   mon2000_cmds.load_routine = NULL;	/* load_routine (defaults to SRECs) */
// OBSOLETE   mon2000_cmds.load = NULL;	/* download command */
// OBSOLETE   mon2000_cmds.loadresp = NULL;	/* load response */
// OBSOLETE   mon2000_cmds.prompt = "Mon2000>";	/* monitor command prompt */
// OBSOLETE   mon2000_cmds.line_term = "\r";	/* end-of-line terminator */
// OBSOLETE   mon2000_cmds.cmd_end = NULL;	/* optional command terminator */
// OBSOLETE   mon2000_cmds.target = &mon2000_ops;	/* target operations */
// OBSOLETE   mon2000_cmds.stopbits = SERIAL_1_STOPBITS;	/* number of stop bits */
// OBSOLETE   mon2000_cmds.regnames = m32r_regnames;	/* registers names */
// OBSOLETE   mon2000_cmds.magic = MONITOR_OPS_MAGIC;	/* magic */
// OBSOLETE }				/* init_mon2000_cmds */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE mon2000_open (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   monitor_open (args, &mon2000_cmds, from_tty);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: set_board_address
// OBSOLETE    Tell the BootOne monitor what it's ethernet IP address is. */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_set_board_address (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int resp_len;
// OBSOLETE   char buf[1024];
// OBSOLETE 
// OBSOLETE   if (args && *args)
// OBSOLETE     {
// OBSOLETE       monitor_printf ("ulip %s\n", args);
// OBSOLETE       resp_len = monitor_expect_prompt (buf, sizeof (buf));
// OBSOLETE       /* now parse the result for success */
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     error ("Requires argument (IP address for M32R-EVA board)");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: set_server_address
// OBSOLETE    Tell the BootOne monitor what gdb's ethernet IP address is. */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_set_server_address (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int resp_len;
// OBSOLETE   char buf[1024];
// OBSOLETE 
// OBSOLETE   if (args && *args)
// OBSOLETE     {
// OBSOLETE       monitor_printf ("uhip %s\n", args);
// OBSOLETE       resp_len = monitor_expect_prompt (buf, sizeof (buf));
// OBSOLETE       /* now parse the result for success */
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     error ("Requires argument (IP address of GDB's host computer)");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: set_download_path
// OBSOLETE    Tell the BootOne monitor the default path for downloadable SREC files. */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_set_download_path (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   int resp_len;
// OBSOLETE   char buf[1024];
// OBSOLETE 
// OBSOLETE   if (args && *args)
// OBSOLETE     {
// OBSOLETE       monitor_printf ("up %s\n", args);
// OBSOLETE       resp_len = monitor_expect_prompt (buf, sizeof (buf));
// OBSOLETE       /* now parse the result for success */
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     error ("Requires argument (default path for downloadable SREC files)");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE m32r_upload_command (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   bfd *abfd;
// OBSOLETE   asection *s;
// OBSOLETE   time_t start_time, end_time;	/* for timing of download */
// OBSOLETE   int resp_len, data_count = 0;
// OBSOLETE   char buf[1024];
// OBSOLETE   struct hostent *hostent;
// OBSOLETE   struct in_addr inet_addr;
// OBSOLETE 
// OBSOLETE   /* first check to see if there's an ethernet port! */
// OBSOLETE   monitor_printf ("ust\r");
// OBSOLETE   resp_len = monitor_expect_prompt (buf, sizeof (buf));
// OBSOLETE   if (!strchr (buf, ':'))
// OBSOLETE     error ("No ethernet connection!");
// OBSOLETE 
// OBSOLETE   if (board_addr == 0)
// OBSOLETE     {
// OBSOLETE       /* scan second colon in the output from the "ust" command */
// OBSOLETE       char *myIPaddress = strchr (strchr (buf, ':') + 1, ':') + 1;
// OBSOLETE 
// OBSOLETE       while (isspace (*myIPaddress))
// OBSOLETE 	myIPaddress++;
// OBSOLETE 
// OBSOLETE       if (!strncmp (myIPaddress, "0.0.", 4))	/* empty */
// OBSOLETE 	error ("Please use 'set board-address' to set the M32R-EVA board's IP address.");
// OBSOLETE       if (strchr (myIPaddress, '('))
// OBSOLETE 	*(strchr (myIPaddress, '(')) = '\0';	/* delete trailing junk */
// OBSOLETE       board_addr = xstrdup (myIPaddress);
// OBSOLETE     }
// OBSOLETE   if (server_addr == 0)
// OBSOLETE     {
// OBSOLETE       buf[0] = 0;
// OBSOLETE       gethostname (buf, sizeof (buf));
// OBSOLETE       if (buf[0] != 0)
// OBSOLETE 	hostent = gethostbyname (buf);
// OBSOLETE       if (hostent != 0)
// OBSOLETE 	{
// OBSOLETE #if 1
// OBSOLETE 	  memcpy (&inet_addr.s_addr, hostent->h_addr,
// OBSOLETE 		  sizeof (inet_addr.s_addr));
// OBSOLETE 	  server_addr = (char *) inet_ntoa (inet_addr);
// OBSOLETE #else
// OBSOLETE 	  server_addr = (char *) inet_ntoa (hostent->h_addr);
// OBSOLETE #endif
// OBSOLETE 	}
// OBSOLETE       if (server_addr == 0)	/* failed? */
// OBSOLETE 	error ("Need to know gdb host computer's IP address (use 'set server-address')");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (args == 0 || args[0] == 0)	/* no args: upload the current file */
// OBSOLETE     args = get_exec_file (1);
// OBSOLETE 
// OBSOLETE   if (args[0] != '/' && download_path == 0)
// OBSOLETE     {
// OBSOLETE       if (current_directory)
// OBSOLETE 	download_path = xstrdup (current_directory);
// OBSOLETE       else
// OBSOLETE 	error ("Need to know default download path (use 'set download-path')");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   start_time = time (NULL);
// OBSOLETE   monitor_printf ("uhip %s\r", server_addr);
// OBSOLETE   resp_len = monitor_expect_prompt (buf, sizeof (buf));		/* parse result? */
// OBSOLETE   monitor_printf ("ulip %s\r", board_addr);
// OBSOLETE   resp_len = monitor_expect_prompt (buf, sizeof (buf));		/* parse result? */
// OBSOLETE   if (args[0] != '/')
// OBSOLETE     monitor_printf ("up %s\r", download_path);	/* use default path */
// OBSOLETE   else
// OBSOLETE     monitor_printf ("up\r");	/* rooted filename/path */
// OBSOLETE   resp_len = monitor_expect_prompt (buf, sizeof (buf));		/* parse result? */
// OBSOLETE 
// OBSOLETE   if (strrchr (args, '.') && !strcmp (strrchr (args, '.'), ".srec"))
// OBSOLETE     monitor_printf ("ul %s\r", args);
// OBSOLETE   else				/* add ".srec" suffix */
// OBSOLETE     monitor_printf ("ul %s.srec\r", args);
// OBSOLETE   resp_len = monitor_expect_prompt (buf, sizeof (buf));		/* parse result? */
// OBSOLETE 
// OBSOLETE   if (buf[0] == 0 || strstr (buf, "complete") == 0)
// OBSOLETE     error ("Upload file not found: %s.srec\nCheck IP addresses and download path.", args);
// OBSOLETE   else
// OBSOLETE     printf_filtered (" -- Ethernet load complete.\n");
// OBSOLETE 
// OBSOLETE   end_time = time (NULL);
// OBSOLETE   abfd = bfd_openr (args, 0);
// OBSOLETE   if (abfd != NULL)
// OBSOLETE     {				/* Download is done -- print section statistics */
// OBSOLETE       if (bfd_check_format (abfd, bfd_object) == 0)
// OBSOLETE 	{
// OBSOLETE 	  printf_filtered ("File is not an object file\n");
// OBSOLETE 	}
// OBSOLETE       for (s = abfd->sections; s; s = s->next)
// OBSOLETE 	if (s->flags & SEC_LOAD)
// OBSOLETE 	  {
// OBSOLETE 	    bfd_size_type section_size = bfd_section_size (abfd, s);
// OBSOLETE 	    bfd_vma section_base = bfd_section_lma (abfd, s);
// OBSOLETE 	    unsigned int buffer;
// OBSOLETE 
// OBSOLETE 	    data_count += section_size;
// OBSOLETE 
// OBSOLETE 	    printf_filtered ("Loading section %s, size 0x%lx lma ",
// OBSOLETE 			     bfd_section_name (abfd, s), section_size);
// OBSOLETE 	    print_address_numeric (section_base, 1, gdb_stdout);
// OBSOLETE 	    printf_filtered ("\n");
// OBSOLETE 	    gdb_flush (gdb_stdout);
// OBSOLETE 	  }
// OBSOLETE       /* Finally, make the PC point at the start address */
// OBSOLETE       write_pc (bfd_get_start_address (abfd));
// OBSOLETE       report_transfer_performance (data_count, start_time, end_time);
// OBSOLETE       printf_filtered ("Start address 0x%lx\n", bfd_get_start_address (abfd));
// OBSOLETE     }
// OBSOLETE   inferior_ptid = null_ptid;	/* No process now */
// OBSOLETE 
// OBSOLETE   /* This is necessary because many things were based on the PC at the
// OBSOLETE      time that we attached to the monitor, which is no longer valid
// OBSOLETE      now that we have loaded new code (and just changed the PC).
// OBSOLETE      Another way to do this might be to call normal_stop, except that
// OBSOLETE      the stack may not be valid, and things would get horribly
// OBSOLETE      confused... */
// OBSOLETE 
// OBSOLETE   clear_symtab_users ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_m32r_rom (void)
// OBSOLETE {
// OBSOLETE   /* Initialize m32r RevC monitor target */
// OBSOLETE   init_m32r_cmds ();
// OBSOLETE   init_monitor_ops (&m32r_ops);
// OBSOLETE 
// OBSOLETE   m32r_ops.to_shortname = "m32r";
// OBSOLETE   m32r_ops.to_longname = "m32r monitor";
// OBSOLETE   m32r_ops.to_load = m32r_load_gen;	/* monitor lacks a download command */
// OBSOLETE   m32r_ops.to_doc = "Debug via the m32r monitor.\n\
// OBSOLETE Specify the serial device it is connected to (e.g. /dev/ttya).";
// OBSOLETE   m32r_ops.to_open = m32r_open;
// OBSOLETE   add_target (&m32r_ops);
// OBSOLETE 
// OBSOLETE   /* Initialize mon2000 monitor target */
// OBSOLETE   init_mon2000_cmds ();
// OBSOLETE   init_monitor_ops (&mon2000_ops);
// OBSOLETE 
// OBSOLETE   mon2000_ops.to_shortname = "mon2000";
// OBSOLETE   mon2000_ops.to_longname = "Mon2000 monitor";
// OBSOLETE   mon2000_ops.to_load = m32r_load_gen;	/* monitor lacks a download command */
// OBSOLETE   mon2000_ops.to_doc = "Debug via the Mon2000 monitor.\n\
// OBSOLETE Specify the serial device it is connected to (e.g. /dev/ttya).";
// OBSOLETE   mon2000_ops.to_open = mon2000_open;
// OBSOLETE   add_target (&mon2000_ops);
// OBSOLETE 
// OBSOLETE   add_show_from_set
// OBSOLETE     (add_set_cmd ("download-path", class_obscure, var_string,
// OBSOLETE 		  (char *) &download_path,
// OBSOLETE 		  "Set the default path for downloadable SREC files.",
// OBSOLETE 		  &setlist),
// OBSOLETE      &showlist);
// OBSOLETE 
// OBSOLETE   add_show_from_set
// OBSOLETE     (add_set_cmd ("board-address", class_obscure, var_string,
// OBSOLETE 		  (char *) &board_addr,
// OBSOLETE 		  "Set IP address for M32R-EVA target board.",
// OBSOLETE 		  &setlist),
// OBSOLETE      &showlist);
// OBSOLETE 
// OBSOLETE   add_show_from_set
// OBSOLETE     (add_set_cmd ("server-address", class_obscure, var_string,
// OBSOLETE 		  (char *) &server_addr,
// OBSOLETE 		"Set IP address for download server (GDB's host computer).",
// OBSOLETE 		  &setlist),
// OBSOLETE      &showlist);
// OBSOLETE 
// OBSOLETE   add_com ("upload", class_obscure, m32r_upload_command,
// OBSOLETE       "Upload the srec file via the monitor's Ethernet upload capability.");
// OBSOLETE 
// OBSOLETE   add_com ("tload", class_obscure, m32r_load, "test upload command.");
// OBSOLETE }
