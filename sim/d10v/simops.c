#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "d10v_sim.h"
#include "simops.h"
#include "sys/syscall.h"

enum op_types {
  OP_VOID,
  OP_REG,
  OP_REG_OUTPUT,
  OP_DREG,
  OP_DREG_OUTPUT,
  OP_ACCUM,
  OP_ACCUM_OUTPUT,
  OP_ACCUM_REVERSE,
  OP_CR,
  OP_CR_OUTPUT,
  OP_CR_REVERSE,
  OP_FLAG,
  OP_CONSTANT16,
  OP_CONSTANT3,
  OP_CONSTANT4,
  OP_MEMREF,
  OP_MEMREF2,
  OP_POSTDEC,
  OP_POSTINC,
  OP_PREDEC
};

#if (DEBUG & DEBUG_TRACE) != 0
static void trace_input PARAMS ((char *name,
				 enum op_types in1,
				 enum op_types in2,
				 enum op_types in3));

static void trace_output PARAMS ((enum op_types result));

#ifndef SIZE_INSTRUCTION
#define SIZE_INSTRUCTION 10
#endif

#ifndef SIZE_OPERANDS
#define SIZE_OPERANDS 24
#endif

#ifndef SIZE_VALUES
#define SIZE_VALUES 13
#endif

static void
trace_input (name, in1, in2, in3)
     char *name;
     enum op_types in1;
     enum op_types in2;
     enum op_types in3;
{
  char *comma;
  enum op_types in[3];
  int i;
  char buf[80];
  char *p;
  long tmp;
  char *type;

  switch (State.ins_type)
    {
    default:
    case INS_UNKNOWN:		type = " ?"; break;
    case INS_LEFT:		type = " L"; break;
    case INS_RIGHT:		type = " R"; break;
    case INS_LEFT_PARALLEL:	type = "*L"; break;
    case INS_RIGHT_PARALLEL:	type = "*R"; break;
    case INS_LONG:		type = " B"; break;
    }

  printf ("0x%.6x %s:  %-*s", (unsigned)PC, type, SIZE_INSTRUCTION, name);

  in[0] = in1;
  in[1] = in2;
  in[2] = in3;
  comma = "";
  p = buf;
  for (i = 0; i < 3; i++)
    {
      switch (in[i])
	{
	case OP_VOID:
	  break;

	case OP_REG:
	case OP_REG_OUTPUT:
	case OP_DREG:
	case OP_DREG_OUTPUT:
	  sprintf (p, "%sr%d", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_CR:
	case OP_CR_OUTPUT:
	case OP_CR_REVERSE:
	  sprintf (p, "%scr%d", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_ACCUM:
	case OP_ACCUM_OUTPUT:
	case OP_ACCUM_REVERSE:
	  sprintf (p, "%sa%d", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_CONSTANT16:
	  sprintf (p, "%s%d", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_CONSTANT4:
	  sprintf (p, "%s%d", comma, SEXT4(OP[i]));
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_CONSTANT3:
	  sprintf (p, "%s%d", comma, SEXT3(OP[i]));
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_MEMREF:
	  sprintf (p, "%s@r%d", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_MEMREF2:
	  sprintf (p, "%s@(%d,r%d)", comma, (int16)OP[i], OP[i+1]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_POSTINC:
	  sprintf (p, "%s@r%d+", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_POSTDEC:
	  sprintf (p, "%s@r%d-", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_PREDEC:
	  sprintf (p, "%s@-r%d", comma, OP[i]);
	  p += strlen (p);
	  comma = ",";
	  break;

	case OP_FLAG:
	  if (OP[i] == 0)
	    sprintf (p, "%sf0", comma);

	  else if (OP[i] == 1)
	    sprintf (p, "%sf1", comma);

	  else
	    sprintf (p, "%scarry", comma);

	  p += strlen (p);
	  comma = ",";
	  break;
	}
    }

#if (DEBUG & DEBUG_VALUES) == 0
  *p++ = '\n';
  *p = '\0';
  fputs (buf, stdout);

#else	/* DEBUG_VALUES */
  *p = '\0';
  printf ("%-*s", SIZE_OPERANDS, buf);

  p = buf;
  for (i = 0; i < 3; i++)
    {
      buf[0] = '\0';
      switch (in[i])
	{
	case OP_VOID:
	  printf ("%*s", SIZE_VALUES, "");
	  break;

	case OP_REG_OUTPUT:
	case OP_DREG_OUTPUT:
	case OP_CR_OUTPUT:
	case OP_ACCUM_OUTPUT:
	  printf ("%*s", SIZE_VALUES, "---");
	  break;

	case OP_REG:
	case OP_MEMREF:
	case OP_POSTDEC:
	case OP_POSTINC:
	case OP_PREDEC:
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)State.regs[OP[i]]);
	  break;

	case OP_DREG:
	  tmp = (long)((((uint32) State.regs[OP[i]]) << 16) | ((uint32) State.regs[OP[i]+1]));
	  printf ("%*s0x%.8lx", SIZE_VALUES-10, "", tmp);
	  break;

	case OP_CR:
	case OP_CR_REVERSE:
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)State.cregs[OP[i]]);
	  break;

	case OP_ACCUM:
	case OP_ACCUM_REVERSE:
	  printf ("%*s0x%.2x%.8lx", SIZE_VALUES-12, "",
		  ((int)(State.a[OP[i]] >> 32) & 0xff),
		  ((unsigned long)State.a[OP[i]]) & 0xffffffff);
	  break;

	case OP_CONSTANT16:
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)OP[i]);
	  break;

	case OP_CONSTANT4:
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)SEXT4(OP[i]));
	  break;

	case OP_CONSTANT3:
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)SEXT3(OP[i]));
	  break;

	case OP_FLAG:
	  if (OP[i] == 0)
	    printf ("%*sF0 = %d", SIZE_VALUES-6, "", State.F0 != 0);

	  else if (OP[i] == 1)
	    printf ("%*sF1 = %d", SIZE_VALUES-6, "", State.F1 != 0);

	  else
	    printf ("%*sC = %d", SIZE_VALUES-5, "", State.C != 0);

	  break;

	case OP_MEMREF2:
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)OP[i]);
	  printf ("%*s0x%.4x", SIZE_VALUES-6, "", (uint16)State.regs[OP[++i]]);
	  break;
	}
    }
#endif
}

static void
trace_output (result)
     enum op_types result;
{
#if (DEBUG & DEBUG_VALUES) != 0
  long tmp;

  switch (result)
    {
    default:
      putchar ('\n');
      break;

    case OP_REG:
    case OP_REG_OUTPUT:
      printf (" :: %*s0x%.4x F0=%d F1=%d C=%d\n", SIZE_VALUES-6, "", (uint16)State.regs[OP[0]],
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;

    case OP_DREG:
    case OP_DREG_OUTPUT:
      tmp = (long)((((uint32) State.regs[OP[0]]) << 16) | ((uint32) State.regs[OP[0]+1]));
      printf (" :: %*s0x%.8lx F0=%d F1=%d C=%d\n", SIZE_VALUES-10, "", tmp,
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;

    case OP_CR:
    case OP_CR_OUTPUT:
      printf (" :: %*s0x%.4x F0=%d F1=%d C=%d\n", SIZE_VALUES-6, "", (uint16)State.cregs[OP[0]],
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;

    case OP_CR_REVERSE:
      printf (" :: %*s0x%.4x F0=%d F1=%d C=%d\n", SIZE_VALUES-6, "", (uint16)State.cregs[OP[1]],
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;

    case OP_ACCUM:
    case OP_ACCUM_OUTPUT:
      printf (" :: %*s0x%.2x%.8lx F0=%d F1=%d C=%d\n", SIZE_VALUES-10, "",
	      ((int)(State.a[OP[0]] >> 32) & 0xff),
	      ((unsigned long)State.a[OP[0]]) & 0xffffffff,
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;

    case OP_ACCUM_REVERSE:
      printf (" :: %*s0x%.2x%.8lx F0=%d F1=%d C=%d\n", SIZE_VALUES-10, "",
	      ((int)(State.a[OP[1]] >> 32) & 0xff),
	      ((unsigned long)State.a[OP[1]]) & 0xffffffff,
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;

    case OP_FLAG:
      printf (" :: %*s F0=%d F1=%d C=%d\n", SIZE_VALUES, "",
	      State.F0 != 0, State.F1 != 0, State.C != 0);
      break;
    }

  fflush (stdout);
#endif
}

#else
#define trace_input(NAME, IN1, IN2, IN3)
#define trace_output(RESULT)
#endif

/* abs */
void
OP_4607 ()
{
  trace_input ("abs", OP_REG, OP_VOID, OP_VOID);
  State.F1 = State.F0;
  if ((int16)(State.regs[OP[0]]) < 0)
    {
      State.regs[OP[0]] = -(int16)(State.regs[OP[0]]);
      State.F0 = 1;
    }
  else
    State.F0 = 0;
  trace_output (OP_REG);
}

/* abs */
void
OP_5607 ()
{
  int64 tmp;

  trace_input ("abs", OP_ACCUM, OP_VOID, OP_VOID);
  State.F1 = State.F0;
  State.a[OP[0]] = SEXT40(State.a[OP[0]]);

  if (State.a[OP[0]] < 0 )
    {
      tmp = -State.a[OP[0]];
      if (State.ST)
	{
	  if (tmp > MAX32)
	    State.a[OP[0]] = MAX32;
	  else if (tmp < MIN32)
	    State.a[OP[0]] = MIN32;
	  else
	    State.a[OP[0]] = tmp & MASK40;
	}
      else
	State.a[OP[0]] = tmp & MASK40;
      State.F0 = 1;
    }
  else
    State.F0 = 0;
  trace_output (OP_ACCUM);
}

/* add */
void
OP_200 ()
{
  uint16 tmp = State.regs[OP[0]];
  trace_input ("add", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] += State.regs[OP[1]];
  if ( tmp > State.regs[OP[0]])
    State.C = 1;
  else
    State.C = 0;
  trace_output (OP_REG);
}

/* add */
void
OP_1201 ()
{
  int64 tmp;
  tmp = SEXT40(State.a[OP[0]]) + (SEXT16 (State.regs[OP[1]]) << 16 | State.regs[OP[1]+1]);

  trace_input ("add", OP_ACCUM, OP_REG, OP_VOID);
  if (State.ST)
    {
      if ( tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if ( tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* add */
void
OP_1203 ()
{
  int64 tmp;
  tmp = SEXT40(State.a[OP[0]]) + SEXT40(State.a[OP[1]]);

  trace_input ("add", OP_ACCUM, OP_ACCUM, OP_VOID);
  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if ( tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* add2w */
void
OP_1200 ()
{
  uint32 tmp;
  uint32 tmp1 = (State.regs[OP[0]]) << 16 | State.regs[OP[0]+1];
  uint32 tmp2 = (State.regs[OP[1]]) << 16 | State.regs[OP[1]+1];

  trace_input ("add2w", OP_DREG, OP_DREG, OP_VOID);
  tmp = tmp1 + tmp2;
  if ( (tmp < tmp1) || (tmp < tmp2) )
    State.C = 1;
  else
    State.C = 0;
  State.regs[OP[0]] = tmp >> 16;
  State.regs[OP[0]+1] = tmp & 0xFFFF;
  trace_output (OP_DREG);
}

/* add3 */
void
OP_1000000 ()
{
  uint16 tmp = State.regs[OP[0]];
  State.regs[OP[0]] = State.regs[OP[1]] + OP[2];

  trace_input ("add3", OP_REG_OUTPUT, OP_REG, OP_CONSTANT16);
  if ( tmp > State.regs[OP[0]])
    State.C = 1;
  else
    State.C = 0;
  trace_output (OP_REG);
}

/* addac3 */
void
OP_17000200 ()
{
  int64 tmp;
  tmp = SEXT40(State.a[OP[2]]) + SEXT40 ((State.regs[OP[1]] << 16) | State.regs[OP[1]+1]);

  trace_input ("addac3", OP_DREG_OUTPUT, OP_DREG, OP_ACCUM);
  State.regs[OP[0]] = (tmp >> 16) & 0xffff;
  State.regs[OP[0]+1] = tmp & 0xffff;
  trace_output (OP_DREG);
}

/* addac3 */
void
OP_17000202 ()
{
  int64 tmp;
  tmp = SEXT40(State.a[OP[1]]) + SEXT40(State.a[OP[2]]);

  trace_input ("addac3", OP_DREG_OUTPUT, OP_ACCUM, OP_ACCUM);
  State.regs[OP[0]] = (tmp >> 16) & 0xffff;
  State.regs[OP[0]+1] = tmp & 0xffff;
  trace_output (OP_DREG);
}

/* addac3s */
void
OP_17001200 ()
{
  int64 tmp;
  State.F1 = State.F0;

  trace_input ("addac3s", OP_DREG_OUTPUT, OP_DREG, OP_ACCUM);
  tmp = SEXT40(State.a[OP[2]]) + SEXT40 ((State.regs[OP[1]] << 16) | State.regs[OP[1]+1]);
  if ( tmp > MAX32)
    {
      State.regs[OP[0]] = 0x7fff;
      State.regs[OP[0]+1] = 0xffff;
      State.F0 = 1;
    }      
  else if (tmp < MIN32)
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
  trace_output (OP_DREG);
}

/* addac3s */
void
OP_17001202 ()
{
  int64 tmp;
  State.F1 = State.F0;

  trace_input ("addac3s", OP_DREG_OUTPUT, OP_ACCUM, OP_ACCUM);
  tmp = SEXT40(State.a[OP[1]]) + SEXT40(State.a[OP[2]]);
  if ( tmp > MAX32)
    {
      State.regs[OP[0]] = 0x7fff;
      State.regs[OP[0]+1] = 0xffff;
      State.F0 = 1;
    }      
  else if (tmp < MIN32)
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
  trace_output (OP_DREG);
}

/* addi */
void
OP_201 ()
{
  if (OP[1] == 0)
    OP[1] = 16;
  trace_input ("addi", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] += OP[1];
  trace_output (OP_REG);
}

/* and */
void
OP_C00 ()
{
  trace_input ("and", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] &= State.regs[OP[1]];
  trace_output (OP_REG);
}

/* and3 */
void
OP_6000000 ()
{
  trace_input ("and3", OP_REG_OUTPUT, OP_REG, OP_CONSTANT16);
  State.regs[OP[0]] = State.regs[OP[1]] & OP[2];
  trace_output (OP_REG);
}

/* bclri */
void
OP_C01 ()
{
  trace_input ("bclri", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] &= ~(0x8000 >> OP[1]);
  trace_output (OP_REG);
}

/* bl.s */
void
OP_4900 ()
{
  trace_input ("bl.s", OP_CONSTANT16, OP_VOID, OP_VOID);
  State.regs[13] = PC+1;
  PC += SEXT8 (OP[0]);
  trace_output (OP_VOID);
}

/* bl.l */
void
OP_24800000 ()
{
  trace_input ("bl.l", OP_CONSTANT16, OP_VOID, OP_VOID);
  State.regs[13] = PC+1;
  PC += OP[0];
  trace_output (OP_VOID);
}

/* bnoti */
void
OP_A01 ()
{
  trace_input ("bnoti", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] ^= 0x8000 >> OP[1];
  trace_output (OP_REG);
}

/* bra.s */
void
OP_4800 ()
{
  trace_input ("bra.s", OP_CONSTANT16, OP_VOID, OP_VOID);
  PC += SEXT8 (OP[0]);
  trace_output (OP_VOID);
}

/* bra.l */
void
OP_24000000 ()
{
  trace_input ("bra.l", OP_CONSTANT16, OP_VOID, OP_VOID);
  PC += OP[0];
  trace_output (OP_VOID);
}

/* brf0f.s */
void
OP_4A00 ()
{
  trace_input ("brf0f.s", OP_CONSTANT16, OP_VOID, OP_VOID);
  if (State.F0 == 0)
    PC += SEXT8 (OP[0]);
  trace_output (OP_FLAG);
}

/* brf0f.l */
void
OP_25000000 ()
{
  trace_input ("brf0f.l", OP_CONSTANT16, OP_VOID, OP_VOID);
  if (State.F0 == 0)
    PC += OP[0];
  trace_output (OP_FLAG);
}

/* brf0t.s */
void
OP_4B00 ()
{
  trace_input ("brf0t.s", OP_CONSTANT16, OP_VOID, OP_VOID);
  if (State.F0)
    PC += SEXT8 (OP[0]);
  trace_output (OP_FLAG);
}

/* brf0t.l */
void
OP_25800000 ()
{
  trace_input ("brf0t.l", OP_CONSTANT16, OP_VOID, OP_VOID);
  if (State.F0)
    PC += OP[0];
  trace_output (OP_FLAG);
}

/* bseti */
void
OP_801 ()
{
  trace_input ("bseti", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] |= 0x8000 >> OP[1];
  trace_output (OP_REG);
}

/* btsti */
void
OP_E01 ()
{
  trace_input ("btsti", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] & (0x8000 >> OP[1])) ? 1 : 0;
  trace_output (OP_FLAG);
}

/* clrac */
void
OP_5601 ()
{
  trace_input ("clrac", OP_ACCUM_OUTPUT, OP_VOID, OP_VOID);
  State.a[OP[0]] = 0;
  trace_output (OP_ACCUM);
}

/* cmp */
void
OP_600 ()
{
  trace_input ("cmp", OP_REG, OP_REG, OP_VOID);
  State.F1 = State.F0;
  State.F0 = ((int16)(State.regs[OP[0]]) < (int16)(State.regs[OP[1]])) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmp */
void
OP_1603 ()
{
  trace_input ("cmp", OP_ACCUM, OP_ACCUM, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (SEXT40(State.a[OP[0]]) < SEXT40(State.a[OP[1]])) ? 1 : 0;
  trace_output (OP_FLAG);
}

/* cmpeq */
void
OP_400 ()
{
  trace_input ("cmpeq", OP_REG, OP_REG, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] == State.regs[OP[1]]) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmpeq */
void
OP_1403 ()
{
  trace_input ("cmpeq", OP_ACCUM, OP_ACCUM, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.a[OP[0]] == State.a[OP[1]]) ? 1 : 0;
  trace_output (OP_FLAG);
}

/* cmpeqi.s */
void
OP_401 ()
{
  trace_input ("cmpeqi.s", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] == SEXT4(OP[1])) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmpeqi.l */
void
OP_2000000 ()
{
  trace_input ("cmpeqi.l", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] == OP[1]) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmpi.s */
void
OP_601 ()
{
  trace_input ("cmpi.s", OP_REG, OP_CONSTANT4, OP_VOID);
  State.F1 = State.F0;
  State.F0 = ((int16)(State.regs[OP[0]]) < SEXT4(OP[1])) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmpi.l */
void
OP_3000000 ()
{
  trace_input ("cmpi.l", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = ((int16)(State.regs[OP[0]]) < (int16)(OP[1])) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmpu */
void
OP_4600 ()
{
  trace_input ("cmpu", OP_REG, OP_REG, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] < State.regs[OP[1]]) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cmpui */
void
OP_23000000 ()
{
  trace_input ("cmpui", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] < OP[1]) ? 1 : 0;  
  trace_output (OP_FLAG);
}

/* cpfg */
void
OP_4E09 ()
{
  uint8 *src, *dst;
  
  trace_input ("cpfg", OP_FLAG, OP_VOID, OP_VOID);
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
  trace_output (OP_FLAG);
}

/* dbt */
void
OP_5F20 ()
{
  printf("***** DBT *****  PC=%x\n",PC);
  State.exception = SIGTRAP;
}

/* divs */
void
OP_14002800 ()
{
  uint16 foo, tmp, tmpf;

  trace_input ("divs", OP_DREG, OP_REG, OP_VOID);
  foo = (State.regs[OP[0]] << 1) | (State.regs[OP[0]+1] >> 15);
  tmp = (int16)foo - (int16)(State.regs[OP[1]]);
  tmpf = (foo >= State.regs[OP[1]]) ? 1 : 0;
  State.regs[OP[0]] = (tmpf == 1) ? tmp : foo;
  State.regs[OP[0]+1] = (State.regs[OP[0]+1] << 1) | tmpf;
  trace_output (OP_DREG);
}

/* exef0f */
void
OP_4E04 ()
{
  trace_input ("exef0f", OP_VOID, OP_VOID, OP_VOID);
  State.exe = (State.F0) ? 0 : 1;
  trace_output (OP_FLAG);
}

/* exef0t */
void
OP_4E24 ()
{
  trace_input ("exef0t", OP_VOID, OP_VOID, OP_VOID);
  State.exe = State.F0;
  trace_output (OP_FLAG);
}

/* exef1f */
void
OP_4E40 ()
{
  trace_input ("exef1f", OP_VOID, OP_VOID, OP_VOID);
  State.exe = (State.F1) ? 0 : 1;
  trace_output (OP_FLAG);
}

/* exef1t */
void
OP_4E42 ()
{
  trace_input ("exef1t", OP_VOID, OP_VOID, OP_VOID);
  State.exe = State.F1;
  trace_output (OP_FLAG);
}

/* exefaf */
void
OP_4E00 ()
{
  trace_input ("exefaf", OP_VOID, OP_VOID, OP_VOID);
  State.exe = (State.F0 | State.F1) ? 0 : 1;
  trace_output (OP_FLAG);
}

/* exefat */
void
OP_4E02 ()
{
  trace_input ("exefat", OP_VOID, OP_VOID, OP_VOID);
  State.exe = (State.F0) ? 0 : (State.F1); 
  trace_output (OP_FLAG);
}

/* exetaf */
void
OP_4E20 ()
{
  trace_input ("exetaf", OP_VOID, OP_VOID, OP_VOID);
  State.exe = (State.F1) ? 0 : (State.F0);
  trace_output (OP_FLAG);
}

/* exetat */
void
OP_4E22 ()
{
  trace_input ("exetat", OP_VOID, OP_VOID, OP_VOID);
  State.exe = (State.F0) ? (State.F1) : 0;
  trace_output (OP_FLAG);
}

/* exp */
void
OP_15002A00 ()
{
  uint32 tmp, foo;
  int i;

  trace_input ("exp", OP_REG_OUTPUT, OP_DREG, OP_VOID);
  if (((int16)State.regs[OP[1]]) >= 0)
    tmp = (State.regs[OP[1]] << 16) | State.regs[OP[1]+1];
  else
    tmp = ~((State.regs[OP[1]] << 16) | State.regs[OP[1]+1]);
  
  foo = 0x40000000;
  for (i=1;i<17;i++)
    {
      if (tmp & foo)
	{
	  State.regs[OP[0]] = i-1;
	  trace_output (OP_REG);
	  return;
	}
      foo >>= 1;
    }
  State.regs[OP[0]] = 16;
  trace_output (OP_REG);
}

/* exp */
void
OP_15002A02 ()
{
  int64 tmp, foo;
  int i;

  trace_input ("exp", OP_REG_OUTPUT, OP_ACCUM, OP_VOID);
  if (SEXT40(State.a[OP[1]]) >= 0)
    tmp = State.a[OP[1]];
  else
    tmp = ~(State.a[OP[1]]);
  
  foo = 0x4000000000LL;
  for (i=1;i<25;i++)
    {
      if (tmp & foo)
	{
	  State.regs[OP[0]] = i-9;
	  trace_output (OP_REG);
	  return;
	}
      foo >>= 1;
    }
  State.regs[OP[0]] = 16;
  trace_output (OP_REG);
}

/* jl */
void
OP_4D00 ()
{
  trace_input ("jl", OP_REG, OP_VOID, OP_VOID);
  State.regs[13] = PC+1;
  PC = State.regs[OP[0]]; 
  trace_output (OP_VOID);
}

/* jmp */
void
OP_4C00 ()
{
  trace_input ("jmp", OP_REG, OP_VOID, OP_VOID);
  PC = State.regs[OP[0]]; 
  trace_output (OP_VOID);
}

/* ld */
void
OP_30000000 ()
{
  trace_input ("ld", OP_REG_OUTPUT, OP_MEMREF2, OP_VOID);
  State.regs[OP[0]] = RW (OP[1] + State.regs[OP[2]]);
  trace_output (OP_REG);
}

/* ld */
void
OP_6401 ()
{
  trace_input ("ld", OP_REG_OUTPUT, OP_POSTDEC, OP_VOID);
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
  INC_ADDR(State.regs[OP[1]],-2);
  trace_output (OP_REG);
}

/* ld */
void
OP_6001 ()
{
  trace_input ("ld", OP_REG_OUTPUT, OP_POSTINC, OP_VOID);
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
  INC_ADDR(State.regs[OP[1]],2);
  trace_output (OP_REG);
}

/* ld */
void
OP_6000 ()
{
  trace_input ("ld", OP_REG_OUTPUT, OP_MEMREF, OP_VOID);
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
  trace_output (OP_REG);
}

/* ld2w */
void
OP_31000000 ()
{
  trace_input ("ld2w", OP_REG_OUTPUT, OP_MEMREF, OP_VOID);
  State.regs[OP[0]] = RW (OP[1] + State.regs[OP[2]]);
  State.regs[OP[0]+1] = RW (OP[1] + State.regs[OP[2]] + 2);
  trace_output (OP_DREG);
}

/* ld2w */
void
OP_6601 ()
{
  trace_input ("ld2w", OP_REG_OUTPUT, OP_POSTDEC, OP_VOID);
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
  State.regs[OP[0]+1] = RW (State.regs[OP[1]]+2);
  INC_ADDR(State.regs[OP[1]],-4);
  trace_output (OP_DREG);
}

/* ld2w */
void
OP_6201 ()
{
  trace_input ("ld2w", OP_REG_OUTPUT, OP_POSTINC, OP_VOID);
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
  State.regs[OP[0]+1] = RW (State.regs[OP[1]]+2);
  INC_ADDR(State.regs[OP[1]],4);
  trace_output (OP_REG);
}

/* ld2w */
void
OP_6200 ()
{
  trace_input ("ld", OP_REG_OUTPUT, OP_MEMREF, OP_VOID);
  State.regs[OP[0]] = RW (State.regs[OP[1]]);
  State.regs[OP[0]+1] = RW (State.regs[OP[1]]+2);
  trace_output (OP_REG);
}

/* ldb */
void
OP_38000000 ()
{
  trace_input ("ldb", OP_REG_OUTPUT, OP_MEMREF2, OP_VOID);
  State.regs[OP[0]] = RB (OP[1] + State.regs[OP[2]]);
  SEXT8 (State.regs[OP[0]]);
  trace_output (OP_REG);
}

/* ldb */
void
OP_7000 ()
{
  trace_input ("ldb", OP_REG_OUTPUT, OP_MEMREF, OP_VOID);
  State.regs[OP[0]] = RB (State.regs[OP[1]]);
  SEXT8 (State.regs[OP[0]]);
  trace_output (OP_REG);
}

/* ldi.s */
void
OP_4001 ()
{
  trace_input ("ldi.s", OP_REG_OUTPUT, OP_CONSTANT4, OP_VOID);
  State.regs[OP[0]] = SEXT4(OP[1]);
  trace_output (OP_REG);
}

/* ldi.l */
void
OP_20000000 ()
{
  trace_input ("ldi.s", OP_REG_OUTPUT, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] = OP[1];
  trace_output (OP_REG);
}

/* ldub */
void
OP_39000000 ()
{
  trace_input ("ldub", OP_REG_OUTPUT, OP_MEMREF2, OP_VOID);
  State.regs[OP[0]] = RB (OP[1] + State.regs[OP[2]]);
  trace_output (OP_REG);
}

/* ldub */
void
OP_7200 ()
{
  trace_input ("ldub", OP_REG_OUTPUT, OP_MEMREF, OP_VOID);
  State.regs[OP[0]] = RB (State.regs[OP[1]]);
  trace_output (OP_REG);
}

/* mac */
void
OP_2A00 ()
{
  int64 tmp;

  trace_input ("mac", OP_ACCUM, OP_REG, OP_REG);
  tmp = SEXT40 ((int16)(State.regs[OP[1]]) * (int16)(State.regs[OP[2]]));

  if (State.FX)
    tmp = SEXT40( (tmp << 1) & MASK40);

  if (State.ST && tmp > MAX32)
    tmp = MAX32;

  tmp += SEXT40(State.a[OP[0]]);
  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if (tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* macsu */
void
OP_1A00 ()
{
  int64 tmp;

  trace_input ("macsu", OP_ACCUM, OP_REG, OP_REG);
  tmp = SEXT40 ((int16)State.regs[OP[1]] * State.regs[OP[2]]);
  if (State.FX)
    tmp = SEXT40( (tmp << 1) & MASK40);

  State.a[OP[0]] = (SEXT40 (State.a[OP[0]]) + tmp) & MASK40;
  trace_output (OP_ACCUM);
}

/* macu */
void
OP_3A00 ()
{
  int64 tmp;

  trace_input ("macu", OP_ACCUM, OP_REG, OP_REG);
  tmp = SEXT40 (State.regs[OP[1]] * State.regs[OP[2]]);
  if (State.FX)
    tmp = SEXT40( (tmp << 1) & MASK40);
  State.a[OP[0]] = (SEXT40 (State.a[OP[0]]) + tmp) & MASK40;
  trace_output (OP_ACCUM);
}

/* max */
void
OP_2600 ()
{
  trace_input ("max", OP_REG, OP_REG, OP_VOID);
  State.F1 = State.F0;
  if (State.regs[OP[1]] > State.regs[OP[0]])
    {
      State.regs[OP[0]] = State.regs[OP[1]];
      State.F0 = 1;
    }
  else
    State.F0 = 0;    
  trace_output (OP_REG);
}

/* max */
void
OP_3600 ()
{
  int64 tmp;

  trace_input ("max", OP_ACCUM, OP_DREG, OP_VOID);
  State.F1 = State.F0;
  tmp = SEXT16 (State.regs[OP[1]]) << 16 | State.regs[OP[1]+1];
  if (tmp > SEXT40(State.a[OP[0]]))
    {
      State.a[OP[0]] = tmp & MASK40;
      State.F0 = 1;
    }
  else
    State.F0 = 0;
  trace_output (OP_ACCUM);
}

/* max */
void
OP_3602 ()
{
  trace_input ("max", OP_ACCUM, OP_ACCUM, OP_VOID);
  State.F1 = State.F0;
  if (SEXT40(State.a[OP[1]]) > SEXT40(State.a[OP[0]]))
    {
      State.a[OP[0]] = State.a[OP[1]];
      State.F0 = 1;
    }
  else
    State.F0 = 0;
  trace_output (OP_ACCUM);
}


/* min */
void
OP_2601 ()
{
  trace_input ("min", OP_REG, OP_REG, OP_VOID);
  State.F1 = State.F0;
  if (State.regs[OP[1]] < State.regs[OP[0]])
    {
      State.regs[OP[0]] = State.regs[OP[1]];
      State.F0 = 1;
    }
  else
    State.F0 = 0;    
  trace_output (OP_REG);
}

/* min */
void
OP_3601 ()
{
  int64 tmp;

  trace_input ("min", OP_ACCUM, OP_DREG, OP_VOID);
  State.F1 = State.F0;
  tmp = SEXT16 (State.regs[OP[1]]) << 16 | State.regs[OP[1]+1];
  if (tmp < SEXT40(State.a[OP[0]]))
    {
      State.a[OP[0]] = tmp & MASK40;
      State.F0 = 1;
    }
  else
    State.F0 = 0;
  trace_output (OP_ACCUM);
}

/* min */
void
OP_3603 ()
{
  trace_input ("min", OP_ACCUM, OP_ACCUM, OP_VOID);
  State.F1 = State.F0;
  if (SEXT40(State.a[OP[1]]) < SEXT40(State.a[OP[0]]))
    {
      State.a[OP[0]] = State.a[OP[1]];
      State.F0 = 1;
    }
  else
    State.F0 = 0;
  trace_output (OP_ACCUM);
}

/* msb */
void
OP_2800 ()
{
  int64 tmp;

  trace_input ("msb", OP_ACCUM, OP_REG, OP_REG);
  tmp = SEXT40 ((int16)(State.regs[OP[1]]) * (int16)(State.regs[OP[2]]));

  if (State.FX)
    tmp = SEXT40 ((tmp << 1) & MASK40);

  if (State.ST && tmp > MAX32)
    tmp = MAX32;

  tmp = SEXT40(State.a[OP[0]]) - tmp;
  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if (tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* msbsu */
void
OP_1800 ()
{
  int64 tmp;

  trace_input ("msbsu", OP_ACCUM, OP_REG, OP_REG);
  tmp = SEXT40 ((int16)State.regs[OP[1]] * State.regs[OP[2]]);
  if (State.FX)
    tmp = SEXT40( (tmp << 1) & MASK40);

  State.a[OP[0]] = (SEXT40 (State.a[OP[0]]) - tmp) & MASK40;
  trace_output (OP_ACCUM);
}

/* msbu */
void
OP_3800 ()
{
  int64 tmp;

  trace_input ("msbu", OP_ACCUM, OP_REG, OP_REG);
  tmp = SEXT40 (State.regs[OP[1]] * State.regs[OP[2]]);
  if (State.FX)
    tmp = SEXT40( (tmp << 1) & MASK40);

  State.a[OP[0]] = (SEXT40 (State.a[OP[0]]) - tmp) & MASK40;
  trace_output (OP_ACCUM);
}

/* mul */
void
OP_2E00 ()
{
  trace_input ("mul", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] *= State.regs[OP[1]];
  trace_output (OP_REG);
}

/* mulx */
void
OP_2C00 ()
{
  int64 tmp;

  trace_input ("mulx", OP_ACCUM_OUTPUT, OP_REG, OP_REG);
  tmp = SEXT40 ((int16)(State.regs[OP[1]]) * (int16)(State.regs[OP[2]]));

  if (State.FX)
    tmp = SEXT40 ((tmp << 1) & MASK40);

  if (State.ST && tmp > MAX32)
    State.a[OP[0]] = MAX32;
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* mulxsu */
void
OP_1C00 ()
{
  int64 tmp;

  trace_input ("mulxsu", OP_ACCUM_OUTPUT, OP_REG, OP_REG);
  tmp = SEXT40 ((int16)(State.regs[OP[1]]) * State.regs[OP[2]]);

  if (State.FX)
    tmp <<= 1;

  State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* mulxu */
void
OP_3C00 ()
{
  int64 tmp;

  trace_input ("mulxu", OP_ACCUM_OUTPUT, OP_REG, OP_REG);
  tmp = SEXT40 (State.regs[OP[1]] * State.regs[OP[2]]);

  if (State.FX)
    tmp <<= 1;

  State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* mv */
void
OP_4000 ()
{
  trace_input ("mv", OP_REG_OUTPUT, OP_REG, OP_VOID);
  State.regs[OP[0]] = State.regs[OP[1]];
  trace_output (OP_REG);
}

/* mv2w */
void
OP_5000 ()
{
  trace_input ("mv2w", OP_DREG_OUTPUT, OP_DREG, OP_VOID);
  State.regs[OP[0]] = State.regs[OP[1]];
  State.regs[OP[0]+1] = State.regs[OP[1]+1];
  trace_output (OP_DREG);
}

/* mv2wfac */
void
OP_3E00 ()
{
  trace_input ("mv2wfac", OP_DREG_OUTPUT, OP_ACCUM, OP_VOID);
  State.regs[OP[0]] = (State.a[OP[1]] >> 16) & 0xffff;
  State.regs[OP[0]+1] = State.a[OP[1]] & 0xffff;
  trace_output (OP_DREG);
}

/* mv2wtac */
void
OP_3E01 ()
{
  trace_input ("mv2wtac", OP_ACCUM_OUTPUT, OP_DREG, OP_VOID);
  State.a[OP[1]] = (SEXT16 (State.regs[OP[0]]) << 16 | State.regs[OP[0]+1]) & MASK40;
  trace_output (OP_ACCUM);
}

/* mvac */
void
OP_3E03 ()
{
  trace_input ("mvac", OP_ACCUM_OUTPUT, OP_ACCUM, OP_VOID);
  State.a[OP[0]] = State.a[OP[1]];
  trace_output (OP_ACCUM);
}

/* mvb */
void
OP_5400 ()
{
  trace_input ("mvb", OP_REG_OUTPUT, OP_REG, OP_VOID);
  State.regs[OP[0]] = SEXT8 (State.regs[OP[1]] & 0xff);
  trace_output (OP_REG);
}

/* mvf0f */
void
OP_4400 ()
{
  trace_input ("mf0f", OP_REG_OUTPUT, OP_REG, OP_VOID);
  if (State.F0 == 0)
    State.regs[OP[0]] = State.regs[OP[1]];
  trace_output (OP_REG);
}

/* mvf0t */
void
OP_4401 ()
{
  trace_input ("mf0t", OP_REG_OUTPUT, OP_REG, OP_VOID);
  if (State.F0)
    State.regs[OP[0]] = State.regs[OP[1]];
  trace_output (OP_REG);
}

/* mvfacg */
void
OP_1E04 ()
{
  trace_input ("mvfacg", OP_REG_OUTPUT, OP_ACCUM, OP_VOID);
  State.regs[OP[0]] = (State.a[OP[1]] >> 32) & 0xff;
  trace_output (OP_ACCUM);
}

/* mvfachi */
void
OP_1E00 ()
{
  trace_input ("mvfachi", OP_REG_OUTPUT, OP_ACCUM, OP_VOID);
  State.regs[OP[0]] = (State.a[OP[1]] >> 16) & 0xffff;  
  trace_output (OP_REG);
}

/* mvfaclo */
void
OP_1E02 ()
{
  trace_input ("mvfaclo", OP_REG_OUTPUT, OP_ACCUM, OP_VOID);
  State.regs[OP[0]] = State.a[OP[1]] & 0xffff;
  trace_output (OP_REG);
}

/* mvfc */
void
OP_5200 ()
{
  trace_input ("mvfc", OP_REG_OUTPUT, OP_CR, OP_VOID);
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
  trace_output (OP_REG);
}

/* mvtacg */
void
OP_1E41 ()
{
  trace_input ("mvtacg", OP_REG, OP_ACCUM, OP_VOID);
  State.a[OP[1]] &= MASK32;
  State.a[OP[1]] |= (int64)(State.regs[OP[0]] & 0xff) << 32;
  trace_output (OP_ACCUM_REVERSE);
}

/* mvtachi */
void
OP_1E01 ()
{
  uint16 tmp;

  trace_input ("mvtachi", OP_REG, OP_ACCUM, OP_VOID);
  tmp = State.a[OP[1]] & 0xffff;
  State.a[OP[1]] = (SEXT16 (State.regs[OP[0]]) << 16 | tmp) & MASK40;
  trace_output (OP_ACCUM_REVERSE);
}

/* mvtaclo */
void
OP_1E21 ()
{
  trace_input ("mvtaclo", OP_REG, OP_ACCUM, OP_VOID);
  State.a[OP[1]] = (SEXT16 (State.regs[OP[0]])) & MASK40;
  trace_output (OP_ACCUM_REVERSE);
}

/* mvtc */
void
OP_5600 ()
{
  trace_input ("mvtc", OP_REG, OP_CR_OUTPUT, OP_VOID);
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
	  State.exception = SIGILL;
	}
    }
  trace_output (OP_CR_REVERSE);
}

/* mvub */
void
OP_5401 ()
{
  trace_input ("mvub", OP_REG_OUTPUT, OP_REG, OP_VOID);
  State.regs[OP[0]] = State.regs[OP[1]] & 0xff;
  trace_output (OP_REG);
}

/* neg */
void
OP_4605 ()
{
  trace_input ("neg", OP_REG, OP_VOID, OP_VOID);
  State.regs[OP[0]] = 0 - State.regs[OP[0]];
  trace_output (OP_REG);
}

/* neg */
void
OP_5605 ()
{
  int64 tmp;

  trace_input ("neg", OP_ACCUM, OP_VOID, OP_VOID);
  tmp = -SEXT40(State.a[OP[0]]);
  if (State.ST)
    {
      if ( tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if (tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}


/* nop */
void
OP_5E00 ()
{
  trace_input ("nop", OP_VOID, OP_VOID, OP_VOID);
  trace_output (OP_VOID);
}

/* not */
void
OP_4603 ()
{
  trace_input ("not", OP_REG, OP_VOID, OP_VOID);
  State.regs[OP[0]] = ~(State.regs[OP[0]]);  
  trace_output (OP_REG);
}

/* or */
void
OP_800 ()
{
  trace_input ("or", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] |= State.regs[OP[1]];
  trace_output (OP_REG);
}

/* or3 */
void
OP_4000000 ()
{
  trace_input ("or3", OP_REG_OUTPUT, OP_REG, OP_CONSTANT16);
  State.regs[OP[0]] = State.regs[OP[1]] | OP[2];
  trace_output (OP_REG);
}

/* rac */
void
OP_5201 ()
{
  int64 tmp;
  int shift = SEXT3 (OP[2]);

  trace_input ("rac", OP_DREG_OUTPUT, OP_ACCUM, OP_CONSTANT3);
  if (OP[1] != 0)
    {
      fprintf (stderr,"ERROR at PC 0x%x: instruction only valid for A0\n",PC<<2);
      State.exception = SIGILL;
    }

  State.F1 = State.F0;
  if (shift >=0)
    tmp = ((State.a[0] << 16) | (State.a[1] & 0xffff)) << shift;
  else
    tmp = ((State.a[0] << 16) | (State.a[1] & 0xffff)) >> -shift;
  tmp = ( SEXT60(tmp) + 0x8000 ) >> 16;
  if (tmp > MAX32)
    {
      State.regs[OP[0]] = 0x7fff;
      State.regs[OP[0]+1] = 0xffff;
      State.F0 = 1;
    } 
  else if (tmp < MIN32)
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
  trace_output (OP_DREG);
}

/* rachi */
void
OP_4201 ()
{
  int64 tmp;
  int shift = SEXT3 (OP[2]);

  trace_input ("rachi", OP_REG_OUTPUT, OP_ACCUM, OP_CONSTANT3);
  State.F1 = State.F0;
  if (shift >=0)
    tmp = SEXT44 (State.a[1]) << shift;
  else
    tmp = SEXT44 (State.a[1]) >> -shift;
  tmp += 0x8000;

  if (tmp > MAX32)
    {
      State.regs[OP[0]] = 0x7fff;
      State.F0 = 1;
    }
  else if (tmp < 0xfff80000000LL)
    {
      State.regs[OP[0]] = 0x8000;
      State.F0 = 1;
    }
  else
    {
      State.regs[OP[0]] = (tmp >> 16) & 0xffff;
      State.F0 = 0;
    }
  trace_output (OP_REG);
}

/* rep */
void
OP_27000000 ()
{
  trace_input ("rep", OP_REG, OP_CONSTANT16, OP_VOID);
  RPT_S = PC + 1;
  RPT_E = PC + OP[1];
  RPT_C = State.regs[OP[0]];
  State.RP = 1;
  if (RPT_C == 0)
    {
      fprintf (stderr, "ERROR: rep with count=0 is illegal.\n");
      State.exception = SIGILL;
    }
  if (OP[1] < 4)
    {
      fprintf (stderr, "ERROR: rep must include at least 4 instructions.\n");
      State.exception = SIGILL;
    }
  trace_output (OP_VOID);
}

/* repi */
void
OP_2F000000 ()
{
  trace_input ("repi", OP_CONSTANT16, OP_CONSTANT16, OP_VOID);
  RPT_S = PC + 1;
  RPT_E = PC + OP[1];
  RPT_C = OP[0];
  State.RP = 1;
  if (RPT_C == 0)
    {
      fprintf (stderr, "ERROR: repi with count=0 is illegal.\n");
      State.exception = SIGILL;
    }
  if (OP[1] < 4)
    {
      fprintf (stderr, "ERROR: repi must include at least 4 instructions.\n");
      State.exception = SIGILL;
    }
  trace_output (OP_VOID);
}

/* rtd */
void
OP_5F60 ()
{
  fprintf(stderr, "ERROR: rtd - NOT IMPLEMENTED\n");
  State.exception = SIGILL;
}

/* rte */
void
OP_5F40 ()
{
  trace_input ("rte", OP_VOID, OP_VOID, OP_VOID);
  PC = BPC;
  PSW = BPSW;
  trace_output (OP_VOID);
}

/* sadd */
void
OP_1223 ()
{
  int64 tmp;

  trace_input ("sadd", OP_ACCUM, OP_ACCUM, OP_VOID);
  tmp = SEXT40(State.a[OP[0]]) + (SEXT40(State.a[OP[1]]) >> 16);
  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if (tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* setf0f */
void
OP_4611 ()
{
  trace_input ("setf0f", OP_REG_OUTPUT, OP_VOID, OP_VOID);
  State.regs[OP[0]] = (State.F0 == 0) ? 1 : 0;
  trace_output (OP_REG);
}

/* setf0t */
void
OP_4613 ()
{
  trace_input ("setf0t", OP_REG_OUTPUT, OP_VOID, OP_VOID);
  State.regs[OP[0]] = (State.F0 == 1) ? 1 : 0;
  trace_output (OP_REG);
}

/* sleep */
void
OP_5FC0 ()
{
  trace_input ("sleep", OP_VOID, OP_VOID, OP_VOID);
  State.IE = 1;
  trace_output (OP_VOID);
}

/* sll */
void
OP_2200 ()
{
  trace_input ("sll", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] <<= (State.regs[OP[1]] & 0xf);
  trace_output (OP_REG);
}

/* sll */
void
OP_3200 ()
{
  int64 tmp;
  trace_input ("sll", OP_ACCUM, OP_REG, OP_VOID);
  if (State.regs[OP[1]] & 31 <= 16)
    tmp = SEXT40 (State.a[OP[0]]) << (State.regs[OP[1]] & 31);

  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if (tmp < 0xffffff80000000LL)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* slli */
void
OP_2201 ()
{
  trace_input ("slli", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] <<= OP[1];
  trace_output (OP_REG);
}

/* slli */
void
OP_3201 ()
{
  int64 tmp;

  if (OP[1] == 0)
    OP[1] = 16;

  trace_input ("slli", OP_ACCUM, OP_CONSTANT16, OP_VOID);
  tmp = SEXT40(State.a[OP[0]]) << OP[1];

  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if (tmp < 0xffffff80000000LL)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;
  trace_output (OP_ACCUM);
}

/* slx */
void
OP_460B ()
{
  uint16 tmp;

  trace_input ("slx", OP_REG, OP_FLAG, OP_VOID);
  State.regs[OP[0]] = (State.regs[OP[0]] << 1) | State.F0;
  trace_output (OP_REG);
}

/* sra */
void
OP_2400 ()
{
  trace_input ("sra", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] = ((int16)(State.regs[OP[0]])) >> (State.regs[OP[1]] & 0xf);
  trace_output (OP_REG);
}

/* sra */
void
OP_3400 ()
{
  trace_input ("sra", OP_ACCUM, OP_REG, OP_VOID);
  if (State.regs[OP[1]] & 31 <= 16)
    State.a[OP[0]] >>= (State.regs[OP[1]] & 31);
  trace_output (OP_ACCUM);
}

/* srai */
void
OP_2401 ()
{
  trace_input ("srai", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] = ((int16)(State.regs[OP[0]])) >> OP[1];
  trace_output (OP_REG);
}

/* srai */
void
OP_3401 ()
{
  if (OP[1] == 0)
    OP[1] = 16;

  trace_input ("srai", OP_ACCUM, OP_CONSTANT16, OP_VOID);
  State.a[OP[0]] >>= OP[1];
  trace_output (OP_ACCUM);
}

/* srl */
void
OP_2000 ()
{
  trace_input ("srl", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] >>=  (State.regs[OP[1]] & 0xf);
  trace_output (OP_REG);
}

/* srl */
void
OP_3000 ()
{
  trace_input ("srl", OP_ACCUM, OP_REG, OP_VOID);
  if (State.regs[OP[1]] & 31 <= 16)
    State.a[OP[0]] >>= (State.regs[OP[1]] & 31);
  trace_output (OP_ACCUM);
}

/* srli */
void
OP_2001 ()
{
  trace_input ("srli", OP_REG, OP_CONSTANT16, OP_VOID);
  State.regs[OP[0]] >>= OP[1];
  trace_output (OP_REG);
}

/* srli */
void
OP_3001 ()
{
  if (OP[1] == 0)
    OP[1] = 16;

  trace_input ("srli", OP_ACCUM, OP_CONSTANT16, OP_VOID);
  State.a[OP[0]] >>= OP[1];
  trace_output (OP_ACCUM);
}

/* srx */
void
OP_4609 ()
{
  uint16 tmp;

  trace_input ("srx", OP_REG, OP_FLAG, OP_VOID);
  tmp = State.F0 << 15;
  State.regs[OP[0]] = (State.regs[OP[0]] >> 1) | tmp;
  trace_output (OP_REG);
}

/* st */
void
OP_34000000 ()
{
  trace_input ("st", OP_REG, OP_MEMREF2, OP_VOID);
  SW (OP[1] + State.regs[OP[2]], State.regs[OP[0]]);
  trace_output (OP_VOID);
}

/* st */
void
OP_6800 ()
{
  trace_input ("st", OP_REG, OP_MEMREF, OP_VOID);
  SW (State.regs[OP[1]], State.regs[OP[0]]);
  trace_output (OP_VOID);
}

/* st */
void
OP_6C1F ()
{
  trace_input ("st", OP_REG, OP_PREDEC, OP_VOID);
  if ( OP[1] != 15 )
    {
      fprintf (stderr,"ERROR: cannot pre-decrement any registers but r15 (SP).\n");
      State.exception = SIGILL;
      return;
    }
  State.regs[OP[1]] -= 2;
  SW (State.regs[OP[1]], State.regs[OP[0]]);
  trace_output (OP_VOID);
}

/* st */
void
OP_6801 ()
{
  trace_input ("st", OP_REG, OP_POSTINC, OP_VOID);
  SW (State.regs[OP[1]], State.regs[OP[0]]);
  INC_ADDR (State.regs[OP[1]],2);
  trace_output (OP_VOID);
}

/* st */
void
OP_6C01 ()
{
  trace_input ("st", OP_REG, OP_POSTDEC, OP_VOID);
  SW (State.regs[OP[1]], State.regs[OP[0]]);
  INC_ADDR (State.regs[OP[1]],-2);
  trace_output (OP_VOID);
}

/* st2w */
void
OP_35000000 ()
{
  trace_input ("st2w", OP_DREG, OP_MEMREF2, OP_VOID);
  SW (State.regs[OP[2]]+OP[1], State.regs[OP[0]]);
  SW (State.regs[OP[2]]+OP[1]+2, State.regs[OP[0]+1]);
  trace_output (OP_VOID);
}

/* st2w */
void
OP_6A00 ()
{
  trace_input ("st2w", OP_REG, OP_MEMREF, OP_VOID);
  SW (State.regs[OP[1]],   State.regs[OP[0]]);
  SW (State.regs[OP[1]]+2, State.regs[OP[0]+1]);
  trace_output (OP_VOID);
}

/* st2w */
void
OP_6E1F ()
{
  trace_input ("st2w", OP_REG, OP_PREDEC, OP_VOID);
  if ( OP[1] != 15 )
    {
      fprintf (stderr,"ERROR: cannot pre-decrement any registers but r15 (SP).\n");
      State.exception = SIGILL;
      return;
    }
  State.regs[OP[1]] -= 4;
  SW (State.regs[OP[1]],   State.regs[OP[0]]);
  SW (State.regs[OP[1]]+2, State.regs[OP[0]+1]);
  trace_output (OP_VOID);
}

/* st2w */
void
OP_6A01 ()
{
  trace_input ("st2w", OP_REG, OP_POSTDEC, OP_VOID);
  SW (State.regs[OP[1]],   State.regs[OP[0]]);
  SW (State.regs[OP[1]]+2, State.regs[OP[0]+1]);
  INC_ADDR (State.regs[OP[1]],4);
  trace_output (OP_VOID);
}

/* st2w */
void
OP_6E01 ()
{
  trace_input ("st2w", OP_REG, OP_POSTINC, OP_VOID);
  SW (State.regs[OP[1]],   State.regs[OP[0]]);
  SW (State.regs[OP[1]]+2, State.regs[OP[0]+1]);
  INC_ADDR (State.regs[OP[1]],-4);
  trace_output (OP_VOID);
}

/* stb */
void
OP_3C000000 ()
{
  trace_input ("stb", OP_REG, OP_MEMREF2, OP_VOID);
  SB (State.regs[OP[2]]+OP[1], State.regs[OP[0]]);
  trace_output (OP_VOID);
}

/* stb */
void
OP_7800 ()
{
  trace_input ("stb", OP_REG, OP_MEMREF, OP_VOID);
  SB (State.regs[OP[1]], State.regs[OP[0]]);
  trace_output (OP_VOID);
}

/* stop */
void
OP_5FE0 ()
{
  trace_input ("stop", OP_VOID, OP_VOID, OP_VOID);
  State.exception = SIGQUIT;
  trace_output (OP_VOID);
}

/* sub */
void
OP_0 ()
{
  int32 tmp;

  trace_input ("sub", OP_REG, OP_REG, OP_VOID);
  tmp = (int16)State.regs[OP[0]]- (int16)State.regs[OP[1]];
  State.C = (tmp & 0xffff0000) ? 1 : 0;
  State.regs[OP[0]] = tmp & 0xffff;
  trace_output (OP_REG);
}

/* sub */
void
OP_1001 ()
{
  int64 tmp;

  trace_input ("sub", OP_ACCUM, OP_DREG, OP_VOID);
  tmp = SEXT40(State.a[OP[0]]) - (SEXT16 (State.regs[OP[1]]) << 16 | State.regs[OP[1]+1]);
  if (State.ST)
    {
      if ( tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if ( tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;

  trace_output (OP_ACCUM);
}

/* sub */

void
OP_1003 ()
{
  int64 tmp;

  trace_input ("sub", OP_ACCUM, OP_ACCUM, OP_VOID);
  tmp = SEXT40(State.a[OP[0]]) - SEXT40(State.a[OP[1]]);
  if (State.ST)
    {
      if (tmp > MAX32)
	State.a[OP[0]] = MAX32;
      else if ( tmp < MIN32)
	State.a[OP[0]] = MIN32;
      else
	State.a[OP[0]] = tmp & MASK40;
    }
  else
    State.a[OP[0]] = tmp & MASK40;

  trace_output (OP_ACCUM);
}

/* sub2w */
void
OP_1000 ()
{
  int64 tmp;
  int32 a,b;

  trace_input ("sub2w", OP_DREG, OP_DREG, OP_VOID);
  a = (int32)((State.regs[OP[0]] << 16) | State.regs[OP[0]+1]);
  b = (int32)((State.regs[OP[1]] << 16) | State.regs[OP[1]+1]);
  tmp = a-b;
  State.C = (tmp & 0xffffffff00000000LL) ? 1 : 0;  
  State.regs[OP[0]] = (tmp >> 16) & 0xffff;
  State.regs[OP[0]+1] = tmp & 0xffff;
  trace_output (OP_DREG);
}

/* subac3 */
void
OP_17000000 ()
{
  int64 tmp;

  trace_input ("subac3", OP_DREG_OUTPUT, OP_DREG, OP_ACCUM);
  tmp =  SEXT40 ((State.regs[OP[1]] << 16) | State.regs[OP[1]+1]) - SEXT40 (State.a[OP[2]]);
  State.regs[OP[0]] = (tmp >> 16) & 0xffff;
  State.regs[OP[0]+1] = tmp & 0xffff;
  trace_output (OP_DREG);
}

/* subac3 */
void
OP_17000002 ()
{
  int64 tmp;

  trace_input ("subac3", OP_DREG_OUTPUT, OP_ACCUM, OP_ACCUM);
  tmp = SEXT40(State.a[OP[1]]) - SEXT40(State.a[OP[2]]);
  State.regs[OP[0]] = (tmp >> 16) & 0xffff;
  State.regs[OP[0]+1] = tmp & 0xffff;
  trace_output (OP_DREG);
}

/* subac3s */
void
OP_17001000 ()
{
  int64 tmp;

  trace_input ("subac3s", OP_DREG_OUTPUT, OP_DREG, OP_ACCUM);
  State.F1 = State.F0;
  tmp = SEXT40 ((State.regs[OP[1]] << 16) | State.regs[OP[1]+1]) - SEXT40(State.a[OP[2]]);
  if ( tmp > MAX32)
    {
      State.regs[OP[0]] = 0x7fff;
      State.regs[OP[0]+1] = 0xffff;
      State.F0 = 1;
    }      
  else if (tmp < MIN32)
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
  trace_output (OP_DREG);
}

/* subac3s */
void
OP_17001002 ()
{
  int64 tmp;

  trace_input ("subac3s", OP_DREG_OUTPUT, OP_ACCUM, OP_ACCUM);
  State.F1 = State.F0;
  tmp = SEXT40(State.a[OP[1]]) - SEXT40(State.a[OP[2]]);
  if ( tmp > MAX32)
    {
      State.regs[OP[0]] = 0x7fff;
      State.regs[OP[0]+1] = 0xffff;
      State.F0 = 1;
    }      
  else if (tmp < MIN32)
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
  trace_output (OP_DREG);
}

/* subi */
void
OP_1 ()
{
  int32 tmp;
  if (OP[1] == 0)
    OP[1] = 16;

  trace_input ("subi", OP_REG, OP_CONSTANT16, OP_VOID);
  tmp = (int16)State.regs[OP[0]] - OP[1];
  State.C = (tmp & 0xffff0000) ? 1 : 0;
  State.regs[OP[0]] = tmp & 0xffff;  
  trace_output (OP_REG);
}

/* trap */
void
OP_5F00 ()
{
  trace_input ("trap", OP_CONSTANT16, OP_REG, OP_VOID);
  trace_output (OP_VOID);
  
  switch (OP[0])
    {
    default:
      fprintf (stderr, "Unknown trap code %d\n", OP[0]);
      State.exception = SIGILL;

    case 0:
      /* Trap 0 is used for simulating low-level I/O */
      {
	int save_errno = errno;	
	errno = 0;

/* Registers passed to trap 0 */

#define FUNC   State.regs[2]	/* function number, return value */
#define PARM1  State.regs[3]	/* optional parm 1 */
#define PARM2  State.regs[4]	/* optional parm 2 */
#define PARM3  State.regs[5]	/* optional parm 3 */

/* Registers set by trap 0 */

#define RETVAL State.regs[2]	/* return value */
#define RETERR State.regs[3]	/* return error code */

/* Turn a pointer in a register into a pointer into real memory. */

#define MEMPTR(x) ((char *)((x) + State.imem))

	switch (FUNC)
	  {
#if !defined(__GO32__) && !defined(_WIN32)
#ifdef SYS_fork
	  case SYS_fork:
	    RETVAL = fork ();
	    break;
#endif
#ifdef SYS_execve
	  case SYS_execve:
	    RETVAL = execve (MEMPTR (PARM1), (char **) MEMPTR (PARM2),
			     (char **)MEMPTR (PARM3));
	    break;
#endif
#ifdef SYS_execv
	  case SYS_execv:
	    RETVAL = execve (MEMPTR (PARM1), (char **) MEMPTR (PARM2), NULL);
	    break;
#endif
#ifdef SYS_pipe
	  case SYS_pipe:
	    {
	      reg_t buf;
	      int host_fd[2];

	      buf = PARM1;
	      RETVAL = pipe (host_fd);
	      SW (buf, host_fd[0]);
	      buf += sizeof(uint16);
	      SW (buf, host_fd[1]);
	    }
	  break;
#endif
#ifdef SYS_wait
	  case SYS_wait:
	    {
	      int status;

	      RETVAL = wait (&status);
	      SW (PARM1, status);
	    }
	  break;
#endif
#endif

#ifdef SYS_read
	  case SYS_read:
	    RETVAL = d10v_callback->read (d10v_callback, PARM1, MEMPTR (PARM2),
					  PARM3);
	    break;
#endif
#ifdef SYS_write
	  case SYS_write:
	    if (PARM1 == 1)
	      RETVAL = (int)d10v_callback->write_stdout (d10v_callback,
							 MEMPTR (PARM2), PARM3);
	    else
	      RETVAL = (int)d10v_callback->write (d10v_callback, PARM1,
						  MEMPTR (PARM2), PARM3);
	    break;
#endif
#ifdef SYS_lseek
	  case SYS_lseek:
	    RETVAL = d10v_callback->lseek (d10v_callback, PARM1, PARM2, PARM3);
	    break;
#endif
#ifdef SYS_close
	  case SYS_close:
	    RETVAL = d10v_callback->close (d10v_callback, PARM1);
	    break;
#endif
#ifdef SYS_open
	  case SYS_open:
	    RETVAL = d10v_callback->open (d10v_callback, MEMPTR (PARM1), PARM2);
	    break;
#endif
#ifdef SYS_exit
	  case SYS_exit:
	    /* EXIT - caller can look in PARM1 to work out the 
	       reason */
	    State.exception = SIGQUIT;
	    break;
#endif

#ifdef SYS_stat
	  case SYS_stat:
	    /* stat system call */
	    {
	      struct stat host_stat;
	      reg_t buf;

	      RETVAL = stat (MEMPTR (PARM1), &host_stat);

	      buf = PARM2;

	      /* The hard-coded offsets and sizes were determined by using
	       * the D10V compiler on a test program that used struct stat.
	       */
	      SW  (buf,    host_stat.st_dev);
	      SW  (buf+2,  host_stat.st_ino);
	      SW  (buf+4,  host_stat.st_mode);
	      SW  (buf+6,  host_stat.st_nlink);
	      SW  (buf+8,  host_stat.st_uid);
	      SW  (buf+10, host_stat.st_gid);
	      SW  (buf+12, host_stat.st_rdev);
	      SLW (buf+16, host_stat.st_size);
	      SLW (buf+20, host_stat.st_atime);
	      SLW (buf+28, host_stat.st_mtime);
	      SLW (buf+36, host_stat.st_ctime);
	    }
	    break;
#endif

#ifdef SYS_chown
	  case SYS_chown:
	    RETVAL = chown (MEMPTR (PARM1), PARM2, PARM3);
	    break;
#endif
#ifdef SYS_chmod
	  case SYS_chmod:
	    RETVAL = chmod (MEMPTR (PARM1), PARM2);
	    break;
#endif
#ifdef SYS_utime
	  case SYS_utime:
	    /* Cast the second argument to void *, to avoid type mismatch
	       if a prototype is present.  */
	    RETVAL = utime (MEMPTR (PARM1), (void *) MEMPTR (PARM2));
	    break;
#endif
	  default:
	    abort ();
	  }
	RETERR = errno;
	errno = save_errno;
	break;
      }

    case 1:
      /* Trap 1 prints a string */
      {
	char *fstr = State.regs[2] + State.imem;
	fputs (fstr, stdout);
	break;
      }

    case 2:
      /* Trap 2 calls printf */
      {
	char *fstr = State.regs[2] + State.imem;
	printf (fstr, (int16)State.regs[3], (int16)State.regs[4], (int16)State.regs[5]);
	break;
      }

    case 3:
      /* Trap 3 writes a character */
      putchar (State.regs[2]);
      break;
    }
}

/* tst0i */
void
OP_7000000 ()
{
  trace_input ("tst0i", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (State.regs[OP[0]] & OP[1]) ? 1 : 0;
  trace_output (OP_FLAG);
}

/* tst1i */
void
OP_F000000 ()
{
  trace_input ("tst1i", OP_REG, OP_CONSTANT16, OP_VOID);
  State.F1 = State.F0;
  State.F0 = (~(State.regs[OP[0]]) & OP[1]) ? 1 : 0;
  trace_output (OP_FLAG);
}

/* wait */
void
OP_5F80 ()
{
  trace_input ("wait", OP_VOID, OP_VOID, OP_VOID);
  State.IE = 1;
  trace_output (OP_VOID);
}

/* xor */
void
OP_A00 ()
{
  trace_input ("xor", OP_REG, OP_REG, OP_VOID);
  State.regs[OP[0]] ^= State.regs[OP[1]];
  trace_output (OP_REG);
}

/* xor3 */
void
OP_5000000 ()
{
  trace_input ("xor3", OP_REG_OUTPUT, OP_REG, OP_CONSTANT16);
  State.regs[OP[0]] = State.regs[OP[1]] ^ OP[2];
  trace_output (OP_REG);
}

