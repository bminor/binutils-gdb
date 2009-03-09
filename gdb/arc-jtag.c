/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Sameer Dhavale <sameer.dhavale@codito.com>
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com> 

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "inferior.h"
#include "target.h"
#include "breakpoint.h"

#include "arc-tdep.h"

#include <unistd.h>
#include <stdlib.h>

#include <sys/io.h>
#include <sys/types.h>

#include <assert.h>

#include "arc-jtag.h"
#include "arc-jtag-ops.h"
#include "gdbcore.h"
#include "gdbarch.h"
#include "regcache.h"
#include "command.h"
#include "gdbcmd.h"
#include <signal.h>

/* Flag to print debug messages from here. */
/* FIXMEA:
static int debug_arc_jtag_target_message;
*/

#define ENTERMSG  \
do {\
if(debug_arc_jtag_target_message) \
printf_filtered ("--- entered %s:%s()\n", __FILE__, __FUNCTION__); \
} while(0)

#define ENTERARGS(fmt, args...) \
do { \
if(debug_arc_jtag_target_message) \
printf_filtered ("--- entered %s:%s(" fmt ")\n", __FILE__, __FUNCTION__, args);\
} while(0)

#define LEAVEMSG \
do { \
if(debug_arc_jtag_target_message) \
printf_filtered ("--- exited %s:%s()\n", __FILE__, __FUNCTION__); \
} while(0)

/* The number of times a memory read/write operation should be attempted
   before returning an error.
*/
#define MEMORY_TRANSFER_ATTEMPTS 10

/* defined in arc-jtag-ops.c */
extern unsigned int arcjtag_retry_count;

struct target_ops arc_debug_ops;
extern struct jtag_ops arc_jtag_ops;

static void arc_debug_interrupt (int signo);
static void arc_debug_interrupt_twice (int signo);
static void arc_print_processor_variant_info (void);
static int arc_debug_write_aux_register (int hwregno, int *buf);
static int arc_debug_read_aux_register (int hwregno, int *buf);
static int arc_debug_read_core_register (int hwregno, int *buf);


/* Register Mapping information between GDB regnums
   and actual hardware register numbers. 
*/

struct arc_reg_info 
{
  char *name ;
  enum arc_hw_regnums hw_regno;
  char *description;
#ifdef ARC4_JTAG
  enum arc4_jtag_regnums gdbregno;
#else
  enum arc700_jtag_regnums gdbregno;
#endif
  enum ARCProcessorVersion arcVersionSupported;
};

#define RBCR(name, hwregno , desc, gdbregno, version) { #name, hwregno , desc , gdbregno , version } ,

#undef RAUX
struct arc_reg_info arc_bcr_reg_info [] = 
  {
   #include "arc-regnums-defs.h"
  };

#undef RAUX
#undef RBCR
#define RAUX(name, hwregno , desc, gdbregno, version) { #name , hwregno , desc , gdbregno , version } ,
struct arc_reg_info arc_aux_reg_map[] =
  {
    #include "arc-regnums-defs.h"
  };



static void 
arc_update_architecture(void)
{
  unsigned int idinfo;
  unsigned short tmp;
  struct gdbarch_tdep * tdep = gdbarch_tdep (current_gdbarch);

  if (IS_ARC700) {
    tdep->arc_processor_variant_info->arcprocessorversion = ARC700;
    set_gdbarch_decr_pc_after_break (current_gdbarch,0);
  }
  else if(IS_ARC600) {
    tdep->arc_processor_variant_info->arcprocessorversion = ARC600;
    set_gdbarch_decr_pc_after_break (current_gdbarch,2);
  }
  else if(IS_A5) {
    warning ("A5 debugging is unsupported and may be buggy.");
    tdep->arc_processor_variant_info->arcprocessorversion = A5;
  }
  else {
    tdep->arc_processor_variant_info->arcprocessorversion = A4;
    set_gdbarch_decr_pc_after_break (current_gdbarch,0);
  }    
}

/* Get us out of user mode.  */
static unsigned int
clear_status32_user_bit ()
{
  int rd;
#ifndef ARC4_JTAG  
  if(arc_jtag_ops.jtag_read_aux_reg(ARC_HW_STATUS32_REGNUM, &rd) == JTAG_READ_FAILURE)
    error("Failure reading auxillary register 0x%x:status32 register", ARC_HW_STATUS32_REGNUM);

  if(arc_jtag_ops.jtag_write_aux_reg(ARC_HW_STATUS32_REGNUM, rd & ~0x80) == JTAG_READ_FAILURE)
    error("Failure writing to auxillary register 0x%x:status32 register", ARC_HW_STATUS32_REGNUM);
#endif
  return rd;
}

/* Restore a saved status32; use with clear_status32_user_bit().  */
static void
restore_status32_user_bit (unsigned int status32)
{
#ifndef ARC4_JTAG
  if(arc_jtag_ops.jtag_write_aux_reg(ARC_HW_STATUS32_REGNUM, status32) == JTAG_READ_FAILURE)
    error("Failure writing to auxillary register 0x%x:status32 register", ARC_HW_STATUS32_REGNUM);
#endif
}

/* UB bit in the debug register.  It allows brk_s to work in user mode.  */
static void
set_debug_user_bit ()
{
  if(is_arc700 ())
    {
      /* set UB = 1 */
      unsigned int debug;
      if (arc_jtag_ops.jtag_read_aux_reg (ARC_HW_DEBUG_REGNUM, &debug) == JTAG_READ_FAILURE)
	error("Failure reading auxillary register 0x%x:debug register",ARC_HW_DEBUG_REGNUM);
      debug |= 0x10000000;                     
      arc_jtag_ops.jtag_write_aux_reg (ARC_HW_DEBUG_REGNUM, debug);
    }
}



static void
invalidateCaches (void)
{
    if(arc_jtag_ops.jtag_write_aux_reg ( ARC_HW_ICACHE_IVIC , 1)==JTAG_WRITE_FAILURE)
	error("Failure writing 0x1 to auxillary register 0x%x:Icache invalidate\n",ARC_HW_ICACHE_IVIC);
    if(arc_jtag_ops.jtag_write_aux_reg ( ARC_HW_DCACHE_IVIC , 1)==JTAG_WRITE_FAILURE)
	error("Failure writing 0x1 to auxillary register 0x%x:Dcache invalidate\n",ARC_HW_DCACHE_IVIC);
}

static void
disableCaches (void)
{
  /* Disabling Icache */
    if(arc_jtag_ops.jtag_write_aux_reg( ARC_HW_ICACHE_CONTROL ,0x1)==JTAG_WRITE_FAILURE)
      error("Failure writing 0x1 to auxillary register 0x%x:Icache control register\n",ARC_HW_ICACHE_CONTROL);
  /* Disabling Dcache */
    if(arc_jtag_ops.jtag_write_aux_reg( ARC_HW_DCACHE_CONTROL ,0x1)==JTAG_WRITE_FAILURE)
	error("Failure writing 0x1 to auxillary register 0x%x:Dcache control register\n",ARC_HW_DCACHE_CONTROL);
}


/* Function: arc_debug_open
 * Parameters :
 * 1. args :
 * 2. from_tty:
 * Returns : void
 * Description: 
	1. Connect to the jtag target . 
	2. Read the number of action points supported. 
	3. Read the configuration of action points. 
	4. Set up internal data structures for number of hardware
	   breakpoints and watchpoints. 
	5. Set the UB bit to 1 for ARC700 and not for ARC600.
 * 
 */

void
arc_debug_open (char *args, int from_tty)
{
  ENTERARGS("%s", args);
  target_preopen(from_tty);
  
  reopen_exec_file ();
  reread_symbols ();
  
  unpush_target (&arc_debug_ops);  
  arc_jtag_ops.jtag_open();
  push_target (&arc_debug_ops);

  /* Call arc_update_architecture if opened successfully.  */
  arc_update_architecture();
  /* Fixme  :: Should these be in create_inferior or 
     some place else ?. We would not like these here
     when attach starts working. 
  */
  disableCaches();
#ifdef ARC4_JTAG
  if(arc_jtag_ops.jtag_write_aux_reg (ARC_HW_STATUS_REGNUM, 0x02000000)==JTAG_WRITE_FAILURE)
          error("Failure writing 0x0200 0000 to auxillary register 0x%x:status register\n",ARC_HW_STATUS_REGNUM);

  if (from_tty)
    printf_filtered ("Connected to the arcjtag target.\n");  
  
#else
  if(arc_jtag_ops.jtag_write_aux_reg (ARC_HW_STATUS32_REGNUM, 0x1)==JTAG_WRITE_FAILURE)

    error("Failure writing 0x1 to auxillary register 0x%x:status32 register\n",ARC_HW_STATUS32_REGNUM);
  
  /* allow breakpoints in user mode.  */
  set_debug_user_bit ();


  if (from_tty)
    printf_filtered ("Connected to the arcjtag target.\n");  
#endif
}

void arc_debug_close()
{
    arc_jtag_ops.jtag_close();
}

/* Function: arc_debug_attach
 * Parameters :
 * 1. char *x:
 * 2. int i:
 * Returns : void
 * Description: 
 *  1. attach without resetting the board 
 *  2. get all Board configuration registers of interest.
 *  if ARC700 set the UB bit to 1. (This is invalid in the 
 *  ARC600).
 */


void
arc_debug_attach (char *x, int i)
{

  ENTERMSG;
}


/* Function: arc_debug_attach
 * Parameters :
 * 1. char *x:
 * 2. int i:
 * Returns : void
 * Description: 
 *  1. Detach without resetting the board. 
 */
void
arc_debug_detach (char *x, int i)
{
	
  ENTERMSG;

  /* Let it continue.  */
  target_resume (inferior_ptid, 0, 0);
}


/* Function: arc_debug_resume
 * Parameters :
 * 1. ptid_t ptid:
 * 2. int step: 1 - single step , 0 run freely.
 * 3. enum target_signal signal;
 * Returns : void
 * Description: 
 *  	1. What about Pipecleaning?
 *	2. Write 0 to the HALT bit in status32.
 *      3. Send a signal (ignore) in this case. 
 *      4. if(step) use hardware single step on the ARC700.
 *          done by setting the IS bit in the debug register
 *          and clearing the halt bit in status32. 
 *	
 */

void
arc_debug_resume (ptid_t ptid, int step, enum target_signal signal)
{
  
  ENTERARGS("%d,%d,%d", ptid.pid, step, signal);
  /* Make the inferior resume execution, sending a signal if necessary */
  unsigned int rd;

  /* Because breakpoints may have been set/removed.  */
  invalidateCaches ();

  /* This bit is required if breakpoints are to be allowed in user mode.  We
     set it in target_open, but the operating system might clear it.  So we
     set it every time we resume.  */
  set_debug_user_bit ();

  if(step)
  {

    /* reading debug reg */
      if(arc_jtag_ops.jtag_read_aux_reg(ARC_HW_DEBUG_REGNUM,&rd)==JTAG_READ_FAILURE)
	  error("Failure reading auxillary register 0x%x:debug register",ARC_HW_DEBUG_REGNUM);
#ifdef ARC4_JTAG      
      rd |= 0x801; /*Setting the IS and the SS bit in the status register
		     for the A4 core to allow it to single step.  */
#else
      /* Mask for Single Stepping changes for ARC600 and ARC700. */
      if(is_arc700())
	rd |= 0x800;
      else
	if(is_arc600())
	  rd |= 0x801;
#endif
      
      /* Writing to IS bit in DEBUG register for 
	 hardware single instruction stepping.  */
      if(arc_jtag_ops.jtag_write_aux_reg(ARC_HW_DEBUG_REGNUM ,rd)==JTAG_WRITE_FAILURE)
	  error("Failure writing 0x%x to auxillary register 0x%x:debug register\n",rd,ARC_HW_DEBUG_REGNUM);
  }
  else
  {
    /* Restarting the processor by clearing the 'H' bit in the status register*/
#ifdef ARC4_JTAG    
    /* reading the status reg */     
    if(arc_jtag_ops.jtag_read_aux_reg(ARC_HW_STATUS_REGNUM,&rd)==JTAG_READ_FAILURE)
      error("Failure reading auxillary register 0x%x:status register",ARC_HW_STATUS_REGNUM);
  
    rd = rd & ~(0x02000000);
  
    /* starting the halted processor */
    if(arc_jtag_ops.jtag_write_aux_reg(ARC_HW_STATUS_REGNUM,rd)==JTAG_WRITE_FAILURE)
      error("Failure writing 0x%x to auxillary register 0x%x:status register\n",rd,ARC_HW_STATUS_REGNUM);
#else
    /* reading the status32 reg */     
    if(arc_jtag_ops.jtag_read_aux_reg(ARC_HW_STATUS32_REGNUM,&rd)==JTAG_READ_FAILURE)
      error("Failure reading auxillary register 0x%x:status32 register",ARC_HW_STATUS32_REGNUM);
  
    rd = rd & ~(0x1);
  
    /* starting the halted processor */
    if(arc_jtag_ops.jtag_write_aux_reg(ARC_HW_STATUS32_REGNUM,rd)==JTAG_WRITE_FAILURE)
      error("Failure writing 0x%x to auxillary register 0x%x:status32 register\n",rd,ARC_HW_STATUS32_REGNUM);
#endif
  }
}

/* For the ^C signal handler.  */
static  void (*ofunc) (int);

/* The command line interface's stop routine.  This function is installed as
   a signal handler for SIGINT.  The first time a user requests a stop, we
   call target_stop to send a break or ^C.  If there is no response from the
   target (it didn't stop when the user requested it), we ask the user if
   he'd like to detach from the target.  */
static void
arc_debug_interrupt (int signo)
{
  /* If we get the signal twice, do something more drastic.  */
  signal (signo, arc_debug_interrupt_twice);
  
  target_stop ();
}

/* The user typed ^C twice.  */
static void
arc_debug_interrupt_twice (int signo)
{
  signal (signo, ofunc);

  if (query ("Interrupted while waiting for the program.\n\
Give up (and stop debugging it)? "))
    {
      target_mourn_inferior ();
      throw_exception (RETURN_QUIT);
    }

  signal (signo, arc_debug_interrupt);
}

/* Function: arc_debug_wait
 * Parameters :
 * 1. ptid_t ptid:
 * 2. struct target_waitstatus *status: Indicates status at end
                                        of wait for F.E.
 * Returns : void
 * Description: 
 *        Poll status32 for the value of H bit.	
 *        After H bit is set in status32. 
 *        Wait till LD(load pending bit) in the DEBUG register
 *        is cleared.
 *        SH bit is set if flag instruction was used to halt the processor.
 *        BH bit is set if the ARCompact processor stopped due to 
 *        a brk_s instruction. Set the target_waitstatus (signal) to SIGTRAP
 *        only in such a situation. 
 *	
 */

ptid_t
arc_debug_wait (ptid_t ptid, struct target_waitstatus *status)
{
  unsigned int debug;
  ENTERMSG;

  /* signal handler for Control-C.  */
  ofunc = signal (SIGINT, arc_debug_interrupt);

  arc_jtag_ops.jtag_wait();
  /* put the old function back.  */
  signal (SIGINT, ofunc);

  /* If the SH ("self halt") bit is set, we stopped because of the flag
     instruction, which is used by programs to exit.  */
  if (arc_jtag_ops.jtag_read_aux_reg (ARC_HW_DEBUG_REGNUM,
				      &debug) == JTAG_READ_FAILURE)
    {
      error ("Failure reading from debug register");
    }

  /* SH bit of debug register */
  if (debug & ARC_DEBUG_REG_SH_BIT)
    {
      int exitcode;
      status->kind = TARGET_WAITKIND_EXITED;

      /* Exit code of the program.  */
      if (arc_jtag_ops.jtag_read_core_reg (0, &exitcode) == JTAG_READ_FAILURE)
	{
	  warning ("Failure reading from register r0, assuming exit code = 0");
	  status->value.integer = 0;
	}
      status->value.integer = exitcode;
    }
  else
    {
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
    }

#ifndef ARC4_JTAG
  /* Bug #1311 (ARC600): Setting a breakpoint on the last instruction of a
     ZOL causes GDB to stop at LP_START.  Detect this condition and warn the
     user.  */
  if (is_arc600 ())
    {
      unsigned int pc, lp_start, lp_end, lp_count;
      arc_debug_read_core_register (ARC_LP_COUNT_REGNUM, &lp_count);
      if (lp_count != 0)
	{
	  arc_debug_read_aux_register (ARC_HW_PC_REGNUM, &pc);
	  arc_debug_read_aux_register (ARC_HW_LP_START_REGNUM, &lp_start);
	  
	  if (pc == lp_start)
	    {
	      extern struct breakpoint *breakpoint_chain;
	      struct breakpoint *b;
	      arc_debug_read_aux_register (ARC_HW_LP_END_REGNUM, &lp_end);

	      for (b = breakpoint_chain; b; b = b->next)
		{
		  /* lp_end is the address of the last instruction + the
		     size of the last instruction.  We could use the
		     disassembler and find out the size, or just try both
		     possible sizes.  */
		  if ((b->enable_state == bp_enabled && !b->pending) &&
		      b->loc->address == lp_end-4 || b->loc->address == lp_end-2)
		    {
		      warning ("Did you set a breakpoint on the last instruction of \n\
a Zero Overhead Loop ?  Such breakpoints do not work properly.");
		    }
		}
	    }
	}
    }
#endif
  return inferior_ptid;
}

static unsigned int 
arc_get_hw_regnum_mapping ( int regno )
{
  int i;

  if (regno >= ARC_STATUS_REGNUM 
#ifdef ARC4_JTAG
      && regno <= ARC_DEBUG_REGNUM
#else
      && regno <= ARC_AUX_IRQ_PENDING_REGNUM
#endif      
      )
    return  arc_aux_reg_map[regno - ARC_STATUS_REGNUM].hw_regno;
  
  for ( i = 0 ; i < (sizeof(arc_bcr_reg_info) / sizeof (struct arc_reg_info)) ; i++)
    {
      if (regno == arc_bcr_reg_info[i].gdbregno)
	return arc_bcr_reg_info[i].hw_regno;
    }

  return -1;
}


/* Function: arc_debug_fetch_regs.
 * Parameters :
 * 1. int regnum: Register number. If register number is -1.Fetch
 *                all the registers.Read all core registers here.
 * Returns : void
 * Description: 
 *          Set up regcache_raw_supply(current_regcache,regno)
 *
 */
void
arc_debug_fetch_regs (int regno)
{
	/* Read all core registers */
  ENTERARGS("%d",regno);

  int dummyvalue = 0xABCDABCD;
  unsigned int hw_regno;
  unsigned int read_buf;

  if( regno < ARC_NR_CORE_REGS )
  {
      hw_regno = regno;
      if(arc_jtag_ops.jtag_read_core_reg(regno,&read_buf)==JTAG_READ_FAILURE)
	  error("Failure reading from core register 0x%x\n",regno);
  }
  else
  {
#ifndef ARC4_JTAG    
    if( regno > ARC_NR_REGS)
      error("Invalid Register Number\n");
#endif
    
    hw_regno  = arc_get_hw_regnum_mapping (regno);
    if(arc_jtag_ops.jtag_read_aux_reg(hw_regno,&read_buf)==JTAG_READ_FAILURE)
	error("Failure reading auxillary register 0x%x",hw_regno);
  }
  
  if(debug_arc_jtag_target_message)
    {
      printf_filtered("HW_Regno=0x%x",hw_regno);
      printf_filtered("Read Regno 0x%x the value 0x%x\n",hw_regno,read_buf);
    }
  regcache_raw_supply(current_regcache,regno,&read_buf);

  LEAVEMSG;
}


/* Function: arc_debug_fetch_regs.
 * Parameters :
 * 1. int regnum: Register number. If register number is -1.Fetch
 *                all the registers.Read all core registers here.
 * Returns : void
 * Description: 
 *          Use deprecated register information for this or regcache_read_unsigned .
 * FIXME: would need to change to use regcache_raw_supply instead.
 */


void
arc_debug_store_regs (int regno)
{
  /* write_all core registers */
  ENTERARGS("%d", regno);
  unsigned int hw_regno;
  unsigned int write_buf;

  if(debug_arc_jtag_target_message)
    printf_filtered("\n%d",regno);

  regcache_raw_collect(current_regcache,regno,&write_buf);
  if( regno < ARC_NR_CORE_REGS )
  {
      if(arc_jtag_ops.jtag_write_core_reg(regno,write_buf)==JTAG_WRITE_FAILURE)
	  error("Failure writing 0x%x to core register 0x%x",write_buf,regno);
  }
  else
  {
#ifndef ARC4_JTAG    
    if (regno > ARC_NR_REGS)
      error ("Invalid register number \n");
#endif

    hw_regno = arc_get_hw_regnum_mapping (regno);

    if(debug_arc_jtag_target_message)
	printf_filtered("Writing to regno 0x%x the value 0x%x",
			hw_regno,write_buf);
    if(arc_jtag_ops.jtag_write_aux_reg(hw_regno,write_buf)==JTAG_WRITE_FAILURE)
	error("Failure writing 0x%x to auxillary register 0x%x\n",write_buf,hw_regno);
  }

}


/* Function: arc_debug_prepare_to_store.
 * Parameters :
 * 1. int regnum: Register number. If register number is -1.Fetch
 *                all the registers.Read all core registers here.
 * Returns : void
 * Description: 
 *          Use deprecated register information for this. 
 * FIXME: would need to change to use regcache_raw_supply instead.
 */

/* This gets called just before store_regs */
void
arc_debug_prepare_to_store (void)
{
	/* does nothing . Why is this around ? */
  ENTERMSG;
}

/* Read or write memory */



/* Function: arc_debug_xfer_memory. 
 * Parameters : 
 * 1. int regnum: Register number. If register number is -1.Fetch
 *                all the registers.Read all core registers here.
 * Returns : void
 * Description: 
 *    This has been superceded by target_xfer_memory_partial.
 *
 */
int
arc_debug_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
		      struct mem_attrib *attrib, struct target_ops *target)
{
	/* There is no xfer_memory . Its been deprecated in 6.3 .Replace	
	 * this by target_xfer_memory_partial .
	 */
  ENTERARGS("memaddr=%lx, myaddr=%lx, len=%d, write=%d",
	    memaddr, (unsigned long)myaddr, len, write);
  
  return len; /* success */
}


    

LONGEST 
arc_debug_xfer_partial (struct target_ops *ops,
			enum target_object object, 
			const char *annex,
			void *readbuf, 
			const void *writebuf,
			ULONGEST offset, 
			LONGEST len)
{

  int i=0, read_num=0, temp_len=0;
  unsigned int small_buf;
  char query_type;
  ULONGEST temp_offset=0;
  if(debug_arc_jtag_target_message)
    printf("..Entered arc_debug_xfer_partial()...with offset 0x%x\n",(unsigned int)offset);
  /* Handle memory */
  if (object == TARGET_OBJECT_MEMORY)
    {
      int saved_status32;
      int xfered=0;
      int attempts;
      errno = 0;

      /* Get out of user mode so that we can read/write anything anywhere.  */
      saved_status32 = clear_status32_user_bit ();

      if (writebuf != NULL)
	{
	    char *buffer=(char *)xmalloc(4);
	    char *temp_buf = (char *)writebuf;

	    /* Address alignment to integral multiple of four */
	    temp_offset = offset;
	    temp_len = temp_offset % 4;
	    
	    i = 0;
	    if(temp_len)
	    {
		temp_offset = offset - temp_len;
		if(debug_arc_jtag_target_message)
		  {
		    printf("---- Aligning-----------\n");
		    printf("calling write_chunk at 0x%x where \
offset = 0x%x\n",
			   (unsigned int)temp_offset,(unsigned int)offset);
		  }

		attempts = 0;
		do{
		    if (attempts++ == MEMORY_TRANSFER_ATTEMPTS)
		      return 0;
		    xfered = arc_jtag_ops.jtag_memory_chunk_read(temp_offset,
								 (unsigned int *)buffer,4);
		}while(xfered != 4);
		
		for(i=0;i<len && i<(4-temp_len);i++)
		    
		    buffer[i+temp_len]=temp_buf[i];

		attempts = 0;
		do{
		    if (attempts++ == MEMORY_TRANSFER_ATTEMPTS)
		      return 0;
		    xfered = arc_jtag_ops.jtag_memory_chunk_write(temp_offset,
								  (unsigned int *)buffer,4);
		}while(xfered != 4);

		
		temp_buf = (char *)writebuf + i;
		temp_offset = offset + i;
		len = len - i;
	    }
	    if(len>0)
	      len =arc_jtag_ops.jtag_memory_chunk_write(temp_offset,
							(unsigned int *)temp_buf,len);
	    if(debug_arc_jtag_target_message)
	      printf("...leaving arc_debug_xfer_partial() write.. \
with return value %d",(int)len);

	    restore_status32_user_bit (saved_status32);
	  return (len + i);
	}
      else
      {
	  char *buffer=(char *)xmalloc(4);
	  char *temp_buf = (char *)readbuf;
	  /* Address alignment to integral multiple of four */
	  temp_offset= offset;
	  temp_len= temp_offset % 4 ;

	  i = 0;
	  if(temp_len)
	    {
	      temp_offset = offset - temp_len;
	      if(debug_arc_jtag_target_message)
		{
		  printf("---- Aligning-----------\n");
		  printf("calling read_chunk at 0x%x where offset =0x%x \n",
			 (unsigned int)temp_offset,(unsigned int)offset);
		}

	      attempts = 0;
	      do{
		if (attempts++ == MEMORY_TRANSFER_ATTEMPTS)
		  return 0;
		xfered = arc_jtag_ops.jtag_memory_chunk_read(temp_offset,(unsigned int *)buffer,4);
	      }while(xfered != 4);
	      
	      for(i=0;i<len && i<(4-temp_len);i++)
		temp_buf[i]=buffer[i+temp_len];
	      
	      temp_buf = (char *)readbuf + i;
	      temp_offset = offset + i;
	      len = len - i;
	    }
	  if(len>0)
	    len = arc_jtag_ops.jtag_memory_chunk_read(temp_offset,(unsigned int *)temp_buf,len);
	  if(debug_arc_jtag_target_message)
	    {
	      printf("\nlen=%d",(int)len + temp_len);
	      printf("...leaving arc_debug_xfer_partial() read.. \
with return value %d",
		     (int)len + temp_len);
	    }

	  restore_status32_user_bit (saved_status32);
	  return (len + i);
	  
      }

    }

  /* ARC auxillary registers: they are 32bits wide and are in a 32 bit
     address space, although only part of the address space is used.  */
  else if (object == ARC_TARGET_OBJECT_AUXREGS)
    {
      unsigned int regno;

      if (readbuf)
	{
	  for (regno = offset; regno < offset+len; ++regno)
	    {
	      unsigned int rd;

	      if (arc_jtag_ops.jtag_read_aux_reg (regno, &rd) == JTAG_READ_FAILURE)
		{
		  return (regno - offset);
		}
	      ((int *)readbuf)[regno - offset] = rd;
	    }
	}
      else if (writebuf)
	{
	  for (regno = offset; regno < offset+len; ++regno)
	    {
	      if (arc_jtag_ops.jtag_write_aux_reg (regno, ((int*)writebuf)[regno - offset]) ==
		  JTAG_WRITE_FAILURE)
		{
		  return (regno - offset);
		}
	    }
	}

      /* success */
      return (LONGEST)len;
    }
    else
    {
	printf("\nRequested target_object not yet supported with arc-jtag");
    }

  return -1;

}




void
arc_debug_files_info (struct target_ops *target)
{
	/* Do nothing. Just say its a remote target */
  ENTERMSG;
}


/* Function: arc_debug_insert_breakpoint
 * Parameters : 
 * 1. CORE_ADDR addr: Address for breakpoint.
 * 2. char * contents: Contents for the breakpoint.
 * Returns : int
 * Description: 
 * See if you can insert a hardware breakpoint using the actionpoints
 * interface. Use brk_s if architecture is ARC700 and you need to use
 * a software breakpoint.The gdbarch breakpoint should be initialized to
 * the right value if used with target_arc_debug.
 *
 */


int
arc_debug_insert_breakpoint (CORE_ADDR addr, char *contents)
{
	
  ENTERARGS("%x", (unsigned int)addr);
#ifndef ARC4_JTAG
  unsigned int bp = 0x20207fff; /*FIXMEA: what does 0x2020 stand for ?*/
#else
  unsigned int bp = 0x1ffffe00;
#endif
  unsigned int r;
  int instr_size;
  const unsigned char *breakpt_instr;
  breakpt_instr=BREAKPOINT_FROM_PC(&addr,&instr_size);

  /* save the existing value */
  /* r==0 means the read succeeded */
  if(debug_arc_jtag_target_message)
    printf_filtered ("instrcution size = %d and instruction 0x%x",
	   instr_size, *(unsigned int *)breakpt_instr);
  r = target_read_memory (addr, contents, instr_size);
  /* put the breakpoint */
  if(r==0)
    r = target_write_memory (addr, (char *)&bp, instr_size);
  return r;
}


/* Function: arc_debug_remove_breakpoint. 
 * Parameters : 
 * 1. CORE_ADDR addr: Address. 
 * 2. char * contents : contents. 
 * Returns : int.
 * Description: 
 *  Write the old contents back for the breakpoint.
 *
 */

int
arc_debug_remove_breakpoint (CORE_ADDR addr, char *contents)
{
  ENTERARGS("%x, %lx", (unsigned int)addr, *(unsigned long *)contents);

  /* write the old value back */
#ifdef ARC4_JTAG
  return target_write_memory (addr, contents, 4);
#else
  return target_write_memory (addr, contents, 2);
#endif
}



/* Function: arc_debug_kill
 * Parameters : void.

 * Returns : void.
 * Description: Heavy duty arsenal.Kill the process. 
 * Maybe we do a board reset and kill it. Write 1 to Halt
 * in Status32. 
 */

void
arc_debug_kill (void)
{
  ENTERMSG;

  /* Do stuff */

  target_mourn_inferior ();
}

/* Function: arc_debug_load
 * Parameters :
 * 1. char * args: Arguments. 
 * 2. int from_tty: Which terminal.
 * Returns : void.
 * Description: Load the program into jtag.
 */

void
arc_debug_load (char *args, int from_tty)
{
	/* Write to RAM of the ARC700 board by running through the sections .*/
  asection *bss_section;
  CORE_ADDR bss_addr;
  bfd_size_type bss_size;
  char *zero_buf;
  int target_errno;

  ENTERARGS("%s", args);

  generic_load(args, from_tty);

  /* Zero the bss, if it exists.  */
  bss_section = bfd_get_section_by_name (exec_bfd, ".bss");
  if (bss_section)
    {
      bss_addr = bfd_section_lma (exec_bfd, bss_section);
      bss_size = bfd_get_section_size (bss_section);
      zero_buf = (char *)xcalloc (bss_size, 1);
      
      if (debug_arc_jtag_target_message)
	printf_filtered("%s: bss at %x, size = %x\n", __FUNCTION__, (unsigned int)bss_addr,(unsigned int)bss_size);
      
      target_errno = target_write_memory (bss_addr, zero_buf, bss_size);
      free (zero_buf);
      if (target_errno)
	{
	  error ("load: error zeroing bss: %s\n", strerror(target_errno));
	}
    }
  else
    {
      if (debug_arc_jtag_target_message)
	printf_filtered("%s: no bss\n", __FUNCTION__);
    }
      
  clear_symtab_users();
}

/* Function: arc_debug_create_inferior
 * Parameters :
 * 1. char * exec_file:
 * 2. char * args: 
 * 3. char ** env;
 * Returns : void.
 * Description: Set up sanity values for arc_debug_create_inferior. More thought
 * needed for this. 
 */


void
arc_debug_create_inferior (char *exec_file, char *args, char **env,int dummy)
{
  ENTERARGS("%s,%s", exec_file, args);

  /* If no exec file handed to us, get it from the exec-file command
     -- with a good, common error message if none is specified.  */
  if (exec_file == 0)
    exec_file = get_exec_file (1);

  /* We dont really have a PID or anything, but GDB uses this value to check
     if the program is running. */
  inferior_ptid.pid = 42;

  clear_proceed_status();
  /* -1 means resume from current place
     TARGET_SIGNAL_0 means dont give it any signal
     Last arg should be true if you want to single step */
  //proceed ((CORE_ADDR) -1, TARGET_SIGNAL_0, 0);
  proceed (bfd_get_start_address (exec_bfd), TARGET_SIGNAL_0, 0);
}


/* Function: arc_debug_mourn_inferior
 * Parameters :void.
 * Returns : void.
 * Description: Set up sanity values for arc_debug_create_inferior. More thought
 * needed for this. 
 */

void
arc_debug_mourn_inferior (void)
{
  ENTERMSG;

  unpush_target (&arc_debug_ops);

  generic_mourn_inferior ();
}


/* Function: arc_debug_mourn_inferior
 * Parameters :ptid_t ptid.
 * Returns : 1 always. 
 * Description: Checks for return values .
 */


int
arc_debug_thread_alive (ptid_t ptid)
{
  ENTERMSG;
  return 1;
}


/* Function: arc_debug_stop
 * Parameters: void
 * Returns: void.
 * Description: Stop the Processor. We stop by writing FH bit to Debug Register . 
 *	write 1 to the FH bit in the Debug register after 
 *	polling for the DEBUG register to have no loads pending .
 */
void
arc_debug_stop (void)
{

  ENTERMSG;
  int val = 0x2;
  /* Stop using the FH bit in the debug register. */
  arc_debug_write_aux_register (ARC_HW_DEBUG_REGNUM, &val);
  
}

/* Read core register.  Return 0 on success.  */
static int
arc_debug_read_core_register (int hwregno, int *buf)
{
  int rd;
  if(arc_jtag_ops.jtag_read_core_reg(hwregno,&rd)==JTAG_READ_FAILURE)
      error("Failure reading auxillary register 0x%x",hwregno);
  *buf = rd;
  return 0;
}

/* Read aux register.  Return 0 on success.  */
static int
arc_debug_read_aux_register (int hwregno, int *buf)
{
  int rd;
  if(arc_jtag_ops.jtag_read_aux_reg(hwregno,&rd)==JTAG_READ_FAILURE)
      error("Failure reading auxillary register 0x%x",hwregno);
  *buf = rd;
  return 0;
}

/* Write aux register.  Return 0 on success.  */
static int
arc_debug_write_aux_register (int hwregno, int *buf)
{
    if(arc_jtag_ops.jtag_write_aux_reg(hwregno, *buf)==JTAG_WRITE_FAILURE)
	error("Failure writing 0x%x to auxillary register 0x%x\n",*buf,hwregno);
  return 0;
}

/* Helper routine for commands added.  */
/* Print Processor Variant Info.  */
static void
arc_print_processor_variant_info (void)
{
  struct gdbarch_tdep *tdep  = gdbarch_tdep (current_gdbarch);
  
#ifdef ARC4_JTAG
  if (tdep->arc_processor_variant_info && 
      tdep->arc_processor_variant_info->arcprocessorversion == A4)
    {
      printf_filtered ("A4\n");
    }
#else
  if (tdep->arc_processor_variant_info)
    {
      if(tdep->arc_processor_variant_info->arcprocessorversion == ARC700)
	printf_filtered ("ARC700\n");
      else
	printf_filtered ("ARC600\n");
    }
#endif
  else
    {
      printf_filtered ("ARC Processor Information not available \n");
    }

}


static void 
arc_print_bcr_regs (void)
{
  int i = 0;
  unsigned int bcrval = 0;
  for ( i = 0 ; i < (sizeof(arc_bcr_reg_info) / sizeof (struct arc_reg_info)) ; i++)
    {
	if(arc_jtag_ops.jtag_read_aux_reg (arc_bcr_reg_info[i].hw_regno, &bcrval)==JTAG_READ_FAILURE)
	    error("Failure reading auxillary register 0x%x",arc_bcr_reg_info[i].hw_regno);
      printf_filtered ("[%02x] %-15s : 0x%02x\n",arc_bcr_reg_info[i].hw_regno,
		       arc_bcr_reg_info[i].name, bcrval );
    }

}

static void
arc_debug_jtag_reset_board (void)
{
  arc_jtag_ops.jtag_reset_board();
}


/* Function: init_arc_debug_ops
 * Parameters: void
 * Returns: void.
 * Description: Initialize the jtag operations. 
 */

void
init_arc_debug_ops (void)
{
  ENTERMSG;
#ifdef ARC4_JTAG
  arc_debug_ops.to_shortname = "arcjtag";
  arc_debug_ops.to_longname = "Target for debugging an A4 board with JTAG.";
  arc_debug_ops.to_doc = "Debug a remote A4 board via a JTAG"; /* to_doc */
#else
  arc_debug_ops.to_shortname = "arcjtag";
  arc_debug_ops.to_longname = "Target for debugging an ARC700 board with JTAG.";
  arc_debug_ops.to_doc = "Debug a remote ARC700 board via a JTAG"; /* to_doc */
#endif


  arc_debug_ops.to_open = arc_debug_open;
  arc_debug_ops.to_close = arc_debug_close;
  arc_debug_ops.to_attach = arc_debug_attach;
  arc_debug_ops.to_detach = arc_debug_detach;
  arc_debug_ops.to_resume = arc_debug_resume;
  arc_debug_ops.to_wait = arc_debug_wait;

  arc_debug_ops.to_fetch_registers = arc_debug_fetch_regs;
  arc_debug_ops.to_store_registers = arc_debug_store_regs;
  arc_debug_ops.to_prepare_to_store = arc_debug_prepare_to_store;
  //arc_debug_ops.to_xfer_memory = arc_debug_xfer_memory;
  arc_debug_ops.to_xfer_partial = arc_debug_xfer_partial;
  arc_debug_ops.to_files_info = arc_debug_files_info;
  arc_debug_ops.to_insert_breakpoint = arc_debug_insert_breakpoint;
  arc_debug_ops.to_remove_breakpoint = arc_debug_remove_breakpoint;
  arc_debug_ops.to_kill = arc_debug_kill;
  arc_debug_ops.to_load = arc_debug_load;

  arc_debug_ops.to_create_inferior = arc_debug_create_inferior;

  arc_debug_ops.to_mourn_inferior = arc_debug_mourn_inferior;
  arc_debug_ops.to_thread_alive = arc_debug_thread_alive;
  arc_debug_ops.to_stop = arc_debug_stop;

  arc_debug_ops.to_terminal_inferior = NULL;

  
  arc_debug_ops.to_stratum = process_stratum;
  
  arc_debug_ops.to_has_all_memory = 1;
  arc_debug_ops.to_has_memory = 1;
  arc_debug_ops.to_has_stack = 1;
  arc_debug_ops.to_has_registers = 1;
  arc_debug_ops.to_has_execution = 1;

  arc_debug_ops.to_magic = OPS_MAGIC;
}



void
_initialize_arc_debug (void)
{
  ENTERMSG;
  init_arc_debug_ops ();
  add_target (&arc_debug_ops);
  add_setshow_boolean_cmd("arcjtag-debug-target",no_class, 
			  &debug_arc_jtag_target_message,
			  "Set whether to print arc jtag debug messages.\n",
			  "Show whether to print arc jtag debug messages.\n",
			  "If set the jtag debug messages from the target are \
printed.\n",
			  "Whether to print debug jtag messages is %s.\n",		  
			  NULL,NULL,&setlist,&showlist);

  add_setshow_boolean_cmd("arcjtag-debug-statemachine",no_class, 
			  &(arc_jtag_ops.arc_jtag_state_machine_debug),
			  "Set whether to print JTAG state machine \
debug messages \n",
			  "Show whether to print JTAG state machine \
debug messages \n",
			  "If set the JTAG state machine messages are \
printed.\n",
			  "Whether to print JTAG state machine debug \
messages is %s\n",
			  NULL,NULL,&setlist,&showlist);

  add_setshow_uinteger_cmd("arcjtag-retry-count",no_class, &arcjtag_retry_count,
			   "Set the number of attempts to be made for \
a JTAG operation.\n",
			   "Show the number of attempts to be made for \
a JTAG operation.\n",
			   "Indicates the number of times a JTAG operation \
is attempted before returning a failure.\n",
			   "The number of times a JTAG operation is attempted \
before returning a failure is %s.\n",
			   NULL, NULL, &setlist, &showlist);

  add_cmd ("arc-configuration", class_info, arc_print_processor_variant_info,
	   "Show ARC configuration information." , &infolist);

  add_cmd ("arc-bcr-registers", class_info, arc_print_bcr_regs,
           "Show BCR Registers in the ARC Processor Variant", &infolist);

  add_cmd ("arc-reset-board", class_obscure, arc_debug_jtag_reset_board,
           "Reset the board.", &cmdlist);

}
