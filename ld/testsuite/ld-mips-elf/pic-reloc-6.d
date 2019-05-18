#name: MIPS PIC relocation 6 (MIPS16)
#ld: -shared -T pic-reloc-ordinary.ld
#target: [check_shared_lib_support]
#as: -mips16
#error: \A[^\n]*: in function `foo':\n
#error:   \(\.text\+0x0\): relocation R_MIPS16_HI16 against `a local symbol' cannot be used when making a shared object; recompile with -fPIC\n
#error:   \(\.text\+0x8\): relocation R_MIPS16_HI16 against `bar' cannot be used when making a shared object; recompile with -fPIC\Z
