#as: --gsframe
#warning: skipping SFrame FDE due to \.cfi_remember_state without SFrame FRE state
#objdump: --sframe=.sframe
#name: Uninteresting cfi directives generate an empty SFrame section
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
