/* hi */

#include "emul.h"
#include "targ-cpu.h"

#define OUTPUT_FLAVOR					\
	(this_format->flavor)

#define obj_frob_symbol(S,P)				\
	(this_format->frob_symbol) (S, &(P))

#define obj_frob_file()					\
	(this_format->frob_file				\
	 ? this_format->frob_file ()			\
	 : (void) 0)

#define obj_frob_file_after_relocs			\
	(this_format->frob_file_after_relocs)

#define obj_ecoff_set_ext				\
	(this_format->ecoff_set_ext)

#define obj_pop_insert					\
	(this_format->pop_insert)

#define obj_read_begin_hook()				\
	(this_format->read_begin_hook			\
	 ? this_format->read_begin_hook ()		\
	 : (void) 0)

#define obj_symbol_new_hook				\
	(this_format->symbol_new_hook)

#define obj_sec_sym_ok_for_reloc			\
	(this_format->sec_sym_ok_for_reloc)

#define S_GET_SIZE					\
	(this_format->s_get_size)

#define S_SET_SIZE					\
	(this_format->s_set_size)

#define S_GET_ALIGN					\
	(this_format->s_get_align)

#define S_SET_ALIGN					\
	(this_format->s_set_align)

#define OBJ_COPY_SYMBOL_ATTRIBUTES(d,s)			\
	(this_format->copy_symbol_attributes		\
	 ? this_format->copy_symbol_attributes (d, s)	\
	 : (void) 0)

#define OBJ_PROCESS_STAB(SEG,W,S,T,O,D)			\
	(this_format->process_stab			\
	 ? this_format->process_stab (SEG,W,S,T,O,D)	\
	 : (void) 0)

#if defined (OBJ_MAYBE_ECOFF) || (defined (OBJ_MAYBE_ELF) && defined (TC_MIPS))
#define ECOFF_DEBUGGING 1
#endif

#ifdef OBJ_MAYBE_ELF
/* We need obj-elf for OBJ_SYMFIELD_TYPE so that symbol_get_obj is defined */
#include "obj-elf.h"
#endif
