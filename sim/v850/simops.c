#include <signal.h>
#include "v850_sim.h"
#include "simops.h"

void
OP_220 ()
{
}

void
OP_10760 ()
{
}

void
OP_C7C0 ()
{
}

void
OP_760 ()
{
}

void
OP_580 ()
{
}

void
OP_700 ()
{
}

void
OP_581 ()
{
}

void
OP_582 ()
{
}

void
OP_583 ()
{
}

void
OP_584 ()
{
}

void
OP_585 ()
{
}

void
OP_586 ()
{
}

void
OP_587 ()
{
}

void
OP_588 ()
{
}

void
OP_589 ()
{
}

void
OP_58A ()
{
}

void
OP_58B ()
{
}

void
OP_58C ()
{
}

void
OP_400 ()
{
}

void
OP_160 ()
{
}

void
OP_58D ()
{
}

void
OP_58E ()
{
}

void
OP_58F ()
{
}

void
OP_660 ()
{
}


/* add reg, reg */
void
OP_1C0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 + op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < op0 || result < op1);
  ov = ((op0 & 0x80000000) == (op1 & 0x80000000)
	&& (op0 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
}

/* add sign_extend(imm5), reg */
void
OP_240 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;
  int temp;

  /* Compute the result.  */
  temp = (OP[0] & 0x1f);
  temp = (temp << 27) >> 27;
  op0 = temp;
  op1 = State.regs[OP[1]];
  result = op0 + op1;
  
  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < op0 || result < op1);
  ov = ((op0 & 0x80000000) == (op1 & 0x80000000)
	&& (op0 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
}

/* addi sign_extend(imm16), reg, reg */
void
OP_600 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;
  int temp;

  /* Compute the result.  */
  temp = (OP[0] & 0xffff);
  temp = (temp << 16) >> 16;
  op0 = temp;
  op1 = State.regs[OP[1]];
  result = op0 + op1;
  
  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < op0 || result < op1);
  ov = ((op0 & 0x80000000) == (op1 & 0x80000000)
	&& (op0 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
}

/* sub reg1, reg2 */
void
OP_1A0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op1 - op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < -op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  State.regs[OP[1]] = State.regs[OP[0]];
}

/* subr reg1, reg2 */
void
OP_180 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 - op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < -op1);
  ov = ((op0 & 0x80000000) != (op1 & 0x80000000)
	&& (op0 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
}

/* mulh reg1, reg2 */
void
OP_E0 ()
{
  State.regs[OP[1]] = ((State.regs[OP[1]] & 0xffff)
		       * (State.regs[OP[0]] & 0xffff));
}

/* mulh sign_extend(imm5), reg2

   Condition codes */
void
OP_2E0 ()
{
  int value = OP[0];
 
  value = (value << 27) >> 27;

  State.regs[OP[1]] = (State.regs[OP[1]] & 0xffff) * value;
}

/* mulhi imm16, reg1, reg2 */
void
OP_6E0 ()
{
  int value = OP[0];
 
  value = value & 0xffff;

  State.regs[OP[2]] = (State.regs[OP[1]] & 0xffff) * value;
}

/* divh reg1, reg2 */
void
OP_40 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;
  int temp;

  /* Compute the result.  */
  temp = State.regs[OP[0]] & 0xffff;
  temp = (temp << 16) >> 16;
  op0 = temp;
  op1 = State.regs[OP[1]];

  if (op0 == 0xffffffff && op1 == 0x80000000)
    {
      result = 0x80000000;
      ov = 1;
    }
  else if (op0 != 0)
    {
      result = op1 / op0;
      ov = 0;
    }
  else
    ov = 1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (ov ? PSW_OV : 0));
}

void
OP_10720 ()
{
}

void
OP_780 ()
{
}

void
OP_720 ()
{
}

void
OP_60 ()
{
}

void
OP_87C0 ()
{
}

void
OP_300 ()
{
}

/* mov reg, reg */
void
OP_0 ()
{
  State.regs[OP[1]] = State.regs[OP[0]];
}

/* mov sign_extend(imm5), reg */
void
OP_200 ()
{
  int value = OP[0];
 
  value = (value << 27) >> 27;
  State.regs[OP[1]] = value;
}

/* movea sign_extend(imm16), reg, reg  */

void
OP_620 ()
{
  int value = OP[0];
 
  value = (value << 16) >> 16;

  State.regs[OP[2]] = State.regs[OP[1]] + value;
}

/* movhi imm16, reg, reg */
void
OP_640 ()
{
  int value = OP[0];
 
  value = (value & 0xffff) << 16; 

  State.regs[OP[2]] = State.regs[OP[1]] + value;
}

void
OP_7C0 ()
{
}

void
OP_1687E0 ()
{
}

void
OP_1E0 ()
{
}

void
OP_A0 ()
{
}

void
OP_260 ()
{
}

void
OP_740 ()
{
}

void
OP_80 ()
{
}

/* sar zero_extend(imm5),reg1

   XXX condition codes.  */
void
OP_2A0 ()
{
  int temp = State.regs[OP[1]];

  temp >>= (OP[0] & 0x1f);

  State.regs[OP[1]] = temp;
}

/* sar reg1, reg2

   XXX condition codes.  */
void
OP_A007E0 ()
{
  int temp = State.regs[OP[1]];

  temp >>= (State.regs[OP[0]] & 0x1f);

  State.regs[OP[1]] = temp;
}

/* shl zero_extend(imm5),reg1

   XXX condition codes.  */
void
OP_2C0 ()
{
  State.regs[OP[1]] <<= (OP[0] & 0x1f);
}

/* shl reg1, reg2

   XXX condition codes.  */
void
OP_C007E0 ()
{
  State.regs[OP[1]] <<= (State.regs[OP[0]] & 0x1f);
}

/* shr zero_extend(imm5),reg1

   XXX condition codes.  */
void
OP_280 ()
{
  State.regs[OP[1]] >>= (OP[0] & 0x1f);
}

/* shr reg1, reg2

   XXX condition codes.  */
void
OP_8007E0 ()
{
  State.regs[OP[1]] >>= (State.regs[OP[0]] & 0x1f);
}

void
OP_500 ()
{
}

void
OP_47C0 ()
{
}

void
OP_7E0 ()
{
}

/* or reg, reg */
void
OP_100 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 | op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
}

/* ori zero_extend(imm16), reg, reg */
void
OP_680 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  op0 = OP[0] & 0xffff;
  op1 = State.regs[OP[1]];
  result = op0 | op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  State.psw |= (z ? PSW_Z : 0);
}

/* and reg, reg */
void
OP_140 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 & op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
}

/* andi zero_extend(imm16), reg, reg */
void
OP_6C0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  op0 = OP[0] & 0xffff;
  op1 = State.regs[OP[1]];
  result = op0 & op1;

  /* Compute the condition codes.  */
  z = (result == 0);

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= (z ? PSW_Z : 0);
}

/* xor reg, reg */
void
OP_120 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 ^ op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
}

/* xori zero_extend(imm16), reg, reg */
void
OP_6A0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  op0 = OP[0] & 0xffff;
  op1 = State.regs[OP[1]];
  result = op0 ^ op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  State.psw |= (z ? PSW_Z : 0);
}

/* not reg1, reg2 */
void
OP_20 ()
{
  unsigned int op0, result, z, s, cy, ov;

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  result = ~op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.psw &= ~(PSW_Z | PSW_S | PSW_OV);
  State.psw |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
}

void
OP_C0 ()
{
}

void
OP_480 ()
{
}

void
OP_380 ()
{
}

void
OP_501 ()
{
}

/* di, not supported */
void
OP_16007E0 ()
{
  abort ();
}

/* ei, not supported */
void
OP_16087E0 ()
{
  abort ();
}

/* halt, not supported */
void
OP_12007E0 ()
{
  abort ();
}

/* reti, not supported */
void
OP_14007E0 ()
{
  abort ();
}

/* trap, not supportd */
void
OP_10007E0 ()
{
  abort ();
}

/* ldsr, not supported */
void
OP_2007E0 ()
{
  abort ();
}

/* stsr, not supported */
void
OP_4007E0 ()
{
  abort ();
}

