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

   Possible values are 0 (unknown), LITTLE_ENDIAN, BIG_ENDIAN */

#ifndef WITH_HOST_BYTE_ORDER
#define WITH_HOST_BYTE_ORDER		0 /*unknown*/
#endif

#ifndef WITH_TARGET_BYTE_ORDER
#define WITH_TARGET_BYTE_ORDER		0 /*unknown*/
#endif

extern int current_host_byte_order;
#define CURRENT_HOST_BYTE_ORDER (WITH_HOST_BYTE_ORDER \
				 ? WITH_HOST_BYTE_ORDER \
				 : current_host_byte_order)
extern int current_target_byte_order;
#define CURRENT_TARGET_BYTE_ORDER (WITH_TARGET_BYTE_ORDER \
				   ? WITH_TARGET_BYTE_ORDER \
				   : current_target_byte_order)


/* PowerPC XOR endian.

   In addition to the above, the simulator can support the PowerPC's
   horrible XOR endian mode.  This feature makes it possible to
   control the endian mode of a processor using the MSR. */

#ifndef WITH_XOR_ENDIAN
#define WITH_XOR_ENDIAN		8
#endif


/* Intel host BSWAP support:

   Whether to use bswap on the 486 and pentiums rather than the 386
   sequence that uses xchgb/rorl/xchgb */
#ifndef WITH_BSWAP
#define	WITH_BSWAP 0
#endif


/* SMP support:

   Sets a limit on the number of processors that can be simulated.  If
   WITH_SMP is set to zero (0), the simulator is restricted to
   suporting only on processor (and as a consequence leaves the SMP
   code out of the build process).

   The actual number of processors is taken from the device
   /options/smp@<nr-cpu> */

#ifndef WITH_SMP
#define WITH_SMP                        5
#endif
#if WITH_SMP
#define MAX_NR_PROCESSORS		WITH_SMP
#else
#define MAX_NR_PROCESSORS		1
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

   Three environments are available - UEA (user), VEA (virtual) and
   OEA (perating).  The former two are environment that users would
   expect to see (VEA includes things like coherency and the time
   base) while OEA is what an operating system expects to see.  By
   setting these to specific values, the build process is able to
   eliminate non relevent environment code

   CURRENT_ENVIRONMENT specifies which of vea or oea is required for
   the current runtime. */

#define USER_ENVIRONMENT		1
#define VIRTUAL_ENVIRONMENT		2
#define OPERATING_ENVIRONMENT		3

#ifndef WITH_ENVIRONMENT
#define WITH_ENVIRONMENT		0
#endif

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

#define WITH_EVENTS                     (WITH_ENVIRONMENT != USER_ENVIRONMENT)


/* Time base:

   The PowerPC architecture includes the addition of both a time base
   register and a decrement timer.  Like events adds to the overhead
   of of some instruction cycles. */

#ifndef WITH_TIME_BASE
#define WITH_TIME_BASE			(WITH_ENVIRONMENT != USER_ENVIRONMENT)
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

/* Whether to check instructions for reserved bits being set */

#ifndef WITH_RESERVED_BITS
#define WITH_RESERVED_BITS		1
#endif

/* include monitoring code */

#define MONITOR_INSTRUCTION_ISSUE	1
#define MONITOR_LOAD_STORE_UNIT		2
#ifndef WITH_MON
#define WITH_MON			(MONITOR_LOAD_STORE_UNIT \
					 | MONITOR_INSTRUCTION_ISSUE)
#endif

/* Current CPU model (models are in the generated models.h include file)  */
#ifndef WITH_MODEL
#define WITH_MODEL			0
#endif

#define CURRENT_MODEL (WITH_MODEL	\
		       ? WITH_MODEL	\
		       : current_model)

#ifndef WITH_DEFAULT_MODEL
#define WITH_DEFAULT_MODEL		DEFAULT_MODEL
#endif

#define MODEL_ISSUE_IGNORE		(-1)
#define MODEL_ISSUE_PROCESS		1

#ifndef WITH_MODEL_ISSUE
#define WITH_MODEL_ISSUE		0
#endif

extern int current_model_issue;
#define CURRENT_MODEL_ISSUE (WITH_MODEL_ISSUE	\
			     ? WITH_MODEL_ISSUE	\
			     : current_model_issue)

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

      0  Make a normal external call to functions in the module.

      1  Include the module but to not inline functions within it.
         This allows functions within the module to inline functions
	 from other modules that have been included.

      2  Both include the module and inline functions contained within
         it.

   Finally, this is not for the faint harted.  I've seen GCC get up to
   200mb trying to compile what this can create */

/* Your compilers inline reserved word */

#ifndef INLINE
#if defined(__GNUC__) && defined(__OPTIMIZE__) && \
  (DEFAULT_INLINE || SIM_ENDIAN_INLINE || BITS_INLINE || CPU_INLINE || VM_INLINE || CORE_INLINE \
   || EVENTS_INLINE || MON_INLINE || INTERRUPTS_INLINE || REGISTERS_INLINE || DEVICE_TREE_INLINE \
   || DEVICES_INLINE || SPREG_INLINE || SEMANTICS_INLINE || IDECODE_INLINE || MODEL_INLINE)
#define INLINE __inline__
#else
#define INLINE /*inline*/
#endif
#endif

/* Default prefix for static functions */

#ifndef STATIC_INLINE
#define STATIC_INLINE static INLINE
#endif

/* Default macro to simplify control several of key the inlines */

#ifndef DEFAULT_INLINE
#define	DEFAULT_INLINE			0
#endif

/* Code that converts between hosts and target byte order.  Used on
   every memory access (instruction and data).  (See sim-endian.h for
   additional byte swapping configuration information) */

#ifndef SIM_ENDIAN_INLINE
#define SIM_ENDIAN_INLINE		DEFAULT_INLINE
#endif

/* Low level bit manipulation routines used to work around a compiler
   bug in 2.6.3.  */

#ifndef BITS_INLINE
#define BITS_INLINE			DEFAULT_INLINE
#endif

/* Code that gives access to various CPU internals such as registers.
   Used every time an instruction is executed */

#ifndef CPU_INLINE
#define CPU_INLINE			DEFAULT_INLINE
#endif

/* Code that translates between an effective and real address.  Used
   by every load or store. */

#ifndef VM_INLINE
#define VM_INLINE			DEFAULT_INLINE
#endif

/* Code that loads/stores data to/from the memory data structure.
   Used by every load or store */

#ifndef CORE_INLINE
#define CORE_INLINE			DEFAULT_INLINE
#endif

/* Code to check for and process any events scheduled in the future.
   Called once per instruction cycle */

#ifndef EVENTS_INLINE
#define EVENTS_INLINE			DEFAULT_INLINE
#endif

/* Code monotoring the processors performance.  It counts events on
   every instruction cycle */

#ifndef MON_INLINE
#define MON_INLINE			DEFAULT_INLINE
#endif

/* Code called on the rare occasions that an interrupt occures. */

#ifndef INTERRUPTS_INLINE
#define INTERRUPTS_INLINE		0
#endif

/* Code called on the rare occasion that either gdb or the device tree
   need to manipulate a register within a processor */

#ifndef REGISTERS_INLINE
#define REGISTERS_INLINE		0
#endif

/* Code called on the rare occasion that a processor is manipulating
   real hardware instead of RAM.

   Also, most of the functions in devices.c are always called through
   a jump table.

   There seems to be some problem with making either device_tree or
   devices inline.  It reports the message: device_tree_find_node()
   not a leaf */

#ifndef DEVICE_TREE_INLINE
#define DEVICE_TREE_INLINE		0
#endif

#ifndef DEVICES_INLINE
#define DEVICES_INLINE			0
#endif

/* Code called whenever information on a Special Purpose Register is
   required.  Called by the mflr/mtlr pseudo instructions */

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
#define SEMANTICS_INLINE		(DEFAULT_INLINE ? 1 : 0)
#endif

/* Code to decode an instruction. Normally called on every instruction
   cycle */

#ifndef IDECODE_INLINE
#define IDECODE_INLINE			DEFAULT_INLINE
#endif

/* Model specific code used in simulating functional units.  Note, it actaully
   pays NOT to inline the PowerPC model functions (at least on the x86).  This
   is because if it is inlined, each PowerPC instruction gets a separate copy
   of the code, which is not friendly to the cache.  */

#ifndef MODEL_INLINE
#define	MODEL_INLINE			(DEFAULT_INLINE ? 1 : 0)
#endif

/* Code to print out what options we were compiled with.  Because this
   is called at process startup, it doesn't have to be inlined, but
   if it isn't brought in and the model routines are inline, the model
   routines will be pulled in twice.  */

#ifndef OPTIONS_INLINE
#define OPTIONS_INLINE			(DEFAULT_INLINE ? 1 : 0)
#endif

#endif /* _CONFIG_H */
