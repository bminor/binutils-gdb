#as: --gsframe
#warning: skipping SFrame FDE; CFI insn DW_CFA_def_cfa_offset \(0xe\)
#objdump: --sframe=.sframe
#name: SFrame supports only FP/SP based CFA
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
