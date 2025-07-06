#name: R_MIPS16_HI16 and R_MIPS16_LO16 relocs n32
#source: ../../../gas/testsuite/gas/mips/mips16-hilo.s
#source: mips16-hilo.s
#as: -march=mips3
#objdump: -mmips:16 --prefix-addresses -dr --show-raw-insn
#ld: -Tmips16-hilo.ld -e 0x500000 -N
#dump: mips16-hilo.d
