/*
 * This file is part of SIS.
 * 
 * SIS, SPARC instruction simulator V1.6 Copyright (C) 1995 Jiri Gaisler,
 * European Space Agency
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include "sis.h"
#include "bfd.h"
#include <dis-asm.h>

#include "remote-sim.h"

#ifndef fprintf
extern          fprintf();
#endif

#define	VAL(x)	strtol(x,(char *)NULL,0)

extern char   **buildargv(char *input);

extern struct disassemble_info dinfo;
extern struct pstate sregs;
extern struct estate ebase;

extern int      ctrl_c;
extern int      nfp;
extern int      ift;
extern int      rom8;
extern int      wrp;
extern int      sis_verbose;
extern char    *sis_version;
extern struct estate ebase;
extern struct evcell evbuf[];
extern struct irqcell irqarr[];
extern int      irqpend, ext_irl;
extern int      sparclite;
extern int      termsave;
extern char     uart_dev1[], uart_dev2[];

int             sis_gdb_break = 1;

host_callback *sim_callback;

run_sim(sregs, go, icount, dis)
    struct pstate  *sregs;
    int             go;
    unsigned int    icount;
    int             dis;
{
    int             mexc, ws;

    if (sis_verbose)
	(*sim_callback->printf_filtered) (sim_callback, "resuming at %x\n",
					  sregs->pc);
   init_stdio();
   sregs->starttime = time(NULL);
    while ((!sregs->err_mode & (go || (icount > 0))) &&
	   ((sregs->bptnum == 0) || !(sregs->bphit = check_bpt(sregs)))) {

	sregs->fhold = 0;
	sregs->hold = 0;
	sregs->icnt = 0;

	check_interrupts(sregs);
	if (sregs->trap) {
	    sregs->err_mode = execute_trap(sregs);
	} else {
	    if (sregs->psr & 0x080)
		sregs->asi = 8;
	    else
		sregs->asi = 9;

	    mexc = memory_read(sregs->asi, sregs->pc, &sregs->inst, &sregs->hold);
	    if (sregs->annul) {
		sregs->annul = 0;
		sregs->icnt = 1;
		sregs->pc = sregs->npc;
		sregs->npc = sregs->npc + 4;
	    } else {
		if (mexc) {
		    sregs->trap = I_ACC_EXC;
		} else {
		    if (sregs->histlen) {
			sregs->histbuf[sregs->histind].addr = sregs->pc;
			sregs->histbuf[sregs->histind].time = ebase.simtime;
			sregs->histind++;
			if (sregs->histind >= sregs->histlen)
			    sregs->histind = 0;
		    }
		    if (dis) {
			printf(" %8d ", ebase.simtime);
			dis_mem(sregs->pc, 1, &dinfo);
		    }
		    if ((sis_gdb_break) && (sregs->inst == 0x91d02001)) {
			if (sis_verbose)
			    (*sim_callback->printf_filtered) (sim_callback,
							      "SW BP hit at %x\n", sregs->pc);
			sim_stop();
			restore_stdio();
			clearerr(stdin);
			return (BPT_HIT);
		    } else
			dispatch_instruction(sregs);
		}
		icount--;
	    }
	    if (sregs->trap) {
		sregs->err_mode = execute_trap(sregs);
	    }
	}
	advance_time(sregs);
	if (ctrl_c) {
	    go = icount = 0;
	}
    }
    sim_stop();
    sregs->tottime += time(NULL) - sregs->starttime;
    restore_stdio();
    clearerr(stdin);
    if (sregs->err_mode)
	error_mode(sregs->pc);
    if (sregs->err_mode)
	return (ERROR);
    if (sregs->bphit) {
	if (sis_verbose)
	    (*sim_callback->printf_filtered) (sim_callback,
					      "HW BP hit at %x\n", sregs->pc);
	return (BPT_HIT);
    }
    if (ctrl_c) {
	ctrl_c = 0;
	return (CTRL_C);
    }
    return (TIME_OUT);
}

void
sim_set_callbacks (sd, ptr)
     SIM_DESC sd;
     host_callback *ptr;
{
  sim_callback = ptr;
}

void
sim_size (memsize)
     int memsize;
{
}

SIM_DESC
sim_open(kind, argv)
     SIM_OPEN_KIND kind;
     char **argv;
{

    int             argc = 0;
    int             cont = 1;
    int             stat = 1;
    int             grdl = 0;
    int             freq = 15;

    (*sim_callback->printf_filtered) (sim_callback, "\n SIS - SPARC instruction simulator %s\n", sis_version);
    (*sim_callback->printf_filtered) (sim_callback, " Bug-reports to Jiri Gaisler ESA/ESTEC (jgais@wd.estec.esa.nl)\n");
    while (argv[argc])
      argc++;
    while (stat < argc) {
	if (argv[stat][0] == '-') {
	    if (strcmp(argv[stat], "-v") == 0) {
		sis_verbose = 1;
	    } else
	    if (strcmp(argv[stat], "-nfp") == 0) {
		(*sim_callback->printf_filtered) (sim_callback, "no FPU\n");
		nfp = 1;
	    } else
            if (strcmp(argv[stat], "-ift") == 0) {
                ift = 1;
	    } else
	    if (strcmp(argv[stat], "-sparclite") == 0) {
		(*sim_callback->printf_filtered) (sim_callback, "simulating Sparclite\n");
		sparclite = 1;
	    } else
            if (strcmp(argv[stat], "-wrp") == 0) {
                wrp = 1;
	    } else
            if (strcmp(argv[stat], "-rom8") == 0) {
                rom8 = 1;
	    } else 
	    if (strcmp(argv[stat], "-uart1") == 0) {
		if ((stat + 1) < argc)
		    strcpy(uart_dev1, argv[++stat]);
	    } else
	    if (strcmp(argv[stat], "-uart2") == 0) {
		if ((stat + 1) < argc)
		    strcpy(uart_dev2, argv[++stat]);
	    } else
	    if (strcmp(argv[stat], "-nogdb") == 0) {
		(*sim_callback->printf_filtered) (sim_callback, "disabling GDB trap handling for breakpoints\n");
		sis_gdb_break = 0;
	    } else
	    if (strcmp(argv[stat], "-freq") == 0)
		if ((stat + 1) < argc) {
		    freq = VAL(argv[++stat]);
		    (*sim_callback->printf_filtered) (sim_callback, " ERC32 freq %d Mhz\n", freq);
		}
	} else
	    bfd_load(argv[stat]);
	stat++;
    }
    sregs.freq = freq;
    termsave = fcntl(0, F_GETFL, 0);
    INIT_DISASSEMBLE_INFO(dinfo, stdout,(fprintf_ftype)fprintf);
    dinfo.endian = BFD_ENDIAN_BIG;
    init_signals();
    reset_all();
    ebase.simtime = 0;
    init_sim();
    init_bpt(&sregs);
    reset_stat(&sregs);

    /* Fudge our descriptor for now.  */
    return (SIM_DESC) 1;
}

void
sim_close(sd, quitting)
     SIM_DESC sd;
     int quitting;
{

    exit_sim();
    fcntl(0, F_SETFL, termsave);

};

/* For communication from sim_load to sim_create_inferior.  */
static bfd_vma start_address;

SIM_RC
sim_load(sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char *prog;
     bfd *abfd;
     int from_tty;
{
    start_address = bfd_load (prog);
    return (0);
}

SIM_RC
sim_create_inferior(sd, argv, env)
     SIM_DESC sd;
     char **argv;
     char **env;
{
    ebase.simtime = 0;
    reset_all();
    reset_stat(&sregs);
    sregs.pc = start_address & ~3;
    sregs.npc = sregs.pc + 4;
    return SIM_RC_OK;
}

void
sim_store_register(sd, regno, value)
     SIM_DESC sd;
    int             regno;
    unsigned char  *value;
{
    /* FIXME: Review the computation of regval.  */
    int             regval = (value[0] << 24) | (value[1] << 16) | (value[2] << 8) | value[3];
    set_regi(&sregs, regno, regval);
}


void
sim_fetch_register(sd, regno, buf)
     SIM_DESC sd;
    int             regno;
    unsigned char  *buf;
{
    get_regi(&sregs, regno, buf);
}

int
sim_write(sd, mem, buf, length)
     SIM_DESC sd;
    SIM_ADDR             mem;
    unsigned char  *buf;
    int             length;
{
    return (sis_memory_write(mem, buf, length));
}

int
sim_read(sd, mem, buf, length)
     SIM_DESC sd;
     SIM_ADDR mem;
     unsigned char *buf;
     int length;
{
    return (sis_memory_read(mem, buf, length));
}

void
sim_info(sd, verbose)
     SIM_DESC sd;
     int verbose;
{
    show_stat(&sregs);
}

int             simstat = OK;

void
sim_stop_reason(sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop * reason;
     int *sigrc;
{

    switch (simstat) {
	case CTRL_C:
	*reason = sim_stopped;
	*sigrc = SIGINT;
	break;
    case OK:
    case TIME_OUT:
    case BPT_HIT:
	*reason = sim_stopped;
#ifdef _WIN32
#define SIGTRAP 5
#endif
	*sigrc = SIGTRAP;
	break;
    case ERROR:
	*sigrc = 0;
	*reason = sim_exited;
    }
    ctrl_c = 0;
    simstat = OK;
}

/* Flush all register windows out to the stack.  Starting after the invalid
   window, flush all windows up to, and including the current window.  This
   allows GDB to do backtraces and look at local variables for frames that
   are still in the register windows.  Note that strictly speaking, this
   behavior is *wrong* for several reasons.  First, it doesn't use the window
   overflow handlers.  It therefore assumes standard frame layouts and window
   handling policies.  Second, it changes system state behind the back of the
   target program.  I expect this to mainly pose problems when debugging trap
   handlers.
*/

#define PSR_CWP 0x7

static void
flush_windows ()
{
  int invwin;
  int cwp;
  int win;
  int ws;

  /* Keep current window handy */

  cwp = sregs.psr & PSR_CWP;

  /* Calculate the invalid window from the wim. */

  for (invwin = 0; invwin <= PSR_CWP; invwin++)
    if ((sregs.wim >> invwin) & 1)
      break;

  /* Start saving with the window after the invalid window. */

  invwin = (invwin - 1) & PSR_CWP;

  for (win = invwin; ; win = (win - 1) & PSR_CWP)
    {
      uint32 sp;
      int i;

      sp = sregs.r[(win * 16 + 14) & 0x7f];

      for (i = 0; i < 16; i++)
	memory_write (11, sp + 4 * i, &sregs.r[(win * 16 + 16 + i) & 0x7f], 2,
		      &ws);

      if (win == cwp)
	break;
    }
}

void
sim_resume(SIM_DESC sd, int step, int siggnal)
{
    simstat = run_sim(&sregs, 1, 0, 0);

    if (sis_gdb_break) flush_windows ();
}

int
sim_trace (sd)
     SIM_DESC sd;
{
  /* FIXME: unfinished */
  sim_resume (sd, 0, 0);
  return 1;
}

void
sim_kill(SIM_DESC sd)
{
}

void
sim_do_command(sd, cmd)
     SIM_DESC sd;
    char           *cmd;
{
    exec_cmd(&sregs, cmd);
}

#if 0 /* FIXME: These shouldn't exist.  */

int
sim_insert_breakpoint(int addr)
{
    if (sregs.bptnum < BPT_MAX) {
	sregs.bpts[sregs.bptnum] = addr & ~0x3;
	sregs.bptnum++;
	if (sis_verbose)
	    (*sim_callback->printf_filtered) (sim_callback, "inserted HW BP at %x\n", addr);
	return 0;
    } else
	return 1;
}

int
sim_remove_breakpoint(int addr)
{
    int             i = 0;

    while ((i < sregs.bptnum) && (sregs.bpts[i] != addr))
	i++;
    if (addr == sregs.bpts[i]) {
	for (; i < sregs.bptnum - 1; i++)
	    sregs.bpts[i] = sregs.bpts[i + 1];
	sregs.bptnum -= 1;
	if (sis_verbose)
	    (*sim_callback->printf_filtered) (sim_callback, "removed HW BP at %x\n", addr);
	return 0;
    }
    return 1;
}

#endif
