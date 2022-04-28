# The emit-relocs-28 test was creating space for unnecessary relocations and
# correspondingly adding the __rela_dyn_{start,end} symbols to span them.
# This was happening because of Morello changes which applied on non-Morello
# links by mistake.  This testcase is to ensure that that does not happen.
#source: emit-relocs-28.s
#as: -mabi=ilp32
#ld: -m [aarch64_choose_ilp32_emul] --defsym globala=0x11000 --defsym globalb=0x45000 --defsym globalc=0x1234  -e0 --emit-relocs
#readelf: --symbols

#failif
#...
.* __rela_dyn_start
#...
