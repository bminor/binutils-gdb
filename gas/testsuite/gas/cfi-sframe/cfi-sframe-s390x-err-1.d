#name: SFrame generation on s390x - .cfi_def_cfa_register with non-SP/FP register
#as: --gsframe
#warning: non-SP/FP register 10 in \.cfi_def_cfa_register
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: SFRAME_F_FDE_FUNC_START_PCREL
    Num FDEs: 0
    Num FREs: 0

#pass
