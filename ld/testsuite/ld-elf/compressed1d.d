#source: compress1.s
#as: --compress-debug-sections=none
#ld: -r --compress-debug-sections=zlib-gnu
#readelf: -SW
#xfail: d30v-*-* dlx-*-* fr30-*-* frv-*-elf ft32-*-* iq2000-*-*
#xfail: mn10200-*-* moxie-*-* msp430-*-* mt-*-* pj-*-* riscv*-*-*
#xfail: xgate-*-*
# Not all ELF targets use the elf.em emulation...
# RISC-V has linker relaxations that delete code, so text label subtractions
# do not get resolved at assembly time, which results in a compressed section.

#failif
#...
  \[[ 0-9]+\] \.zdebug_aranges[ 	]+(PROGBITS|MIPS_DWARF)[ 	0-9a-z]+ .*
#...
