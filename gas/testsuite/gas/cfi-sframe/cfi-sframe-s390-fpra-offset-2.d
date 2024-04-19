#objdump: --sframe=.sframe
#name: SFrame generation on s390 - FP and RA saved on stack
#...
Contents of the SFrame section .sframe:

  Header :

    Version: SFRAME_VERSION_2
    Flags: NONE
    Num FDEs: 1
    Num FREs: 5

  Function Index :

    func idx \[0\]: pc = 0x0, size = 34 bytes
    STARTPC +CFA +FP +RA +
    0+0000 +sp\+160 +u +u +
    0+0006 +sp\+160 +c\-72 +u\* +
    0+000c +sp\+160 +c\-72 +c\-48 +
    0+001a +sp\+160 +c-72 +u\* +
    0+0020 +sp\+160 +u +u +
#pass
