/*  This file is part of the program psim.

    Copyright (C) 1994-1996, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#include <stdarg.h>
#include <ctype.h>

#include "bfd.h"
#include "sim-main.h"
#include "sim-utils.h"
#include "sim-options.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif


#define SIM_ADDR unsigned

SIM_DESC
sim_open (SIM_OPEN_KIND kind,
	  host_callback *callback,
	  struct _bfd *abfd,
	  char **argv)
{
  char *buf;
  SIM_DESC sd = sim_state_alloc (kind, callback);

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    return 0;

#define TIC80_MEM_START 0x2000000
#define TIC80_MEM_SIZE 0x100000

  /* main memory */
  asprintf (&buf, "memory region 0x%lx,0x%lx",
	    TIC80_MEM_START, TIC80_MEM_SIZE);
  sim_do_command (sd, buf);
  free (buf);
  /* interrupt memory */
  sim_do_command (sd, "memory region 0x1010000,0x1000");
  /* some memory at zero */
  sim_do_command (sd, "memory region 0,0x100000");
  
  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      /* Uninstall the modules to avoid memory leaks,
	 file descriptor leaks, etc.  */
      sim_module_uninstall (sd);
      return 0;
    }

  /* check for/establish the a reference program image */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL),
			   abfd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  /* establish any remaining configuration options */
  if (sim_config (sd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      /* Uninstall the modules to avoid memory leaks,
	 file descriptor leaks, etc.  */
      sim_module_uninstall (sd);
      return 0;
    }

 /* FIXME: for now */
  return sd;
}


void
sim_close (SIM_DESC sd, int quitting)
{
  /* Uninstall the modules to avoid memory leaks,
     file descriptor leaks, etc.  */
  sim_module_uninstall (sd);
}


/* FIXME - these magic numbers need to be moved elsewhere */

#define SP_REGNUM 1		/* Contains address of top of stack */
#define FP_REGNUM 31		/* Contains address of executing stack frame */
#define PC_REGNUM 32		/* Contains program counter (FIXME?) */
#define NPC_REGNUM 33		/* Contains the next program counter (FIXME?) */
#define A0_REGNUM 34		/* Accumulator register 0 */
#define A3_REGNUM 37		/* Accumulator register 1 */

#define R0_REGNUM 0             /* General Purpose Register 0 - for sim */
#define Rn_REGNUM 31            /* Last General Purpose Register - for sim */
#define An_REGNUM A3_REGNUM     /* Last Accumulator register - for sim */

int
sim_fetch_register (SIM_DESC sd, int regnr, unsigned char *buf, int length)
{
  if (regnr == R0_REGNUM)
    memset (buf, 0, sizeof (unsigned32));
  else if (regnr > R0_REGNUM && regnr <= Rn_REGNUM)
    *(unsigned32*)buf = H2T_4 (STATE_CPU (sd, 0)->reg[regnr - R0_REGNUM]);
  else if (regnr == PC_REGNUM)
    *(unsigned32*)buf = H2T_4 (STATE_CPU (sd, 0)->cia.ip);
  else if (regnr == NPC_REGNUM)
    *(unsigned32*)buf = H2T_4 (STATE_CPU (sd, 0)->cia.dp);
  else if (regnr >= A0_REGNUM && regnr <= An_REGNUM)
    *(unsigned64*)buf = H2T_8 (STATE_CPU (sd, 0)->acc[regnr - A0_REGNUM]);
  else
    sim_io_error (sd, "sim_fetch_register - unknown register nr %d", regnr);
  return -1;
}


int
sim_store_register (SIM_DESC sd, int regnr, unsigned char *buf, int length)
{
  if (regnr >= R0_REGNUM && regnr <= Rn_REGNUM)
    STATE_CPU (sd, 0)->reg[regnr - R0_REGNUM] = T2H_4 (*(unsigned32*)buf);
  else if (regnr == PC_REGNUM)
    STATE_CPU (sd, 0)->cia.ip = T2H_4 (*(unsigned32*)buf);
  else if (regnr == NPC_REGNUM)
    STATE_CPU (sd, 0)->cia.dp = T2H_4 (*(unsigned32*)buf);
  else if (regnr >= A0_REGNUM && regnr <= An_REGNUM)
    STATE_CPU (sd, 0)->acc[regnr - A0_REGNUM] = T2H_8 (*(unsigned64*)buf);
  else
    sim_io_error (sd, "sim_store_register - unknown register nr %d", regnr);
  return -1;
}


SIM_RC
sim_create_inferior (SIM_DESC sd,
		     struct _bfd *abfd,
		     char **argv,
		     char **envp)
{
  /* clear all registers */
  memset (&STATE_CPU (sd, 0)->reg, 0, sizeof (STATE_CPU (sd, 0)->reg));
  memset (&STATE_CPU (sd, 0)->acc, 0, sizeof (STATE_CPU (sd, 0)->acc));
  memset (&STATE_CPU (sd, 0)->cr, 0, sizeof (STATE_CPU (sd, 0)->cr));
  STATE_CPU (sd, 0)->is_user_mode = 0;
  memset (&STATE_CPU (sd, 0)->cia, 0, sizeof (STATE_CPU (sd, 0)->cia));
  /* initialize any modules */
  sim_module_init (sd);
  /* set the stack-pointer/program counter */
  if (abfd != NULL)
    STATE_CPU (sd, 0)->cia.ip = bfd_get_start_address (abfd);
  else
    STATE_CPU (sd, 0)->cia.ip = 0;
  STATE_CPU (sd, 0)->cia.dp = (STATE_CPU (sd, 0)->cia.ip
			       + sizeof (instruction_word));
  STATE_CPU (sd, 0)->cr[IE_CR] |= IE_CR_IE;
  STATE_CPU (sd, 0)->reg[1] = TIC80_MEM_START + TIC80_MEM_SIZE - 16;
  return SIM_RC_OK;
}


void
sim_do_command (SIM_DESC sd, char *cmd)
{
  if (sim_args_command (sd, cmd) != SIM_RC_OK)
    sim_io_eprintf (sd, "Unknown command `%s'\n", cmd);
}
