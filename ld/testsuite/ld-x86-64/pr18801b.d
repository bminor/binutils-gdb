#source: pr18801.s
#as: --64
#ld: -melf_x86_64 -shared
#error: read-only segment has dynamic IFUNC relocations; recompile with -fPIC
