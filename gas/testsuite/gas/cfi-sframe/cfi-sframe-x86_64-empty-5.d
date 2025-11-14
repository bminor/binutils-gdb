#as: --gsframe
#warning: RA register 16 in \.cfi\_register
#objdump: --sframe=.sframe
#name: DW_CFA_register with RA register
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: SFRAME_F_FDE_FUNC_START_PCREL
#?    CFA fixed FP offset: \-?\d+
#?    CFA fixed RA offset: \-?\d+
    Num FDEs: 0
    Num FREs: 0

#pass
