#name: MIPS16e2 R_MIPS16_HI16 and R_MIPS16_LO16 relocs n32
#source: ../../../gas/testsuite/gas/mips/mips-hilo.s
#source: mips-hilo.s
#as: -mips16 -mmips16e2 -march=mips64r2
#objdump: -d
#ld: -Tmips-hilo.ld -e 0x500000 -N
#notarget: mips*el-ps2-elf*
#dump: mips16e2-hilo.d
