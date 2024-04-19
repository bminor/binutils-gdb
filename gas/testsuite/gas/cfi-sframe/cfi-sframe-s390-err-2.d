#name: SFrame generation on s390 - non-SP/FP CFA base register
#as: --gsframe
#warning: skipping SFrame FDE due to .cfi_def_cfa defining non-SP/FP register 10 as CFA base register
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: NONE
    Num FDEs: 0
    Num FREs: 0

#pass
