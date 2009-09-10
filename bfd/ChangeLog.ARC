2008-11-12  Irfan Rafiq  <irfan.rafiq@arc.com>
        * elf32-arc.c (arc_elf_final_write_processing,
                       arc_elf_object_p): Sync with arch_info_struct        
        * cpu-arc.c (arch_info_struct): Update info for ARC600/ARC700.
        
        
2008-10-10  Irfan Rafiq  <irfan.rafiq@arc.com>
        
        *  bfd-in2.h  : Add relocation types.
        *  config.bfd : Add arc-linux-uclibc target.
        *  config.in  : Update version.
        *  libbfd.h   : Sync. relocation types.
        

2008-02-27  J"orn Rennecke  <joern.rennecke@arc.com>

	* elf32-arc.c (BFD_DEBUG_PIC): Simplify change between debug /
	non-debug mode by using #if instead of comments.
	* (arc_plugin_one_reloc) <R_ARC_PLT32>: Also support 'b' instruction.
	* (elf_arc_check_relocs): Use bfd_make_section_with_flags instead of
	separate bfd_make_section / bfd_set_section_flags for
	SEC_LINKER_CREATED sections.
	(elf_arc_relocate_section): Fix two printf format issue.
	(elf_arc_adjust_dynamic_symbol): Likewise.
	(elf_arc_size_dynamic_sections): Set SEC_EXCLUDE flag instead of
	manually removing the section from the list.

2007-10-31  J"orn Rennecke  <joern.rennecke@arc.com>

	* configure.in (AM_INIT_AUTOMAKE): Update date.
	* configure: Regenerate.
