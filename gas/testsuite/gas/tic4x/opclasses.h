/* Opcode infix
   B  condition             16--20   U,C,Z,LO,HI, etc.
   C  condition             23--27   U,C,Z,LO,HI, etc.

   Arguments
   ,  required arg follows
   ;  optional arg follows

   General addressing modes
   *  indirect               0--15   *+AR0(5), *++AR0(IR0)
   #  direct (for ldp only)  0--15   @start, start
   @  direct                 0--15   @start, start
   F  short float immediate  0--15   3.5, 0e-3.5e-1
   Q  register               0--15   R0, AR0, DP, SP
   R  register              16--20   R0, AR0, DP, SP
   S  short int immediate    0--15   -5, 5
   D  src and dst same reg

   Three operand addressing modes
   E  register               0--7    R0, R7, R11
   G  register               8--15   R0, R7, R11
   I  indirect(short)        0--7    *+AR0(1), *+AR0(IR0)
   J  indirect(short)        8--15   *+AR0(1), *+AR0(IR0)
   R  register              16--20   R0, R7, R11
   W  short int (C4x)        0--7    -3, 5
   C  indirect(short) (C4x)  0--7    *+AR0(5)
   O  indirect(short) (C4x)  8--15   *+AR0(5)

   Parallel instruction addressing modes
   E  register               0--7    R0, R7, R11
   G  register               8--15   R0, R7, R11
   H  register              18--16   R0, R7
   I  indirect(short)        0--7    *+AR0(1), *+AR0(IR0)
   J  indirect(short)        8--15   *+AR0(1), *+AR0(IR0)
   K  register              19--21   R0, R7
   L  register              22--24   R0, R7
   M  register (R2,R3)      22--22   R2, R3
   N  register (R0,R1)      23--23   R0, R1

   Misc. addressing modes
   A  address register      22--24   AR0, AR7
   B  unsigned integer       0--23   @start, start  (absolute on C3x, relative on C4x)
   P  displacement (PC Rel)  0--15   @start, start
   U  unsigned integer       0--15   0, 65535
   V  vector (C4x: 0--8)     0--4    25, 7
   T  integer (C4x)         16--20   -5, 12
   Y  address reg (C4x)     16--20   AR0, DP, SP, IR0
   X  expansion reg (C4x)    0--4    IVTP, TVTP
   Z  expansion reg (C4x)   16--20   IVTP, TVTP
*/

/* A: General 2-operand integer operations
   Syntax: <i> src, dst
      src = Register (Q), Direct (@), Indirect (*), Signed immediate (S)
      dst = Register (R)
   Instr: 15/8 - ABSI, ADDC, ADDI, ASH, CMPI, LDI, LSH, MPYI, NEGB, NEGI,
                SUBB, SUBC, SUBI, SUBRB, SUBRI, C4x: LBn, LHn, LWLn, LWRn,
                MBn, MHn, MPYSHI, MPYUHI
*/
#define A_CLASS(name) \
name##_A:                         &\
  name  AR1, AR0        /* Q;R */ &\
  name  AR0             /* Q;R */ &\
  name  @start, AR0     /* @,R */ &\
  name  *+AR0(5), AR0   /* *,R */ &\
  name  -5, AR0         /* S,R */


/* AB: General 2-operand integer operation with condition
   Syntax: <i>c src, dst
       c   = Condition
       src = Register (Q), Direct (@), Indirect (*), Signed immediate (S)
       dst = Register (R)
   Instr: 1/0 - LDIc
*/
#define AB_CLASS(name) \
name##_AB:                        &\
  name  AR1, AR0        /* Q;R */ &\
  name  AR0             /* Q;R */ &\
  name  @start, AR0     /* @,R */ &\
  name  *+AR0(5), AR0   /* *,R */ &\
  name  -5, AR0         /* S,R */


/* AU: General 2-operand unsigned integer operation
   Syntax: <i> src, dst
        src = Register (Q), Direct (@), Indirect (*), Unsigned immediate (U)
        dst = Register (R)
   Instr: 6/2 - AND, ANDN, NOT, OR, TSTB, XOR, C4x: LBUn, LHUn
*/
#define AU_CLASS(name) \
name##_AU:                        &\
  name  AR1, AR0        /* Q;R */ &\
  name  AR0             /* Q;R */ &\
  name  @start, AR0     /* @,R */ &\
  name  *+AR0(5), AR0   /* *,R */ &\
  name  5, AR0          /* U,R */


/* AF: General 2-operand float to integer operation
   Syntax: <i> src, dst
        src = Register 0-11 (Q), Direct (@), Indirect (*), Float immediate (F)
        dst = Register (R)
   Instr: 1/0 - FIX
*/
#define AF_CLASS(name) \
name##_AF:                        &\
  name  R1, R0          /* Q;R */ &\
  name  R0              /* Q;R */ &\
  name  @start, AR0     /* @,R */ &\
  name  *+AR0(5), AR0   /* *,R */ &\
  name  3.5, AR0        /* F,R */


/* A2: Limited 1-operand (integer) operation
   Syntax: <i> src
       src = Register (Q), Indirect (*), None
   Instr: 1/0 - NOP
*/
#define A2_CLASS(name) \
name##_A2:                        &\
  name  AR0               /* Q */ &\
  name  *+AR0(5)          /* * */ &\
  name                    /*   */


/* A3: General 1-operand unsigned integer operation
   Syntax: <i> src
        src = Register (Q), Direct (@), Indirect (*), Unsigned immediate (U)
   Instr: 1/0 - RPTS
*/
#define A3_CLASS(name) \
name##_A3:                        &\
  name  AR1               /* Q */ &\
  name  @start            /* @ */ &\
  name  *+AR0(5)          /* * */ &\
  name  5                 /* U */


/* A6: Limited 2-operand integer operation
   Syntax: <i> src, dst
       src = Direct (@), Indirect (*)
       dst = Register (R)
   Instr: 1/1 - LDII, C4x: SIGI
*/
#define A6_CLASS(name) \
name##_A6:                        &\
  name  @start, AR0     /* @,R */ &\
  name  *+AR0(5), AR0   /* *,R */


/* A7: Limited 2-operand integer store operation
   Syntax: <i> src, dst
       src = Register (R)
       dst = Direct (@), Indirect (*)
   Instr: 2/0 - STI, STII
*/
#define A7_CLASS(name) \
name##_A7:                        &\
  name  AR0, @start     /* R,@ */ &\
  name  AR0, *+AR0(5)   /* R,* */


/* AY: General 2-operand signed address load operation
   Syntax: <i> src, dst
        src = Register (Q), Direct (@), Indirect (*), Signed immediate (S)
        dst = Address register - ARx, IRx, DP, BK, SP (Y)
   Instr: 0/1 - C4x: LDA
   Note: Q and Y should *never* be the same register
*/
#define AY_CLASS(name) \
name##_AY:                        &\
  name  AR1, AR0        /* Q,Y */ &\
  name  @start, AR0     /* @,Y */ &\
  name  *+AR0(5), AR0   /* *,Y */ &\
  name  -5, AR0         /* S,Y */


/* B: General 2-operand float operation
   Syntax: <i> src, dst
       src = Register 0-11 (Q), Direct (@), Indirect (*), Float immediate (F)
       dst = Register 0-11 (R)
   Instr: 12/2 - ABSF, ADDF, CMPF, LDE, LDF, LDM, MPYF, NEGF, NORM, RND,
                 SUBF, SUBRF, C4x: RSQRF, TOIEEE
*/
#define B_CLASS(name) \
name##_B:                         &\
  name  R1, R0          /* Q;R */ &\
  name  R0              /* Q;R */ &\
  name  @start, R0      /* @,R */ &\
  name  *+AR0(5), R0    /* *,R */ &\
  name  3.5, R0         /* F,R */


/* BA: General 2-operand integer to float operation
   Syntax: <i> src, dst
       src = Register (Q), Direct (@), Indirect (*), Float immediate (F)
       dst = Register 0-11 (R)
   Instr: 0/1 - C4x: CRCPF
*/
#define BA_CLASS(name) \
name##_BA:                        &\
  name  AR1, R0         /* Q;R */ &\
  name  R0              /* Q;R */ &\
  name  @start, R0      /* @,R */ &\
  name  *+AR0(5), R0    /* *,R */ &\
  name  3.5, R0         /* F,R */


/* BB: General 2-operand conditional float operation
   Syntax: <i>c src, dst
       c   = Condition
       src = Register 0-11 (Q), Direct (@), Indirect (*), Float immediate (F)
       dst = Register 0-11 (R)
   Instr: 1/0 - LDFc
*/
#define BB_CLASS(name) \
name##_BB:                        &\
  name  R1, R0          /* Q;R */ &\
  name  R0              /* Q;R */ &\
  name  @start, R0      /* @,R */ &\
  name  *+AR0(5), R0    /* *,R */ &\
  name  3.5, R0         /* F,R */


/* BI: General 2-operand integer to float operation (yet different to BA)
   Syntax: <i> src, dst
       src = Register (Q), Direct (@), Indirect (*), Signed immediate (S)
       dst = Register 0-11 (R)
   Instr: 1/0 - FLOAT
*/
#define BI_CLASS(name) \
name##_BI:                        &\
  name  AR1, R0         /* Q;R */ &\
  name  R0              /* Q;R */ &\
  name  @start, R0      /* @,R */ &\
  name  *+AR0(5), R0    /* *,R */ &\
  name  -5, R0          /* S,R */


/* B6: Limited 2-operand float operation 
   Syntax: <i> src, dst
       src = Direct (@), Indirect (*)
       dst = Register 0-11 (R)
   Instr: 1/1 - LDFI, C4x: FRIEEE
*/
#define B6_CLASS(name) \
name##_B6:                        &\
  name  @start, R0      /* @,R */ &\
  name  *+AR0(5), R0    /* *,R */


/* B7: Limited 2-operand float store operation
   Syntax: <i> src, dst
       src = Register 0-11 (R)
       dst = Direct (@), Indirect (*)
   Instr: 2/0 - STF, STFI
*/
#define B7_CLASS(name) \
name##_B7:                        &\
  name  R0, @start      /* R,@ */ &\
  name  R0, *+AR0(5)    /* R,* */


/* D: Decrement and brach operations
   Syntax: <i>c ARn, dst
       c   = condition
       ARn = AR register 0-11 (A)
       dst = Register (Q), PC-relative (P)
   Instr: 2/0 - DBc, DBcD
   Alias: <name1> <name2>
*/
#define D_CLASS(name1, name2) \
name1##_D:                      &\
  name1  AR0, R0      /* A,Q */ &\
  name1  AR0, start   /* A,P */ &\
name2##_D:                      &\
  name2  AR0, R0      /* A,Q */ &\
  name2  AR0, start   /* A,P */


/* J: General conditional branch operations
   Syntax: <i>c dst
       c   = Condition
       dst = Register (Q), PC-relative (P)
   Instr: 2/3 - Bc, BcD, C4x: BcAF, BcAT, LAJc
   Alias: <name1> <name2>
*/
#define J_CLASS(name1, name2) \
name1##_J:                &\
  name1  R0       /* Q */ &\
  name1  start    /* P */ &\
name2##_J:                &\
  name2  R0       /* Q */ &\
  name2  start    /* P */


/* LL: Load-load parallell operation
   Syntax: <i> src2, dst2 || <i> src1, dst1
       src1 = Indirect 0,1,IR0,IR1 (J)
       dst1 = Register 0-7 (K)
       src2 = Indirect 0,1,IR0,IR1 (I)
       dst2 = Register 0-7 (L)
   Instr: 2/0 - LDF||LDF, LDI||LDI
   Alias: i||i, i1||i2, i2||i1
*/
#define LL_CLASS(name) \
name##_LL:                                                          &\
  name     *+AR0(1), R0  &||  name     *+AR1(1), R1   /* I,L|J,K */ &\
  name##2  *+AR0(1), R0  &||  name##1  *+AR1(1), R1   /* I,L|J,K */ &\
  name##1  *+AR1(1), R1  &||  name##2  *+AR0(1), R0   /* J,K|I,L */


/* LS: Store-store parallell operation
   Syntax: <i> src2, dst2 || <i> src1, dst1
       src1 = Register 0-7 (H)
       dst1 = Indirect 0,1,IR0,IR1 (J)
       src2 = Register 0-7 (L)
       dst2 = Indirect 0,1,IR0,IR1 (I)
   Instr: 2/0 - STF||STF, STI||STI
   Alias: i||i, i1||i2, i2||i1.
*/
#define LS_CLASS(name) \
name##_LS:                                                          &\
  name     R0, *+AR0(1)  &||  name     R1, *+AR1(1)   /* L,I|H,J */ &\
  name##2  R0, *+AR0(1)  &||  name##1  R1, *+AR1(1)   /* L,I|H,J */ &\
  name##1  R1, *+AR1(1)  &||  name##2  R0, *+AR0(1)   /* H,J|L,I */


/* M: General multiply and add/sub operations
   Syntax: <ia> src3,src4,dst1 || <ib> src2,src1,dst2 [00] - Manual
           <ia> src3,src1,dst1 || <ib> src2,src4,dst2 [01] - Manual
           <ia> src1,src3,dst1 || <ib> src2,src4,dst2 [01]
           <ia> src1,src2,dst1 || <ib> src4,src3,dst2 [02] - Manual
           <ia> src3,src1,dst1 || <ib> src4,src2,dst2 [03] - Manual
           <ia> src1,src3,dst1 || <ib> src4,src2,dst2 [03]
       src1 = Register 0-7 (K)
       src2 = Register 0-7 (H)
       src3 = Indirect 0,1,IR0,IR1 (J)
       src4 = Indirect 0,1,IR0,IR1 (I)
       dst1 = Register 0-1 (N)
       dst2 = Register 2-3 (M)
   Instr: 4/0 - MPYF3||ADDF3, MPYF3||SUBF3, MPYI3||ADDI3, MPYI3||SUBI3
   Alias: a||b, a3||n, a||b3, a3||b3, b||a, b3||a, b||a3, b3||a3
*/
#define M_CLASS(namea, nameb) \
namea##_##nameb##_M:                                                                          &\
  namea     *+AR0(1), *+AR1(1), R0  &||  nameb     R0, R1, R2               /* I,J,N|H,K;M */ &\
  namea     *+AR0(1), *+AR1(1), R0  &||  nameb     R0, R2                   /* I,J,N|H,K;M */ &\
  namea     *+AR0(1), R0, R0        &||  nameb     R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea     *+AR0(1), R0            &||  nameb     R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea     R0, *+AR0(1), R0        &||  nameb     R0, *+AR1(1), R2         /* K,J,N|H,I,M */ &\
  namea     R2, R1, R0              &||  nameb     *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea     R2, R0                  &||  nameb     *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea     *+AR0(1), R1, R0        &||  nameb     *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea     *+AR0(1), R0            &||  nameb     *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea     *+AR0(1), R1, R0        &||  nameb     *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea     *+AR0(1), R0            &||  nameb     *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea     R0, *+AR0(1), R0        &||  nameb     *+AR1(1), R0, R2         /* K,J,N|I,H;M */ &\
  namea     R0, *+AR0(1), R0        &||  nameb     *+AR1(1), R2             /* K,J,N|I,H;M */ &\
namea##3_##nameb##_M:                                                                         &\
  namea##3  *+AR0(1), *+AR1(1), R0  &||  nameb     R0, R1, R2               /* I,J,N|H,K;M */ &\
  namea##3  *+AR0(1), *+AR1(1), R0  &||  nameb     R0, R2                   /* I,J,N|H,K;M */ &\
  namea##3  *+AR0(1), R0, R0        &||  nameb     R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea##3  *+AR0(1), R0            &||  nameb     R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea##3  R0, *+AR0(1), R0        &||  nameb     R0, *+AR1(1), R2         /* K,J,N|H,I,M */ &\
  namea##3  R2, R1, R0              &||  nameb     *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea##3  R2, R0                  &||  nameb     *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea##3  *+AR0(1), R1, R0        &||  nameb     *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea##3  *+AR0(1), R0            &||  nameb     *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea##3  *+AR0(1), R1, R0        &||  nameb     *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea##3  *+AR0(1), R0            &||  nameb     *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea##3  R0, *+AR0(1), R0        &||  nameb     *+AR1(1), R0, R2         /* K,J,N|I,H;M */ &\
  namea##3  R0, *+AR0(1), R0        &||  nameb     *+AR1(1), R2             /* K,J,N|I,H;M */ &\
namea##_##nameb##3_M:                                                                         &\
  namea     *+AR0(1), *+AR1(1), R0  &||  nameb##3  R0, R1, R2               /* I,J,N|H,K;M */ &\
  namea     *+AR0(1), *+AR1(1), R0  &||  nameb##3  R0, R2                   /* I,J,N|H,K;M */ &\
  namea     *+AR0(1), R0, R0        &||  nameb##3  R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea     *+AR0(1), R0            &||  nameb##3  R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea     R0, *+AR0(1), R0        &||  nameb##3  R0, *+AR1(1), R2         /* K,J,N|H,I,M */ &\
  namea     R2, R1, R0              &||  nameb##3  *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea     R2, R0                  &||  nameb##3  *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea     *+AR0(1), R1, R0        &||  nameb##3  *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea     *+AR0(1), R0            &||  nameb##3  *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea     *+AR0(1), R1, R0        &||  nameb##3  *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea     *+AR0(1), R0            &||  nameb##3  *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea     R0, *+AR0(1), R0        &||  nameb##3  *+AR1(1), R0, R2         /* K,J,N|I,H;M */ &\
  namea     R0, *+AR0(1), R0        &||  nameb##3  *+AR1(1), R2             /* K,J,N|I,H;M */ &\
namea##3_##nameb##3_M:                                                                        &\
  namea##3  *+AR0(1), *+AR1(1), R0  &||  nameb##3  R0, R1, R2               /* I,J,N|H,K;M */ &\
  namea##3  *+AR0(1), *+AR1(1), R0  &||  nameb##3  R0, R2                   /* I,J,N|H,K;M */ &\
  namea##3  *+AR0(1), R0, R0        &||  nameb##3  R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea##3  *+AR0(1), R0            &||  nameb##3  R0, *+AR1(1), R2         /* J,K;N|H,I,M */ &\
  namea##3  R0, *+AR0(1), R0        &||  nameb##3  R0, *+AR1(1), R2         /* K,J,N|H,I,M */ &\
  namea##3  R2, R1, R0              &||  nameb##3  *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea##3  R2, R0                  &||  nameb##3  *+AR0(1), *+AR1(1), R2   /* H,K;N|I,J,M */ &\
  namea##3  *+AR0(1), R1, R0        &||  nameb##3  *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea##3  *+AR0(1), R0            &||  nameb##3  *+AR1(1), R3, R2         /* J,K;N|I,H;M */ &\
  namea##3  *+AR0(1), R1, R0        &||  nameb##3  *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea##3  *+AR0(1), R0            &||  nameb##3  *+AR1(1), R2             /* J,K;N|I,H;M */ &\
  namea##3  R0, *+AR0(1), R0        &||  nameb##3  *+AR1(1), R0, R2         /* K,J,N|I,H;M */ &\
  namea##3  R0, *+AR0(1), R0        &||  nameb##3  *+AR1(1), R2             /* K,J,N|I,H;M */ &\
nameb##_##namea##_M:                                                                          &\
  nameb     R0, R1, R2              &||  namea     *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb     R0, R2                  &||  namea     *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb     R0, *+AR1(1), R2        &||  namea     *+AR0(1), R0, R0         /* H,I,M|J,K;N */ &\
  nameb     R0, *+AR1(1), R2        &||  namea     *+AR0(1), R0             /* H,I,M|J,K;N */ &\
  nameb     R0, *+AR1(1), R2        &||  namea     R0, *+AR0(1), R0         /* H,I,M|K,J,N */ &\
  nameb     *+AR0(1), *+AR1(1), R2  &||  namea     R2, R1, R0               /* I,J,M|H,K;N */ &\
  nameb     *+AR0(1), *+AR1(1), R2  &||  namea     R2, R0                   /* I,J,M|H,K;N */ &\
  nameb     *+AR1(1), R3, R2        &||  namea     *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R3, R2        &||  namea     *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R2            &||  namea     *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R2            &||  namea     *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R0, R2        &||  namea     R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
  nameb     *+AR1(1), R2            &||  namea     R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
nameb##3_##namea##3_M:                                                                        &\
  nameb##3  R0, R1, R2              &||  namea##3  *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb##3  R0, R2                  &||  namea##3  *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb##3  R0, *+AR1(1), R2        &||  namea##3  *+AR0(1), R0, R0         /* H,I,M|J,K;N */ &\
  nameb##3  R0, *+AR1(1), R2        &||  namea##3  *+AR0(1), R0             /* H,I,M|J,K;N */ &\
  nameb##3  R0, *+AR1(1), R2        &||  namea##3  R0, *+AR0(1), R0         /* H,I,M|K,J,N */ &\
  nameb##3  *+AR0(1), *+AR1(1), R2  &||  namea##3  R2, R1, R0               /* I,J,M|H,K;N */ &\
  nameb##3  *+AR0(1), *+AR1(1), R2  &||  namea##3  R2, R0                   /* I,J,M|H,K;N */ &\
  nameb##3  *+AR1(1), R3, R2        &||  namea##3  *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R3, R2        &||  namea##3  *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R2            &||  namea##3  *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R2            &||  namea##3  *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R0, R2        &||  namea##3  R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
  nameb##3  *+AR1(1), R2            &||  namea##3  R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
nameb##_##namea##3_M:                                                                         &\
  nameb     R0, R1, R2              &||  namea##3  *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb     R0, R2                  &||  namea##3  *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb     R0, *+AR1(1), R2        &||  namea##3  *+AR0(1), R0, R0         /* H,I,M|J,K;N */ &\
  nameb     R0, *+AR1(1), R2        &||  namea##3  *+AR0(1), R0             /* H,I,M|J,K;N */ &\
  nameb     R0, *+AR1(1), R2        &||  namea##3  R0, *+AR0(1), R0         /* H,I,M|K,J,N */ &\
  nameb     *+AR0(1), *+AR1(1), R2  &||  namea##3  R2, R1, R0               /* I,J,M|H,K;N */ &\
  nameb     *+AR0(1), *+AR1(1), R2  &||  namea##3  R2, R0                   /* I,J,M|H,K;N */ &\
  nameb     *+AR1(1), R3, R2        &||  namea##3  *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R3, R2        &||  namea##3  *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R2            &||  namea##3  *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R2            &||  namea##3  *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb     *+AR1(1), R0, R2        &||  namea##3  R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
  nameb     *+AR1(1), R2            &||  namea##3  R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
nameb##3_##namea##_M:                                                                         &\
  nameb##3  R0, R1, R2              &||  namea     *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb##3  R0, R2                  &||  namea     *+AR0(1), *+AR1(1), R0   /* H,K;M|I,J,N */ &\
  nameb##3  R0, *+AR1(1), R2        &||  namea     *+AR0(1), R0, R0         /* H,I,M|J,K;N */ &\
  nameb##3  R0, *+AR1(1), R2        &||  namea     *+AR0(1), R0             /* H,I,M|J,K;N */ &\
  nameb##3  R0, *+AR1(1), R2        &||  namea     R0, *+AR0(1), R0         /* H,I,M|K,J,N */ &\
  nameb##3  *+AR0(1), *+AR1(1), R2  &||  namea     R2, R1, R0               /* I,J,M|H,K;N */ &\
  nameb##3  *+AR0(1), *+AR1(1), R2  &||  namea     R2, R0                   /* I,J,M|H,K;N */ &\
  nameb##3  *+AR1(1), R3, R2        &||  namea     *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R3, R2        &||  namea     *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R2            &||  namea     *+AR0(1), R1, R0         /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R2            &||  namea     *+AR0(1), R0             /* I,H;M|J,K;N */ &\
  nameb##3  *+AR1(1), R0, R2        &||  namea     R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\
  nameb##3  *+AR1(1), R2            &||  namea     R0, *+AR0(1), R0         /* I,H;M|K,J,N */ &\


/* P: General 2-operand operation with parallell store
   Syntax: <ia> src2, dst1 || <ib> src3, dst2
       src2 = Indirect 0,1,IR0,IR1 (I)
       dst1 = Register 0-7 (L)
       src3 = Register 0-7 (H)
       dst2 = Indirect 0,1,IR0,IR1 (J)
   Instr: 9/2 - ABSF||STF, ABSI||STI, FIX||STI, FLOAT||STF, LDF||STF,
                LDI||STI, NEGF||STF, NEGI||STI, NOT||STI, C4x: FRIEEE||STF,
                TOIEEE||STF
   Alias: a||b, b||a
*/
#define P_CLASS(namea, nameb) \
namea##_##nameb##_P:                                            &\
  namea  *+AR0(1), R0  &||  nameb  R1, *+AR1(1)   /* I,L|H,J */ &\
  nameb  R1, *+AR1(1)  &||  namea  *+AR0(1), R0   /* H,J|I,L */


/* Q: General 3-operand operation with parallell store
   Syntax: <ia> src1, src2, dst1 || <ib> src3, dst2
       src1 = Register 0-7 (K)
       src2 = Indirect 0,1,IR0,IR1 (I)
       dst1 = Register 0-7 (L)
       src3 = Register 0-7 (H)
       dst2 = Indirect 0,1,IR0,IR1 (J)
   Instr: 4/0 - ASH3||STI, LSH3||STI, SUBF3||STF, SUBI3||STI
   Alias: a||b, b||a, a3||b, b||a3
*/
#define Q_CLASS(namea, nameb) \
namea##_##nameb##_Q:                                                            &\
  namea     R0, *+AR0(1), R0  &||  nameb     R1, *+AR1(1)       /* K,I,L|H,J */ &\
  nameb     R1, *+AR1(1)      &||  namea     R0, *+AR0(1), R0   /* H,J|K,I,L */ &\
namea##3_##nameb##_Q:                                                           &\
  namea##3  R0, *+AR0(1), R0  &||  nameb     R1, *+AR1(1)       /* K,I,L|H,J */ &\
  nameb     R1, *+AR1(1)      &||  namea##3  R0, *+AR0(1), R0   /* H,J|K,I,L */


/* QC: General commutative 3-operand operation with parallell store
   Syntax: <ia> src2, src1, dst1 || <ib> src3, dst2
           <ia> src1, src2, dst1 || <ib> src3, dst2 - Manual
       src1 = Register 0-7 (K)
       src2 = Indirect 0,1,IR0,IR1 (I)
       dst1 = Register 0-7 (L)
       src3 = Register 0-7 (H)
       dst2 = Indirect 0,1,IR0,IR1 (J)
   Instr: 7/0 - ADDF3||STF, ADDI3||STI, AND3||STI, MPYF3||STF, MPYI3||STI,
                OR3||STI, XOR3||STI
   Alias: a||b, b||a, a3||b, b||a3
*/
#define QC_CLASS(namea, nameb) \
namea##_##nameb##_QC:                                                           &\
  namea     *+AR0(1), R1, R0  &||  nameb     R1, *+AR1(1)       /* I,K;L|H,J */ &\
  namea     *+AR0(1), R0      &||  nameb     R1, *+AR1(1)       /* I,K;L|H,J */ &\
  namea     R0, *+AR0(1), R0  &||  nameb     R1, *+AR1(1)       /* K,I,L|H,J */ &\
  nameb     R1, *+AR1(1)      &||  namea     *+AR0(1), R1, R0   /* H,J|I,K;L */ &\
  nameb     R1, *+AR1(1)      &||  namea     *+AR0(1), R0       /* H,J|I,K;L */ &\
  nameb     R1, *+AR1(1)      &||  namea     R0, *+AR0(1), R0   /* H,J|K,I,L */ &\
namea##3_##nameb##_QC:                                                          &\
  namea##3  *+AR0(1), R1, R0  &||  nameb     R1, *+AR1(1)       /* I,K;L|H,J */ &\
  namea##3  *+AR0(1), R0      &||  nameb     R1, *+AR1(1)       /* I,K;L|H,J */ &\
  namea##3  R0, *+AR0(1), R0  &||  nameb     R1, *+AR1(1)       /* K,I,L|H,J */ &\
  nameb     R1, *+AR1(1)      &||  namea##3  *+AR0(1), R1, R0   /* H,J|I,K;L */ &\
  nameb     R1, *+AR1(1)      &||  namea##3  *+AR0(1), R0       /* H,J|I,K;L */ &\
  nameb     R1, *+AR1(1)      &||  namea##3  R0, *+AR0(1), R0   /* H,J|K,I,L */


/* R: General register integer operation
   Syntax: <i> dst
       dst = Register (R)
   Instr: 6/0 - POP, PUSH, ROL, ROLC, ROR, RORC
*/
#define R_CLASS(name) \
name##_R:            &\
  name  AR0   /* R */


/* RF: General register float operation
   Syntax: <i> dst
       dst = Register 0-11 (R)
   Instr: 2/0 - POPF, PUSHF
*/
#define RF_CLASS(name) \
name##_RF:            &\
  name  F0   /* R */


/* S: General 3-operand float operation
   Syntax: <i> src2, src1, dst
       src2 = Register 0-11 (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C)
       src1 = Register 0-11 (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (O)
       dst  = Register 0-11 (R)
   Instr: 1/0 - SUBF3
   Alias: i, i3
*/
#define S_CLASS(name) \
name##_S:                                       &\
  name     R2, R1, R0               /* E,G;R */ &\
  name     R1, R0                   /* E,G;R */ &\
  name     R1, *+AR0(1), R0         /* E,J,R */ &\
  name     *+AR0(1), R1, R0         /* I,G;R */ &\
  name     *+AR0(1), R0             /* I,G;R */ &\
  name     *+AR0(1), *+AR1(1), R0   /* I,J,R */ &\
  .ifdef TEST_C4X                               &\
  name     *+AR0(5), R1, R0         /* C,G;R */ &\
  name     *+AR0(5), R0             /* C,G;R */ &\
  name     *+AR0(5), *+AR1(5), R0   /* C,O,R */ &\
  .endif                                        &\
name##3_S:                                      &\
  name##3  R2, R1, R0               /* E,G;R */ &\
  name##3  R1, R0                   /* E,G;R */ &\
  name##3  R1, *+AR0(1), R0         /* E,J,R */ &\
  name##3  *+AR0(1), R1, R0         /* I,G;R */ &\
  name##3  *+AR0(1), R0             /* I,G;R */ &\
  name##3  *+AR0(1), *+AR1(1), R0   /* I,J,R */ &\
  .ifdef TEST_C4X                               &\
  name##3  *+AR0(5), R1, R0         /* C,G;R */ &\
  name##3  *+AR0(5), R0             /* C,G;R */ &\
  name##3  *+AR0(5), *+AR1(5), R0   /* C,O,R */ &\
  .endif                                        


/* SC: General commutative 3-operand float operation
   Syntax: <i> src2, src1, dst - Manual
           <i> src1, src2, dst
       src2 = Register 0-11 (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C)
       src1 = Register 0-11 (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (O)
       dst  = Register 0-11 (R)
   Instr: 2/0 - ADDF3, MPYF3
   Alias: i, i3
*/
#define SC_CLASS(name) \
name##_SC:                                      &\
  name     R2, R1, R0               /* E,G;R */ &\
  name     R1, R0                   /* E,G;R */ &\
  name     R1, *+AR0(1), R0         /* E,J,R */ &\
  name     *+AR0(1), R1, R0         /* I,G;R */ &\
  name     *+AR0(1), R0             /* I,G;R */ &\
  name     *+AR0(1), *+AR1(1), R0   /* I,J,R */ &\
  .ifdef TEST_C4X                               &\
  name     *+AR0(5), R1, R0         /* C,G;R */ &\
  name     *+AR0(5), R0             /* C,G;R */ &\
  name     R1, *+AR0(5), R0         /* G,C,R */ &\
  name     *+AR0(5), *+AR1(5), R0   /* C,O,R */ &\
  .endif                                        &\
name##3_SC:                                     &\
  name##3  R2, R1, R0               /* E,G;R */ &\
  name##3  R1, R0                   /* E,G;R */ &\
  name##3  R1, *+AR0(1), R0         /* E,J,R */ &\
  name##3  *+AR0(1), R1, R0         /* I,G;R */ &\
  name##3  *+AR0(1), R0             /* I,G;R */ &\
  name##3  *+AR0(1), *+AR1(1), R0   /* I,J,R */ &\
  .ifdef TEST_C4X                               &\
  name##3  *+AR0(5), R1, R0         /* C,G;R */ &\
  name##3  *+AR0(5), R0             /* C,G;R */ &\
  name##3  R1, *+AR0(5), R0         /* G,C,R */ &\
  name##3  *+AR0(5), *+AR1(5), R0   /* C,O,R */ &\
  .endif                                        


/* S2: General 3-operand float operation with 2 args
   Syntax: <i> src2, src1
       src2 = Register 0-11 (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C)
       src1 = Register 0-11 (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (O)
   Instr: 1/0 - CMPF3
   Alias: i, i3
*/
#define S2_CLASS(name) \
name##_S2:                                    &\
  name     R2, R1                   /* E,G */ &\
  name     R1, *+AR0(1)             /* E,J */ &\
  name     *+AR0(1), R1             /* I,G */ &\
  name     *+AR0(1), *+AR1(1)       /* I,J */ &\
  .ifdef TEST_C4X                             &\
  name     *+AR0(5), R1             /* C,G */ &\
  name     *+AR0(5), *+AR1(5)       /* C,O */ &\
  .endif                                      &\
name##3_S2:                                   &\
  name##3  R2, R1                   /* E,G */ &\
  name##3  R1, *+AR0(1)             /* E,J */ &\
  name##3  *+AR0(1), R1             /* I,G */ &\
  name##3  *+AR0(1), *+AR1(1)       /* I,J */ &\
  .ifdef TEST_C4X                             &\
  name##3  *+AR0(5), R1             /* C,G */ &\
  name##3  *+AR0(5), *+AR1(5)       /* C,O */ &\
  .endif                                        


/* T: General 3-operand integer operand
   Syntax: <i> src2, src1, dst
       src2 = Register (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C), Immediate (W)
       src1 = Register (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (O)
       dst  = Register (R)
   Instr: 5/0 - ANDN3, ASH3, LSH3, SUBB3, SUBI3
   Alias: i, i3
*/
#define T_CLASS(name) \
name##_T:                                        &\
  name     AR2, AR1, AR0             /* E,G;R */ &\
  name     AR1, AR0                  /* E,G;R */ &\
  name     AR1, *+AR0(1), AR0        /* E,J,R */ &\
  name     *+AR0(1), AR1, AR0        /* I,G;R */ &\
  name     *+AR0(1), AR0             /* I,G;R */ &\
  name     *+AR1(1), *+AR0(1), AR0   /* I,J,R */ &\
  .ifdef   TEST_C4X                              &\
  name     5, AR1, AR0               /* W,G;R */ &\
  name     5, AR0                    /* W,G;R */ &\
  name     *+AR0(5), AR1, AR0        /* C,G;R */ &\
  name     *+AR0(5), AR0             /* C,G;R */ &\
  name     5, *+AR0(5), AR0          /* W,O,R */ &\
  name     *+AR0(5), *+AR1(5), AR0   /* C,O,R */ &\
  .endif                                         &\
name##3_T:                                       &\
  name##3  AR2, AR1, AR0             /* E,G;R */ &\
  name##3  AR1, AR0                  /* E,G;R */ &\
  name##3  AR1, *+AR0(1), AR0        /* E,J,R */ &\
  name##3  *+AR0(1), AR1, AR0        /* I,G;R */ &\
  name##3  *+AR0(1), AR0             /* I,G;R */ &\
  name##3  *+AR1(1), *+AR0(1), AR0   /* I,J,R */ &\
  .ifdef   TEST_C4X                              &\
  name##3  -5, AR1, AR0              /* W,G;R */ &\
  name##3  -5, AR0                   /* W,G;R */ &\
  name##3  *+AR0(5), AR1, AR0        /* C,G;R */ &\
  name##3  *+AR0(5), AR0             /* C,G;R */ &\
  name##3  -5, *+AR0(5), AR0         /* W,O,R */ &\
  name##3  *+AR0(5), *+AR1(5), AR0   /* C,O,R */ &\
  .endif


/* TC: General commutative 3-operand integer operation
   Syntax: <i> src2, src1, dst
           <i> src1, src2, dst
       src2 = Register (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C), Immediate (W)
       src1 = Register (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (O)
       dst  = Register (R)
   Instr: 6/2 - ADDC3, ADDI3, AND3, MPYI3, OR3, XOR3, C4x: MPYSHI, MPYUHI
   Alias: i, i3
*/
#define TC_CLASS(name) \
name##_TC:                                       &\
  name     AR2, AR1, AR0             /* E,G;R */ &\
  name     AR1, AR0                  /* E,G;R */ &\
  name     AR1, *+AR0(1), AR0        /* E,J,R */ &\
  name     *+AR0(1), AR1, AR0        /* I,G;R */ &\
  name     *+AR0(1), AR0             /* I,G;R */ &\
  name     *+AR1(1), *+AR0(1), AR0   /* I,J,R */ &\
  .ifdef   TEST_C4X                              &\
  name     5, AR1, AR0               /* W,G;R */ &\
  name     5, AR0                    /* W,G;R */ &\
  name     AR1, -5, AR0              /* G,W,R */ &\
  name     *+AR0(5), AR1, AR0        /* C,G;R */ &\
  name     *+AR0(5), AR0             /* C,G;R */ &\
  name     AR1, *+AR0(5), AR0        /* G,C,R */ &\
  name     5, *+AR0(5), AR0          /* W,O,R */ &\
  name     *+AR0(5), -5, AR0         /* O,W,R */ &\
  name     *+AR0(5), *+AR1(5), AR0   /* C,O,R */ &\
  .endif                                         &\
name##3_TC:                                      &\
  name##3  AR2, AR1, AR0             /* E,G;R */ &\
  name##3  AR1, AR0                  /* E,G;R */ &\
  name##3  AR1, *+AR0(1), AR0        /* E,J,R */ &\
  name##3  *+AR0(1), AR1, AR0        /* I,G;R */ &\
  name##3  *+AR0(1), AR0             /* I,G;R */ &\
  name##3  *+AR1(1), *+AR0(1), AR0   /* I,J,R */ &\
  .ifdef   TEST_C4X                              &\
  name##3  -5, AR1, AR0              /* W,G;R */ &\
  name##3  -5, AR0                   /* W,G;R */ &\
  name##3  AR1, -5, AR0              /* G,W,R */ &\
  name##3  *+AR0(5), AR1, AR0        /* C,G;R */ &\
  name##3  *+AR0(5), AR0             /* C,G;R */ &\
  name##3  AR1, *+AR0(5), AR0        /* G,C,R */ &\
  name##3  -5, *+AR0(5), AR0         /* W,O,R */ &\
  name##3  *+AR0(5), -5, AR0         /* O,W,R */ &\
  name##3  *+AR0(5), *+AR1(5), AR0   /* C,O,R */ &\
  .endif


/* T2: General 3-operand integer operation with 2 args
   Syntax: <i> src2, src1
       src2 = Register (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C), Immediate (W)
       src1 = Register (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (O)
   Instr: 1/0 - CMPI3
   Alias: i, i3
*/
#define T2_CLASS(name) \
name##_T2:                                     &\
  name     AR2, AR1                  /* E,G */ &\
  name     AR1, *+AR0(1)             /* E,J */ &\
  name     *+AR0(1), AR1             /* I,G */ &\
  name     *+AR1(1), *+AR0(1)        /* I,J */ &\
  .ifdef   TEST_C4X                            &\
  name     -5, AR1                   /* W,G */ &\
  name     *+AR0(5), AR1             /* C,G */ &\
  name     -5, *+AR0(5)              /* W,O */ &\
  name     *+AR0(5), *+AR1(5)        /* C,O */ &\
  .endif                                       &\
name##3_T2:                                    &\
  name##3  AR2, AR1                  /* E,G */ &\
  name##3  AR1, *+AR0(1)             /* E,J */ &\
  name##3  *+AR0(1), AR1             /* I,G */ &\
  name##3  *+AR1(1), *+AR0(1)        /* I,J */ &\
  .ifdef   TEST_C4X                            &\
  name##3  -5, AR1                   /* W,G */ &\
  name##3  *+AR0(5), AR1             /* C,G */ &\
  name##3  -5, *+AR0(5)              /* W,O */ &\
  name##3  *+AR0(5), *+AR1(5)        /* C,O */ &\
  .endif


/* T2C: General commutative 3-operand integer operation with 2 args 
   Syntax: <i> src2, src1 - Manual
           <i> src1, src2 
       src2 = Register (E), Indirect 0,1,IR0,IR1 (I), C4x T2: Indirect (C), Immediate (W)
       src1 = Register (G), Indirect 0,1,IR0,IR1 (J), C4x T2: Indirect (0)
   Instr: 1/0 - TSTB3
   Alias: i, i3
*/
#define T2C_CLASS(name) \
name##_T2C:                                    &\
  name     AR2, AR1                  /* E,G */ &\
  name     AR1, *+AR0(1)             /* E,J */ &\
  name     *+AR0(1), AR1             /* I,G */ &\
  name     *+AR1(1), *+AR0(1)        /* I,J */ &\
  .ifdef   TEST_C4X                            &\
  name     5, AR1                    /* W,G */ &\
  name     AR1, -5                   /* G,W */ &\
  name     *+AR0(5), AR1             /* C,G */ &\
  name     AR1, *+AR0(5)             /* G,C */ &\
  name     5, *+AR0(5)               /* W,O */ &\
  name     *+AR0(5), -5              /* O,W */ &\
  name     *+AR0(5), *+AR1(5)        /* C,O */ &\
  .endif                                       &\
name##3_T2C:                                   &\
  name##3  AR2, AR1                  /* E,G */ &\
  name##3  AR1, *+AR0(1)             /* E,J */ &\
  name##3  *+AR0(1), AR1             /* I,G */ &\
  name##3  *+AR1(1), *+AR0(1)        /* I,J */ &\
  .ifdef   TEST_C4X                            &\
  name##3  -5, AR1                   /* W,G */ &\
  name##3  AR1, -5                   /* G,W */ &\
  name##3  *+AR0(5), AR1             /* C,G */ &\
  name##3  AR1, *+AR0(5)             /* G,C */ &\
  name##3  -5, *+AR0(5)              /* W,O */ &\
  name##3  *+AR0(5), -5              /* O,W */ &\
  name##3  *+AR0(5), *+AR1(5)        /* C,O */ &\
  .endif
