/* BFD library support routines for the Hitachi H8/300 architecture.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Hacked by Steve Chamberlain of Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include"bfd.h"
#include"sysdep.h"

#define DEFINE_TABLE
#define INSIM
#include"opcode/h8300.h"

#define MAXSAME 14

#define PTWO 256
static struct h8_opcode *h8_opcodes_sorted[PTWO][MAXSAME];

/* Run through the opcodes and sort them into order to make them easy
   to disassemble
 */

char *cs = "/*";
char *ce = "*/";

char *nibs[] =
{
  "foo",
  "(b0&0xf)",
  "((b1>>4)&0xf)",
  "((b1)&0xf)",
  "((pc[2]>>4)&0xf)",
  "((pc[2])&0xf)",
  "((pc[3]>>4)&0xf)",
  "((pc[3])&0xf)",
  0, 0};

char *abs8[] =
{
  "foo",
  "foo",
  "b1",
  "foo",
};

#define sorted_key noperands
char *abs16[] =
{
  "foo",
  "foo",
  "foo",
  "foo",

  "(pc[2]<<8)|pc[3]",
  "foo",
  "foo",
  "foo",
};
char *empty="fake";
init ()
{
  unsigned int i;
  struct h8_opcode *p;

  for (p = h8_opcodes; p->name; p++)
  {
    int n1 = 0;
    int n2 = 0;
    int j;
    for (j = 0; p->data.nib[j] != E; j++)
    {
      if ((int)p->data.nib[j] == ABS16ORREL8SRC)
       p->data.nib[j] = ABS16SRC;
      if ((int)p->data.nib[j ] == ABS16OR8SRC)
       p->data.nib[j] = ABS16SRC;
      if ((int)p->data.nib[j] == ABS16OR8DST)
       p->data.nib[j] = ABS16DST;
    }
    /* Kill push and pop, they're duplicates */
    if (strcmp(p->name,"push")==0) {
      p->name = empty;
    }
    if (strcmp(p->name,"pop")==0) {
      p->name = empty;
    }


    if ((int) p->data.nib[0] < 16)
    {
      n1 = (int) p->data.nib[0];
    }
    else
     n1 = 0;
    if ((int) p->data.nib[1] < 16)
    {
      n2 = (int) p->data.nib[1];
    }
    else
     n2 = 0;
#if 0
    if ((int) p->data.nib[2] < 16)
    {
      n3 = (int) p->data.nib[2];
    }
    else if ((int) p->data.nib[2] & B30)
    {
      n3 = 0;
    }
    else if ((int) p->data.nib[2] & B31)
    {
      n3 = 0x8;
    }


    if ((int) p->data.nib[3] < 16)
    {
      n4 = (int) p->data.nib[3];
    }
    else if ((int) p->data.nib[3] & B30)
    {
      n4 = 0;
    }
    else if ((int) p->data.nib[3] & B31)
    {
      n4 = 0x8;
    }
#endif
    for (i = 0; i < MAXSAME; i++)
    {
      int j = /* ((n3 >> 3) * 512) + ((n4 >> 3) * 256) + */ n1 * 16 + n2;

      if (h8_opcodes_sorted[j][i] == (struct h8_opcode *) NULL)
      {
	h8_opcodes_sorted[j][i] = p;
	p->sorted_key = j;
	break;
      }
    }

    if (i == MAXSAME)
     abort ();

    /* Just make sure there are an even number of nibbles in it, and
       that the count is the same s the length */
    for (i = 0; p->data.nib[i] != E; i++)
     /*EMPTY*/ ;
    if (i & 1)
     abort ();
    p->length = i / 2;
  }
  for (i = 0; i < PTWO; i++)
  {
    if (h8_opcodes_sorted[i][0])
     p = h8_opcodes_sorted[i][0];
    else
     h8_opcodes_sorted[i][0] = p;
  }
}

unsigned int
DEFUN (bfd_h8_disassemble, (addr, data, stream),
       bfd_vma addr AND
       CONST bfd_byte * data AND
       FILE * stream)
{
  /* Find the first entry in the table for this opcode */
  CONST static char *regnames[] =
  {
    "r0h", "r1h", "r2h", "r3h", "r4h", "r5h", "r6h", "r7h",
    "r0l", "r1l", "r2l", "r3l", "r4l", "r5l", "r6l", "r7l"};

  int rs = 0;
  int rd = 0;
  int rdisp = 0;
  int abs = 0;
  struct h8_opcode **p = h8_opcodes_sorted[(unsigned) (data[0])];
  struct h8_opcode *q;

  /* Find the exact opcode/arg combo */
  while (*p)
    {
      op_type *nib;
      unsigned int len = 0;

      q = *p++;
      nib = q->data.nib;
      while (*nib != E)
	{
	  op_type looking_for = *nib;
	  int thisnib = data[len >> 1];

	  thisnib = (len & 1) ? (thisnib & 0xf) : ((thisnib >> 4) & 0xf);
	  if ((int) looking_for & (int) B31)
	    {
	      if (((int) thisnib & 0x8) == 0)
		goto fail;
	      looking_for = (op_type) ((int) looking_for & ~(int) B31);
	    }
	  if ((int) looking_for & (int) B30)
	    {
	      if (((int) thisnib & 0x8) != 0)
		goto fail;
	      looking_for = (op_type) ((int) looking_for & ~(int) B30);
	    }
	  switch (looking_for)
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	      if ((int) looking_for != thisnib)
		goto fail;
	      break;
	    case ABS16SRC:
	    case ABS16OR8SRC:
	    case ABS16ORREL8SRC:
	    case ABS16DST:
	    case ABS16OR8DST:
	    case DISPSRC:
	    case DISPDST:
	    case IMM16:
	      abs = (data[len >> 1]) * 256 + data[(len + 2) >> 1];
	      len += 3;
	      nib += 3;
	      break;
	    case DISPREG:
	      rdisp = thisnib;
	      break;
	    case KBIT:
	      abs = thisnib == 0x8 ? 2 : 1;
	      break;
	    case IMM8:
	    case ABS8SRC:
	    case ABS8DST:
	    case MEMIND:
	    case DISP8:
	      abs = data[len >> 1];
	      len++;
	      nib++;
	      break;
	    case IMM3:
	      abs = thisnib;
	      break;
	    case RS8:
	    case RS16:
	    case RSINC:
	    case RSIND:
	      rs = thisnib;
	      break;
	    case RD16:
	    case RDDEC:
	    case RD8:
	    case RDIND:
	      rd = thisnib;
	      break;
	    default:
	      fprintf (stream, "Dont understand \n");
	      goto found;
	    }
	  len++;
	  nib++;
	}
      goto found;
    fail:
      ;

    }
  fprintf (stream, "%02x %02x        .word\tH'%x,H'%x",
	   data[0], data[1],
	   data[0], data[1]);
  return 2;
found:;
  {
    int i;

    for (i = 0; i < q->length; i++)
      {
	fprintf (stream, "%02x", data[i]);
      }
    for (; i < 6; i++)
      {
	fprintf (stream, "  ");
      }
  }
  fprintf (stream, "%s\t", q->name);
  /* Now print out the args */
  {
    op_type *args = q->args.nib;
    int hadone = 0;

    while (*args != E)
      {
	if (hadone)
	  fprintf (stream, ",");
	switch ((int) (*args) & ~((int) B30 | (int) B31))
	  {
	  case IMM16:
	  case IMM8:
	  case IMM3:
	    fprintf (stream, "#0x%x", (unsigned) abs);
	    break;
	  case RD8:
	    fprintf (stream, "%s", regnames[rd]);
	    break;
	  case RS8:
	    fprintf (stream, "%s", regnames[rs]);
	    break;
	  case RD16:
	    fprintf (stream, "r%d", rd & 0x7);
	    break;
	  case RS16:
	    fprintf (stream, "r%d", rs & 0x7);
	    break;
	  case RSINC:
	    fprintf (stream, "@r%d+", rs & 0x7);
	    break;
	  case RDDEC:
	    fprintf (stream, "@-r%d", rd & 0x7);
	    break;
	  case RDIND:
	    fprintf (stream, "@r%d", rd & 0x7);
	    break;
	  case RSIND:
	    fprintf (stream, "@r%d", rs & 0x7);
	    break;
	  case ABS8SRC:
	  case ABS16SRC:
	  case ABS16OR8SRC:
	  case ABS16ORREL8SRC:
	  case ABS16OR8DST:
	  case ABS16DST:
	  case ABS8DST:
	    fprintf (stream, "@0x%x", (unsigned) abs);
	    break;
	  case MEMIND:
	    fprintf (stream, "@@%d (%x)", abs, abs);
	    break;
	  case DISP8:
	    fprintf (stream, ".%s%d (%x)", (char) abs > 0 ? "+" : "", (char) abs,
		     addr + (char) abs + 2);
	    break;
	  case DISPSRC:
	  case DISPDST:
	    fprintf (stream, "@(0x%x,r%d)", abs, rdisp & 0x7);
	    break;
	  case CCR:
	    fprintf (stream, "ccr");
	    break;
	  case KBIT:
	    fprintf (stream, "#%d", abs);
	    break;
	  default:
	    abort ();
	  }
	hadone = 1;
	args++;
      }
  }
  return q->length;
}

void
decode (p, fetch, size)
     struct h8_opcode *p;
     int fetch;
     int size;
{
  int i;
  char *ss = size == 8 ? "BYTE" : "WORD";

  for (i = 0; p->data.nib[i] != E; i++)
  {
    switch (p->data.nib[i])
    {
     case RS8:
      if (fetch)
       printf ("srca = BYTE_REG(%s);\n", nibs[i]);
      break;
     case RS16 | B30:
     case RS16 | B31:
     case RS16:
      if (fetch)
       printf ("srca = WORD_REG(%s);\n", nibs[i]);
      break;
     case RD8:
      if (fetch)
       printf ("srcb = BYTE_REG(%s);\n", nibs[i]);
      else
       printf ("SET_BYTE_REG(%s,dst);\n", nibs[i]);
      break;
     case RD16 | B30:
     case RD16 | B31:
     case RD16:
      if (fetch)
       printf ("srcb = WORD_REG(%s);\n", nibs[i]);
      else
       printf ("SET_WORD_REG(%s,dst);\n", nibs[i]);
      break;
     case IMM8:
      if (fetch)
       printf ("srca = b1;\n", i);
      break;
     case RSINC:
     case RSINC | B30:
     case RSINC | B31:

      if (fetch)
      {
	printf("rn = %s &0x7;\n", nibs[i]);
	printf ("srca = %s_MEM(reg[rn]);\n", size == 8 ? "BYTE"
		: "WORD")	 ;
	printf("reg[rn]+=%d;\n", size/8);
      }
      break;
     case RSIND:
     case RSIND | B30:
     case RSIND | B31:
      if (fetch) {
	printf("lval = WORD_REG(%s);\n", nibs[i]);
	printf("srca = %s_MEM(lval);\n", ss);
      }
      break;

     case RDIND:
     case RDIND | B30:
     case RDIND | B31:
      if (fetch) {
	printf("rn = %s & 0x7;\n", nibs[i] );
	printf("lval = reg[rn];\n");
	printf("srcb = %s_MEM(lval);\n", size==8 ? "BYTE" : "WORD");
      }
      else 
      {
	printf("SET_%s_MEM(lval,dst);\n",ss);
      }
      break;

     case MEMIND:
      if (fetch)
      {
	printf("lval = WORD_MEM(b1);\n");
      }
      break;
     case RDDEC:
     case RDDEC | B30:
     case RDDEC | B31:
      if (!fetch)
      {
	printf("rn = %s & 0x7;\n", nibs[i]);
	printf("reg[rn]-=%d;\n", size/8);
	printf ("SET_%s_MEM(reg[rn], dst);\n",
		size == 8 ? "BYTE" : "WORD");
      }
      break;

     case IMM16:
      if (fetch)
       printf ("srca =WORD_IMM;\n", i);
      break;
     case ABS8SRC:
      if (fetch) {

	printf("lval = (0xff00) + b1;\n");	    
	printf ("srca = BYTE_MEM(lval);\n");
      }

      break;
     case ABS8DST:
      if (fetch)
      {
	printf("lval = (0xff00) + b1;\n");	    
	printf ("srcb = BYTE_MEM(lval);\n");
      }
      else
      {
	printf ("SET_BYTE_MEM(lval,dst);\n");
      }
      break;
     case KBIT:
      if (fetch)
       printf ("srca = KBIT(%d);\n", i);
      break;
     case ABS16ORREL8SRC:
     case ABS16SRC:
      if (fetch) 
      {
	printf("lval = ((pc[2] << 8) + pc[3]);\n");	    
	printf("srca = %s_MEM(lval);\n", size==8 ? "BYTE" : "WORD");
      }
      break;
     case DISPREG|B30:
     case DISPREG|B31:
     case DISPREG:
      printf("rn = %s & 0x7;\n", nibs[i]);
      break;
     case DISPSRC:
      if (fetch)
      {
	printf("lval = 0xffff&((pc[2] << 8) + pc[3] +reg[rn]);\n");	    
	printf("srca = %s_MEM(lval);\n", size == 8 ? "BYTE" : "WORD");
      }
      break;
     case DISPDST:
      if (fetch)
      {
	printf("lval = 0xffff&((pc[2] << 8) + pc[3] +reg[rn]);\n");	    
	printf("srcb = %s_MEM(lval);\n", size == 8 ? "BYTE" : "WORD");
      }
      else {
	printf("SET_%s_MEM(lval,dst);\n",ss);
      }
      break;
     case ABS16DST:
      if (fetch)
      {
	printf("lval = ((pc[2] << 8) + pc[3]);\n");	    
	printf("srcb = WORD_MEM(lval);\n");
      }
      else
      {
	printf ("SET_WORD_MEM(lval,dst);\n", i);
      }
      break;
     case IGNORE:
      break;
     case DISP8:
      printf("			/* DISP8 handled in opcode */\n");
      break;
     default:
      if (p->data.nib[i] > HexF)
      {
	printf("exception = SIGILL;\n");
      }
    }
  }
}

#define NNIBS 8

typedef int keytype[NNIBS];
struct key
{
  keytype k;
  int nmatch;
/*  struct h8_opcode *match[200];*/
};

struct key keys[50];
int nkeys;

calckey (p, key)
struct h8_opcode *p;
 keytype key;
{
  int j;

  for (j = 0; (p->data.nib[j] != E); j++)
    {
      if (p->data.nib[j] == RS8 || p->data.nib[j] == RD8)
	p->size = 8;
      if (p->size != 8)
       p->size=16;
      if (p->data.nib[j] > HexF)
	key[j] = p->data.nib[j];
      else 
	key[j] = 0;
    }
  for (; j < NNIBS; j++)
    key[j] = 0;
}

lookup (key)
keytype key;
{
  int n;

  /* See if this key matches one already found */
  for (n = 0; n < nkeys; n++)
    {
      int i;

      for (i = 0; i < NNIBS; i++)
	{
	  if ((keys[n].k[i]&0x3f) != (key[i]&0x3f))
	    break;
	}
      if (i == NNIBS)
	break;
    }
  return n;
}
BADNEWS(x) {
if (x == 0x7c) return 0;
if (x == 0x7d) return 0;
if (x == 0x7e) return 0;
if (x == 0x7f) return 0;
return 1;
}

sortmodes ()
{
  /* look through all the addressing modes, work out which ones are unique*/
  struct h8_opcode **p;
  keytype key;

  int n;
  int i;

  memset (keys, 0, sizeof (keys));


  for (i = 0; i < PTWO; i++)
    {
      p = h8_opcodes_sorted[i];
      while (*p)
	{
	  int j;
	  int a = 0;

if (BADNEWS((*p)->sorted_key)) {
	  calckey (*p, key);
	  n = lookup (key);
	  if (n == nkeys)
	    {
	      /* No hit, insert */
	      memcpy (keys[n].k, key, sizeof (keys[n].k));
	      nkeys++;
	    }
	  /* Link in the p */
#if 0
/*e*/	  keys[n].match[keys[n].nmatch++] = *p;
#endif
	}
	  (*p)->idx = n;
	  p++;
	}
    }

#if 0
  for (n = 0; n < nkeys; n++)
    {
      printf ("%d %d %d %d %d %d %d %d\n", keys[n][0], keys[n][1], keys[n][2], keys[n][3],
	      keys[n][4], keys[n][5], keys[n][6], keys[n][7]);

    }
#endif

}
void
enop ()
{
  printf ("%s Nop does nothing %s", cs, ce);
}

void
esleep ()
{
  printf ("exception = SIGSTOP;\n");
}

rv (x)
{
  printf ("v= 0;\n");
}

void
mov (p, s, sz)
     struct h8_opcode *p;
     char *s;
     int sz;
{
  printf("dst = srca;\n");
}

tn (s)
{
/*  printf ("n = dst & 0x%x;\n", s == 8 ? 0x80 : 0x8000);*/
}

tz (s)
{
/*  printf ("z = ~(dst & 0x%x);\n", s == 8 ? 0xff : 0xffff);*/
}

tv (s)
{
/*  printf ("v =  (( (~(srca ^ srcb)  & (srca ^ dst))) & (1<<%d));\n", s);*/
}

tc (s)
{
  printf ("c = dst & 0x%x;\n", 1 << s);
}

log_flags (s)
{
  tn (s);
  tz (s);
  rv (s);
}

alu_flags (s)
{
  tn (s);
  tz (s);
  tv (s);
  tc (s);
}

void
bclr (p)
     struct h8_opcode *p;
{
  printf ("dst = srca & ~(1<<srcb);\n");
}

void
biand (p)
     struct h8_opcode *p;
{
  printf ("c =(C && ! ((srca >> srcb)&1));\n");
}

void
bild (p)
     struct h8_opcode *p;
{
  printf ("c = ( ! ((srca >> srcb)&1));\n");
}


void
andc (p)
     struct h8_opcode *p;
{
  printf ("SET_CCR(GET_CCR() & srca);\n");
}

void
addx (p)
     struct h8_opcode *p;
{
  printf ("dst = srca + srcb+ (c != 0);\n");
}

void
subx (p)
     struct h8_opcode *p;
{
  printf ("dst = srcb - srca - (c != 0);\n");
}

void
add (p, s, sz)
     struct h8_opcode *p;
     char *s;
     int sz;
{
  printf ("%s;\n", s);
  alu_flags (sz);

}

void
adds (p, s)
     struct h8_opcode *p;
     char *s;
{
  printf ("%s;\n", s);
}

void
bra (p, a)
     struct h8_opcode *p;
     char *a;
{
  printf ("if (%s) npc += (((char *)pc)[1]);\n", a);
}


void
bsr (p, a)
     struct h8_opcode *p;
     char *a;
{
  printf("reg[7]-=2;\n");
  printf("tmp = reg[7];\n");
  printf ("SET_WORD_MEM(tmp, npc-mem);\n");
  printf ("npc += (((char *)pc)[1]);\n");
}

void
cmp (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  decode(p, 1, s);
printf("srca = -srca;\n");
  printf ("dst = srca + srcb;\n");
}

void
jsr (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf("if (b1 == 0xc4) {\n");
  printf("printf(\"%%c\", reg[2]);\n");
  printf("}\n");
  printf("else {\n");
  printf("reg[7]-=2;\n");
  printf("tmp = reg[7];\n");
  printf ("SET_WORD_MEM(tmp, npc-mem);\n");
  printf ("npc = lval + mem;\n");
  printf("}");
}

void
jmp (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf ("npc = lval + mem;\n");
}

void
rts (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf ("tmp = reg[7];\n");
  printf ("reg[7]+=2;\n");
  printf ("npc = mem + WORD_MEM(tmp);\n");
}

void
setf (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf ("tmp = GET_CCR();\n");
  printf ("tmp %s= srca;\n",a);
}

void
bpt (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf ("exception = SIGTRAP;\n");
  printf( "npc = pc;\n");
}

void
log (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf ("dst = srcb %s srca;\n", a);
}

void
ulog (p, a, s)
     struct h8_opcode *p;
     char *a;
     int s;
{
  printf ("dst = %s srcb ;\n", a);
}

void
nop()
{
}

void
rotl()
{
printf("c = srcb & 0x80;\n");
printf("dst = srcb << 1;\n");
printf("if (c) dst|=1;\n");
}
void
rotr()
{
printf("c = srcb & 1;\n");
printf("dst = srcb >> 1;\n");
printf("if (c) dst|=0x80;\n");
}

void
rotxl()
{
printf("tmp = srcb & 0x80;\n");
printf("dst = srcb << 1;\n");
printf("if (c) dst|=1;\n");
printf("c = tmp;\n");
}
void
rotxr()
{
printf("tmp = srcb & 1;\n");
printf("dst = srcb >> 1;\n");
printf("if (c) dst|=0x80;\n");
printf("c = tmp;\n");
}


void
shal()
{
printf("c = srcb&0x80;\n");
printf("dst = srcb << 1;\n");
}

void
shar()
{
printf("c = srcb&0x1;\n");
printf("if (srcb&0x80) dst = (srcb>>1) | 0x80;\n");
printf("else  dst = (srcb>>1) &~ 0x80;\n");
}

void
shll()
{
printf("c = srcb&0x80;\n");
printf("dst = srcb << 1;\n");
}

void
shlr()
{
printf("c = srcb&0x1;\n");
printf("dst = (srcb>>1) &~ 0x80;\n");
}

void
divxu()
{
  printf("srca = BYTE_REG(pc[1]>>4);\n");
  printf("srcb = WORD_REG(pc[1]&0x7);\n");
  printf("n = srca & 0x80\;\n");
  printf("z = !srca;\n");
  printf("if (srca) dst = srcb / srca;tmp = srcb %% srca;\n");
  printf("reg[pc[1]&0x7] = (dst & 0xff) | (tmp << 8);\n");
}
void
mulxu()
{
  printf("srca = BYTE_REG(pc[1]>>4);\n");
  printf("srcb = WORD_REG(pc[1]&0x7);\n");
  printf("dst = (srcb&0xff) * srca;\n");
  printf("reg[pc[1]&0x7] = dst;\n");
}
char saf[]="goto setflags;";
char sf[] = "goto shiftflags;";
char af8[] = "goto aluflags8;";
char af16[] = "goto aluflags16;";
char lf[] = "goto logflags;";
char mf8[]= "goto movflags8;";
char mf16[]= "goto movflags16;";
char nx[] = "goto next;";
struct
{
char *ftype;
  int decode;
  char *name;
  void (*func) ();
  char *arg;
  int size;

}


table  [] =
{
  saf,1,"orc",setf,"|",0,
  saf,1,"xorc",setf,"^",0,
  saf,1,"andc",setf,"&",0,
  0,1,  "bild", bild, 0, 0,
  0,1,  "nop", nop, 0, 0,
  0,1,  "bclr", bclr, 0, 0,
  0,1,  "biand", biand, 0, 0,
  0,1,  "bra", bra, "1", 0,
  0,1,  "brn", bra, "0", 0,
  0,1,  "bf", bra, "0", 0,
  0,1,  "bhi", bra, "(C||Z)==0", 0,
  0,1,  "bls", bra, "(C||Z)==1", 0,
  0,1,  "bcs", bra, "C==1", 0,
  0,1,  "bcc", bra, "C==0", 0,
  0,1,  "bpl", bra, "N==0", 0,
  0,1,  "bmi", bra, "N==1", 0,
  0,1,  "bvs", bra, "V==1", 0,
  0,1,  "bvc", bra, "V==0", 0,
  0,1,  "bge", bra, "(N^V)==0", 0,
  0,1,  "bgt", bra, "(Z|(N^V))==0", 0,
  0,1,  "blt", bra, "(N^V)==1", 0,
  0,1,  "ble", bra, "(Z|(N^V))==1", 0,
  0,1,  "beq", bra, "Z==1", 0,
  0,1,  "bne", bra, "Z==0", 0,
  0,1,  "bsr", bsr, "", 0,
  0,1,  "jsr", jsr, 0, 0,
  0,1,  "jmp", jmp, 0, 0,
  0,0,  "rts", rts, 0, 0,
  0,1,  "andc", andc, 0, 0,
  0,1,  "nop", enop, 0, 0,
  sf,1, "shal",shal,0,0,
  sf,1, "shar",shar,0,0,
  sf,1, "shll",shll,0,0,
  sf,1, "shlr",shlr,0,0,
  sf,1, "rotxl",rotxl,0,0,
  sf,1, "rotxr",rotxr,0,0,
  sf,1, "rotl",rotl,0,0,
  sf,1, "rotr",rotr,0,0,
  lf,1, "xor", log, "^",0,
  lf,1, "and", log, "&",0,
  lf,1, "or",  log, "|",0,
  lf,1,"not", ulog," ~",0,  
  lf,1,"neg", ulog," - ",0,  
  0,1,  "adds", adds, "dst = srca + srcb", 0,
  0,1,  "subs", adds, "srca = -srca; dst = srcb + srca", 0,
  af8,1,  "add.b", add, "dst = srca + srcb", 8,
  af16,1,  "add.w", add, "dst = srca + srcb", 16,
  af16,1,  "sub.w", add, "srca = -srca; dst = srcb + srca", 16,
  af8,1,  "sub.b", add, "srca = -srca; dst = srcb + srca", 8,
  af8,1,  "addx", addx, 0, 8,
  af8,1,  "subx", subx, 0, 8,
  af8,0,  "cmp.b", cmp, 0, 8,
  af16,0,  "cmp.w", cmp, 0, 16,
  0,1,  "sleep", esleep, 0, 0,
  0,0,  "bpt", bpt, 0, 8,
  nx,0, "divxu", divxu,0,0,
  nx,0, "mulxu", mulxu,0,0,
  mf8,1,  "mov.b", mov, 0, 8,
  mf16,1,  "mov.w", mov, 0, 16,
  0, 0
 };

edo (p)
     struct h8_opcode *p;
{
  int i;

  printf ("%s %s %s\n", cs, p->name, ce);

  for (i = 0; table[i].name; i++)
  {
    if (strcmp (table[i].name, p->name) == 0)
    {
      printf ("{\n");
      if (table[i].decode) decode(p, 1,p->size);
      printf ("cycles += %d;\n", p->time);
      printf ("npc = pc + %d;\n", p->length);
      table[i].func (p, table[i].arg, table[i].size);
      if (table[i].decode) decode(p, 0,p->size);
      if (table[i].ftype) printf(table[i].ftype);
      else printf ("goto next;\n");
      printf ("}\n");
      return;
    }
  }
  printf ("%s not found %s\n", cs, ce);
	printf("exception = SIGILL;\n");
  printf ("break;\n");
}

int
owrite (i)
{
  /* write if statements to select the right opcode */
  struct h8_opcode **p;
  int needand = 1;

  p = h8_opcodes_sorted[i];
  printf ("case 0x%03x:\n", i);

  if (p[1] == 0)
    {
      /* See if the next few also match */
      while (h8_opcodes_sorted[i + 1][0] == *p)
	{
	  i++;
	  printf ("case 0x%03x:\n", i);
	}

      /* Dont need any if's this is the only one */
      edo (*p);
    }
  else
    {
      while (*p)
	{
	  /* start two nibbles in since we know we match in the first byte */
	  int c;
	  int nib = 2;
	  int byte = 1;
	  int mask1[5];
	  int mask0[5];
	  int nibshift = 4;
	  int any = 0;

	  for (c = 0; c < 5; c++)
	    {
	      mask1[c] = 0;
	      mask0[c] = 0;
	    }
	  printf ("%s %x%x", cs, (*p)->data.nib[0], (*p)->data.nib[1]);
	  while ((c = (*p)->data.nib[nib]) != E)
	    {
	      if (c & B30)
		{
		  /* bit 3 must be zero */
		  mask0[byte] |= 0x8 << nibshift;
		  printf ("0");
		  any = 1;
		}
	      else if (c & B31)
		{
		  /* bit 3 must be one */
		  mask1[byte] |= 0x8 << nibshift;
		  printf ("8");
		  any = 1;
		}
	      else if (c <= HexF)
		{
		  mask0[byte] |= ((~c) & 0xf) << nibshift;
		  mask1[byte] |= (c & 0xf) << nibshift;
		  printf ("%x", c);
		  any = 1;
		}
	      else
		{
		  printf ("x");
		}
	      nib++;
	      if (nibshift == 4)
		{
		  nibshift = 0;
		}
	      else
		{
		  byte++;
		  nibshift = 4;
		}
	    }
	  printf ("*/\n");
	  if (any)
	    {
	      printf ("if (");
	      needand = 0;
	      for (c = 1; c < byte; c++)
		{
		  if (mask0[c] | mask1[c])
		    {
		      if (needand)
			printf ("\n&&");
		      printf ("((pc[%d]&0x%02x)==0x%x)",
			      c, mask0[c] | mask1[c], mask1[c]);
		      needand = 1;
		    }
		}
	      printf (")\n");
	    }
	  edo (*p);
	  p++;

	}
    }
  return i;
}

print_key (i)
{
  int j;

  printf ("%s", cs);
  for (j = 0; j < 8; j++)
    {
/*      printf ("(0x%02x ", keys[i].k[j]);*/
      if (keys[i].k[j] & B30)
	printf ("B30");
      else if (keys[i].k[j] & B31)
	printf ("B31");
      switch (keys[i].k[j])
	{
	case KBIT:
	  printf ("KBIT");
	  break;
	case IMM3:
	  printf ("IMM3");
	  break;
	case RD8:
	  printf ("RD8");
	  break;
	case RD16:
	  printf ("RD16");
	  break;
	case RS8:
	  printf ("RS8");
	  break;
	case RS16:
	  printf ("RS16");
	  break;
	case IMM8:
	  printf ("IMM8");
	  break;
	case IMM16:
	  printf ("IMM16");
	  break;
	case CCR:
	  printf ("CCR");
	  break;
	case ABS8SRC:
	  printf ("ABS8SRC");
	  break;
	case ABS8DST:
	  printf ("ABS8SDST");
	  break;
	case DISP8:
	  printf ("DISP8");
	  break;
	case ABS16SRC:
	  printf ("ABS16SRC");
	  break;
	case ABS16OR8SRC:
	  printf ("ABS16OR8SRC");
	  break;
	case ABS16DST:
	  printf ("ABS16SDST");
	  break;
	case ABS16OR8DST:
	  printf ("ABS16OR8SDST");
	  break;
	case DISPSRC:
	  printf ("DISPSRC");
	  break;
	case DISPDST:
	  printf ("DISPSDST");
	  break;
	case DISPREG:
	  printf ("DISPREG");
	  break;
	case RDDEC:
	  printf ("RDDEC");
	  break;
	case RSINC:
	  printf ("RSINC");
	  break;
	case RDIND:
	  printf ("RDIND");
	  break;
	case RSIND:
	  printf ("RSIND");
	  break;
	case MEMIND:
	  printf ("MEMIND");
	  break;
	case ABS16ORREL8SRC:
	  printf ("ABS16ORREL8SRC");
	  break;
	case IGNORE:
	  printf ("IGNORE");
	  break;
	default:
	  printf ("**");
	  break;
	}
      printf (" ");
    }
  printf ("%s\n", ce);
}

dofetch (fetch)
int fetch;
{
  int i;
  int done_for[PTWO];
  memset (done_for, 0, sizeof (done_for));
  printf ("switch (b0) {\n");
  for (i = 0; i < nkeys; i++)
  {
    struct h8_opcode *p;
    struct h8_opcode *l = 0;
    int k;

    printf ("%s KEY %d %s\n", cs,i,ce);
    print_key (i);

   {
     int j;

     l = 0;
     for (p = h8_opcodes; p->name; p++)
     {
       int key[8];

       if (p->idx == i && p->name != empty)
       {
	 l = p;
	 if (done_for[p->sorted_key] != i + 1000)
	 {
	   done_for[p->sorted_key] = i + 1000;
	   printf ("case 0x%03x:\n", p->sorted_key);
	   printf("%s %s %s\n", cs, p->name, ce);
	 }
       }
     }
     if (l)
     {

       decode (l, fetch, l->size);
       printf ("break;\n");
     }
     else
      printf ("			/* NO */\n");
   }
  }
  printf ("}\n");
}

remove_dups()
{
  struct h8_opcode *s;
  struct h8_opcode *d;
  for (d = s = h8_opcodes; s->name; s++)
  {
    int doit = 1;
    if (strcmp(s->name,"push") ==0) doit = 0;
    if (strcmp(s->name,"pop") ==0) doit = 0;
    if (doit)
    { 
      *d++ = *s; 
    }
  }
  *d++ = *s++;
}
main ()
{
  int i;
remove_dups();
  init ();

  sortmodes ();

#if 0
  printf("%s fetch the args %s\n", cs,ce);
  dofetch (1);
#endif
  printf("%s do the operation %s\n", cs,ce);
  printf ("switch (b0) \n{\n");
  for (i = 0; i < PTWO; i++)
    {
      i = owrite (i);
    }
  printf ("}\n");
#if 0
  printf("%s store the result %s\n", cs,ce);
  dofetch (0);
#endif

  return 0;
}
