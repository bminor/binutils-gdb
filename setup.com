$! setup files for openVMS/AXP
$!
$ copy [.bfd.hosts]alphavms.h [.bfd]sysdep.h
$ copy [.bfd.hosts]alphavms.h [.opcodes]sysdep.h
$ copy [.bfd]bfd.h-vms [.bfd]bfd.h
$ copy [.libiberty]alloca-norm.h [.libiberty]alloca-conf.h
$ copy [.binutils]config.h-vms [.binutils]config.h 
$ define aout [-.INCLUDE.AOUT]
$ define coff [-.INCLUDE.COFF]
$ define elf [-.INCLUDE.ELF]
$ define mpw [-.INCLUDE.MPW]
$ define nlm [-.INCLUDE.NLM]
$ define opcode [-.INCLUDE.OPCODE]
