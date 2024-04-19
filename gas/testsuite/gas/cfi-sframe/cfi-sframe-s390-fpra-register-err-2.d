#name: SFrame generation on s390 - FP and RA saved in register
#as: --gsframe
#warning: skipping SFrame FDE due to .cfi_register specifying RA register
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: NONE
    Num FDEs: 0
    Num FREs: 0

#pass
