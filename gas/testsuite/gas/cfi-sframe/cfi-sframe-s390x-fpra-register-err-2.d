#name: SFrame generation on s390x - RA and then FP saved in register
#as: --gsframe
#warning: RA register 14 in .cfi_register
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: SFRAME_F_FDE_FUNC_START_PCREL
    Num FDEs: 0
    Num FREs: 0

#pass
