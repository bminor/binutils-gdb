.text
__start:
	# Use a GOT relocation because the linker currently doesn't do the
	# padding and alignment unless we have GOT relocations.  This is
	# another linker bug that we need to fix.
        adrp  c0, :got:__data_rel_ro_startsym
	ret
.zero 0x100
.byte 1
.section .data.rel.ro,"aw",@progbits
# We use a linker defined symbol that points into .data.rel.ro so that the
# linker will need to ensure this section is aligned and padded so that its
# bounds can be precisely represented by a capability.  This is done in the
# linker script.
#
# Here we ensure that the size of the section is large enough that it will need
# padding.
.zero 0x100001
.section .got,"aw",@progbits
.capinit __data_rel_ro_startsym
.xword 0
.xword 0
