// OBSOLETE /****************************************************************************
// OBSOLETE 
// OBSOLETE 		THIS SOFTWARE IS NOT COPYRIGHTED
// OBSOLETE 
// OBSOLETE    HP offers the following for use in the public domain.  HP makes no
// OBSOLETE    warranty with regard to the software or it's performance and the
// OBSOLETE    user accepts the software "AS IS" with all faults.
// OBSOLETE 
// OBSOLETE    HP DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD
// OBSOLETE    TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OBSOLETE    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// OBSOLETE 
// OBSOLETE ****************************************************************************/
// OBSOLETE 
// OBSOLETE /****************************************************************************
// OBSOLETE  *  Header: remcom.c,v 1.34 91/03/09 12:29:49 glenne Exp $
// OBSOLETE  *
// OBSOLETE  *  Module name: remcom.c $
// OBSOLETE  *  Revision: 1.34 $
// OBSOLETE  *  Date: 91/03/09 12:29:49 $
// OBSOLETE  *  Contributor:     Lake Stevens Instrument Division$
// OBSOLETE  *
// OBSOLETE  *  Description:     low level support for gdb debugger. $
// OBSOLETE  *
// OBSOLETE  *  Considerations:  only works on target hardware $
// OBSOLETE  *
// OBSOLETE  *  Written by:      Glenn Engel $
// OBSOLETE  *  ModuleState:     Experimental $
// OBSOLETE  *
// OBSOLETE  *  NOTES:           See Below $
// OBSOLETE  *
// OBSOLETE  *  Modified for M32R by Michael Snyder, Cygnus Support.
// OBSOLETE  *
// OBSOLETE  *  To enable debugger support, two things need to happen.  One, a
// OBSOLETE  *  call to set_debug_traps() is necessary in order to allow any breakpoints
// OBSOLETE  *  or error conditions to be properly intercepted and reported to gdb.
// OBSOLETE  *  Two, a breakpoint needs to be generated to begin communication.  This
// OBSOLETE  *  is most easily accomplished by a call to breakpoint().  Breakpoint()
// OBSOLETE  *  simulates a breakpoint by executing a trap #1.
// OBSOLETE  *
// OBSOLETE  *  The external function exceptionHandler() is
// OBSOLETE  *  used to attach a specific handler to a specific M32R vector number.
// OBSOLETE  *  It should use the same privilege level it runs at.  It should
// OBSOLETE  *  install it as an interrupt gate so that interrupts are masked
// OBSOLETE  *  while the handler runs.
// OBSOLETE  *
// OBSOLETE  *  Because gdb will sometimes write to the stack area to execute function
// OBSOLETE  *  calls, this program cannot rely on using the supervisor stack so it
// OBSOLETE  *  uses it's own stack area reserved in the int array remcomStack.
// OBSOLETE  *
// OBSOLETE  *************
// OBSOLETE  *
// OBSOLETE  *    The following gdb commands are supported:
// OBSOLETE  *
// OBSOLETE  * command          function                               Return value
// OBSOLETE  *
// OBSOLETE  *    g             return the value of the CPU registers  hex data or ENN
// OBSOLETE  *    G             set the value of the CPU registers     OK or ENN
// OBSOLETE  *
// OBSOLETE  *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
// OBSOLETE  *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
// OBSOLETE  *    XAA..AA,LLLL: Write LLLL binary bytes at address     OK or ENN
// OBSOLETE  *                  AA..AA
// OBSOLETE  *
// OBSOLETE  *    c             Resume at current address              SNN   ( signal NN)
// OBSOLETE  *    cAA..AA       Continue at address AA..AA             SNN
// OBSOLETE  *
// OBSOLETE  *    s             Step one instruction                   SNN
// OBSOLETE  *    sAA..AA       Step one instruction from AA..AA       SNN
// OBSOLETE  *
// OBSOLETE  *    k             kill
// OBSOLETE  *
// OBSOLETE  *    ?             What was the last sigval ?             SNN   (signal NN)
// OBSOLETE  *
// OBSOLETE  * All commands and responses are sent with a packet which includes a
// OBSOLETE  * checksum.  A packet consists of
// OBSOLETE  *
// OBSOLETE  * $<packet info>#<checksum>.
// OBSOLETE  *
// OBSOLETE  * where
// OBSOLETE  * <packet info> :: <characters representing the command or response>
// OBSOLETE  * <checksum>    :: <two hex digits computed as modulo 256 sum of <packetinfo>>
// OBSOLETE  *
// OBSOLETE  * When a packet is received, it is first acknowledged with either '+' or '-'.
// OBSOLETE  * '+' indicates a successful transfer.  '-' indicates a failed transfer.
// OBSOLETE  *
// OBSOLETE  * Example:
// OBSOLETE  *
// OBSOLETE  * Host:                  Reply:
// OBSOLETE  * $m0,10#2a               +$00010203040506070809101112131415#42
// OBSOLETE  *
// OBSOLETE  ****************************************************************************/
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /************************************************************************
// OBSOLETE  *
// OBSOLETE  * external low-level support routines
// OBSOLETE  */
// OBSOLETE extern void putDebugChar();	/* write a single character      */
// OBSOLETE extern int getDebugChar();	/* read and return a single char */
// OBSOLETE extern void exceptionHandler();	/* assign an exception handler   */
// OBSOLETE 
// OBSOLETE /*****************************************************************************
// OBSOLETE  * BUFMAX defines the maximum number of characters in inbound/outbound buffers
// OBSOLETE  * at least NUMREGBYTES*2 are needed for register packets 
// OBSOLETE  */
// OBSOLETE #define BUFMAX 400
// OBSOLETE 
// OBSOLETE static char initialized;  /* boolean flag. != 0 means we've been initialized */
// OBSOLETE 
// OBSOLETE int     remote_debug;
// OBSOLETE /*  debug >  0 prints ill-formed commands in valid packets & checksum errors */
// OBSOLETE 
// OBSOLETE static const unsigned char hexchars[]="0123456789abcdef";
// OBSOLETE 
// OBSOLETE #define NUMREGS 24
// OBSOLETE 
// OBSOLETE /* Number of bytes of registers.  */
// OBSOLETE #define NUMREGBYTES (NUMREGS * 4)
// OBSOLETE enum regnames { R0,  R1,  R2,  R3,  R4,  R5,  R6,   R7,
// OBSOLETE 		R8,  R9,  R10, R11, R12, R13, R14,  R15,
// OBSOLETE 		PSW, CBR, SPI, SPU, BPC, PC,  ACCL, ACCH };
// OBSOLETE 
// OBSOLETE enum SYS_calls {
// OBSOLETE   	SYS_null, 
// OBSOLETE 	SYS_exit,
// OBSOLETE 	SYS_open,
// OBSOLETE 	SYS_close,
// OBSOLETE 	SYS_read,
// OBSOLETE 	SYS_write,
// OBSOLETE 	SYS_lseek,
// OBSOLETE 	SYS_unlink,
// OBSOLETE 	SYS_getpid,
// OBSOLETE 	SYS_kill,
// OBSOLETE 	SYS_fstat,
// OBSOLETE 	SYS_sbrk,
// OBSOLETE 	SYS_fork,
// OBSOLETE 	SYS_execve,
// OBSOLETE 	SYS_wait4,
// OBSOLETE 	SYS_link,
// OBSOLETE 	SYS_chdir,
// OBSOLETE 	SYS_stat,
// OBSOLETE 	SYS_utime,
// OBSOLETE 	SYS_chown,
// OBSOLETE 	SYS_chmod,
// OBSOLETE 	SYS_time,
// OBSOLETE 	SYS_pipe };
// OBSOLETE 
// OBSOLETE static int registers[NUMREGS];
// OBSOLETE 
// OBSOLETE #define STACKSIZE 8096
// OBSOLETE static unsigned char remcomInBuffer[BUFMAX];
// OBSOLETE static unsigned char remcomOutBuffer[BUFMAX];
// OBSOLETE static int  remcomStack[STACKSIZE/sizeof(int)];
// OBSOLETE static int*  stackPtr = &remcomStack[STACKSIZE/sizeof(int) - 1];
// OBSOLETE 
// OBSOLETE static unsigned int save_vectors[18];	/* previous exception vectors */
// OBSOLETE 
// OBSOLETE /* Indicate to caller of mem2hex or hex2mem that there has been an error. */
// OBSOLETE static volatile int mem_err = 0;
// OBSOLETE 
// OBSOLETE /* Store the vector number here (since GDB only gets the signal
// OBSOLETE    number through the usual means, and that's not very specific).  */
// OBSOLETE int gdb_m32r_vector = -1;
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE #include "syscall.h" /* for SYS_exit, SYS_write etc. */
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* Global entry points:
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE extern void handle_exception(int);
// OBSOLETE extern void set_debug_traps(void);
// OBSOLETE extern void breakpoint(void);
// OBSOLETE 
// OBSOLETE /* Local functions:
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static int  computeSignal(int);
// OBSOLETE static void putpacket(unsigned char *);
// OBSOLETE static unsigned char *getpacket(void);
// OBSOLETE 
// OBSOLETE static unsigned char *mem2hex(unsigned char *, unsigned char *, int, int);
// OBSOLETE static unsigned char *hex2mem(unsigned char *, unsigned char *, int, int);
// OBSOLETE static int  hexToInt(unsigned char **, int *);
// OBSOLETE static unsigned char *bin2mem(unsigned char *, unsigned char *, int, int);
// OBSOLETE static void stash_registers(void);
// OBSOLETE static void restore_registers(void);
// OBSOLETE static int  prepare_to_step(int);
// OBSOLETE static int  finish_from_step(void);
// OBSOLETE static unsigned long crc32 (unsigned char *, int, unsigned long);
// OBSOLETE 
// OBSOLETE static void gdb_error(char *, char *);
// OBSOLETE static int  gdb_putchar(int), gdb_puts(char *), gdb_write(char *, int);
// OBSOLETE 
// OBSOLETE static unsigned char *strcpy (unsigned char *, const unsigned char *);
// OBSOLETE static int   strlen (const unsigned char *);
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * This function does all command procesing for interfacing to gdb.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE void 
// OBSOLETE handle_exception(int exceptionVector)
// OBSOLETE {
// OBSOLETE   int    sigval, stepping;
// OBSOLETE   int    addr, length, i;
// OBSOLETE   unsigned char * ptr;
// OBSOLETE   unsigned char   buf[16];
// OBSOLETE   int binary;
// OBSOLETE 
// OBSOLETE   /* Do not call finish_from_step() if this is not a trap #1
// OBSOLETE    * (breakpoint trap).  Without this check, the finish_from_step()
// OBSOLETE    * might interpret a system call trap as a single step trap.  This
// OBSOLETE    * can happen if: the stub receives 's' and exits, but an interrupt
// OBSOLETE    * was pending; the interrupt is now handled and causes the stub to
// OBSOLETE    * be reentered because some function makes a system call.  
// OBSOLETE    */
// OBSOLETE   if (exceptionVector == 1)	/* Trap exception? */
// OBSOLETE     if (!finish_from_step())	/* Go see if stepping state needs update. */
// OBSOLETE       return;		/* "false step": let the target continue */
// OBSOLETE 
// OBSOLETE   gdb_m32r_vector = exceptionVector;
// OBSOLETE 
// OBSOLETE   if (remote_debug)
// OBSOLETE     {
// OBSOLETE       mem2hex((unsigned char *) &exceptionVector, buf, 4, 0);
// OBSOLETE       gdb_error("Handle exception %s, ", buf);
// OBSOLETE       mem2hex((unsigned char *) &registers[PC], buf, 4, 0);
// OBSOLETE       gdb_error("PC == 0x%s\n", buf);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* reply to host that an exception has occurred */
// OBSOLETE   sigval = computeSignal( exceptionVector );
// OBSOLETE 
// OBSOLETE   ptr = remcomOutBuffer;
// OBSOLETE  
// OBSOLETE   *ptr++ = 'T';         /* notify gdb with signo, PC, FP and SP */
// OBSOLETE   *ptr++ = hexchars[sigval >> 4];
// OBSOLETE   *ptr++ = hexchars[sigval & 0xf];
// OBSOLETE  
// OBSOLETE   *ptr++ = hexchars[PC >> 4];
// OBSOLETE   *ptr++ = hexchars[PC & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((unsigned char *)&registers[PC], ptr, 4, 0);     /* PC */
// OBSOLETE   *ptr++ = ';';
// OBSOLETE  
// OBSOLETE   *ptr++ = hexchars[R13 >> 4];
// OBSOLETE   *ptr++ = hexchars[R13 & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((unsigned char *)&registers[R13], ptr, 4, 0);    /* FP */
// OBSOLETE   *ptr++ = ';';
// OBSOLETE  
// OBSOLETE   *ptr++ = hexchars[R15 >> 4];
// OBSOLETE   *ptr++ = hexchars[R15 & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((unsigned char *)&registers[R15], ptr, 4, 0);    /* SP */
// OBSOLETE   *ptr++ = ';';
// OBSOLETE   *ptr++ = 0;
// OBSOLETE  
// OBSOLETE   if (exceptionVector == 0)     /* simulated SYS call stuff */
// OBSOLETE     {
// OBSOLETE       mem2hex((unsigned char *) &registers[PC], buf, 4, 0);
// OBSOLETE       switch (registers[R0]) {
// OBSOLETE       case SYS_exit:
// OBSOLETE 	gdb_error("Target program has exited at %s\n", buf);
// OBSOLETE 	ptr = remcomOutBuffer;
// OBSOLETE 	*ptr++ = 'W';
// OBSOLETE 	sigval = registers[R1] & 0xff;
// OBSOLETE 	*ptr++ = hexchars[sigval >> 4];
// OBSOLETE 	*ptr++ = hexchars[sigval & 0xf];
// OBSOLETE 	*ptr++ = 0;
// OBSOLETE 	break;
// OBSOLETE       case SYS_open:
// OBSOLETE 	gdb_error("Target attempts SYS_open call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_close:
// OBSOLETE 	gdb_error("Target attempts SYS_close call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_read:
// OBSOLETE 	gdb_error("Target attempts SYS_read call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_write:
// OBSOLETE 	if (registers[R1] == 1 ||       /* write to stdout  */
// OBSOLETE 	    registers[R1] == 2)		/* write to stderr  */
// OBSOLETE 	  {				/* (we can do that) */
// OBSOLETE 	    registers[R0] = gdb_write((void *) registers[R2], registers[R3]);
// OBSOLETE 	    return;
// OBSOLETE 	  }
// OBSOLETE 	else
// OBSOLETE 	  gdb_error("Target attempts SYS_write call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_lseek:
// OBSOLETE 	gdb_error("Target attempts SYS_lseek call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_unlink:
// OBSOLETE 	gdb_error("Target attempts SYS_unlink call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_getpid:
// OBSOLETE 	gdb_error("Target attempts SYS_getpid call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_kill:
// OBSOLETE 	gdb_error("Target attempts SYS_kill call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       case SYS_fstat:
// OBSOLETE 	gdb_error("Target attempts SYS_fstat call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       default:
// OBSOLETE 	gdb_error("Target attempts unknown SYS call at %s\n", buf);
// OBSOLETE 	break;
// OBSOLETE       }
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   putpacket(remcomOutBuffer);
// OBSOLETE 
// OBSOLETE   stepping = 0;
// OBSOLETE 
// OBSOLETE   while (1==1) {
// OBSOLETE     remcomOutBuffer[0] = 0;
// OBSOLETE     ptr = getpacket();
// OBSOLETE     binary = 0;
// OBSOLETE     switch (*ptr++) {
// OBSOLETE       default:	/* Unknown code.  Return an empty reply message. */
// OBSOLETE 	break;
// OBSOLETE       case 'R':
// OBSOLETE 	if (hexToInt (&ptr, &addr))
// OBSOLETE 	  registers[PC] = addr;
// OBSOLETE 	strcpy(remcomOutBuffer, "OK");
// OBSOLETE 	break;
// OBSOLETE       case '!':
// OBSOLETE 	strcpy(remcomOutBuffer, "OK");
// OBSOLETE 	break;
// OBSOLETE     case 'X': /* XAA..AA,LLLL:<binary data>#cs */
// OBSOLETE       binary = 1;
// OBSOLETE     case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
// OBSOLETE       /* TRY TO READ '%x,%x:'.  IF SUCCEED, SET PTR = 0 */
// OBSOLETE       {
// OBSOLETE         if (hexToInt(&ptr,&addr))
// OBSOLETE           if (*(ptr++) == ',')
// OBSOLETE             if (hexToInt(&ptr,&length))
// OBSOLETE               if (*(ptr++) == ':')
// OBSOLETE                 {
// OBSOLETE                   mem_err = 0;
// OBSOLETE                   if (binary)
// OBSOLETE                     bin2mem (ptr, (unsigned char *) addr, length, 1);
// OBSOLETE                   else
// OBSOLETE                     hex2mem(ptr, (unsigned char*) addr, length, 1);
// OBSOLETE                   if (mem_err) {
// OBSOLETE                     strcpy (remcomOutBuffer, "E03");
// OBSOLETE                     gdb_error ("memory fault", "");
// OBSOLETE                   } else {
// OBSOLETE                     strcpy(remcomOutBuffer,"OK");
// OBSOLETE                   }
// OBSOLETE                   ptr = 0;
// OBSOLETE                 }
// OBSOLETE         if (ptr)
// OBSOLETE           {
// OBSOLETE             strcpy(remcomOutBuffer,"E02");
// OBSOLETE           }
// OBSOLETE       }
// OBSOLETE 	break;
// OBSOLETE       case 'm': /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
// OBSOLETE 		/* TRY TO READ %x,%x.  IF SUCCEED, SET PTR = 0 */
// OBSOLETE 	if (hexToInt(&ptr,&addr))
// OBSOLETE 	  if (*(ptr++) == ',')
// OBSOLETE 	    if (hexToInt(&ptr,&length))
// OBSOLETE 	      {
// OBSOLETE 		ptr = 0;
// OBSOLETE 		mem_err = 0;
// OBSOLETE 		mem2hex((unsigned char*) addr, remcomOutBuffer, length, 1);
// OBSOLETE 		if (mem_err) {
// OBSOLETE 		  strcpy (remcomOutBuffer, "E03");
// OBSOLETE 		  gdb_error ("memory fault", "");
// OBSOLETE 		}
// OBSOLETE 	      }
// OBSOLETE 	if (ptr)
// OBSOLETE 	  {
// OBSOLETE 	    strcpy(remcomOutBuffer,"E01");
// OBSOLETE 	  }
// OBSOLETE 	break;
// OBSOLETE       case '?': 
// OBSOLETE 	remcomOutBuffer[0] = 'S';
// OBSOLETE 	remcomOutBuffer[1] =  hexchars[sigval >> 4];
// OBSOLETE 	remcomOutBuffer[2] =  hexchars[sigval % 16];
// OBSOLETE 	remcomOutBuffer[3] = 0;
// OBSOLETE 	break;
// OBSOLETE       case 'd': 
// OBSOLETE 	remote_debug = !(remote_debug);  /* toggle debug flag */
// OBSOLETE 	break;
// OBSOLETE       case 'g': /* return the value of the CPU registers */
// OBSOLETE 	mem2hex((unsigned char*) registers, remcomOutBuffer, NUMREGBYTES, 0);
// OBSOLETE 	break;
// OBSOLETE       case 'P': /* set the value of a single CPU register - return OK */
// OBSOLETE 	{
// OBSOLETE 	  int regno;
// OBSOLETE 
// OBSOLETE 	  if (hexToInt (&ptr, &regno) && *ptr++ == '=')
// OBSOLETE 	    if (regno >= 0 && regno < NUMREGS)
// OBSOLETE 	      {
// OBSOLETE 		int stackmode;
// OBSOLETE 
// OBSOLETE 		hex2mem (ptr, (unsigned char *) &registers[regno], 4, 0);
// OBSOLETE 		/*
// OBSOLETE 		 * Since we just changed a single CPU register, let's
// OBSOLETE 		 * make sure to keep the several stack pointers consistant.
// OBSOLETE 		 */
// OBSOLETE 		stackmode = registers[PSW] & 0x80;
// OBSOLETE 		if (regno == R15)	/* stack pointer changed */
// OBSOLETE 		  {			/* need to change SPI or SPU */
// OBSOLETE 		    if (stackmode == 0)
// OBSOLETE 		      registers[SPI] = registers[R15];
// OBSOLETE 		    else
// OBSOLETE 		      registers[SPU] = registers[R15];
// OBSOLETE 		  }
// OBSOLETE 		else if (regno == SPU)	/* "user" stack pointer changed */
// OBSOLETE 		  {
// OBSOLETE 		    if (stackmode != 0)	/* stack in user mode: copy SP */
// OBSOLETE 		      registers[R15] = registers[SPU];
// OBSOLETE 		  }
// OBSOLETE 		else if (regno == SPI)	/* "interrupt" stack pointer changed */
// OBSOLETE 		  {
// OBSOLETE 		    if (stackmode == 0)	/* stack in interrupt mode: copy SP */
// OBSOLETE 		      registers[R15] = registers[SPI];
// OBSOLETE 		  }
// OBSOLETE 		else if (regno == PSW)	/* stack mode may have changed! */
// OBSOLETE 		  {			/* force SP to either SPU or SPI */
// OBSOLETE 		    if (stackmode == 0)	/* stack in user mode */
// OBSOLETE 		      registers[R15] = registers[SPI];
// OBSOLETE 		    else		/* stack in interrupt mode */
// OBSOLETE 		      registers[R15] = registers[SPU];
// OBSOLETE 		  }
// OBSOLETE 		strcpy (remcomOutBuffer, "OK");
// OBSOLETE 		break;
// OBSOLETE 	      }
// OBSOLETE 	  strcpy (remcomOutBuffer, "E01");
// OBSOLETE 	  break;
// OBSOLETE 	}
// OBSOLETE       case 'G': /* set the value of the CPU registers - return OK */
// OBSOLETE 	hex2mem(ptr, (unsigned char*) registers, NUMREGBYTES, 0);
// OBSOLETE 	strcpy(remcomOutBuffer,"OK");
// OBSOLETE 	break;
// OBSOLETE       case 's': /* sAA..AA	Step one instruction from AA..AA(optional) */
// OBSOLETE 	stepping = 1;
// OBSOLETE       case 'c': /* cAA..AA	Continue from address AA..AA(optional) */
// OBSOLETE 		/* try to read optional parameter, pc unchanged if no parm */
// OBSOLETE 	if (hexToInt(&ptr,&addr))
// OBSOLETE 	  registers[ PC ] = addr;
// OBSOLETE 	
// OBSOLETE 	if (stepping)	/* single-stepping */
// OBSOLETE 	  {
// OBSOLETE 	    if (!prepare_to_step(0))	/* set up for single-step */
// OBSOLETE 	      {
// OBSOLETE 		/* prepare_to_step has already emulated the target insn:
// OBSOLETE 		   Send SIGTRAP to gdb, don't resume the target at all.  */
// OBSOLETE 		ptr = remcomOutBuffer;
// OBSOLETE 		*ptr++ = 'T';           /* Simulate stopping with SIGTRAP */
// OBSOLETE 		*ptr++ = '0';
// OBSOLETE 		*ptr++ = '5';
// OBSOLETE 
// OBSOLETE 		*ptr++ = hexchars[PC >> 4];     /* send PC */
// OBSOLETE 		*ptr++ = hexchars[PC & 0xf];
// OBSOLETE 		*ptr++ = ':';
// OBSOLETE 		ptr = mem2hex((unsigned char *)&registers[PC], ptr, 4, 0);
// OBSOLETE 		*ptr++ = ';';
// OBSOLETE 
// OBSOLETE 		*ptr++ = hexchars[R13 >> 4];    /* send FP */
// OBSOLETE 		*ptr++ = hexchars[R13 & 0xf];
// OBSOLETE 		*ptr++ = ':';
// OBSOLETE 		ptr = mem2hex((unsigned char *)&registers[R13], ptr, 4, 0);
// OBSOLETE 		*ptr++ = ';';
// OBSOLETE 
// OBSOLETE 		*ptr++ = hexchars[R15 >> 4];    /* send SP */
// OBSOLETE 		*ptr++ = hexchars[R15 & 0xf];
// OBSOLETE 		*ptr++ = ':';
// OBSOLETE 		ptr = mem2hex((unsigned char *)&registers[R15], ptr, 4, 0);
// OBSOLETE 		*ptr++ = ';';
// OBSOLETE 		*ptr++ = 0;
// OBSOLETE 
// OBSOLETE 		break;	
// OBSOLETE 	      }
// OBSOLETE 	  }
// OBSOLETE 	else	/* continuing, not single-stepping */
// OBSOLETE 	  {
// OBSOLETE 	    /* OK, about to do a "continue".  First check to see if the 
// OBSOLETE 	       target pc is on an odd boundary (second instruction in the 
// OBSOLETE 	       word).  If so, we must do a single-step first, because 
// OBSOLETE 	       ya can't jump or return back to an odd boundary!  */
// OBSOLETE 	    if ((registers[PC] & 2) != 0)
// OBSOLETE 	      prepare_to_step(1);
// OBSOLETE 	  }
// OBSOLETE 
// OBSOLETE 	return;
// OBSOLETE 
// OBSOLETE       case 'D':	/* Detach */
// OBSOLETE #if 0
// OBSOLETE 	/* I am interpreting this to mean, release the board from control 
// OBSOLETE 	   by the remote stub.  To do this, I am restoring the original
// OBSOLETE 	   (or at least previous) exception vectors.
// OBSOLETE 	 */
// OBSOLETE 	for (i = 0; i < 18; i++)
// OBSOLETE 	  exceptionHandler (i, save_vectors[i]);
// OBSOLETE 	putpacket ("OK");
// OBSOLETE 	return;		/* continue the inferior */
// OBSOLETE #else
// OBSOLETE 	strcpy(remcomOutBuffer,"OK");
// OBSOLETE 	break;
// OBSOLETE #endif
// OBSOLETE     case 'q':
// OBSOLETE       if (*ptr++ == 'C' &&
// OBSOLETE 	  *ptr++ == 'R' &&
// OBSOLETE 	  *ptr++ == 'C' &&
// OBSOLETE 	  *ptr++ == ':')
// OBSOLETE 	{
// OBSOLETE 	  unsigned long start, len, our_crc;
// OBSOLETE 
// OBSOLETE 	  if (hexToInt (&ptr, (int *) &start) &&
// OBSOLETE 	      *ptr++ == ','                   &&
// OBSOLETE 	      hexToInt (&ptr, (int *) &len))
// OBSOLETE 	    {
// OBSOLETE 	      remcomOutBuffer[0] = 'C';
// OBSOLETE 	      our_crc = crc32 ((unsigned char *) start, len, 0xffffffff);
// OBSOLETE 	      mem2hex ((char *) &our_crc, 
// OBSOLETE 		       &remcomOutBuffer[1], 
// OBSOLETE 		       sizeof (long), 
// OBSOLETE 		       0); 
// OBSOLETE 	    } /* else do nothing */
// OBSOLETE 	} /* else do nothing */
// OBSOLETE       break;
// OBSOLETE 
// OBSOLETE       case 'k': /* kill the program */
// OBSOLETE 	continue;
// OBSOLETE       } /* switch */
// OBSOLETE 
// OBSOLETE     /* reply to the request */
// OBSOLETE     putpacket(remcomOutBuffer);
// OBSOLETE   }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* qCRC support */
// OBSOLETE 
// OBSOLETE /* Table used by the crc32 function to calcuate the checksum. */
// OBSOLETE static unsigned long crc32_table[256] = {0, 0};
// OBSOLETE 
// OBSOLETE static unsigned long
// OBSOLETE crc32 (unsigned char *buf, int len, unsigned long crc)
// OBSOLETE {
// OBSOLETE   if (! crc32_table[1])
// OBSOLETE     {
// OBSOLETE       /* Initialize the CRC table and the decoding table. */
// OBSOLETE       int i, j;
// OBSOLETE       unsigned long c;
// OBSOLETE 
// OBSOLETE       for (i = 0; i < 256; i++)
// OBSOLETE 	{
// OBSOLETE 	  for (c = i << 24, j = 8; j > 0; --j)
// OBSOLETE 	    c = c & 0x80000000 ? (c << 1) ^ 0x04c11db7 : (c << 1);
// OBSOLETE 	  crc32_table[i] = c;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   while (len--)
// OBSOLETE     {
// OBSOLETE       crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
// OBSOLETE       buf++;
// OBSOLETE     }
// OBSOLETE   return crc;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int 
// OBSOLETE hex (unsigned char ch)
// OBSOLETE {
// OBSOLETE   if ((ch >= 'a') && (ch <= 'f')) return (ch-'a'+10);
// OBSOLETE   if ((ch >= '0') && (ch <= '9')) return (ch-'0');
// OBSOLETE   if ((ch >= 'A') && (ch <= 'F')) return (ch-'A'+10);
// OBSOLETE   return (-1);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* scan for the sequence $<data>#<checksum>     */
// OBSOLETE 
// OBSOLETE unsigned char *
// OBSOLETE getpacket (void)
// OBSOLETE {
// OBSOLETE   unsigned char *buffer = &remcomInBuffer[0];
// OBSOLETE   unsigned char checksum;
// OBSOLETE   unsigned char xmitcsum;
// OBSOLETE   int count;
// OBSOLETE   char ch;
// OBSOLETE 
// OBSOLETE   while (1)
// OBSOLETE     {
// OBSOLETE       /* wait around for the start character, ignore all other characters */
// OBSOLETE       while ((ch = getDebugChar ()) != '$')
// OBSOLETE 	;
// OBSOLETE 
// OBSOLETE retry:
// OBSOLETE       checksum = 0;
// OBSOLETE       xmitcsum = -1;
// OBSOLETE       count = 0;
// OBSOLETE 
// OBSOLETE       /* now, read until a # or end of buffer is found */
// OBSOLETE       while (count < BUFMAX)
// OBSOLETE 	{
// OBSOLETE 	  ch = getDebugChar ();
// OBSOLETE           if (ch == '$')
// OBSOLETE 	    goto retry;
// OBSOLETE 	  if (ch == '#')
// OBSOLETE 	    break;
// OBSOLETE 	  checksum = checksum + ch;
// OBSOLETE 	  buffer[count] = ch;
// OBSOLETE 	  count = count + 1;
// OBSOLETE 	}
// OBSOLETE       buffer[count] = 0;
// OBSOLETE 
// OBSOLETE       if (ch == '#')
// OBSOLETE 	{
// OBSOLETE 	  ch = getDebugChar ();
// OBSOLETE 	  xmitcsum = hex (ch) << 4;
// OBSOLETE 	  ch = getDebugChar ();
// OBSOLETE 	  xmitcsum += hex (ch);
// OBSOLETE 
// OBSOLETE 	  if (checksum != xmitcsum)
// OBSOLETE 	    {
// OBSOLETE 	      if (remote_debug)
// OBSOLETE 		{
// OBSOLETE 		  unsigned char buf[16];
// OBSOLETE 
// OBSOLETE 		  mem2hex((unsigned char *) &checksum, buf, 4, 0);
// OBSOLETE 		  gdb_error("Bad checksum: my count = %s, ", buf);
// OBSOLETE 		  mem2hex((unsigned char *) &xmitcsum, buf, 4, 0);
// OBSOLETE 		  gdb_error("sent count = %s\n", buf);
// OBSOLETE 		  gdb_error(" -- Bad buffer: \"%s\"\n", buffer); 
// OBSOLETE 		}
// OBSOLETE 	      putDebugChar ('-');	/* failed checksum */
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      putDebugChar ('+');	/* successful transfer */
// OBSOLETE 
// OBSOLETE 	      /* if a sequence char is present, reply the sequence ID */
// OBSOLETE 	      if (buffer[2] == ':')
// OBSOLETE 		{
// OBSOLETE 		  putDebugChar (buffer[0]);
// OBSOLETE 		  putDebugChar (buffer[1]);
// OBSOLETE 
// OBSOLETE 		  return &buffer[3];
// OBSOLETE 		}
// OBSOLETE 
// OBSOLETE 	      return &buffer[0];
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* send the packet in buffer.  */
// OBSOLETE 
// OBSOLETE static void 
// OBSOLETE putpacket (unsigned char *buffer)
// OBSOLETE {
// OBSOLETE   unsigned char checksum;
// OBSOLETE   int  count;
// OBSOLETE   char ch;
// OBSOLETE 
// OBSOLETE   /*  $<packet info>#<checksum>. */
// OBSOLETE   do {
// OBSOLETE     putDebugChar('$');
// OBSOLETE     checksum = 0;
// OBSOLETE     count    = 0;
// OBSOLETE 
// OBSOLETE     while (ch=buffer[count]) {
// OBSOLETE       putDebugChar(ch);
// OBSOLETE       checksum += ch;
// OBSOLETE       count += 1;
// OBSOLETE     }
// OBSOLETE     putDebugChar('#');
// OBSOLETE     putDebugChar(hexchars[checksum >> 4]);
// OBSOLETE     putDebugChar(hexchars[checksum % 16]);
// OBSOLETE   } while (getDebugChar() != '+');
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Address of a routine to RTE to if we get a memory fault.  */
// OBSOLETE 
// OBSOLETE static void (*volatile mem_fault_routine)() = 0;
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE set_mem_err (void)
// OBSOLETE {
// OBSOLETE   mem_err = 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Check the address for safe access ranges.  As currently defined,
// OBSOLETE    this routine will reject the "expansion bus" address range(s).
// OBSOLETE    To make those ranges useable, someone must implement code to detect
// OBSOLETE    whether there's anything connected to the expansion bus. */
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE mem_safe (unsigned char *addr)
// OBSOLETE {
// OBSOLETE #define BAD_RANGE_ONE_START	((unsigned char *) 0x600000)
// OBSOLETE #define BAD_RANGE_ONE_END	((unsigned char *) 0xa00000)
// OBSOLETE #define BAD_RANGE_TWO_START	((unsigned char *) 0xff680000)
// OBSOLETE #define BAD_RANGE_TWO_END	((unsigned char *) 0xff800000)
// OBSOLETE 
// OBSOLETE   if (addr < BAD_RANGE_ONE_START)	return 1;	/* safe */
// OBSOLETE   if (addr < BAD_RANGE_ONE_END)		return 0;	/* unsafe */
// OBSOLETE   if (addr < BAD_RANGE_TWO_START)	return 1;	/* safe */
// OBSOLETE   if (addr < BAD_RANGE_TWO_END)		return 0;	/* unsafe */
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* These are separate functions so that they are so short and sweet
// OBSOLETE    that the compiler won't save any registers (if there is a fault
// OBSOLETE    to mem_fault, they won't get restored, so there better not be any
// OBSOLETE    saved).  */
// OBSOLETE static int
// OBSOLETE get_char (unsigned char *addr)
// OBSOLETE {
// OBSOLETE #if 1
// OBSOLETE   if (mem_fault_routine && !mem_safe(addr))
// OBSOLETE     {
// OBSOLETE       mem_fault_routine ();
// OBSOLETE       return 0;
// OBSOLETE     }
// OBSOLETE #endif
// OBSOLETE   return *addr;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE set_char (unsigned char *addr, unsigned char val)
// OBSOLETE {
// OBSOLETE #if 1
// OBSOLETE   if (mem_fault_routine && !mem_safe (addr))
// OBSOLETE     {
// OBSOLETE       mem_fault_routine ();
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE #endif
// OBSOLETE   *addr = val;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Convert the memory pointed to by mem into hex, placing result in buf.
// OBSOLETE    Return a pointer to the last char put in buf (null).
// OBSOLETE    If MAY_FAULT is non-zero, then we should set mem_err in response to
// OBSOLETE    a fault; if zero treat a fault like any other fault in the stub.  */
// OBSOLETE 
// OBSOLETE static unsigned char *
// OBSOLETE mem2hex (unsigned char *mem, unsigned char *buf, int count, int may_fault)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   unsigned char ch;
// OBSOLETE 
// OBSOLETE   if (may_fault)
// OBSOLETE     mem_fault_routine = set_mem_err;
// OBSOLETE   for (i=0;i<count;i++) {
// OBSOLETE     ch = get_char (mem++);
// OBSOLETE     if (may_fault && mem_err)
// OBSOLETE       return (buf);
// OBSOLETE     *buf++ = hexchars[ch >> 4];
// OBSOLETE     *buf++ = hexchars[ch % 16];
// OBSOLETE   }
// OBSOLETE   *buf = 0;
// OBSOLETE   if (may_fault)
// OBSOLETE     mem_fault_routine = 0;
// OBSOLETE   return(buf);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Convert the hex array pointed to by buf into binary to be placed in mem.
// OBSOLETE    Return a pointer to the character AFTER the last byte written. */
// OBSOLETE 
// OBSOLETE static unsigned char* 
// OBSOLETE hex2mem (unsigned char *buf, unsigned char *mem, int count, int may_fault)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   unsigned char ch;
// OBSOLETE 
// OBSOLETE   if (may_fault)
// OBSOLETE     mem_fault_routine = set_mem_err;
// OBSOLETE   for (i=0;i<count;i++) {
// OBSOLETE     ch = hex(*buf++) << 4;
// OBSOLETE     ch = ch + hex(*buf++);
// OBSOLETE     set_char (mem++, ch);
// OBSOLETE     if (may_fault && mem_err)
// OBSOLETE       return (mem);
// OBSOLETE   }
// OBSOLETE   if (may_fault)
// OBSOLETE     mem_fault_routine = 0;
// OBSOLETE   return(mem);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Convert the binary stream in BUF to memory.
// OBSOLETE 
// OBSOLETE    Gdb will escape $, #, and the escape char (0x7d).
// OBSOLETE    COUNT is the total number of bytes to write into
// OBSOLETE    memory. */
// OBSOLETE static unsigned char *
// OBSOLETE bin2mem (unsigned char *buf, unsigned char *mem, int count, int may_fault)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   unsigned char ch;
// OBSOLETE 
// OBSOLETE   if (may_fault)
// OBSOLETE     mem_fault_routine = set_mem_err;
// OBSOLETE   for (i = 0; i < count; i++)
// OBSOLETE     {
// OBSOLETE       /* Check for any escaped characters. Be paranoid and
// OBSOLETE          only unescape chars that should be escaped. */
// OBSOLETE       if (*buf == 0x7d)
// OBSOLETE         {
// OBSOLETE           switch (*(buf+1))
// OBSOLETE             {
// OBSOLETE             case 0x3:  /* # */
// OBSOLETE             case 0x4:  /* $ */
// OBSOLETE             case 0x5d: /* escape char */
// OBSOLETE               buf++;
// OBSOLETE               *buf |= 0x20;
// OBSOLETE               break;
// OBSOLETE             default:
// OBSOLETE               /* nothing */
// OBSOLETE               break;
// OBSOLETE             }
// OBSOLETE         }
// OBSOLETE 
// OBSOLETE       set_char (mem++, *buf++);
// OBSOLETE 
// OBSOLETE       if (may_fault && mem_err)
// OBSOLETE         return mem;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (may_fault)
// OBSOLETE     mem_fault_routine = 0;
// OBSOLETE   return mem;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* this function takes the m32r exception vector and attempts to
// OBSOLETE    translate this number into a unix compatible signal value */
// OBSOLETE 
// OBSOLETE static int 
// OBSOLETE computeSignal (int exceptionVector)
// OBSOLETE {
// OBSOLETE   int sigval;
// OBSOLETE   switch (exceptionVector) {
// OBSOLETE     case 0  : sigval = 23; break; /* I/O trap                    */
// OBSOLETE     case 1  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 2  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 3  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 4  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 5  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 6  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 7  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 8  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 9  : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 10 : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 11 : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 12 : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 13 : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 14 : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 15 : sigval = 5;  break; /* breakpoint                  */
// OBSOLETE     case 16 : sigval = 10; break; /* BUS ERROR (alignment)       */
// OBSOLETE     case 17 : sigval = 2;  break; /* INTerrupt                   */
// OBSOLETE     default : sigval = 7;  break; /* "software generated"        */
// OBSOLETE   }
// OBSOLETE   return (sigval);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /**********************************************/
// OBSOLETE /* WHILE WE FIND NICE HEX CHARS, BUILD AN INT */
// OBSOLETE /* RETURN NUMBER OF CHARS PROCESSED           */
// OBSOLETE /**********************************************/
// OBSOLETE static int 
// OBSOLETE hexToInt (unsigned char **ptr, int *intValue)
// OBSOLETE {
// OBSOLETE   int numChars = 0;
// OBSOLETE   int hexValue;
// OBSOLETE 
// OBSOLETE   *intValue = 0;
// OBSOLETE   while (**ptr)
// OBSOLETE     {
// OBSOLETE       hexValue = hex(**ptr);
// OBSOLETE       if (hexValue >=0)
// OBSOLETE         {
// OBSOLETE 	  *intValue = (*intValue <<4) | hexValue;
// OBSOLETE 	  numChars ++;
// OBSOLETE         }
// OBSOLETE       else
// OBSOLETE 	break;
// OBSOLETE       (*ptr)++;
// OBSOLETE     }
// OBSOLETE   return (numChars);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE   Table of branch instructions:
// OBSOLETE   
// OBSOLETE   10B6		RTE	return from trap or exception
// OBSOLETE   1FCr		JMP	jump
// OBSOLETE   1ECr		JL	jump and link
// OBSOLETE   7Fxx		BRA	branch
// OBSOLETE   FFxxxxxx	BRA	branch (long)
// OBSOLETE   B09rxxxx	BNEZ	branch not-equal-zero
// OBSOLETE   Br1rxxxx	BNE	branch not-equal
// OBSOLETE   7Dxx		BNC	branch not-condition
// OBSOLETE   FDxxxxxx	BNC	branch not-condition (long)
// OBSOLETE   B0Arxxxx	BLTZ	branch less-than-zero
// OBSOLETE   B0Crxxxx	BLEZ	branch less-equal-zero
// OBSOLETE   7Exx		BL	branch and link
// OBSOLETE   FExxxxxx	BL	branch and link (long)
// OBSOLETE   B0Drxxxx	BGTZ	branch greater-than-zero
// OBSOLETE   B0Brxxxx	BGEZ	branch greater-equal-zero
// OBSOLETE   B08rxxxx	BEQZ	branch equal-zero
// OBSOLETE   Br0rxxxx	BEQ	branch equal
// OBSOLETE   7Cxx		BC	branch condition
// OBSOLETE   FCxxxxxx	BC	branch condition (long)
// OBSOLETE   */
// OBSOLETE 
// OBSOLETE static int 
// OBSOLETE isShortBranch (unsigned char *instr)
// OBSOLETE {
// OBSOLETE   unsigned char instr0 = instr[0] & 0x7F;		/* mask off high bit */
// OBSOLETE 
// OBSOLETE   if (instr0 == 0x10 && instr[1] == 0xB6)	/* RTE */
// OBSOLETE     return 1;		/* return from trap or exception */
// OBSOLETE 
// OBSOLETE   if (instr0 == 0x1E || instr0 == 0x1F)		/* JL or JMP */
// OBSOLETE     if ((instr[1] & 0xF0) == 0xC0)
// OBSOLETE       return 2;					/* jump thru a register */
// OBSOLETE 
// OBSOLETE   if (instr0 == 0x7C || instr0 == 0x7D || 	/* BC, BNC, BL, BRA */
// OBSOLETE       instr0 == 0x7E || instr0 == 0x7F)
// OBSOLETE     return 3;					/* eight bit PC offset */
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE isLongBranch (unsigned char *instr)
// OBSOLETE {
// OBSOLETE   if (instr[0] == 0xFC || instr[0] == 0xFD ||	/* BRA, BNC, BL, BC */
// OBSOLETE       instr[0] == 0xFE || instr[0] == 0xFF)	/* 24 bit relative */
// OBSOLETE     return 4;
// OBSOLETE   if ((instr[0] & 0xF0) == 0xB0)		/* 16 bit relative */
// OBSOLETE     {
// OBSOLETE       if ((instr[1] & 0xF0) == 0x00 || 		/* BNE, BEQ */
// OBSOLETE 	  (instr[1] & 0xF0) == 0x10)
// OBSOLETE 	return 5;
// OBSOLETE       if (instr[0] == 0xB0)	/* BNEZ, BLTZ, BLEZ, BGTZ, BGEZ, BEQZ */
// OBSOLETE 	if ((instr[1] & 0xF0) == 0x80 || (instr[1] & 0xF0) == 0x90 || 
// OBSOLETE 	    (instr[1] & 0xF0) == 0xA0 || (instr[1] & 0xF0) == 0xB0 ||
// OBSOLETE 	    (instr[1] & 0xF0) == 0xC0 || (instr[1] & 0xF0) == 0xD0)
// OBSOLETE 	  return 6;
// OBSOLETE     }
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* if address is NOT on a 4-byte boundary, or high-bit of instr is zero, 
// OBSOLETE    then it's a 2-byte instruction, else it's a 4-byte instruction.  */
// OBSOLETE 
// OBSOLETE #define INSTRUCTION_SIZE(addr) \
// OBSOLETE     ((((int) addr & 2) || (((unsigned char *) addr)[0] & 0x80) == 0) ? 2 : 4)
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE isBranch (unsigned char *instr)
// OBSOLETE {
// OBSOLETE   if (INSTRUCTION_SIZE(instr) == 2)
// OBSOLETE     return isShortBranch(instr);
// OBSOLETE   else
// OBSOLETE     return isLongBranch(instr);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE willBranch (unsigned char *instr, int branchCode)
// OBSOLETE {
// OBSOLETE   switch (branchCode) 
// OBSOLETE     {
// OBSOLETE     case 0:	return 0;	/* not a branch */
// OBSOLETE     case 1:	return 1;	/* RTE */
// OBSOLETE     case 2:	return 1;	/* JL or JMP    */
// OBSOLETE     case 3:			/* BC, BNC, BL, BRA (short) */
// OBSOLETE     case 4:			/* BC, BNC, BL, BRA (long) */
// OBSOLETE       switch (instr[0] & 0x0F) 
// OBSOLETE 	{
// OBSOLETE 	case 0xC:		/* Branch if Condition Register */
// OBSOLETE 	  return (registers[CBR] != 0);
// OBSOLETE 	case 0xD:		/* Branch if NOT Condition Register */
// OBSOLETE 	  return (registers[CBR] == 0);
// OBSOLETE 	case 0xE:		/* Branch and Link */
// OBSOLETE 	case 0xF:		/* Branch (unconditional) */
// OBSOLETE 	  return 1;
// OBSOLETE 	default:		/* oops? */
// OBSOLETE 	  return 0;
// OBSOLETE 	}
// OBSOLETE     case 5: 			/* BNE, BEQ */
// OBSOLETE       switch (instr[1] & 0xF0) 
// OBSOLETE 	{
// OBSOLETE 	case 0x00:		/* Branch if r1 equal to r2 */
// OBSOLETE 	  return (registers[instr[0] & 0x0F] == registers[instr[1] & 0x0F]);
// OBSOLETE 	case 0x10:		/* Branch if r1 NOT equal to r2 */
// OBSOLETE 	  return (registers[instr[0] & 0x0F] != registers[instr[1] & 0x0F]);
// OBSOLETE 	default:		/* oops? */
// OBSOLETE 	  return 0;
// OBSOLETE 	}
// OBSOLETE     case 6: 			/* BNEZ, BLTZ, BLEZ, BGTZ, BGEZ ,BEQZ */
// OBSOLETE       switch (instr[1] & 0xF0) 
// OBSOLETE 	{
// OBSOLETE 	case 0x80:		/* Branch if reg equal to zero */
// OBSOLETE 	  return (registers[instr[1] & 0x0F] == 0);
// OBSOLETE 	case 0x90:		/* Branch if reg NOT equal to zero */
// OBSOLETE 	  return (registers[instr[1] & 0x0F] != 0);
// OBSOLETE 	case 0xA0:		/* Branch if reg less than zero */
// OBSOLETE 	  return (registers[instr[1] & 0x0F] < 0);
// OBSOLETE 	case 0xB0:		/* Branch if reg greater or equal to zero */
// OBSOLETE 	  return (registers[instr[1] & 0x0F] >= 0);
// OBSOLETE 	case 0xC0:		/* Branch if reg less than or equal to zero */
// OBSOLETE 	  return (registers[instr[1] & 0x0F] <= 0);
// OBSOLETE 	case 0xD0:		/* Branch if reg greater than zero */
// OBSOLETE 	  return (registers[instr[1] & 0x0F] > 0);
// OBSOLETE 	default:		/* oops? */
// OBSOLETE 	  return 0;
// OBSOLETE 	}
// OBSOLETE     default:			/* oops? */
// OBSOLETE       return 0;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int 
// OBSOLETE branchDestination (unsigned char *instr, int branchCode)
// OBSOLETE { 
// OBSOLETE   switch (branchCode) { 
// OBSOLETE   default: 
// OBSOLETE   case 0:					/* not a branch */ 
// OBSOLETE     return 0;
// OBSOLETE   case 1:					/* RTE */ 
// OBSOLETE     return registers[BPC] & ~3; 		/* pop BPC into PC */
// OBSOLETE   case 2: 					/* JL or JMP */ 
// OBSOLETE     return registers[instr[1] & 0x0F] & ~3;	/* jump thru a register */ 
// OBSOLETE   case 3: 		/* BC, BNC, BL, BRA (short, 8-bit relative offset) */ 
// OBSOLETE     return (((int) instr) & ~3) + ((char) instr[1] << 2);
// OBSOLETE   case 4: 		/* BC, BNC, BL, BRA (long, 24-bit relative offset) */ 
// OBSOLETE     return ((int) instr + 
// OBSOLETE 	    ((((char) instr[1] << 16) | (instr[2] << 8) | (instr[3])) << 2)); 
// OBSOLETE   case 5: 		/* BNE, BEQ (16-bit relative offset) */ 
// OBSOLETE   case 6: 		/* BNEZ, BLTZ, BLEZ, BGTZ, BGEZ ,BEQZ (ditto) */ 
// OBSOLETE     return ((int) instr + ((((char) instr[2] << 8) | (instr[3])) << 2)); 
// OBSOLETE   }
// OBSOLETE 
// OBSOLETE   /* An explanatory note: in the last three return expressions, I have
// OBSOLETE      cast the most-significant byte of the return offset to char.
// OBSOLETE      What this accomplishes is sign extension.  If the other
// OBSOLETE      less-significant bytes were signed as well, they would get sign
// OBSOLETE      extended too and, if negative, their leading bits would clobber
// OBSOLETE      the bits of the more-significant bytes ahead of them.  There are
// OBSOLETE      other ways I could have done this, but sign extension from
// OBSOLETE      odd-sized integers is always a pain. */
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE branchSideEffects (unsigned char *instr, int branchCode)
// OBSOLETE {
// OBSOLETE   switch (branchCode)
// OBSOLETE     {
// OBSOLETE     case 1:			/* RTE */
// OBSOLETE       return;			/* I <THINK> this is already handled... */
// OBSOLETE     case 2:			/* JL (or JMP) */
// OBSOLETE     case 3:			/* BL (or BC, BNC, BRA) */
// OBSOLETE     case 4:
// OBSOLETE       if ((instr[0] & 0x0F) == 0x0E)		/* branch/jump and link */
// OBSOLETE 	registers[R14] = (registers[PC] & ~3) + 4;
// OBSOLETE       return;
// OBSOLETE     default:			/* any other branch has no side effects */
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static struct STEPPING_CONTEXT {
// OBSOLETE   int stepping;			/* true when we've started a single-step */
// OBSOLETE   unsigned long  target_addr;	/* the instr we're trying to execute */
// OBSOLETE   unsigned long  target_size;	/* the size of the target instr */
// OBSOLETE   unsigned long  noop_addr;	/* where we've inserted a no-op, if any */
// OBSOLETE   unsigned long  trap1_addr;	/* the trap following the target instr */
// OBSOLETE   unsigned long  trap2_addr;	/* the trap at a branch destination, if any */
// OBSOLETE   unsigned short noop_save;	/* instruction overwritten by our no-op */
// OBSOLETE   unsigned short trap1_save;	/* instruction overwritten by trap1 */
// OBSOLETE   unsigned short trap2_save;	/* instruction overwritten by trap2 */
// OBSOLETE   unsigned short continue_p;	/* true if NOT returning to gdb after step */
// OBSOLETE } stepping;
// OBSOLETE 
// OBSOLETE /* Function: prepare_to_step
// OBSOLETE    Called from handle_exception to prepare the user program to single-step.
// OBSOLETE    Places a trap instruction after the target instruction, with special 
// OBSOLETE    extra handling for branch instructions and for instructions in the 
// OBSOLETE    second half-word of a word.  
// OBSOLETE 
// OBSOLETE    Returns: True  if we should actually execute the instruction; 
// OBSOLETE 	    False if we are going to emulate executing the instruction,
// OBSOLETE 	    in which case we simply report to GDB that the instruction 
// OBSOLETE 	    has already been executed.  */
// OBSOLETE 
// OBSOLETE #define TRAP1  0x10f1;	/* trap #1 instruction */
// OBSOLETE #define NOOP   0x7000;  /* noop    instruction */
// OBSOLETE 
// OBSOLETE static unsigned short trap1 = TRAP1;
// OBSOLETE static unsigned short noop  = NOOP;
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE prepare_to_step(continue_p)
// OBSOLETE      int continue_p;	/* if this isn't REALLY a single-step (see below) */
// OBSOLETE {
// OBSOLETE   unsigned long pc = registers[PC];
// OBSOLETE   int branchCode   = isBranch((unsigned char *) pc);
// OBSOLETE   unsigned char *p;
// OBSOLETE 
// OBSOLETE   /* zero out the stepping context 
// OBSOLETE      (paranoia -- it should already be zeroed) */
// OBSOLETE   for (p = (unsigned char *) &stepping;
// OBSOLETE        p < ((unsigned char *) &stepping) + sizeof(stepping);
// OBSOLETE        p++)
// OBSOLETE     *p = 0;
// OBSOLETE 
// OBSOLETE   if (branchCode != 0)			/* next instruction is a branch */
// OBSOLETE     {
// OBSOLETE       branchSideEffects((unsigned char *) pc, branchCode);
// OBSOLETE       if (willBranch((unsigned char *)pc, branchCode))
// OBSOLETE 	registers[PC] = branchDestination((unsigned char *) pc, branchCode);
// OBSOLETE       else
// OBSOLETE 	registers[PC] = pc + INSTRUCTION_SIZE(pc);
// OBSOLETE       return 0;			/* branch "executed" -- just notify GDB */
// OBSOLETE     }
// OBSOLETE   else if (((int) pc & 2) != 0)		/* "second-slot" instruction */
// OBSOLETE     {
// OBSOLETE       /* insert no-op before pc */
// OBSOLETE       stepping.noop_addr  =  pc - 2;
// OBSOLETE       stepping.noop_save  = *(unsigned short *) stepping.noop_addr;
// OBSOLETE       *(unsigned short *) stepping.noop_addr  = noop;
// OBSOLETE       /* insert trap  after  pc */
// OBSOLETE       stepping.trap1_addr =  pc + 2;
// OBSOLETE       stepping.trap1_save = *(unsigned short *) stepping.trap1_addr;
// OBSOLETE       *(unsigned short *) stepping.trap1_addr = trap1;
// OBSOLETE     }
// OBSOLETE   else					/* "first-slot" instruction */
// OBSOLETE     {
// OBSOLETE       /* insert trap  after  pc */
// OBSOLETE       stepping.trap1_addr = pc + INSTRUCTION_SIZE(pc);	
// OBSOLETE       stepping.trap1_save = *(unsigned short *) stepping.trap1_addr;
// OBSOLETE       *(unsigned short *) stepping.trap1_addr = trap1;
// OBSOLETE     }
// OBSOLETE   /* "continue_p" means that we are actually doing a continue, and not 
// OBSOLETE      being requested to single-step by GDB.  Sometimes we have to do
// OBSOLETE      one single-step before continuing, because the PC is on a half-word
// OBSOLETE      boundary.  There's no way to simply resume at such an address.  */
// OBSOLETE   stepping.continue_p = continue_p;
// OBSOLETE   stepping.stepping = 1;		/* starting a single-step */
// OBSOLETE   return 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: finish_from_step
// OBSOLETE    Called from handle_exception to finish up when the user program 
// OBSOLETE    returns from a single-step.  Replaces the instructions that had
// OBSOLETE    been overwritten by traps or no-ops, 
// OBSOLETE 
// OBSOLETE    Returns: True  if we should notify GDB that the target stopped.
// OBSOLETE 	    False if we only single-stepped because we had to before we
// OBSOLETE 	    could continue (ie. we were trying to continue at a 
// OBSOLETE 	    half-word boundary).  In that case don't notify GDB:
// OBSOLETE 	    just "continue continuing".  */
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE finish_from_step (void)
// OBSOLETE {
// OBSOLETE   if (stepping.stepping)	/* anything to do? */
// OBSOLETE     {
// OBSOLETE       int continue_p = stepping.continue_p;
// OBSOLETE       unsigned char *p;
// OBSOLETE 
// OBSOLETE       if (stepping.noop_addr)	/* replace instr "under" our no-op */
// OBSOLETE 	*(unsigned short *) stepping.noop_addr  = stepping.noop_save;
// OBSOLETE       if (stepping.trap1_addr)	/* replace instr "under" our trap  */
// OBSOLETE 	*(unsigned short *) stepping.trap1_addr = stepping.trap1_save;
// OBSOLETE       if (stepping.trap2_addr)  /* ditto our other trap, if any    */
// OBSOLETE 	*(unsigned short *) stepping.trap2_addr = stepping.trap2_save;
// OBSOLETE 
// OBSOLETE       for (p = (unsigned char *) &stepping;	/* zero out the stepping context */
// OBSOLETE 	   p < ((unsigned char *) &stepping) + sizeof(stepping);
// OBSOLETE 	   p++)
// OBSOLETE 	*p = 0;
// OBSOLETE 
// OBSOLETE       return !(continue_p);
// OBSOLETE     }
// OBSOLETE   else 	/* we didn't single-step, therefore this must be a legitimate stop */
// OBSOLETE     return 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE struct PSWreg {		/* separate out the bit flags in the PSW register */
// OBSOLETE   int pad1 : 16;
// OBSOLETE   int bsm  : 1;
// OBSOLETE   int bie  : 1;
// OBSOLETE   int pad2 : 5;
// OBSOLETE   int bc   : 1;
// OBSOLETE   int sm   : 1;
// OBSOLETE   int ie   : 1;
// OBSOLETE   int pad3 : 5;
// OBSOLETE   int c    : 1;
// OBSOLETE } *psw;
// OBSOLETE 
// OBSOLETE /* Upon entry the value for LR to save has been pushed.
// OBSOLETE    We unpush that so that the value for the stack pointer saved is correct.
// OBSOLETE    Upon entry, all other registers are assumed to have not been modified
// OBSOLETE    since the interrupt/trap occured.  */
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE stash_registers:
// OBSOLETE 	push r0
// OBSOLETE 	push r1
// OBSOLETE 	seth r1, #shigh(registers)
// OBSOLETE 	add3 r1, r1, #low(registers)
// OBSOLETE 	pop r0		; r1
// OBSOLETE 	st r0, @(4,r1)
// OBSOLETE 	pop r0		; r0
// OBSOLETE 	st r0, @r1
// OBSOLETE 	addi r1, #4	; only add 4 as subsequent saves are `pre inc'
// OBSOLETE 	st r2, @+r1
// OBSOLETE 	st r3, @+r1
// OBSOLETE 	st r4, @+r1
// OBSOLETE 	st r5, @+r1
// OBSOLETE 	st r6, @+r1
// OBSOLETE 	st r7, @+r1
// OBSOLETE 	st r8, @+r1
// OBSOLETE 	st r9, @+r1
// OBSOLETE 	st r10, @+r1
// OBSOLETE 	st r11, @+r1
// OBSOLETE 	st r12, @+r1
// OBSOLETE 	st r13, @+r1    ; fp
// OBSOLETE 	pop r0		; lr (r14)
// OBSOLETE 	st r0, @+r1
// OBSOLETE 	st sp, @+r1	; sp contains right value at this point
// OBSOLETE 	mvfc r0, cr0
// OBSOLETE 	st r0, @+r1	; cr0 == PSW
// OBSOLETE 	mvfc r0, cr1
// OBSOLETE 	st r0, @+r1	; cr1 == CBR
// OBSOLETE 	mvfc r0, cr2
// OBSOLETE 	st r0, @+r1	; cr2 == SPI
// OBSOLETE 	mvfc r0, cr3
// OBSOLETE 	st r0, @+r1	; cr3 == SPU
// OBSOLETE 	mvfc r0, cr6
// OBSOLETE 	st r0, @+r1	; cr6 == BPC
// OBSOLETE 	st r0, @+r1	; PC  == BPC
// OBSOLETE 	mvfaclo r0
// OBSOLETE 	st r0, @+r1	; ACCL
// OBSOLETE 	mvfachi r0
// OBSOLETE 	st r0, @+r1	; ACCH
// OBSOLETE 	jmp lr");
// OBSOLETE 
// OBSOLETE /* C routine to clean up what stash_registers did.
// OBSOLETE    It is called after calling stash_registers.
// OBSOLETE    This is separate from stash_registers as we want to do this in C
// OBSOLETE    but doing stash_registers in C isn't straightforward.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE cleanup_stash (void)
// OBSOLETE {
// OBSOLETE   psw = (struct PSWreg *) &registers[PSW];	/* fields of PSW register */
// OBSOLETE   psw->sm = psw->bsm;		/* fix up pre-trap values of psw fields */
// OBSOLETE   psw->ie = psw->bie;
// OBSOLETE   psw->c  = psw->bc;
// OBSOLETE   registers[CBR] = psw->bc;		/* fix up pre-trap "C" register */
// OBSOLETE 
// OBSOLETE #if 0 /* FIXME: Was in previous version.  Necessary?
// OBSOLETE 	 (Remember that we use the "rte" insn to return from the
// OBSOLETE 	 trap/interrupt so the values of bsm, bie, bc are important.  */
// OBSOLETE   psw->bsm = psw->bie = psw->bc = 0;	/* zero post-trap values */
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE   /* FIXME: Copied from previous version.  This can probably be deleted
// OBSOLETE      since methinks stash_registers has already done this.  */
// OBSOLETE   registers[PC] = registers[BPC];	/* pre-trap PC */
// OBSOLETE 
// OBSOLETE   /* FIXME: Copied from previous version.  Necessary?  */
// OBSOLETE   if (psw->sm)			/* copy R15 into (psw->sm ? SPU : SPI) */
// OBSOLETE     registers[SPU] = registers[R15];
// OBSOLETE   else
// OBSOLETE     registers[SPI] = registers[R15];
// OBSOLETE }
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE restore_and_return:
// OBSOLETE 	seth r0, #shigh(registers+8)
// OBSOLETE 	add3 r0, r0, #low(registers+8)
// OBSOLETE 	ld r2, @r0+	; restore r2
// OBSOLETE 	ld r3, @r0+	; restore r3
// OBSOLETE 	ld r4, @r0+	; restore r4
// OBSOLETE 	ld r5, @r0+	; restore r5
// OBSOLETE 	ld r6, @r0+	; restore r6
// OBSOLETE 	ld r7, @r0+	; restore r7
// OBSOLETE 	ld r8, @r0+	; restore r8
// OBSOLETE 	ld r9, @r0+	; restore r9
// OBSOLETE 	ld r10, @r0+	; restore r10
// OBSOLETE 	ld r11, @r0+	; restore r11
// OBSOLETE 	ld r12, @r0+	; restore r12
// OBSOLETE 	ld r13, @r0+	; restore r13
// OBSOLETE 	ld r14, @r0+	; restore r14
// OBSOLETE 	ld r15, @r0+	; restore r15
// OBSOLETE 	addi r0, #4	; don't restore PSW (rte will do it)
// OBSOLETE 	ld r1, @r0+	; restore cr1 == CBR (no-op, because it's read only)
// OBSOLETE 	mvtc r1, cr1
// OBSOLETE 	ld r1, @r0+	; restore cr2 == SPI
// OBSOLETE 	mvtc r1, cr2
// OBSOLETE 	ld r1, @r0+	; restore cr3 == SPU
// OBSOLETE 	mvtc r1, cr3
// OBSOLETE 	addi r0, #4	; skip BPC
// OBSOLETE 	ld r1, @r0+	; restore cr6 (BPC) == PC
// OBSOLETE 	mvtc r1, cr6
// OBSOLETE 	ld r1, @r0+	; restore ACCL
// OBSOLETE 	mvtaclo r1
// OBSOLETE 	ld r1, @r0+	; restore ACCH
// OBSOLETE 	mvtachi r1
// OBSOLETE 	seth r0, #shigh(registers)
// OBSOLETE 	add3 r0, r0, #low(registers)
// OBSOLETE 	ld r1, @(4,r0)	; restore r1
// OBSOLETE 	ld r0, @r0	; restore r0
// OBSOLETE 	rte");
// OBSOLETE 
// OBSOLETE /* General trap handler, called after the registers have been stashed.
// OBSOLETE    NUM is the trap/exception number.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE process_exception (int num)
// OBSOLETE {
// OBSOLETE   cleanup_stash ();
// OBSOLETE   asm volatile ("
// OBSOLETE 	seth r1, #shigh(stackPtr)
// OBSOLETE 	add3 r1, r1, #low(stackPtr)
// OBSOLETE 	ld r15, @r1		; setup local stack (protect user stack)
// OBSOLETE 	mv r0, %0
// OBSOLETE 	bl handle_exception
// OBSOLETE 	bl restore_and_return"
// OBSOLETE 		: : "r" (num) : "r0", "r1");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void _catchException0 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException0:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #0
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException1 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException1:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	bl cleanup_stash
// OBSOLETE 	seth r1, #shigh(stackPtr)
// OBSOLETE 	add3 r1, r1, #low(stackPtr)
// OBSOLETE 	ld r15, @r1		; setup local stack (protect user stack)
// OBSOLETE 	seth r1, #shigh(registers + 21*4) ; PC
// OBSOLETE 	add3 r1, r1, #low(registers + 21*4)
// OBSOLETE 	ld r0, @r1
// OBSOLETE 	addi r0, #-4		; back up PC for breakpoint trap.
// OBSOLETE 	st r0, @r1		; FIXME: what about bp in right slot?
// OBSOLETE 	ldi r0, #1
// OBSOLETE 	bl handle_exception
// OBSOLETE 	bl restore_and_return");
// OBSOLETE 
// OBSOLETE void _catchException2 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException2:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #2
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException3 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException3:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #3
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException4 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException4:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #4
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException5 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException5:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #5
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException6 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException6:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #6
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException7 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException7:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #7
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException8 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException8:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #8
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException9 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException9:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #9
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException10 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException10:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #10
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException11 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException11:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #11
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException12 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException12:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #12
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException13 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException13:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #13
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException14 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException14:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #14
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException15 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException15:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #15
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException16 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException16:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #16
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE void _catchException17 ();
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE _catchException17:
// OBSOLETE 	push lr
// OBSOLETE 	bl stash_registers
// OBSOLETE 	; Note that at this point the pushed value of `lr' has been popped
// OBSOLETE 	ldi r0, #17
// OBSOLETE 	bl process_exception");
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* this function is used to set up exception handlers for tracing and
// OBSOLETE    breakpoints */
// OBSOLETE void 
// OBSOLETE set_debug_traps (void)
// OBSOLETE {
// OBSOLETE   /*  extern void remcomHandler(); */
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   for (i = 0; i < 18; i++)		/* keep a copy of old vectors */
// OBSOLETE     if (save_vectors[i] == 0)		/* only copy them the first time */
// OBSOLETE       save_vectors[i] = getExceptionHandler (i);
// OBSOLETE 
// OBSOLETE   stackPtr  = &remcomStack[STACKSIZE/sizeof(int) - 1];
// OBSOLETE 
// OBSOLETE   exceptionHandler (0, _catchException0);
// OBSOLETE   exceptionHandler (1, _catchException1);
// OBSOLETE   exceptionHandler (2, _catchException2);
// OBSOLETE   exceptionHandler (3, _catchException3);
// OBSOLETE   exceptionHandler (4, _catchException4);
// OBSOLETE   exceptionHandler (5, _catchException5);
// OBSOLETE   exceptionHandler (6, _catchException6);
// OBSOLETE   exceptionHandler (7, _catchException7);
// OBSOLETE   exceptionHandler (8, _catchException8);
// OBSOLETE   exceptionHandler (9, _catchException9);
// OBSOLETE   exceptionHandler (10, _catchException10);
// OBSOLETE   exceptionHandler (11, _catchException11);
// OBSOLETE   exceptionHandler (12, _catchException12);
// OBSOLETE   exceptionHandler (13, _catchException13);
// OBSOLETE   exceptionHandler (14, _catchException14);
// OBSOLETE   exceptionHandler (15, _catchException15);
// OBSOLETE   exceptionHandler (16, _catchException16);
// OBSOLETE   /*  exceptionHandler (17, _catchException17); */
// OBSOLETE 
// OBSOLETE   initialized = 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* This function will generate a breakpoint exception.  It is used at the
// OBSOLETE    beginning of a program to sync up with a debugger and can be used
// OBSOLETE    otherwise as a quick means to stop program execution and "break" into
// OBSOLETE    the debugger. */
// OBSOLETE 
// OBSOLETE #define BREAKPOINT() asm volatile ("	trap #2");
// OBSOLETE 
// OBSOLETE void 
// OBSOLETE breakpoint (void)
// OBSOLETE {
// OBSOLETE   if (initialized)
// OBSOLETE     BREAKPOINT();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* STDOUT section:
// OBSOLETE    Stuff pertaining to simulating stdout by sending chars to gdb to be echoed.
// OBSOLETE    Functions: gdb_putchar(char ch)
// OBSOLETE               gdb_puts(char *str)
// OBSOLETE               gdb_write(char *str, int len)
// OBSOLETE               gdb_error(char *format, char *parm)
// OBSOLETE 	      */
// OBSOLETE  
// OBSOLETE /* Function: gdb_putchar(int)
// OBSOLETE    Make gdb write a char to stdout.
// OBSOLETE    Returns: the char */
// OBSOLETE  
// OBSOLETE static int
// OBSOLETE gdb_putchar (int ch)
// OBSOLETE {
// OBSOLETE   char buf[4];
// OBSOLETE  
// OBSOLETE   buf[0] = 'O';
// OBSOLETE   buf[1] = hexchars[ch >> 4];
// OBSOLETE   buf[2] = hexchars[ch & 0x0F];
// OBSOLETE   buf[3] = 0;
// OBSOLETE   putpacket(buf);
// OBSOLETE   return ch;
// OBSOLETE }
// OBSOLETE  
// OBSOLETE /* Function: gdb_write(char *, int)
// OBSOLETE    Make gdb write n bytes to stdout (not assumed to be null-terminated).
// OBSOLETE    Returns: number of bytes written */
// OBSOLETE  
// OBSOLETE static int
// OBSOLETE gdb_write (char *data, int len)
// OBSOLETE {
// OBSOLETE   char *buf, *cpy;
// OBSOLETE   int i;
// OBSOLETE  
// OBSOLETE   buf = remcomOutBuffer;
// OBSOLETE   buf[0] = 'O';
// OBSOLETE   i = 0;
// OBSOLETE   while (i < len)
// OBSOLETE     {
// OBSOLETE       for (cpy = buf+1; 
// OBSOLETE 	   i < len && cpy < buf + sizeof(remcomOutBuffer) - 3; 
// OBSOLETE 	   i++)
// OBSOLETE 	{
// OBSOLETE 	  *cpy++ = hexchars[data[i] >> 4];
// OBSOLETE 	  *cpy++ = hexchars[data[i] & 0x0F];
// OBSOLETE 	}
// OBSOLETE       *cpy = 0;
// OBSOLETE       putpacket(buf);
// OBSOLETE     }
// OBSOLETE   return len;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: gdb_puts(char *)
// OBSOLETE    Make gdb write a null-terminated string to stdout.
// OBSOLETE    Returns: the length of the string */
// OBSOLETE  
// OBSOLETE static int
// OBSOLETE gdb_puts (char *str)
// OBSOLETE {
// OBSOLETE   return gdb_write(str, strlen(str));
// OBSOLETE }
// OBSOLETE  
// OBSOLETE /* Function: gdb_error(char *, char *)
// OBSOLETE    Send an error message to gdb's stdout.
// OBSOLETE    First string may have 1 (one) optional "%s" in it, which
// OBSOLETE    will cause the optional second string to be inserted.  */
// OBSOLETE  
// OBSOLETE static void
// OBSOLETE gdb_error (char *format, char *parm)
// OBSOLETE {
// OBSOLETE   char buf[400], *cpy;
// OBSOLETE   int len;
// OBSOLETE  
// OBSOLETE   if (remote_debug)
// OBSOLETE     {
// OBSOLETE       if (format && *format)
// OBSOLETE 	len = strlen(format);
// OBSOLETE       else
// OBSOLETE 	return;             /* empty input */
// OBSOLETE 
// OBSOLETE       if (parm && *parm)
// OBSOLETE 	len += strlen(parm);
// OBSOLETE  
// OBSOLETE       for (cpy = buf; *format; )
// OBSOLETE 	{
// OBSOLETE 	  if (format[0] == '%' && format[1] == 's') /* include second string */
// OBSOLETE 	    {
// OBSOLETE 	      format += 2;          /* advance two chars instead of just one */
// OBSOLETE 	      while (parm && *parm)
// OBSOLETE 		*cpy++ = *parm++;
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    *cpy++ = *format++;
// OBSOLETE 	}
// OBSOLETE       *cpy = '\0';
// OBSOLETE       gdb_puts(buf);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE  
// OBSOLETE static unsigned char *
// OBSOLETE strcpy (unsigned char *dest, const unsigned char *src)
// OBSOLETE {
// OBSOLETE   unsigned char *ret = dest;
// OBSOLETE 
// OBSOLETE   if (dest && src)
// OBSOLETE     {
// OBSOLETE       while (*src)
// OBSOLETE 	*dest++ = *src++;
// OBSOLETE       *dest = 0;
// OBSOLETE     }
// OBSOLETE   return ret;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE strlen (const unsigned char *src)
// OBSOLETE {
// OBSOLETE   int ret;
// OBSOLETE 
// OBSOLETE   for (ret = 0; *src; src++)
// OBSOLETE     ret++;
// OBSOLETE 
// OBSOLETE   return ret;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE void exit (code)
// OBSOLETE      int code;
// OBSOLETE {
// OBSOLETE   _exit (code);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int atexit (void *p)
// OBSOLETE {
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void abort (void)
// OBSOLETE {
// OBSOLETE   _exit (1);
// OBSOLETE }
// OBSOLETE #endif
