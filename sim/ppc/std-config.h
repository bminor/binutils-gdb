/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _CONFIG_H_
#define _CONFIG_H_


/* endianness of the host/target:

   If the build process is aware (at compile time) of the endianness
   of the host/target it is able to eliminate slower generic endian
   handling code.

   If ENDIAN_OK is true then no byte swapping is required.  If it is
   false, copy-in / copy-out functions assume that data should be byte
   reversed as part of the copy. */

#define WITH_HOST_BYTE_ORDER		0 /*unknown*/
#define WITH_TARGET_BYTE_ORDER		0 /*unknown*/

extern int current_host_byte_order;
extern int current_target_byte_order;
#define CURRENT_HOST_BYTE_ORDER (WITH_HOST_BYTE_ORDER \
				 ? WITH_HOST_BYTE_ORDER \
				 : current_host_byte_order)
#define CURRENT_TARGET_BYTE_ORDER (WITH_TARGET_BYTE_ORDER \
				   ? WITH_TARGET_BYTE_ORDER \
				   : current_target_byte_order)


/* SMP support:

   Sets a limit on the number of processors that can be simulated.  If
   WITH_SMP is set to zero (0), the simulator is restricted to
   suporting only on processor (and as a consequence leaves the SMP
   code out of the build process). */

#ifndef WITH_SMP
#define WITH_SMP                        0
#endif


/* Word size of host/target:

   Set these according to your host and target requirements.  At this
   point in time, I've only compiled (not run) for a 64bit and never
   built for a 64bit host.  This will always remain a compile time
   option */

#ifndef WITH_TARGET_WORD_BITSIZE
#define WITH_TARGET_WORD_BITSIZE        32 /* compiled only */
#endif
#ifndef WITH_HOST_WORD_BITSIZE
#define WITH_HOST_WORD_BITSIZE		32 /* 64bit ready? */
#endif


/* Program environment:

   Two environments are available. VEA (or virtual environment
   architecture) and OEA (or operating environment architecture).  The
   former is the environment that a user program would see while the
   latter is the environment as seen by an operating system.  By
   setting these to specific values, the build process is able to
   eliminate non relevent environment code

   CURRENT_ENVIRONMENT specifies which of vea or oea is required for
   the current runtime. */

#define WITH_ENVIRONMENT		0
#define VIRTUAL_ENVIRONMENT		1
#define OPERATING_ENVIRONMENT		2

extern int current_environment;
#define CURRENT_ENVIRONMENT (WITH_ENVIRONMENT \
			     ? WITH_ENVIRONMENT \
			     : current_environment)


/* Optional VEA/OEA code: 

   The below, required for the OEA model may also be included in the
   VEA model however, as far as I can tell only make things
   slower... */


/* Events.  Devices modeling real H/W need to be able to efficiently
   schedule things to do at known times in the future.  The event
   queue implements this.  Unfortunatly this adds the need to check
   for any events once each full instruction cycle. */

#define WITH_EVENTS                     (WITH_ENVIRONMENT != VIRTUAL_ENVIRONMENT)


/* Time base:

   The PowerPC architecture includes the addition of both a time base
   register and a decrement timer.  Like events adds to the overhead
   of of some instruction cycles. */

#ifndef WITH_TIME_BASE
#define WITH_TIME_BASE			1
#endif


/* Callback/Default Memory.

   Core includes a builtin memory type (raw_memory) that is
   implemented using an array.  raw_memory does not require any
   additional functions etc.

   Callback memory is where the core calls a core device for the data
   it requires.

   Default memory is an extenstion of this where for addresses that do
   not map into either a callback or core memory range a default map
   can be used.

   The OEA model uses callback memory for devices and default memory
   for buses.

   The VEA model uses callback memory to capture `page faults'.

   While it may be possible to eliminate callback/default memory (and
   hence also eliminate an additional test per memory fetch) it
   probably is not worth the effort.

   BTW, while raw_memory could have been implemented as a callback,
   profiling has shown that there is a biger win (at least for the
   x86) in eliminating a function call for the most common
   (raw_memory) case. */

#define WITH_CALLBACK_MEMORY		1


/* Alignment:

   The PowerPC may or may not handle miss aligned transfers.  An
   implementation normally handles miss aligned transfers in big
   endian mode but generates an exception in little endian mode.

   This model.  Instead allows both little and big endian modes to
   either take exceptions or handle miss aligned transfers.

   If 0 is specified then for big-endian mode miss alligned accesses
   are permitted (NONSTRICT_ALIGNMENT) while in little-endian mode the
   processor will fault on them (STRICT_ALIGNMENT). */

#define NONSTRICT_ALIGNMENT    		1
#define STRICT_ALIGNMENT	       	2

#ifndef WITH_ALIGNMENT
#define WITH_ALIGNMENT     		0
#endif
extern int current_alignment;
#define CURRENT_ALIGNMENT (WITH_ALIGNMENT \
			   ? WITH_ALIGNMENT \
			   : current_alignment)


/* Floating point suport:

   Still under development. */

#define SOFT_FLOATING_POINT		1
#define HARD_FLOATING_POINT		2

#ifndef WITH_FLOATING_POINT
#define WITH_FLOATING_POINT		HARD_FLOATING_POINT
#endif
extern int current_floating_point;
#define CURRENT_FLOATING_POINT (WITH_FLOATING_POINT \
				? WITH_FLOATING_POINT \
				: current_floating_point)


/* Debugging:

   Control the inclusion of debugging code. */

/* Include the tracing code.  Disabling this eliminates all tracing
   code */

#ifndef WITH_TRACE
#define WITH_TRACE                      1
#endif

/* include code that checks assertions scattered through out the
   program */

#ifndef WITH_ASSERT
#define WITH_ASSERT			1
#endif

/* include profiling code that doesn't yet exist */

#ifndef WITH_PROFILE
#define WITH_PROFILE			1
#endif


/* INSTRUCTION TABLE CODE GENERATION:

   The program gen takes the files ppc.instructions and spr.table and
   creates from them code that provides:

   o	instruction decode and issue
   o	spr information

   The program gen does this according to the configuration
   information that follows. */


/* Line numbering of generated code:

   When generating the semantic and idecode files, gen can also output
   line number information (w.r.t. ppc.instructions).  It may be
   useful to disable this if you suspect that gen.c is incorrectly
   generating itermediate code files.  */

#ifndef WITH_LINE_NUMBERS
#define WITH_LINE_NUMBERS               1
#endif


/* Instruction cache:

   Instead of the idecode routine calling the semantic function
   directly, idecode can instead return a descriptor of the
   instruction (cache entry).

   With level one caching, idecode just returns the address of the
   semantic function.  With level two caching, in addition to this,
   the idecode routine decodes key fields within the instruction and
   also enters them into the cache.  The table IDECODE_CACHE_RULES
   controls what goes into the cache.*/

#ifndef	WITH_IDECODE_CACHE
#define WITH_IDECODE_CACHE              0
#endif
#ifndef	IDECODE_CACHE_SIZE
#define IDECODE_CACHE_SIZE		1024
#endif


/* Semantic code expansion:

   For a given instruction there is the potential to improve
   performance bo creating copies of the instructions code for one or
   more of its possible variations.  Eg branch being relative.  This
   macro determines of semantic functions should be expanded.  How
   well they are expanded is determined by the table
   WITH_IDECODE_OPCODE_RULES. */

#ifndef WITH_IDECODE_EXPAND_SEMANTICS
#define WITH_IDECODE_EXPAND_SEMANTICS   0
#endif


/* SPR database:

   The attributes of the SPR's are kept in a `lookup table'.  This
   table can be implemented as either a true table or a switch
   statement.

   A swith statement may be a performance advantage if the SPR's are
   known at compile time.  The compiler is then able to eliminate the
   switch. */

#ifndef WITH_SPREG_LOOKUP_TABLE
#define WITH_SPREG_LOOKUP_TABLE         1
#endif


/* Instruction decode:

   The table that follows is used by gen to construct a decision tree
   that can identify each possible instruction.  Gen then outputs this
   decision tree as (according to config) a table or switch statement
   as the function idecode.

   In parallel to this, as mentioned above, WITH_EXPANDED_SEMANTICS
   determines of the semantic functions themselves should be expanded
   in a similar way.

   The table contains the following entries:

   <valid>

   Must be 1 for the entry to be considered.  The last entry must be
   zero.
	
   <first>
   <last>

   Range of bits (within the instruction) that should be searched for
   an instruction field.  Within such ranges, gen looks for opcodes
   (constants), registers (strings) and reserved bits (slash) and
   according to the rules that follows includes or excludes them from
   a possible instruction field.

   <force_first>
   <force_last>

   If an instructioin field was found, enlarge the field size so that
   it is forced to at least include bits starting from <force_first>
   (<force_last>).  To stop this occuring, use <force_first> = <last>
   + 1 and <force_last> = <first> - 1.

   <force_slash>

   Treat `/' fields as a constant instead of variable when looking for
   an instruction field.

   <force_expansion>

   Treat any contained register (string) fields as constant when
   determining the instruction field.  For the instruction decode (and
   controled by IDECODE_EXPAND_SEMANTICS) this forces the expansion of
   what would otherwize be non constant bits of an instruction.

   <use_switch>

   Should this table be expanded using a switch statement (val 1) and
   if so, should it be padded with entries so as to force the compiler
   to generate a jump table (val 2).

   <special_mask>
   <special_value>
   <special_rule>

   Special rule to fine tune how specific (or groups) of instructions
   are expanded.  The applicability of the rule is determined by

     <special_mask> != 0 && (instruction> & <special_mask>) == <special_value>

   Where <instruction> is obtained by looking only at constant fields
   with in an instructions spec.  When determining an expansion, the
   rule is only considered when a node contains a single instruction.
   <special_rule> can be any of:

        0: for this instruction, expand by earlier rules
   	1: expand bits <force_low> .. <force_hi> only
	2: boolean expansion of only zero/non-zero cases

   Ok? */


#define WITH_IDECODE_OPCODE_RULES { \
  { 1,  0,  5,  0,  5, 0, 0,                1, 0x00000000, 0x00000000, 0 }, \
  { 1, 21, 31, 32, -1, 0, 0,                1, 0x00000000, 0x00000000, 0 }, \
  { 0 } \
}


/* Instruction unpacking:

   Once the instruction has been decoded, the register (and other)
   fields within the instruction need to be extracted.

   The table that follows determines how each field should be treated.
   Importantly it considers the case where the extracted field is to
   be used immediatly or stored in an instruction cache.

   <valid>

   Zero marks the end of the table.  More importantly 1. indicates
   that the entry is valid and can be cached. 2. indicates that that
   the entry is valid but can not be cached.

   <old_name>

   The field name as given in the instruction spec.

   <new_name>

   A name for <old_name> once it has been extracted from the
   instructioin (and possibly stored in the instruction cache).

   <type>

   String specifying the storage type for <new_name> (the extracted
   field>.

   <expression>

   Specifies how to get <new_name> from <old_name>.  If null, old and
   new name had better be the same. */

#define WITH_IDECODE_CACHE_RULES { \
  { 1, "RA", "RA", 0, 0 }, \
  { 1, "RA", "rA", "signed_word *", \
      "(cpu_registers(processor)->gpr + RA)" }, \
  { 1, "RT", "RT", 0, 0 }, \
  { 1, "RT", "rT", "signed_word *", \
      "(cpu_registers(processor)->gpr + RT)" }, \
  { 2, "RS", "RS", 0, 0 }, \
  { 1, "RS", "rS", "signed_word *", \
      "(cpu_registers(processor)->gpr + RS)" }, \
  { 2, "RB", "RB", 0, 0 }, \
  { 1, "RB", "rB", "signed_word *", \
      "(cpu_registers(processor)->gpr + RB)" }, \
  { 2, "FRA", "FRA", 0, 0 }, \
  { 1, "FRA", "frA", "unsigned64 *", \
      "(cpu_registers(processor)->fpr + FRA)" }, \
  { 2, "FRB", "FRB", 0, 0 }, \
  { 1, "FRB", "frB", "unsigned64 *", \
      "(cpu_registers(processor)->fpr + FRB)" }, \
  { 2, "FRC", "FRC", 0, 0 }, \
  { 1, "FRC", "frC", "unsigned64 *", \
      "(cpu_registers(processor)->fpr + FRC)" }, \
  { 2, "FRS", "FRS", 0, 0 }, \
  { 1, "FRS", "frS", "unsigned64 *", \
      "(cpu_registers(processor)->fpr + FRS)" }, \
  { 2, "FRT", "FRT", 0, 0 }, \
  { 1, "FRT", "frT", "unsigned64 *", \
      "(cpu_registers(processor)->fpr + FRT)" }, \
  { 1, "SI", "EXTS_SI", "unsigned_word", \
      "((signed_word)(signed16)instruction)" }, \
  { 2, "BI", "BI", 0, 0 }, \
  { 1, "BI", "BIT32_BI", 0, \
      "BIT32(BI)" }, \
  { 2, "BA", "BA", 0, 0 }, \
  { 1, "BA", "BIT32_BA", 0, \
       "BIT32(BA)" }, \
  { 2, "BB", "BB", 0, 0 }, \
  { 1, "BB", "BIT32_BB", 0, \
      "BIT32(BB)" }, \
  { 1, "BD", "EXTS_BD_0b00", "unsigned_word", \
      "(((signed_word)(signed16)instruction) & ~3)" }, \
/*{ 1, "BD", "CIA_plus_EXTS_BD_0b00", "unsigned_word", */ \
/*    "CIA + EXTS(BD_0b00)" }, */ \
  { 1, "LI", "EXTS_LI_0b00", "unsigned_word", \
      "((((signed_word)(signed32)(instruction << 6)) >> 6) & ~0x3)" }, \
  { 1, "D", "EXTS_D", "unsigned_word", \
      "((signed_word)(signed16)(instruction))" }, \
  { 1, "DS", "EXTS_DS_0b00", "unsigned_word", \
      "(((signed_word)(signed16)instruction) & ~0x3)" }, \
  { 0 } \
};



/* INLINE CODE SELECTION:

   GCC -O3 attempts to inline any function or procedure in scope.  The
   options below facilitate fine grained control over what is and what
   isn't made inline.  For instance it can control things down to a
   specific modules static routines.  This control is implemented in
   two parts.  Doing this allows the compiler to both eliminate the
   overhead of function calls and (as a consequence) also eliminate
   further dead code.

   Experementing with CISC (x86) I've found that I can achieve an
   order of magintude speed improvement (x3-x5).  In the case of RISC
   (sparc) while the performance gain isn't as great it is still
   significant.

   Part One - Static functions: It is possible to control how static
   functions within each module are to be compiled.  On a per module
   or global basis, it is possible to specify that a modules static
   functions should be compiled inline.  This is controled by the the
   macro's STATIC_INLINE and INLINE_STATIC_<module>.

   Part Two - External functions: Again it is possible to allow the
   inlining of calls to external functions.  This is far more
   complicated and much heaver on the compiler.  In this case, it is
   controled by the <module>_INLINE macro's.  Where each can have a
   value:

      0  ppc.c should call external module

      1  ppc.c should have local copy (and hence possibly facilitate
         the in lineing of that modules external calls)

      2  ppc.c should inline this module

   Finally, this is not for the faint harted.  I've seen GCC get up to
   200mb trying to compile what this can create */

/* Your compilers inline reserved word */

#ifndef INLINE
#if defined(__GNUC__) && defined(__OPTIMIZE__)
#define INLINE __inline__
#else
#define INLINE /*inline*/
#endif
#endif

/* Default prefix for static functions */

#ifndef STATIC_INLINE
#define STATIC_INLINE static INLINE
#endif

/* Default macro to control several of the inlines */

#ifndef DEFAULT_INLINE
#define	DEFAULT_INLINE			0
#endif

/* Code that does byte swapping used on any memory access */

#ifndef ENDIAN_INLINE
#define ENDIAN_INLINE			DEFAULT_INLINE
#endif

/* Instruction cache if in use */

#if 0 /*DNE*/
#ifndef ICACHE_INLINE
#define ICACHE_INLINE			0
#endif
#endif

/* Given a translated address, core maps it onto either simulator data
   or a function call, this is performed once for each
   data/instruction access */


#ifndef CORE_INLINE
#define CORE_INLINE			DEFAULT_INLINE
#endif


/* The cpu object.  May things call upon this module to manipulate
   each cpu object for instance register updates (from semantics) or
   instruction execution from psim */

#ifndef VM_INLINE
#define VM_INLINE			DEFAULT_INLINE
#endif

/* Physical memory is implemented using the memory map module */

#ifndef CPU_INLINE
#define CPU_INLINE			DEFAULT_INLINE
#endif

/* handle the queue of events to happen in the future */

#ifndef EVENTS_INLINE
#define EVENTS_INLINE			DEFAULT_INLINE
#endif

/* not so important register manipulation code.  Most important
   register operations are performed directly on the register file */

#ifndef REGISTERS_INLINE
#define REGISTERS_INLINE		DEFAULT_INLINE
#endif

/* interrupt handling code */

#ifndef INTERRUPTS_INLINE
#define INTERRUPTS_INLINE		DEFAULT_INLINE
#endif

/* device code. While possibly important, this isn't as critical as
   the cpu/memory path

   There seems to be some problem with making either device_tree or
   devices inline.  It reports the message:
   device_tree_find_node() not a leaf  */

#ifndef DEVICE_TREE_INLINE
#define DEVICE_TREE_INLINE		0
#endif

#ifndef DEVICES_INLINE
#define DEVICES_INLINE			0
#endif

/* Special Purpose Register tables.  Provide information on the
   attributes of given SPR's. */

#ifndef SPREG_INLINE
#define SPREG_INLINE			DEFAULT_INLINE
#endif

/* Functions modeling the semantics of each instruction.  Two cases to
   consider, firstly of idecode is implemented with a switch then this
   allows the idecode function to inline each semantic function
   (avoiding a call).  The second case is when idecode is using a
   table, even then while the semantic functions can't be inlined,
   setting it to one still enables each semantic function to inline
   anything they call (if that code is marked for being inlined).

   WARNING: you need lots (like 200mb of swap) of swap.  Setting this
   to 1 is useful when using a table as it enables the sematic code to
   inline all of their called functions */

#ifndef SEMANTICS_INLINE
#define SEMANTICS_INLINE		0
#endif

/* Functions that decode an instruction.  Called by the cpu module.
   Part of the performance critical fetch - decode - issue sequence */

#ifndef IDECODE_INLINE
#define IDECODE_INLINE			DEFAULT_INLINE
#endif



/* If you're confused by the above, check out some of the generic
   configurations below. */


#if 0
/* Allow the expansion of the semantic functions.  That is, if the
   branch instruction is called with AA=0 and AA=1, generate separate
   functions for each case */

#undef WITH_IDECODE_EXPAND_SEMANTICS
#define WITH_IDECODE_EXPAND_SEMANTICS 1

#undef WITH_IDECODE_OPCODE_RULES
#define WITH_IDECODE_OPCODE_RULES { \
  { 1,  0,  5,  0,  5, 0, 0,                0, 0x00000000, 0x00000000, 0 }, \
  { 1, 21, 31, 32, -1, 0, "OE,LR,AA,Rc,LK", 0, 0x00000000, 0x00000000, 0 }, \
  { 1,  6,  9,  6,  9, 0, "BO",             0, 0xfc000000, 0x40000000, 1 }, \
  { 1, 11, 15, 11, 15, 0, "RA",             0, 0xfc000000, 0x38000000, 2 }, \
  { 1, 11, 15, 11, 15, 0, "RA",             0, 0xfc000000, 0x3c000000, 2 }, \
  { 0 } \
}
#endif


#if 0
/* eliminate any debugging noise */

#undef WITH_TRACE
#define WITH_TRACE 0

#undef WITH_ASSERT
#define WITH_ASSERT 0

#endif


#if 0
/* A reasonable set of inline macro's that give the compiler a
   fighting chance at eliminating much of the function call overhead.

   Typically, with the below the -O3 option (to get inline of all
   functioins) isn't of any greate benefit. */

#undef INLINE
#define INLINE inline

#undef STATIC_INLINE
#define STATIC_INLINE static INLINE

#undef ENDIAN_INLINE
#define ENDIAN_INLINE 2

#if 0 /*DNE*/
#undef ICACHE_INLINE
#define ICACHE_INLINE 0
#endif

#undef CORE_INLINE
#define CORE_INLINE 2

#undef VM_INLINE
#define VM_INLINE 2

#undef CPU_INLINE
#define CPU_INLINE 2

#undef EVENTS_INLINE
#define EVENTS_INLINE 2

#undef REGISTERS_INLINE
#define REGISTERS_INLINE 2

#undef INTERRUPTS_INLINE
#define INTERRUPTS_INLINE 2

#undef DEVICE_TREE_INLINE
#define DEVICE_TREE_INLINE 0

#undef DEVICES_INLINE
#define DEVICES_INLINE 0

#undef SPREG_INLINE
#define SPREG_INLINE 2

#undef SEMANTICS_INLINE
#define SEMANTICS_INLINE 1 /* not 2! as it blows away the compiler */

#undef IDECODE_INLINE
#define IDECODE_INLINE 2

#endif


#if 0
/* Enable the full cracking cache.  The cracked instruction cache
   appears to give best performance if most functions have been lined
   as well */

#undef WITH_IDECODE_CACHE
#define WITH_IDECODE_CACHE 2

#endif



#if 0
/* With the VEA model, can eliminate some things.  Not least of which
   is support for the OEA model */

#undef WITH_ENVIRONMENT
#define WITH_ENVIRONMENT VIRTUAL_ENVIRONMENT

#undef WITH_EVENTS
#define WITH_EVENTS 0

#undef WITH_SMP
#define WITH_SMP 0

#undef WITH_TARGET_BYTE_ORDER 
#define WITH_TARGET_BYTE_ORDER WITH_HOST_BYTE_ORDER

#endif




#if 0
/* Finally, the expansion rules below are extreemly agressive.  Only
   consider them if your build machine is VERY VERY VERY VERY VERY
   well configured */

#undef WITH_IDECODE_EXPAND_SEMANTICS
#define WITH_IDECODE_EXPAND_SEMANTICS 1

#undef WITH_IDECODE_OPCODE_RULES
#define WITH_IDECODE_OPCODE_RULES { \
  { 1,  0,  5,  0,  5, 0, 0,                0, 0x00000000, 0x00000000, 0 }, \
  { 1, 21, 31, 32, -1, 0, "OE,LR,AA,Rc,LK", 0, 0x00000000, 0x00000000, 0 }, \
  { 1,  6, 15,  6, 15, 0, "BO,BI",          0, 0xfc000000, 0x40000000, 0 }, \
  { 1, 11, 15, 11, 15, 0, "RA",             0, 0xfc000000, 0x38000000, 0 }, \
  { 1, 11, 15, 11, 15, 0, "RA",             0, 0xfc000000, 0x3c000000, 0 }, \
  { 1, 11, 20, 11, 20, 0, "spr",            0, 0xfc000000, 0x7c000000, 0 }, \
  { 0 } \
}
#endif


#endif /* _CONFIG_H */
