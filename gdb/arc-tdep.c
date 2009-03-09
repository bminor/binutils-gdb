/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Soam Vasani <soam.vasani@codito.com>
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
#include <string.h>

#include "defs.h"
#include "arch-utils.h"
#include "dis-asm.h"
#include "gdbtypes.h"
#include "frame.h"
#include "frame-unwind.h"
#include "target.h"
#include "breakpoint.h"
#include "inferior.h"
#include "regcache.h"
#include "reggroups.h"
#include "trad-frame.h"
#include "dwarf2-frame.h"
#include "gdbtypes.h"
#include "gdb_assert.h"
#include "gdbcore.h"
#include "observer.h"
#include "osabi.h"

#include "opcode/arc.h"

#include "arc-tdep.h"


//#define ARC_DEBUG 1

#if ARC_DEBUG
#  define ENTERMSG printf ("--- entered %s:%s()\n", __FILE__, __FUNCTION__)
#  define ENTERARGS(fmt, args...) printf ("--- entered %s:%s(" fmt ")\n", __FILE__, __FUNCTION__, args)
#  define LEAVEMSG printf ("--- exited %s:%s()\n", __FILE__, __FUNCTION__)
#else
#  define ENTERMSG
#  define ENTERARGS(fmt, args...) 
#  define LEAVEMSG
#endif

#define ARC_STATUS32_A1 0x8
#define ARC_STATUS32_A2 0x10
#define ARC_STATUS32_AE 0x20
#define ARC_STATUS32_L  0x100


static CORE_ADDR arc_frame_align (struct gdbarch *gdbarch, CORE_ADDR sp);
/* The frame unwind cache for the ARC 
 */

struct arc_unwind_cache
{
  /* blink offset in stack */
  int blink_offset ;

  /* Caller's PC */
  CORE_ADDR prev_pc;

  
  /* The stack pointer at the time this frame was created; i.e. the
     caller's stack pointer when this function was called.  It is used
     to identify this frame.  */
  
  CORE_ADDR prev_sp;
  /* The frame base */
  CORE_ADDR frame_base;
  /* Frame size */
  int framesize;
  
  /* offset of sp from the stack frame base */
  LONGEST sp_offset;
  /* offset of fp from the stack frame base */
  LONGEST fp_offset;
  /* Is this a leaf function */
  int is_leaf ;
  /* Is there a frame pointer */
  int uses_fp;
  
  
  /* Offsets for each register in the stack frame */
  struct trad_frame_saved_reg *saved_regs;
};




/* Function Prototypes */

static CORE_ADDR arc_unwind_sp (struct gdbarch *gdbarch, 
			       struct frame_info *next_frame);

static CORE_ADDR arc_unwind_pc (struct gdbarch *gdbarch, 
			       struct frame_info *next_frame);

static struct arc_unwind_cache * arc_frame_unwind_cache (struct frame_info *next_frame, 
							 void ** this_prologue);

static CORE_ADDR arc_scan_prologue (CORE_ADDR pc, 
				     struct frame_info *next_frame,
				     struct arc_unwind_cache *info);


static int arc_binutils_reg_to_regnum (int reg);

extern struct arcDisState arcAnalyzeInstr (  bfd_vma address,disassemble_info* info );
extern struct arcDisState a4AnalyzeInstr (  bfd_vma address,disassemble_info* info );

/* defined in opcodes, but there's no header file with this prototype... */
disassembler_ftype arcompact_get_disassembler (void *);

/*  Standard register type for the ARC platform . 
 *  It would be builtin_type_uint32 until 
 *  we consider the DSP extensions 
 */

static struct type *
arc_register_type (struct gdbarch *gdbarch, int regnum)
{
  return builtin_type_uint32;
}


void printFrameInfo(struct arc_unwind_cache * info)
{
#ifdef ARC_DEBUG
  printf("-------------------\n");
  printf("%lx \n",info );
  printf("prev_sp = %lx \n",info->prev_sp);
  printf("prev_pc = %lx \n",info->prev_pc);
  printf("frame_base is %lx \n",info->frame_base);
  printf("framesize is %lx \n",info->framesize);
  printf("Blink offset %lx \n",info->blink_offset);
  printf("sp_offset = %lx \n",info->sp_offset );
  printf("fp_offset is %lx \n",info->fp_offset);
  printf("is_leaf = %d, uses_fp=%d",info->is_leaf, info->uses_fp);
#endif
}

/* Print the instruction state returned 
   by the disassembler . Used for internal
   debugging only 
*/

void printInsnState(struct arcDisState state)
{
#ifdef ARC_DEBUG
  printf("---------------------------------\n");
  printf("Instruction Length %d\n", state.instructionLen);
  printf("Opcode [0x%x] : Cond [%x]\n", state._opcode, state._cond);
  printf("Words 1 [%lx] : 2 [%lx]\n", state.words[0], state.words[1]);
  printf("ea present [%x] : memload [%x]\n", state._ea_present, state._mem_load);
  printf("Load length [%d]:\n", state._load_len);
  printf("Address writeback [%d]\n", state._addrWriteBack);
  printf("ea reg1 is [%x] offset [%x] \n", state.ea_reg1, state._offset);
  printf("ea reg2 is [%x] \n", state.ea_reg2);
  printf("operands buffer is %s \n", state.operandBuffer);
  printf("SourceType is %d \n",state.sourceType);
  printf("Flow is %d\n",state.flow);
  printf("Branch is %d,'%c'\n",state.isBranch, state.isBranch);
#endif
}


/* Scan the prologue and update the 
 * corresponding frame cache for the frame unwinder for unwinding
 * frames without debug info . In such a situation GDB attempts to 
 * parse the prologue for this purpose . This currently would attempt
 * to parse the prologue generated by our gcc 2.95 .(We should support 
 * Metaware generated binaries at some suitable point of time )
 * This function is called with the pc where gdb stopped , the next_frame 
 * to be filled in (if need be?) and the existing cached info . 

 * scan_prologue is called by our unwinder as well 
 * as from skip_prologue in the case that it cannot detect 
 * the end of the prologue. next_frame is set to NULL if we are called from 
 * arc_skip_prologue in an attempt to discover the end of the prologue. In
 * such a case we don't fill the frame info that is passed to us :-)

 * Todos. 
 * 1. Support 32 bit normal frames generated by GCC 2.95 .
 * 2. Support 16 and 32 bit mixed frames generated by GCC 2.95
 * 3. Support 32 bit normal variadic function frames by GCC 2.95
 * 4. Support 32 bit normal frames from GCC 3.4.x with variadic args
 * 5. Support 16 and 32 bit normal frames from GCC 3.4.x with variadic args
 * 6. Support 16 and 32 bit mixed frames generated by GCC 3.4.x 
 * 7. Support Metaware generated prologues .( The difference is 
 * in the use of thunks to identify the saving and restoring of 
 * callee saves :-) May have to do some hackery even in next_pc. 
 * since the call is going to create its own set of problems 
 * with our stack setup :-( 
 * We attempt to use the disassembler interface from the opcodes
 * library to do our disassembling .

 * The usual 32 bit normal 
 * gcc -O0 prologue looks like this. 

 * Complete Prologue for all GCC frames (Cases #1 to #6 in Todos above)

 * sub  sp, sp, limm    ; space for variadic arguments.
 * st.a blink, [sp,-4]  ; push blink (if not a leaf function) 
 * sub  sp, sp , limm   ; (optional space creation for callee saves )
 * st   r13, [sp]    ; pushes of all callee saves.
 * st   r14, [sp,4]  ; pushes of more callee saves.
 * XXXX
 * st.a fp , [sp,-4]  ; push fp (if fp has to be saved ) 
 * mov  fp , sp ; Set the current frame up correctly 
 * sub  sp , sp , #immediate ; Create space for local vars on the stack.
 */



/* Macros to be used with disassembling the prologue 
 * and update the frame info.The *FI macros are to update
 * the frame info and the ACT macros are to actually do the 
 * action on a corresponding match. 
 *
*/
#define CHECKOPDSTRING(targetcheck,searchstring)  \
                               if(strstr(targetcheck,searchstring) == targetcheck)  \
                                     {continue;}

#define CHECKOPDSTRINGANDACT(targetcheck,searchstring,action)  \
                               if(strstr(targetcheck,searchstring) == targetcheck)  \
                                     {\
                                       action;\
				       continue;}


/* The frame info changes by changing the decrementing
   the sp_offset and setting the leaf function to be NIL; 
   Also the offset of the blink register from the previous
   value of sp is calculated. Finally this can be updated 
   as 
   info->blink_offset = info-> prev_sp + info->blink_offset ;
   Also the addition below is coz the offsets are usually negative 
*/

#define PUSHBLINKACT do { \
                          if(info) \
                            {      \
                              info->sp_offset += current_instr._offset; \
                              info->blink_offset = info->sp_offset ; \
                              info->is_leaf = 0;\
                            }}while(0);
                      

#define ISPUSHBLINK(state) CHECKOPDSTRING(state.operandBuffer,"blink") 
#define ISPUSHBLINKFI(state) CHECKOPDSTRINGANDACT(state.operandBuffer,"blink",PUSHBLINKACT) 


#define PUSHFPACT    do { \
                          if(info)  \
                            {       \
                               info->sp_offset += current_instr._offset ; \
                               info->fp_offset = -info->sp_offset; \
                            }}while(0);
                             
#define ISPUSHFP(state)    CHECKOPDSTRING(state.operandBuffer,"fp")
#define ISPUSHFPFI(state)    CHECKOPDSTRINGANDACT(state.operandBuffer,"fp",PUSHFPACT)
#define ISINSTRUCTION(insnname,searchstring)    !strcmp(insnname,searchstring)



#define UPDATEFPACT    do {\
                             if(info) {\
					 info->uses_fp = 1;\
			       }}while(0);



#define ISUPDATEFPFI(state)  \
 if(ISINSTRUCTION(state.instrBuffer,"mov")) \
{ \
  CHECKOPDSTRINGANDACT(state.operandBuffer,"fp,sp",UPDATEFPACT); \
}



#define ISUPDATEFP(state)  \
 if(ISINSTRUCTION(state.instrBuffer,"mov")) \
{ \
  CHECKOPDSTRING(state.operandBuffer,"fp,sp") \
}



#define ISSUBSP(state)   \
if(ISINSTRUCTION(state.instrBuffer,"sub"))\
{ \
  CHECKOPDSTRING(state.operandBuffer,"sp,sp") \
}

#define UPDATESTACKSPACE(state) do { \
                                 if(info){ \
/* Eat up sp,sp */  \
                                      int tmp = atoi(state.operandBuffer + 6); \
                                      info->sp_offset -= tmp; \
                                        }}while(0);


#define ISSUBSPFI(state)   \
if(ISINSTRUCTION(state.instrBuffer,"sub") \
   || ISINSTRUCTION(state.instrBuffer,"sub_s"))\
{ \
  CHECKOPDSTRINGANDACT(state.operandBuffer,"sp,sp",UPDATESTACKSPACE(state)) \
}

/*Function to scan the prologue of a A4 binary

ARCtangent-A4 Prolog
 The stack back-trace data structure is a 16-byte structure which is
  used to save the return register (blink, 4 bytes), the frame pointer
  register (fp, 4-bytes) and 8-bytes is reserved.
 
 The compiler-generated prolog code does the following:
  --> Allocates space for register arguments in case of variadic function
        (functions with variable argument lists)
  --> Saves the return address register (blink)
  --> Saves the caller's frame pointer (fp), if required, and
        sets the new frame pointer to this location
  --> Decrements the stack pointer to account for the new stack frame
  --> Saves required non-volatile general-purpose registers into the
        register save area
  --> Stores the arguments above the stack back-trace data structure


  Demo Patterns:
     st         blink,[sp,4] ; Saves the return address
     st         fp,[sp] ; Saves the callers frame pointer
     mov        fp,sp   ; Saves 
     sub        sp,sp,24

0xa  538e7e20     sub        sp,sp,32     ; Space for variadic args
0x2  100e3e04     st         blink,[sp,4] ; Saves the return address
0x2  100e3600     st         fp,[sp]      ; Saves the callers frame pointer
0xc  636e3800     mov        fp,sp        ; Resets stack pointer to fp
0xa  538e7e18     sub        sp,sp,24     ; Decrements sp to add for new
                                          ; stack frame
0x2  100d81fc     st         r0,[fp,-4]   ; Stores the args 
0x2  100d83f8     st         r1,[fp,-8]   ; ----"-------
     ...

*/

/* FIXMEA:
called from arc_skip_prologue as
     skip_pc = arc_scan_prologue(pc,NULL,NULL);
        Then it is supposed to return the first valid pc
	after the prologue

   Prologue analysis does the rest...  
   Currently our scan prologue does not
    support getting input for the frame unwinder

  
  pc = frame_func_unwind(next_frame);
  arc_scan_prologue (pc, next_frame, info);

*/

#ifdef ARC4_JTAG
static CORE_ADDR 
a4_scan_prologue (CORE_ADDR pc, struct frame_info *next_frame,
		    struct arc_unwind_cache *info)
{
  /* End of prologue */
  CORE_ADDR prologue_ends_pc = pc;
  int i = 0;
  struct arcDisState current_instr, instr_in_delay;
  int insn_length;

  /* Initializations to use the opcodes 
   * library .
   */
  
  struct disassemble_info di;

  unsigned int saved_regs_mask = 0;
  /* An arbitrary length on the length of the 
     prologue. If next_frame is NULL this means that there was 
     no debug info and we are called from arc_skip_prologue
  */
  /*FIXMEANOW: pc + 64 is probably the max size of the prologue*/
  CORE_ADDR final_pc = (next_frame)?frame_pc_unwind(next_frame):pc+(16*4);
  
  


  if (info)
    {
      /* All functions start as leaf functions until 
	 we identify push blink 
      */
      info->is_leaf = 1;
    }
  


  /* Initializations to use the opcodes 
   * library .
   */
  init_disassemble_info(&di, gdb_stderr, fprintf_unfiltered);
  di.arch = gdbarch_bfd_arch_info(current_gdbarch)->arch;
  di.mach = gdbarch_bfd_arch_info(current_gdbarch)->mach;
  di.endian = gdbarch_byte_order(current_gdbarch);
  di.read_memory_func = target_read_memory;
  
  
  for (prologue_ends_pc= pc;
       prologue_ends_pc< final_pc; 
       prologue_ends_pc += current_instr.instructionLen ) /*FIXMEA: This could as
							    well be 4 */
    {
      current_instr = a4AnalyzeInstr(prologue_ends_pc, &di);
      printInsnState(current_instr);
#ifdef ARC_DEBUG
      printf("Prologue PC: %d\n", prologue_ends_pc);
      printf("Final PC: %d\n", final_pc);
#endif


      if (current_instr._opcode == 0x2)
	{
	  // Saves the return address    st blink,[sp,4]   0x100e3e04
	  // Save the callers fp         st fp,[sp]        0x100e3600
	  // Saves the args              st rX,[fp, #imm]  0x100d8xxx
	  if (current_instr.ea_reg1 == 28)
	    {
	      if( strstr(current_instr.operandBuffer,"blink") == current_instr.operandBuffer)
		{
                  if(info)
		    {      
		      info->sp_offset += current_instr._offset;
 //		      info->blink_offset = info->sp_offset ;
		      info->blink_offset = current_instr._offset;
		      info->is_leaf = 0;

#ifdef ARC_DEBUG
		      printf("Blink instruction:\n");
		      printFrameInfo(info);
#endif
		    }
		  continue;
		}
	      else
		if(strstr(current_instr.operandBuffer,"fp") == current_instr.operandBuffer)
		{
		  if(info)
		    {
/* 		      info->sp_offset += current_instr._offset ; */
/* 		      info->fp_offset = info->sp_offset; */
		      info->fp_offset = 0;
		    }
		  continue;
		}
	    }
	  else if (current_instr.ea_reg1 == 27)
	    {
	      /* Saving of arguments onto the stack using the
		 frame pointer (r27).  */
	      if(info)
		{
		  // Save regs offsets 
		}
#ifdef ARC_DEBUG	      
	      printf(" Saving registers onto stack\n%s\n",current_instr.operandBuffer);
#endif	      
	      continue;
	    }
	  //	  ISPUSHBLINK(current_instr);
	}
      else if (current_instr._opcode == 0xc)
	{
	  // Resets stack pointer to fp
	  // 0x636e3800
	  //	  636e3800     mov        fp,sp
	  if (current_instr.words[0] == 0x636e3800)
	    {
	      if (info)
		{
		  info->uses_fp = 1;
		}
	      continue;
	    }
	}
      else if (current_instr._opcode == 0xa)
	{
	  // Decrements stackpointer to add for new stack frame
	  // 0x538e7e18   sub        sp,sp,#imm
	  // 538e7e20     sub        sp,sp,32
	  if( current_instr.words[0] == 0x538e7e20)
	    {
	      //sub sp,sp, 32  //// variadic
	      if (info)
		{
		  int tmp = atoi(current_instr.operandBuffer + 6); 
		  info->sp_offset -= tmp; 
		}
	      continue;
	    }
	  else if((current_instr.words[0] & 0xffffff00) == 0x538e7e00)
	    {
	      // sub sp,sp,xx
	      if(info)
		{
		  int tmp = atoi(current_instr.operandBuffer + 6); 
		  info->sp_offset -= tmp; 
		}
	      continue;
	    }
	}

      /* Found a instruction that is not in
         the prologue*/
#ifdef ARC_DEBUG
      printf("End of Prologue reached \n");
#endif
      break;
    }

  /* Means we were called from skip_prologue */
  if((next_frame == NULL)&& (info == NULL))
    {
      return prologue_ends_pc;
    }


  info->framesize = -info->sp_offset;
  /* Compute the previous frame's stack pointer (which is also the
     frame's ID's stack address), and this frame's base pointer.  */
  if(info->uses_fp)
    {

      ULONGEST this_base;
      /* The SP was moved to the FP.  This indicates that a new frame
         was created.  Get THIS frame's FP value by unwinding it from
         the next frame.  */
      frame_unwind_unsigned_register(next_frame, ARC_FP_REGNUM,
				     &this_base);
      info->frame_base = this_base;
      info->saved_regs[ARC_FP_REGNUM].addr = info->frame_base;

      /* The previous sp is the current frame base + the offset of the
	 fp in the current frame */
      info->prev_sp = info->frame_base + info->fp_offset;
      for(i = 13; i < 26 ; i++ )
	{
	  if(saved_regs_mask & (1 << i))
	    info->saved_regs[i].addr += info->frame_base ;
	}
      
      printFrameInfo(info);
   
    }
  else
    {
      ULONGEST this_base;
      /* Assume that the FP is this frame's SP but with that pushed
         stack space added back.  */
      frame_unwind_unsigned_register (next_frame, ARC_SP_REGNUM, &this_base);
      info->frame_base = this_base;

      /* In such a case it would be the previous SP + the size of the current frame */
      info->prev_sp = info->frame_base + info->framesize;
     
    }
  

  if(!info->is_leaf)
    {

      /* Usually blink is saved before the callee save registers and
	 below the space created for variadic arguments  . We maintain
	 info->blink_offset as negative when we stored it initially
       */
      info->saved_regs[ARC_BLINK_REGNUM].addr = info->prev_sp + info->blink_offset;
#ifdef ARC_DEBUG
      printf("blink offset is [%x] \n",info->blink_offset);
#endif
    }
      
    /* The PC is found in blink (the actual register or located on the stack).  */
  // FIXMEA:
  //info->saved_regs[ARC_STATUS_REGNUM] |= (info->saved_regs[ARC_BLINK_REGNUM] & 0xffffff)>>2;
  info->saved_regs[ARC_STATUS_REGNUM] = info->saved_regs[ARC_BLINK_REGNUM];
  return prologue_ends_pc;

}
#endif

static CORE_ADDR 
arc_scan_prologue (CORE_ADDR pc, struct frame_info *next_frame,
		    struct arc_unwind_cache *info)
{
#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf("\narc_scan_prologue called\n");
#endif
#else
  /* End of prologue */
  CORE_ADDR prologue_ends_pc = pc;
  int i = 0;
  struct arcDisState current_instr, instr_in_delay;
  int insn_length;

  /* Initializations to use the opcodes 
   * library .
   */
  
  struct disassemble_info di;

  unsigned int saved_regs_mask = 0;
  /* An arbitrary length on the length of the 
     prologue. If next_frame is NULL this means that there was 
     no debug info and we are called from arc_skip_prologue
  */
  CORE_ADDR final_pc = (next_frame)?frame_pc_unwind(next_frame):pc+64; 
  
  


  if (info)
    {
      /* All functions start as leaf functions until 
	 we identify push blink 
      */
      info->is_leaf = 1;
      
    }
  


  /* Initializations to use the opcodes 
   * library .
   */
  init_disassemble_info(&di, gdb_stderr, fprintf_unfiltered);
  di.arch = gdbarch_bfd_arch_info(current_gdbarch)->arch;
  di.mach = gdbarch_bfd_arch_info(current_gdbarch)->mach;
  di.endian = gdbarch_byte_order(current_gdbarch);
  di.read_memory_func = target_read_memory;
  
  
  for(prologue_ends_pc= pc; prologue_ends_pc< final_pc; 
      prologue_ends_pc += current_instr.instructionLen )
    {
      current_instr = arcAnalyzeInstr(prologue_ends_pc, &di);
      printInsnState(current_instr);
        /* Might be a push or a pop */
      if(current_instr._opcode == 0x3)
	{
	  if(current_instr._addrWriteBack)
	    {
	      /* This is a st.a  */
	      if((current_instr.ea_reg1 == 28) &&
		 (current_instr._offset == -4))
		{
		  
		  /* This is a push something at sp */
		  /* Is it a push of the blink */
		  ISPUSHBLINKFI(current_instr);
		  /* Is it a push for fp */
		  ISPUSHFPFI(current_instr);
		  
		}
	      
	    } 
	  else
	    {
	      /* Is this a store of some register onto 
		 the stack using the stack pointer.*/
	      if(current_instr.ea_reg1 == 28)
		{
		  if(current_instr.sourceType == ARC_REGISTER )
		    {
		      /* R13..R26 are the callee saved registers.  [R27 (fp)
			 is also a callee saved register, but it's usually
			 pushed using st.a and so handled in the st.a case
			 above.]  */
		      if((current_instr.source_operand.registerNum > 12 
			  && current_instr.source_operand.registerNum <= 26))
			{
			  if(info)
			    {
			      printFrameInfo(info);
			      /* Save up the offsets for the correct instruction */
			      info->saved_regs[current_instr.source_operand.registerNum].addr
				= - info->sp_offset - current_instr._offset;
			      saved_regs_mask |= (1 << current_instr.source_operand.registerNum);
			    }
			  continue;
			}
		      
		    }		  
		  
		}
	      /* Is this the store of some register on the 
		 stack using the frame pointer. We check 
		 for argument registers getting saved and
		 restored.
	      */
	      if(current_instr.ea_reg1 == 27)
		if((current_instr.source_operand.registerNum <= 7))
		  {
		    /* Saving argument registers.Don't save them in saved_regs, just skip. 
		     */
		    continue;
		  }
		
	      
	      
	    }
	}

      if(current_instr._opcode == 0x4)
	{
	  /* A major opcode 0x4 instruction */
	  /* We are usually interested in a 
	     mov or a sub */
	  ISUPDATEFPFI(current_instr);
	  ISSUBSPFI(current_instr);
	}
      if(current_instr._opcode == 0x18)
	{
	  /* sub_s sp,sp,constant */
	  ISSUBSPFI(current_instr);
	  /* push_s blink */
	  if(strcmp(current_instr.instrBuffer,"push_s") == 0)
	    {
	      if(strcmp(current_instr.operandBuffer,"blink") == 0)
		{
		  if(info) 
		    {   
		      info->sp_offset += 4; 
		      info->blink_offset = info->sp_offset ; 
		      info->is_leaf = 0;
		    }
		  continue;
		}
	    }
	}

      /* If we reach here . we have 
       * reached end of the prologue
       */
      break;
      
    }
  
  /* Means we were called from skip_prologue */
  if((next_frame == NULL)&& (info == NULL))
    {
      return prologue_ends_pc;
    }
  
  
  info->framesize = -info->sp_offset;
  /* Compute the previous frame's stack pointer (which is also the
     frame's ID's stack address), and this frame's base pointer.  */
  if(info->uses_fp)
    {

      ULONGEST this_base;
      /* The SP was moved to the FP.  This indicates that a new frame
         was created.  Get THIS frame's FP value by unwinding it from
         the next frame.  */
      frame_unwind_unsigned_register(next_frame, ARC_FP_REGNUM,
				     &this_base);
      info->frame_base = this_base;
      info->saved_regs[ARC_FP_REGNUM].addr = info->frame_base;

      /* The previous sp is the current frame base + the offset of the 
	 fp in the current frame */
      info->prev_sp = info->frame_base + info->fp_offset;
      for(i = 13; i < 26 ; i++ )
	{
	  if(saved_regs_mask & (1 << i))
	    info->saved_regs[i].addr += info->frame_base ;
	}
      
      printFrameInfo(info);
   
    }
  else
    {
      ULONGEST this_base;      
      /* Assume that the FP is this frame's SP but with that pushed
         stack space added back.  */
      frame_unwind_unsigned_register (next_frame, ARC_SP_REGNUM, &this_base);
      info->frame_base = this_base;

      /* In such a case it would be the previous SP + the size of the current frame */
      info->prev_sp = info->frame_base + info->framesize;
     
    }
  

  if(!info->is_leaf)
    {

      /* Usually blink is saved before the callee save registers and 
	 below the space created for variadic arguments  . We maintain
	 info->blink_offset as negative when we stored it initially
       */
      info->saved_regs[ARC_BLINK_REGNUM].addr = info->prev_sp + info->blink_offset;
#ifdef ARC_DEBUG
      printf("blink offset is [%x] \n",info->blink_offset);
#endif
    }
      
    /* The PC is found in blink (the actual register or located on the stack).  */
  info->saved_regs[PC_REGNUM] = info->saved_regs[ARC_BLINK_REGNUM];
  /*info->saved_regs[ARC_PC_REGNUM] = info->saved_regs[ARC_BLINK_REGNUM];*/
  return prologue_ends_pc;
#endif
}


/* Skip the prologue for the function at pc. 
 * This is done by checking from the line  
 * information picked up during dwarf reading
 * FIXME: more stuff to be added when we 
 * parse the prologue.
 */

static CORE_ADDR
arc_skip_prologue (CORE_ADDR pc)
{
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_skip_prologue called\n");
#endif
  // FIXMEA: cleanup #else
  unsigned long inst;
  unsigned long addend = 4;
  CORE_ADDR skip_pc = pc;
  CORE_ADDR func_addr, func_end = 0;
  char *func_name;
  struct symtab_and_line sal;

  /* If we're in a dummy frame, don't even try to skip the prologue.  */
  if (deprecated_pc_in_call_dummy (pc))
    return pc;

  /* See what the symbol table says.  */

  if (find_pc_partial_function (pc, &func_name, &func_addr, &func_end))
    {
      struct symbol *sym;

      /* Found a function.  */
      sym = lookup_symbol (func_name, NULL, VAR_DOMAIN, NULL, NULL);
      if (sym && SYMBOL_LANGUAGE (sym) != language_asm)
        {
	  /* Don't use this trick for assembly source files.  */
	  sal = find_pc_line (func_addr, 0);
	  if ((sal.line != 0) && (sal.end < func_end))
	    return sal.end;
        }
    }
  
#ifdef ARC4_JTAG 
  skip_pc = a4_scan_prologue(pc, NULL, NULL);
#else
  skip_pc = arc_scan_prologue(pc,NULL,NULL);
#endif  
  return skip_pc;		/* End of prologue */

  //#endif
}

/* Breakpoint from pc. Return whatever is in the tdep 
 * structure. The tdep structure is changed depending 
 * on the correct target / architecture chosen. 
 */

static const unsigned char *
arc_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{

  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  *lenptr = tdep->arc_breakpoint_size;
  return tdep->arc_breakpoint_insn;
}


/* Assuming NEXT_FRAME->prev is a dummy, return the frame ID of that
   dummy frame.  The frame ID's base needs to match the TOS value
   saved by save_dummy_frame_tos(), and the PC match the dummy frame's
   breakpoint. */

static struct frame_id
arc_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_id_build (arc_unwind_sp (gdbarch, next_frame),
			 frame_pc_unwind (next_frame));
}

/* The workhorse : frame_unwind_cache for the ARC700 platform .
 */
static struct arc_unwind_cache *
arc_frame_unwind_cache  (struct frame_info *next_frame,
			 void **this_prologue_cache)
{
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_frame_unwind_cache called\n ");
#endif
  //#else
  CORE_ADDR pc;
  struct arc_unwind_cache *info;
  int i;

 
  if ((*this_prologue_cache))
    return (*this_prologue_cache);
  
  info = FRAME_OBSTACK_ZALLOC (struct arc_unwind_cache);
  (*this_prologue_cache) = info;
  info->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  /* Zero all fields.  */
  info->blink_offset = 0;
  info->prev_pc = 0;
  info->prev_sp = 0;
  info->frame_base = 0;
  info->framesize = 0;
  info->sp_offset = 0;
  info->fp_offset = 0;
  info->prev_pc = 0;
  info->is_leaf = 0;
  info->uses_fp = 0;

  /* Prologue analysis does the rest...  */
  /* Currently our scan prologue does not
   * support getting input for the frame unwinder
   */ 
  
  pc = frame_func_unwind(next_frame);
#ifdef ARC4_JTAG
  a4_scan_prologue (pc, next_frame, info);
#else
  arc_scan_prologue (pc, next_frame, info);
#endif

  return info;
  //#endif
}



/*
 * Construct frame id for the normal frame 
 */

static void
arc_frame_this_id (struct frame_info *next_frame,
		    void **this_prologue_cache,
		    struct frame_id *this_id)
{
  // FIXMEA: cleanup #ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\n arc_frame_this_id called()\n  ");
#endif
  //#else

  struct arc_unwind_cache *info
    = arc_frame_unwind_cache (next_frame, this_prologue_cache);
  CORE_ADDR base;
  CORE_ADDR func;
  struct frame_id id;

  /* The FUNC is easy.  */
  func = frame_func_unwind (next_frame);

 /* This is meant to halt the backtrace at the entry point (_start).  */
  if (func <= gdbarch_tdep (current_gdbarch)->lowest_pc)
    return;
  
  /* Hopefully the prologue analysis either correctly determined the
     frame's base (which is the SP from the previous frame), or set
     that base to "NULL".  */
  base = info->prev_sp;
  if (base == 0)
    return;

  id = frame_id_build (base, func);

  (*this_id) = id;
  //#endif

}


/* 
 * Unwind and obtain the register information 
 */
static void
arc_frame_prev_register (struct frame_info *next_frame,
			  void **this_prologue_cache,
			  int regnum, int *optimizedp,
			  enum lval_type *lvalp, CORE_ADDR *addrp,
			  int *realnump, void *bufferp)
{
  // FIXMEA:
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\n arc_frame_prev_register() called for regnum %d\n ",regnum );
#endif
  //#else
  struct arc_unwind_cache *info
    = arc_frame_unwind_cache (next_frame, this_prologue_cache);


  /* If we are asked to unwind the PC, then we need to return blink
     instead.  The saved value of PC points into this frame's
     prologue, not the next frame's resume location.  */
#ifdef ARC4_JTAG
  if (regnum == ARC_STATUS_REGNUM)
#else
  if (regnum == PC_REGNUM)
#endif
    regnum = ARC_BLINK_REGNUM;

  /* SP is generally not saved to the stack, but this frame is
     identified by NEXT_FRAME's stack pointer at the time of the call.
     The value was already reconstructed into PREV_SP.  */
  if (regnum == ARC_SP_REGNUM)
    {
      *lvalp = not_lval;
      if (bufferp)
	store_unsigned_integer (bufferp, 4, info->prev_sp);
      return;
    }


  trad_frame_get_prev_register (next_frame, info->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, bufferp);


#ifdef ARC_DEBUG
  printf("-*-*-*\n Regnum =%d, realnump=%d,%d \n",regnum, (char *)(bufferp), *((char*)bufferp));
#endif
  //#endif
}




static const struct frame_unwind arc_frame_unwind = {
  NORMAL_FRAME,
  arc_frame_this_id,
  arc_frame_prev_register
};

const struct frame_unwind *
arc_frame_sniffer (struct frame_info *next_frame)
{
  return &arc_frame_unwind;
}


/* read-only registers */
static int
arc_cannot_store_register (int regno)
{
  if(
#ifndef ARC4_JTAG
     regno == ARC_EFA_REGNUM ||
     regno == ARC_ERET_REGNUM ||
     regno == ARC_STATUS32_L1_REGNUM ||
     regno == ARC_STATUS32_L2_REGNUM ||
     regno == ARC_ERSTATUS_REGNUM ||
#endif
     regno == ARC_ILINK1_REGNUM ||
     regno == ARC_ILINK2_REGNUM 
     )
    {
      /* No warning should be printed.  arc_cannot_store_register being
	 called does not imply that someone is actually writing to regnum.  */

      /*warning("writing to read-only register: %s\n", arc_register_name(regno));*/
      return 1;
    }
  return 0;
}

/* Returns true if the insn at PC is a branch. *fall_thru is the address of
   the next insn.  *target is the branch target. */
static int
arc_next_pc(CORE_ADDR pc, CORE_ADDR *fall_thru, CORE_ADDR *target)
{
#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_next_pc called\n");
#endif
#else
  struct arcDisState instr, instr_d;
  int insn_length;
  struct disassemble_info di;
  int two_targets = 0;
  
  init_disassemble_info(&di, NULL, NULL);
  di.arch = gdbarch_bfd_arch_info(current_gdbarch)->arch;
  di.mach = gdbarch_bfd_arch_info(current_gdbarch)->mach;
  di.endian = gdbarch_byte_order(current_gdbarch);
  di.read_memory_func = target_read_memory;
  
  instr = arcAnalyzeInstr(pc, &di);

  *fall_thru = pc + instr.instructionLen;

#ifdef ARC_DEBUG
  printf("--- arc_next_pc(%x) = %x, isBranch = %d, tcnt = %d [%x], flow = %s (%d), "
	 "reg for indirect jump = %d, nullifyMode = %s\n",
	 pc, *fall_thru, instr.isBranch, instr.tcnt, instr.targets[0],
	 (instr.flow == direct_jump || instr.flow == direct_call) ? "direct" : "indirect",
	 instr.flow,
	 instr.register_for_indirect_jump,
	 (instr.nullifyMode == BR_exec_always ? "delay slot" : "no delay"));
#endif

  if(instr.isBranch)
    {
      two_targets = 1;
      
      if(instr.flow == direct_jump || instr.flow == direct_call)
	*target = instr.targets[0];
      else
	regcache_cooked_read(current_regcache,
			     arc_binutils_reg_to_regnum(instr.register_for_indirect_jump),
			     target);
    }

  /* for instructions with delay slots, the fall thru is not the instruction
     immediately after the branch instruction, but the one after that */
  if(instr.isBranch && instr.nullifyMode == BR_exec_always)
    {
      instr_d = arcAnalyzeInstr(*fall_thru, &di);
      *fall_thru += instr_d.instructionLen;
    }

  /* zero-overhead loops:
        if(status32[L] == 0 && next_pc == lp_end && lp_count > 1)
          next_pc = lp_start;
  */
  {
    unsigned int lp_end, lp_start, lp_count, status32;

    regcache_cooked_read(current_regcache, ARC_LP_START_REGNUM, &lp_start);
    regcache_cooked_read(current_regcache, ARC_LP_END_REGNUM, &lp_end);
    regcache_cooked_read(current_regcache, ARC_LP_COUNT_REGNUM, &lp_count);
#ifndef ARC4_JTAG    
    regcache_cooked_read(current_regcache, ARC_STATUS32_REGNUM, &status32);
#endif

    if( !(status32 & ARC_STATUS32_L) && *fall_thru == lp_end && lp_count > 1)
      {
	two_targets = 1;
	*target = lp_start;
      }
  }
  
  return two_targets;
#endif //
}

/* this is called with insert_breakpoints_p = 1 before single-stepping and
   with insert_breakpoints_p = 0 after the step */
void
arc_software_single_step(enum target_signal ignore, int insert_breakpoints_p)
{
#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_software_single_step called\n" );
#endif
#else
  typedef char binsn_quantum[BREAKPOINT_MAX];

  static CORE_ADDR fall_thru, branch_target;
  static binsn_quantum break_mem[2];
  static char two_breakpoints;
  CORE_ADDR pc;

  {
#ifdef ARC_DEBUG
    unsigned int efa, ret;
    regcache_cooked_read(current_regcache, ARC_EFA_REGNUM, &efa);
    //    regcache_cooked_read(current_regcache, ARC_RET_REGNUM, &ret);
    
    printf("--- arc_software_single_step, efa = %x, ret = %x, (%s)\n", efa, ret,
	   (insert_breakpoints_p ? "add" : "remove"));
#endif
  }
  
  if (insert_breakpoints_p)
    {
      pc = read_pc ();
      two_breakpoints = arc_next_pc (pc, &fall_thru, &branch_target);

      if (two_breakpoints && branch_target == pc) 
	{
	  warning ("Cannot single-step branch-to-self or single instruction zero overhead loop,\n"
		   "    Stepping across it.");
	  /* Don't insert/remove the branch-target breakpoint.  */
	  two_breakpoints = 0;
	}
      
      target_insert_breakpoint (fall_thru, break_mem[0]);
      if(two_breakpoints)
	target_insert_breakpoint (branch_target, break_mem[1]);
    }
  else
    {
      target_remove_breakpoint (fall_thru, break_mem[0]);
      if(two_breakpoints)
	target_remove_breakpoint (branch_target, break_mem[1]);
    }
#endif
}

/*
 * mapping from binutils/gcc register number to
 * GDB register number ("regnum")
 */
static int
arc_binutils_reg_to_regnum (int reg)
{
#ifdef ARC4_JTAG
    if (reg >= 0 && reg <= 26)
    return reg;
  else if (reg == 27)			     /* fp */
    return ARC_FP_REGNUM;
  else if (reg == 28)			     /* sp */
    return ARC_SP_REGNUM;
  else if (reg == 29)			     /* ilink1 */
    return ARC_ILINK1_REGNUM;
  else if (reg == 30)			     /* ilink2 */
    return ARC_ILINK2_REGNUM;
  else if (reg == 31)			     /* blink */
    return ARC_BLINK_REGNUM;

#else
  /* from gcc/config/arc/arc.h */

  if (reg >= 0 && reg <= 26)
    return reg;
  else if (reg == 27)			     /* fp */
    return ARC_FP_REGNUM;
  else if (reg == 28)			     /* sp */
    return ARC_SP_REGNUM;
  else if (reg == 29)			     /* ilink1 */
    return ARC_ILINK1_REGNUM;
  else if (reg == 30)			     /* ilink2 */
    return ARC_ILINK2_REGNUM;
  else if (reg == 31)			     /* blink */
    return ARC_BLINK_REGNUM;
  else if (reg >= 32 && reg <= 59)	     /* reserved */
    ;
  else if (reg == 60)			     /* lp_count */
    return ARC_LP_COUNT_REGNUM;
  else if (reg == 61)			     /* reserved */
    ;
  else if (reg == 62)			     /* no such register */
    ;
/*   else if (reg == 63)			     /\* PCL *\/ */
/*     return ARC_RET_REGNUM; */

#endif
  warning ("Unmapped register #%d encountered\n", reg);
  return -1; 
}


static void
arc_add_reggroups (struct gdbarch *gdbarch)
{
  reggroup_add (gdbarch, general_reggroup);
  reggroup_add (gdbarch, all_reggroup);
  reggroup_add (gdbarch, system_reggroup);
}

int
arc_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
			  struct reggroup *group)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int tdep_answer;
  
  tdep_answer = tdep->register_reggroup_p (regnum, group);
  if(tdep_answer != -1)
    return tdep_answer;
  
  if (group == all_reggroup)
    return 1;
  else if (group == save_reggroup || group == restore_reggroup)
    {
      /* don't save/restore read-only registers. */
      return (!arc_cannot_store_register(regnum)); 
    }
  else if (group == general_reggroup)
    {
#ifndef ARC4_JTAG
      if (regnum == ARC_STATUS32_REGNUM)
	return 0;
#endif
      return 1;
    }
  else
    {
      internal_error(__FILE__, __LINE__, "bad register group");
    }
}



static void
arc_dwarf2_frame_init_reg (struct gdbarch *gdbarch, int regnum,
                            struct dwarf2_frame_state_reg *reg)
{
#ifdef ARC4_JTAG
  // FIXMEA: Clean up.  if ( debug_arc_jtag_target_message)
#ifdef ARC_DEBUG  
  printf ("\n arc_dwarf2_frame_init_reg called.\n Regno no:%d,0x%x\n",regnum,regnum);
#endif
    /* The return address column.  */
  if (regnum == ARC_STATUS_REGNUM)
    reg->how = DWARF2_FRAME_REG_RA;
  
  /* The call frame address.  */
  if (regnum == ARC_SP_REGNUM)
    reg->how = DWARF2_FRAME_REG_CFA;

#else
  /* The return address column.  */
  if (regnum == PC_REGNUM)
    reg->how = DWARF2_FRAME_REG_RA;
  
  /* The call frame address.  */
  if (regnum == ARC_SP_REGNUM)
    reg->how = DWARF2_FRAME_REG_CFA;
#endif
}

static CORE_ADDR
arc_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST pc;
#ifdef ARC4_JTAG
  frame_unwind_unsigned_register (next_frame, ARC_STATUS_REGNUM, &pc);
  pc = pc & 0x00ffffff;
  pc = pc << 2;
#else
  frame_unwind_unsigned_register (next_frame, PC_REGNUM, &pc);
#endif
  return pc;
}

static CORE_ADDR
arc_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;
  frame_unwind_unsigned_register (next_frame, SP_REGNUM, &sp);
  return sp;
}



static void
arc_extract_return_value (struct type *type, struct regcache *regcache,
			  void *valbuf)
{
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_extract_return_value called\n");
#endif
  //#else
  ULONGEST val;
  int len = TYPE_LENGTH (type);
  
  if (len <= 4)
    {
      /* Get the return value from R0.  */
      regcache_cooked_read_unsigned (regcache, ARC_RETURN1_REGNUM, &val);
      store_unsigned_integer (valbuf, len, val);
    }
  else if (len <= 8)
    {
      /* Get the return value from R0 and R1.  */
      /* R0 holds the lower-order bytes */
      regcache_cooked_read_unsigned (regcache, ARC_RETURN1_REGNUM, &val);
      store_unsigned_integer (valbuf, 4, val);
      regcache_cooked_read_unsigned (regcache, ARC_RETURN2_REGNUM, &val);
      store_unsigned_integer ((char *)valbuf + 4, len - 4, val);
    }
  else
    error ("arc_extract_return_value: type length too large");
  //#endif
}

static void
arc_store_return_value (struct type *type, struct regcache *regcache,
			const void *valbuf)
{
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_store_return_value called\n ");
#endif
  //#else
  ULONGEST val;
  int len = TYPE_LENGTH (type);
  
  if (len <= 4)
    {
      /* Put the return value in R0.  */
      val = extract_unsigned_integer (valbuf, len);
      regcache_cooked_write_unsigned (regcache, ARC_RETURN1_REGNUM, val);
    }
  else if (len <= 8)
    {
      /* Put the return value in R10 and R11.  */
      val = extract_unsigned_integer (valbuf, 4);
      regcache_cooked_write_unsigned (regcache, ARC_RETURN1_REGNUM, val);
      val = extract_unsigned_integer ((char *)valbuf + 4, len - 4);
      regcache_cooked_write_unsigned (regcache, ARC_RETURN2_REGNUM, val);
    }
  else
    error ("arc_store_return_value: type length too large.");
  //#endif
}


static enum return_value_convention
arc_return_value (struct gdbarch *gdbarch, struct type *valtype,
		  struct regcache *regcache, void *readbuf,
		  const void *writebuf)
{
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_return_value called");
#endif
  //#else
  /* This will change with the ABI */
  int struct_return = (TYPE_CODE (valtype) == TYPE_CODE_STRUCT ||
		       TYPE_CODE (valtype) == TYPE_CODE_UNION ||
		       TYPE_LENGTH (valtype) > 8);
  

  if (writebuf != NULL)
    {
      gdb_assert (!struct_return);
      arc_store_return_value (valtype, regcache, writebuf);
    }

  if (readbuf != NULL)
    {
      gdb_assert (!struct_return);
      arc_extract_return_value (valtype, regcache, readbuf);
    }

  if (struct_return)
    return RETURN_VALUE_STRUCT_CONVENTION;
  else
    return RETURN_VALUE_REGISTER_CONVENTION;
  //#endif
}

/* Signal Trampoline Frame Unwinder. These 
 * unwinders allow frame unwinding to happen 
 * from within signal handlers. 
 */

static struct arc_unwind_cache *
arc_sigtramp_frame_cache (struct frame_info *next_frame,
			  void **this_cache)
{
  // FIXMEA: cleanup#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_sigtramp_frame_cache called");
#endif
  //#else
  struct arc_unwind_cache *cache;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  CORE_ADDR addr;
  char buf[4];

  if (*this_cache)
    return *this_cache;

  cache = FRAME_OBSTACK_ZALLOC (struct arc_unwind_cache);
  (*this_cache) = cache;
  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  /* Zero all fields.  */
  cache->blink_offset = 0;
  cache->prev_pc = 0;
  cache->prev_sp = 0;
  cache->frame_base = 0;
  cache->framesize = 0;
  cache->sp_offset = 0;
  cache->fp_offset = 0;
  cache->prev_pc = 0;
  cache->is_leaf = 0;
  cache->uses_fp = 0;

  
  frame_unwind_register (next_frame, SP_REGNUM, buf);
  cache->frame_base = extract_unsigned_integer (buf, 4);

  addr = tdep->sigcontext_addr (next_frame);
  if (tdep->sc_reg_offset)
    {
      int i;
      
      for (i = 0; i < tdep->sc_num_regs; i++)
	if (tdep->sc_reg_offset[i] != -1)
	  cache->saved_regs[i].addr = addr + tdep->sc_reg_offset[i];
    }

  return cache;
  //#endif
}

static void
arc_sigtramp_frame_this_id (struct frame_info *next_frame, void **this_cache,
			    struct frame_id *this_id)
{
  //FIXMEA: cleanup #ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_sigtramp_frame_this_id called");
#endif
  //#else
  struct arc_unwind_cache *cache = 
    arc_sigtramp_frame_cache (next_frame, this_cache);
  
  (*this_id) = frame_id_build (cache->frame_base, frame_pc_unwind (next_frame));
  //#endif
}

static void
arc_sigtramp_frame_prev_register (struct frame_info *next_frame,
				  void **this_cache,
				  int regnum, int *optimizedp,
				  enum lval_type *lvalp, CORE_ADDR *addrp,
				  int *realnump, void *valuep)
{
  // FIXMEA: cleanup#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_sigtramp_frame_prev_register called");
#endif
  //#else
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  /* Make sure we've initialized the cache.  */ 
  struct arc_unwind_cache *cache = 
    arc_sigtramp_frame_cache (next_frame, this_cache);

  /* on a signal, the PC is in ret */
#ifdef ARC4_JTAG
  if (regnum == ARC_STATUS_REGNUM)
#else
  if(regnum == PC_REGNUM)
#endif
    regnum = tdep->pc_regnum_in_sigcontext;
  
  trad_frame_get_prev_register (next_frame, cache->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, valuep);
  //#endif
}


static const struct frame_unwind arc_sigtramp_frame_unwind =
{
  SIGTRAMP_FRAME,
  arc_sigtramp_frame_this_id,
  arc_sigtramp_frame_prev_register
};

const struct frame_unwind *
arc_sigtramp_frame_sniffer (struct frame_info *next_frame)
{
  //FIXMEA: cleanup#ifdef ARC4_JTAG
#ifdef ARC_DEBUG  
  printf ("\narc_sigtramp_frame_sniffer called() ");
#endif
  //#else
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  struct gdbarch_tdep *tdep = gdbarch_tdep (get_frame_arch (next_frame));

  /* We shouldn't even bother if we don't have a sigcontext_addr
     handler.  */
  if (tdep->sigcontext_addr == NULL)
    return NULL;

  if (tdep->sigtramp_p != NULL)
    {
      if (tdep->sigtramp_p (next_frame))
	{
	  return &arc_sigtramp_frame_unwind;
	}
    }

  return NULL;
  //#endif
}



/* Allow calls to be made to functions defined in the debuggee. 
   a.k.a dummy calls 
*/
/* When arguments must be pushed onto the stack, they go on in reverse
   order.  The below implements a FILO (stack) to do this.  
   Copied from d10v-tdep.c.  */

struct stack_item
{
  int len;
  struct stack_item *prev;
  void *data;
};

static struct stack_item *
push_stack_item (struct stack_item *prev, void *contents, int len)
{
  struct stack_item *si;
  si = xmalloc (sizeof (struct stack_item));
  si->data = xmalloc (len);
  si->len = len;
  si->prev = prev;
  memcpy (si->data, contents, len);
  return si;
}

static struct stack_item *
pop_stack_item (struct stack_item *si)
{
  struct stack_item *dead = si;
  si = si->prev;
  xfree (dead->data);
  xfree (dead);
  return si;
}




/* arc_push_dummy_call :
 *  gdbarch  : gdbarch structure for the backend to use if needed.
 *  function : 
 *  regcache : 
 *  bp_addr  : Return address for the breakpoint.
 *  sp       : Current value of sp.
 *  struct_return: struct_return is 1 if structures are returned by 
 *                 the function.
 *  struct_addr: Hidden address for returning a struct.
 */


static CORE_ADDR
arc_push_dummy_call(struct gdbarch *gdbarch, struct value *function,
		     struct regcache *regcache, CORE_ADDR bp_addr, int nargs,
		     struct value **args, CORE_ADDR sp, int struct_return,
		     CORE_ADDR struct_addr)

{
  //#ifdef ARC4_JTAG
#ifdef ARC_DEBUG
  printf ("\narc_push_dummy_call called");
#endif
  // #else
  int stack_alloc;
  int stack_offset;
  int argreg;
  int argnum;

  CORE_ADDR regval;
  struct stack_item *si = NULL;


  /* Push the return address.  */
#ifdef ARC4_JTAG
  CORE_ADDR modified_bp_addr;
  modified_bp_addr = arc_debug_fetch_regs(ARC_STATUS_REGNUM);
  regcache_raw_collect(regcache, ARC_STATUS_REGNUM, &modified_bp_addr);
  modified_bp_addr = modified_bp_addr & 0xff000000;
  bp_addr = bp_addr >>2;
  modified_bp_addr |= bp_addr;
  regcache_cooked_write_unsigned (regcache, ARC_BLINK_REGNUM, modified_bp_addr);
#else
  regcache_cooked_write_unsigned (regcache, ARC_BLINK_REGNUM, bp_addr);
#endif

  /* Are we returning a value using a structure return or a normal value
     return?  struct_addr is the address of the reserved space for the return
     structure to be written on the stack.
  */
  /* FIXME:: Ramana :: What about 4 byte structures returned in r0 as
     claimed by Metaware. 
  */
  
  /* Now load as many as possible of the first arguments into registers,
     and push the rest onto the stack.  */
  argreg = ARC_ARG0_REGNUM;
 
  if (struct_return)
    {
      regcache_cooked_write_unsigned (regcache, ARC_ARG0_REGNUM, struct_addr);
      argreg++;
#ifdef ARC4_JTAG
      sp = sp - 16;
#endif
    }

  stack_offset = 0;

  for (argnum = 0; argnum < nargs; argnum++)
    {
      int len;
      char *val;
      int reg_demand;
      int i;
      
      len = TYPE_LENGTH (VALUE_TYPE (args[argnum]));
      val = (char *) VALUE_CONTENTS (args[argnum]);
      
      /* How may registers worth of storage do we need for this argument?  */
      reg_demand = (len / 4) + (len % 4 != 0 ? 1 : 0);
        
      if (argreg + reg_demand - 1 <= ARC_ARG7_REGNUM)
        {
          /* Data passed by value.  Fits in available register(s).  */
          for (i = 0; i < reg_demand; i++)
            {
              regcache_cooked_write_unsigned (regcache, argreg, 
					      *(unsigned long *) val);
              argreg++;
              val += 4;
            }
        }
      else if (argreg <= ARC_ARG7_REGNUM)
        {
          /* Data passed by value. Does not fit in available register(s).
             Use the register(s) first, then the stack.  */
          for (i = 0; i < reg_demand; i++)
            {
              if (argreg <= ARC_ARG7_REGNUM)
                {
		  regcache_cooked_write_unsigned (regcache, argreg, 
						  *(unsigned long *) val);
                  argreg++;
                  val += 4;
                }
              else
                {
		  /* Push item for later so that pushed arguments
		     come in the right order.  */
		  si = push_stack_item (si, val, 4);
                  val += 4;
                }
            }
        }
      else if (len > (2 * 4))
        {
	  /* FIXME */
	  internal_error (__FILE__, __LINE__, "We don't do this");
        }
      else
        {
          /* Data passed by value.  No available registers.  Put it on
             the stack.  */
	   si = push_stack_item (si, val, len);
        }
    }

  while (si)
    {
      /* fp_arg must be word-aligned (i.e., don't += len) to match
	 the function prologue.  */
      sp = (sp - si->len) & ~3;
#ifdef ARC4_JTAG
      write_memory (sp + 16, si->data, si->len);
#else
      write_memory (sp, si->data, si->len);
#endif
      si = pop_stack_item (si);
    }

  /* Finally, update the SP register.  */
  regcache_cooked_write_unsigned (regcache, ARC_SP_REGNUM, sp);

  return sp;
  //#endif
}

/* Align Frame */
static CORE_ADDR
arc_frame_align (struct gdbarch *gdbarch, CORE_ADDR sp)
{
  /* Align to the normal alignment on the stack).  */
  return sp & ~3;
}


/* Print interesting information about the floating point processor
   (if present) or emulator.  */
static void
arc_print_float_info (struct gdbarch *gdbarch, struct ui_file *file,
		      struct frame_info *frame, const char *args)
{
  printf("Software FPU \n");
}


/* Set the main_name to "_main" if required.
   This is set as an observer of inferior_created.  */
static void
arc_set_main_name (struct target_ops *objfile, int from_tty)
{
  struct minimal_symbol *umainsym, *mainsym;

  /* Old ARC toolchains prepend an underscore to symbol names.  If there is
     an _main but no main, then we're probably debugging a binary that was
     made with the old toolchain.  */
  umainsym = lookup_minimal_symbol ("_main", NULL, NULL);
  mainsym = lookup_minimal_symbol ("main", NULL, NULL);
  if(umainsym && !mainsym)
    {
      set_main_name ("_main");
    }

  /* If we don't have any symbols, the default, i.e. "main", will get used.  */
}


/* The following piece of code is borrowed from d10v */
static void
a4_address_to_pointer (struct type *type, void *buf, CORE_ADDR addr)
{
#ifdef ARC4_JTAG
  if (TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_FUNC
      || TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_METHOD)
    store_unsigned_integer (buf, TYPE_LENGTH (type), (addr>>2) & 0xffffff);
  else
    store_unsigned_integer (buf, TYPE_LENGTH (type), addr);
#endif
}

static CORE_ADDR
a4_pointer_to_address (struct type *type, const void *buf)
{
#ifdef ARC4_JTAG  
  CORE_ADDR addr = extract_unsigned_integer (buf, TYPE_LENGTH(type));
  /* Is it a code address?  */
  if (TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_FUNC
      || TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_METHOD
      || TYPE_CODE_SPACE (TYPE_TARGET_TYPE (type)))
    return ((addr<<2) & 0x2ffffff);
  else
    return addr;
#endif
}

static struct gdbarch *
arc_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch_tdep *tdep;
  struct gdbarch *gdbarch;

  tdep = xmalloc (sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);
  
  /* Fixme :: Worry about default initialization of breakpoints 
     for the ARC platform. In our case currently this is handled
     out of arc-linux-tdep.c for default arc linux breakpoints.
  */

  info.osabi = CONFIG_OSABI;
  gdbarch_init_osabi(info, gdbarch);
  
  /* Put stuff in gdbarch. */

  /* Characters are unsigned by default */
  set_gdbarch_char_signed (gdbarch, 0);

  set_gdbarch_print_float_info (gdbarch, arc_print_float_info);
  set_gdbarch_sp_regnum (gdbarch, ARC_SP_REGNUM);
  set_gdbarch_register_type (gdbarch, arc_register_type);  

  set_gdbarch_cannot_store_register (gdbarch, arc_cannot_store_register);


  /* Advance PC across function entry code.  */
  set_gdbarch_skip_prologue (gdbarch, arc_skip_prologue);

  
  /* Hook in the Dwarf-2 frame sniffer.  */
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, arc_binutils_reg_to_regnum);
  dwarf2_frame_set_init_reg (gdbarch, arc_dwarf2_frame_init_reg);
  frame_unwind_append_sniffer (gdbarch, dwarf2_frame_sniffer);

  /* signal frames */
  frame_unwind_append_sniffer (gdbarch, arc_sigtramp_frame_sniffer);



  /* The stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  set_gdbarch_unwind_pc (gdbarch, arc_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, arc_unwind_sp);
  set_gdbarch_unwind_dummy_id (gdbarch, arc_unwind_dummy_id);
  frame_unwind_append_sniffer (gdbarch, arc_frame_sniffer);
 

  set_gdbarch_return_value (gdbarch, arc_return_value);

  /* Add the arc register groups.  */
  arc_add_reggroups (gdbarch);
  set_gdbarch_register_reggroup_p (gdbarch, arc_register_reggroup_p);
  
  /* Breakpoint manipulation.  */
  set_gdbarch_breakpoint_from_pc (gdbarch, arc_breakpoint_from_pc);
  set_gdbarch_frame_align(gdbarch,arc_frame_align);

  /* Dummy Frame handling */
  set_gdbarch_push_dummy_call (gdbarch, arc_push_dummy_call);
  set_gdbarch_call_dummy_location (gdbarch,AT_ENTRY_POINT);
  
  /* Disassembly.  */
  {
    /* the arc libopcodes wants abfd so that it can find out what CPU
       extensions are there */
    bfd abfd;
    abfd.sections = NULL;


#ifndef ARC4_JTAG
    set_gdbarch_print_insn(gdbarch, arcompact_get_disassembler(&abfd));
#else
    set_gdbarch_print_insn(gdbarch, arc_get_disassembler(&abfd));
#endif
  }

#ifdef ARC4_JTAG  
  set_gdbarch_address_to_pointer (gdbarch, a4_address_to_pointer);
  set_gdbarch_pointer_to_address (gdbarch, a4_pointer_to_address);
#endif
  //#ifndef ARC4_JTAG
  /* Set main_name to _main if necessary.  Ideally we'd want a hook that
     gets called when symbols are loaded, but it seems there isn't one; so
     we'll use this.  This will not work if the user does "target remote
     ..." and then "add-symbol-file ..."  */
  observer_attach_inferior_created (arc_set_main_name);
  //#endif

#ifdef ARC4_JTAG
  //  set_gdbarch_write_pc (gdbarch, a4_write_pc);
#endif

  CONFIG_INIT_TDEP (gdbarch);
  
  return gdbarch;
}

static void
arc_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
}

void
_initialize_arc_tdep (void)
{
  gdbarch_register (bfd_arch_arc, arc_gdbarch_init, arc_dump_tdep);
}

