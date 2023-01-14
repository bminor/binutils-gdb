#name: %pcrel_lo overflow with an addend
#source: pcrel-lo-addend-2.s
#as: -march=rv32ic
#ld: -m[riscv_choose_ilp32_emul] --no-relax
#error: .*dangerous relocation: %pcrel_lo overflow with an addend
