/* OBSOLETE /* umax host stuff. */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1995, 1999, 2000, 2001 */
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
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "inferior.h" */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE #include <sys/param.h> */
/* OBSOLETE #include <sys/dir.h> */
/* OBSOLETE #include <signal.h> */
/* OBSOLETE #include <sys/ioctl.h> */
/* OBSOLETE #include <fcntl.h> */
/* OBSOLETE  */
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE #include <sys/ptrace.h> */
/* OBSOLETE #define PTRACE_ATTACH PT_ATTACH */
/* OBSOLETE #define PTRACE_DETACH PT_FREEPROC */
/* OBSOLETE  */
/* OBSOLETE #include <sys/file.h> */
/* OBSOLETE #include "gdb_stat.h" */
/* OBSOLETE  */
/* OBSOLETE /* Work with core dump and executable files, for GDB.  */
/* OBSOLETE    This code would be in corefile.c if it weren't machine-dependent. */ */
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
/* OBSOLETE       /* 4.2-style (and perhaps also sysV-style) core dump file.  */ */
/* OBSOLETE       { */
/* OBSOLETE 	struct ptrace_user u; */
/* OBSOLETE 	int reg_offset; */
/* OBSOLETE  */
/* OBSOLETE 	val = myread (corechan, &u, sizeof u); */
/* OBSOLETE 	if (val < 0) */
/* OBSOLETE 	  perror_with_name (filename); */
/* OBSOLETE 	data_start = exec_data_start; */
/* OBSOLETE  */
/* OBSOLETE 	data_end = data_start + u.pt_dsize; */
/* OBSOLETE 	stack_start = stack_end - u.pt_ssize; */
/* OBSOLETE 	data_offset = sizeof u; */
/* OBSOLETE 	stack_offset = data_offset + u.pt_dsize; */
/* OBSOLETE 	reg_offset = 0; */
/* OBSOLETE  */
/* OBSOLETE 	memcpy (&core_aouthdr, &u.pt_aouthdr, sizeof (AOUTHDR)); */
/* OBSOLETE 	printf_unfiltered ("Core file is from \"%s\".\n", u.pt_comm); */
/* OBSOLETE 	if (u.pt_signal > 0) */
/* OBSOLETE 	  printf_unfiltered ("Program terminated with signal %d, %s.\n", */
/* OBSOLETE 			     u.pt_signal, safe_strsignal (u.pt_signal)); */
/* OBSOLETE  */
/* OBSOLETE 	/* Read the register values out of the core file and store */
/* OBSOLETE 	   them where `read_register' will find them.  */ */
/* OBSOLETE  */
/* OBSOLETE 	{ */
/* OBSOLETE 	  register int regno; */
/* OBSOLETE  */
/* OBSOLETE 	  for (regno = 0; regno < NUM_REGS; regno++) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      char buf[MAX_REGISTER_RAW_SIZE]; */
/* OBSOLETE  */
/* OBSOLETE 	      val = lseek (corechan, register_addr (regno, reg_offset), 0); */
/* OBSOLETE 	      if (val < 0) */
/* OBSOLETE 		perror_with_name (filename); */
/* OBSOLETE  */
/* OBSOLETE 	      val = myread (corechan, buf, sizeof buf); */
/* OBSOLETE 	      if (val < 0) */
/* OBSOLETE 		perror_with_name (filename); */
/* OBSOLETE 	      supply_register (regno, buf); */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
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
/* OBSOLETE       validate_files (); */
/* OBSOLETE     } */
/* OBSOLETE   else if (from_tty) */
/* OBSOLETE     printf_unfiltered ("No core file now.\n"); */
/* OBSOLETE } */
