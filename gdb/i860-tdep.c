/* Machine-dependent code which would otherwise be in inflow.c and core.c,
   for GDB, the GNU debugger.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.
   This code is for the i860 cpu.
   
   GDB is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY.  No author or distributor accepts responsibility to anyone
   for the consequences of using it or for whether it serves any
   particular purpose or works at all, unless he says so in writing.
   Refer to the GDB General Public License for full details.
   
   Everyone is granted permission to copy, modify and redistribute GDB,
   but only under the conditions described in the GDB General Public
   License.  A copy of this license is supposed to have been given to you
   along with GDB so you can know your rights and responsibilities.  It
   should be in a file named COPYING.  Among other things, the copyright
   notice and this notice must be preserved on all copies.
   
   In other words, go ahead and share GDB, but don't try to stop
   anyone else from sharing it farther.  Help stamp out software hoarding!
   */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#include "defs.h"
#include "tm-i860.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"
#include "symtab.h"
#include "value.h"

#include "i860-opcode.h"

#include "breakpoint.h"
#include "i860-break.h"
#include "command.h"
#include "target.h"

#ifdef notdef
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#endif

#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/* #include <sys/reg.h> */
#include "i860_reg.h"

#include <a.out.h>
#include <sys/file.h>
#include <core.h>

#include <sys/user.h>
#include <elf.h>
#include <sys/elftypes.h>
#include <sys/elf_860.h>
#include <libelf.h>


extern int read_memory();
extern int write_memory();
extern int read_memory_integer();
extern int print_insn();
extern void bzero();
extern void bcopy();

int btdebug = 0;    /* change value to 1 to enable debugging code */
int ansi_conformant;

#define  BTDEBUG  if (btdebug) btdebug_message

extern int errno;
extern char registers[];
CORE_ADDR get_saved_basereg();

#define INSTRUCTION_LENGTH 4
#define REGISTER_LENGTH 4
#define ALIGN_ARG(size,len)  ((size + (len-1))&(-len)) 
#define NUM_FLOAT_ARG_REGS 8
#define NUM_INT_ARG_REGS 12

/* routine to print debugging messages */
void btdebug_message(char *format, ...)
{	
   va_list	arglist;
   va_start( arglist, format );
   
   if( btdebug )
      vfprintf (stderr,  format, arglist );
   va_end  ( arglist 	  );
}


/* Peggy Fieland. Routine that attempts to find the start of the entry sequence
   for a routine. */
/* maximum number of instrutions to search back */
#define MAX_ROUTINE_SIZE 4096
CORE_ADDR find_entry_start(pc)
CORE_ADDR pc;
{
   CORE_ADDR instr, top_pc;
   int i;
   
   top_pc = pc;
   for (i = 0; i <  MAX_ROUTINE_SIZE; ++i)

      {
         instr = (unsigned)( adj_read_memory_integer (top_pc));
         /* Recognize "addu|adds -X,sp,sp" insn. */

         if ((instr & 0xEFFF0000) == 0x84420000) 
            {
	      return (top_pc);
            } 
         top_pc -= INSTRUCTION_LENGTH;
      }
   return (0);
}



/* Written by Peggy Fieland (Margaret_Fieland@vos.stratus.com) */
/* get the contents of a base register.  Used for dwarf OP_BASEREG */
/* At present, only OP_BASEREG generated is for R28.  NOTE that for stuff based on R28,
   the value we want is the VALUE AT PROCEDURE INVOKATION, and thus is the frame we
   use to get the value is the caller's frame. */
CORE_ADDR get_saved_basereg (frame, basereg)
FRAME frame;
int basereg;
{
   CORE_ADDR addr;
   if (basereg == R28)   /* Unconditionally ??? */
      {
         frame = get_prev_frame (frame);
         get_saved_register((char *) &addr, (int *) NULL,  (CORE_ADDR *) NULL, frame,
                            basereg, (enum lval_type *)NULL);

      }
   else
      get_saved_register((char *) &addr, (int *) NULL,  (CORE_ADDR *) NULL, frame,
                         basereg, (enum lval_type *)NULL);

   return (addr);
}
  


/* return nonzero if the routine containing pc has been 
 * compiled with -g. We assume -g if the first instruction is 
 * an  addu|adds -X,sp and the second is st.l fp,XX(sp) 
 *
 *	based on  skip_prologue();
 */

static int g_routine(pc)

     CORE_ADDR pc;
{
   CORE_ADDR instr;
   CORE_ADDR top_pc;
   
   top_pc = get_pc_function_start(pc);
   if (top_pc == NULL)
     top_pc = find_entry_start (pc);

   if (top_pc != NULL)
      {
         instr = (unsigned)( adj_read_memory_integer (top_pc));
         /* Recognize "addu|adds -X,sp,sp" insn. */
   
         if ((instr & 0xEFFF0000) == 0x84420000) 
            {
               top_pc += INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (top_pc));
      
               if( (instr & 0xFFE0F801) == 0x1C401801 )    /* st.l fp,X(sp) */
                  return(1);
            } 
      }
   return(0);
}


/* return the stack offset where the fp register is stored */
static int find_fp_offset(pc)
CORE_ADDR pc;
{
   int fp_off,i;
   CORE_ADDR	instr;
   
   /* look for the instruction and examine the offset */
   
   for (i=INSTRUCTION_LENGTH*1; i< INSTRUCTION_LENGTH*4; i+=INSTRUCTION_LENGTH){
      instr =   (unsigned)(adj_read_memory_integer(pc+i));
      if( (instr & 0xFFE0F801) == 0x1C401801) {    /* st.l fp,X(sp) */
         
         fp_off = SIGN_EXT16(((instr&0x001F0000) >> 5) | 
                             (instr&0x000007FE));
         return(fp_off);
      }
   }
   return(0);
}


/* return the stack offset where r1 (return linkage ) register is stored */
static CORE_ADDR find_r1(pc,sp,fp)
CORE_ADDR pc,sp, fp;
{
   int r1_off,i;
   CORE_ADDR	instr, ret_pc;
   
   /* look for the instruction and examine the offset */
   
   for (i=INSTRUCTION_LENGTH*1; i< INSTRUCTION_LENGTH*4; i+=INSTRUCTION_LENGTH)
      {
         instr = (unsigned)( adj_read_memory_integer(pc+i));
         if ((instr & 0xFFE0F801) == 0x1C400801) 
            { 
               /* st.l r1,X(sp) */
         
               r1_off = SIGN_EXT16(((instr&0x001F0000) >> 5) | 
                                   (instr&0x000007FE));
               ret_pc = read_memory_integer(sp+r1_off,sizeof(long));
               return(ret_pc);
            }
         else if ((instr & 0xFFE0F801) == 0x1C600801) 
         { 
            /* st.l r1,X(fp) */
            r1_off = SIGN_EXT16(((instr&0x001F0000) >> 5) | 
                                (instr&0x000007FE));
            ret_pc = read_memory_integer(fp+r1_off,sizeof(long));
            return(ret_pc);
         }
      }
   return(0);
}

CORE_ADDR skip_prologue(CORE_ADDR);

/* does routine starting at pc build a stack frame of any kind?? */
static int has_a_frame(pc)
CORE_ADDR pc;
{
   if( skip_prologue(pc) != pc )return(1);
   else return(0);
}


/* written by Peggy Fieland Margaret_Fieland@vos.stratus.com
   Routine to validate the return register and the frame pointer
   This routine is called when the routine we are in doesn't have a frame
   In that case, we assume that the return address and frame pointer have 
   not been touched.  In the following routine, we try to range check them 
  to see if they are valid. */

static int valid_regs (rp, fp)
CORE_ADDR rp, fp;
{
   if ( ( (rp % 4) != 0) | ( (fp % 16) != 0) )
      return (0);
   else 
      return (1);
}
/* get the pc and frame pointer (or sp )
 * for the routine that called us
 * when we (this_pc) is not within a  -g routine
 * if caller is non g we return sp for fp
 */

/* note this is written for Metaware version R2.1d compiler */
/* Modified by Peggy Fieland Margaret_Fieland@vos.stratus.com */
static int caller_pc(this_pc,this_sp,this_fp,to_pc,to_fp, called_from_frame_chain)
     CORE_ADDR this_pc,this_sp, this_fp;
     CORE_ADDR *to_pc, *to_fp;
     int called_from_frame_chain;
{
   CORE_ADDR func_start;
   int sp_offset,offset;
   CORE_ADDR sp,pc,fp,instr;
   
   BTDEBUG("caller_pc %x sp = %x\n",this_pc,this_sp);
   
   func_start = get_pc_function_start(this_pc);

   if (func_start == NULL)
      func_start = find_entry_start (this_pc);

   BTDEBUG("caller_pc func_start %x\n", func_start);

   if ((func_start == NULL))
      {
               /* error in traceback */
               fprintf(stderr, "error, unable to find start of function\n");
               return(0);
               
      }

   if (func_start!= NULL)
      {
         if( has_a_frame(func_start) ){
      
            BTDEBUG("has_a_frame\n");
      
            /* if our caller has a preamble and 
             * declares space for a stack frame
             * then we must work to find our return address
             */
            instr = (unsigned)( adj_read_memory_integer (func_start));
            /* Recognize "addu|adds -X,sp,sp" insn. */
      
            if ((instr & 0xEFFF0000) == 0x84420000)
               sp_offset=SIGN_EXT16(instr&0x0000FFFF);
         }
         else 	
            { 
               /* if we get here, procedure doesn't have a frame.  If we
                  do anything weird, the frame pointer and return register have
                  the values we want.  Check them to see if they are valid. */
                  
               CORE_ADDR temp_rp, temp_fp;

               /* temporary warning, since at the moment we don't have support for
                  the shared library */
               
               temp_rp = read_register(RP_REGNUM);
               temp_fp = read_register(FP_REGNUM);

               if (!valid_regs(temp_rp, temp_fp))
                  {           
                     fprintf(stderr, 
                             "error - unable to find return address, traceback terminating\n");
                     return(0);
                  }
               BTDEBUG("caller_pc no frame, using r1 %x and fp %x\n",
                       temp_rp, temp_fp);
               *to_pc = temp_rp;
               *to_fp = temp_fp;
               return (1);
            }
      
      BTDEBUG("sp_offset = %d %x\n",sp_offset,sp_offset);
      
         pc = find_r1(func_start, this_sp, this_fp);
      
      if(pc == NULL)
         {

            /* r1 wasn't stored between pc and function start */
            pc = read_register (RP_REGNUM);
         }

      sp= this_sp - sp_offset;
      
      BTDEBUG("callers pc = %x sp = %x\n",pc,sp);
      
      /* our caller a -g routine  ?
       * if he is we have to find his real fp
       * else provide the sp as his fp
       */
      
      if( g_routine(pc) ){
         
         BTDEBUG("caller_a_g\n");
         
         if( ! (offset = find_fp_offset(func_start)) ) {
            fprintf(stderr, "error - unable to find caller frame for routine at 0x%x, "
                    "traceback terminating\n", func_start);
            return(0);
         }
         BTDEBUG("offset = %x %d\n",offset,offset);
         
         fp = read_memory_integer(this_sp+offset,sizeof(long));
         *to_pc = CLEAN_PC(pc);
         *to_fp = fp;
         return(1);
      }else  
         *to_pc = CLEAN_PC(pc);
      *to_fp = sp;
      return(1);
   } else {
/*      pc = read_register(RP_REGNUM); */
/*       pc = 0;  */
      /* if we get here, procedure doesn't have a frame.  If we didn't
         do anything weird, the frame pointer and return register have
         the values we want.  Check them to see if they are valid. */
                  
      CORE_ADDR temp_rp, temp_fp;
      
      temp_rp = read_register(RP_REGNUM);
      temp_fp = read_register(FP_REGNUM);
      
      if (!valid_regs(temp_rp, temp_fp))
         {           
            fprintf(stderr, 
                    "error - unable to find return address, traceback terminating\n");

            return(0);
         }
      BTDEBUG("caller_pc no frame, using r1 %x and fp %x\n",
              temp_rp, temp_fp);
      *to_pc = temp_rp;
      *to_fp = temp_fp;
      return (1);
   }
}

/*
 ** Figure out address to place next breakpoint. Avoid tricky spots, 
 **	ie. delayed instruction slots etc.
 ** Need to upgrade this later to allow delayed instruction breakpoints
 **	with fix-up work done AFTER breakpoint.
 ** Note that this routine DOES deal with dual instruction mode
 */
#define BIM 0x8008

static branch_type
   place_brk (addr, mode, brk)
CORE_ADDR addr; 
int mode;
struct breakpoint *brk;
{
   CORE_ADDR  instr;
   CORE_ADDR nextadr, prevadr;
   int val = not_branch;
   long offset; /* Must be signed for sign-extend */
   prevadr = nextadr = 0;
   
   brk->address1 = 0;
   
   if (mode == SINGLE_STEP_MODE)
      {
         if (INDIM || ENDIM)
            {
			nextadr = brk->address	 = (addr +  INSTRUCTION_LENGTH*2);
  			instr	=  (unsigned)(adj_read_memory_integer ((addr + INSTRUCTION_LENGTH)));
			brk->mode	= DIM;
            }
         else
            {
			nextadr = brk->address	 = (addr + INSTRUCTION_LENGTH);
  			instr	=  (unsigned)(adj_read_memory_integer (addr));
			if (STDIM) 
                  brk->mode = DIM;
			else
                  brk->mode = SIM;
            }
         
         
         /*
          ** For br/call one more sequential instruction gets executed and  then we
          **	continue at the current addr + offset. We are definitely going to
          **	the dest. We are NOT allowed to place a breakpoint in the "delay"
          **	slot - (the next sequential instruction) so we only place 1 breakpoint
          **	at the destination.
          ** For the bc/bnc the next instruction executed is EITHER the next sequential
          **	or the destination of the branch, we therefore place 2 breakpoints one
          **	at each location.
          ** For the bc.t/bnc.t either 1 more sequential instruction is performed
          **	followed by a branch (like br/call) OR we skip the sequential 
          **	instruction and keep going. We therefore place a breakpoint at the
          **	destination of the branch AND the second sequential instruction after
          **	the branch. Again a breakpoint is NOT allowed in the "delay slot"
          */
         if ((instr & 0xE0000000) == 0x60000000 &&	/* CTRL format */
             (instr & 0xF8000000) != 0x60000000)		/* not pfld.y  */
            {
               if      ((instr & 0xF8000000) == 0x68000000)	/* br or call */
                  val = uncond_d;
               else if ((instr & 0xF4000000) == 0x74000000)	/* bc.t/bnc.t */
                  val = cond_d;
               else if ((instr & 0xF4000000) == 0x70000000)	/* bc or bnc  */
                  val = cond;
               offset = (instr & 0x03ffffff);
               if (offset & 0x02000000)			/*?sign extend*/
                  offset |= 0xFC000000;
               if (val == uncond_d)			/* br/call*/
                  prevadr = 0;
               else if (val == cond_d)			/* bc.t/bnc.t */
                  {
                     if ((INDIM) && !(ENDIM))
                        prevadr = nextadr + (2*INSTRUCTION_LENGTH);
                     else
                        prevadr = nextadr + INSTRUCTION_LENGTH;
                  } else {				/* bc  /bnc   */
                     if ((INDIM) && !(ENDIM))
                        prevadr = nextadr;
                     else
                        prevadr = nextadr;
                  }
               nextadr += (offset << 2);
            }
         /*
          ** We treat the bri/calli the same way as the br/call case.
          */
         else if ((instr & 0xFC00003F) == 0x4C000002 ||		/* calli    */
                  (instr & 0xFC000000) == 0x40000000)		/* bri      */
            {
               val	= uncond_d;
               offset	= ((instr & 0x0000F800) >> 11);
               nextadr	= (read_register(offset + R0) & 0xFFFFFFFC);
               prevadr = 0;
            }
         /*
          ** We treat the bte/btne the same way as the bc/bnc case.
          */
         else if ((instr & 0xF0000000) == 0x50000000) 		/* bte/btne */
            {
               val = cond;
               offset = SIGN_EXT16(((instr & 0x001F0000) >> 5)  | 
                                   (instr & 0x000007FF));
               if ((INDIM) && !(ENDIM))
                  prevadr = nextadr;
               else
                  prevadr = nextadr;
               
               nextadr += (offset << 2);
            }
         /*
          ** We treat the bte/btne the same way as the bc/bnc case.
          **	With the caveat that the 2 breakpoints may turn out to be at the same
          **	address in which case we ignore one of them.
          */
         else if ((instr & 0xFC000000) == 0xB4000000)		/* bla */
            {
               val	= cond_d;
               offset	= SIGN_EXT16(((instr & 0x001F0000) >> 5)  | 
                                      (instr & 0x000007FF));
               if ((INDIM) && !(ENDIM))
                  {
                     prevadr = nextadr + 2*INSTRUCTION_LENGTH;
                  } else	{
                     prevadr = nextadr + INSTRUCTION_LENGTH;
                  }
               nextadr += (offset << 2);
               if (prevadr == nextadr) prevadr = 0;
            }
      } else	{
         int adjust = 0;
         
         nextadr = addr;
         
         if (ISDIM(FOPADR(addr)))
            {
               if (ISDIM(FOPADR(nextadr- INSTRUCTION_LENGTH*2)))
                  {
                     instr	=  (unsigned)(adj_read_memory_integer(CORADR(addr
                                                       -(INSTRUCTION_LENGTH*2))));
                     brk->mode	= DIM;
                  } else	{
                     instr	=  (unsigned)(adj_read_memory_integer(addr-INSTRUCTION_LENGTH));
                     brk->mode	= RIM;
                  }
            } else {
               if (ISDIM(addr-INSTRUCTION_LENGTH))
                  {
                     instr	=  (unsigned)(adj_read_memory_integer(addr-INSTRUCTION_LENGTH));
                     brk->mode	= BIM;
                  } else	{
                     instr	=  (unsigned)(adj_read_memory_integer (addr-INSTRUCTION_LENGTH));
                     brk->mode	= SIM;
                  }
            }
         
         /* examine the PREVIOUS instruction to determine if we are in a branch delay
            slot.  If we are, dont set a break here -- set it on the previous instruction.
            This code also accounts for dual instruction mode */
         if ((instr & 0xE0000000) == 0x60000000 &&
             (instr & 0xF8000000) != 0x60000000)		/* not pfld.y  */
            {
               adjust++;
               /* br    /call */
               /* bc	 /bnc  */
               /* bc.t	 /bnc.t*/
               if ((instr & 0xF8000000) == 0x68000000) /* br or call */
                  BTDEBUG(" Breakpoint adjusted to avoid br/call delay slot and multiple breakpoints\n");
               
               if ((instr & 0xF4000000) == 0x74000000) /* bc.t or bnc.t */
                  BTDEBUG(" Breakpoint adjusted to avoid bc.t/bnc.t delay slot and"
                          "multiple breakpoints\n");

               /* it IS really OK to set a break on the instruction AFTER the conditional branch
                  -- it DOESN't have a delay slot */
               if ((instr & 0xF4000000) == 0x70000000) /* bc / bnc */
                  adjust = 0; 
            } else if
               ((instr & 0xFC00003F) == 0x4C000002 ||	/* bri/  calli */
                (instr & 0xFC000000) == 0x40000000)	
                  {
                     adjust++;
                     BTDEBUG(" Breakpoint adjusted to avoid calli/bri delay slot and"
                             " multiple breakpoints\n");
                  } else if
                     ((instr & 0xF0000000) == 0x50000000) /* bte - btne */
                        {
                           /* it's OK to set a break here -- we are NOT in aa branch delay slot */
                           /*
                           adjust++;
                           printf(" Breakpoint adjusted to avoid bte/btne multiple breakpoints\n");
                           */
                           adjust = 0;
                        } else if
                           ((instr & 0xFC000000) == 0xB4000000)
                              {
                                 adjust++;
                                 BTDEBUG(" Breakpoint adjusted to avoid bla delay slot and"
                                         " multiple breakpoints\n");
                              }
         if (adjust != 0)
            {
               if (brk->mode == DIM)
                  {
                     nextadr -= INSTRUCTION_LENGTH*2;
                     nextadr = CORADR(nextadr); 
                  }
               else
                  nextadr -= INSTRUCTION_LENGTH;
            }
         
      }
   
   if (brk->mode == RIM) 
      brk->mode = DIM;
   if (brk->mode == BIM) 
      brk->mode = SIM;
   
   if (nextadr != NULL)
      {
         if (brk->mode == DIM)
            {
               brk->act_addr[0] = CORADR(nextadr);
               brk->act_addr[1] = FOPADR(nextadr);
            } else	{
               brk->act_addr[0] = nextadr;
               brk->act_addr[1] = 0;
            }
      }
   
   if (prevadr != NULL)
      {
         brk->address1 = prevadr;
         if (brk->mode == DIM)
            {
               brk->act_addr[2] = CORADR(prevadr);
               brk->act_addr[3] = FOPADR(prevadr);
            } else	{
               brk->act_addr[2] = prevadr;
               brk->act_addr[3] = 0;
            }
      } else	{
         brk->act_addr[2] = brk->act_addr[3] = 0;
      }
   return val;
}

/* This routine checks to see if r1 has been stored into the frame between
   the addresses prologue_start and prologue_end. Recognize stores of r1 
   relative to both the sp and fp registers. */
static int has_stored_r1(CORE_ADDR prologue_start, CORE_ADDR prologue_end)
{
   CORE_ADDR instr;
   CORE_ADDR addr;
   
   BTDEBUG("has_stored_r1, prologue_start %x, prologue_end %x\n",
           prologue_start, prologue_end);

   for (addr = prologue_start; addr <= prologue_end;  addr += INSTRUCTION_LENGTH)
      {
   
         instr =  (unsigned)(adj_read_memory_integer (addr));
         if   ((instr & 0xFFE0F801) == 0x1C400801 /* st.l r1,X(sp) */
            || (instr & 0xFFE0F801) == 0x1C600801) /* st.l r1,X(fp) */
            return (1);
      }
   return 0;
}
/* This is used when GDB is exiting.  It gives less chance of error.*/


/* Simulate single-step ptrace call for sun4.  Code written by Gary
   Beihl (beihl@mcc.com).  */
/* Modified for i860 by Jim Hanko (hanko@orc.olivetti.com) */


static struct breakpoint brk;
typedef char binsn_quantum[sizeof break_insn];

/* Non-zero if we just simulated a single-step ptrace call.  This is
   needed because we cannot remove the breakpoints in the inferior
   process until after the `wait' in `wait_for_inferior'.  Used for
   i860. */

int one_stepped;

/* single_step() is called just before we want to resume the inferior,
   if we want to single-step it but there is no hardware or kernel single-step
   support.  We find all the possible targets of the coming instruction and 
   breakpoint them.

   single_step is also called just after the inferior stops.  If we had
   set up a simulated single-step, we undo our damage.  */
/* Note that we don't need the parameter, but it's dictated as part of the interface. */
void
   single_step (signal)
int signal;
{
   CORE_ADDR pc;
   branch_type place_brk();
   int *shadow0, *shadow1, *shadow2, *shadow3;

   shadow0 = (int *) &brk.shadow_contents[0];
   shadow1 = (int *) &brk.shadow_contents[4];
   shadow2 = (int *) &brk.shadow_contents[8];
   shadow3 = (int *) &brk.shadow_contents[12];
   pc = read_register (PC_REGNUM);
   
   if (!one_stepped)
      {
         brk.address = pc;
         place_brk (pc, SINGLE_STEP_MODE, &brk);
	 *shadow0 = *shadow1 = *shadow2 = *shadow3 = 0;
         
         if (brk.mode == DIM)
            {
               if (btdebug != 0)
                  {
                     btdebug_message(" DIM1 -> %x : ", brk.act_addr[3]);
                     print_insn( brk.act_addr[3], stderr);
                     btdebug_message("\t -|-  %x : ", brk.act_addr[2]);
                     print_insn( brk.act_addr[2], stderr);
                     btdebug_message("\n");
                  }
               if (( brk.address1 != NULL))
                  { 
                     adj_read_memory  (brk.act_addr[2], shadow2,
                                       INSTRUCTION_LENGTH);
                     adj_write_memory (brk.act_addr[2], break_insn, INSTRUCTION_LENGTH);
                     adj_read_memory  (brk.act_addr[3], shadow3,
                                       INSTRUCTION_LENGTH);
                     /*		adj_write_memory (brk.act_addr[3], float_insn,
                              INSTRUCTION_LENGTH); */
                     
                  }
               if (btdebug != 0)
                  {
                     if ( brk.address1 != 0)
                        btdebug_message(" DIM2 ->");
                     else
                        btdebug_message(" DIM1 ->");
               
                     btdebug_message(" %x : ", brk.act_addr[1]);
                     print_insn( brk.act_addr[1], stderr);
                     btdebug_message("\t -|-  %x : ", brk.act_addr[0]);
                     print_insn( brk.act_addr[0], stderr);
                     btdebug_message("\n");
                  }
               
               adj_read_memory  (brk.act_addr[0], shadow0,
                                 INSTRUCTION_LENGTH);
               adj_write_memory (brk.act_addr[0], break_insn,
                                 INSTRUCTION_LENGTH);
               adj_read_memory  (brk.act_addr[1], shadow1,
                                 INSTRUCTION_LENGTH);
               /*		adj_write_memory (brk.act_addr[1], float_insn,
                                 INSTRUCTION_LENGTH); */
               
            } 
         else	{
            if (brk.address1 != NULL)
               {
                  if (btdebug)
                     {
                        btdebug_message(" SIM1 ->");
                        btdebug_message(" %x : ", brk.act_addr[2]);
                        print_insn( brk.act_addr[2], stderr);
                        btdebug_message("\n");
                     }
                  adj_read_memory  (brk.act_addr[2], shadow2,
                                    INSTRUCTION_LENGTH);
                  adj_write_memory (brk.act_addr[2], break_insn, INSTRUCTION_LENGTH);
               }
            if (btdebug)
               {
                  if ( brk.address1 != NULL)
                     btdebug_message(" SIM2 ->");
                  else
                     btdebug_message(" SIM1 ->");
                  
                  btdebug_message(" %x : ", brk.act_addr[0]);
                  print_insn( brk.act_addr[0], stderr);
                  btdebug_message("\n");
               }
            adj_read_memory  (brk.act_addr[0], shadow0,
                              INSTRUCTION_LENGTH);
            adj_write_memory (brk.act_addr[0], break_insn,INSTRUCTION_LENGTH);
         }
         
         /* Let it go */
         one_stepped = 1;
         return;
      }
   else
      {
         /* Remove breakpoints */
         if (brk.mode == DIM)
            {
               adj_write_memory (brk.act_addr[0], shadow0,
                                 INSTRUCTION_LENGTH);
               adj_write_memory (brk.act_addr[1], shadow1,
                                 INSTRUCTION_LENGTH);
            } else	{
               adj_write_memory (brk.act_addr[0], shadow0,
                                 INSTRUCTION_LENGTH);
            }
         
         if (brk.address1 != NULL)
            {
               if (brk.mode == DIM)
                  {
                     adj_write_memory (brk.act_addr[2], shadow2,
                                 INSTRUCTION_LENGTH);
                     adj_write_memory (brk.act_addr[3], shadow3,
                                 INSTRUCTION_LENGTH);
                  } else	{
                     adj_write_memory (brk.act_addr[2], shadow2,
                                 INSTRUCTION_LENGTH);
                  }
            }
         one_stepped = 0;
      }
}



/* Written for i860 by Jim Hanko (hanko@orc.olivetti.com) */
/* This code was based on SPARC code written by Gary Beihl (beihl@mcc.com),
   by Michael Tiemann (tiemann@corto.inria.fr).  */
/* This routine returns the first memory address following the prologue code,
   if there is a prologue. */

struct command_line *get_breakpoint_commands ();

CORE_ADDR 
   skip_prologue (pc)
CORE_ADDR pc;
{
   CORE_ADDR instr;
   int regno;
   
   instr =  (unsigned)(adj_read_memory_integer (pc));
   
   /* Recognize "addu|adds -X,sp,sp" insn. */
   if ((instr & 0xEFFF0000) == 0x84420000)
      {
         pc += INSTRUCTION_LENGTH;
         instr =  (unsigned)(adj_read_memory_integer (pc));
      }
   else
      return(pc);					/* No frame! */
   
   /* Recognize store of return addr and frame pointer into frame */
   for (; ;)
      {
         if ((instr & 0xFFE0F801) == 0x1C400801 ||  /* st.l r1,X(sp) */
             (instr & 0xFFE0F801) == 0x1C401801)    /* st.l fp,X(sp) */
            {
               pc += INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
         else
            break;
      }
   
   /* Recognize "addu|adds X,sp,fp" insn. */
   if ((instr & 0xEFFF0000) == 0x84430000)
      {
         pc += INSTRUCTION_LENGTH;
         instr =  (unsigned)(adj_read_memory_integer (pc));
      }
   
   /* Now recognize stores into the frame from the registers. */
   
   for (; ;)
      {
         if ((instr & 0xFFA00003) == 0x1C200001 ||	/* st.l rn,X(fp|sp) */
             (instr & 0xFFA00001) == 0x4C200000)	/* fst.y fn,X(fp|sp) */
            {
               regno = (instr >> 11) & 0x1f;
               if (regno == 0)			/* source reg == 0? quit */
                  break;
               pc += INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
         else
            break;
      }
   
   return(pc);
}

#if 0
/* This routine is uncalled.  Remove it sometime. */
/* Set *nextpc to branch target if we find a branch.  If it is not a branch, 
   set it to the next instruction (addr + 4) */


branch_type
   isabranch (addr,  nextpc)
CORE_ADDR addr, *nextpc;
{
   CORE_ADDR instr;
   branch_type val = not_branch;
   long offset; /* Must be signed for sign-extend */
   
   BTDEBUG(" isabranch\n");
   *nextpc = addr;
   instr =  (unsigned)(adj_read_memory_integer (addr));
   
   if ((instr & 0xE0000000) == 0x60000000 &&		/* CTRL format */
       (instr & 0xF8000000) != 0x60000000)		/* not pfld.y  */
      {
         if ((instr & 0xF8000000) == 0x68000000)		/* br or call */
            val = uncond_d;
         else if ((instr & 0xF4000000) == 0x74000000)	/* bc.t or bnc.t */
            val = cond_d;
         else if ((instr & 0xF4000000) == 0x70000000)	/* bc or bnc */
            val = cond;
         
         offset = (instr & 0x03ffffff);
         if (offset & 0x02000000)	/* sign extend? */
            offset |= 0xFC000000;
         *nextpc = addr + 4 + (offset << 2);
      }
   else if ((instr & 0xFC00003F) == 0x4C000002 ||	/* calli */
            (instr & 0xFC000000) == 0x40000000)		/* bri */
      {
         val = uncond_d;
         offset = ((instr & 0x0000F800) >> 11);
         *nextpc = (read_register(offset) & 0xFFFFFFFC);
      }
   else if ((instr & 0xF0000000) == 0x50000000)		/* bte or btne */
      {
         val = cond;
         
         offset = SIGN_EXT16(((instr & 0x001F0000) >> 5)  | (instr & 0x000007FF));
         *nextpc = addr + 4 + (offset << 2);
      }
   else if ((instr & 0xFC000000) == 0xB4000000)         /* bla */
      {
         val = cond_d;
         
         offset = SIGN_EXT16(((instr & 0x001F0000) >> 5)  | (instr & 0x000007FF));
         *nextpc = addr + 4 + (offset << 2);
      }
   
   BTDEBUG(" Final addr - %x\n", *nextpc);
   /*BTDEBUG("isabranch ret: %d\n",val); */
   return val;
}
#endif

/* set in call_function() [valops.c] to the address of the "call dummy" code
   so dummy frames can be easily recognized; also used in wait_for_inferior() 
   [infrun.c]. When not used, it points into the ABI's 'reserved area' */

CORE_ADDR call_dummy_set = 0;	/* true if dummy call being done */
CORE_ADDR call_dummy_start;	/* address of call dummy code */

/* this routine routine gets the values of the registers stored in the frame
   and stores their values into the frame_saved_regs structure. */

void 
frame_find_saved_regs(frame_info, frame_saved_regs)
     struct frame_info *frame_info;
     struct frame_saved_regs *frame_saved_regs;
{
   register CORE_ADDR pc;
   CORE_ADDR instr;
   long offset, spdelta = 0;
   int i, size, reg;
   int r1_off = -1, fp_off = -1;
   int framesize;
   
   bzero (frame_saved_regs, sizeof(*frame_saved_regs));
   
   if (call_dummy_set && frame_info->pc >= call_dummy_start && 
       frame_info->pc <= call_dummy_start + CALL_DUMMY_LENGTH)
      {
         /* DUMMY frame - all registers stored in order at fp; old sp is
            at fp + NUM_REGS*4 */
         
         for (i = 1; i < NUM_REGS; i++) /* skip reg 0 */
            /* the register numbers used in the instruction and the ones used to index
            the regs array are not the same -- compensate */
            frame_saved_regs->regs[i+R0] = frame_info->frame + i*REGISTER_LENGTH;
         
         call_dummy_set = 0;
         return;
      }
   
   pc = get_pc_function_start (frame_info->pc); 
   if (pc == NULL)
     pc = find_entry_start (frame_info->pc);

   if (pc != NULL)
      {
         instr =  (unsigned)(adj_read_memory_integer (pc));
         /* Recognize "addu|adds -X,sp,sp" insn. */
         if ((instr & 0xEFFF0000) == 0x84420000)
            {
               framesize = -SIGN_EXT16(instr & 0x0000FFFF);
               pc += INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
      }
   else
      goto punt;					/* No frame! */
   
   /* Recognize store of return addr and frame pointer into frame */
   for (; ;)
      {
         if ((instr & 0xFFE0F801) == 0x1C400801)  /* st.l r1,X(sp) */
            {
               r1_off = SIGN_EXT16(((instr&0x001F0000) >> 5) | (instr&0x000007FE));
               pc += INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
         else if ((instr & 0xFFE0F801) == 0x1C401801)    /* st.l fp,X(sp) */
            {
               fp_off = SIGN_EXT16(((instr&0x001F0000) >> 5) | (instr&0x000007FE));
               pc += INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
         else
            break;
      }
   
   /* Recognize "addu|adds X,sp,fp" insn. */
   if ((instr & 0xEFFF0000) == 0x84430000)
      {
         spdelta = SIGN_EXT16(instr & 0x0000FFFF);
         pc += INSTRUCTION_LENGTH;
         instr =  (unsigned)(adj_read_memory_integer (pc));
      }
   
   /* Now recognize stores into the frame from the registers. */
   
   for (; ;)
      {
         if ((instr & 0xFFC00003) == 0x1C400001)	/* st.l rn,X(fp|sp) */
            {
               offset = SIGN_EXT16(((instr&0x001F0000) >> 5) | (instr&0x000007FE));
               reg = (instr >> 11) & 0x1F;
               if (reg == 0)
                  break;
               if ((instr & 0x00200000) == 0)	/* was this using sp? */
                  if (spdelta != 0)			/* and we know sp-fp delta */
                     offset -= spdelta;		/* if so, adjust the offset */
                  else
                     break;				/* if not, give up */
               
               
               /* Handle the case where the return address is stored after the fp 
                  is adjusted */
               
               if (reg == 1)
                  frame_saved_regs->regs[PC_REGNUM] = frame_info->frame + offset;
               else
                  frame_saved_regs->regs[reg+R0] = frame_info->frame + offset;
               
               pc +=  INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
         else if ((instr & 0xFFC00001) == 0x2C400000) /* fst.y fn,X(fp|sp) */
            {
               /*
                * The number of words in a floating store based on 3 LSB of instr
                */
               static int fst_sizes[] = {2, 0, 1, 0, 4, 0, 1, 0};
               
               size = fst_sizes[instr & 7];
               reg = ((instr >> 16) & 0x1F) + FP0_REGNUM;
               if (reg == 0)
                  break;
               
               if (size > 1)					/* align the offset */
                  offset = SIGN_EXT16(instr & 0x0000FFF8);	/* drop 3 bits */
               else
                  offset = SIGN_EXT16(instr & 0x0000FFFC);	/* drop 2 bits */
               
               if ((instr & 0x00200000) == 0)	/* was this using sp? */
                  if (spdelta != 0)			/* and we know sp-fp delta */
                     offset -= spdelta;		/* if so, adjust the offset */
                  else
                     break;				/* if not, give up */
               
               for (i = 0; i < size; i++)
                  {
                     frame_saved_regs->regs[reg] = frame_info->frame + offset;
                     
                     offset +=  REGISTER_LENGTH;
                     reg++;
                  }
               
               pc +=  INSTRUCTION_LENGTH;
               instr =  (unsigned)(adj_read_memory_integer (pc));
            }
         else
            break;
      }
   
   punt: ;
   if (framesize != 0 && spdelta != 0)
      frame_saved_regs->regs[SP_REGNUM] = frame_info->frame+(framesize-spdelta);
   else
      frame_saved_regs->regs[SP_REGNUM] = frame_info->frame + 8;
   
   if ((spdelta != 0) && fp_off != -1)
      frame_saved_regs->regs[FP_REGNUM] = frame_info->frame - spdelta + fp_off;
   else
      frame_saved_regs->regs[FP_REGNUM] = frame_info->frame;
   
   if ((spdelta != 0) && r1_off != -1)
      frame_saved_regs->regs[PC_REGNUM] = frame_info->frame - spdelta + r1_off;
   else
      frame_saved_regs->regs[PC_REGNUM] = frame_info->frame + 4;
}


/* get the frame pointer of the caller.
 * note that only routines that have been compiled with
 * -g have full (XX)fp style stack frames
 * if we are not returning to a non -g caller then we
 * return the sp at entry to us as it is the caller's
 * frame reference.
 */

frame_chain(thisframe)
     FRAME thisframe;
{
   CORE_ADDR fp, pc;
   CORE_ADDR func_start;
   CORE_ADDR instr;
   int offset;
   CORE_ADDR thisfp = thisframe->frame;
   struct frame_saved_regs fsr;   
   CORE_ADDR thissp;
           
      /* get the frame pointer actually sp for  a non -g
    * for the routine that called us routine 
    */
   
   BTDEBUG("FRAME_CHAIN(%x)\n",thisframe);
   
   if ( !read_memory_integer (thisframe->frame,sizeof(long)) )
      {
         return (0);
      }
   
   if( ! g_routine(thisframe->pc) ){
      thissp = get_saved_basereg (thisframe, SP_REGNUM);

      BTDEBUG( "non g at %x\n",thisframe->pc);
      caller_pc(thisframe->pc, thissp, thisfp,&pc,&fp, 1);
      BTDEBUG("caller_pc returned %x %x \n",pc,fp);
      return(fp);
      
   }/* else a -g routine */
   
   
   fp = read_memory_integer (thisfp, sizeof(long));
   
   if (fp < thisfp || fp > (unsigned) STACK_END_ADDR)
      {
         /* handle the Metaware-type pseudo-frame */
         
         func_start = get_pc_function_start(thisframe->pc);
	 if (func_start == NULL)
	   func_start = find_entry_start (thisframe->pc);

         if (func_start != NULL)
            {
               
               instr =  (unsigned)(adj_read_memory_integer (func_start));
               /* Recognize "addu|adds -X,sp,sp" insn. */
               if ((instr & 0xEFFF0000) == 0x84420000)
                  offset = SIGN_EXT16(instr & 0x0000FFFF);
               
            }
         
         fp = 0;
         if (offset < 0)
            fp = thisfp - offset;
      }
   BTDEBUG("frame_chain returned %d\n",fp);
   return(fp);
}

/* This function returns 1 if there is no stored r1, 0 otherwise.
   The function returns 1 if the pc is in a function prologue,
   or the function prologue didn't save the return pointer in
   the stack frame,  0 otherwise */

int no_stored_rp(CORE_ADDR pc)
{
   CORE_ADDR func_start, prologue_end;
   
   func_start = get_pc_function_start(pc);
   if (func_start == NULL)
     func_start = find_entry_start (pc);

   if (func_start != NULL)
      {
         prologue_end = func_start;
         SKIP_PROLOGUE(prologue_end);
         if ( (pc >= func_start) && (pc <= prologue_end))
            {
               BTDEBUG("no_stored_rp: pc %x is in prologue \n",pc);
               return 1;
            }
         /* otherwise, see if the entry sequence stored the return pointer.
            If it didn't, return 1 */
         /* Some procedures , at least, store the return pointer AFTER 
            the prologue sequence, so check for stores from function start to
            present pc value. */
         if (!has_stored_r1(func_start, pc))
            {  
               BTDEBUG("no_stored_rp, for pc %x, prologue didn't store r1\n",pc);
               return 1;
            }
      }
   BTDEBUG("no_stored_rp for pc %x return pointer was stored \n", pc);

   return 0; 
}

/* get the PC of the caller */
CORE_ADDR frame_saved_pc(frame_struct)
FRAME frame_struct;
{
     CORE_ADDR frame;
     CORE_ADDR pc;
     CORE_ADDR pc1;
     CORE_ADDR sp ;
     CORE_ADDR fp;
     struct frame_saved_regs fsr;                                                 
     
     frame = frame_struct->frame;
     pc = frame_struct->pc;
     
     
     BTDEBUG("frame_saved_pc input: frame %x, pc %x",
           frame, pc);

     /* First see if this is the current frame. If it is, return the value in r1,
        as it may not have been stored */

     fp = read_register(FP_REGNUM);

     /* check to see if we are in an entry sequence, where the return pointer has not yet been stored */
     if (fp == frame &&  no_stored_rp(pc))
        {
           pc = read_register(RP_REGNUM);
           frame_struct->rp = pc;
        }
     else if( ! g_routine(pc) )
        {
           sp = get_saved_basereg (frame_struct, SP_REGNUM);

           caller_pc(pc,sp,frame_struct->frame, &pc,&frame, 0);
        }
     else 
        {
           
           pc = read_memory_integer (frame + 4, sizeof(long)); 
   
           if (inside_entry_file(pc))
              {
                 
                 BTDEBUG("pc %x outside entry file \n",pc);

                 pc1 = read_memory_integer (frame, sizeof(long));
               
                 if (!inside_entry_file(pc1))
                    pc = pc1;
                 else
                    pc = 0;
              }
        } 
     BTDEBUG(" returning pc %x\n", CLEAN_PC(pc));
     return(CLEAN_PC(pc));

  }

/* Pass arguments to a function in the inferior process - ABI compliant
   Modified by Peggy Fieland (Margaret_Fieland@vos.stratus.com) to account
   for newer ABI conventions. Note that now, unfortunately, we MUST KNOW
   if we expect a float or a double.  For now, we will assume that the
   caller of this routine has the types of these arguments correct.... 
   NOTE THAT THIS ROUTINE DOES NO ARGUMENT COERCION -- it's all in the
   caller.
   Modified by Peggy Fieland to handle memory argument lists.
   */

#define IS_EVEN_REG(fl) (((fl - FP0_REGNUM)%2) == 0)
CORE_ADDR 
pass_function_arguments(args, nargs, struct_return, struct_addr, sp)
     value *args;
     int nargs;
     int struct_return;
     CORE_ADDR struct_addr;
     CORE_ADDR sp;
{
   int ireg = (struct_return) ? R17 : R16;
   int freg = F8;
   int i;
   struct type *type;
   value arg;
   signed long tmp;
   unsigned long ul_tmp;
   signed short s_tmp;
   unsigned short us_tmp;
   signed char c_tmp;
   unsigned char uc_tmp;
   CORE_ADDR arg_ptr;
   int len;

   if (struct_return)
      {
         write_register(R16, struct_addr);
      }

   arg_ptr = sp; /* Space was allocated for memory argument list in i860_arg_coerce */

   /* Loop through the arguments, putting the values in a register or memory as appropriate. */
   
   for (i = 0; i < nargs; i++) 
      {
         arg = args[i];
         type = VALUE_TYPE(arg);
         len = TYPE_LENGTH(type);
         if (type == builtin_type_double) 
            {
               /* see ABI . Note freg MUST BE INCREMENTED even if arg goes into the
                  memory argument list for this code to work correctly for subsequent
                  arguments. */
               if (!IS_EVEN_REG(freg))
                  freg += 1;
               /* see if argument can be put in a register, or whether it must go
                  into the memory argument list */
               if (freg < F8 + NUM_FLOAT_ARG_REGS)
                  {
                     /* It can go in a register */
                     bcopy(VALUE_CONTENTS(arg), &tmp, sizeof(double));
                     write_register_bytes(REGISTER_BYTE(freg), (char *) &tmp, TYPE_LENGTH(type));
                     freg += 2;
                  }
               else
                  {
                     /* It goes into memory argument list */
                     arg_ptr = ALIGN_ARG( arg_ptr, sizeof(double));
                     write_memory (arg_ptr, VALUE_CONTENTS (arg), len);
                     arg_ptr += len;
                  }
                  
            }
         else if (type == builtin_type_float)
            {
               if (freg < F8 + NUM_FLOAT_ARG_REGS)
                  {
                     /* It can go in a register */
                     bcopy(VALUE_CONTENTS(arg), &tmp, sizeof(long));
                     write_register_bytes (REGISTER_BYTE(freg), (char *) &tmp, TYPE_LENGTH(type));
                     freg++;
                  }
               else
                  { 
                     /* It goes into the memory argument list */
                     arg_ptr = ALIGN_ARG(arg_ptr, sizeof(float));
                     write_memory (arg_ptr, VALUE_CONTENTS (arg), len);
                     arg_ptr += len;
                  }
            }
         else
            {
               /* All structs are passed by value, and hence they all go into the memory
                  argument list (see ABI); otherwise, as above, see if we have run
                  out of registers */
               
               /* Cast value correctly so we can load it into a register or into the
                  memory argument list  -- see ABI */
               if (TYPE_LENGTH(type)    < sizeof(long))
                  {
                     if (TYPE_FLAGS(type) & TYPE_FLAG_UNSIGNED)
                        arg = value_cast(builtin_type_unsigned_int, arg);
                     else 
                        arg = value_cast (builtin_type_int, arg);
                     type = VALUE_TYPE(arg);
                     len = TYPE_LENGTH(type);
                  }
               
               if ((TYPE_CODE(type) == TYPE_CODE_STRUCT) || (ireg >= R16 + NUM_INT_ARG_REGS))
                  {
                     /* It goes into the memory argument list. Minimum alignment requirements
                        are on a 4-byte boundary */
                     
                     if  ((TYPE_CODE(type) == TYPE_CODE_INT) ||
                          (TYPE_CODE(type) == TYPE_CODE_ENUM) ||
                          (TYPE_CODE(type) == TYPE_CODE_CHAR) ||
                          (TYPE_CODE(type) == TYPE_CODE_BOOL))
                        arg_ptr = ALIGN_ARG(arg_ptr, len);
                     else
                        arg_ptr = ALIGN_ARG (arg_ptr, sizeof(long)); /* align on 4-byte boundary */
                     write_memory (arg_ptr, VALUE_CONTENTS (arg), len);
                     arg_ptr += len;
                  }
               else
                  {

                     bcopy(VALUE_CONTENTS(arg), &tmp, sizeof(long));
                     write_register(ireg, tmp);
                     ireg++;
                  }
                  
            }
      }
   
   
   return (sp);

}


#define SPACES		"       "
#define P_SPACES	"   "
#define BYTE 0xff

int screen_lines=24;

char *spec_reg[] = {
   "fsr", "db", "dirbase", "fir", "psr", "epsr",
};

char *doro_reg[] = {
   "scp", "cbsp", "pt_cs", "intmsk", "intack",
};
#define NREGS 32


get_reg(regno)
{
   char raw_buffer[32];
   int addr;
   int virtual_buffer;

   /* NOTE that only  integer and floating point registers can be relative to a frame */

   if ((regno >= R0) && (regno <= F31)) /* user register */
      read_relative_register_raw_bytes (regno, raw_buffer);
   else
      bcopy (&registers[regno << 2], raw_buffer, sizeof (long));

   REGISTER_CONVERT_TO_VIRTUAL (addr, raw_buffer, &virtual_buffer);
   return(virtual_buffer);
}


#if 0
/* This routine is uncalled.  Remove it sometime. */

/*
 ** Figure out whether we are in a delayed slot and if so then take necessary
 **	action to resume properly - remember trap pre-empts instruction
 */
int
   wasabranch (addr, nextpc, ss)
CORE_ADDR addr, *nextpc;
int ss;
{
   CORE_ADDR nextadr, instr;
   int val = not_branch;
   long offset; /* Must be signed for sign-extend */
   
   if (ss)
      {
         if (INDIM)
            {
			nextadr = CORADR((int)(addr +  INSTRUCTION_LENGTH*2));
  			instr	=  (unsigned)(adj_read_memory_integer (CORADR(addr)));
            }
         else
            {
			nextadr = addr +  INSTRUCTION_LENGTH;
  			instr	=  (unsigned)(adj_read_memory_integer (addr));
            }
      } else	{
         if (ISDIM(addr))
            {
			nextadr = CORADR(addr);
  			instr	=  (unsigned)(adj_read_memory_integer (nextadr));
            }
         else
            {
			nextadr = addr;
  			instr	=  (unsigned)(adj_read_memory_integer (addr));
            }
      }
   
   
   if ((instr & 0xE0000000) == 0x60000000 &&		/* CTRL format */
       (instr & 0xF8000000) != 0x60000000)		/* not pfld.y  */
      {
         if ((instr & 0xF8000000) == 0x68000000)		/* br or call */
            val = uncond_d;
         else if ((instr & 0xF4000000) == 0x74000000)	/* bc.t or bnc.t */
            val = cond_d;
         else if ((instr & 0xF4000000) == 0x70000000)	/* bc or bnc */
            val = cond;
         
         offset = (instr & 0x03ffffff);
         if (offset & 0x02000000)	/* sign extend? */
            offset |= 0xFC000000;
         nextadr += (offset << 2);
      }
   else if ((instr & 0xFC00003F) == 0x4C000002 ||	/* calli */
            (instr & 0xFC000000) == 0x40000000)		/* bri */
      {
         if (ss)
            {
               val	= uncond_d;
      		offset	= ((instr & 0x0000F800) >> 11);
      		nextadr	= (read_register(offset) & 0xFFFFFFFC);
            } else	{
               val	= uncond_d;
            }
      }
   else if ((instr & 0xF0000000) == 0x50000000)		/* bte or btne */
      {
         val = cond;
         
         offset = SIGN_EXT16(((instr & 0x001F0000) >> 5)  | (instr & 0x000007FF));
         nextadr += (offset << 2);
      }
   else if ((instr & 0xFC000000) == 0xB4000000)         /* bla */
      {
         val = cond_d;
         
         offset = SIGN_EXT16(((instr & 0x001F0000) >> 5)  | (instr & 0x000007FF));
         nextadr += (offset << 2);
      }
   
   *nextpc = nextadr;
   return val;
}
#endif


/* i860-specific routine to print the register set. Note that we ALWAYS print information
   on the floating point registers, so we ignore the parameter fpregs. 
   NOTE also  that only  integer and floating point registers can be relative to a frame --
   see subroutine get_reg (above ) */

void i860_do_registers_info(regnum,fpregs)
     int regnum;
     int fpregs;
{
   register int i;
   unsigned int val;
   unsigned int j,k;
   
   
   if (regnum == -1)
      printf_filtered (
                       "Register       Contents (relative to selected stack frame)\n\n");
   
   if (regnum != -1) /* print one register */
      {
         val = get_reg(regnum);
         printf("%-4s 0x%08x\t", reg_names[regnum], val);
         printf("\n\t"); fflush(stdout);
      }
   else	/* print all registers */
      {
         
         printf("\n Control/Status Registers :- \n\t");
         for (j=0; j<=DB; j++)
            {
               val = get_reg(j);
               printf("%-4s 0x%08x\t", reg_names[j], val);
            }
         printf("\n\t"); fflush(stdout);
         
         /* EPSR */
         val = get_reg(EPSR);
         printf("%-4s 0x%08x\t", reg_names[EPSR], val);
         
         /* FSR */
         val = get_reg(FSR);
         printf("%-4s 0x%08x\t", reg_names[FSR], val);
         
         /* CCR */
         val = get_reg(CCR);
         printf("%-4s 0x%08x\t", reg_names[CCR], val);
         /* BEAR*/
         val = get_reg(BEAR);
         printf("%-4s 0x%08x\t", reg_names[BEAR], val);
         
         
#ifdef JIM_ADD_PRIV
         for (j=P0; j<=P3; j++)
            {
               val = get_reg(j);
               printf("%-4s 0x%08x\t", reg_names[j], val);
            }
#endif
         
         printf("\n Integer Registers :- \n\t");
         for (j=R0; j<=R31; j++)
            {
               if (j != IREGS  && (j % REGISTER_LENGTH == 0)) 
                  {
                     printf("\n\t");  fflush(stdout);
                  }
               val = get_reg(j);
               printf("%-4s 0x%08x\t", reg_names[j], val);
            }
         
         printf("\n Floating Registers :- \n\t");
         for (j=F0; j<=F31; j++)
            {
               if (j != FREGS && (j % REGISTER_LENGTH == 0)) 
                  {
                     printf("\n\t");  fflush(stdout);
                  }
               val = get_reg(j);
               printf("%-4s 0x%08x\t", reg_names[j], val);
            }
         
         printf("\n Special Registers :- \n\t");
         for (j=SPC_KI; j<=SPC_MERGE; j+=2)
            {
               unsigned int valh;
               if (j == SPC_T)
                  {
                     printf("\n\t");  fflush(stdout);
                  }
               val = get_reg(j);
               valh = get_reg(j+1);
               printf("%-6s 0x%08x %08x\t", reg_names[j], val,valh);
            }
         
         printf("\n Graphics Pipeline :- \n");
         {
            unsigned int valh;
            j = PSV_I1;
            val = get_reg(j);
            valh = get_reg(j+1);
            printf("\t\t\t%-8s 0x%08x %08x \n", reg_names[j], val,valh);
         }
         
         printf(" Memory Load Pipeline :- \n");
         for (j=PSV_L1; j<=PSV_L3; j+=REGISTER_LENGTH)
            {
               unsigned int valh, val2,val3;
               
               val = get_reg(j);
               valh = get_reg(j+1);
               val2 = get_reg(j+2);
               val3 = get_reg(j+3);

               printf("\t\t%-8s 0x%08x %08x %08x %08x\n", reg_names[j], 
                      val,valh,val2,val3);
            }
         
         printf("\n Adder Pipeline :-\t\tMultiplier Pipeline :-\t\tFSR results :-\n");
         for (i=PSV_FSR1,j=PSV_A1,k=PSV_M1; j<=PSV_A3; i++,j+=2,k+=2)
            {
               unsigned int valh,val2,val3,val4;

               val4 = get_reg(i);
               val = get_reg(j);
               valh = get_reg(j+1);
               val2 = get_reg(k);
               val3 = get_reg(k+1);

               printf("  %-4s 0x%08x %08x\t", reg_names[j], val,valh);
               printf("%-4s 0x%08x %08x\t", reg_names[k], val2,val3);
               printf("%-4s 0x%08x\n", reg_names[i], val4);
            }
         
      }
   
   
}



/* The following set of routines was adapted from existing code previously
   in an i860-specific version of breakpoint.c by Peggy Fieland
   (Margaret_Fieland@vos.stratus.com) */
/* routines to set a data breakpoint by setting the value in the DB register. 
   Note that "hitting" the breakpoint will generate a data access trap. We
   do not have a special trap handler. */
unsigned int dbrkval, dbrkmod;
void i860_dbrk_breakpoint()
{
   BTDEBUG("i860_dbrk_breakpoint was called , dbrkval %x\n", dbrkval);
   
   if (dbrkval != 0)
      {
         *(int *)&registers[DB<<2] = dbrkval;
      }
   else
      {
         *(int *)&registers[DB<<2] = 0;
      }

   *(int *)&registers[PSR<<2] &= ~3;
   *(int *)&registers[PSR<<2] |= dbrkmod;

   store_inferior_registers(DB);
   store_inferior_registers(PSR);

}

/* set a "read" data breakpoint. */
void
d_ro_break_command(char *arg, int num)
{
	dbrkval = strtoul(arg, NULL, 0);
	dbrkmod = 0x01;
     BTDEBUG(" ro_dbreak - %x %x\n", dbrkval, dbrkmod);
}

/* set a "write" data breakpoint. */
void
d_wo_break_command(char *arg, int num)
{
	dbrkval = strtoul(arg, NULL, 0);
	dbrkmod = 0x02;
     BTDEBUG(" wo_dbreak - %x %x\n", dbrkval, dbrkmod);
}

/* set a "read/write" data breakpoint. */
void
d_rw_break_command(char *arg, int num)
{  	
  dbrkval  =  strtoul(arg, NULL,   0);
  dbrkmod =  0x03;
  BTDEBUG(" rw_dbreak - %x %x\n", dbrkval, dbrkmod);
 }

/* clear data breakpoint. */
 void clear_dbreak(char *arg, int num)
 { 
   dbrkval  = 0;
   dbrkmod = 0;
 }

/*   i860-specific  breakpoint initialization.    Includes  adding the
i860-specific     data          breakpoint       commands.   */   
void i860_init_breakpoints() 
{ 
  dbrkval = dbrkmod = 0;
  add_com ("dbro", class_breakpoint, d_ro_break_command,
	   "Set a data breakpoint READ ONLY, 32-bit data element.");
  add_com ("dbwo", class_breakpoint, d_wo_break_command,
	   "Set a data breakpoint WRITE ONLY, 32-bit data element.");
  add_com ("dbrw", class_breakpoint, d_rw_break_command,
	   "Set a data breakpoint READ/WRITE, 32-bit data element.");
  add_com ("dclear", class_breakpoint, clear_dbreak,
	   "clear the current data breakpoint.");
  add_com_alias ("dc", "dclear", class_breakpoint, 1);

}

/* i860-specific code to insert a breakpoint. */
int i860_insert_breakpoint(b)
struct breakpoint *b;
{
   int val;
   int *shadow0, *shadow1, *shadow2, *shadow3;

   shadow0 = (int *)&b->shadow_contents[0];
   shadow1 = (int *)&b->shadow_contents[4];
   shadow2 = (int *)&b->shadow_contents[8];
   shadow3 = (int *)&b->shadow_contents[12];

   place_brk( b->address, BREAK_MODE, b );

   if (b->mode == DIM)
      {
         
         adj_read_memory (b->act_addr[0], shadow0,  INSTRUCTION_LENGTH);
         val =  adj_write_memory (b->act_addr[0], break_insn,  INSTRUCTION_LENGTH);
         if (val != 0 ) return val;
         adj_read_memory (b->act_addr[1], shadow1,  INSTRUCTION_LENGTH);
         /*	val = adj_write_memory (b->act_addr[1], float_insn,  INSTRUCTION_LENGTH); */
         if (val != 0) return val;
      } 
   else	
      {
         adj_read_memory (b->act_addr[0], shadow0,  INSTRUCTION_LENGTH);
         val = adj_write_memory (b->act_addr[0], break_insn,  INSTRUCTION_LENGTH);
      }
   if (b->address1 != 0)
      {
         if (b->mode == DIM)
            {

               adj_read_memory (b->act_addr[2], shadow2,  INSTRUCTION_LENGTH);
               val = adj_write_memory (b->act_addr[2], break_insn,  INSTRUCTION_LENGTH);
               if (val) return val;
               adj_read_memory (b->act_addr[3], shadow3,  INSTRUCTION_LENGTH);
               /*	val = adj_write_memory (b->act_addr[3], float_insn,  INSTRUCTION_LENGTH); */
               if (val != 0) return val;
            }
         else	
            {
               adj_read_memory (b->act_addr[2], shadow0,  INSTRUCTION_LENGTH);
               val = adj_write_memory (b->act_addr[2], break_insn,  INSTRUCTION_LENGTH);
            }
      }
   if (val != 0)
      return val;

   b->inserted = 1;
   return 0;
}

int i860_remove_breakpoint(b)
struct breakpoint *b;
{
   int val;
   int *shadow0, *shadow1, *shadow2, *shadow3;

   shadow0 = (int *)&b->shadow_contents[0];
   shadow1 = (int *)&b->shadow_contents[4];
   shadow2 = (int *)&b->shadow_contents[8];
   shadow3 = (int *)&b->shadow_contents[12];


   if (b->inserted)
      {
         if (b->mode == DIM)
            {
               val =adj_write_memory (b->act_addr[0], shadow0,
                                      INSTRUCTION_LENGTH);
               val =adj_write_memory (b->act_addr[1],shadow1,
                                       INSTRUCTION_LENGTH);
               if (b->address1 != NULL)
                  {
                     val =adj_write_memory (b->act_addr[2],shadow2,
                                             INSTRUCTION_LENGTH);
                     val =adj_write_memory (b->act_addr[3], shadow3,
                                             INSTRUCTION_LENGTH);
                  }
            } 
         else	
            {
               val =adj_write_memory (b->act_addr[0], shadow0,
                                       INSTRUCTION_LENGTH);
               if (b->address1 != NULL)
                  {
                     val =adj_write_memory (b->act_addr[2],shadow0,
                                            INSTRUCTION_LENGTH);
                  }
            }
         if (val != 0)
            return val;
         b->inserted = 0;
      }

   return 0;


}


#ifdef USE_PROC_FS	/* Target dependent support for /proc */

#include <sys/procfs.h>

/* The following routines were added by Peggy Fieland (Margaret_Fieland@vos.stratus.com)
They were adapted from the m-68k versions of the routines .*/

/*  Given a pointer to a floating point register set in /proc format
    (fpregset_t *), unpack the register contents and supply them as gdb's
    idea of the current floating point register values. */

void 
supply_fpregset (fpregsetp)
fpregset_t *fpregsetp;
{
  register int regno;
  
  BTDEBUG("supply_fregset called \n");
  
  for (regno = F0 ; regno <= F31 ; regno++)
    {
      supply_register (regno, (char *) &(fpregsetp -> fpu.r_freg[regno-F0]));
    }
}

/*  Given a pointer to a floating point register set in /proc format
    (fpregset_t *), update the register specified by REGNO from gdb's idea
    of the current floating point register set.  If REGNO is -1, update
    them all. */

void
fill_fpregset (fpregsetp, regno)
fpregset_t *fpregsetp;
int regno;
{
  int regi;
  char *to;
  char *from;
  extern char registers[];
  BTDEBUG("fill_fregset  regno %d\n",regno);

  for (regi = F0 ; regi <= F31 ; regi++)
    {
      if ((regno == -1) || (regno == regi))
	{
	  from = (char *) &registers[REGISTER_BYTE (regi)];
       to = (char *) &(fpregsetp -> fpu.r_freg[regi-F0]);
	  bcopy (from, to, REGISTER_RAW_SIZE (regno));
	}
    }
}


/*  Given a pointer to a general register set in /proc format (gregset_t *),
    unpack the register contents and supply them as gdb's idea of the current
    register values. */

void
supply_gregset (gregsetp)
gregset_t *gregsetp;
{
  register int regno;
  register greg_t *regp = (greg_t *) gregsetp;

  BTDEBUG("supply_gregset called \n");

  for (regno = 0 ; regno <= R31 ; regno++)
    {
      supply_register (regno, (char *) (regp + regno));
    }
}

/*  Given a pointer to a general register set in /proc format (gregset_t *),
    update the register specified by REGNO from gdb's idea
    of the current general register set.  If REGNO is -1, update
    them all. */

void
fill_gregset (gregsetp, regno)
gregset_t *gregsetp;
int regno;
{
  int regi;
  extern char registers[];
  register greg_t *regp = (greg_t *) gregsetp;
  BTDEBUG("fill_gregset regno %d \n",regno);

  for (regi = 0 ; regi <= R31 ; regi++)
    {
      if ((regno == -1) || (regno == regi))
	{
        *(regp + regi) = *(int *) &registers[REGISTER_BYTE (regi)];
	}

   }
}
#endif


/* Push an empty stack frame, to record the current PC, etc.  */
/* We have this frame with fp pointing to a block where all GDB-visible
   registers are stored in the order GDB knows them, and sp at the next
   alignment point below fp.  Note: fp + NUM_REGS*4 was the old sp
   */
extern CORE_ADDR text_end;
CORE_ADDR dummy_start_addr;
void i860_push_frame()
{    
   register CORE_ADDR old_fp = read_register(FP_REGNUM);
   register CORE_ADDR old_sp = read_register(SP_REGNUM);
   register CORE_ADDR fp ;
   extern char registers[];	
					 
   fp = old_sp - REGISTER_BYTES;							 
   write_memory(fp, registers, REGISTER_BYTES);  /* write out old register values */
   /* reset FP and SP */
   write_register(FP_REGNUM, fp);
   write_register(SP_REGNUM, (fp &~ 15));	/* re-align */		 
   call_dummy_set = 1;
}
/* Discard from the stack the innermost frame, 
   restoring all saved registers.  */

void i860_pop_frame()
{ register FRAME frame = get_current_frame ();			 	       
  register CORE_ADDR fp;					 	       
  struct frame_info *fi;						       
  int i;
  
  fi = get_frame_info (frame);                                                 
  fp = fi->frame;                                                              

  if (call_dummy_set && fi -> pc >= call_dummy_start && 
       fi -> pc <= call_dummy_start + CALL_DUMMY_LENGTH)
     {
  
        read_memory(fp, registers, REGISTER_BYTES);

        target_store_registers(-1);

        {
           /* since we stomped on code that will be executed when we exit the program,
              restore it. */
           extern REGISTER_TYPE call_save_code[4];
     
           write_memory     (call_dummy_start, (char *) call_save_code, 16);
     
        }
        call_dummy_set = 0;
     }
  else
     {
        register int regnum;                                                         
        struct frame_saved_regs fsr;                                                 
        char raw_buffer[12];                                                         

        get_frame_saved_regs (fi, &fsr);                                             
        for (regnum = FP0_REGNUM + 31; regnum >= FP0_REGNUM; regnum--)               
           if (fsr.regs[regnum])                                                      
              write_register (regnum, read_memory_integer (fsr.regs[regnum], 4));      
        for (regnum = R31; regnum >= 1; regnum--)                                     
           if (fsr.regs[regnum])                                                      
              if (regnum != SP_REGNUM)                                                 
                 write_register (regnum, read_memory_integer (fsr.regs[regnum], 4));    
              else                                                                     
                 write_register (SP_REGNUM, fsr.regs[SP_REGNUM]);                       
        if (fsr.regs[PS_REGNUM])                                                     
           write_register (PS_REGNUM, read_memory_integer (fsr.regs[PS_REGNUM], 4));  
        if (fsr.regs[FPS_REGNUM])                                                    
           write_register (FPS_REGNUM, read_memory_integer (fsr.regs[FPS_REGNUM],4)); 
        if (fsr.regs[PC_REGNUM])                                                     
           write_register (PC_REGNUM,CLEAN_PC( read_memory_integer (fsr.regs[PC_REGNUM], 4)));  
          }    
  
  flush_cached_frames ();					 	       
  
  set_current_frame (create_new_frame (read_register (FP_REGNUM),              
                                        read_pc ()));

}

CORE_ADDR  i860_arg_coerce(nargs, args, struct_return, sp)
     int nargs;
     value *args;
     int struct_return;
     CORE_ADDR sp;
{

  register int scalar;
  register enum type_code code2;
  register struct type *type;
  int i;
  value arg;
  int num_int_args = 0;
  int num_float_args = 0;
  int size = 0;
  CORE_ADDR arg_ptr;
  
  /* if we return a structure, it's address is in R16, and thus it uses up one of the integer
     argument registers. See the ABI. */
  if (struct_return)     
     num_int_args += 1;

  /* loop to do the right thing with all the arguments and calculate the size of the memory
     argument list. We keep count of the number of integer and the number of float parameters,
     as well as the size of the memory argument list. */

  for (i = 0; i < nargs; i++)
     {    

        /* NOTE that this is all hunky dory in spite of the fact that we don't actually
           have the signature of the called procedure EXCEPT if we are passing in floats!
           This is true, since registers are 4 bytes, and the minimum alignment in the
           memory argument list is 4 bytes. See the ABI for more gory details. The switch
           "ansi-conformant" is an attempt to get around this problem. */
           
        code2 = TYPE_CODE (VALUE_TYPE(args[i]));

        /* Only coerce if we've got switch "ansi-conformant" off.
           Actually, it's OK ( and probably helpful) to coerce ALL integer arguments
           (see comment above), but never mind, we make them the right size in
           pass_function_arguments. */

        if ((!ansi_conformant) && (code2 != TYPE_CODE_STRUCT))
           value_arg_coerce(args[i]);

        arg = args[i];
        type = VALUE_TYPE(args[i]);

        /* All structures are passed by value in the memory argument list. */
        if (code2 == TYPE_CODE_STRUCT)
           {
              size = ALIGN_ARG(size, sizeof(long));
              size += TYPE_LENGTH(type);   
           }
        else if (type == builtin_type_float)
           {
              num_float_args += 1;
              if (num_float_args > NUM_FLOAT_ARG_REGS)
                 {
                    size = ALIGN_ARG(size, TYPE_LENGTH(type)) ;
                    size += TYPE_LENGTH(type);
                 }
           }
        else if (type == builtin_type_double)
           {
              /* floating register alignment  -- see ABI */
              if ((num_float_args%2) != 0)
                 num_float_args += 1;

              num_float_args += 2; /* use up two registers */

              if (num_float_args > NUM_FLOAT_ARG_REGS)
                 {
                    size = ALIGN_ARG(size, TYPE_LENGTH(type)) ;
                    size += TYPE_LENGTH(type);
                 }
           }
        else
           {
              int len = max (sizeof(long), TYPE_LENGTH(type));
              
              num_int_args += 1;

              if (num_int_args > NUM_INT_ARG_REGS)
                 {
                    /* see ABI -- in-memory arguments have AT LEAST word alignment */
                    if  ((TYPE_CODE(type) == TYPE_CODE_INT) ||
                         (TYPE_CODE(type) == TYPE_CODE_ENUM) ||
                         (TYPE_CODE(type) == TYPE_CODE_CHAR) ||
                         (TYPE_CODE(type) == TYPE_CODE_BOOL))
                       size  = ALIGN_ARG(size, len);
                    else
                       size = ALIGN_ARG(size, sizeof(long));
                    size += len;
                 }
           }
                         
     }
  

  /* recalculate the stack pointer, leaving enough space for the memory argument list and
     realigning the stack pointer. */
  if (size != 0)
     {
        arg_ptr = sp -  size;

        arg_ptr = arg_ptr & (-16); /* realign stack */
        write_register (R28,arg_ptr);
        sp = arg_ptr;
     }

return (sp);

}
void i860_extract_return_value(type,regbuf,valbuf)
struct type *type;
char regbuf[REGISTER_BYTES];
char *valbuf;
{
  register int len = TYPE_LENGTH (type);
  double tmp_db;
  float tmp_flt;

   if ((TYPE_CODE(type) == TYPE_CODE_FLT))
      {
         if (len == sizeof (float))
            {
               /* FIXME 
                  NOTE that this assumes that the function declaration was ANSI_CONFORMANT --
                  at the present time I can't think of ANY WAY to disambiguate the two following
                  cases:
                  float really_does_return_a_float(float ff) 
                  { ...}             
                  and
                  float actually_returns_a_double(ff)
                  float ff;
                  {...}
                  */
               bcopy ((char *) (regbuf) + REGISTER_BYTE(ADJ_FREG(F8)), (valbuf), TYPE_LENGTH (type)) ; 
            }
         else
            bcopy ((char *) (regbuf) + REGISTER_BYTE(F8), (valbuf), TYPE_LENGTH (type)) ;
      }
   else 
      bcopy ((char *) (regbuf) + REGISTER_BYTE(R16), (valbuf), TYPE_LENGTH (type));

}
void i860_store_return_value(type,valbuf)
struct type *type;
char *valbuf;
{
  register int len = TYPE_LENGTH (type);
  double tmp_db;

   if ((TYPE_CODE(type) == TYPE_CODE_FLT) )
      {
         write_register_bytes (REGISTER_BYTE (F8), valbuf, len);
      }
   else 
      write_register_bytes (REGISTER_BYTE (R16), valbuf, TYPE_LENGTH (type));
 
}

