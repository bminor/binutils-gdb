_divert__(-1)

Switches:

_define__(<_ALL_ARCH__>,<0>)
_define__(<_INTERNALS__>,<0>)

_define__(<_AOUT__>,<1>)
_define__(<_BOUT__>,<0>)
_define__(<_COFF__>,<0>)
_define__(<_ELF__>,<0>)

_define__(<_AMD29K__>,<0>)
_define__(<_I80386__>,<0>)
_define__(<_I960__>,<0>)
_define__(<_M680X0__>,<0>)
_define__(<_SPARC__>,<0>)
_define__(<_VAX__>,<0>)

Text:

Default names for assembler, compiler, linker, debugger; 
	individual configs may override
_define__(<_AS__>,<as>)
_define__(<_GCC__>,<gcc>)
_define__(<_LD__>,<ld>)
_define__(<_GDB__>,<gdb>)

Text for host; individual configs *should* override, but this may
catch some flubs
_define__(<_HOST__>,<machine specific>)

"Machine Dependent" nodename
_define__(<_MACH_DEP__>,<Machine Dependent>)

_divert__<>