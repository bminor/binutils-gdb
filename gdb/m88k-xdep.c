/* Copyright (C) 1988, 1990 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include "gdbcore.h"
#include <sys/user.h>
#ifndef USER			/* added to support BCS ptrace_user */

#define USER ptrace_user
#endif
#include <sys/ioctl.h>
#include <fcntl.h>

#ifdef COFF_ENCAPSULATE
#include "a.out.encap.h"
#else
#include <a.out.h>
#endif

#include <sys/file.h>
#include <sys/stat.h>

#include "symtab.h"
#include "setjmp.h"
#include "value.h"

/* define offsets to the pc instruction offsets in ptrace_user struct */
#define SXIP_OFFSET (char *)&u.pt_sigframe.dg_sigframe.sc_sxip - \
				(char *)&u

#define SNIP_OFFSET (char *)&u.pt_sigframe.dg_sigframe.sc_snip - \
				(char *)&u

#define SFIP_OFFSET (char *)&u.pt_sigframe.dg_sigframe.sc_sfip - (char *)&u
extern int have_symbol_file_p();

extern jmp_buf stack_jmp;

extern int errno;
extern char registers[REGISTER_BYTES];

void
fetch_inferior_registers ()
{
  register int regno;
  register unsigned int regaddr;
  char buf[MAX_REGISTER_RAW_SIZE];
  register int i;

  struct USER u;
  unsigned int offset;

  offset = (char *) &u.pt_r0 - (char *) &u; 
  regaddr = offset; /* byte offset to r0;*/

/*  offset = ptrace (3, inferior_pid, offset, 0) - KERNEL_U_ADDR; */
  for (regno = 0; regno < NUM_REGS; regno++)
    {
      /*regaddr = register_addr (regno, offset);*/
	/* 88k enhancement  */
        
      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
 	{
 	  *(int *) &buf[i] = ptrace (3, inferior_pid, regaddr, 0);
 	  regaddr += sizeof (int);
 	}
      supply_register (regno, buf);
    }
    /* now load up registers 36 - 38; special pc registers */
    *(int *) &buf[0] = ptrace (3,inferior_pid,(char *)&u.pt_sigframe.dg_sigframe.sc_sxip - (char *)&u ,0);
    supply_register (SXIP_REGNUM, buf);
    *(int *) &buf[0] = ptrace (3, inferior_pid, (char *)&u.pt_sigframe.dg_sigframe.sc_snip - (char *)&u ,0);
    supply_register (SNIP_REGNUM, buf);
    *(int *) &buf[0] = ptrace (3, inferior_pid, (char *)&u.pt_sigframe.dg_sigframe.sc_sfip - (char *)&u ,0);
    supply_register (SFIP_REGNUM, buf);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

store_inferior_registers (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[80];

  struct USER u;

#if defined(BCS)
#if defined(DGUX)

  unsigned int offset = (char *) &u.pt_r0 - (char *) &u;

#endif /* defined (DGUX) */
#else

  unsigned int offset = (char *) &u.pt_r0 - (char *) &u;

#endif /* defined(BCS) */
/*  offset = ptrace (3, inferior_pid, offset, 0) - KERNEL_U_ADDR; */
  regaddr = offset;

  if (regno >= 0)
    {
/*      regaddr = register_addr (regno, offset); */
        if (regno < PC_REGNUM)
           { 
	     regaddr = offset + regno * sizeof (int);
             errno = 0;
             ptrace (6, inferior_pid, regaddr, read_register (regno));
             if (errno != 0)
	       {
	         sprintf (buf, "writing register number %d", regno);
	         perror_with_name (buf);
	       }
           }
	else if (regno == SXIP_REGNUM)
             ptrace (6, inferior_pid, SXIP_OFFSET, read_register(regno));
	else if (regno == SNIP_REGNUM)
	     ptrace (6, inferior_pid, SNIP_OFFSET, read_register(regno));
	else if (regno == SFIP_REGNUM)
	     ptrace (6, inferior_pid, SFIP_OFFSET, read_register(regno));
	else printf ("Bad register number for store_inferior routine\n");
    }
  else { 
         for (regno = 0; regno < NUM_REGS - 3; regno++)
           {
      /*      regaddr = register_addr (regno, offset); */
              errno = 0;
              regaddr = offset + regno * sizeof (int);
              ptrace (6, inferior_pid, regaddr, read_register (regno));
              if (errno != 0)
         	{
	          sprintf (buf, "writing register number %d", regno);
	          perror_with_name (buf);
	        }
           }
	 ptrace (6,inferior_pid,SXIP_OFFSET,read_register(SXIP_REGNUM));
	 ptrace (6,inferior_pid,SNIP_OFFSET,read_register(SNIP_REGNUM));
	 ptrace (6,inferior_pid,SFIP_OFFSET,read_register(SFIP_REGNUM));
       }	
           

}

#if 0
/* Core files are now a function of BFD.  */

void
core_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{
  int val;
  extern char registers[];

  /* Need symbol file and one with tdesc info for corefiles to work */
  if (!have_symbol_file_p())
    error ("Requires symbol-file and exec-file");
  if (!execfile)
    error ("Requires exec-file and symbol-file");

  /* Discard all vestiges of any previous core file
     and mark data and stack spaces as empty.  */

  if (corefile)
    free (corefile);
  corefile = 0;

  if (corechan >= 0)
    close (corechan);
  corechan = -1;

  data_start = 0;
  data_end = 0;
  stack_start = STACK_END_ADDR;
  stack_end = STACK_END_ADDR;

  /* Now, if a new core file was specified, open it and digest it.  */

  if (filename)
    {
      filename = tilde_expand (filename);
      make_cleanup (free, filename);
      
      if (have_inferior_p ())
	error ("To look at a core file, you must kill the inferior with \"kill\".");
      corechan = open (filename, O_RDONLY, 0);
      if (corechan < 0)
	perror_with_name (filename);
      /* 4.2-style (and perhaps also sysV-style) core dump file.  */
      {
	struct USER u;

	int reg_offset;

	val = myread (corechan, &u, sizeof u);
	if (val < 0)
	  perror_with_name (filename);
	data_start = u.pt_o_data_start;

	data_end = data_start +  u.pt_dsize;
	stack_start = stack_end -  u.pt_ssize;
	data_offset = u.pt_dataptr;
	stack_offset = data_offset + u.pt_dsize;

#if defined(BCS)
#if defined(DGUX)

	  reg_offset = 2048;


#endif /* defined (DGUX) */
#else

	/* original code: */
	reg_offset = (int) u.pt_r0 - KERNEL_U_ADDR;

#endif /* defined(BCS) */

	/* I don't know where to find this info.
	   So, for now, mark it as not available.  */
/*	N_SET_MAGIC (core_aouthdr, 0);  */
	bzero ((char *) &core_aouthdr, sizeof core_aouthdr);

	/* Read the register values out of the core file and store
	   them where `read_register' will find them.  */

	{
	  register int regno;

	  for (regno = 0; regno < NUM_REGS; regno++)
	    {
	      char buf[MAX_REGISTER_RAW_SIZE];

	      val = lseek (corechan, register_addr (regno, reg_offset), 0);
	      if (val < 0)
		perror_with_name (filename);

 	      val = myread (corechan, buf, sizeof buf);
	      if (val < 0)
		perror_with_name (filename);
	      supply_register (regno, buf);
	    }
	}
      }
      if (filename[0] == '/')
	corefile = savestring (filename, strlen (filename));
      else
	{
	  corefile = concat (current_directory, "/", filename);
	}
      init_tdesc();
      current_context = init_dcontext();
      set_current_frame ( create_new_frame(get_frame_base (read_pc()), 
					    read_pc ()));
      select_frame (get_current_frame (), 0);
      validate_files ();
    }
  else if (from_tty)
    printf ("No core file now.\n");
}
#endif

/* blockend is the address of the end of the user structure */
m88k_register_u_addr (blockend, regnum)
{
  struct USER u;
  int ustart = blockend - sizeof (struct USER);
  switch (regnum)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:          return (ustart + ((int) &u.pt_r0 - (int) &u) + sizeof(REGISTER_TYPE) * regnum);
    case PSR_REGNUM:  return (ustart + ((int) &u.pt_psr - (int) &u));
    case FPSR_REGNUM: return (ustart + ((int) &u.pt_fpsr - (int) &u));
    case FPCR_REGNUM: return (ustart + ((int) &u.pt_fpcr - (int) &u));
    case SXIP_REGNUM: return (ustart + SXIP_OFFSET); 
    case SNIP_REGNUM: return (ustart + SNIP_OFFSET);
    case SFIP_REGNUM: return (ustart + SFIP_OFFSET); 
    default: return (blockend + sizeof (REGISTER_TYPE) * regnum);
    }
}
