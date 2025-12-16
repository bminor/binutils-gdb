#name: SFrame generation on s390x - .cfi_register with SP
#as: --gsframe
#warning: SP register 15 in \.cfi\_register
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_3
    Flags: SFRAME_F_FDE_FUNC_START_PCREL
    Num FDEs: 0
    Num FREs: 0

#pass
