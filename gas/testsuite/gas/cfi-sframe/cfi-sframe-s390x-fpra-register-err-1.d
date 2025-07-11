#name: SFrame generation on s390x - FP without RA saved in registers
#as: --gsframe
#warning: FP without RA on stack
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: SFRAME_F_FDE_FUNC_START_PCREL
    Num FDEs: 0
    Num FREs: 0

#pass
