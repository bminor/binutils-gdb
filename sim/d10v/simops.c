#include "d10v_sim.h"
#include "simops.h"

/* #define DEBUG 1 */

/* abs */
void
OP_4607 ()
{
#ifdef DEBUG
  printf("   abs\tr%d\n",OP[0]);
#endif
  State.F1 = State.F0;
  if ((int16)(State.regs[OP[0]]) < 0)
    {
      State.regs[OP[0]] = -(int16)(State.regs[OP[0]]);
      State.F0 = 1;
    }
  else
    State.F0 = 0;
}

/* abs */
void
OP_5607 ()
{
  int64 tmp;

#ifdef DEBUG
  printf("   abs\ta%d\n",OP[0]);
#endif
  State.F1 = State.F0;  
  if (State.a[OP[0]] & BIT40 )
    {
      tmp = -State.a[OP[0]] & MASK40;
      if (State.ST)
	{
	  if ( !(tmp & BIT40) && (tmp > MAX32))
	    State.a[OP[0]] = MAX32;
	  else if ( (tmp & BIT40) && (tmp < MIN32))
	    State.a[OP[0]] = MIN32;
	  else
	    State.a[OP[0]] = tmp;
	}
      else
	State.a[OP[0]] = tmp;
      State.F0 = 1;
    }
  else
    State.F0 = 0;
}

/* add */
void
OP_200 ()
{
  uint16 tmp = State.regs[OP[0]];
#ifdef DEBUG 
  printf("   add\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] += State.regs[OP[1]];
  if ( tmp > State.regs[OP[0]])
    State.C = 1;
  else
    State.C = 0;
}

/* add */
void
OP_1201 ()
{
#ifdef DEBUG
  printf("   add\ta%d,r%d\n",OP[0],OP[1]);
#endif
}

/* add */
void
OP_1203 ()
{
#ifdef DEBUG
printf("   add\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* add2w */
void
OP_1200 ()
{
  uint32 tmp;
  uint32 tmp1 = (State.regs[OP[0]]) << 16 | State.regs[OP[0]+1];
  uint32 tmp2 = (State.regs[OP[1]]) << 16 | State.regs[OP[1]+1];
#ifdef DEBUG 
  printf("   add2w\tr%d,r%d\n",OP[0],OP[1]);
#endif
  tmp = tmp1 + tmp2;
  if ( (tmp < tmp1) || (tmp < tmp2) )
    State.C = 1;
  else
    State.C = 0;
  State.regs[OP[0]] = tmp >> 16;
  State.regs[OP[0]+1] = tmp & 0xFFFF;
}

/* add3 */
void
OP_1000000 ()
{
  uint16 tmp = State.regs[OP[0]];
#ifdef DEBUG
  printf("   add3\tr%d,r%d,0x%x\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = State.regs[OP[1]] + OP[2];
  if ( tmp > State.regs[OP[0]])
    State.C = 1;
  else
    State.C = 0;
}

/* addac3 */
void
OP_17000200 ()
{
#ifdef DEBUG
printf("   addac3\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* addac3 */
void
OP_17000202 ()
{
#ifdef DEBUG
printf("   addac3\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* addac3s */
void
OP_17001200 ()
{
#ifdef DEBUG
printf("   addac3s\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* addac3s */
void
OP_17001202 ()
{
#ifdef DEBUG
printf("   addac3s\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* addi */
void
OP_201 ()
{
#ifdef DEBUG
  printf("   addi\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] += OP[1];
}

/* and */
void
OP_C00 ()
{
#ifdef DEBUG
  printf("   and\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] &= State.regs[OP[1]];
}

/* and3 */
void
OP_6000000 ()
{
#ifdef DEBUG
  printf("   and3\tr%d,r%d,0x%x\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = State.regs[OP[1]] & OP[2];
}

/* bclri */
void
OP_C01 ()
{
#ifdef DEBUG
  printf("   bclri\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] &= ~(0x8000 >> OP[1]);
}

/* bl.s */
void
OP_4900 ()
{
#ifdef DEBUG
  printf("   bl.s\t0x%x\n",OP[0]);
#endif
  State.regs[13] = PC+1;
  PC += SEXT8 (OP[0]);
}

/* bl.l */
void
OP_24800000 ()
{
#ifdef DEBUG
  printf("   bl.l\t0x%x\n",OP[0]);
#endif
  State.regs[13] = PC+1;
  PC += OP[0];
}

/* bnoti */
void
OP_A01 ()
{
#ifdef DEBUG
  printf("   bnoti\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] ^= 0x8000 >> OP[1];
}

/* bra.s */
void
OP_4800 ()
{
#ifdef DEBUG
  printf("   bra.s\t0x%x\n",OP[0]);
#endif
  PC += SEXT8 (OP[0]);
}

/* bra.l */
void
OP_24000000 ()
{
#ifdef DEBUG
  printf("   bra.l\t0x%x\n",OP[0]);
#endif
  PC += OP[0];
}

/* brf0f.s */
void
OP_4A00 ()
{
#ifdef DEBUG
  printf("   brf0f.s\t0x%x\n",OP[0]);
#endif
  if (State.F0 == 0)
    PC += SEXT8 (OP[0]);
}

/* brf0f.l */
void
OP_25000000 ()
{
#ifdef DEBUG
  printf("   brf0f.l\t0x%x\n",OP[0]);
#endif
  if (State.F0 == 0)
    PC += OP[0];
}

/* brf0t.s */
void
OP_4B00 ()
{
#ifdef DEBUG
  printf("   brf0t.s\t0x%x\n",OP[0]);
#endif
  if (State.F0)
    PC += SEXT8 (OP[0]);
}

/* brf0t.l */
void
OP_25800000 ()
{
#ifdef DEBUG
  printf("   brf0t.l\t0x%x\n",OP[0]);
#endif
  if (State.F0)
    PC += OP[0];
}

/* bseti */
void
OP_801 ()
{
#ifdef DEBUG
  printf("   bseti\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] |= 0x8000 >> OP[1];
}

/* btsti */
void
OP_E01 ()
{
#ifdef DEBUG
  printf("   btsti\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] & (0x8000 >> OP[1])) ? 1 : 0;
}

/* clrac */
void
OP_5601 ()
{
#ifdef DEBUG
  printf("   clrac\ta%d\n",OP[0]);
#endif
  State.a[OP[0]] = 0;
}

/* cmp */
void
OP_600 ()
{
#ifdef DEBUG
  printf("   cmp\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = ((int16)(State.regs[OP[0]]) < (int16)(State.regs[OP[1]])) ? 1 : 0;  
}

/* cmp */
void
OP_1603 ()
{
#ifdef DEBUG
printf("   cmp\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* cmpeq */
void
OP_400 ()
{
#ifdef DEBUG
  printf("   cmpeq\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] == State.regs[OP[1]]) ? 1 : 0;  
}

/* cmpeq */
void
OP_1403 ()
{
#ifdef DEBUG
printf("   cmpeq\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* cmpeqi.s */
void
OP_401 ()
{
#ifdef DEBUG
  printf("   cmpeqi.s\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] == SEXT4(OP[1])) ? 1 : 0;  
}

/* cmpeqi.l */
void
OP_2000000 ()
{
#ifdef DEBUG
  printf("   cmpeqi.l\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] == OP[1]) ? 1 : 0;  
}

/* cmpi.s */
void
OP_601 ()
{
#ifdef DEBUG
  printf("   cmpi.s\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = ((int16)(State.regs[OP[0]]) < SEXT4(OP[1])) ? 1 : 0;  
}

/* cmpi.l */
void
OP_3000000 ()
{
#ifdef DEBUG
  printf("   cmpi.l\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = ((int16)(State.regs[OP[0]]) < (int16)(OP[1])) ? 1 : 0;  
}

/* cmpu */
void
OP_4600 ()
{
#ifdef DEBUG
  printf("   cmpu\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] < State.regs[OP[1]]) ? 1 : 0;  
}

/* cmpui */
void
OP_23000000 ()
{
#ifdef DEBUG
  printf("   cmpui\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] < OP[1]) ? 1 : 0;  
}

/* cpfg */
void
OP_4E09 ()
{
  uint8 *src, *dst;
#ifdef DEBUG
  printf("   cpfg\t%x,%x\n",OP[0],OP[1]);
#endif
  
  if (OP[0] == 0)
    dst = &State.F0;
  else
    dst = &State.F1;
  
  if (OP[1] == 0)
    src = &State.F0;
  else if (OP[1] == 1)
    src = &State.F1;
  else
    src = &State.C;

  *dst = *src;
}

/* dbt */
void
OP_5F20 ()
{
#ifdef DEBUG
  printf("   dbt\n");
#endif
}

/* divs */
void
OP_14002800 ()
{
  uint16 foo, tmp, tmpf;
#ifdef DEBUG
  printf("   divs\tr%d,r%d\n",OP[0],OP[1]);
#endif
  foo = (State.regs[OP[0]] << 1) | (State.regs[OP[0]+1] >> 15);
  tmp = (int16)foo - (int16)(State.regs[OP[1]]);
  tmpf = (foo >= State.regs[OP[1]]) ? 1 : 0;
  State.regs[OP[0]] = (tmpf == 1) ? tmp : foo;
  State.regs[OP[0]+1] = (State.regs[OP[0]+1] << 1) | tmpf;
}

/* exef0f */
void
OP_4E04 ()
{
#ifdef DEBUG
  printf("   exef0f\n");
#endif
  State.exe = (State.F0) ? 0 : 1;
}

/* exef0t */
void
OP_4E24 ()
{
#ifdef DEBUG
  printf("   exef0t\n");
#endif
  State.exe = State.F0;
}

/* exef1f */
void
OP_4E40 ()
{
#ifdef DEBUG
  printf("   exef1f\n");
#endif
  State.exe = (State.F1) ? 0 : 1;
}

/* exef1t */
void
OP_4E42 ()
{
#ifdef DEBUG
  printf("   exef1t\n");
#endif
  State.exe = State.F1;
}

/* exefaf */
void
OP_4E00 ()
{
#ifdef DEBUG
  printf("   exefaf\n");
#endif
  State.exe = (State.F0 | State.F1) ? 0 : 1;
}

/* exefat */
void
OP_4E02 ()
{
#ifdef DEBUG
  printf("   exefat\n");
#endif
  State.exe = (State.F0) ? 0 : (State.F1); 
}

/* exetaf */
void
OP_4E20 ()
{
#ifdef DEBUG
  printf("   exetaf\n");
#endif
  State.exe = (State.F1) ? 0 : (State.F0);
}

/* exetat */
void
OP_4E22 ()
{
#ifdef DEBUG
  printf("   exetat\n");
#endif
  State.exe = (State.F0) ? (State.F1) : 0;
}

/* exp */
void
OP_15002A00 ()
{
  uint32 tmp, foo;
  int i;

#ifdef DEBUG
  printf("   exp\tr%d,r%d\n",OP[0],OP[1]);
#endif
  if (((int16)State.regs[OP[0]]) >= 0)
    tmp = (State.regs[OP[0]] << 16) | State.regs[OP[0]+1];
  else
    tmp = ~((State.regs[OP[0]] << 16) | State.regs[OP[0]+1]);
  
  foo = 0x40000000;
  for (i=1;i<16;i++)
    {
      if (tmp & foo)
	{
	  State.regs[OP[0]] = i-1;
	  return;
	}
    }
  State.regs[OP[0]] = 16;
}

/* exp */
void
OP_15002A02 ()
{
#ifdef DEBUG
printf("   exp\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* jl */
void
OP_4D00 ()
{
#ifdef DEBUG
  printf("   jl\t%x\n",OP[0]);
#endif
  State.regs[13] = PC+1;
  PC = State.regs[OP[0]]; 
}

/* jmp */
void
OP_4C00 ()
{
#ifdef DEBUG
  printf("   jmp\tr%d\n",OP[0]);
#endif
  PC = State.regs[OP[0]]; 
}

/* ld */
void
OP_30000000 ()
{
#ifdef DEBUG
  printf("   ld\tr%d,@(0x%x,r%d)\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = RW (OP[1] + State.regs[OP[2]]);
}

/* ld */
void
OP_6401 ()
{
#ifdef DEBUG
  printf("   ld\tr%d,@r%d-\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RW (State.regs[OP[2]]);
  State.regs[OP[1]] -= 2;
}

/* ld */
void
OP_6001 ()
{
#ifdef DEBUG
  printf("   ld\tr%d,@r%d+\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RW (State.regs[OP[2]]);
  State.regs[OP[1]] += 2;
}

/* ld */
void
OP_6000 ()
{
#ifdef DEBUG
  printf("   ld\tr%d,@r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
}

/* ld2w */
void
OP_31000000 ()
{
#ifdef DEBUG
  printf("   ld2w\tr%d,@(0x%x,r%d)\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = RW (OP[1] + State.regs[OP[2]]);
  State.regs[OP[0]+1] = RW (OP[1] + State.regs[OP[2]] + 2);
}

/* ld2w */
void
OP_6601 ()
{
#ifdef DEBUG
  printf("   ld2w\tr%d,@r%d-\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RW (State.regs[OP[2]]);
  State.regs[OP[0]+1] = RW (State.regs[OP[2]]+2);
  State.regs[OP[1]] -= 4;
}

/* ld2w */
void
OP_6201 ()
{
#ifdef DEBUG
  printf("   ld2w\tr%d,@r%d+\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RW (State.regs[OP[2]]);
  State.regs[OP[0]+1] = RW (State.regs[OP[2]]+2);
  State.regs[OP[1]] += 4;
}

/* ld2w */
void
OP_6200 ()
{
#ifdef DEBUG
  printf("   ld2w\tr%d,@r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RW (State.regs[OP[2]]);
  State.regs[OP[0]+1] = RW (State.regs[OP[2]]+2);
}

/* ldb */
void
OP_38000000 ()
{
#ifdef DEBUG
  printf("   ldb\tr%d,@(0x%x,r%d)\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = RB (OP[1] + State.regs[OP[2]]);
  SEXT8 (State.regs[OP[0]]);
}

/* ldb */
void
OP_7000 ()
{
#ifdef DEBUG
  printf("   ldb\tr%d,@r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RB (State.regs[OP[1]]);
  SEXT8 (State.regs[OP[0]]);
}

/* ldi.s */
void
OP_4001 ()
{
#ifdef DEBUG
  printf("   ldi.s\tr%d,%x\n",OP[0],SEXT4(OP[1]));
#endif
  State.regs[OP[0]] = SEXT4(OP[1]);
}

/* ldi.l */
void
OP_20000000 ()
{
#ifdef DEBUG
  printf("   ldi.l\tr%d,%d\t;0x%x\n",OP[0],OP[1],OP[1]);
#endif
  State.regs[OP[0]] = OP[1];
}

/* ldub */
void
OP_39000000 ()
{
#ifdef DEBUG
  printf("   ldub\tr%d,@(0x%x,r%d)\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = RB (OP[1] + State.regs[OP[2]]);
}

/* ldub */
void
OP_7200 ()
{
#ifdef DEBUG
  printf("   ldub\tr%d,@r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = RB (State.regs[OP[1]]);
}

/* mac */
void
OP_2A00 ()
{
#ifdef DEBUG
printf("   mac\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* macsu */
void
OP_1A00 ()
{
#ifdef DEBUG
printf("   macsu\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* macu */
void
OP_3A00 ()
{
#ifdef DEBUG
printf("   macu\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* max */
void
OP_2600 ()
{
#ifdef DEBUG
  printf("   max\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  if (State.regs[OP[1]] > State.regs[OP[0]])
    {
      State.regs[OP[0]] = State.regs[OP[1]];
      State.F0 = 1;
    }
  else
    State.F0 = 0;    
}

/* max */
void
OP_3600 ()
{
#ifdef DEBUG
printf("   max\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* max */
void
OP_3602 ()
{
#ifdef DEBUG
printf("   max\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* min */
void
OP_2601 ()
{
#ifdef DEBUG
  printf("   min\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.F1 = State.F0;
  if (State.regs[OP[1]] < State.regs[OP[0]])
    {
      State.regs[OP[0]] = State.regs[OP[1]];
      State.F0 = 1;
    }
  else
    State.F0 = 0;    
}

/* min */
void
OP_3601 ()
{
#ifdef DEBUG
printf("   min\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* min */
void
OP_3603 ()
{
#ifdef DEBUG
printf("   min\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* msb */
void
OP_2800 ()
{
#ifdef DEBUG
printf("   msb\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* msbsu */
void
OP_1800 ()
{
#ifdef DEBUG
printf("   msbsu\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* msbu */
void
OP_3800 ()
{
#ifdef DEBUG
printf("   msbu\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* mul */
void
OP_2E00 ()
{
#ifdef DEBUG
  printf("   mul\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] *= State.regs[OP[1]];
}

/* mulx */
void
OP_2C00 ()
{
#ifdef DEBUG
printf("   mulx\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* mulxsu */
void
OP_1C00 ()
{
#ifdef DEBUG
printf("   mulxsu\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* mulxu */
void
OP_3C00 ()
{
#ifdef DEBUG
printf("   mulxu\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* mv */
void
OP_4000 ()
{
#ifdef DEBUG
  printf("   mv\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = State.regs[OP[1]];
}

/* mv2w */
void
OP_5000 ()
{
#ifdef DEBUG
  printf("   mv2w\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = State.regs[OP[1]];
  State.regs[OP[0]+1] = State.regs[OP[1]+1];
}

/* mv2wfac */
void
OP_3E00 ()
{
#ifdef DEBUG
  printf("   mv2wfac\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = (State.a[OP[1]] >> 16) & 0xffff;
  State.regs[OP[0]+1] = State.a[OP[1]] & 0xffff;
}

/* mv2wtac */
void
OP_3E01 ()
{
#ifdef DEBUG
  printf("   mv2wtac\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.a[OP[1]] = SEXT16 (State.regs[OP[0]]) << 16 | State.regs[OP[0]+1];
}

/* mvac */
void
OP_3E03 ()
{
#ifdef DEBUG
  printf("   mvac\ta%d,a%d\n",OP[0],OP[1]);
#endif
  State.a[OP[0]] = State.a[OP[1]];
}

/* mvb */
void
OP_5400 ()
{
#ifdef DEBUG
  printf("   mvb\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = SEXT8 (State.regs[OP[1]] & 0xff);
}

/* mvf0f */
void
OP_4400 ()
{
#ifdef DEBUG
  printf("   mvf0f\tr%d,r%d\n",OP[0],OP[1]);
#endif
  if (State.F0 == 0)
    State.regs[OP[0]] = State.regs[OP[1]];
}

/* mvf0t */
void
OP_4401 ()
{
#ifdef DEBUG
  printf("   mvf0t\tr%d,r%d\n",OP[0],OP[1]);
#endif
  if (State.F0)
    State.regs[OP[0]] = State.regs[OP[1]];
}

/* mvfacg */
void
OP_1E04 ()
{
#ifdef DEBUG
  printf("   mvfacg\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = (State.a[OP[1]] >> 32) & 0xff;
}

/* mvfachi */
void
OP_1E00 ()
{
#ifdef DEBUG
  printf("   mvfachi\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = (State.a[OP[1]] >> 16) & 0xffff;  
}

/* mvfaclo */
void
OP_1E02 ()
{
#ifdef DEBUG
  printf("   mvfaclo\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = State.a[OP[1]] & 0xffff;
}

/* mvfc */
void
OP_5200 ()
{
#ifdef DEBUG
  printf("   mvfc\tr%d,cr%d\n",OP[0],OP[1]);
#endif
  if (OP[1] == 0)
    {
      /* PSW is treated specially */
      PSW = 0;
      if (State.SM) PSW |= 0x8000;
      if (State.EA) PSW |= 0x2000;
      if (State.DB) PSW |= 0x1000;
      if (State.IE) PSW |= 0x400;
      if (State.RP) PSW |= 0x200;
      if (State.MD) PSW |= 0x100;
      if (State.FX) PSW |= 0x80;
      if (State.ST) PSW |= 0x40;
      if (State.F0) PSW |= 8;
      if (State.F1) PSW |= 4;
      if (State.C) PSW |= 1;
    }
  State.regs[OP[0]] = State.cregs[OP[1]];
}

/* mvtacg */
void
OP_1E41 ()
{
#ifdef DEBUG
  printf("   mvtacg\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.a[OP[1]] &= MASK32;
  State.a[OP[1]] |= (int64)(State.regs[OP[0]] & 0xff) << 32;
}

/* mvtachi */
void
OP_1E01 ()
{
  uint16 tmp;
#ifdef DEBUG
  printf("   mvtachi\tr%d,a%d\n",OP[0],OP[1]);
#endif
  tmp = State.a[OP[1]] & 0xffff;
  State.a[OP[1]] = SEXT16 (State.regs[OP[0]]) << 16 | tmp;
  printf("put 0x%llx\n",State.a[OP[1]]);
}

/* mvtaclo */
void
OP_1E21 ()
{
#ifdef DEBUG
  printf("   mvtaclo\tr%d,a%d\n",OP[0],OP[1]);
#endif
  State.a[OP[1]] = SEXT16 (State.regs[OP[0]]);
}

/* mvtc */
void
OP_5600 ()
{
#ifdef DEBUG
  printf("   mvtc\tr%d,cr%d\n",OP[0],OP[1]);
#endif
  State.cregs[OP[1]] =  State.regs[OP[0]];
  if (OP[1] == 0)
    {
      /* PSW is treated specially */
      State.SM = (PSW & 0x8000) ? 1 : 0;
      State.EA = (PSW & 0x2000) ? 1 : 0;
      State.DB = (PSW & 0x1000) ? 1 : 0;
      State.IE = (PSW & 0x400) ? 1 : 0;
      State.RP = (PSW & 0x200) ? 1 : 0;
      State.MD = (PSW & 0x100) ? 1 : 0;
      State.FX = (PSW & 0x80) ? 1 : 0;
      State.ST = (PSW & 0x40) ? 1 : 0;
      State.F0 = (PSW & 8) ? 1 : 0;
      State.F1 = (PSW & 4) ? 1 : 0;
      State.C = PSW & 1;
      if (State.ST && !State.FX)
	{
	  fprintf (stderr,"ERROR at PC 0x%x: ST can only be set when FX is set.\n",PC<<2);
	  exit (1);
	}
    }
}

/* mvub */
void
OP_5401 ()
{
#ifdef DEBUG
  printf("   mvub\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = State.regs[OP[1]] & 0xff;
}

/* neg */
void
OP_4605 ()
{
#ifdef DEBUG
  printf("   neg\tr%d\n",OP[0]);
#endif
  State.regs[OP[0]] = 0 - State.regs[OP[0]];
}

/* neg */
void
OP_5605 ()
{
  int64 tmp;
#ifdef DEBUG
  printf("   neg\ta%d\n",OP[0]);
#endif
  tmp = -State.a[OP[0]] & MASK40;
  if (State.ST)
    {
      if ( !(tmp & BIT40) && (tmp > MAX32))
	State.a[OP[0]] = MAX32;
      else if ( (tmp & BIT40) && (tmp < MIN32))
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp;
    }
  else
    State.a[OP[0]] = tmp;
}


/* nop */
void
OP_5E00 ()
{
}

/* not */
void
OP_4603 ()
{
#ifdef DEBUG
  printf("   not\tr%d\n",OP[0]);
#endif
  State.regs[OP[0]] = ~(State.regs[OP[0]]);  
}

/* or */
void
OP_800 ()
{
#ifdef DEBUG
  printf("   or\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] |= State.regs[OP[1]];
}

/* or3 */
void
OP_4000000 ()
{
#ifdef DEBUG
  printf("   or3\tr%d,r%d,0x%x\n",OP[0],OP[1],OP[2]);
#endif
  State.regs[OP[0]] = State.regs[OP[1]] | OP[2];
}

/* rac */
void
OP_5201 ()
{
  int64 tmp;
  int shift = SEXT3 (OP[2]);
#ifdef DEBUG
  printf("   rac\tr%d,a%d,%d\n",OP[0],OP[1],shift);
#endif
  State.F1 = State.F0;
  if (shift >=0)
    tmp = ((State.a[0] << 16) | (State.a[1] & 0xffff)) << shift;
  else
    tmp = ((State.a[0] << 16) | (State.a[1] & 0xffff)) >> -shift;
  tmp = (tmp + 0x8000) & MASK44;
  if ( !(tmp & BIT44) && (tmp > MAX32))
    {
      State.regs[OP[0]] = 0x7fff;
      State.regs[OP[0]+1] = 0xffff;
      State.F0 = 1;
    } 
  else if ((tmp & BIT44) && (tmp < 0xfff80000000LL))
    {
      State.regs[OP[0]] = 0x8000;
      State.regs[OP[0]+1] = 0;
      State.F0 = 1;
    } 
  else
    {
      State.regs[OP[0]] = (tmp >> 16) & 0xffff;
      State.regs[OP[0]+1] = tmp & 0xffff;
      State.F0 = 0;
    }
}

/* rachi */
void
OP_4201 ()
{
#ifdef DEBUG
printf("   rachi\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* rep */
void
OP_27000000 ()
{
#ifdef DEBUG
  printf("   rep\tr%d,0x%x\n",OP[0],OP[1]);
#endif
  RPT_S = PC + 1;
  RPT_E = PC + OP[1];
  RPT_C = State.regs[OP[0]];
  State.RP = 1;
  if (RPT_C == 0)
    {
      fprintf (stderr, "ERROR: rep with count=0 is illegal.\n");
      exit(1);
    }
}

/* repi */
void
OP_2F000000 ()
{
#ifdef DEBUG
  printf("   repi\t%d,0x%x\n",OP[0],OP[1]);
#endif
  RPT_S = PC + 1;
  RPT_E = PC + OP[1];
  RPT_C = OP[0];
  State.RP = 1;
  if (RPT_C == 0)
    {
      fprintf (stderr, "ERROR: rep with count=0 is illegal.\n");
      exit(1);
    }
}

/* rtd */
void
OP_5F60 ()
{
#ifdef DEBUG
printf("   rtd\n");
#endif
}

/* rte */
void
OP_5F40 ()
{
#ifdef DEBUG
printf("   rte\n");
#endif
}

/* sadd */
void
OP_1223 ()
{
#ifdef DEBUG
printf("   sadd\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* setf0f */
void
OP_4611 ()
{
#ifdef DEBUG
printf("   setf0f\t%x\n",OP[0]);
#endif
}

/* setf0t */
void
OP_4613 ()
{
#ifdef DEBUG
printf("   setf0t\t%x\n",OP[0]);
#endif
}

/* sleep */
void
OP_5FC0 ()
{
#ifdef DEBUG
printf("   sleep\n");
#endif
}

/* sll */
void
OP_2200 ()
{
#ifdef DEBUG
  printf("   sll\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] <<= (State.regs[OP[1]] & 0xf);
}

/* sll */
void
OP_3200 ()
{
#ifdef DEBUG
printf("   sll\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* slli */
void
OP_2201 ()
{
#ifdef DEBUG
  printf("   slli\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] <<= OP[1];
}

/* slli */
void
OP_3201 ()
{
#ifdef DEBUG
printf("   slli\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* slx */
void
OP_460B ()
{
  uint16 tmp;
#ifdef DEBUG
  printf("   slx\tr%d\n",OP[0]);
#endif
  State.regs[OP[0]] = (State.regs[OP[0]] << 1) | State.F0;
}

/* sra */
void
OP_2400 ()
{
#ifdef DEBUG
  printf("   sra\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = ((int16)(State.regs[OP[0]])) >> (State.regs[OP[1]] & 0xf);
}

/* sra */
void
OP_3400 ()
{
#ifdef DEBUG
printf("   sra\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* srai */
void
OP_2401 ()
{
#ifdef DEBUG
  printf("   srai\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] = ((int16)(State.regs[OP[0]])) >> OP[1];
}

/* srai */
void
OP_3401 ()
{
#ifdef DEBUG
printf("   srai\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* srl */
void
OP_2000 ()
{
#ifdef DEBUG
  printf("   srl\tr%d,r%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] >>=  (State.regs[OP[1]] & 0xf);
}

/* srl */
void
OP_3000 ()
{
#ifdef DEBUG
printf("   srl\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* srli */
void
OP_2001 ()
{
#ifdef DEBUG
printf("   srli\tr%d,%d\n",OP[0],OP[1]);
#endif
  State.regs[OP[0]] >>= OP[1];
}

/* srli */
void
OP_3001 ()
{
#ifdef DEBUG
printf("   srli\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* srx */
void
OP_4609 ()
{
  uint16 tmp;
#ifdef DEBUG
  printf("   srx\tr%d\n",OP[0]);
#endif
  tmp = State.F0 << 15;
  State.regs[OP[0]] = (State.regs[OP[0]] >> 1) | tmp;
}

/* st */
void
OP_34000000 ()
{
#ifdef DEBUG
  printf("   st\tr%d,@(0x%x,r%d)\n",OP[0],OP[1],OP[2]);
#endif
  SW (OP[1] + State.regs[OP[2]], State.regs[OP[0]]);
}

/* st */
void
OP_6800 ()
{
#ifdef DEBUG
  printf("   st\tr%d,@r%d\n",OP[0],OP[1]);
#endif
  SW (State.regs[OP[1]], State.regs[OP[0]]);
}

/* st */
void
OP_6C1F ()
{
#ifdef DEBUG
printf("   st\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* st */
void
OP_6801 ()
{
#ifdef DEBUG
printf("   st\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* st */
void
OP_6C01 ()
{
#ifdef DEBUG
printf("   st\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* st2w */
void
OP_35000000 ()
{
#ifdef DEBUG
printf("   st2w\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* st2w */
void
OP_6A00 ()
{
#ifdef DEBUG
printf("   st2w\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* st2w */
void
OP_6E1F ()
{
#ifdef DEBUG
printf("   st2w\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* st2w */
void
OP_6A01 ()
{
#ifdef DEBUG
printf("   st2w\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* st2w */
void
OP_6E01 ()
{
#ifdef DEBUG
printf("   st2w\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* stb */
void
OP_3C000000 ()
{
#ifdef DEBUG
printf("   stb\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* stb */
void
OP_7800 ()
{
#ifdef DEBUG
printf("   stb\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* stop */
void
OP_5FE0 ()
{
#ifdef DEBUG
  printf("   stop\n");
#endif
  exit(1);
}

/* sub */
void
OP_0 ()
{
#ifdef DEBUG
printf("   sub\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* sub2w */
void
OP_1000 ()
{
#ifdef DEBUG
printf("   sub2w\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* subac3 */
void
OP_17000000 ()
{
#ifdef DEBUG
printf("   subac3\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* subac3 */
void
OP_17000002 ()
{
#ifdef DEBUG
printf("   subac3\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* subac3s */
void
OP_17001000 ()
{
#ifdef DEBUG
printf("   subac3s\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* subac3s */
void
OP_17001002 ()
{
#ifdef DEBUG
printf("   subac3s\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

/* subi */
void
OP_1 ()
{
#ifdef DEBUG
printf("   subi\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* trap */
void
OP_5F00 ()
{
#ifdef DEBUG
  printf("   trap\t%d\n",OP[0]);
#endif
  
  /* for now, trap 0 is used for simulating IO */

  if (OP[0] == 0)
    {
      char *fstr = State.regs[2] + State.imem;
      printf (fstr,State.regs[3],State.regs[4],State.regs[5]);
    }
  else if (OP[0] == 1 )
    {
      char *fstr = State.regs[2] + State.imem;
      puts (fstr);
    }
}

/* tst0i */
void
OP_7000000 ()
{
#ifdef DEBUG
printf("   tst0i\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* tst1i */
void
OP_F000000 ()
{
#ifdef DEBUG
printf("   tst1i\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* wait */
void
OP_5F80 ()
{
#ifdef DEBUG
printf("   wait\n");
#endif
}

/* xor */
void
OP_A00 ()
{
#ifdef DEBUG
printf("   xor\t%x,%x\n",OP[0],OP[1]);
#endif
}

/* xor3 */
void
OP_5000000 ()
{
#ifdef DEBUG
printf("   xor3\t%x,%x,%x\n",OP[0],OP[1],OP[2]);
#endif
}

