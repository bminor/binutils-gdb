#name: MIPS PIC relocation 5
#ld: -shared -T pic-reloc-ordinary.ld
#target: [check_shared_lib_support]
#error: \A[^\n]*: in function `foo':\n
#error:   \(\.text\+0x0\): relocation R_(MICRO|)MIPS_HI16 against `a local symbol' cannot be used when making a shared object; recompile with -fPIC\n
#error:   \(\.text\+0x8\): relocation R_(MICRO|)MIPS_HI16 against `bar' cannot be used when making a shared object; recompile with -fPIC\Z
