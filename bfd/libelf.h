#include "elf/common.h"
#include "elf/internal.h"
#include "elf/external.h"

struct elf_backend_data {
  void (*elf_info_to_howto) PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));
  enum bfd_architecture arch;
};

extern bfd_target *elf_object_p PARAMS ((bfd *));
extern bfd_target *elf_core_file_p PARAMS ((bfd *));
extern boolean elf_mkobject PARAMS ((bfd *));
extern boolean elf_write_object_contents PARAMS ((bfd *));
extern char *elf_core_file_failing_command PARAMS ((bfd *));
extern int elf_core_file_failing_signal PARAMS ((bfd *));
extern boolean elf_core_file_matches_executable_p PARAMS ((bfd *, bfd *));
extern boolean elf_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						 file_ptr, bfd_size_type));
extern unsigned int elf_get_symtab_upper_bound PARAMS ((bfd *));
extern unsigned int elf_get_symtab PARAMS ((bfd *, asymbol **));
extern unsigned int elf_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
extern unsigned int elf_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
						    arelent **, asymbol **));
extern asymbol *elf_make_empty_symbol PARAMS ((bfd *));
extern void elf_print_symbol PARAMS ((bfd *, PTR, asymbol *,
				      bfd_print_symbol_type));
extern alent *elf_get_lineno PARAMS ((bfd *, asymbol *));
extern boolean elf_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
					  unsigned long));
extern boolean elf_find_nearest_line PARAMS ((bfd *, asection *, asymbol **,
					      bfd_vma, CONST char **,
					      CONST char **, unsigned int *));
extern int elf_sizeof_headers PARAMS ((bfd *, boolean));

/* If the target doesn't have reloc handling written yet:  */
extern void elf_no_info_to_howto PARAMS ((bfd *, arelent *,
					  Elf_Internal_Rela *));
