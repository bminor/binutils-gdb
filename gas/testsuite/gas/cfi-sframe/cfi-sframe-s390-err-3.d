#name: SFrame generation on s390 - non-default RA register
#as: --gsframe
#warning: skipping SFrame FDE due to non-default return-address register 7
#objdump: --sframe=.sframe
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: NONE
    Num FDEs: 0
    Num FREs: 0

#pass
