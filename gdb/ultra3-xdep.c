/* OBSOLETE /* Host-dependent code for GDB, for NYU Ultra3 running Sym1 OS. */
/* OBSOLETE    Copyright 1988, 1989, 1991, 1992, 1993, 1995, 2000 */
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
/* OBSOLETE /* Assorted operating system circumventions */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef SYM1 */
/* OBSOLETE  */
/* OBSOLETE /* FIXME: Kludge this for now. It really should be system call. */ */
/* OBSOLETE int */
/* OBSOLETE getpagesize (void) */
/* OBSOLETE { */
/* OBSOLETE   return (8192); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* FIXME: Fake out the fcntl() call, which we don't have.  */ */
/* OBSOLETE fcntl (int fd, int cmd, int arg) */
/* OBSOLETE { */
/* OBSOLETE  */
/* OBSOLETE   switch (cmd) */
/* OBSOLETE     { */
/* OBSOLETE     case F_GETFL: */
/* OBSOLETE       return (O_RDONLY); */
/* OBSOLETE       break; */
/* OBSOLETE     default: */
/* OBSOLETE       printf_unfiltered ("Ultra3's fcntl() failing, cmd = %d.\n", cmd); */
/* OBSOLETE       return (-1); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /*  */
/* OBSOLETE  * 4.2 Signal support, requires linking with libjobs. */
/* OBSOLETE  */ */
/* OBSOLETE static int _SigMask; */
/* OBSOLETE #define sigbit(s)       (1L << ((s)-1)) */
/* OBSOLETE  */
/* OBSOLETE init_SigMask (void) */
/* OBSOLETE { */
/* OBSOLETE   /* Taken from the sym1 kernel in machdep.c:startup() */ */
/* OBSOLETE   _SigMask = sigbit (SIGTSTP) | sigbit (SIGTTOU) | sigbit (SIGTTIN) | */
/* OBSOLETE     sigbit (SIGCHLD) | sigbit (SIGTINT); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE sigmask (int signo) */
/* OBSOLETE { */
/* OBSOLETE   return (1 << (signo - 1)); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE sigsetmask (unsigned int sigmask) */
/* OBSOLETE { */
/* OBSOLETE   int i, mask = 1; */
/* OBSOLETE   int lastmask = _SigMask; */
/* OBSOLETE  */
/* OBSOLETE   for (i = 0; i < NSIG; i++) */
/* OBSOLETE     { */
/* OBSOLETE       if (sigmask & mask) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  if (!(_SigMask & mask)) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      sighold (i + 1); */
/* OBSOLETE 	      _SigMask |= mask; */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE       else if (_SigMask & mask) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  sigrelse (i + 1); */
/* OBSOLETE 	  _SigMask &= ~mask; */
/* OBSOLETE 	} */
/* OBSOLETE       mask <<= 1; */
/* OBSOLETE     } */
/* OBSOLETE   return (lastmask); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE sigblock (unsigned int sigmask) */
/* OBSOLETE { */
/* OBSOLETE   int i, mask = 1; */
/* OBSOLETE   int lastmask = _SigMask; */
/* OBSOLETE  */
/* OBSOLETE   for (i = 0; i < NSIG; i++) */
/* OBSOLETE     { */
/* OBSOLETE       if ((sigmask & mask) && !(_SigMask & mask)) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  sighold (i + 1); */
/* OBSOLETE 	  _SigMask |= mask; */
/* OBSOLETE 	} */
/* OBSOLETE       mask <<= 1; */
/* OBSOLETE     } */
/* OBSOLETE   return (lastmask); */
/* OBSOLETE } */
/* OBSOLETE #endif /* SYM1 */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Initialization code for this module.  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE _initialize_ultra3 (void) */
/* OBSOLETE { */
/* OBSOLETE #ifdef SYM1 */
/* OBSOLETE   init_SigMask (); */
/* OBSOLETE #endif */
/* OBSOLETE } */
