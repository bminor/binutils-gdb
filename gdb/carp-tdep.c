#include "defs.h"
#include "tm.h"
#include "target.h"

/* #define CARP */

/* BAD HACKS */

int carp_sizeof_struct_frame_saved_regs;
/* #define SIZEOF_STRUCT_FRAME_SAVED_REGS carp_sizeof_struct_frame_saved_regs */




#if 0
@item ADDITIONAL_OPTIONS
@item ADDITIONAL_OPTION_CASES
@item ADDITIONAL_OPTION_HANDLER
@item ADDITIONAL_OPTION_HELP
These are a set of macros that allow the addition of additional command
line options to GDB.  They are currently used only for the unsupported
i960 Nindy target, and should not be used in any other configuration.
#endif


#if 0
@item ADDR_BITS_REMOVE (addr)
If a raw machine address includes any bits that are not really part of
the address, then define this macro to expand into an expression that
zeros those bits in @var{addr}.  For example, the two low-order bits of
a Motorola 88K address may be used by some kernels for their own
purposes, since addresses must always be 4-byte aligned, and so are of
no use for addressing.  Those bits should be filtered out with an
expression such as @code{((addr) & ~3)}.
#endif
CORE_ADDR carp_addr_bits_remove (CORE_ADDR addr) { return 0; }
/* #define ADDR_BITS_REMOVE(addr) carp_addr_bits_remove (addr) */


#if 0
@item BEFORE_MAIN_LOOP_HOOK
Define this to expand into any code that you want to execute before the
main loop starts.  Although this is not, strictly speaking, a target
conditional, that is how it is currently being used.  Note that if a
configuration were to define it one way for a host and a different way
for the target, GDB will probably not compile, let alone run correctly.
This is currently used only for the unsupported i960 Nindy target, and
should not be used in any other configuration.
#endif


#if 0
@item BELIEVE_PCC_PROMOTION
Define if the compiler promotes a short or char parameter to an int, but
still reports the parameter as its original type, rather than the
promoted type.
#endif
int carp_believe_pcc_promotion;
/* #define BELIEVE_PCC_PROMOTION carp_believe_pcc_promotion */


#if 0
@item BELIEVE_PCC_PROMOTION_TYPE
Define this if GDB should believe the type of a short argument when
compiled by pcc, but look within a full int space to get its value.
Only defined for Sun-3 at present.
#endif
int carp_believe_pcc_promotion_type;
/* #define BELIEVE_PCC_PROMOTION_TYPE carp_believe_pcc_promotion_type */


#if 0
@item BITS_BIG_ENDIAN
Define this if the numbering of bits in the targets does *not* match the
endianness of the target byte order.  A value of 1 means that the bits
are numbered in a big-endian order, 0 means little-endian.
#endif
int carp_bits_big_endian;
/* #define BITS_BIG_ENDIAN carp_bits_big_endian */


#if 0
@item BREAKPOINT
This is the character array initializer for the bit pattern to put into
memory where a breakpoint is set.  Although it is common to use a trap
instruction for a breakpoint, it is not required; for instance, the bit
pattern could be an invalid instruction.  The breakpoint must be no
longer than the shortest instruction of the architecture.
#endif
char *carp_breakpoint;
/* #define BREAKPOINT carp_breakpoint */


#if 0
@item BIG_BREAKPOINT
@item LITTLE_BREAKPOINT
Similar to BREAKPOINT, but used for bi-endian targets.
#endif
char *carp_big_breakpoint;
char *carp_little_breakpoint;
/* #define BIG_BREAKPOINT carp_big_breakpoint */
/* #define LITTLE_BREAKPOINT carp_little_breakpoint */


#if 0
@item CALL_DUMMY
valops.c
@item CALL_DUMMY_LOCATION
inferior.h
@item CALL_DUMMY_STACK_ADJUST
valops.c
@item CALL_DUMMY_START_OFFSET
#endif
char *carp_call_dummy (void) { return 0; }
/* #define CALL_DUMMY carp_call_dummy () */
int carp_sizeof_call_dummy;
/* #define SIZEOF_CALL_DUMMY carp_sizeof_call_dummy */
int carp_call_dummy_location;
/* #define CALL_DUMMY_LOCATION carp_call_dummy_location */
int carp_call_stack_adjust;
/* #define CALL_DUMMY_STACK_ADJUST carp_call_stack_adjust */
int carp_call_start_offset;
/* #define CALL_DUMMY_START_OFFSET carp_call_start_offset */


#if 0
@item CANNOT_FETCH_REGISTER (regno)
A C expression that should be nonzero if @var{regno} cannot be fetched
from an inferior process.  This is only relevant if
@code{FETCH_INFERIOR_REGISTERS} is not defined.
#endif
int carp_cannot_fetch_register;
/* #define CANNOT_FETCH_REGISTER(regno) carp_cannot_fetch_register(regno) */


#if 0
@item CANNOT_STORE_REGISTER (regno)
A C expression that should be nonzero if @var{regno} should not be
written to the target.  This is often the case for program counters,
status words, and other special registers.  If this is not defined, GDB
will assume that all registers may be written.
#endif
int carp_cannot_store_register (int regno) { return 0; }
/* #define CANNOT_STORE_REGISTER(regno) carp_cannot_store_register(regno) */


#if 0
@item CHILL_PRODUCER
@item GCC_PRODUCER
@item GPLUS_PRODUCER
@item LCC_PRODUCER
If defined, these are the producer strings in a DWARF 1 file.  All of
these have reasonable defaults already.
#endif


#if 0
@item DO_DEFERRED_STORES
@item CLEAR_DEFERRED_STORES
Define this to execute any deferred stores of registers into the inferior,
and to cancel any deferred stores.
Currently only implemented correctly for native Sparc configurations?
#endif
void carp_do_deferred_stores (void) { return; }
void carp_clear_deferred_stores (void) { return; }
/* #define DO_DEFERRED_STORES carp_do_deferred_stores () */
/* #define CLEAR_DEFERRED_STORES carp_clear_deferred_stores () */


#if 0
@item CPLUS_MARKER
Define this to expand into the character that G++ uses to distinguish
compiler-generated identifiers from programmer-specified identifiers.
By default, this expands into @code{'$'}.  Most System V targets should
define this to @code{'.'}.
#endif


#if 0
@item DBX_PARM_SYMBOL_CLASS
Hook for the @code{SYMBOL_CLASS} of a parameter when decoding DBX symbol
information.  In the i960, parameters can be stored as locals or as
args, depending on the type of the debug record.
#endif


#if 0
@item DECR_PC_AFTER_BREAK
Define this to be the amount by which to decrement the PC after the
program encounters a breakpoint.  This is often the number of bytes in
BREAKPOINT, though not always.  For most targets this value will be 0.
#endif
int carp_decr_pc_after_break;
/* #define DECR_PC_AFTER_BREAK carp_decr_pc_after_break */


#if 0
@item DECR_PC_AFTER_HW_BREAK
Similarly, for hardware breakpoints.
#endif
int carp_decr_pc_after_hw_break;
/* #define DECR_PC_AFTER_HW_BREAK carp_decr_pc_after_hw_break */


#if 0
@item DISABLE_UNSETTABLE_BREAK addr
If defined, this should evaluate to 1 if @var{addr} is in a shared
library in which breakpoints cannot be set and so should be disabled.
#endif


#if 0
@item DO_REGISTERS_INFO
If defined, use this to print the value of a register or all registers.
#endif
void carp_do_registers_info (int regnum, int fpregs) { return; }
/* #define DO_REGISTERS_INFO(r, f) carp_do_registers_info (r, f) */


#if 0
@item END_OF_TEXT_DEFAULT
This is an expression that should designate the end of the text section
(? FIXME ?)
#endif


#if 0
@item EXTRACT_RETURN_VALUE(type,regbuf,valbuf)
Define this to extract a functions return value of type @var{type} from
the raw register state @var{regbuf} and copy that, in virtual format,
into @var{valbuf}.
#endif
void carp_extract_return_value (struct type *type, char *regbuf, char *valbuf) { return; }
/* #define EXTRACT_RETURN_VALUE(type,regbuf,valbuf) carp_extract_return_value(type,regbuf,valbuf) */

 
#if 0
@item EXTRACT_STRUCT_VALUE_ADDRESS(regbuf)
Define this to extract from an array @var{regbuf} containing the (raw)
register state, the address in which a function should return its
structure value, as a CORE_ADDR (or an expression that can be used as
one).
#endif
CORE_ADDR carp_extract_struct_value_address (char *regbuf) { return 0; }
/* #define EXTRACT_STRUCT_VALUE_ADDRESS(regbuf) carp_extract_struct_value_address(regbuf) */


#if 0
@item EXTRA_FRAME_INFO
If defined, this must be a list of slots that may be inserted into the
@code{frame_info} structure defined in @code{frame.h}.
#endif


#if 0
@item FIX_CALL_DUMMY
#endif
CORE_ADDR carp_fix_call_dummy (char *dummyname, CORE_ADDR start_sp, CORE_ADDR fun, int nargs, struct value **args, struct type *type, int gcc_p) { return 0; }
/* #define FIX_CALL_DUMMY(dummy1, start_sp, funaddr, nargs, args, value_type, using_gcc) carp_fix_call_dummy (dummy1, start_sp, funaddr, nargs, args, value_type, using_gcc) */


#if 0
@item FLOAT_INFO
If defined, then the info float command will print information about
the processors floating point unit.
#endif
void carp_float_info (void) { return; }
/* #define FLOAT_INFO carp_float_info () */


#if 0
@item FP_REGNUM
The number of the frame pointer register.
#endif
int carp_fp_regnum;
/* #define FP_REGNUM carp_fp_regnum */


#if 0
@item FRAMELESS_FUNCTION_INVOCATION(fi, frameless)
Define this to set the variable @var{frameless} to 1 if the function
invocation represented by @var{fi} does not have a stack frame
associated with it.  Otherwise set it to 0.
#endif
int carp_frameless_function_invocation (struct frame_info *fi) { return 0; }
/* #define FRAMELESS_FUNCTION_INVOCATION(fi,frameless) (frameless) = carp_frameless_function_invocation (fi) */


#if 0
@item FRAME_ARGS_ADDRESS
stack.c
#endif
CORE_ADDR carp_frame_args_address (struct frame_info *fi) { return 0; };
/* #define FRAME_ARGS_ADDRESS(FI) carp_frame_args_address (FI) */


#if 0
@item FRAME_ARGS_ADDRESS_CORRECT
stack.c
#endif
CORE_ADDR carp_frame_args_address_correct (struct frame_info *fi) { return 0; }
/* #define FRAME_ARGS_ADDRESS_CORRECT(FI) carp_frame_args_address_correct(FI) */


#if 0
@item FRAME_ARGS_SKIP
#endif
int carp_frame_args_skip;
/* #define FRAME_ARGS_SKIP carp_frame_args_skip */


#if 0
@item FRAME_CHAIN(frame)
Given @var{frame}, return a pointer to the calling frame.
#endif
CORE_ADDR carp_frame_chain (struct frame_info *fi) { return 0; }
/* #define FRAME_CHAIN(frame) carp_frame_chain (frame) */


#if 0
@item FRAME_CHAIN_COMBINE(chain,frame)
Define this to take the frame chain pointer and the frames nominal
address and produce the nominal address of the callers frame.
Presently only defined for HP PA.
#endif
CORE_ADDR carp_frame_chain_combine (CORE_ADDR address, struct frame_info *fi) { return 0; }
/* #define FRAME_CHAIN_COMBINE(address, frame) carp_frame_chain_combine (address, frame) */



#if 0
@item FRAME_CHAIN_VALID(chain,thisframe)
Define this to be an expression that returns zero if the given frame is
an outermost frame, with no caller, and nonzero otherwise.  The default
definition is nonzero if the chain pointer is nonzero and given frames
PC is not inside the startup file (such as @file{crt0.o}).  The
alternate default definition (which is used if
FRAME_CHAIN_VALID_ALTERNATE is defined) is nonzero if the chain pointer
is nonzero and the given frames PC is not in @code{main()} or a known
entry point function (such as @code{_start()}).
#endif
int carp_frame_chain_valid (CORE_ADDR address, struct frame_info *fi) { return 0; }
/* #define FRAME_CHAIN_VALID(address, frame) carp_frame_chain_valid (address, frame) */


#if 0
@item FRAME_CHAIN_VALID_ALTERNATE
Define this in order to use the alternate default definition of
@code{FRAME_CHAIN_VALID}.
#endif


#if 0
@item FRAME_FIND_SAVED_REGS
stack.c
#endif
void carp_frame_find_saved_regs (struct frame_info *frame, struct frame_saved_regs *saved_regs_addr) { return; }
/* #define FRAME_FIND_SAVED_REGS(frame, saved_regs_addr) carp_frame_find_saved_regs (frame, &(saved_regs_addr)) */


#if 0
@item FRAME_LOCALS_ADDRESS
stack.c
#endif
CORE_ADDR carp_frame_locals_address (struct frame_info *fi) { return 0; };
/* #define FRAME_LOCALS_ADDRESS(FI) carp_frame_locals_address (FI) */


#if 0
@item FRAME_NUM_ARGS (val, fi)
For the frame described by fi, set val to the number of arguments
that are being passed.
#endif
int carp_frame_num_args (struct frame_info *fi) { return 0; }
/* #define FRAME_NUM_ARGS(val, fi) (val) = carp_frame_num_args (fi) */


#if 0
@item FRAME_SAVED_PC(frame)
Given @var{frame}, return the pc saved there.  That is, the return
address.
#endif
CORE_ADDR carp_frame_saved_pc (struct frame_info *fi) { return 0; }
/* #define FRAME_SAVED_PC(fi) carp_frame_saved_pc (fi) */


#if 0
@item FUNCTION_EPILOGUE_SIZE
For some COFF targets, the @code{x_sym.x_misc.x_fsize} field of the
function end symbol is 0.  For such targets, you must define
@code{FUNCTION_EPILOGUE_SIZE} to expand into the standard size of a
functions epilogue.
#endif
int carp_function_epilogue_size;
/* #define FUNCTION_EPILOGUE_SIZE carp_function_epilogue_size */


#if 0
@item FUNCTION_START_OFFSET
#endif
int carp_function_start_offset;
/* #define FUNCTION_START_OFFSET carp_function_start_offset */


#if 0
@item GCC_COMPILED_FLAG_SYMBOL
@item GCC2_COMPILED_FLAG_SYMBOL
If defined, these are the names of the symbols that GDB will look for to
detect that GCC compiled the file.  The default symbols are
@code{gcc_compiled.} and @code{gcc2_compiled.}, respectively. (Currently
only defined for the Delta 68.)
#endif


#if 0
@item GDB_TARGET_IS_HPPA
This determines whether horrible kludge code in dbxread.c and
partial-stab.h is used to mangle multiple-symbol-table files from
HPPAs.  This should all be ripped out, and a scheme like elfread.c
used.
#endif


#if 0
@item GDB_TARGET_IS_MACH386
@item GDB_TARGET_IS_SUN3
@item GDB_TARGET_IS_SUN386
Kludges that should go away.
#endif


#if 0
@item GET_LONGJMP_TARGET
For most machines, this is a target-dependent parameter.  On the
DECstation and the Iris, this is a native-dependent parameter, since
<setjmp.h> is needed to define it.

This macro determines the target PC address that longjmp() will jump to,
assuming that we have just stopped at a longjmp breakpoint.  It takes a
CORE_ADDR * as argument, and stores the target PC value through this
pointer.  It examines the current state of the machine as needed.
#endif


#if 0
@item GET_SAVED_REGISTER
Define this if you need to supply your own definition for the function
@code{get_saved_register}.  Currently this is only done for the a29k.
#endif
void get_saved_register (char *raw_buffer, int *optimized, CORE_ADDR *addrp, struct frame_info *frame, int regnum, enum lval_type *lval) { return; }
/* #define GET_SAVED_REGISTER */


#if 0
@item HAVE_REGISTER_WINDOWS
Define this if the target has register windows.
@item REGISTER_IN_WINDOW_P (regnum)
Define this to be an expression that is 1 is the given register is in
the window.
#endif
int carp_have_register_windows;
int carp_register_in_window_p (int regnum) { return 0; }
/* #define HAVE_REGISTER_WINDOWS carp_have_register_windows */
/* #define REGISTER_IN_WINDOW_P(regnum) carp_register_in_window_p (regnum) */


#if 0
@item REGISTER_VIRTUAL_TYPE
#endif
struct type *carp_register_virtual_type (int regno) { return 0; }
/* #define REGISTER_VIRTUAL_TYPE(regno) carp_register_virtual_type (regno) */



#if 0
@item IBM6000_TARGET
Shows that we are configured for an IBM RS/6000 target.  This
conditional should be eliminated (FIXME) and replaced by
feature-specific macros.  It was introduced in haste and we are
repenting at leisure.
#endif


#if 0
@item IEEE_FLOAT
Define this if the target system uses IEEE-format floating point numbers.
#endif
int carp_ieee_float;
/* #define IEEE_FLOAT carp_ieee_float */


#if 0
@item INIT_EXTRA_FRAME_INFO (fromleaf, fci)
If defined, this should be a C expression or statement that fills in the
@code{EXTRA_FRAME_INFO} slots of the given frame @var{fci}.
#endif
void carp_init_extra_frame_info (CORE_ADDR fromleaf, struct frame_info *fci) { return; }
/* #define INIT_EXTRA_FRAME_INFO(fromleaf, fci) carp_init_extra_frame_info (fromleaf, fci) */



#if 0
@item INIT_FRAME_PC (fromleaf, prev)
This is a C statement that sets the pc of the frame pointed to by
@var{prev}.  [By default...]
#endif
void carp_init_frame_pc (CORE_ADDR fromleaf, struct frame_info *fci) { return; }
/* #define INIT_FRAME_PC(fromleaf, fci) carp_init_frame_pc (fromleaf, fci) */


#if 0
@item INNER_THAN
Define this to be either @code{<} if the targets stack grows downward
in memory, or @code{>} is the stack grows upwards.
#endif
/* #define INNER_THAN < */


#if 0
@item IN_SIGTRAMP (pc, name)
Define this to return true if the given pc and/or name indicates that
the current function is a sigtramp.
#endif
int carp_in_sigtramp (CORE_ADDR pc, char *name) { return 0; }
/* #define IN_SIGTRAMP(pc, name) carp_in_sigtramp (pc, name) */


#if 0
@item SIGTRAMP_START (pc)
@item SIGTRAMP_END (pc)
Define these to be the start and end address of the sigtramp for the
given pc.  On machines where the address is just a compile time
constant, the macro expansion will typically just ignore the supplied
pc.
#endif
CORE_ADDR carp_sigtramp_start (CORE_ADDR pc) { return 0; }
CORE_ADDR carp_sigtramp_end (CORE_ADDR pc) { return 0; }
/* #define SIGTRAMP_START(pc) carp_sigtramp_start (pc) */
/* #define SIGTRAMP_END(pc) carp_sigtramp_end (pc) */


#if 0
@item IN_SOLIB_TRAMPOLINE pc name
Define this to evaluate to nonzero if the program is stopped in the
trampoline that connects to a shared library.
#endif


#if 0
@item IS_TRAPPED_INTERNALVAR name
This is an ugly hook to allow the specification of special actions that
should occur as a side-effect of setting the value of a variable
internal to GDB.  Currently only used by the h8500.  Note that this
could be either a host or target conditional.
#endif
int carp_is_trapped_internalvar (char *name) { return 0; }
/* #define IS_TRAPPED_INTERNALVAR(name) carp_is_trapped_internalvar (name) */


#if 0
@item KERNEL_DEBUGGING
tm-ultra3.h
#endif


#if 0
@item MAX_REGISTER_RAW_SIZE
#endif
int carp_max_register_raw_size;
/* #define MAX_REGISTER_RAW_SIZE carp_max_register_raw_size */


#if 0
@item MAX_REGISTER_VIRTUAL_SIZE
#endif
int carp_max_register_virtual_size;
/* #define MAX_REGISTER_VIRTUAL_SIZE carp_max_register_virtual_size */


#if 0
@item MIPSEL
mips-tdep.c
#endif


#if 0
@item NEED_TEXT_START_END
Define this if GDB should determine the start and end addresses of the
text section.  (Seems dubious.)
#endif
int carp_need_text_start_end;
/* #define NEED_TEXT_START_END carp_need_text_start_end */


#if 0
@item NO_HIF_SUPPORT
(Specific to the a29k.)
#endif


#if 0
@item NO_SINGLE_STEP
Define this if the target does not support single-stepping.  If this is
defined, you must supply, in @code{*-tdep.c}, the function
@code{single_step}, which takes a target_signal as argument and returns
nothing.  It must insert breakpoints at each possible destinations of
the next instruction.  See @code{sparc-tdep.c} and @code{rs6000-tdep.c}
for examples.
#endif
int one_stepped;
void single_step (enum target_signal signal) { return; }
/* #define NO_SINGLE_STEP */


#if 0
@item NUM_REGS
#endif
int carp_num_regs;
/* #define NUM_REGS carp_num_regs */


#if 0
@item PCC_SOL_BROKEN
(Used only in the Convex target.)
#endif


#if 0
@item PC_IN_CALL_DUMMY
inferior.h
#endif
int carp_pc_in_call_dummy (CORE_ADDR pc, CORE_ADDR sp, CORE_ADDR fp) { return 0; }
/* #define PC_IN_CALL_DUMMY(pc, sp, fp) carp_pc_in_call_dummy (pc, sp, fp) */


#if 0
@item PC_LOAD_SEGMENT
If defined, print information about the load segment for the program
counter.  (Defined only for the RS/6000.)
#endif


#if 0
@item PC_REGNUM
If the program counter is kept in a register, then define this macro to
be the number of that register.  This need be defined only if
@code{TARGET_WRITE_PC} is not defined.
#endif
int carp_pc_regnum;
/* #define PC_REGNUM carp_pc_regnum */


#if 0
@item NPC_REGNUM
The number of the next program counter register, if defined.
#endif


#if 0
@item NNPC_REGNUM
The number of the next next program counter register, if defined.
Currently, this is only defined for the Motorola 88K.
#endif


#if 0
@item PRINT_REGISTER_HOOK (regno)
If defined, this must be a function that prints the contents of the
given register to standard output.
#endif


#if 0
@item PRINT_TYPELESS_INTEGER
This is an obscure substitute for @code{print_longest} that seems to
have been defined for the Convex target.
#endif


#if 0
@item PROCESS_LINENUMBER_HOOK
A hook defined for XCOFF reading.
#endif


#if 0
@item PROLOGUE_FIRSTLINE_OVERLAP
(Only used in unsupported Convex configuration.)
#endif


#if 0
@item PS_REGNUM
If defined, this is the number of the processor status register.  (This
definition is only used in generic code when parsing "$ps".)
#endif


#if 0
@item POP_FRAME
Used in @samp{call_function_by_hand} to remove an artificial stack
frame.
#endif
void carp_pop_frame (void) { return; }
/* #define POP_FRAME carp_pop_frame () */


#if 0
@item PUSH_ARGUMENTS (nargs, args, sp, struct_return, struct_addr)
Define this to push arguments onto the stack for inferior function call.
#endif
CORE_ADDR carp_push_arguments (int nargs, struct value **args, CORE_ADDR sp, unsigned char struct_return, CORE_ADDR struct_addr) { return 0; }
/* #define PUSH_ARGUMENTS(NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR) (SP) = carp_push_arguments (NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR) */


#if 0
@item PUSH_DUMMY_FRAME
Used in @samp{call_function_by_hand} to create an artificial stack frame.
#endif
void carp_push_dummy_frame (void) { return; }
/* #define PUSH_DUMMY_FRAME carp_push_dummy_frame () */


#if 0
@item REGISTER_BYTE
#endif
int carp_register_byte (int regnum) { return 0; }
#define REGISTER_BYTE(regnum) carp_register_byte (regnum)


#if 0
@item REGISTER_BYTES
The total amount of space needed to store GDBs copy of the machines
register state.
#endif
int carp_register_bytes;
/* #define REGISTER_BYTES carp_register_bytes */


#if 0
@item REGISTER_NAMES
Define this to expand into an initializer of an array of strings.  Each
string is the name of a register.
#endif
char **carp_register_names;
/* #define REGISTER_NAMES carp_register_names */


#if 0
@item REGISTER_RAW_SIZE
#endif
int carp_register_raw_size (int regnum) { return 0; }
/* #define REGISTER_RAW_SIZE(regnum) carp_register_raw_size (regnum) */


#if 0
@item REGISTER_SIZE
Size of a general purpose register?
#endif
int carp_register_size;
/* #define REGISTER_SIZE carp_register_size */


#if 0
@item REGISTER_VIRTUAL_SIZE
#endif
int carp_register_virtual_size (int regnum) { return 0; }
/* #define REGISTER_VIRTUAL_SIZE(regnum) carp_register_virtual_size (regnum) */


#if 0
@item REG_STRUCT_HAS_ADDR (gcc_p, type)
Define this to return 1 if the given type will be passed by pointer
rather than directly.
#endif
int carp_reg_struct_has_addr (int gcc_p, struct type *type) { return 0; }
/* #define REG_STRUCT_HAS_ADDR(gcc_p, type) carp_reg_struct_has_addr (gcc_p, type) */


#if 0
@item SAVED_PC_AFTER_CALL (frame)
#endif
CORE_ADDR carp_saved_pc_after_call (struct frame_info *frame) { return 0; }
/* #define SAVED_PC_AFTER_CALL(frame) carp_saved_pc_after_call (frame) */


#if 0
@item SDB_REG_TO_REGNUM (regnum)
Define this to convert sdb register numbers into GDB regnums.  If not
defined, no conversion will be done.
#endif
int carp_sdb_reg_to_regnum (int regnum) { return 0; }
/* #define SDB_REG_TO_REGNUM(regnum) carp_sdb_reg_to_regnum (regnum) */


#if 0
@item SET_TRAPPED_INTERNALVAR
#endif
void carp_set_trapped_internalvar (struct internalvar *var, struct value *newval, int bitpos, int bitsize, int offset) { return; }
/* #dfine SET_TRAPPED_INTERNALVAR(var, newval, bitpos, bitsize, offset) carp_set_trapped_internalvar (var, newval, bitpos, bitsize, offset) */


#if 0
@item SHIFT_INST_REGS
(Only used for m88k targets.)
#endif


#if 0
@item SKIP_PROLOGUE
A C statement that advances the PC across any function entry prologue
instructions so as to reach real code.
#endif
CORE_ADDR carp_skip_prologue (CORE_ADDR pc) { return 0; }
/* #define SKIP_PROLOGUE(pc) (pc) = carp_skip_prologue (pc) */


#if 0
@item SKIP_PROLOGUE_FRAMELESS_P
A C statement that should behave similarly, but that can stop as soon as
the function is known to have a frame.  If not defined,
@code{SKIP_PROLOGUE} will be used instead.
#endif
CORE_ADDR carp_skip_prologue_frameless_p (CORE_ADDR pc) { return 0; }
/* #define SKIP_PROLOGUE_FRAMELESS_P(pc) (pc) = carp_skip_prologue_frameless_p (pc) */


#if 0
@item SKIP_TRAMPOLINE_CODE (pc)
If the target machine has trampoline code that sits between callers and
the functions being called, then define this macro to return a new PC
that is at the start of the real function.
#endif
CORE_ADDR carp_skip_trampoline_code (CORE_ADDR pc) { return 0; }
/* #define SKIP_TRAMPOLINE_CODE(pc) (pc) = carp_skip_trampoline_code (pc) */


#if 0
@item SP_REGNUM
Define this to be the number of the register that serves as the stack
pointer.
#endif
int carp_sp_regnum;
/* #define SP_REGNUM carp_sp_regnum */


#if 0
@item STAB_REG_TO_REGNUM
Define this to convert stab register numbers (as gotten from r
declarations) into GDB regnums.  If not defined, no conversion will be
done.
#endif
int carp_stab_reg_to_regnum (int regnum) { return 0; }
/* #define STAB_REG_TO_REGNUM(regnum) carp_stab_reg_to_regnum (regnum) */


#if 0
@item STACK_ALIGN (addr)
Define this to adjust the address to the alignment required for the
processors stack.
#endif
CORE_ADDR carp_stack_align (CORE_ADDR pc) { return 0; }
/* #define STACK_ALIGN(pc) carp_stack_align (pc) */


#if 0
@item STEP_SKIPS_DELAY (addr)
Define this to return true if the address is of an instruction with a
delay slot.  If a breakpoint has been placed in the instructions delay
slot, GDB will single-step over that instruction before resuming
normally.  Currently only defined for the Mips.
#endif


#if 0
@item STORE_STRUCT_RETURN (type, valbuf)
A C expression that stores a function return value of type @var{type},
where @var{valbuf} is the address of the value to be stored.
#endif
void carp_store_struct_return (CORE_ADDR addr, CORE_ADDR sp) { return; }
/* #define STORE_STRUCT_RETURN(addr,sp) carp_store_struct_return (addr, sp) */


#if 0
@item STORE_RETURN_VALUE (type, valbuf)
A C expression that stores a function return value of type @var{type},
where @var{valbuf} is the address of the value to be stored.
#endif
void carp_store_return_value (struct type *type, char *valbuf) { return; }
/* #define STORE_RETURN_VALUE(type,valbuf) carp_store_return_value (type, valbuf) */


#if 0
@item SUN_FIXED_LBRAC_BUG
(Used only for Sun-3 and Sun-4 targets.)
#endif


#if 0
@item SYMBOL_RELOADING_DEFAULT
The default value of the symbol-reloading variable.  (Never defined in
current sources.)
#endif


#if 0
@item TARGET_BYTE_ORDER
The ordering of bytes in the target.  This must be defined to be either
@code{BIG_ENDIAN} or @code{LITTLE_ENDIAN}.
#endif
int carp_target_byte_order;
/* #define TARGET_BYTE_ORDER carp_target_byte_order */


#if 0
@item TARGET_CHAR_BIT
Number of bits in a char; defaults to 8.
#endif


#if 0
@item TARGET_COMPLEX_BIT
Number of bits in a complex number; defaults to @code{2 * TARGET_FLOAT_BIT}.
#endif
int carp_target_complex_bit;
/* #define TARGET_COMPLEX_BIT carp_target_complex_bit */


#if 0
@item TARGET_DOUBLE_BIT
Number of bits in a double float; defaults to @code{8 * TARGET_CHAR_BIT}.
#endif
int carp_target_double_bit;
/* #define TARGET_DOUBLE_BIT carp_target_double_bit */


#if 0
@item TARGET_DOUBLE_COMPLEX_BIT
Number of bits in a double complex; defaults to @code{2 * TARGET_DOUBLE_BIT}.
#endif
int carp_target_double_complex_bit;
/* #define TARGET_DOUBLE_COMPLEX_BIT carp_target_double_complex_bit */


#if 0
@item TARGET_FLOAT_BIT
Number of bits in a float; defaults to @code{4 * TARGET_CHAR_BIT}.
#endif
int carp_target_float_bit;
/* #define TARGET_FLOAT_BIT carp_target_float_bit */


#if 0
@item TARGET_INT_BIT
Number of bits in an integer; defaults to @code{4 * TARGET_CHAR_BIT}.
#endif
int carp_target_int_bit;
/* #define TARGET_INT_BIT carp_target_int_bit */


#if 0
@item TARGET_LONG_BIT
Number of bits in a long integer; defaults to @code{4 * TARGET_CHAR_BIT}.
#endif
int carp_target_long_bit;
/* #define TARGET_LONG_BIT carp_target_long_bit */


#if 0
@item TARGET_LONG_DOUBLE_BIT
Number of bits in a long double float;
defaults to @code{2 * TARGET_DOUBLE_BIT}.
#endif
int carp_target_long_double_bit;
/* #define TARGET_LONG_DOUBLE_BIT carp_target_long_double_bit */


#if 0
@item TARGET_LONG_LONG_BIT
Number of bits in a long long integer; defaults to @code{2 * TARGET_LONG_BIT}.
#endif
int carp_target_long_long_bit;
/* #define TARGET_LONG_LONG_BIT carp_target_long_long_bit */


#if 0
@item TARGET_PTR_BIT
Number of bits in a pointer; defaults to @code{TARGET_INT_BIT}.
#endif
int carp_target_ptr_bit;
/* #define TARGET_PTR_BIT carp_target_ptr_bit */


#if 0
@item TARGET_SHORT_BIT
Number of bits in a short integer; defaults to @code{2 * TARGET_CHAR_BIT}.
#endif
int carp_target_short_bit;
/* #define TARGET_SHORT_BIT carp_target_short_bit */


#if 0
@item TARGET_READ_PC
@item TARGET_WRITE_PC (val, pid)
@item TARGET_READ_SP
@item TARGET_WRITE_SP
@item TARGET_READ_FP
@item TARGET_WRITE_FP
These change the behavior of @code{read_pc}, @code{write_pc},
@code{read_sp}, @code{write_sp}, @code{read_fp} and @code{write_fp}.
For most targets, these may be left undefined.  GDB will call the read
and write register functions with the relevant @code{_REGNUM} argument.

These macros are useful when a target keeps one of these registers in a
hard to get at place; for example, part in a segment register and part
in an ordinary register.
#endif
CORE_ADDR carp_target_read_pc (int pid) { return 0; }
CORE_ADDR carp_target_read_sp (void) { return 0; }
CORE_ADDR carp_target_read_fp (void) { return 0; }
void carp_target_write_pc (CORE_ADDR pc, int pid) { return; }
void carp_target_write_sp (CORE_ADDR sp) { return; }
void carp_target_write_fp (CORE_ADDR fp) { return; }
/* #define TARGET_READ_PC(pid) carp_target_read_pc (pid) */
/* #define TARGET_WRITE_PC(val,pid) carp_target_write_pc (val, pid) */
/* #define TARGET_READ_SP() carp_target_read_sp () */
/* #define TARGET_WRITE_SP(val) carp_target_write_sp (val) */
/* #define TARGET_READ_FP() carp_target_read_fp () */
/* #define TARGET_WRITE_FP(val) carp_target_write_fp (val) */


#if 0
@item USE_STRUCT_CONVENTION (gcc_p, type)
If defined, this must be an expression that is nonzero if a value of the
given @var{type} being returned from a function must have space
allocated for it on the stack.  @var{gcc_p} is true if the function
being considered is known to have been compiled by GCC; this is helpful
for systems where GCC is known to use different calling convention than
other compilers.
#endif


#if 0
@item VALUE_OF_TRAPPED_INTERNALVAR
#endif
struct value *carp_value_of_trapped_internalvar (struct internalvar *var) { return 0; }
/* #define VALUE_OF_TRAPPED_INTERNALVAR(var) carp_value_of_trapped_internalvar (var) */


#if 0
@item VARIABLES_INSIDE_BLOCK (desc, gcc_p)
For dbx-style debugging information, if the compiler puts variable
declarations inside LBRAC/RBRAC blocks, this should be defined to be
nonzero.  @var{desc} is the value of @code{n_desc} from the
@code{N_RBRAC} symbol, and @var{gcc_p} is true if GDB has noticed the
presence of either the @code{GCC_COMPILED_SYMBOL} or the
@code{GCC2_COMPILED_SYMBOL}.  By default, this is 0.
#endif


#if 0
@item OS9K_VARIABLES_INSIDE_BLOCK (desc, gcc_p)
Similarly, for OS/9000.  Defaults to 1.
#endif
