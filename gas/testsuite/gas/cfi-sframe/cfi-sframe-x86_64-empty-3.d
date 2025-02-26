#as: --gsframe
#warning: skipping SFrame FDE; \.cfi\_escape with op \(0x14\)
#objdump: --sframe=.sframe
#name: CFI_escape with multiple DWARF expr
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: NONE
#?    CFA fixed FP offset: \-?\d+
#?    CFA fixed RA offset: \-?\d+
    Num FDEs: 0
    Num FREs: 0

#pass
