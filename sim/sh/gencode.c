/* Simulator/Opcode generator for the Hitachi Super-H architecture.

   Written by Steve Chamberlain of Cygnus Support.
   sac@cygnus.com

   This file is part of SH sim


		THIS SOFTWARE IS NOT COPYRIGHTED

   Cygnus offers the following for use in the public domain.  Cygnus
   makes no warranty with regard to the software or it's performance
   and the user accepts the software "AS IS" with all faults.

   CYGNUS DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO
   THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*/

/* This program generates the opcode table for the assembler and
   the simulator code

   -t		prints a pretty table for the assembler manual
   -s		generates the simulator code jump table
   -x		generates the simulator code switch statement
   default 	generates the opcode tables

*/

typedef struct
{
  char *name;
  char *code;
  char *stuff[10];
  int index;
}

op;


op tab[] =
{

  {"add #<imm>,<REG_N>", "0111nnnni8*1....", "R[n] += SEXT(i);"},
  {"add <REG_M>,<REG_N>", "0011nnnnmmmm1100", "R[n] += R[m];"},
  {"addc <REG_M>,<REG_N>", "0011nnnnmmmm1110", "ult = R[n]; R[n] += (R[m]+T); T = ult>R[n];"},
  {"addv <REG_M>,<REG_N>", "0011nnnnmmmm1111",
   "long ans;",
   "ans = R[n] + R[m];",
   "T = ((~R[n] & R[m] & ans) | (R[n] & R[m] & ~ans)) >>31;",
   "R[n] = ans;"},

  {"and #<imm>,R0", "11001001i8*1....", "R0&=i;"},
  {"and <REG_M>,<REG_N>", "0010nnnnmmmm1001", "R[n]&=R[m];"},
  {"and.b #<imm>,@(R0,GBR)", "11001101i8*1....", "WBAT(GBR+R0, RBAT(GBR+R0) & i);"},

  {"bf <bdisp8>", "10001011i8p1....", "if(T==0) {PC+=(SEXT(i)<<1)+2;C+=2;}"},
  {"bra <bdisp12>", "1010i12.........", "ult = PC; PC=PC+(i<<1)+2;SL(ult+2);"},
  {"bsr <bdisp12>", "1011i12.........", "PR = PC; PC=PC+(i<<1)+2;SL(PR+2);"},
  {"bt <bdisp8>", "10001001i8p1....", "if(T==1) {PC+=(SEXT(i)<<1)+2;C+=2;}"},
  {"clrmac", "0000000000101000", "MACH = MACL = 0;"},
  {"clrt", "0000000000001000", "T= 0;"},
  {"cmp/eq #<imm>,R0", "10001000i8*1....", "T = R0 == SEXT(i);"},
  {"cmp/eq <REG_M>,<REG_N>", "0011nnnnmmmm0000", "T=R[n]==R[m];"},
  {"cmp/ge <REG_M>,<REG_N>", "0011nnnnmmmm0011", "T=R[n]>=R[m];"},
  {"cmp/gt <REG_M>,<REG_N>", "0011nnnnmmmm0111", "T=R[n]>R[m];"},
  {"cmp/hi <REG_M>,<REG_N>", "0011nnnnmmmm0110", "T=UR[n]>UR[m];"},
  {"cmp/hs <REG_M>,<REG_N>", "0011nnnnmmmm0010", "T=UR[n]>=UR[m];"},
  {"cmp/pl <REG_N>", "0100nnnn00010101", "T = R[n]>0;"},
  {"cmp/pz <REG_N>", "0100nnnn00010001", "T = R[n]>=0;"},
  {"cmp/str <REG_M>,<REG_N>", "0010nnnnmmmm1100", "ult = R[n] ^ R[m]; T=((ult&0xff000000)==0) |((ult&0xff0000)==0) |((ult&0xff00)==0) |((ult&0xff)==0); "},
  {"div0s <REG_M>,<REG_N>", "0010nnnnmmmm0111", "Q=R[n]<0; M=R[m]<0; T=M!=Q;;"},
  {"div0u", "0000000000011001", "M=Q=T=0;"},
  {"div1 <REG_M>,<REG_N>", "0011nnnnmmmm0100", "T=div1(R,n,m,T);"},
  {"exts.b <REG_M>,<REG_N>", "0110nnnnmmmm1110", "R[n] = SEXT(R[m]);"},
  {"exts.w <REG_M>,<REG_N>", "0110nnnnmmmm1111", "R[n] = SEXTW(R[m]);"},
  {"extu.b <REG_M>,<REG_N>", "0110nnnnmmmm1100", "R[n] = R[m] & 0xff;"},
  {"extu.w <REG_M>,<REG_N>", "0110nnnnmmmm1101", "R[n] = R[m] & 0xffff;"},
  {"jmp @<REG_N>", "0100nnnn00101011", "ult = PC; PC=R[n]-2; SL(ult+2);"},
  {"jsr @<REG_N>", "0100nnnn00001011", "PR = PC; PC=R[n]-2; SL(PR+2);"},
  {"ldc <REG_N>,GBR", "0100nnnn00011110", "GBR=R[n];"},
  {"ldc <REG_N>,SR", "0100nnnn00001110", "SET_SR(R[n]);"},
  {"ldc <REG_N>,VBR", "0100nnnn00101110", "VBR=R[n];"},
  {"ldc.l @<REG_N>+,GBR", "0100nnnn00010111", "GBR=RLAT(R[n]);R[n]+=4;;"},
  {"ldc.l @<REG_N>+,SR", "0100nnnn00000111", "SET_SR(RLAT(R[n]));R[n]+=4;;"},
  {"ldc.l @<REG_N>+,VBR", "0100nnnn00100111", "VBR=RLAT(R[n]);R[n]+=4;;"},
  {"lds <REG_N>,MACH", "0100nnnn00001010", "MACH = SEXT(R[n]);"},
  {"lds <REG_N>,MACL", "0100nnnn00011010", "MACL= R[n];"},
  {"lds <REG_N>,PR", "0100nnnn00101010", "PR = R[n];"},
  {"lds.l @<REG_N>+,MACH", "0100nnnn00000110", "MACH = SEXT(RLAT(R[n]));R[n]+=4;"},
  {"lds.l @<REG_N>+,MACL", "0100nnnn00010110", "MACL = RLAT(R[n]);R[n]+=4;"},
  {"lds.l @<REG_N>+,PR", "0100nnnn00100110", "PR = RLAT(R[n]);R[n]+=4;;"},
  {"mac.w @<REG_M>+,@<REG_N>+", "0100nnnnmmmm1111", "abort();"},
  {"mov #<imm>,<REG_N>", "1110nnnni8*1....", "R[n] = SEXT(i);"},
  {"mov <REG_M>,<REG_N>", "0110nnnnmmmm0011", "R[n] = R[m];"},
{"mov.b <REG_M>,@(R0,<REG_N>)", "0000nnnnmmmm0100", "WBAT(R[n]+R0, R[m]);"},
{"mov.b <REG_M>,@-<REG_N>", "0010nnnnmmmm0100", "R[n]--; WBAT(R[n], R[m]);"},
  {"mov.b <REG_M>,@<REG_N>", "0010nnnnmmmm0000", "WBAT(R[n], R[m]);"},
  {"mov.b @(<disp>,<REG_M>),R0", "10000100mmmmi4*1", "R0=RBAT(i+R[m]);"},
  {"mov.b @(<disp>,GBR),R0", "11000100i8*1....", "R0=RBAT(i+GBR);"},
  {"mov.b @(R0,<REG_M>),<REG_N>", "0000nnnnmmmm1100", "R[n]=RBAT(R0+R[m]);"},
{"mov.b @<REG_M>+,<REG_N>", "0110nnnnmmmm0100", "R[n] = RBAT(R[m]);R[m]++;"},
  {"mov.b @<REG_M>,<REG_N>", "0110nnnnmmmm0000", "R[n]=RBAT(R[m]);"},
  {"mov.b R0,@(<disp>,<REG_M>)", "10000000mmmmi4*1", "R0=RBAT(i+R[m]);"},
  {"mov.b R0,@(<disp>,GBR)", "11000000i8*1....", "R0 = RBAT(i+GBR);"},
  {"mov.l <REG_M>,@(<disp>,<REG_N>)", "0001nnnnmmmmi4*4", "WLAT(i+R[n],R[m]);"},
  {"mov.l <REG_M>,@(R0,<REG_N>)", "0000nnnnmmmm0110", "WLAT(R0+R[n],R[m]);"},
{"mov.l <REG_M>,@-<REG_N>", "0010nnnnmmmm0110", "R[n]-=4;WLAT(R[n],R[m]);"},
  {"mov.l <REG_M>,@<REG_N>", "0010nnnnmmmm0010", "WLAT(R[n], R[m]);"},
  {"mov.l @(<disp>,<REG_N>),<REG_M>", "0101nnnnmmmmi4*4", "R[m]=RLAT(i+R[n]);"},
  {"mov.l @(<disp>,GBR),R0", "11000110i4*4", "R0=RLAT(i+GBR);"},
  {"mov.l @(<disp>,PC),<REG_N>", "1101nnnni8p4....", "R[n]=RLAT(i+4+PC);"},
  {"mov.l @(R0,<REG_M>),<REG_N>", "0000nnnnmmmm1110", "R[n]=RLAT(R0+R[m]);"},
{"mov.l @<REG_M>+,<REG_N>", "0110nnnnmmmm0110", "R[n]=RLAT(R[m]);R[m]+=4;"},
  {"mov.l @<REG_M>,<REG_N>", "0110nnnnmmmm0010", "R[n]=RLAT(R[m]);"},
  {"mov.l R0,@(<disp>,GBR)", "11000010i8*4....", "R0=RLAT(R0+GBR);"},
  {"mov.w <REG_M>,@(R0,<REG_N>)", "0000nnnnmmmm0101", "WWAT(R0+R[n],R[m]);"},
{"mov.w <REG_M>,@-<REG_N>", "0010nnnnmmmm0101", "R[n]-=2;WWAT(R[n],R[m]);"},
  {"mov.w <REG_M>,@<REG_N>", "0010nnnnmmmm0001", "WWAT(R[n],R[m]);"},
  {"mov.w @(<disp>,<REG_M>),R0", "10000101mmmmi4*2", "R0=RSWAT(i+R[m]);"},
  {"mov.w @(<disp>,GBR),R0", "11000101i8*2....", "R0=RSWAT(i+GBR);"},
  {"mov.w @(<disp>,PC),<REG_N>", "1001nnnni8p2....", "R[n]=RSWAT(PC+i+4);"},
{"mov.w @(R0,<REG_M>),<REG_N>", "0000nnnnmmmm1101", "R[n]=RSWAT(R0+R[m]);"},
{"mov.w @<REG_M>+,<REG_N>", "0110nnnnmmmm0101", "R[n]=RSWAT(R[m]);R[m]+=2;"},
  {"mov.w @<REG_M>,<REG_N>", "0110nnnnmmmm0001", "R[n]=RSWAT(R[m]);"},
  {"mov.w R0,@(<disp>,<REG_M>)", "10000001mmmmi4*2", "R0=RSWAT(i+R[m]);"},
  {"mov.w R0,@(<disp>,GBR)", "11000001i8*2....", "R0=RSWAT(i+GBR);"},
  {"mova @(<disp>,PC),R0", "11000111i8p4....", "R0=i+4+PC;"},
  {"movt <REG_N>", "0000nnnn00101001", "R[n]=T;"},
  {"muls <REG_M>,<REG_N>", "0010nnnnmmmm1111",
   "MACL=((long)(short)R[n])*((long)(short)R[m]);"},
{"mulu <REG_M>,<REG_N>","0010nnnnmmmm1110", "MACL=((unsigned long)(unsigned short)R[n])*((unsigned long)(unsigned short)R[m]);"},  
  {"neg <REG_M>,<REG_N>", "0110nnnnmmmm1011", "R[n] = - R[m];"},
  {"negc <REG_M>,<REG_N>", "0110nnnnmmmm1010",  "ult=0-R[m];R[n]=ult-T;T=SBIT(R[n])!=SBIT(ult);"},
  {"nop", "0000000000001001", ""},
  {"not <REG_M>,<REG_N>", "0110nnnnmmmm0111", "R[n]=~R[m];"},
  {"or #<imm>,R0", "11001011i8*1....", "R0|=i;"},
  {"or <REG_M>,<REG_N>", "0010nnnnmmmm1011", "R[n]|=R[m];"},
  {"or.b #<imm>,@(R0,GBR)", "11001111i8*1....", "WBAT(R0+GBR,RBAT(R0+GBR)|i);"},
  {"rotcl <REG_N>", "0100nnnn00100100", "ult = R[n] <0;R[n] = (R[n]<<1)|T;T=ult;"},
  {"rotcr <REG_N>", "0100nnnn00100101", "ult = R[n]&1;R[n]=(UR[n]>>1)|(T<<31);T=ult;"},
  {"rotl <REG_N>", "0100nnnn00000100", "T=R[n]<0;R[n]<<=1;R[n]|=T;"},
  {"rotr <REG_N>", "0100nnnn00000101", "T=R[n]&1;UR[n]>>=1;R[n]|=(T<<31);"},
  {"rte", "0000000000101011", "abort();"},
  {"rts", "0000000000001011", "ult=PC;PC=PR+2;SL(ult+2);"},
  {"sett", "0000000000011000", "T=1;"},
  {"shal <REG_N>", "0100nnnn00100000", "T=R[n]<0; R[n]<<=1;"},
  {"shar <REG_N>", "0100nnnn00100001", "T=R[n]&1; R[n]>>=1;"},
  {"shll <REG_N>", "0100nnnn00000000", "T=R[n]<0; R[n]<<=1;"},
  {"shll16 <REG_N>", "0100nnnn00101000", "R[n]<<=16;"},
  {"shll2 <REG_N>", "0100nnnn00001000", "R[n]<<=2;"},
  {"shll8 <REG_N>", "0100nnnn00011000", "R[n]<<=8;"},
  {"shlr <REG_N>", "0100nnnn00000001", "T=R[n]&1;R[n]=UR[n]>>1;"},
  {"shlr16 <REG_N>", "0100nnnn00101001", "R[n]=UR[n]>>16;"},
  {"shlr2 <REG_N>", "0100nnnn00001001", "R[n]=UR[n]>>2;"},
  {"shlr8 <REG_N>", "0100nnnn00011001", "R[n]=UR[n]>>8;"},
  {"sleep", "0000000000011011", "abort();"},
  {"stc GBR,<REG_N>", "0000nnnn00010010", "R[n]=GBR;"},
  {"stc SR,<REG_N>", "0000nnnn00000010", "R[n]=GET_SR();"},
  {"stc VBR,<REG_N>", "0000nnnn00100010", "R[n]=VBR;"},
  {"stc.l GBR,@-<REG_N>", "0100nnnn00010011", "R[n]-=4;WLAT(R[n],GBR);;"},
  {"stc.l SR,@-<REG_N>", "0100nnnn00000011", "R[n]-=4;WLAT(R[n],GET_SR());"},
  {"stc.l VBR,@-<REG_N>", "0100nnnn00100011", "R[n]-=4;WLAT(R[n],VBR);"},
  {"sts MACH,<REG_N>", "0000nnnn00001010", "R[n]=MACH;"},
  {"sts MACL,<REG_N>", "0000nnnn00011010", "R[n]=MACL;"},
  {"sts PR,<REG_N>", "0000nnnn00101010", "R[n]=PR;"},
  {"sts.l MACH,@-<REG_N>", "0100nnnn00000010", "R[n]-=4;WLAT(R[n],MACH);"},
  {"sts.l MACL,@-<REG_N>", "0100nnnn00010010", "R[n]-=4;WLAT(R[n],MACL);"},
  {"sts.l PR,@-<REG_N>", "0100nnnn00100010", "R[n]-=4;WLAT(R[n],PR);"},
  {"sub <REG_M>,<REG_N>", "0011nnnnmmmm1000", "R[n]-=R[m];"},
  {"subc <REG_M>,<REG_N>", "0011nnnnmmmm1010", "ult = R[n];R[n]-=R[m]+T;T=ult<UR[n];"},
  {"subv <REG_M>,<REG_N>", "0011nnnnmmmm1011", "abort();"},
  {"swap.b <REG_M>,<REG_N>", "0110nnnnmmmm1000", "R[n]=((R[m]<<8)&0xff00)|((R[m]>>8)&0x00ff);"},
  {"swap.w <REG_M>,<REG_N>", "0110nnnnmmmm1001", "R[n]=((R[m]<<16)&0xffff0000)|((R[m]>>16)&0x00ffff);"},
  {"tas.b @<REG_N>", "0100nnnn00011011", "ult=RBAT(R[n]);T=ult==0;WBAT(R[n],ult|0x80);"},
  {"trapa #<imm>", "11000011i8*1....", "trap(i,R);"},
  {"tst #<imm>,R0", "11001000i8*1....", "T=(R0&i)==0;"},
  {"tst <REG_M>,<REG_N>", "0010nnnnmmmm1000", "T=(R[n]&R[m])==0;"},
  {"tst.b #<imm>,@(R0,GBR)", "11001100i8*1....", "T=(RBAT(GBR+R0)&i)==0;"},
  {"xor #<imm>,R0", "11001010i8*1....", "R0^=i;"},
  {"xor <REG_M>,<REG_N>", "0010nnnnmmmm1010", "R[n]^=R[m];"},
  {"xor.b #<imm>,@(R0,GBR)", "11001110i8*1....", "ult=RBAT(GBR+R0);ult^=i;WBAT(GBR+R0,ult);"},
  {"xtrct <REG_M>,<REG_N>", "0010nnnnmmmm1101", "R[n]=((R[n]>>16)&0xffff)|((R[m]<<16)&0xffff0000);"},

#if 0
{"mul.l <REG_M>,<REG_N>","0000nnnnmmmm0111","/* mull */"},
{"dmuls.l <REG_M>,<REG_N>","0011nnnnmmmm1101","/* dmuls.l */"},
{"dmulu.l <REG_M>,<REG_N>","0011nnnnmmmm0101",""},
{"mac.l @<REG_M>+,@<REG_N>+","0000nnnnmmmm1111",""},
{"bt/s <bdisp8>","10001101i8p1....",""},
{"bf/s <bdisp8>","10001111i8p1....",""},
{"dt <REG_N>","0100nnnn00010000",""},
{"braf @<REG_N>","0000nnnn00100011",""},
{"bsrf @<REG_N>","0000nnnn00000011",""},
{"mulu <REG_M>,<REG_N>","0010nnnnmmmm1110",""},
{"muls <REG_M>,<REG_N>","0010nnnnmmmm1111",""},     
#endif

  {0, 0}};

/* Tables of things to put into enums for sh-opc.h */
static char *nibble_type_list[] =
{
  "HEX_0",
  "HEX_1",
  "HEX_2",
  "HEX_3",
  "HEX_4",
  "HEX_5",
  "HEX_6",
  "HEX_7",
  "HEX_8",
  "HEX_9",
  "HEX_A",
  "HEX_B",
  "HEX_C",
  "HEX_D",
  "HEX_E",
  "HEX_F",
  "REG_N",
  "REG_M",
  "BRANCH_12",
  "BRANCH_8",
  "DISP_8",
  "DISP_4",
  "IMM_4",
  "IMM_4BY2",
  "IMM_4BY4",
  "PCRELIMM_8BY2",
  "PCRELIMM_8BY4",
  "IMM_8",
  "IMM_8BY2",
  "IMM_8BY4",
  0
};
static
char *arg_type_list[] =
{
  "A_END",
  "A_BDISP12",
  "A_BDISP8",
  "A_DEC_M",
  "A_DEC_N",
  "A_DISP_GBR",
  "A_DISP_PC",
  "A_DISP_REG_M",
  "A_DISP_REG_N",
  "A_GBR",
  "A_IMM",
  "A_INC_M",
  "A_INC_N",
  "A_IND_M",
  "A_IND_N",
  "A_IND_R0_REG_M",
  "A_IND_R0_REG_N",
  "A_MACH",
  "A_MACL",
  "A_PR",
  "A_R0",
  "A_R0_GBR",
  "A_REG_M",
  "A_REG_N",
  "A_SR",
  "A_VBR",
  0,
};

static void
make_enum_list (name, s)
     char *name;
     char **s;
{
  int i = 1;
  printf ("typedef enum {\n");
  while (*s)
    {
      printf ("\t%s,\n", *s, i);
      s++;
      i++;
    }
  printf ("} %s;\n", name);
}

static void
gengastab ()
{
  op *p;
  sorttab ();
  for (p = tab; p->name; p++)
    {
      printf ("%s %-30s\n", p->code, p->name);
    }


}


static void 
genopc ()
{
  op *p;
  make_enum_list ("sh_nibble_type", nibble_type_list);
  make_enum_list ("sh_arg_type", arg_type_list);

  printf ("typedef struct {\n");
  printf ("char *name;\n");
  printf ("sh_arg_type arg[3];\n");
  printf ("sh_nibble_type nibbles[4];\n");
  printf ("} sh_opcode_info;\n");
  printf ("#ifdef DEFINE_TABLE\n");
  printf ("sh_opcode_info sh_table[]={\n");
  for (p = tab; p->name; p++)
    {
      printf ("\n\/\* %s %-20s*/", p->code, p->name);
      think (p);
    }
  printf ("0};\n");
  printf ("#endif\n");
}


m (ptr, a, rep)
     char **ptr;
     char *a;
     char *rep;
{
  int l = strlen (a);
  if (strncmp (*ptr, a, l) == 0)
    {
      printf ("%s", rep);
      *ptr += l;
      if (**ptr)
	printf (",");
    }
}

think (o)
     op *o;
{
  int t;
  char *n;
  char *p;

  printf ("{\"");
  n = o->name;
  while (*n && *n != ' ')
    {
      printf ("%c", *n);
      n++;
    }
  printf ("\",{");

  p = n;

  if (!*p)
    {
      printf ("0");
    }
  while (*p)
    {
      while (*p == ',' || *p == ' ')
	p++;
      m (&p, "#<imm>", "A_IMM");
      m (&p, "R0", "A_R0");
      m (&p, "<REG_N>", "A_REG_N");
      m (&p, "@<REG_N>+", "A_INC_N");
      m (&p, "@<REG_N>", "A_IND_N");
      m (&p, "@-<REG_N>", "A_DEC_N");
      m (&p, "<REG_M>", " A_REG_M");
      m (&p, "@<REG_M>+", "A_INC_M");
      m (&p, "@<REG_M>", "A_IND_M");
      m (&p, "@-<REG_M>", "A_DEC_M");
      m (&p, "@(<disp>,PC)", "A_DISP_PC");
      m (&p, "@(<disp>,<REG_M>)", "A_DISP_REG_M");
      m (&p, "@(<disp>,<REG_N>)", "A_DISP_REG_N");
      m (&p, "@(R0,<REG_N>)", "A_IND_R0_REG_N");
      m (&p, "@(R0,<REG_M>)", "A_IND_R0_REG_M");
      m (&p, "@(<disp>,GBR)", "A_DISP_GBR");
      m (&p, "@(R0,GBR)", "A_R0_GBR");
      m (&p, "<bdisp8>", "A_BDISP8");
      m (&p, "<bdisp12>", "A_BDISP12");
      m (&p, "SR", "A_SR");
      m (&p, "GBR", "A_GBR");
      m (&p, "VBR", "A_VBR");
      m (&p, "MACH", "A_MACH");
      m (&p, "MACL", "A_MACL");
      m (&p, "PR", "A_PR");

    }
  printf ("},{");

  p = o->code;
  while (*p)
    {
      m (&p, "0000", "HEX_0");
      m (&p, "0001", "HEX_1");
      m (&p, "0010", "HEX_2");
      m (&p, "0011", "HEX_3");
      m (&p, "0100", "HEX_4");
      m (&p, "0101", "HEX_5");
      m (&p, "0110", "HEX_6");
      m (&p, "0111", "HEX_7");

      m (&p, "1000", "HEX_8");
      m (&p, "1001", "HEX_9");
      m (&p, "1010", "HEX_A");
      m (&p, "1011", "HEX_B");
      m (&p, "1100", "HEX_C");
      m (&p, "1101", "HEX_D");
      m (&p, "1110", "HEX_E");
      m (&p, "1111", "HEX_F");
      m (&p, "i8*1....", "IMM_8");
      m (&p, "i4*1", "IMM_4");
      m (&p, "i8p4....", "PCRELIMM_8BY4");
      m (&p, "i8p2....", "PCRELIMM_8BY2");
      m (&p, "i8*2....", "IMM_8BY2");
      m (&p, "i4*2", "IMM_4BY2");
      m (&p, "i8*4....", "IMM_8BY4");
      m (&p, "i4*4", "IMM_4BY4");
      m (&p, "i12.........", "BRANCH_12");
      m (&p, "i8p1....", "BRANCH_8");
      m (&p, "nnnn", "REG_N");
      m (&p, "mmmm", "REG_M");

    }
  printf ("}},\n");
}

qfunc (a, b)
     op *a;
     op *b;
{
  char bufa[9];
  char bufb[9];
  memcpy (bufa, a->code, 4);
  memcpy (bufa + 4, a->code + 12, 4);
  bufa[8] = 0;

  memcpy (bufb, b->code, 4);
  memcpy (bufb + 4, b->code + 12, 4);
  bufb[8] = 0;
  return (strcmp (bufa, bufb));
}


sorttab ()
{
  op *p = tab;

  int len = 0;

  while (p->name)
    {
      p++;
      len++;
    }

  qsort (tab, len, sizeof (*p), qfunc);

}

/* Convert a string of 4 binary digits into an int */

static
int
bton (s)
     char *s;

{
  int n = 0;
  int v = 8;
  while (v)
    {
      if (*s == '1')
	n |= v;
      v >>= 1;
      s++;
    }
  return n;
}

static char table[1 << 16];

/* Take an opcode expand all varying fields in it out and fill all the 
  right entries in 'table' with the opcode index*/
    
expand_opcode (shift, val, i, s)
     char *s;
{
  int j;

  if (*s == 0)
    {
      table[val] = i;
    }
  else
    {
      switch (s[0])
	{

	case '0':
	case '1':
	  {

	    int n = bton (s);
	    if (n >= 0)
	      {
		expand_opcode (shift - 4, val | (n << shift), i, s + 4);
	      }
	    break;
	  }
	case 'n':
	case 'm':
	  for (j = 0; j < 16; j++)
	    {
	      expand_opcode (shift - 4, val | (j << shift), i, s + 4);

	    }
	  break;

	default:
	  for (j = 0; j < (1 << (shift + 4)); j++)
	    {
	      table[val | j] = i;
	    }
	}
    }
}

/* Print the jump table used to index an opcode into a switch
  statement entry */
static void

dumptable ()
{
  int lump = 256;
  int online = 16;

  int i = 0;

  while (i < 1 << 16)
    {
      int j = 0;
      int nc = 0;

      printf ("unsigned char sh_jump_table%x[%d]={\n", i, lump);

      while (j < lump)
	{
	  int k = 0;
	  while (k < online)
	    {
	      printf ("%2d", table[i + j + k]);
	      if (j + k < lump)
		printf (",");

	      k++;
	    }
	  j += k;
	  printf ("\n");
	}
      i += j;
      printf ("};\n");
    }

}


static void
filltable ()
{
  op *p;
  int index = 1;

  sorttab ();
  for (p = tab; p->name; p++)
    {
      p->index = index++;
      expand_opcode (12, 0, p->index, p->code);
    }
}

static void 
gensim ()
{
  op *p;
  int j;

  printf ("{\n");
  printf ("switch (jump_table[iword]) {\n");

  for (p = tab; p->name; p++)
    {
      int sextbit = -1;
   
      char *s = p->code;

      printf ("\/\* %s %s *\/\n", p->name, p->code);
      printf ("case %d:      \n", p->index);

      printf ("{\n");
      while (*s)
	{
	  switch (*s)
	    {
	    case '0':
	    case '1':
	    case '.':
	      s += 4;
	      break;
	    case 'n':
	      printf ("int n =  (iword >>8) & 0xf;\n");
	      s += 4;
	      break;
	    case 'm':
	      printf ("int m =  (iword >>4) & 0xf;\n");
	      s += 4;

	      break;

	    case 'i':
	      printf ("int i = (iword & 0x");

	      switch (s[1])
		{
		case '4':
		  printf ("f");
		  break;
		case '8':
		  printf ("ff");
		  break;
		case '1':
		  sextbit = 12;
		  
		  printf ("fff");
		  break;
		}
	      printf (")");

	      switch (s[3])
		{
		case '1':
		  break;
		case '2':
		  printf ("<<1");
		  break;
		case '4':
		  printf ("<<2");
		  break;
		}
	      printf (";\n");
	      s += 4;
	    }
	}
      if (sextbit>0) 
	{
	  printf("i = (i ^ (1<<%d))-(1<<%d);\n",sextbit-1,sextbit-1);
	}
      
      for (j = 0; j < 10; j++)
	{
	  if (p->stuff[j])
	    {
	      printf ("%s\n", p->stuff[j]);
	    }
	}
      printf ("break;\n", p->stuff);
      printf ("}\n");
    }
  printf ("}\n}\n");
}


int
main (ac, av)
     char **av;

{
  if (ac > 1)
    {
      if (strcmp (av[1], "-t") == 0)
	{
	  gengastab ();
	}
      else if (strcmp (av[1], "-s") == 0)
	{
	  filltable ();
	  dumptable ();

	}
      else if (strcmp (av[1], "-x") == 0)
	{
	  filltable ();
	  gensim ();
	}
    }
  else
    {
      genopc ();
    }
  return 0;
}
