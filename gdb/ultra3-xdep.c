/* Host-dependent code for GDB, for NYU Ultra3 running Sym1 OS.
   Copyright (C) 1988, 1989, 1991, 1992 Free Software Foundation, Inc.
   Contributed by David Wood (wood@nyu.edu) at New York University.

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

#define DEBUG
#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "value.h"

#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>  

#include "gdbcore.h"

#include <sys/file.h>
#include <sys/stat.h>

/* Assumes support for AMD's Binary Compatibility Standard
   for ptrace().  If you define ULTRA3, the ultra3 extensions to
   ptrace() are used allowing the reading of more than one register
   at a time. 

   This file assumes KERNEL_DEBUGGING is turned off.  This means
   that if the user/gdb tries to read gr64-gr95 or any of the 
   protected special registers we silently return -1 (see the
   CANNOT_STORE/FETCH_REGISTER macros).  */
#define	ULTRA3

#if !defined (offsetof)
# define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif

extern int errno;
struct ptrace_user pt_struct;

/*  
 * Takes a register number as defined in tm.h via REGISTER_NAMES, and maps
 * it to an offset in a struct ptrace_user defined by AMD's BCS.
 * That is, it defines the mapping between gdb register numbers and items in
 * a struct ptrace_user.
 * A register protection scheme is set up here.  If a register not
 * available to the user is specified in 'regno', then an address that
 * will cause ptrace() to fail is returned.
 */
unsigned int 
register_addr (regno,blockend)
     unsigned int	regno;
     char		*blockend;
{
  if ((regno >= LR0_REGNUM) && (regno < LR0_REGNUM + 128)) {
    return(offsetof(struct ptrace_user,pt_lr[regno-LR0_REGNUM]));
  } else if ((regno >= GR96_REGNUM) && (regno < GR96_REGNUM + 32)) {
    return(offsetof(struct ptrace_user,pt_gr[regno-GR96_REGNUM]));
  } else {
    switch (regno) {
	case GR1_REGNUM: return(offsetof(struct ptrace_user,pt_gr1));
	case CPS_REGNUM: return(offsetof(struct ptrace_user,pt_psr));
	case NPC_REGNUM: return(offsetof(struct ptrace_user,pt_pc0));
	case PC_REGNUM:  return(offsetof(struct ptrace_user,pt_pc1));
	case PC2_REGNUM: return(offsetof(struct ptrace_user,pt_pc2));
	case IPC_REGNUM: return(offsetof(struct ptrace_user,pt_ipc));
	case IPA_REGNUM: return(offsetof(struct ptrace_user,pt_ipa));
	case IPB_REGNUM: return(offsetof(struct ptrace_user,pt_ipb));
	case Q_REGNUM:   return(offsetof(struct ptrace_user,pt_q));
	case BP_REGNUM:  return(offsetof(struct ptrace_user,pt_bp));
	case FC_REGNUM:  return(offsetof(struct ptrace_user,pt_fc));
	default:
	     fprintf_filtered(stderr,"register_addr():Bad register %s (%d)\n", 
				reg_names[regno],regno);
	     return(0xffffffff);	/* Should make ptrace() fail */
    }
  }
}


/* Assorted operating system circumventions */

#ifdef SYM1

/* FIXME: Kludge this for now. It really should be system call. */
int
getpagesize()
{ return(8192); }

/* FIXME: Fake out the fcntl() call, which we don't have.  */
fcntl(fd, cmd, arg)
int fd, cmd, arg;
{

  switch (cmd) {
	case F_GETFL: return(O_RDONLY);	break;
	default:	
		printf("Ultra3's fcntl() failing, cmd = %d.\n",cmd);
		return(-1);
  }
}


/* 
 * 4.2 Signal support, requires linking with libjobs.
 */
static int	_SigMask;
#define sigbit(s)       (1L << ((s)-1))

init_SigMask()
{
	/* Taken from the sym1 kernel in machdep.c:startup() */
	_SigMask = sigbit (SIGTSTP) | sigbit (SIGTTOU) | sigbit (SIGTTIN) |
                        sigbit (SIGCHLD) | sigbit (SIGTINT);
}

sigmask(signo)
	int signo;
{
	return (1 << (signo-1));
}

sigsetmask(sigmask)
unsigned int sigmask;
{
	int i, mask = 1;
	int lastmask = _SigMask;

	for (i=0 ; i<NSIG ; i++) {
		if (sigmask & mask) { 
			if (!(_SigMask & mask)) {
				sighold(i+1);
				_SigMask |= mask;
			}
		} else if (_SigMask & mask) {
			sigrelse(i+1);
			_SigMask &= ~mask;
		}
		mask <<= 1;
	}
	return (lastmask);
}

sigblock(sigmask)
unsigned int sigmask;
{
	int i, mask = 1;
	int lastmask = _SigMask;

	for (i=0 ; i<NSIG ; i++) {
		if ((sigmask & mask) && !(_SigMask & mask)) {
			sighold(i+1);
			_SigMask |= mask;
		}
		mask <<= 1;
	}
	return (lastmask);
}
#endif /* SYM1 */


/* Initialization code for this module.  */

_initialize_ultra3 ()
{
#ifdef SYM1
	init_SigMask();
#endif
}
