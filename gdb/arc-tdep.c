/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008, 2009 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors:
      Soam Vasani          <soam.vasani@codito.com>
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com>
      Richard Stuckey      <richard.stuckey@arc.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module provides support for the ARC processor family's target     */
/*     dependencies.  In particular, it has knowledge of the processor ABI.   */
/*                                                                            */
/*     See                                                                    */
/*           ARCompact Instruction Set Architecture                           */
/*                   Programmer's Reference                                   */
/*                         (5115-018)                                         */
/*                                                                            */
/*     for a description of ARC processor architecture, and                   */
/*                                                                            */
/*           System V ABI Supplement                                          */
/*                   4093-004                                                 */
/*                                                                            */
/*     for a complete definition of the ABI.                                  */
/*                                                                            */
/*                                                                            */
/* Stack Frame Layout:                                                        */
/*     This shows the layout of the stack frame for the general case of a     */
/*     function call; a given function might not have a variable number of    */
/*     arguments or local variables, or might not save any registers, so it   */
/*     would not have the corresponding frame areas.  Additionally, a leaf    */
/*     function (i.e. one which calls no other functions) does not need to    */
/*     save the contents of the BLINK register (which holds its return        */
/*     address), and a function might not have a frame pointer.               */
/*                                                                            */
/*     N.B. the stack grows downward, so SP points below FP in memory; SP     */
/*          always points to the last used word on the stack, not the first   */
/*          one.                                                              */
/*                                                                            */
/*                   |                       |   |                            */
/*                   |      arg word N       |   | caller's                   */
/*                   |           :           |   | frame                      */
/*                   |      arg word 10      |   |                            */
/*                   |      arg word 9       |   |                            */
/*       old SP ---> |-----------------------| --                             */
/*                   |   var arg word 8      |   |                            */
/*                   |           :           |   |                            */
/*                   |   var arg word P+1    |   |                            */
/*                   |-----------------------|   |                            */
/*                   |                       |   |                            */
/*                   |      callee-saved     |   |                            */
/*                   |        registers      |   |                            */
/*                   |                       |   |                            */
/*                   |-----------------------|   |                            */
/*                   |      saved blink (*)  |   |                            */
/*                   |-----------------------|   | callee's                   */
/*                   |      saved FP         |   | frame                      */
/*           FP ---> |-----------------------|   |                            */
/*                   |                       |   |                            */
/*                   |         local         |   |                            */
/*                   |       variables       |   |                            */
/*                   |                       |   |                            */
/*                   |       register        |   |                            */
/*                   |      spill area       |   |                            */
/*                   |                       |   |                            */
/*                   |     outgoing args     |   |                            */
/*                   |                       |   |                            */
/*           SP ---> |-----------------------| --                             */
/*                   |                       |                                */
/*                   |         unused        |                                */
/*                   |                       |                                */
/*                               |                                            */
/*                               |                                            */
/*                               V                                            */
/*                           downwards                                        */
/*                                                                            */
/*     The list of arguments to be passed to a function is considered to be a */
/*     sequence of N words (as though all the parameters were stored in order */
/*     in memory with each parameter occupying an integral number of words).  */
/*     Words 1 .. 8 are passed in registers 0 .. 7; if the function has more  */
/*     than 8 words of arguments then words 9 .. N are passed on the stack in */
/*     the caller's frame.                                                    */
/*                                                                            */
/*     If the function has a variable number of arguments, e.g. it has a form */
/*     such as                                                                */
/*                      function(p1, p2, ...);                                */
/*                                                                            */
/*     and P words are required to hold the values of the named parameters    */
/*     (which are passed in registers 0 .. P-1), then the remaining 8 - P     */
/*     words passed in registers P .. 7 are spilled into the top of the frame */
/*     so that the anonymous parameter words occupy a continous region.       */
/*                                                                            */
/*     (*) if saved.                                                          */
/*                                                                            */
/* Usage:                                                                     */
/*     The module exports a function _initialize_arc_tdep: the call to this   */
/*     function is generated by the gdb build mechanism, so this function     */
/*     should not be explicitly called.                                       */
/*                                                                            */
/*     The operations provided by this module are registered with gdb during  */
/*     initialization; gdb then calls them via function pointers, rather than */
/*     by name (this allows gdb to handle multiple target architectures):     */
/*                                                                            */
/*          set_gdbarch_XXX (gdbarch, <function>);                            */
/*                                                                            */
/*                                                                            */
/* Build Configuration:                                                       */
/*     The ARC gdb may be built in two different configurations, according to */
/*     the nature of the target that it is to debug:                          */
/*                                                                            */
/*     1) arc-elf32:                                                          */
/*           for debugging 'bare-metal' builds of user code (i.e. built with  */
/*           newlib)                                                          */
/*                                                                            */
/*           ARC-specific modules:                                            */
/*                arc-tdep                                                    */
/*                arc-elf32-tdep                                              */
/*                arc-xiss                                                    */
/*                arc-jtag                                                    */
/*                arc-jtag-ops                                                */
/*                arc-jtag-actionpoints                                       */
/*                arc-gpio                                                    */
/*                arc-remote-fileio                                           */
/*                arc-registers                                               */
/*                arc-architecture                                            */
/*                arc-board                                                   */
/*                arc-arguments                                               */
/*                arc-memory                                                  */
/*                arc-inst-tracing                                            */
/*                                                                            */
/*     2) arc-linux-uclibc:                                                   */
/*           for deugging user mode Linux applications, via communication to  */
/*           the remote gdbserver process, running on Linux for ARC700        */
/*                                                                            */
/*           ARC-specific modules:                                            */
/*                arc-tdep                                                    */
/*                arc-linux-tdep                                              */
/*                                                                            */
/*     This module provides operations which are common to both; operations   */
/*     which are specific to one, or which have different variants in each    */
/*     configuration, are provided by the other modules.                      */
/*                                                                            */
/*                                                                            */
/* Debug Targets:                                                             */
/*     The ARC gdb supports a number of debug targets.  These are:            */
/*                                                                            */
/*         arc-elf32-gdb                                                      */
/*             built-in simulator                       'target sim'          */
/*             ARCangel 4 h/w emulator                  'target arcjtag'      */
/*             dynamically loaded xISS simulator        'target arcxiss'      */
/*             separately executing xISS simulator      'target remote'       */
/*                                                                            */
/*         arc-linux-uclibc-gdb                                               */
/*             gdbserver running on ARC Linux           'target remote'       */
/*                                                                            */
/*     It should, in theory, be possible for either debugger to connect to    */
/*     any remote target which supports the gdb Remote Serial Protocol.       */
/*     However, any such target MUST agree with the debugger on the register  */
/*     numbering scheme that is used, as this controls the order of register  */
/*     contents held in the RSP 'G' (set all registers) packet and the 'g'    */
/*     (get all registers) response packet, as well as the register numbers   */
/*     used in the 'P' (set one register) and 'p' (get one register) packets, */
/*     and in the 'T' stop reply packet.  The schemes used by each debugger   */
/*     are defined in the arc-elf32-tdep and arc-linux-tdep modules.          */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <byteswap.h>

/* gdb header files */
#include "defs.h"
#include "config.h"
#include "arch-utils.h"
#include "dis-asm.h"
#include "frame.h"
#include "frame-unwind.h"
#include "inferior.h"
#include "regcache.h"
#include "reggroups.h"
#include "trad-frame.h"
#include "dwarf2-frame.h"
#include "gdbcore.h"
#include "observer.h"
#include "osabi.h"
#include "gdbcmd.h"
#include "block.h"
#include "dictionary.h"
#include "language.h"
#include "demangle.h"
#include "objfiles.h"
#include "gdb_assert.h"

/* ARC header files */

/* N.B. one and only one of ARC_ELF32_TARGET and ARC_LINUX_TARGET must be defined!  */

#ifdef ARC_ELF32_TARGET
#ifdef ARC_LINUX_TARGET
#error ARC build is not correctly configured (both flags set)
#else
#include "config/arc/tm-embed.h"
#endif
#else
#ifdef ARC_LINUX_TARGET
#include "config/arc/tm-linux.h"
#else
#error ARC build is not correctly configured (no flag set)
#endif
#endif

#include "opcode/arc.h"
#include "opcodes/arc-dis.h"
#include "opcodes/arc-ext.h"
#include "opcodes/arcompact-dis.h"
#include "arc-support.h"
#include "arc-tdep.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef struct
{
    const char  *name;
    CORE_ADDR    address;
    Boolean      is_argument;
    Boolean      is_callee;
    Boolean      is_array;
    unsigned int size;
    unsigned int element_size;
} LocalVariable;


/* The frame unwind cache for the ARC.  */

typedef struct
{
    /* BLINK save location offset from previous SP (-ve value).  */
    int blink_save_offset_from_prev_sp;

    /* The stack pointer at the time this frame was created; i.e. the
       caller's stack pointer when this function was called.  It is used
       to identify this frame.  */
    CORE_ADDR prev_sp;

    /* The frame base (as held in FP).
       N.B. this is NOT the address of the lowest word in the frame!  */
    CORE_ADDR frame_base;

    /* Change in SP from previous SP (-ve value) - this is computed by scanning
       the prologue of the function: initially 0, it is updated for each
       instruction which changes SP (either explicitly by a subtraction from SP
       or implicitly by a push operation), so at each point in the prologue it
       gives the difference between the previous SP (i.e. before the function
       was called) and the current SP at that point; at the end of the prologue
       it holds the total change in SP, i.e. the size of the frame.  */
    LONGEST delta_sp;

    /* Offset of old stack pointer from frame base (+ve value).  */
    LONGEST old_sp_offset_from_fp;

    /* Is this a leaf function? */
    Boolean is_leaf;
    /* Is there a frame pointer? */
    Boolean uses_fp;

    /* Offsets for each register in the stack frame.  */
    struct trad_frame_saved_reg *saved_regs;
    unsigned int                 saved_regs_mask;
} UnwindCache;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define DEBUG_COMMAND               "arc-debug"
#define SHOW_FRAME_COMMAND          "arc-show-frame"
#define SHOW_FRAME_COMMAND_USAGE    "Usage: " SHOW_FRAME_COMMAND " [ <FRAME> ]\n"

#define NEW_LINE                     _("\n")


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* Global debug flag.  */
Boolean arc_debug_target;


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

#define WORD_ALIGNED(addr)         ((addr) & ~(BYTES_IN_WORD - 1))
#define WORDS_OCCUPIED(bytes)      (((bytes) + BYTES_IN_WORD - 1) / BYTES_IN_WORD)
#define ROUND_UP_TO_WORDS(bytes)   (WORDS_OCCUPIED(bytes) * BYTES_IN_WORD)



/* Macros to be used with disassembling the prologue and update the frame info.
   The *FI macros are to update the frame info and the ACT macros are to
   actually do the action on a corresponding match.  */

#define IS_INSTRUCTION(insn_name, search_string)    !strcmp(insn_name, search_string)

#define CHECK_OPERAND_STRING_AND_ACT(target_check, search_string, action) \
    if (strstr(target_check, search_string) == target_check)              \
    {                                                                     \
        action;                                                           \
        return TRUE;                                                      \
    }


/* The frame info changes by changing the decrementing the delta_sp and setting
   the leaf function flag to be False (if this function prologue is saving blink
   then it must be going to call another function - so it can not be a leaf!);
   also the offset of the blink register save location from the previous value
   of sp is recorded.  This will eventually used to compute the address of the
   save location:

           <blink saved address> = <prev sp> + <blink offset from prev sp>

   The addition (+=) below is because the sp offset and the instruction offset
   are negative - so incrementing the sp offset by the instruction offset is
   actually making the sp offset more negative, correctly reflecting that SP
   is moving further down the downwards-growing stack.  */

#define PUSH_BLINK(offset)                                           \
    {                                                                \
        info->delta_sp += offset;                                    \
        info->blink_save_offset_from_prev_sp = (int) info->delta_sp; \
        info->is_leaf = FALSE;                                       \
    }

#define PUSH_BLINK_ACT                          \
    do {                                        \
           if (info) PUSH_BLINK(instr->_offset) \
       } while (0);


#define IS_PUSH_BLINK_FI(state) CHECK_OPERAND_STRING_AND_ACT(state->operandBuffer, "blink", PUSH_BLINK_ACT)


/* At the point that that FP is pushed onto the stack (so saving the dynamic
   link chain pointer to the previous frame), at the address that will be the
   base of the new frame, we know the offset of SP from the previous SP - so the
   offset of the old SP from the new frame base is known (the -ve delta_sp is
   negated to give the +ve old_sp_offset_from_fp).  */
#define PUSH_FP_ACT  do {                                                       \
                            if (info)                                           \
                            {                                                   \
                                 info->delta_sp += instr->_offset;              \
                                 info->old_sp_offset_from_fp = -info->delta_sp; \
                        }} while (0);

#define IS_PUSH_FP_FI(state) CHECK_OPERAND_STRING_AND_ACT(state->operandBuffer, "fp", PUSH_FP_ACT)


#define UPDATE_FP_ACT    do {                         \
                             if (info)                \
                                info->uses_fp = TRUE; \
                          } while (0);

#define IS_UPDATE_FP_FI(state)                                                    \
   if (IS_INSTRUCTION(state->instrBuffer, "mov"))                                 \
   {                                                                              \
      CHECK_OPERAND_STRING_AND_ACT(state->operandBuffer, "fp,sp", UPDATE_FP_ACT); \
   }


#define UPDATE_STACK_SPACE(state)    do {                                                 \
                                       if (info) {                                        \
                                          /* Eat up sp,sp.  */                            \
                                          int immediate = atoi(state->operandBuffer + 6); \
                                          info->delta_sp -= immediate;                    \
                                      }} while (0);


#define IS_SUB_SP_FI(state)                                                                    \
    if (IS_INSTRUCTION(state->instrBuffer, "sub") ||                                           \
        IS_INSTRUCTION(state->instrBuffer, "sub_s"))                                           \
    {                                                                                          \
        CHECK_OPERAND_STRING_AND_ACT(state->operandBuffer, "sp,sp", UPDATE_STACK_SPACE(state)) \
    }


/* -------------------------------------------------------------------------- */
/*                              forward declarations                          */
/* -------------------------------------------------------------------------- */

static CORE_ADDR scan_prologue (CORE_ADDR          entrypoint,
                                struct frame_info *next_frame,
                                UnwindCache       *info);


/* -------------------------------------------------------------------------- */
/*                               local debug functions                        */
/* -------------------------------------------------------------------------- */

/* Print information for a frame.  */

static void
printFrameInfo (const char  *message,
                UnwindCache *info,
                Boolean      addresses_known)
{
    unsigned int i;

    DEBUG("-------------------\n");
    DEBUG("%s (info = %p)\n",     message, info);
    DEBUG("prev_sp               = %lx\n", (long unsigned int) info->prev_sp);
    DEBUG("frame_base            = %lx\n", (long unsigned int) info->frame_base);
    DEBUG("blink offset          = %d\n",                      info->blink_save_offset_from_prev_sp);
    DEBUG("delta_sp              = %d\n",  (int)               info->delta_sp);
    DEBUG("old_sp_offset_from_fp = %d\n",  (int)               info->old_sp_offset_from_fp);
    DEBUG("is_leaf = %d, uses_fp = %d\n", info->is_leaf, info->uses_fp);

     for (i = ARC_ABI_FIRST_CALLEE_SAVED_REGISTER; i < ARC_ABI_LAST_CALLEE_SAVED_REGISTER; i++)
     {
        if (info->saved_regs_mask & (1 << i))
            DEBUG("saved register R%02d %s 0x%lx\n",
                  i,
                  (addresses_known) ? "address" : "offset",
                  (unsigned long) info->saved_regs[i].addr);
    }
    DEBUG("-------------------\n");
}


static const char*
ARC_Debugger_OperandType_Image (enum ARC_Debugger_OperandType value)
{
    switch (value)
    {
        case ARC_LIMM          : return "LIMM";
        case ARC_SHIMM         : return "SHIMM";
        case ARC_REGISTER      : return "REGISTER";
        case ARCOMPACT_REGISTER: return "COMPACT REGISTER";
        case ARC_UNDEFINED     : return "UNDEFINED";
    }
    return "?";
}


/* Print the instruction state returned by the disassembler.
   Used for internal debugging only.  */

static void
printInsnState (struct arcDisState state)
{
    DEBUG("---------------------------------\n");
    DEBUG("Instruction Length %d\n",          state.instructionLen);
    DEBUG("Opcode [0x%x] : Cond [%x]\n",      state._opcode, state._cond);
    DEBUG("Words 1 [%lx] : 2 [%lx]\n",        state.words[0], state.words[1]);
    DEBUG("Ea present [%x] : memload [%x]\n", state._ea_present, state._mem_load);
    DEBUG("Load Length [%d]:\n",              state._load_len);
    DEBUG("Address Writeback [%d]\n",         state._addrWriteBack);
    DEBUG("EA reg1 is [%x] offset [%x]\n",    state.ea_reg1, state._offset);
    DEBUG("EA reg2 is [%x]\n",                state.ea_reg2);
    DEBUG("Instr   buffer is %s\n",           state.instrBuffer);
    DEBUG("Operand buffer is %s\n",           state.operandBuffer);
    DEBUG("SourceType is %s\n",               ARC_Debugger_OperandType_Image(state.sourceType));
    DEBUG("Source operand is %u\n",           state.source_operand.registerNum);   /* All fields of the union have same type.  */
    DEBUG("Flow is %d\n",                     state.flow);
    DEBUG("Branch is %d\n",                   state.isBranch);
    DEBUG("---------------------------------\n");
}


/* -------------------------------------------------------------------------- */
/*                     local functions for the disassembler                   */
/* -------------------------------------------------------------------------- */

/* Wrapper for the target_read_memory function.  */

static int
read_memory_for_disassembler (bfd_vma                  memaddr,
                              bfd_byte                *myaddr,
                              unsigned int             length,
                              struct disassemble_info *info)    // unused
{
    return target_read_memory((CORE_ADDR) memaddr, (gdb_byte*) myaddr, (int) length);
}


/* This is a callback function which gets called by gdb whenever the current
   object file changes.  */

static void
set_disassembler (struct objfile *objfile)
{
    if (objfile)
    {
       /* The ARC libopcodes wants obfd so that it can find out what CPU
          extensions are defined in the file.  */
        set_gdbarch_print_insn(current_gdbarch, arcompact_get_disassembler(objfile->obfd));
//      dump_ARC_extmap();
    }
}


/* This function is supplied to gdb as the disassembler until such time as we do
   have a disassembler available.  */

static int
dummy_disassembler (bfd_vma address, disassemble_info *info)
{
    error(_("No disassembly operation yet available (no executable file loaded)"));
    return 0;
}


/* -------------------------------------------------------------------------- */
/*               local functions for decoding call chains                     */
/* -------------------------------------------------------------------------- */

/* Simple utility function to create a new frame cache structure.  */

static UnwindCache*
create_cache (struct frame_info *next_frame)
{
    UnwindCache *cache = FRAME_OBSTACK_ZALLOC (UnwindCache);

    /* Zero all fields.  */
    cache->blink_save_offset_from_prev_sp = 0;
    cache->prev_sp                        = 0;
    cache->frame_base                     = 0;
    cache->delta_sp                       = 0;
    cache->old_sp_offset_from_fp          = 0;
    cache->is_leaf                        = FALSE;
    cache->uses_fp                        = FALSE;

    /* Allocate space for saved register info.  */
    cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

    return cache;
}


/* Compute the previous frame's stack pointer (which is also the frame's ID's
   stack address), and this frame's base pointer.  */

static void
find_previous_stack_pointer (UnwindCache       *info,
                             struct frame_info *next_frame)
{
    ENTERARGS("next_frame = %p", next_frame);

    /* If the frame has a frame pointer.  */
    if (info->uses_fp)
    {
        ULONGEST     this_base;
        unsigned int i;

        /* The SP was moved to the FP. This indicates that a new frame
           was created. Get THIS frame's FP value by unwinding it from
           the next frame. The old contents of FP were saved in the location
           at the base of this frame, so this also gives us the address of
           the FP save location.  */
	this_base = frame_unwind_register_unsigned(next_frame, ARC_FP_REGNUM);
        info->frame_base                     = (CORE_ADDR) this_base;
        info->saved_regs[ARC_FP_REGNUM].addr = (long long) this_base;

        /* The previous SP is the current frame base + the difference between
           that frame base and the previous SP.  */
        info->prev_sp = info->frame_base + (CORE_ADDR) info->old_sp_offset_from_fp;

        for (i = ARC_ABI_FIRST_CALLEE_SAVED_REGISTER; i < ARC_ABI_LAST_CALLEE_SAVED_REGISTER; i++)
        {
            /* If this register has been saved, add the previous stack pointer
               to the offset from the previous stack pointer at which the
               register was saved, so giving the address at which it was saved.  */
            if (info->saved_regs_mask & (1 << i))
            {
                info->saved_regs[i].addr += info->prev_sp;

#ifdef DUMP_SAVED_REGISTERS
                /* This is a really useful debugging aid: we can debug a test
                   program which loads known values into the callee-saved
                   registers, then calls another function which uses those
                   registers (and hence must save them) then hits a breakpoint;
                   traversing the stack chain (e.g. with the 'where' command)
                   should then execute this code, and we should see those known
                   values being dumped, so showing that we have got the right
                   addresses for the save locations!  */
                {
                    unsigned int contents;

                    DEBUG("saved R%02d is at 0x%lx\n", i, (long unsigned int) info->saved_regs[i].addr);

                    if (target_read_memory((CORE_ADDR) info->saved_regs[i].addr,
                                           (gdb_byte*) &contents,
                                           BYTES_IN_REGISTER) == 0)
                    {
                        DEBUG("saved R%02d contents: 0x%0x\n", i, contents);
                    }
                }
#endif
            }
        }
    }
    else
    {
        ULONGEST this_sp;

        /* Get the stack pointer for this frame by getting the saved SP
           from the next frame.  */
 	this_sp = frame_unwind_register_unsigned (next_frame, ARC_SP_REGNUM);

        /* The previous SP is this frame's SP plus the known difference between
           the previous SP and this frame's SP (the delta_sp is negated as it is
           a negative quantity).  */
        info->prev_sp = (CORE_ADDR) (this_sp + (ULONGEST) (-info->delta_sp));

        /* Assume that the FP is this frame's SP.  */
        info->frame_base = (CORE_ADDR) this_sp;
    }

    /* If the function owning this frame is not a leaf function.  */
    if (!info->is_leaf)
    {
        /* Usually blink is saved above the callee save registers and below the
           space created for variable arguments. The quantity

                    info->blink_save_offset_from_prev_sp

           is negative, so adding it to the the previous SP gives the address of
           a location further down the stack from that SP.  */
        info->saved_regs[ARC_BLINK_REGNUM].addr =
            (LONGEST) (info->prev_sp + info->blink_save_offset_from_prev_sp);
    }
}


/* The workhorse : frame_unwind_cache for the ARC700 target.  */

static UnwindCache *
frame_unwind_cache (struct frame_info *next_frame,
                    void             **this_prologue_cache)
{
    ENTERMSG;

    if ((*this_prologue_cache) == NULL)
    {
        CORE_ADDR    entrypoint = frame_func_unwind(next_frame, NORMAL_FRAME);
        UnwindCache *cache      = create_cache(next_frame);

        /* Return the newly-created cache.  */
        *this_prologue_cache = cache;

        /* Prologue analysis does the rest... */

        /* Currently our scan prologue does not support getting input for the
           frame unwinder.  */
        (void) scan_prologue(entrypoint, next_frame, cache);
    }

    return *this_prologue_cache;
}


/* -------------------------------------------------------------------------- */
/*               local functions for decoding function prologues              */
/* -------------------------------------------------------------------------- */

/* This function determines whether the given register, which is being saved
   by a function prologue on the stack at a known offset from the current SP,
   is a callee-saved register.  If it is, the information in the frame unwind
   cache is updated.  */

static Boolean
is_callee_saved_register (unsigned int  reg,
                          int           offset,
                          UnwindCache * info)
{
    if (ARC_ABI_FIRST_CALLEE_SAVED_REGISTER <= reg && reg <= ARC_ABI_LAST_CALLEE_SAVED_REGISTER)
    {
        DEBUG("register R%02u saved\n", reg);

        if (info)
        {
            /* We can not determine the address of the location in the stack
               frame in which the register was saved, as we do not (yet) know
               the frame or stack pointers for the frame; so the most we can do
               is to record the offset from the old SP of that location, which
               we can compute as we know the offset of SP from the old SP, and
               the offset of the location from SP (which is the offset in the
               store instruction).

               N.B. the stack grows downward, so the store offset is positive,
                    but the delta-SP is negative, so the save offset is also
                    negative.

                                             |            |
                              old sp ------> |------------|
                            /                |            |  \
                            :                |            |  :
                            :                |            |  :    -ve
                            :                |            |  : save offset
                            :                |------------|  :
                    -ve     :                |  save loc  |  /
                  delta sp  :                |------------| <--- store address
                            :            /   |            |
                            :     +ve    :   |            |
                            :    store   :   |            |
                            :    offset  :   |            |
                            \            \   |            |
                              sp' ---------> |            |
                                             |            |
                                             |            |
                                             |------------| <---- frame base
                                             |            |
                                             |            |
                                             |            |
                                             |     |      |
                                                   |
                                                   V
                                               downwards

               where sp' is the stack pointer at the current point in the code */

            info->saved_regs[reg].addr = info->delta_sp + offset;

            /* We now know that this register has been saved, so set the
               corresponding bit in the save mask.  */
            info->saved_regs_mask |= (1 << reg);

            printFrameInfo("after callee register save", info, FALSE);

            return TRUE;
        }
    }

    return FALSE;
}


/* This function determines whether the given disassembled instruction may be
   part of a function prologue.  If it is, the information in the frame unwind
   cache may be updated.  */

static Boolean
is_in_prologue (UnwindCache *info, struct arcDisState *instr)
{
    /* Might be a push or a pop */
    if (instr->_opcode == 0x3)
    {
        if (instr->_addrWriteBack != (char) 0)
        {
            /* This is a st.a instruction.  */
            if (instr->ea_reg1 == ARC_ABI_STACK_POINTER)
            {
                if (instr->_offset == -4)
                {
                    /* This is a push something at SP.  */
                    /* Is it a push of the blink?  */
                    IS_PUSH_BLINK_FI(instr);

                    /* Is it a push for fp?  */
                    IS_PUSH_FP_FI(instr);
                }
                else
                {
                    if (instr->sourceType == ARC_REGISTER )
                    {
                        /* st.a <reg>, [sp,<offset>] */

                        if (is_callee_saved_register(instr->source_operand.registerNum,
                                                     instr->_offset,
                                                     info))
                        {
                            /* This is a push onto the stack, so change delta_sp.  */
                            info->delta_sp += instr->_offset;
                            return TRUE;
                        }
                    }
                }
           }
        }
        else
        {
            if (instr->sourceType == ARC_REGISTER )
            {
                /* Is this a store of some register onto the stack using the
                   stack pointer?  */
                if (instr->ea_reg1 == ARC_ABI_STACK_POINTER)
                {
                    /* st <reg>, [sp,offset] */

                    if (is_callee_saved_register(instr->source_operand.registerNum,
                                                 instr->_offset,
                                                 info))
                        /* This is NOT a push onto the stack, so do not change delta_sp.  */
                        return TRUE;
                }

                /* Is this the store of some register on the stack using the
                   frame pointer? We check for argument registers getting saved
                   and restored.  */
                if (instr->ea_reg1 == ARC_ABI_FRAME_POINTER)
                {
                    if (IS_ARGUMENT_REGISTER(instr->source_operand.registerNum))
                    {
                        /* Saving argument registers. Don't set the bits in the
                           saved mask, just skip.  */
                        return TRUE;
                    }
                }
            }
        }
    }

    else if (instr->_opcode == 0x4)
    {
        /* A major opcode 0x4 instruction.  */
        /* We are usually interested in a mov or a sub.  */
        IS_UPDATE_FP_FI(instr);
        IS_SUB_SP_FI(instr);
    }

    else if (instr->_opcode == 0x18)
    {
        /* sub_s sp,sp,constant */
        IS_SUB_SP_FI(instr);

        /* push_s blink */
        if (strcmp(instr->instrBuffer, "push_s") == 0)
        {
            if (strcmp(instr->operandBuffer, "blink") == 0)
            {
                if (info)
                {
                    /* SP is decremented by the push_s instruction (before it
                       stores blink at the stack location addressed by SP).  */
                    PUSH_BLINK(-BYTES_IN_REGISTER)
                }
                return TRUE;
            }
        }
        else if (strcmp(instr->instrBuffer, "st_s") == 0)
        {
            unsigned int reg;
            int          offset;

            if (sscanf(instr->operandBuffer, "r%u,[sp,%d]", &reg, &offset) == 2)
            {
                /* st_s <reg>,[sp,<offset>] */

                if (is_callee_saved_register(reg, offset, info))
                    /* This is NOT a push onto the stack, so do not change delta_sp.  */
                    return TRUE;
            }
        }
    }

    return FALSE;
}


/* Scan the prologue and update the corresponding frame cache for the frame
   unwinder for unwinding frames without debug info. In such a situation GDB
   attempts to parse the prologue for this purpose. This currently would attempt
   to parse the prologue generated by our gcc 2.95 compiler (we should support
   Metaware generated binaries at some suitable point of time).

   This function is called with:
      entrypoint : the address of the functon entry point
      next_frame : the next frame to be filled in (if need be)
      info       : the existing cached info.

   Returns: the address of the first instruction after the prologue.

   This function is called by our unwinder as well as from arc_skip_prologue
   in the case that it cannot detect the end of the prologue.

   'next_frame' and 'info' are NULL if this function is called from
   arc_skip_prologue in an attempt to discover the end of the prologue.
   In this case we don't fill in the 'info' structure that is passed in.

   TODOs:
      1. Support 32 bit normal frames generated by GCC 2.95
      2. Support 16 and 32 bit mixed frames generated by GCC 2.95
      3. Support 32 bit normal variadic function frames by GCC 2.95
      4. Support 32 bit normal frames from GCC 3.4.x with variadic args
      5. Support 16 and 32 bit normal frames from GCC 3.4.x with variadic args
      6. Support 16 and 32 bit mixed frames generated by GCC 3.4.x
      7. Support Metaware generated prologues
           (The difference is in the use of thunks to identify the saving and
            restoring of callee saves: may have to do some hackery even in
            next_pc, since the call is going to create its own set of problems
            with our stack setup).

   We attempt to use the disassembler interface from the opcodes library to do
   our disassembling.

   The usual 32 bit normal gcc -O0 prologue looks like this:

   Complete Prologue for all GCC frames (Cases #1 to #6 in TODOs above):

      sub  sp, sp, limm         ; space for variadic arguments
      st.a blink, [sp,-4]       ; push blink (if not a leaf function) - decrements sp
      sub  sp, sp , limm        ; (optional space creation for callee saves)
      st   r13, [sp]            ; push of first callee saved register
      st   r14, [sp,4]          ; push of next callee saved register
      ...
      st.a fp , [sp,-4]         ; push fp (if fp has to be saved) - decrements sp
      mov  fp , sp              ; set the current frame up correctly
      sub  sp , sp , #immediate ; create space for local vars on the stack   */

/* 3 instructions before and after callee saves, and max number of saves; assume each is 4-byte inst.  */
#define MAX_PROLOGUE_LENGTH   ((6 + (ARC_ABI_LAST_CALLEE_SAVED_REGISTER - ARC_ABI_FIRST_CALLEE_SAVED_REGISTER + 1)) * 4)

static CORE_ADDR
scan_prologue (CORE_ADDR          entrypoint,
               struct frame_info *next_frame,
               UnwindCache       *info)
{
    ENTERARGS("next_frame = %p, info = %p", next_frame, info);

    {
        /* Will be set to end of prologue.  */
        CORE_ADDR               prologue_ends_pc = entrypoint;
        struct disassemble_info di;

        /* An arbitrary limit on the length of the prologue. If next_frame is
           NULL this means that there was no debug info and we are called from
           arc_skip_prologue; otherwise, if we know the frame, we can find the
           pc within the function.

           N.B. that pc will usually be after the end of the prologue, but
                it could actually be within the prologue (i.e. execution has
                halted within the prologue, e.g. at a breakpoint); in that
                case, do NOT go beyond that pc, as the instructions at the
                pc and after have not been executed yet, so have had no effect!  */
        CORE_ADDR final_pc = (next_frame) ? frame_pc_unwind(next_frame)
                                          : entrypoint + MAX_PROLOGUE_LENGTH;

        if (info)
        {
            /* Assume that the function is a leaf function until we find out
               that it is not (i.e. when we find the 'push blink' instruction
               in the prologue).  */
            info->is_leaf = TRUE;

            /* No registers known to be saved, as yet.  */
            info->saved_regs_mask = 0;
        }

        /* Initializations to use the opcodes library.  */
        arc_initialize_disassembler(&di);

        DEBUG("Prologue PC: %lx\n", (unsigned long) prologue_ends_pc);
        DEBUG("Final    PC: %lx\n", (unsigned long) final_pc);

        /* Look at each instruction in the prologue.  */
        while (prologue_ends_pc < final_pc)
        {
            struct arcDisState current_instr = arcAnalyzeInstr(prologue_ends_pc, &di);

            printInsnState(current_instr);

            /* If this instruction is in the prologue, fields in the info will be updated,
               and the saved registers mask may be updated.  */
            if (!is_in_prologue(info, &current_instr))
            {
                /* Found a instruction that is not in the prologue.  */
                DEBUG("End of Prologue reached \n");
                break;
            }

            prologue_ends_pc += current_instr.instructionLen;
        }

        /* Means we were not called from arc_skip_prologue.  */
        if (!((next_frame == NULL) && (info == NULL)))
        {
            printFrameInfo("after prologue", info, FALSE);

            find_previous_stack_pointer(info, next_frame);

            /* The PC is found in blink (the actual register or located on the stack).  */
            info->saved_regs[ARC_PC_REGNUM] = info->saved_regs[ARC_BLINK_REGNUM];

            printFrameInfo("after previous SP found", info, TRUE);
        }

        return prologue_ends_pc;
    }
}


/* -------------------------------------------------------------------------- */
/*              local functions for handling function return values           */
/* -------------------------------------------------------------------------- */

/* This function gets the return value of a function from the registers used to
   return it, according to the convention used by the ABI.

   Parameters:
      type     : the information for the return type of the function
      regcache : the register cache holding the register contents
      valbuf   : a buffer to be filled with the return value
*/

static void
extract_return_value (struct type     *type,
                      struct regcache *regcache,
                      gdb_byte        *valbuf)

{
    unsigned int len = TYPE_LENGTH (type);

    ENTERMSG;

    if (len <= BYTES_IN_REGISTER)
    {
        ULONGEST val;

        /* Get the return value from one register.  */
        regcache_cooked_read_unsigned (regcache, ARC_ABI_RETURN_REGNUM, &val);
        store_unsigned_integer (valbuf, (int) len, val);

        DEBUG("returning 0x%08lX\n", (unsigned long) val);
    }
    else if (len <= BYTES_IN_REGISTER * 2)
    {
        ULONGEST low, high;

        /* Get the return value from two registers.  */
        regcache_cooked_read_unsigned (regcache, ARC_ABI_RETURN_LOW_REGNUM,  &low);
        regcache_cooked_read_unsigned (regcache, ARC_ABI_RETURN_HIGH_REGNUM, &high);

        store_unsigned_integer (valbuf,                     BYTES_IN_REGISTER,             low);
        store_unsigned_integer (valbuf + BYTES_IN_REGISTER, (int) len - BYTES_IN_REGISTER, high);

        DEBUG("returning 0x%08lX%08lX\n",
              (unsigned long) high, (unsigned long) low);
    }
    else
        error(_("%s: type length %u too large"), __FUNCTION__, len);
}


/* This function loads the return value of a function into the registers used to
   return it, according to the convention used by the ABI.

   Parameters:
      type     : the information for the return type of the function
      regcache : the register cache holding the register contents
      valbuf   : a buffer holding the return value
*/

static void
store_return_value (struct type     *type,
                    struct regcache *regcache,
                    const  gdb_byte *valbuf)
{
    unsigned int len = TYPE_LENGTH (type);

    ENTERMSG;

    if (len <= BYTES_IN_REGISTER)
    {
        ULONGEST val;

        /* Put the return value into one register.  */
        val = extract_unsigned_integer (valbuf, (int) len);
        regcache_cooked_write_unsigned (regcache, ARC_ABI_RETURN_REGNUM, val);

        DEBUG("storing 0x%08lX\n", (unsigned long) val);
    }
    else if (len <= BYTES_IN_REGISTER * 2)
    {
        ULONGEST low, high;

        /* Put the return value into  two registers.  */
        low  = extract_unsigned_integer (valbuf,                     BYTES_IN_REGISTER);
        high = extract_unsigned_integer (valbuf + BYTES_IN_REGISTER, (int) len - BYTES_IN_REGISTER);

        regcache_cooked_write_unsigned (regcache, ARC_ABI_RETURN_LOW_REGNUM,  low);
        regcache_cooked_write_unsigned (regcache, ARC_ABI_RETURN_HIGH_REGNUM, high);

        DEBUG("storing 0x%08lX%08lX\n",
              (unsigned long) high, (unsigned long) low);
    }
    else
        error(_("arc_store_return_value: type length too large."));
}


/* -------------------------------------------------------------------------- */
/*                 local functions for handling the stack frame               */
/* -------------------------------------------------------------------------- */

/* This is copied from file stack.c in the gdb sources.
   It identifies a frame from information (such as frame number) given by the
   user (in the frame_exp parameter).  */

static struct frame_info*
parse_frame_specification_1 (const char *frame_exp,
                             const char *message,
                             int        *selected_frame_p)
{
    int           numargs = 0;
    struct value *args[4];
    CORE_ADDR     addrs[ARRAY_SIZE (args)];

    if (frame_exp)
    {
        while (TRUE)
        {
            char           *addr_string;
            struct cleanup *cleanup;
            const char     *p;

            /* Skip leading white space.  */
            while (isspace (*frame_exp))
                frame_exp++;

            if (*frame_exp == '\0')
                break;

            /* Parse the argument, extract it, save it.  */
            for (p = frame_exp; (*p != '\0') && !isspace (*p); p++);

            addr_string = savestring (frame_exp, (size_t) (p - frame_exp));
            frame_exp   = p;
            cleanup     = make_cleanup (xfree, addr_string);

            /* NOTE: Parse and evaluate expression, but do not use
               functions such as parse_and_eval_long or
               parse_and_eval_address to also extract the value.
               Instead value_as_long and value_as_address are used.
               This avoids problems with expressions that contain
               side-effects.  */
            if (numargs >= (int) ARRAY_SIZE (args))
                error (_("Too many args in frame specification"));

            args[numargs++] = parse_and_eval (addr_string);

            do_cleanups (cleanup);
        }
    }

    /* If no args, default to the selected frame.  */
    if (numargs == 0)
    {
        if (selected_frame_p != NULL)
            (*selected_frame_p) = 1;
        return get_selected_frame (message);
    }

    /* None of the remaining use the selected frame.  */
    if (selected_frame_p != NULL)
        (*selected_frame_p) = 0;

    /* Assume the single arg[0] is an integer, and try using that to
       select a frame relative to current.  */
    if (numargs == 1)
    {
        int                level = (int) value_as_long (args[0]);
        struct frame_info *fid   = find_relative_frame (get_current_frame (), &level);

        if (level == 0)
            /* find_relative_frame was successful.  */
            return fid;
    }

    /* Convert each value into a corresponding address.  */
    {
        int i;
        for (i = 0; i < numargs; i++)
            addrs[i] = value_as_address (args[i]);
    }

    /* Assume that the single arg[0] is an address, use that to identify
       a frame with a matching ID.  Should this also accept stack/pc or
       stack/pc/special.  */
    if (numargs == 1)
    {
        struct frame_id id = frame_id_build_wild (addrs[0]);
        struct frame_info *fid;

        /* If (s)he specifies the frame with an address, he deserves
           what (s)he gets.  Still, give the highest one that matches.
           (NOTE: cagney/2004-10-29: Why highest, or outer-most, I don't
           know).  */
        for (fid = get_current_frame ();
             fid != NULL;
             fid = get_prev_frame (fid))
        {
            if (frame_id_eq (id, get_frame_id (fid)))
            {
                while (frame_id_eq (id, frame_unwind_id (fid)))
                    fid = get_prev_frame (fid);
                return fid;
            }
        }
    }

    /* We couldn't identify the frame as an existing frame, but
       perhaps we can create one with a single argument.  */
    if (numargs == 1)
        return create_new_frame (addrs[0], 0);
    else if (numargs == 2)
        return create_new_frame (addrs[0], addrs[1]);
    else
        error (_("Too many args in frame specification"));
}


/* Return an array (and count) of the local variables and parameters declared
   in the function which owns the given frame.

   Parameters:
      frame    : the frame information for the function
      variables: a pointer to an existing array of variable information (may be NULL)
      count    : (IN/OUT) the total number of variables found
      callee   : TRUE if the function is a callee of another function

   Result:
      A pointer to an array containing one element for each parameter, and, if
      'callee' is FALSE, each local variable, of the function.  */

static LocalVariable*
find_variables (struct frame_info *frame,
                LocalVariable     *variables,
                unsigned int      *count,
                Boolean            callee)
{
    struct block *block = get_frame_block (frame, 0);
    unsigned int  vars  = *count;

    while (block)
    {
        struct dict_iterator iter;
        struct symbol       *sym;

        ALL_BLOCK_SYMBOLS (block, iter, sym)
        {
            Boolean   is_arg = (SYMBOL_CLASS(sym) == LOC_COMPUTED_ARG);
            CORE_ADDR addr;

            switch (SYMBOL_CLASS (sym))
            {
                case LOC_COMPUTED:
                case LOC_COMPUTED_ARG:
                    addr = SYMBOL_OPS (sym)->get_variable_address(sym, frame);

                    /* For callees, we are only interested in the arguments.  */
                    if ((addr != 0) && (!callee || is_arg))
                    {
                        struct type *type = SYMBOL_TYPE (sym);

                        vars++;
                        variables = xrealloc(variables, sizeof(LocalVariable) * vars);

                        if (variables)
                        {
                            LocalVariable *var = &variables[vars - 1];

                            var->name        = SYMBOL_PRINT_NAME (sym);
                            var->address     = addr;
                            var->is_callee   = callee;
                            var->is_argument = is_arg;
                            var->is_array    = (TYPE_CODE (type) == TYPE_CODE_ARRAY);
                            var->size        = SYMBOL_OPS (sym)->get_variable_size(sym, frame);

                            if (var->is_array)
                                var->element_size = TYPE_LENGTH (TYPE_TARGET_TYPE (type));
                        }
                        else
                        {
                            *count = 0;
                            return NULL;
                        }
                    }
                    break;

                default:
                  /* Ignore symbols which are not locals.  */
                  break;
            }
        }

        /* After handling the function's top-level block, stop.  Don't continue
           to its superblock, the block of per-file symbols.  */
        if (BLOCK_FUNCTION (block))
            break;
        block = BLOCK_SUPERBLOCK (block);
    }

    *count = vars;
    return variables;
}


/* Return an array (and count) of the local variables declared in the function
   which owns the given frame, and also those declared in the function which is
   the callee (if any) of that function in the current call chain.  */

static LocalVariable*
find_local_variables (struct frame_info *frame, unsigned int *count)
{
    struct frame_info *callee = get_next_frame(frame);
    LocalVariable     *variables;

    *count = 0;

    variables = find_variables(frame, NULL, count, FALSE);

    if (callee)
        variables = find_variables(callee, variables, count, TRUE);

    return variables;
}


/* Try to add the name of a local variable or function parameter to a line of
   output, if a given address lies within the range of locations occupied by
   that data item.

   Parameters:
      line                   : the line in which any output is to be placed
      location               : the address of a location on the stack
      variables              : an array of local variables and parameters
      num_variables          : the number of elements in the array
      is_following_element   : set to TRUE if the location lies within an
                               array element, and it is not the 0th element
      elements_are_word_sized: set to TRUE if the array element is word-sized

   Returns TRUE if the given location holds a local variable or parameter
   (i.e. information has been added to the line of output).  */

static Boolean
add_local_name (char          *line,
                CORE_ADDR      location,
                LocalVariable *variables,
                unsigned int   num_variables,
                Boolean       *is_following_element,
                Boolean       *elements_are_word_sized)
{
    unsigned int i;

    *is_following_element    = FALSE;
    *elements_are_word_sized = FALSE;

    /* Look at each of the local variables / parameters in the array.  */
    for (i = 0; i < num_variables; i++)
    {
        LocalVariable *var   = &variables[i];
        int            index = -1;

        /* is the variable an array?  */
        if (var->is_array)
        {
            /* If we know the size of the array, and the size of its elements. */
            if (var->size > 0 && var->element_size > 0)
            {
                /* What is the offset of the given stack location from the start
                   of the array?  */
                int offset = (int) ((long int) location - (long int) var->address);

                /* Does that offset actually lie within the array?  */
                if (0 <= offset && offset < (int) var->size)
                {
                    /* Compute the index of the array element which contains the
                       location.  */
                    index = offset / var->element_size;
                }

                if (var->element_size == BYTES_IN_WORD)
                    *elements_are_word_sized = FALSE;
            }
        }

        /* If the variable starts in the given location, or the variable is an
           array and one of its elements contains the location. */
        if (var->address == location || index >= 0)
        {
            int n;

            /* What is the variable?  */
            if (var->is_callee)
                n = sprintf(line, _("callee parameter"));
            else if (var->is_argument)
                n = sprintf(line, _("parameter"));
            else
                n = sprintf(line, _("local variable"));

            line[n] = ' ';
            n++;
            n += sprintf(line + n, _("'%s'"),  var->name);
            line[n] = ' ';

            /* If it is an array element.  */
            if (index >= 0)
            {
                /* Add the array element index to the output.  */
                (void) sprintf(line + n, _("[%u]"), index);

                /* Is it an element which follows another element?  */
                *is_following_element = (index > 0);
            }
            else if (var->size > BYTES_IN_WORD)
            {
                /* It occupies more than one word.  */
                (void) sprintf(line + n + 1, _("(%u words)"),
                               WORDS_OCCUPIED(var->size));
            }

            return TRUE;
        }
     }

    return FALSE;
}


/* Try to identify the given frame, and output that identification.  */

static void
identify_frame (struct frame_info *frame)
{
    enum language          func_lang = language_unknown;
    char                  *func_name = NULL;
    char                  *demangled = NULL;
    struct symbol         *func;
    struct symtab_and_line sal;

    find_frame_sal (frame, &sal);
    func = get_frame_function (frame);

    /* Have we found the function owning the frame?  */
    if (func)
    {
        func_name = DEPRECATED_SYMBOL_NAME (func);
        func_lang = SYMBOL_LANGUAGE        (func);
    }
    else
    {
        struct minimal_symbol *msymbol;

        /* Try to find the symbol most closely associated with the PC
           corresponding to the frame.  */
        msymbol = lookup_minimal_symbol_by_pc (get_frame_pc (frame));

        if (msymbol != NULL)
        {
            func_name = DEPRECATED_SYMBOL_NAME (msymbol);
            func_lang = SYMBOL_LANGUAGE        (msymbol);
        }
    }

    /* Have we found a name?  */
    if (func_name)
    {
        /* If user wants to see raw output, no problem.
           (demangle is a global flag which can be set by user command).  */
        if (demangle)
        {
            demangled = language_demangle (language_def (func_lang),
                                           func_name,
                                           DMGL_ANSI | DMGL_PARAMS);

            /* If the demangler fails, try the demangled name from the symbol
               table. That'll have parameters, but that's preferable to
               displaying a mangled name.  */
            if (demangled == NULL)
            {
                if (func == NULL)
                    func_name = _("<unknown function>");
                else
                    func_name = SYMBOL_PRINT_NAME (func);
            }
            else
                func_name = demangled;
        }
    }
    else
        func_name = _("<unknown function>");

    printf_filtered(_("Frame of function: %s"), func_name);
    if (sal.symtab)
        printf_filtered(_(" (%s:%d)"), sal.symtab->filename, sal.line);
    printf_filtered(NEW_LINE);

    if (demangled != NULL)
        xfree (demangled);
}


/* -------------------------------------------------------------------------- */
/*                       local functions called from gdb                      */
/* -------------------------------------------------------------------------- */

/*  Standard register type for the ARC platform.
    It would be builtin_type_uint32 until we consider the DSP extensions.  */

static struct type *
arc_register_type (struct gdbarch *gdbarch, int regnum)
{
    return builtin_type_uint32;
}


/* Skip the prologue for the function at pc.
   Returns the address of the first instruction after the prologue. */

static CORE_ADDR
arc_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
    CORE_ADDR function_addr, function_end = 0;
    char     *function_name;

   /* This is done by checking from the line information read from the DWARF,
      if possible; otherwise, we scan the function prologue to find its end.  */

    ENTERMSG;

    /* If we're in a dummy frame, don't even try to skip the prologue.  */
    if (deprecated_pc_in_call_dummy (pc))
        return pc;

    /* See what the symbol table says.  */
    if (find_pc_partial_function (pc, &function_name, &function_addr, &function_end))
    {
        /* Found a function.  */
        struct symbol *sym = lookup_symbol (function_name, NULL, VAR_DOMAIN, NULL, NULL);

        if ((sym != NULL) && SYMBOL_LANGUAGE (sym) != language_asm)
        {
            /* Don't use this trick for assembly source files.  */
            struct symtab_and_line sal = find_pc_line (function_addr, 0);

            if ((sal.line != 0) && (sal.end < function_end))
                return sal.end;
        }
    }

    /* Find the address of the first instruction after the prologue by scanning
       through it - no other information is needed, so pass NULL for the other
       parameters.  */
    return scan_prologue(pc, NULL, NULL);
}


/* Construct frame id for the normal frame.  */

static void
arc_frame_this_id (struct frame_info *next_frame,
                   void             **this_prologue_cache,
                   struct frame_id   *this_id)
{
    ENTERMSG;

    /* FIXME: to what should *this_id be set if the frame base can not be found? */

    {
        /* Find the entry point of the function which owns the frame.  */
        CORE_ADDR entrypoint = frame_func_unwind (next_frame, NORMAL_FRAME);

        /* This is meant to halt the backtrace at the entry point (_start)
           (it assumes that there is no code at a lower address).  */
        if (entrypoint > gdbarch_tdep (current_gdbarch)->lowest_pc)
        {
            UnwindCache *info = frame_unwind_cache (next_frame, this_prologue_cache);
            CORE_ADDR    base = info->prev_sp;

            /* Hopefully the prologue analysis either correctly determined the
               frame's base (which is the SP from the previous frame), or set
               that base to "NULL".  */

            if (base != 0)
            {
                /* Build the ID from the frame base address.  */
                *this_id = frame_id_build (base, entrypoint);

#if 0
                printf("*** Frame ID: %x ==> (%x %x %x)\n",
                       base,
                       this_id->stack_addr,
                       this_id->code_addr,
                       this_id->special_addr);
#endif
            }
        }
    }
}


/* Unwind and obtain the register information.  */

static void
arc_frame_prev_register (struct frame_info *next_frame,
                         void             **this_prologue_cache,
                         int                regnum,
                         int               *optimized,
                         enum lval_type    *lval,
                         CORE_ADDR         *addr,
                         int               *realnum,
                         gdb_byte          *buffer)
{
    ENTERARGS("regnum %d ", regnum);

    {
        UnwindCache *info = frame_unwind_cache (next_frame, this_prologue_cache);

        /* If we are asked to unwind the PC, then we need to return blink
           instead: the saved value of PC points into this frame's function's prologue,
           not the next frame's function's resume location.  */
        if (regnum == ARC_PC_REGNUM)
            regnum = ARC_BLINK_REGNUM;

        /* SP is generally not saved to the stack, but this frame is identified
           by next_frame's stack pointer at the time of the call. The value was
           already reconstructed into prev_sp.  */
        if (regnum == ARC_SP_REGNUM)
        {
            /* This value is not an L-value, i.e. it can not be changed, because
               it is implicit in the structure of the call-chain.  */
            *lval = not_lval;

            if (buffer)
                store_unsigned_integer (buffer, BYTES_IN_REGISTER, info->prev_sp);
            return;
        }

        trad_frame_get_prev_register (next_frame,
                                      info->saved_regs,
                                      regnum,
                                      optimized,
                                      lval,
                                      addr,
                                      realnum,
                                      buffer);

        DEBUG("-*-*-*\n Regnum = %d, buffer = %p\n", regnum, buffer);
    }
}


/* This function is passed to gdb to enable it to unwind frames.  */

static const struct frame_unwind *
arc_frame_sniffer (struct frame_info *next_frame)
{
    static const struct frame_unwind arc_frame_unwind =
    {
        NORMAL_FRAME,                // type
        arc_frame_this_id,           // this_id
        arc_frame_prev_register,     // prev_register
        NULL,                        // unwind_data
        NULL,                        // sniffer
        NULL,                        // prev_pc
        NULL                         // dealloc_cache
    };

    return &arc_frame_unwind;
}


/* Get the breakpoint which is appropriate for address at which it is to be set.

   Return whatever is in the ARC-private tdep structure (this has been set up
   according to the correct target / architecture chosen).

   Fortunately, the ARC processor does not have separate instruction sets (like
   the ARM's normal 32-bit and 16-bit Thumb instructions), so the bp instruction
   to be used does not depend on the address (although the ARC does have both
   16- and 32-bit instructions, they may be freely intermixed).  */

static const unsigned char *
arc_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pcptr, int *lenptr)
{
    struct gdbarch_tdep *tdep = gdbarch_tdep       (gdbarch);
    int    end                = gdbarch_byte_order (gdbarch);

    /* Return the breakpoint instruction length.  */
    *lenptr = (int) tdep->breakpoint_size;

    /* Return the breakpoint instruction code.  */
    if (end == BFD_ENDIAN_LITTLE)
        return tdep->le_breakpoint_instruction;
    if (end == BFD_ENDIAN_BIG)
        return tdep->be_breakpoint_instruction;

    internal_error (__FILE__, __LINE__, "target endianness is not known");
}


/* Determine whether the given register is a member of the given group.

   Returns 0, 1, or -1:
      0 means the register is not in the group.
      1 means the register is in the group.
     -1 means the tdep has nothing to say about this register and group.  */

static int
arc_register_reggroup_p (struct gdbarch  *gdbarch,
                         int              regnum,
                         struct reggroup *group)
{
    struct gdbarch_tdep *tdep        = gdbarch_tdep (gdbarch);
    int                  tdep_answer = tdep->register_reggroup_p (regnum, group);

    /* If the configuration-specific tdep knows about this register.  */
    if (tdep_answer != -1)
        return tdep_answer;

    if (group == all_reggroup)
        return 1;

    if (group == save_reggroup || group == restore_reggroup)
    {
        /* Don't save/restore read-only registers.  */
	return (!gdbarch_cannot_store_register(current_gdbarch, regnum));
    }

    if (group == system_reggroup)
    {
        if (regnum == ARC_ILINK1_REGNUM ||
            regnum == ARC_ILINK2_REGNUM)
            return 1;

        return 0;
    }

    internal_error(__FILE__, __LINE__, _("bad register group"));
    return 0;
}


/* This function is used by the DWARF-2 frame sniffer.
   It is the architecture-specific register state initialization function:
   it tells gdb how to find certain registers in the DWARF-2 Call Frame Information.  */

static void
arc_dwarf2_frame_init_reg (struct gdbarch                *gdbarch,
                           int                            regnum,
                           struct dwarf2_frame_state_reg *reg,
                           struct frame_info             *info)
{
//  ENTERARGS("Regno no:%d, 0x%x", regnum, (unsigned int) regnum);

    /* Make sure that we know the number of the PC!  */
    arc_check_pc_defined(gdbarch);

    /* The return address column.  */
    if (regnum == ARC_PC_REGNUM)
        reg->how = DWARF2_FRAME_REG_RA;

    /* The call frame address.  */
    if (regnum == ARC_SP_REGNUM)
        reg->how = DWARF2_FRAME_REG_CFA;
}


/* Unwind the frame to find the previous frame's PC.  */

static CORE_ADDR
arc_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
    ULONGEST pc;

    /* Make sure that we know the number of the PC!  */
    arc_check_pc_defined(gdbarch);

    pc = frame_unwind_register_unsigned (next_frame, ARC_PC_REGNUM);

    DEBUG("unwind PC: 0x%08lx\n", (unsigned long) pc);

    return (CORE_ADDR) pc;
}


/* Unwind the frame to find the previous frame's SP.  */

static CORE_ADDR
arc_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
    ULONGEST sp;

    sp  = frame_unwind_register_unsigned (next_frame, ARC_SP_REGNUM);

    DEBUG("unwind SP: 0x%08lx\n", (unsigned long) sp);

    return (CORE_ADDR) sp;
}


/* This function returns the convention used by the ABI for returning a result
   of the given type from a function; it may also be required to:

     a) set the return value (this is for the situation where the debugger user
        has issued a "return <value>" command to request immediate return from
        the current function with the given result; or

     b) get the return value ((this is for the situation where the debugger user
        has executed a "call <function>" command to execute the specified
        function in the target program, and that function has a non-void result
        which must be returned to the user.

  Parameters:
      gdbarch : gdbarch structure for the backend to use if needed
      valtype : the information for the return type of the function
      regcache: the register cache to be used for altered register values
      readbuf : if non-NULL, read the return value into this buffer
      writebuf: if non-NULL, write the return value from this buffer
 */

static enum return_value_convention
arc_return_value (struct gdbarch  *gdbarch,
                  struct type     *valtype,
                  struct regcache *regcache,
                  gdb_byte        *readbuf,
                  const gdb_byte  *writebuf)
{
    ENTERARGS("readbuf = %p, writebuf = %p", readbuf, writebuf);

    {
        /* If the return type is a struct, or a union, or would occupy more
           than two registers, the ABI uses the "struct return convention": the
           calling function passes a hidden first parameter to the callee (in R0).
           That parameter is the address at which the value being returned
           should be stored.  Otherwise, the result is returned in registers.  */
        Boolean is_struct_return = (TYPE_CODE   (valtype) == TYPE_CODE_STRUCT ||
                                    TYPE_CODE   (valtype) == TYPE_CODE_UNION  ||
                                    TYPE_LENGTH (valtype) > 2 * BYTES_IN_REGISTER);

        /* case a) */
        if (writebuf != NULL)
        {
            /* gdb should not ask us to set a struct return value: it should
               know the struct return location and write the value there
               itself.  */
            gdb_assert (!is_struct_return);
            store_return_value (valtype, regcache, writebuf);
        }

        /* case b) */
        if (readbuf != NULL)
        {
            /* gdb should not ask us to get a struct return value: it should
               know the struct return location and read the value from there
               itself.  */
            gdb_assert (!is_struct_return);
            extract_return_value (valtype, regcache, readbuf);
        }

        return (is_struct_return) ? RETURN_VALUE_STRUCT_CONVENTION
                                  : RETURN_VALUE_REGISTER_CONVENTION;
    }
}


/* Assuming that next_frame->prev is a dummy, return the frame ID of that dummy
   frame.  The frame ID's base needs to match the TOS value saved by
   save_dummy_frame_tos() (!!!! WHAT IS THIS???), and the PC to match the dummy
   frame's breakpoint.  */

static struct frame_id
arc_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
    return frame_id_build (arc_unwind_sp (gdbarch, next_frame),
                           frame_pc_unwind (next_frame));
}


/* Signal Trampoline Frame Unwinder.
   These unwinders allow frame unwinding to happen from within signal handlers.  */

static UnwindCache *
arc_sigtramp_frame_cache (struct frame_info *next_frame,
                          void             **this_cache)
{
    ENTERMSG;

    if (*this_cache == NULL)
    {
        struct gdbarch_tdep *tdep  = gdbarch_tdep (current_gdbarch);
        UnwindCache         *cache = create_cache(next_frame);
        gdb_byte             buf[BYTES_IN_REGISTER];

        *this_cache = cache;

        /* Get the stack pointer and use it as the frame base.  */
        frame_unwind_register (next_frame, ARC_SP_REGNUM, buf);
        cache->frame_base = (CORE_ADDR) extract_unsigned_integer (buf, BYTES_IN_REGISTER);

        /* If the ARC-private target-dependent info has a table of offsets of
           saved register contents within a O/S signal context structure.  */
        if (tdep->sc_reg_offset)
        {
            /* Find the address of the sigcontext structure.  */
            CORE_ADDR    addr = tdep->sigcontext_addr (next_frame);
            unsigned int i;

            /* For each register, if its contents have been saved within the
               sigcontext structure, determine the address of those contents.  */
            for (i = 0; i < tdep->sc_num_regs; i++)
                if (tdep->sc_reg_offset[i] != REGISTER_NOT_PRESENT)
                    cache->saved_regs[i].addr = (LONGEST) (addr + tdep->sc_reg_offset[i]);
        }
    }

    return *this_cache;
}


/* Construct id for the given frame.  */

static void
arc_sigtramp_frame_this_id (struct frame_info *next_frame,
                            void             **this_cache,
                            struct frame_id   *this_id)
{
    UnwindCache *cache;

    ENTERMSG;

    cache = arc_sigtramp_frame_cache (next_frame, this_cache);

    *this_id = frame_id_build (cache->frame_base, frame_pc_unwind (next_frame));
}


/* Retrieve the value of the register in the frame.  */

static void
arc_sigtramp_frame_prev_register (struct frame_info *next_frame,
                                  void             **this_cache,
                                  int                regnum,
                                  int               *optimizedp,
                                  enum lval_type    *lvalp,
                                  CORE_ADDR         *addrp,
                                  int               *realnump,
                                  gdb_byte          *valuep)
{
    ENTERMSG;

    {
        struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

        /* Make sure we've initialized the cache.  */
        UnwindCache *cache = arc_sigtramp_frame_cache (next_frame, this_cache);

        /* On a signal, the PC is in ret.  */
        if (regnum == ARC_PC_REGNUM)
            regnum = tdep->pc_regnum_in_sigcontext;

        trad_frame_get_prev_register (next_frame,
                                      cache->saved_regs,
                                      regnum,
                                      optimizedp,
                                      lvalp,
                                      addrp,
                                      realnump,
                                      valuep);
    }
}


static const struct frame_unwind *
arc_sigtramp_frame_sniffer (struct frame_info *next_frame)
{
    ENTERMSG;

    {
        struct gdbarch_tdep *tdep = gdbarch_tdep (get_frame_arch (next_frame));

        /* We don't even bother if we don't have a sigcontext_addr handler.  */
        if ((tdep->sigcontext_addr != NULL) &&
            (tdep->is_sigtramp     != NULL) &&
            tdep->is_sigtramp (next_frame))
        {
            static const struct frame_unwind arc_sigtramp_frame_unwind =
            {
                SIGTRAMP_FRAME,                   // type
                arc_sigtramp_frame_this_id,       // this_id
                arc_sigtramp_frame_prev_register, // prev_register
                NULL,                             // unwind_data
                NULL,                             // sniffer
                NULL,                             // prev_pc
                NULL                              // dealloc_cache
            };

            return &arc_sigtramp_frame_unwind;
        }

        return NULL;
    }
}


/* Allow calls to be made to functions defined in the debuggee (A.K.A dummy calls).

   Parameters:
      gdbarch      : gdbarch structure for the backend to use if needed.
      function     : the function to be called
      regcache     : the register cache to be used for altered register values
      bp_addr      : return address for the breakpoint.
      nargs        : the number of arguments to the function
      args         : the arguments to be passed to the function
      sp           : current value of SP.
      struct_return: 1 if structures are returned by the function.
      struct_addr  : hidden address for returning a struct.  */

static CORE_ADDR
arc_push_dummy_call (struct gdbarch  *gdbarch,
                     struct value    *function,
                     struct regcache *regcache,
                     CORE_ADDR        bp_addr,
                     int              nargs,
                     struct value   **args,
                     CORE_ADDR        sp,
                     int              struct_return,
                     CORE_ADDR        struct_addr)

{
    int arg_reg = ARC_ABI_FIRST_ARGUMENT_REGISTER;

    ENTERARGS("nargs = %d, struct_return = %d", nargs, struct_return);

    /* Push the return address.  */
    regcache_cooked_write_unsigned (regcache, ARC_BLINK_REGNUM, bp_addr);

    DEBUG("set BLINK = %X\n", (unsigned int) bp_addr);

    /* Are we returning a value using a structure return instead of a normal
       value return? If so, struct_addr is the address of the reserved space
       for the return structure to be written on the stack, and that address
       is passed to that function as a hidden first argument.  */

    if (struct_return)
    {
        /* Pass the return address in the first argument register.  */
        regcache_cooked_write_unsigned (regcache, arg_reg, struct_addr);

        DEBUG("struct return address 0x%08lX passed in R%d", struct_addr, arg_reg);

        arg_reg++;
    }

    /* If the function has arguments.  */
    if (nargs > 0)
    {
        Boolean      big_endian  = gdbarch_byte_order (gdbarch) == BFD_ENDIAN_BIG;
        unsigned int total_space = 0;
        gdb_byte    *memory_image;
        gdb_byte    *data;
        int          i;

        /* How much space do the arguments occupy in total?
           N.B. must round each argument's size up to an integral number of words.  */
        for (i = 0; i < nargs; i++)
        {
            unsigned int len   = TYPE_LENGTH (value_type (args[i]));
            unsigned int space = ROUND_UP_TO_WORDS(len);

            total_space += space;

            DEBUG("function arg[%d]: %d bytes -> %d\n", i, len, space);
        }

        /* Allocate a buffer to hold a memory image of the arguments.  */
        memory_image = XCALLOC((size_t) total_space, gdb_byte);
        if (memory_image == NULL)
        {
            /* could not do the call! */
            return 0;
        }

        /* Now copy all of the arguments into the buffer, correctly aligned.  */
        data = memory_image;
        for (i = 0; i < nargs; i++)
        {
            unsigned int len    = TYPE_LENGTH (value_type (args[i]));
            unsigned int space  = ROUND_UP_TO_WORDS(len);
            gdb_byte*    actual = data;
            unsigned int w;

            /* Parameters smaller than a word are normally aligned by gcc to the
               least significant byte, so if we have a big-endian target we put
               the data at the end of the word rather than at the start.  */
            if ((len < BYTES_IN_WORD) && big_endian)
               actual += BYTES_IN_WORD - len;

            (void) memcpy(actual, value_contents (args[i]), (size_t) len);

            DEBUG("function arg[%d] =", i);
            for (w = 0; w < space / BYTES_IN_WORD; w++)
                DEBUG(" 0x%08X", *((int*) data + w));
            DEBUG("\n");

            data += space;
        }

        /* Now load as much as possible of the memory image into registers.  */
        data = memory_image;
        while (arg_reg <= ARC_ABI_LAST_ARGUMENT_REGISTER)
        {
            ARC_RegisterContents contents = *(ARC_RegisterContents*) data;

            DEBUG("passing 0x%08X in register R%d\n", (unsigned int) contents, arg_reg);

            /* Convert to target byte order if necessary.  */
            if (HOST_AND_TARGET_ENDIANNESS_DIFFER(gdbarch))
            {
                contents = __bswap_32(contents);
                DEBUG("byte-swapped to 0x%08X\n", contents);
            }

            regcache_cooked_write_unsigned (regcache,
                                            arg_reg,
                                            (ULONGEST) contents);

            data        += BYTES_IN_REGISTER;
            total_space -= BYTES_IN_REGISTER;

            /* If all the data is now in registers.  */
            if (total_space == 0)
                break;

            arg_reg++;
        }

        /* If there is any data left, push it onto the stack (in a single write operation).  */
        if (total_space > 0)
        {
            DEBUG("passing %d bytes on stack\n", total_space);

            sp -= total_space;
            write_memory (sp, data, (int) total_space);
        }

        xfree(memory_image);
    }

    /* Finally, update the SP register.  */
    regcache_cooked_write_unsigned (regcache, ARC_SP_REGNUM, sp);

    DEBUG("set SP = %X\n", (unsigned int) sp);

    return sp;
}


/* Align frame.  */

static CORE_ADDR
arc_frame_align (struct gdbarch *gdbarch, CORE_ADDR sp)
{
    /* Align to the normal alignment on the stack).  */
    return WORD_ALIGNED(sp);
}


/* Print interesting information about the floating point processor
   (if present) or emulator.  */

static void
arc_print_float_info (struct gdbarch *gdbarch, struct ui_file *file,
                      struct frame_info *frame, const char *args)
{
    printf(_("Software FPU\n"));
}


/* Set the main_name to "_main" if required.
   This is set as an observer of inferior_created.  */

static void
arc_set_main_name (struct target_ops *objfile, int from_tty)
{
    /* Old ARC toolchains prepend an underscore to symbol names.  If there is
       an _main but no main, then we're probably debugging a binary that was
       made with the old toolchain.  */
    struct minimal_symbol *umainsym = lookup_minimal_symbol ("_main", NULL, NULL);
    struct minimal_symbol *mainsym  = lookup_minimal_symbol ("main",  NULL, NULL);

    if ((umainsym != NULL) && (mainsym == NULL))
    {
        set_main_name ("_main");
    }

    /* If we don't have any symbols, the default, i.e. "main", will get used.  */
}


/* This initialization function is called by gdb.
   See gdbarch.h for a description of its parameters.  */

static struct gdbarch*
arc_gdbarch_init (struct gdbarch_info  info,
                  struct gdbarch_list *arches)
{
    /* Allocate the ARC-private target-dependent information structure, and the
       gdb target-independent information structure.  */
    struct gdbarch_tdep *tdep    = xmalloc (sizeof (struct gdbarch_tdep));
    struct gdbarch      *gdbarch = gdbarch_alloc (&info, tdep);

    memset(tdep, 0, sizeof(*tdep));

    /* Put stuff in gdbarch. */

    /* Breakpoint manipulation.  */
    set_gdbarch_breakpoint_from_pc  (gdbarch, arc_breakpoint_from_pc);
    set_gdbarch_decr_pc_after_break (gdbarch, 0);

    info.osabi = CONFIG_OSABI;
    gdbarch_init_osabi(info, gdbarch);

    /* Characters are unsigned by default.  */
    set_gdbarch_char_signed (gdbarch, 0);

    set_gdbarch_sp_regnum        (gdbarch, ARC_SP_REGNUM);
    set_gdbarch_register_type    (gdbarch, arc_register_type);
    set_gdbarch_print_float_info (gdbarch, arc_print_float_info);

    /* Advance PC across function entry code.  */
    set_gdbarch_skip_prologue (gdbarch, arc_skip_prologue);

    /* Hook in the Dwarf-2 frame sniffer.  */
    dwarf2_frame_set_init_reg   (gdbarch, arc_dwarf2_frame_init_reg);
    frame_unwind_append_sniffer (gdbarch, dwarf2_frame_sniffer);

    /* Signal frames.  */
    frame_unwind_append_sniffer (gdbarch, arc_sigtramp_frame_sniffer);

    /* The stack grows downward.  */
    set_gdbarch_inner_than      (gdbarch, core_addr_lessthan);
    set_gdbarch_unwind_pc       (gdbarch, arc_unwind_pc);
    set_gdbarch_unwind_sp       (gdbarch, arc_unwind_sp);
    set_gdbarch_unwind_dummy_id (gdbarch, arc_unwind_dummy_id);
    frame_unwind_append_sniffer (gdbarch, arc_frame_sniffer);
    set_gdbarch_return_value    (gdbarch, arc_return_value);

    /* Add the ARC register groups.  */
    reggroup_add (gdbarch, general_reggroup);
    reggroup_add (gdbarch, all_reggroup);
    reggroup_add (gdbarch, system_reggroup);

    set_gdbarch_register_reggroup_p (gdbarch, arc_register_reggroup_p);

    set_gdbarch_cannot_step_breakpoint (gdbarch, 1);
    set_gdbarch_frame_align(gdbarch, arc_frame_align);

    /* Dummy Frame handling.  */
    set_gdbarch_push_dummy_call     (gdbarch, arc_push_dummy_call);
    set_gdbarch_call_dummy_location (gdbarch, AT_ENTRY_POINT);

    /* N.B. we do not want to call arcompact_get_disassembler at this point, as we do
            not as yet have an executable file, so do not know whether we want
            the disassembler for the A4 architecture (still supported by the
            opcodes library!) or the one for the ARCompact architecture; also,
            it is better to pass the arcompact_get_disassembler function a valid bfd
            structure, rather than a faked one, so that it can read the ARC-
            specific sections in the ELF file (for whatever reason...).

            So just set up a dummy disassembler at this point (gdb requires that
            *some* disassembler is defined for an architecture), and set up a
            callback which will set up the appropriate disassembler once an
            executable file has been selected for debugging.  */

#if 0
    {
        /* The arc libopcodes wants abfd so that it can find out what CPU
           extensions are there.  */
        bfd abfd;

        abfd.sections = NULL;

        set_gdbarch_print_insn(gdbarch, arcompact_get_disassembler(&abfd));
    }
#endif
    set_gdbarch_print_insn(gdbarch, dummy_disassembler);
    (void) observer_attach_new_objfile(set_disassembler);

    /* Set main_name to _main if necessary.  Ideally we'd want a hook that
       gets called when symbols are loaded, but it seems there isn't one; so
       we'll use this.  This will not work if the user does "target remote
       ..." and then "add-symbol-file ...".  */
    (void) observer_attach_inferior_created (arc_set_main_name);

    /* Initialize the target-dependent modules (if any).  */
    CONFIG_INIT_TDEP

    /* Return a pointer to the new object - indicates that a new architecture
       has been created.  */
    return gdbarch;
}


static void
arc_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
    /* Do nothing.  */
}


/* -------------------------------------------------------------------------- */
/*                     local functions implementing commands                  */
/* -------------------------------------------------------------------------- */

/* <command>  [ <frame> ]
   Display annotated stack frame.  */

static void
arc_show_stack_frame_command (char *arg, int from_tty)
{
    struct frame_info *fi;
    gdb_byte           value[MAX_REGISTER_SIZE];
    CORE_ADDR          pc, fp, sp, old_sp;
    CORE_ADDR          addr;
    enum lval_type     lval;
    int                optimized;
    int                realnum;
    int                frame_size;
    unsigned int      *frame_contents;
    LocalVariable     *variables;
    unsigned int       num_variables;

    /* Find the frame.  */
    fi = parse_frame_specification_1 (arg, _("No stack."), NULL);
    gdb_assert(fi != NULL);

    /* Find out PC, FP and SP for the frame.  */
    pc = get_frame_pc(fi);
    sp = get_frame_sp(fi);
    get_frame_register(fi, ARC_FP_REGNUM, value);
    fp = (CORE_ADDR) extract_unsigned_integer (value, BYTES_IN_REGISTER);

    DEBUG("*** PC = %x, FP = %x, SP = %x\n", (unsigned int) pc, (unsigned int) fp, (unsigned int) sp);

    if (pc == 0)
    {
        warning(_("invalid frame"));
        return;
    }

    frame_register_unwind (fi, ARC_SP_REGNUM, &optimized, &lval, &addr, &realnum, value);
    old_sp = (CORE_ADDR) extract_unsigned_integer (value, BYTES_IN_REGISTER);

    /* Find the local variables and parameters.  */
    variables = find_local_variables(fi, &num_variables);

    printf_filtered(NEW_LINE);
    identify_frame(fi);

    DEBUG(_("\n\n*** FRAME: 0x%x .. 0x%x\n\n"), (unsigned int) sp, (unsigned int) (old_sp - 1));

    frame_size = (int) (old_sp - sp);

    frame_contents = xmalloc((size_t) frame_size);

    if (frame_contents)
    {
        /* Read all of the frame's contents from target memory.  */
        if (target_read_memory(sp, (gdb_byte*) frame_contents, frame_size) == 0)
        {
            int          numregs       = ARC_TOTAL_REGS;
            int          i             = frame_size / BYTES_IN_WORD - 1;
            unsigned int the_same      = 0;
            unsigned int previous_word = 0;
            Boolean      first_word    = TRUE;
            Boolean      is_following_element;
            Boolean      elements_are_word_sized;

            addr = old_sp - BYTES_IN_WORD;

            printf_filtered(NEW_LINE);

            /* Traverse the frame from high address to low address, one word at a time.  */
            while (i >= 0)
            {
                unsigned int this_word = frame_contents[i];
                Boolean      print     = first_word;        /* Always print the first word. */
                char         line[256];
                int          n;

                /* Start with a blank line.  */
                (void) memset(line, (int) ' ', sizeof(line));

                /* Does FP and/or SP address this particular stack location?  */
                if (addr == sp && fp == sp)
                {
                    (void) strcpy(line + 14, _("SP/FP ===> "));
                    line[25] = ' ';
                    print = TRUE;
                }
                else if (addr == sp)
                {
                    (void) strcpy(line + 17, _("SP ===> "));
                    line[25] = ' ';
                    print = TRUE;
                }
                else if (addr == fp)
                {
                    (void) strcpy(line + 17, _("FP ===> "));
                    line[25] = ' ';
                    print = TRUE;
                }

                /* Does this stack location hold a local variable or parameter?  */
                if ((add_local_name(line + 38,
                                    addr,
                                    variables,
                                    num_variables,
                                    &is_following_element,
                                    &elements_are_word_sized)))
                {
                    /* Yes - so we want to print it (but this may be revised below!).  */
                    print = TRUE;
                }
                else
                {
                    int r;

                    /* Does this location hold a saved register?  */
                    for (r = 0; r < numregs; r++)
                    {
                        if (r != ARC_SP_REGNUM &&
                            gdbarch_register_reggroup_p (current_gdbarch, r, all_reggroup))
                        {
                            CORE_ADDR saved_addr;

                            /* Find out the location of the saved register without
                               fetching the corresponding value.  */
                            frame_register_unwind (fi, r, &optimized, &lval, &saved_addr, &realnum, NULL);

                            if (!optimized && lval == lval_memory && saved_addr == addr)
                            {
                                (void) sprintf(line + 38, _("saved register '%s'"), gdbarch_register_name(current_gdbarch, r));
                                print = TRUE;
                                break;
                            }
                        }
                    }
                }

                /* Do we think we need to print out the line?  */
                if (print)
                {
                    /* If the location is a non-zeroth word-sized array element,
                       and this word is the same as the previous word.  */
                    if (is_following_element && elements_are_word_sized && (this_word == previous_word))
                    {
                        /* No need to print it - one more word the same!  */
                        the_same++;
                        print = FALSE;
                    }
                }
                else
                {
                    /* N.B. this will not be the first word (since print is FALSE) */

                    /* This location does not hold anything "interesting",  but
                       if this word is not the same as the previous word, we
                       want to print it. */
                    if (this_word == previous_word)
                        the_same++;
                    else
                        print = TRUE;
                }

                /* OK, now we really know whether we need to print out the line.  */
                if (print)
                {
                    n = sprintf(line, _("0x%08X:"), (unsigned int) addr);
                    line[n] = ' ';

                    n = sprintf(line + 25, "%08X", this_word);
                    line[25 + n] = ' ';

                    n = (int) sizeof(line) - 1;
                    while (line[n] == ' ') n--;
                    line[n + 1] = '\0';

                    /* If we did not print the previous word because it was the
                       same as the word before, but not the same as any preceding
                       words, print it now (there is no point in outputting a
                       message which says simply "... 1 words omitted" - we might
                       just as well print out the word!  */
                    if (the_same == 1)
                        printf_filtered(_("0x%08X:              %08X\n"), (unsigned int) addr + BYTES_IN_WORD, previous_word);
                    else if (the_same != 0)
                        /* 2 or more words the same... */
                        printf_filtered(_("... %u words omitted\n"), the_same);

                    /* Now print out the line with the current word (and other information).  */
                    printf_filtered(_("%s\n"), line);

                    /* No words omitted.  */
                    the_same = 0;
                }

                first_word    = FALSE;
                previous_word = this_word;

                addr -= BYTES_IN_WORD;
                i--;
            }

            printf_filtered(NEW_LINE);
        }

        xfree(frame_contents);
    }

    xfree(variables);
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Initialize the module. This function is called from the gdb core on start-up.  */

void
_initialize_arc_tdep (void)
{
    /* Register the ARC processor architecture with gdb, providing an
       initialization function and a dump function; 'bfd_arch_arc' is
       an enumeration value specifically denoting the ARC architecture.  */
    gdbarch_register(bfd_arch_arc, arc_gdbarch_init, arc_dump_tdep);

    /* Register ARC-specific commands with gdb.  */

    add_setshow_boolean_cmd(DEBUG_COMMAND,
                            no_class,
                            &arc_debug_target,
                            _("Set whether to print ARC debug messages.\n"),
                            _("Show whether to print ARC debug messages.\n"),
                            _("If set debug messages are printed.\n"),
                            NULL,
                            NULL,
                            &setlist,
                            &showlist);

    (void) add_cmd(SHOW_FRAME_COMMAND,
                   class_obscure,
                   arc_show_stack_frame_command,
                   _("Display the stack frame with annotation.\n"
                     SHOW_FRAME_COMMAND_USAGE
                     "<FRAME> may be the number or address of a frame.\n"),
                   &cmdlist);
}


/* Initialize the info structure to enable the disassembler to be used.  */

void
arc_initialize_disassembler (struct disassemble_info *info)
{
    /* N.B. this type cast is not strictly correct: the return types differ!  */
    init_disassemble_info(info, gdb_stdout, (fprintf_ftype) fprintf_filtered);
    info->arch             = gdbarch_bfd_arch_info(current_gdbarch)->arch;
    info->mach             = gdbarch_bfd_arch_info(current_gdbarch)->mach;
    info->endian           = gdbarch_byte_order   (current_gdbarch);
    info->read_memory_func = read_memory_for_disassembler;
}

/******************************************************************************/
