#name: %pcrel_lo overflow with an addend
#source: pcrel-lo-addend-2.s
#as: -march=rv32ic
#ld: -melf32lriscv
#error: .*dangerous relocation: %pcrel_lo overflow with an addend
