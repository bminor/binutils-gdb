#ifndef TM_PPC64LINUX_H
#define TM_PPC64LINUX_H

#include "rs6000/tm-rs6000.h"

/* Avoid warning from redefinition in tm-sysv4.h (included from tm-linux.h) */
#undef SKIP_TRAMPOLINE_CODE
#include "tm-linux.h"

#include "gdbarch.h"
/* Reset these back to the default.  Should really fix this for other
   powerpc tm.h so it can be chosen at runtime  */
#undef SOFTWARE_SINGLE_STEP_P
#define SOFTWARE_SINGLE_STEP_P() (gdbarch_software_single_step_p (current_gdbarch))
#undef SOFTWARE_SINGLE_STEP
#define SOFTWARE_SINGLE_STEP(sig, insert_breakpoints_p) (gdbarch_software_single_step (current_gdbarch, sig, insert_breakpoints_p))

#ifdef DEAD_CODE
/* FIXME: tm-linux defines this which a dead interface, as far as I
   can tell. The macro is unused and there is no definition for the
   function the macro calls. It is here so we know to get rid of it
   there. */
/* Make sure nexti gets the help it needs for debugging assembly code
   without symbols */

#define AT_SUBROUTINE_CALL_INSTRUCTION_TARGET(prevpc,stoppc) \
   at_subroutine_call_instruction_target(prevpc,stoppc)
extern int at_subroutine_call_instruction_target();
#endif /* DEAD_CODE */

/* We _want_ the SVR4 section offset calculations (see syms_from_objfile()
   in symfile.c) */
#undef IBM6000_TARGET

/* Default offset from SP where the LR is stored */
#undef DEFAULT_LR_SAVE
#define	DEFAULT_LR_SAVE 16

/* Say that we're using ELF, not XCOFF.  */
#define ELF_OBJECT_FORMAT 1

/* Say that we're using ELF64 since ABI is closer to XCOFF64. */
#define ELF64_OBJECT_FORMAT 1

#undef PROCESS_LINENUMBER_HOOK

#undef TEXT_SEGMENT_BASE
#define TEXT_SEGMENT_BASE 1

/* don't think it is but it can't hurt (can it?) */
#define TARGET_BYTE_ORDER_SELECTABLE_P 1

/* The value of symbols of type N_SO and N_FUN maybe null when 
   it shouldn't be. */
#define SOFUN_ADDRESS_MAYBE_MISSING

#undef CHILD_SPECIAL_WAITSTATUS

/* Use generic shared library machinery.  */
#include "solib.h"

/* function entrypoints have a '.' prepended to them (elfread.c) */
#define DROP_TEXT_NAME_PREFIX_CHAR '.'
/* skip data symbols if they are from the .opd section (elfread.c) */
#define SKIP_DATA_IN_OPD

/* define our own child_xfer_memory() ppc-linux-nat.c */
#define CHILD_XFER_MEMORY

#undef bfd_get_start_address
#define bfd_get_start_address(abfd) ppc64_bfd_get_start_address (abfd)
extern  CORE_ADDR ppc64_bfd_get_start_address (bfd *);
struct link_map_offsets *ppc64_linux_svr4_fetch_link_map_offsets (void);
extern CORE_ADDR ppc64_linux_convert_from_func_ptr_addr (CORE_ADDR addr);

/* define additional name for solib functions (solib-svr4.c) to hinge on. */
#define SOLIB_BREAK_NAME "._dl_debug_state"


#endif  /* #ifndef TM_PPC64LINUX_H */
