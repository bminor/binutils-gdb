/* Machine-dependent code which would otherwise be in inflow.c and core.c,
   for GDB, the GNU debugger.
   Copyright (C) 1992 Free Software Foundation, Inc.
   This code is for the i860 cpu.
   Contributed by Peggy Fieland (pfieland@stratus.com)
   
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

#include "tm-i860.h"
#include "i860-opcode.h"

#include "break.h"
#include "command.h"

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
extern int store_inferior_registers(int);
extern int outside_startup_file();

int btdebug = 0;    /* change value to 1 to enable debugging code */

#define  BTDEBUG  if (btdebug) btdebug_message

extern int errno;
extern int attach_flag;

#define INSTRUCTION_LENGTH 4
#define REGISTER_LENGTH 4

/* routine to print debugging messages */
void btdebug_message(char *format, ...)
{	
   va_list	arglist;
   va_start( arglist, format );
   
   if( btdebug )
      vfprintf (stderr,  format, arglist );
   va_end  ( arglist 	  );
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
static int find_r1_offset(pc)
CORE_ADDR pc;
{
   int r1_off,i;
   CORE_ADDR	instr;
   
   /* look for the instruction and examine the offset */
   
   for (i=INSTRUCTION_LENGTH*1; i< INSTRUCTION_LENGTH*4; i+=INSTRUCTION_LENGTH){
      instr = (unsigned)( adj_read_memory_integer(pc+i));
      if ((instr & 0xFFE0F801) == 0x1C400801) { /* st.l r1,X(sp) */
         
         r1_off = SIGN_EXT16(((instr&0x001F0000) >> 5) | 
                             (instr&0x000007FE));
         return(r1_off);
      }
   }
   return(-1);
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
static int caller_pc(this_pc,this_sp,to_pc,to_fp)
     CORE_ADDR this_pc,this_sp;
     CORE_ADDR *to_pc, *to_fp;
{
   CORE_ADDR func_start;
   int sp_offset,offset;
   CORE_ADDR sp,pc,fp,instr;
   
   BTDEBUG("caller_pc %x sp = %x\n",this_pc,this_sp);
   
   func_start = get_pc_function_start(this_pc);

   BTDEBUG("caller_pc func_start %x\n", func_start);

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
               /* if we get here, procedure doesn't have a frame.  If we didn't
                  do anything weird, the frame pointer and return register have
                  the values we want.  Check them to see if they are valid. */
                  
               CORE_ADDR temp_rp, temp_fp;

               temp_rp = read_register(RP_REGNUM);
               temp_fp = read_register(FP_REGNUM);

               if (!valid_regs(temp_rp, temp_fp))
                  {           
                     printf("error frame_chain\n");
                     return(0);
                  }
               BTDEBUG("caller_pc no frame, using r1 %x and fp %x\n",
                       temp_rp, temp_fp);
               *to_pc = temp_rp;
               *to_fp = temp_fp;
               return (1);
            }
      
      BTDEBUG("sp_offset = %d %x\n",sp_offset,sp_offset);
      
      offset = find_r1_offset(func_start);
      
      if( offset < 0 ){
         printf("cant find return address for routine at %x\n",
                func_start);
         return(0);
      }
      pc = read_memory_integer(this_sp+offset,sizeof(long));
      sp= this_sp - sp_offset;
      
      BTDEBUG("callers pc = %x sp = %x\n",pc,sp);
      
      /* our caller a -g routine  ?
       * if he is we have to find his real fp
       * else provide the sp as his fp
       */
      
      if( g_routine(pc) ){
         
         BTDEBUG("caller_a_g\n");
         
         if( ! (offset = find_fp_offset(func_start)) ) {
            printf("error fp_offset\n");
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
            printf("error frame_chain\n");
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
   extern char registers[];
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
                  printf(" Breakpoint adjusted to avoid br/call delay slot and multiple breakpoints\n");
               
               if ((instr & 0xF4000000) == 0x74000000) /* bc.t or bnc.t */
                  printf(" Breakpoint adjusted to avoid bc.t/bnc.t delay slot and multiple breakpoints\n");
               /* it IS really OK to set a break on the instruction AFTER the conditional branch
                  -- it DOESN't have a delay slot */
               if ((instr & 0xF4000000) == 0x70000000) /* bc / bnc */
                  /* printf(" Breakpoint adjusted to avoid bc/bnc delay slot and multiple breakpoints\n"); */
                  adjust = 0; 
            } else if
               ((instr & 0xFC00003F) == 0x4C000002 ||	/* bri/  calli */
                (instr & 0xFC000000) == 0x40000000)	
                  {
                     adjust++;
                     printf(" Breakpoint adjusted to avoid calli/bri delay slot and multiple breakpoints\n");
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
                                 printf(" Breakpoint adjusted to avoid bla delay slot and multiple breakpoints\n");
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
   
   pc = read_register (PC_REGNUM);
   
   if (!one_stepped)
      {
         brk.address = pc;
         place_brk (pc, SINGLE_STEP_MODE, &brk);
         brk.shadow_contents[0] = brk.shadow_contents[1] = 0;
         brk.shadow_contents[2] = brk.shadow_contents[3] = 0;
         
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
                     adj_read_memory  (brk.act_addr[2], &brk.shadow_contents[2],
                                       INSTRUCTION_LENGTH);
                     adj_write_memory (brk.act_addr[2], break_insn, INSTRUCTION_LENGTH);
                     adj_read_memory  (brk.act_addr[3], &brk.shadow_contents[3],
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
               
               adj_read_memory  (brk.act_addr[0], &brk.shadow_contents[0],	
                                 INSTRUCTION_LENGTH);
               adj_write_memory (brk.act_addr[0], break_insn,
                                 INSTRUCTION_LENGTH);
               adj_read_memory  (brk.act_addr[1], &brk.shadow_contents[1],	
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
                  adj_read_memory  (brk.act_addr[2], &brk.shadow_contents[2],
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
            adj_read_memory  (brk.act_addr[0], &brk.shadow_contents[0],
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
               adj_write_memory (brk.act_addr[0], &brk.shadow_contents[0],
                                 INSTRUCTION_LENGTH);
               adj_write_memory (brk.act_addr[1], &brk.shadow_contents[1],
                                 INSTRUCTION_LENGTH);
            } else	{
               adj_write_memory (brk.act_addr[0], &brk.shadow_contents[0],
                                 INSTRUCTION_LENGTH);
            }
         
         if (brk.address1 != NULL)
            {
               if (brk.mode == DIM)
                  {
                     adj_write_memory (brk.act_addr[2], &brk.shadow_contents[2], 
                                 INSTRUCTION_LENGTH);
                     adj_write_memory (brk.act_addr[3], &brk.shadow_contents[3],
                                 INSTRUCTION_LENGTH);
                  } else	{
                     adj_write_memory (brk.act_addr[2], &brk.shadow_contents[2], 
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
            if (i != SP_REGNUM && i != FP0_REGNUM && i != FP0_REGNUM + 1)
               /* the register numbers used in the instruction and the ones used to index
                  the regs array are not the same -- compensate */
               frame_saved_regs->regs[i+R0] = frame_info->frame + i*REGISTER_LENGTH;
         
         frame_saved_regs->regs[SP_REGNUM] = frame_info->frame + NUM_REGS*REGISTER_LENGTH;
         
         call_dummy_set = 0;
         return;
      }
   
   pc = get_pc_function_start (frame_info->pc); 
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
   
   /* get the frame pointer actually sp for  a non -g
    * for the routine that called us routine 
    */
   
   BTDEBUG("FRAME_CHAIN(%x)\n",thisframe);
   
   if ( !read_memory_integer (thisframe->frame,sizeof(long)) )
      {
         return (0);
      }
   
   if( ! g_routine(thisframe->pc) ){
      BTDEBUG( "non g at %x\n",thisframe->pc);
      caller_pc(thisframe->pc,thisframe->sp,&pc,&fp);
      BTDEBUG("caller_pc returned %x %x \n",pc,fp);
      return(fp);
      
   }/* else a -g routine */
   
   
   fp = read_memory_integer (thisfp, sizeof(long));
   
   if (fp < thisfp || fp > (unsigned) STACK_END_ADDR)
      {
         /* handle the Metaware-type pseudo-frame */
         
         func_start = get_pc_function_start(thisframe->pc);
         
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
     
     frame = frame_struct->frame;
     pc = frame_struct->pc;
     sp = frame_struct->sp;

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
           caller_pc(pc,sp,&pc,&frame);
        }
     else 
        {
           
           pc = read_memory_integer (frame + 4, sizeof(long)); 
   
           if (!outside_startup_file(pc))
              {
                 
                 BTDEBUG("pc %x outside startup file \n",pc);

                 pc1 = read_memory_integer (frame, sizeof(long));
               
                 if (outside_startup_file(pc1))
                    pc = pc1;
                 else
                    pc = 0;
              }
        } 
     BTDEBUG(" returning pc %x\n", CLEAN_PC(pc));
     return(CLEAN_PC(pc));

  }

/* Pass arguments to a function in the inferior process - ABI compliant
   Note that this routine DOES NOT HANDLE memory argument lists, ie
   it gives up if there are too many arguments to pass in registers.*/

void
pass_function_arguments(args, nargs, struct_return)
     value *args;
     int nargs;
     int struct_return;
{
   int ireg = (struct_return) ? 17 : 16;
   int freg = FP0_REGNUM + 8;
   int i;
   struct type *type;
   value arg;
   long tmp;
   value value_arg_coerce();
   
   
   for (i = 0; i < nargs; i++) 
      {
         arg = value_arg_coerce(args[i]);
         type = VALUE_TYPE(arg);
         if (type == builtin_type_double) 
            {
               write_register_bytes(REGISTER_BYTE(freg), VALUE_CONTENTS(arg), sizeof(double));
               freg += 2;
            }
         else
            {
               bcopy(VALUE_CONTENTS(arg), &tmp, sizeof(long));
               write_register(ireg, tmp);
               ireg++;
            }
      }
   if (ireg >= 28 || freg >= FP0_REGNUM + 16)
      error("Too many arguments to function");
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

#if 0
/* This routine is uncalled -- remove this routine sometime */
get_reg(regno)
{
   char raw_buffer[32];
   int addr;
   int virtual_buffer;
   
   read_relative_register_raw_bytes (regno, raw_buffer);
   REGISTER_CONVERT_TO_VIRTUAL (addr, raw_buffer, &virtual_buffer);
   return(virtual_buffer);
}
#endif


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

extern char registers[];

/* i860-specific routine to print the register set. Note that we ALWAYS print information
   on the floating point registers, so we ignore the parameter fpregs */
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
         if ((regnum >=F0 ) && (regnum <= F31))
            bcopy (&registers[ADJ_FREG(regnum)<<2], &val, sizeof (long));
         else
            bcopy (&registers[regnum<<2], &val, sizeof (long));
         printf("%-4s 0x%08x\t", reg_names[regnum], val);
         printf("\n\t"); fflush(stdout);
      }
   else	/* print all registers */
      {
         
         printf("\n Control/Status Registers :- \n\t");
         for (j=0; j<=DB; j++)
            {
               bcopy (&registers[j<<2], &val, sizeof (long));
               printf("%-4s 0x%08x\t", reg_names[j], val);
            }
         printf("\n\t"); fflush(stdout);
         
         /* EPSR */
         bcopy (&registers[EPSR<<2], &val, sizeof (long));
         printf("%-4s 0x%08x\t", reg_names[EPSR], val);
         
         /* FSR */
         bcopy (&registers[FSR<<2], &val, sizeof (long));
         printf("%-4s 0x%08x\t", reg_names[FSR], val);
         
         /* CCR */
         bcopy (&registers[CCR<<2], &val, sizeof (long));
         printf("%-4s 0x%08x\t", reg_names[CCR], val);
         /* BEAR*/
         bcopy (&registers[BEAR<<2], &val, sizeof (long));
         printf("%-4s 0x%08x\t", reg_names[BEAR], val);
         
         
#ifdef JIM_ADD_PRIV
         for (j=P0; j<=P3; j++)
            {
               bcopy (&registers[j<<2], &val, sizeof (long));
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
               bcopy (&registers[j<<2], &val, sizeof (long));
               printf("%-4s 0x%08x\t", reg_names[j], val);
            }
         
         printf("\n Floating Registers :- \n\t");
         for (j=F0; j<=F31; j++)
            {
               if (j != FREGS && (j % REGISTER_LENGTH == 0)) 
                  {
                     printf("\n\t");  fflush(stdout);
                  }
               bcopy (&registers[ADJ_FREG(j)<<2], &val, sizeof (long));
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
               bcopy (&registers[j<<2], &val, sizeof (long));
               bcopy (&registers[(j+1)<<2], &valh, sizeof (long));
               printf("%-6s 0x%08x %08x\t", reg_names[j], val,valh);
            }
         
         printf("\n Graphics Pipeline :- \n");
         {
            unsigned int valh;
            j = PSV_I1;
            bcopy (&registers[j<<2], &val, sizeof (long));
            bcopy (&registers[(j+1)<<2], &valh, sizeof (long));
            printf("\t\t\t%-8s 0x%08x %08x \n", reg_names[j], val,valh);
         }
         
         printf(" Memory Load Pipeline :- \n");
         for (j=PSV_L1; j<=PSV_L3; j+=REGISTER_LENGTH)
            {
               unsigned int valh, val2,val3;
               bcopy (&registers[j<<2], &val, sizeof (long));
               bcopy (&registers[(j+1)<<2], &valh, sizeof (long));
               bcopy (&registers[(j+2)<<2], &val2, sizeof (long));
               bcopy (&registers[(j+3)<<2], &val3, sizeof (long));
               printf("\t\t%-8s 0x%08x %08x %08x %08x\n", reg_names[j], 
                      val,valh,val2,val3);
            }
         
         printf("\n Adder Pipeline :-\t\tMultiplier Pipeline :-\t\tFSR results :-\n");
         for (i=PSV_FSR1,j=PSV_A1,k=PSV_M1; j<=PSV_A3; i++,j+=2,k+=2)
            {
               unsigned int valh,val2,val3,val4;
               bcopy (&registers[i<<2], &val4, sizeof (long));
               bcopy (&registers[j<<2], &val, sizeof (long));
               bcopy (&registers[(j+1)<<2], &valh, sizeof (long));
               bcopy (&registers[k<<2], &val2, sizeof (long));
               bcopy (&registers[(k+1)<<2], &val3, sizeof (long));
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
d_ro_break_command(arg)
char *arg;
{
	dbrkval = strtoul(arg, NULL, 0);
	dbrkmod = 0x01;
     BTDEBUG(" ro_dbreak - %x %x\n", dbrkval, dbrkmod);
}

/* set a "write" data breakpoint. */
void
d_wo_break_command(arg)
char *arg;
{
	dbrkval = strtoul(arg, NULL, 0);
	dbrkmod = 0x02;
     BTDEBUG(" wo_dbreak - %x %x\n", dbrkval, dbrkmod);
}

/* set a "read/write" data breakpoint. */
void
d_rw_break_command(arg)
char *arg;
{
	dbrkval = strtoul(arg, NULL, 0);
	dbrkmod = 0x03;
     BTDEBUG(" rw_dbreak - %x %x\n", dbrkval, dbrkmod);
}

/* clear data breakpoint. */
void
clear_dbreak()
{
	dbrkval = 0;
	dbrkmod = 0;
}

/* i860-specific breakpoint initialization.  Includes adding the i860-specific
   data breakpoint commands. */
void 
i860_init_breakpoints()
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

   place_brk( b->address, BREAK_MODE, b );
   if (b->mode == DIM)
      {
         
         adj_read_memory (b->act_addr[0], &b->shadow_contents[0],  INSTRUCTION_LENGTH);
         val =  adj_write_memory (b->act_addr[0], break_insn,  INSTRUCTION_LENGTH);
         if (val != 0 ) return val;
         adj_read_memory (b->act_addr[1], &b->shadow_contents[1],  INSTRUCTION_LENGTH);
         /*	val = adj_write_memory (b->act_addr[1], float_insn,  INSTRUCTION_LENGTH); */
         if (val != 0) return val;
      } 
   else	
      {
         adj_read_memory (b->act_addr[0], &b->shadow_contents[0],  INSTRUCTION_LENGTH);
         val = adj_write_memory (b->act_addr[0], break_insn,  INSTRUCTION_LENGTH);
      }
   if (b->address1 != 0)
      {
         if (b->mode == DIM)
            {

               adj_read_memory (b->act_addr[2], &b->shadow_contents[2],  INSTRUCTION_LENGTH);
               val = adj_write_memory (b->act_addr[2], break_insn,  INSTRUCTION_LENGTH);
               if (val) return val;
               adj_read_memory (b->act_addr[3], &b->shadow_contents[3],  INSTRUCTION_LENGTH);
               /*	val = adj_write_memory (b->act_addr[3], float_insn,  INSTRUCTION_LENGTH); */
               if (val != 0) return val;
            }
         else	
            {
               adj_read_memory (b->act_addr[2], &b->shadow_contents[0],  INSTRUCTION_LENGTH);
               val = adj_write_memory (b->act_addr[2], break_insn,  INSTRUCTION_LENGTH);
            }
      }
   if (val != 0)
      return val;
   BTDEBUG("Inserted breakpoint at 0x%x, shadow 0x%x, 0x%x.\n",
           b->address, b->shadow_contents[0], b->shadow_contents[1]);
   b->inserted = 1;
   return 0;
}

int i860_remove_breakpoint(b)
struct breakpoint *b;
{
   int val;

   if (b->inserted)
      {
         if (b->mode == DIM)
            {
               val =adj_write_memory (b->act_addr[0], &(b->shadow_contents[0]),  
                                      INSTRUCTION_LENGTH);
               val =adj_write_memory (b->act_addr[1], &(b->shadow_contents[1]), 
                                       INSTRUCTION_LENGTH);
               if (b->address1 != NULL)
                  {
                     val =adj_write_memory (b->act_addr[2], &(b->shadow_contents[2]), 
                                             INSTRUCTION_LENGTH);
                     val =adj_write_memory (b->act_addr[3], &(b->shadow_contents[3]), 
                                             INSTRUCTION_LENGTH);
                  }
            } 
         else	
            {
               val =adj_write_memory (b->act_addr[0], b->shadow_contents, 
                                       INSTRUCTION_LENGTH);
               if (b->address1 != NULL)
                  {
                     val =adj_write_memory (b->act_addr[2], b->shadow_contents,  
                                            INSTRUCTION_LENGTH);
                  }
            }
         if (val != 0)
            return val;
         b->inserted = 0;
         BTDEBUG( "Removed breakpoint at 0x%x, shadow 0x%x, 0x%x.\n",
                 b->address, b->shadow_contents[0], b->shadow_contents[1]);
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
