// OBSOLETE /* Remote target glue for the SPARC Sparclet ROM monitor.
// OBSOLETE 
// OBSOLETE    Copyright 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002 Free
// OBSOLETE    Software Foundation, Inc.
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
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "target.h"
// OBSOLETE #include "monitor.h"
// OBSOLETE #include "serial.h"
// OBSOLETE #include "srec.h"
// OBSOLETE #include "symtab.h"
// OBSOLETE #include "symfile.h"		/* for generic_load */
// OBSOLETE #include "regcache.h"
// OBSOLETE #include <time.h>
// OBSOLETE 
// OBSOLETE extern void report_transfer_performance (unsigned long, time_t, time_t);
// OBSOLETE 
// OBSOLETE static struct target_ops sparclet_ops;
// OBSOLETE 
// OBSOLETE static void sparclet_open (char *args, int from_tty);
// OBSOLETE 
// OBSOLETE /* This array of registers need to match the indexes used by GDB.
// OBSOLETE    This exists because the various ROM monitors use different strings
// OBSOLETE    than does GDB, and don't necessarily support all the registers
// OBSOLETE    either. So, typing "info reg sp" becomes a "r30".  */
// OBSOLETE 
// OBSOLETE /*PSR 0x00000080  impl ver icc AW LE EE EC EF PIL S PS ET CWP  WIM
// OBSOLETE    0x0  0x0 0x0  0  0  0  0  0 0x0 1  0  0 0x00 0x2
// OBSOLETE    0000010
// OBSOLETE    INS        LOCALS       OUTS      GLOBALS
// OBSOLETE    0  0x00000000  0x00000000  0x00000000  0x00000000
// OBSOLETE    1  0x00000000  0x00000000  0x00000000  0x00000000
// OBSOLETE    2  0x00000000  0x00000000  0x00000000  0x00000000
// OBSOLETE    3  0x00000000  0x00000000  0x00000000  0x00000000
// OBSOLETE    4  0x00000000  0x00000000  0x00000000  0x00000000
// OBSOLETE    5  0x00000000  0x00001000  0x00000000  0x00000000
// OBSOLETE    6  0x00000000  0x00000000  0x123f0000  0x00000000
// OBSOLETE    7  0x00000000  0x00000000  0x00000000  0x00000000
// OBSOLETE    pc:  0x12010000 0x00000000    unimp
// OBSOLETE    npc: 0x12010004 0x00001000    unimp     0x1000
// OBSOLETE    tbr: 0x00000000
// OBSOLETE    y:   0x00000000
// OBSOLETE  */
// OBSOLETE /* these correspond to the offsets from tm-* files from config directories */
// OBSOLETE 
// OBSOLETE /* is wim part of psr?? */
// OBSOLETE /* monitor wants lower case */
// OBSOLETE static char *sparclet_regnames[] = {
// OBSOLETE   "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7", 
// OBSOLETE   "o0", "o1", "o2", "o3", "o4", "o5", "o6", "o7", 
// OBSOLETE   "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7", 
// OBSOLETE   "i0", "i1", "i2", "i3", "i4", "i5", "i6", "i7", 
// OBSOLETE 
// OBSOLETE   "", "", "", "", "", "", "", "", /* no FPU regs */
// OBSOLETE   "", "", "", "", "", "", "", "", 
// OBSOLETE   "", "", "", "", "", "", "", "", 
// OBSOLETE   "", "", "", "", "", "", "", "", 
// OBSOLETE 				  /* no CPSR, FPSR */
// OBSOLETE   "y", "psr", "wim", "tbr", "pc", "npc", "", "", 
// OBSOLETE 
// OBSOLETE   "ccsr", "ccpr", "cccrcr", "ccor", "ccobr", "ccibr", "ccir", "", 
// OBSOLETE 
// OBSOLETE   /*       ASR15                 ASR19 (don't display them) */  
// OBSOLETE   "asr1",  "", "asr17", "asr18", "", "asr20", "asr21", "asr22", 
// OBSOLETE /*
// OBSOLETE   "awr0",  "awr1",  "awr2",  "awr3",  "awr4",  "awr5",  "awr6",  "awr7",  
// OBSOLETE   "awr8",  "awr9",  "awr10", "awr11", "awr12", "awr13", "awr14", "awr15", 
// OBSOLETE   "awr16", "awr17", "awr18", "awr19", "awr20", "awr21", "awr22", "awr23", 
// OBSOLETE   "awr24", "awr25", "awr26", "awr27", "awr28", "awr29", "awr30", "awr31", 
// OBSOLETE   "apsr",
// OBSOLETE  */
// OBSOLETE };
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Function: sparclet_supply_register
// OBSOLETE    Just returns with no action.
// OBSOLETE    This function is required, because parse_register_dump (monitor.c)
// OBSOLETE    expects to be able to call it.  If we don't supply something, it will
// OBSOLETE    call a null pointer and core-dump.  Since this function does not 
// OBSOLETE    actually do anything, GDB will request the registers individually.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclet_supply_register (char *regname, int regnamelen, char *val, int vallen)
// OBSOLETE {
// OBSOLETE   return;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclet_load (struct serial *desc, char *file, int hashmark)
// OBSOLETE {
// OBSOLETE   bfd *abfd;
// OBSOLETE   asection *s;
// OBSOLETE   int i;
// OBSOLETE   CORE_ADDR load_offset;
// OBSOLETE   time_t start_time, end_time;
// OBSOLETE   unsigned long data_count = 0;
// OBSOLETE 
// OBSOLETE   /* enable user to specify address for downloading as 2nd arg to load */
// OBSOLETE 
// OBSOLETE   i = sscanf (file, "%*s 0x%lx", &load_offset);
// OBSOLETE   if (i >= 1)
// OBSOLETE     {
// OBSOLETE       char *p;
// OBSOLETE 
// OBSOLETE       for (p = file; *p != '\000' && !isspace (*p); p++);
// OBSOLETE 
// OBSOLETE       *p = '\000';
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     load_offset = 0;
// OBSOLETE 
// OBSOLETE   abfd = bfd_openr (file, 0);
// OBSOLETE   if (!abfd)
// OBSOLETE     {
// OBSOLETE       printf_filtered ("Unable to open file %s\n", file);
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (bfd_check_format (abfd, bfd_object) == 0)
// OBSOLETE     {
// OBSOLETE       printf_filtered ("File is not an object file\n");
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   start_time = time (NULL);
// OBSOLETE 
// OBSOLETE   for (s = abfd->sections; s; s = s->next)
// OBSOLETE     if (s->flags & SEC_LOAD)
// OBSOLETE       {
// OBSOLETE 	bfd_size_type section_size;
// OBSOLETE 	bfd_vma vma;
// OBSOLETE 
// OBSOLETE 	vma = bfd_get_section_vma (abfd, s) + load_offset;
// OBSOLETE 	section_size = bfd_section_size (abfd, s);
// OBSOLETE 
// OBSOLETE 	data_count += section_size;
// OBSOLETE 
// OBSOLETE 	printf_filtered ("%s\t: 0x%4x .. 0x%4x  ",
// OBSOLETE 			 bfd_get_section_name (abfd, s), vma,
// OBSOLETE 			 vma + section_size);
// OBSOLETE 	gdb_flush (gdb_stdout);
// OBSOLETE 
// OBSOLETE 	monitor_printf ("load c r %x %x\r", vma, section_size);
// OBSOLETE 
// OBSOLETE 	monitor_expect ("load: loading ", NULL, 0);
// OBSOLETE 	monitor_expect ("\r", NULL, 0);
// OBSOLETE 
// OBSOLETE 	for (i = 0; i < section_size; i += 2048)
// OBSOLETE 	  {
// OBSOLETE 	    int numbytes;
// OBSOLETE 	    char buf[2048];
// OBSOLETE 
// OBSOLETE 	    numbytes = min (sizeof buf, section_size - i);
// OBSOLETE 
// OBSOLETE 	    bfd_get_section_contents (abfd, s, buf, i, numbytes);
// OBSOLETE 
// OBSOLETE 	    serial_write (desc, buf, numbytes);
// OBSOLETE 
// OBSOLETE 	    if (hashmark)
// OBSOLETE 	      {
// OBSOLETE 		putchar_unfiltered ('#');
// OBSOLETE 		gdb_flush (gdb_stdout);
// OBSOLETE 	      }
// OBSOLETE 	  }			/* Per-packet (or S-record) loop */
// OBSOLETE 
// OBSOLETE 	monitor_expect_prompt (NULL, 0);
// OBSOLETE 
// OBSOLETE 	putchar_unfiltered ('\n');
// OBSOLETE       }				/* Loadable sections */
// OBSOLETE 
// OBSOLETE   monitor_printf ("reg pc %x\r", bfd_get_start_address (abfd));
// OBSOLETE   monitor_expect_prompt (NULL, 0);
// OBSOLETE   monitor_printf ("reg npc %x\r", bfd_get_start_address (abfd) + 4);
// OBSOLETE   monitor_expect_prompt (NULL, 0);
// OBSOLETE 
// OBSOLETE   monitor_printf ("run\r");
// OBSOLETE 
// OBSOLETE   end_time = time (NULL);
// OBSOLETE 
// OBSOLETE   if (hashmark)
// OBSOLETE     putchar_unfiltered ('\n');
// OBSOLETE 
// OBSOLETE   report_transfer_performance (data_count, start_time, end_time);
// OBSOLETE 
// OBSOLETE   pop_target ();
// OBSOLETE   push_remote_target (monitor_get_dev_name (), 1);
// OBSOLETE 
// OBSOLETE   throw_exception (RETURN_QUIT);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Define the monitor command strings. Since these are passed directly
// OBSOLETE    through to a printf style function, we may include formatting
// OBSOLETE    strings. We also need a CR or LF on the end.  */
// OBSOLETE 
// OBSOLETE /* need to pause the monitor for timing reasons, so slow it down */
// OBSOLETE 
// OBSOLETE static char *sparclet_inits[] =
// OBSOLETE {"\n\r\r\n", NULL};
// OBSOLETE 
// OBSOLETE static struct monitor_ops sparclet_cmds;
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE init_sparclet_cmds (void)
// OBSOLETE {
// OBSOLETE   sparclet_cmds.flags = MO_CLR_BREAK_USES_ADDR |
// OBSOLETE     MO_HEX_PREFIX |
// OBSOLETE     MO_NO_ECHO_ON_OPEN |
// OBSOLETE     MO_NO_ECHO_ON_SETMEM |
// OBSOLETE     MO_RUN_FIRST_TIME |
// OBSOLETE     MO_GETMEM_READ_SINGLE;	/* flags */
// OBSOLETE   sparclet_cmds.init = sparclet_inits;	/* Init strings */
// OBSOLETE   sparclet_cmds.cont = "cont\r";	/* continue command */
// OBSOLETE   sparclet_cmds.step = "step\r";	/* single step */
// OBSOLETE   sparclet_cmds.stop = "\r";	/* break interrupts the program */
// OBSOLETE   sparclet_cmds.set_break = "+bp %x\r";		/* set a breakpoint */
// OBSOLETE   sparclet_cmds.clr_break = "-bp %x\r";		/* can't use "br" because only 2 hw bps are supported */
// OBSOLETE   sparclet_cmds.clr_all_break = "-bp %x\r";	/* clear a breakpoint */
// OBSOLETE   "-bp\r";			/* clear all breakpoints */
// OBSOLETE   sparclet_cmds.fill = "fill %x -n %x -v %x -b\r";	/* fill (start length val) */
// OBSOLETE   /* can't use "fi" because it takes words, not bytes */
// OBSOLETE   /* ex [addr] [-n count] [-b|-s|-l]          default: ex cur -n 1 -b */
// OBSOLETE   sparclet_cmds.setmem.cmdb = "ex %x -b\r%x\rq\r";	/* setmem.cmdb (addr, value) */
// OBSOLETE   sparclet_cmds.setmem.cmdw = "ex %x -s\r%x\rq\r";	/* setmem.cmdw (addr, value) */
// OBSOLETE   sparclet_cmds.setmem.cmdl = "ex %x -l\r%x\rq\r";	/* setmem.cmdl (addr, value) */
// OBSOLETE   sparclet_cmds.setmem.cmdll = NULL;	/* setmem.cmdll (addr, value) */
// OBSOLETE   sparclet_cmds.setmem.resp_delim = NULL;	/*": " *//* setmem.resp_delim */
// OBSOLETE   sparclet_cmds.setmem.term = NULL;	/*"? " *//* setmem.term */
// OBSOLETE   sparclet_cmds.setmem.term_cmd = NULL;		/*"q\r" *//* setmem.term_cmd */
// OBSOLETE   /* since the parsing of multiple bytes is difficult due to
// OBSOLETE      interspersed addresses, we'll only read 1 value at a time,
// OBSOLETE      even tho these can handle a count */
// OBSOLETE   /* we can use -n to set count to read, but may have to parse? */
// OBSOLETE   sparclet_cmds.getmem.cmdb = "ex %x -n 1 -b\r";	/* getmem.cmdb (addr, #bytes) */
// OBSOLETE   sparclet_cmds.getmem.cmdw = "ex %x -n 1 -s\r";	/* getmem.cmdw (addr, #swords) */
// OBSOLETE   sparclet_cmds.getmem.cmdl = "ex %x -n 1 -l\r";	/* getmem.cmdl (addr, #words) */
// OBSOLETE   sparclet_cmds.getmem.cmdll = NULL;	/* getmem.cmdll (addr, #dwords) */
// OBSOLETE   sparclet_cmds.getmem.resp_delim = ": ";	/* getmem.resp_delim */
// OBSOLETE   sparclet_cmds.getmem.term = NULL;	/* getmem.term */
// OBSOLETE   sparclet_cmds.getmem.term_cmd = NULL;		/* getmem.term_cmd */
// OBSOLETE   sparclet_cmds.setreg.cmd = "reg %s 0x%x\r";	/* setreg.cmd (name, value) */
// OBSOLETE   sparclet_cmds.setreg.resp_delim = NULL;	/* setreg.resp_delim */
// OBSOLETE   sparclet_cmds.setreg.term = NULL;	/* setreg.term */
// OBSOLETE   sparclet_cmds.setreg.term_cmd = NULL;		/* setreg.term_cmd */
// OBSOLETE   sparclet_cmds.getreg.cmd = "reg %s\r";	/* getreg.cmd (name) */
// OBSOLETE   sparclet_cmds.getreg.resp_delim = " ";	/* getreg.resp_delim */
// OBSOLETE   sparclet_cmds.getreg.term = NULL;	/* getreg.term */
// OBSOLETE   sparclet_cmds.getreg.term_cmd = NULL;		/* getreg.term_cmd */
// OBSOLETE   sparclet_cmds.dump_registers = "reg\r";	/* dump_registers */
// OBSOLETE   sparclet_cmds.register_pattern = "\\(\\w+\\)=\\([0-9a-fA-F]+\\)";	/* register_pattern */
// OBSOLETE   sparclet_cmds.supply_register = sparclet_supply_register;	/* supply_register */
// OBSOLETE   sparclet_cmds.load_routine = sparclet_load;	/* load_routine */
// OBSOLETE   sparclet_cmds.load = NULL;	/* download command (srecs on console) */
// OBSOLETE   sparclet_cmds.loadresp = NULL;	/* load response */
// OBSOLETE   sparclet_cmds.prompt = "monitor>";	/* monitor command prompt */
// OBSOLETE   /* yikes!  gdb core dumps without this delimitor!! */
// OBSOLETE   sparclet_cmds.line_term = "\r";	/* end-of-command delimitor */
// OBSOLETE   sparclet_cmds.cmd_end = NULL;	/* optional command terminator */
// OBSOLETE   sparclet_cmds.target = &sparclet_ops;		/* target operations */
// OBSOLETE   sparclet_cmds.stopbits = SERIAL_1_STOPBITS;	/* number of stop bits */
// OBSOLETE   sparclet_cmds.regnames = sparclet_regnames;	/* registers names */
// OBSOLETE   sparclet_cmds.magic = MONITOR_OPS_MAGIC;	/* magic */
// OBSOLETE };
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclet_open (char *args, int from_tty)
// OBSOLETE {
// OBSOLETE   monitor_open (args, &sparclet_cmds, from_tty);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_sparclet (void)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   init_sparclet_cmds ();
// OBSOLETE 
// OBSOLETE   for (i = 0; i < NUM_REGS; i++)
// OBSOLETE     if (sparclet_regnames[i][0] == 'c' ||
// OBSOLETE 	sparclet_regnames[i][0] == 'a')
// OBSOLETE       sparclet_regnames[i] = 0;	/* mon can't report c* or a* regs */
// OBSOLETE 
// OBSOLETE   sparclet_regnames[0] = 0;	/* mon won't report %G0 */
// OBSOLETE 
// OBSOLETE   init_monitor_ops (&sparclet_ops);
// OBSOLETE   sparclet_ops.to_shortname = "sparclet";	/* for the target command */
// OBSOLETE   sparclet_ops.to_longname = "SPARC Sparclet monitor";
// OBSOLETE   /* use SW breaks; target only supports 2 HW breakpoints */
// OBSOLETE   sparclet_ops.to_insert_breakpoint = memory_insert_breakpoint;
// OBSOLETE   sparclet_ops.to_remove_breakpoint = memory_remove_breakpoint;
// OBSOLETE 
// OBSOLETE   sparclet_ops.to_doc =
// OBSOLETE     "Use a board running the Sparclet debug monitor.\n\
// OBSOLETE Specify the serial device it is connected to (e.g. /dev/ttya).";
// OBSOLETE 
// OBSOLETE   sparclet_ops.to_open = sparclet_open;
// OBSOLETE   add_target (&sparclet_ops);
// OBSOLETE }
