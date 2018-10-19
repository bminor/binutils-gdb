#name: Undefined binary printing in thumb mode
#skip: *-unknown-pe *-*-vxworks
#source: undefined-insn.s
#objdump: -D -b binary -m armv7e-m -M force-thumb
#...
