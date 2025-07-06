#name: R_MIPS_HI16 and R_MIPS_LO16 relocs n32
#source: ../../../gas/testsuite/gas/mips/mips-hilo.s
#source: mips-hilo.s
#objdump: -d
#ld: -Tmips-hilo.ld -e 0x500000 -N
#dump: mips-hilo.d
