#include <signal.h>
#include "v850_sim.h"
#include "simops.h"

void
OP_280 ()
{
}

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
OP_40 ()
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
OP_2E0 ()
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

void
OP_E0 ()
{
}

void
OP_16087E0 ()
{
}

void
OP_16007E0 ()
{
}

/* add reg, reg

   XXX condition codes.  */
void
OP_1C0 ()
{
  State.regs[OP[1]] += State.regs[OP[0]];
}

/* add sign_extend(imm5), reg

   XXX condition codes.  */
void
OP_240 ()
{
  int value = OP[0];
 
  value = (value << 27) >> 27;

  State.regs[OP[1]] += value;
}

/* addi sign_extend(imm16), reg, reg

   XXX condition codes.  */
void
OP_600 ()
{
  int value = OP[0];
 
  value = (value << 16) >> 16;

  State.regs[OP[2]] = State.regs[OP[1]] + value;
}

/* sub reg1, reg2

   XXX condition codes  */
void
OP_1A0 ()
{
  State.regs[OP[1]] -= State.regs[OP[0]];
}

/* subr reg1, reg2

   XXX condition codes */
void
OP_180 ()
{
  State.regs[OP[1]] = State.regs[OP[0]] - State.regs[OP[1]];
}

void
OP_8007E0 ()
{
}

void
OP_C007E0 ()
{
}

void
OP_12007E0 ()
{
}

void
OP_4007E0 ()
{
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
OP_2A0 ()
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
OP_6E0 ()
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

void
OP_14007E0 ()
{
}

/* not reg1, reg2

   XXX condition codes */
void
OP_20 ()
{
  State.regs[OP[1]] = ~State.regs[OP[0]];
}

void
OP_A007E0 ()
{
}

void
OP_500 ()
{
}

void
OP_2C0 ()
{
}

void
OP_10007E0 ()
{
}

void
OP_47C0 ()
{
}

void
OP_2007E0 ()
{
}

void
OP_7E0 ()
{
}

/* or reg, reg

   XXX condition codes.  */
void
OP_100 ()
{
  State.regs[OP[1]] |= State.regs[OP[0]];
}

/* ori zero_extend(imm16), reg, reg

   XXX condition codes */
void
OP_680 ()
{
  int value = OP[0];
 
  value &= 0xffff;

  State.regs[OP[2]] = State.regs[OP[1]] | value;
}

/* and reg, reg

   XXX condition codes.  */
void
OP_140 ()
{
  State.regs[OP[1]] &= State.regs[OP[0]];
}

/* andi zero_extend(imm16), reg, reg

   XXX condition codes.  */
void
OP_6C0 ()
{
  int value = OP[0];
 
  value &= 0xffff;

  State.regs[OP[2]] = State.regs[OP[1]] & value;
}

/* xor reg, reg

   XXX condition codes.  */
void
OP_120 ()
{
  State.regs[OP[1]] ^= State.regs[OP[0]];
}

/* xori zero_extend(imm16), reg, reg

   XXX condition codes.  */
void
OP_6A0 ()
{
  int value = OP[0];
 
  value &= 0xffff;

  State.regs[OP[2]] = State.regs[OP[1]] ^ value;
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
