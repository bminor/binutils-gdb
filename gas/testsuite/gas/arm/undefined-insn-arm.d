#name: Undefined binary printing in arm mode
#skip: *-unknown-pe *-*-vxworks
#source: undefined-insn.s
#objdump: -D -b binary -m armv5te
#...
