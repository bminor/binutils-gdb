#include <signal.h>
#include "v850_sim.h"
#include "simops.h"
#include "sys/syscall.h"
#include "bfd.h"

enum op_types {
  OP_UNKNOWN,
  OP_NONE,
  OP_TRAP,
  OP_REG,
  OP_REG_REG,
  OP_REG_REG_CMP,
  OP_REG_REG_MOVE,
  OP_IMM_REG,
  OP_IMM_REG_CMP,
  OP_IMM_REG_MOVE,
  OP_COND_BR,
  OP_LOAD16,
  OP_STORE16,
  OP_LOAD32,
  OP_STORE32,
  OP_JUMP,
  OP_IMM_REG_REG,
  OP_UIMM_REG_REG,
  OP_BIT,
  OP_EX1,
  OP_EX2,
  OP_LDSR,
  OP_STSR
};

#ifdef DEBUG
static void trace_input PARAMS ((char *name, enum op_types type, int size));
static void trace_output PARAMS ((enum op_types result));
static int init_text_p = 0;
static asection *text;
static bfd_vma text_start;
static bfd_vma text_end;
extern bfd *exec_bfd;

#ifndef SIZE_INSTRUCTION
#define SIZE_INSTRUCTION 6
#endif

#ifndef SIZE_OPERANDS
#define SIZE_OPERANDS 16
#endif

#ifndef SIZE_VALUES
#define SIZE_VALUES 11
#endif

#ifndef SIZE_LOCATION
#define SIZE_LOCATION 40
#endif

static void
trace_input (name, type, size)
     char *name;
     enum op_types type;
     int size;
{
  char buf[1024];
  char *p;
  uint32 values[3];
  int num_values, i;
  char *cond;
  asection *s;
  const char *filename;
  const char *functionname;
  unsigned int linenumber;

  if ((v850_debug & DEBUG_TRACE) == 0)
    return;

  buf[0] = '\0';
  if (!init_text_p)
    {
      init_text_p = 1;
      for (s = exec_bfd->sections; s; s = s->next)
	if (strcmp (bfd_get_section_name (exec_bfd, s), ".text") == 0)
	  {
	    text = s;
	    text_start = bfd_get_section_vma (exec_bfd, s);
	    text_end = text_start + bfd_section_size (exec_bfd, s);
	    break;
	  }
    }

  if (text && PC >= text_start && PC < text_end)
    {
      filename = (const char *)0;
      functionname = (const char *)0;
      linenumber = 0;
      if (bfd_find_nearest_line (exec_bfd, text, (struct symbol_cache_entry **)0, PC - text_start,
				 &filename, &functionname, &linenumber))
	{
	  p = buf;
	  if (linenumber)
	    {
	      sprintf (p, "Line %5d ", linenumber);
	      p += strlen (p);
	    }

	  if (functionname)
	    {
	      sprintf (p, "Func %s ", functionname);
	      p += strlen (p);
	    }
	  else if (filename)
	    {
	      char *q = (char *) strrchr (filename, '/');
	      sprintf (p, "File %s ", (q) ? q+1 : filename);
	      p += strlen (p);
	    }

	  if (*p == ' ')
	    *p = '\0';
	}
    }

  (*v850_callback->printf_filtered) (v850_callback, "0x%.8x: %-*.*s %-*s",
				     (unsigned)PC,
				     SIZE_LOCATION, SIZE_LOCATION, buf,
				     SIZE_INSTRUCTION, name);

  switch (type)
    {
    default:
    case OP_UNKNOWN:
    case OP_NONE:
      strcpy (buf, "unknown");
      break;

    case OP_TRAP:
      sprintf (buf, "%d", OP[0]);
      break;

    case OP_REG:
      sprintf (buf, "r%d", OP[0]);
      break;

    case OP_REG_REG:
    case OP_REG_REG_CMP:
    case OP_REG_REG_MOVE:
      sprintf (buf, "r%d,r%d", OP[0], OP[1]);
      break;

    case OP_IMM_REG:
    case OP_IMM_REG_CMP:
    case OP_IMM_REG_MOVE:
      sprintf (buf, "%d,r%d", OP[1], OP[0]);
      break;

    case OP_COND_BR:
      sprintf (buf, "%d", SEXT9 (OP[0]));
      break;

    case OP_LOAD16:
      sprintf (buf, "%d[r30],r%d", SEXT7 (OP[1]) * size, OP[0]);
      break;

    case OP_STORE16:
      sprintf (buf, "r%d,%d[r30]", OP[0], SEXT7 (OP[1]) * size);
      break;

    case OP_LOAD32:
      sprintf (buf, "%d[r%d],r%d", SEXT16 (OP[2]), OP[0], OP[1]);
      break;

    case OP_STORE32:
      sprintf (buf, "r%d,%d[r%d]", OP[1], SEXT16 (OP[2]), OP[0]);
      break;

    case OP_JUMP:
      sprintf (buf, "%d,r%d", SEXT22 (OP[0]), OP[1]);
      break;

    case OP_IMM_REG_REG:
      sprintf (buf, "%d,r%d,r%d", SEXT16 (OP[0]), OP[1], OP[2]);
      break;

    case OP_UIMM_REG_REG:
      sprintf (buf, "%d,r%d,r%d", OP[0] & 0xffff, OP[1], OP[2]);
      break;

    case OP_BIT:
      sprintf (buf, "%d,%d[r%d]", OP[1] & 0x7, SEXT16 (OP[2]), OP[0]);
      break;

    case OP_EX1:
      switch (OP[0] & 0xf)
	{
	default:  cond = "?";	break;
	case 0x0: cond = "v";	break;
	case 0x1: cond = "c";	break;
	case 0x2: cond = "z";	break;
	case 0x3: cond = "nh";	break;
	case 0x4: cond = "s";	break;
	case 0x5: cond = "t";	break;
	case 0x6: cond = "lt";	break;
	case 0x7: cond = "le";	break;
	case 0x8: cond = "nv";	break;
	case 0x9: cond = "nc";	break;
	case 0xa: cond = "nz";	break;
	case 0xb: cond = "h";	break;
	case 0xc: cond = "ns";	break;
	case 0xd: cond = "sa";	break;
	case 0xe: cond = "ge";	break;
	case 0xf: cond = "gt";	break;
	}

      sprintf (buf, "%s,r%d", cond, OP[1]);
      break;

    case OP_EX2:
      strcpy (buf, "EX2");
      break;

    case OP_LDSR:
    case OP_STSR:
      sprintf (buf, "r%d,s%d", OP[0], OP[1]);
      break;
    }

  if ((v850_debug & DEBUG_VALUES) == 0)
    {
      (*v850_callback->printf_filtered) (v850_callback, "%s\n", buf);
    }
  else
    {
      (*v850_callback->printf_filtered) (v850_callback, "%-*s", SIZE_OPERANDS, buf);
      switch (type)
	{
	default:
	case OP_UNKNOWN:
	case OP_NONE:
	case OP_TRAP:
	  num_values = 0;
	  break;

	case OP_REG:
	case OP_REG_REG_MOVE:
	  values[0] = State.regs[OP[0]];
	  num_values = 1;
	  break;

	case OP_REG_REG:
	case OP_REG_REG_CMP:
	  values[0] = State.regs[OP[1]];
	  values[1] = State.regs[OP[0]];
	  num_values = 2;
	  break;

	case OP_IMM_REG:
	case OP_IMM_REG_CMP:
	  values[0] = SEXT5 (OP[0]);
	  values[1] = OP[1];
	  num_values = 2;
	  break;

	case OP_IMM_REG_MOVE:
	  values[0] = SEXT5 (OP[0]);
	  num_values = 1;
	  break;

	case OP_COND_BR:
	  values[0] = State.pc;
	  values[1] = SEXT9 (OP[0]);
	  values[2] = State.sregs[5];
	  num_values = 3;
	  break;

	case OP_LOAD16:
	  values[0] = SEXT7 (OP[1]) * size;
	  values[1] = State.regs[30];
	  num_values = 2;
	  break;

	case OP_STORE16:
	  values[0] = State.regs[OP[0]];
	  values[1] = SEXT7 (OP[1]) * size;
	  values[2] = State.regs[30];
	  num_values = 3;
	  break;

	case OP_LOAD32:
	  values[0] = SEXT16 (OP[2]);
	  values[1] = State.regs[OP[0]];
	  num_values = 2;
	  break;

	case OP_STORE32:
	  values[0] = State.regs[OP[1]];
	  values[1] = SEXT16 (OP[2]);
	  values[2] = State.regs[OP[0]];
	  num_values = 3;
	  break;

	case OP_JUMP:
	  values[0] = SEXT22 (OP[0]);
	  values[1] = State.pc;
	  num_values = 2;
	  break;

	case OP_IMM_REG_REG:
	  values[0] = SEXT16 (OP[0]) << size;
	  values[1] = State.regs[OP[1]];
	  num_values = 2;
	  break;

	case OP_UIMM_REG_REG:
	  values[0] = (OP[0] & 0xffff) << size;
	  values[1] = State.regs[OP[1]];
	  num_values = 2;
	  break;

	case OP_BIT:
	  num_values = 0;
	  break;

	case OP_EX1:
	  values[0] = State.sregs[5];
	  num_values = 1;
	  break;

	case OP_EX2:
	  num_values = 0;
	  break;

	case OP_LDSR:
	  values[0] = State.regs[OP[0]];
	  num_values = 1;
	  break;

	case OP_STSR:
	  values[0] = State.sregs[OP[1]];
	  num_values = 1;
	}

      for (i = 0; i < num_values; i++)
	(*v850_callback->printf_filtered) (v850_callback, "%*s0x%.8lx", SIZE_VALUES - 10, "", values[i]);

      while (i++ < 3)
	(*v850_callback->printf_filtered) (v850_callback, "%*s", SIZE_VALUES, "");
    }
}

static void
trace_output (result)
     enum op_types result;
{
  if ((v850_debug & (DEBUG_TRACE | DEBUG_VALUES)) == (DEBUG_TRACE | DEBUG_VALUES))
    {
      switch (result)
	{
	default:
	case OP_UNKNOWN:
	case OP_NONE:
	case OP_TRAP:
	case OP_REG:
	case OP_REG_REG_CMP:
	case OP_IMM_REG_CMP:
	case OP_COND_BR:
	case OP_STORE16:
	case OP_STORE32:
	case OP_BIT:
	case OP_EX2:
	  break;

	case OP_LOAD16:
	case OP_STSR:
	  (*v850_callback->printf_filtered) (v850_callback, " :: 0x%.8lx",
					     (unsigned long)State.regs[OP[0]]);
	  break;

	case OP_REG_REG:
	case OP_REG_REG_MOVE:
	case OP_IMM_REG:
	case OP_IMM_REG_MOVE:
	case OP_LOAD32:
	case OP_EX1:
	  (*v850_callback->printf_filtered) (v850_callback, " :: 0x%.8lx",
					     (unsigned long)State.regs[OP[1]]);
	  break;

	case OP_IMM_REG_REG:
	case OP_UIMM_REG_REG:
	  (*v850_callback->printf_filtered) (v850_callback, " :: 0x%.8lx",
					     (unsigned long)State.regs[OP[2]]);
	  break;

	case OP_JUMP:
	  if (OP[1] != 0)
	    (*v850_callback->printf_filtered) (v850_callback, " :: 0x%.8lx",
					       (unsigned long)State.regs[OP[1]]);
	  break;

	case OP_LDSR:
	  (*v850_callback->printf_filtered) (v850_callback, " :: 0x%.8lx",
					     (unsigned long)State.sregs[OP[1]]);
	  break;
	}

      (*v850_callback->printf_filtered) (v850_callback, "\n");
    }
}

#else
#define trace_input(NAME, IN1, IN2)
#define trace_output(RESULT)
#endif


/* sld.b */
void
OP_300 ()
{
  unsigned int op2;
  int result, temp;

  trace_input ("sld.b", OP_LOAD16, 1);
  temp = OP[1];
  temp = SEXT7 (temp);
  op2 = temp;
  result = get_byte (State.mem + State.regs[30] + op2);
  State.regs[OP[0]] = SEXT8 (result);
  trace_output (OP_LOAD16);
}

/* sld.h */
void
OP_400 ()
{
  unsigned int op2;
  int result, temp;

  trace_input ("sld.h", OP_LOAD16, 2);
  temp = OP[1];
  temp = SEXT7 (temp);
  op2 = temp << 1;
  result = get_half (State.mem + State.regs[30] + op2);
  State.regs[OP[0]] = SEXT16 (result);
  trace_output (OP_LOAD16);
}

/* sld.w */
void
OP_500 ()
{
  unsigned int op2;
  int result, temp;

  trace_input ("sld.w", OP_LOAD16, 4);
  temp = OP[1];
  temp = SEXT7 (temp);
  op2 = temp << 2;
  result = get_word (State.mem + State.regs[30] + op2);
  State.regs[OP[0]] = result;
  trace_output (OP_LOAD16);
}

/* sst.b */
void
OP_380 ()
{
  unsigned int op0, op1;
  int temp;

  trace_input ("sst.b", OP_STORE16, 1);
  op0 = State.regs[OP[0]];
  temp = OP[1];
  temp = SEXT7 (temp);
  op1 = temp;
  put_byte (State.mem + State.regs[30] + op1, op0);
  trace_output (OP_STORE16);
}

/* sst.h */
void
OP_480 ()
{
  unsigned int op0, op1;
  int temp;

  trace_input ("sst.h", OP_STORE16, 2);
  op0 = State.regs[OP[0]];
  temp = OP[1];
  temp = SEXT7 (temp);
  op1 = temp << 1;
  put_half (State.mem + State.regs[30] + op1, op0);
  trace_output (OP_STORE16);
}

/* sst.w */
void
OP_501 ()
{
  unsigned int op0, op1;
  int temp;

  trace_input ("sst.w", OP_STORE16, 4);
  op0 = State.regs[OP[0]];
  temp = OP[1];
  temp = SEXT7 (temp);
  op1 = temp << 2;
  put_word (State.mem + State.regs[30] + op1, op0);
  trace_output (OP_STORE16);
}

/* ld.b */
void
OP_700 ()
{
  unsigned int op0, op2;
  int result, temp;

  trace_input ("ld.b", OP_LOAD32, 1);
  op0 = State.regs[OP[0]];
  temp = SEXT16 (OP[2]);
  op2 = temp;
  result = get_byte (State.mem + op0 + op2);
  State.regs[OP[1]] = SEXT8 (result);
  trace_output (OP_LOAD32);
}

/* ld.h */
void
OP_720 ()
{
  unsigned int op0, op2;
  int result, temp;

  trace_input ("ld.h", OP_LOAD32, 2);
  op0 = State.regs[OP[0]];
  temp = SEXT16 (OP[2]);
  temp &= ~0x1;
  op2 = temp;
  result = get_half (State.mem + op0 + op2);
  State.regs[OP[1]] = SEXT16 (result);
  trace_output (OP_LOAD32);
}

/* ld.w */
void
OP_10720 ()
{
  unsigned int op0,  op2;
  int result, temp;

  trace_input ("ld.w", OP_LOAD32, 4);
  op0 = State.regs[OP[0]];
  temp = SEXT16 (OP[2]);
  temp &= ~0x1;
  op2 = temp;
  result = get_word (State.mem + op0 + op2);
  State.regs[OP[1]] = result;
  trace_output (OP_LOAD32);
}

/* st.b */
void
OP_740 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("st.b", OP_STORE32, 1);
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  temp = SEXT16 (OP[2]);
  op2 = temp;
  put_byte (State.mem + op0 + op2, op1);
  trace_output (OP_STORE32);
}

/* st.h */
void
OP_760 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("st.h", OP_STORE32, 2);
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  temp = SEXT16 (OP[2] & ~0x1);
  op2 = temp;
  put_half (State.mem + op0 + op2, op1);
  trace_output (OP_STORE32);
}

/* st.w */
void
OP_10760 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("st.w", OP_STORE32, 4);
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  temp = SEXT16 (OP[2] & ~0x1);
  op2 = temp;
  put_word (State.mem + op0 + op2, op1);
  trace_output (OP_STORE32);
}

/* bv disp9 */
void
OP_580 ()
{
  unsigned int psw;
  int op0;

  trace_input ("bv", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_OV) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bl disp9 */
void
OP_581 ()
{
  unsigned int psw;
  int op0;

  trace_input ("bl", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_CY) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* be disp9 */
void
OP_582 ()
{
  unsigned int psw;
  int op0;

  trace_input ("be", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_Z) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bnh disp 9*/
void
OP_583 ()
{
  unsigned int psw;
  int op0;

  trace_input ("bnh", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((((psw & PSW_CY) != 0) | ((psw & PSW_Z) != 0)) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bn disp9 */
void
OP_584 ()
{
  unsigned int psw;
  int op0;

  trace_input ("bn", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_S) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* br disp9 */
void
OP_585 ()
{
  unsigned int psw;
  int op0;

  trace_input ("br", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  State.pc += op0;
  trace_output (OP_COND_BR);
}

/* blt disp9 */
void
OP_586 ()
{
  unsigned int psw;
  int op0;

  trace_input ("blt", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0)) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* ble disp9 */
void
OP_587 ()
{
  unsigned int psw;
  int op0;

  trace_input ("ble", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((((psw & PSW_Z) != 0)
       || (((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0))) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bnv disp9 */
void
OP_588 ()
{
  unsigned int psw;
  int op0;

  trace_input ("bnv", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_OV) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bnl disp9 */
void
OP_589 ()
{
  unsigned int psw;
  int op0;

  trace_input ("bnl", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_CY) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bne disp9 */
void
OP_58A ()
{
  unsigned int psw;
  int op0;

  trace_input ("bne", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_Z) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bh disp9 */
void
OP_58B ()
{
  unsigned int psw;
  int op0;

  trace_input ("bh", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((((psw & PSW_CY) != 0) | ((psw & PSW_Z) != 0)) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bp disp9 */
void
OP_58C ()
{
  unsigned int psw;
  int op0;

  trace_input ("bp", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_S) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bsa disp9 */
void
OP_58D ()
{
  unsigned int psw;
  int op0;

  trace_input ("bsa", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((psw & PSW_SAT) != 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bge disp9 */
void
OP_58E ()
{
  unsigned int psw;
  int op0;

  trace_input ("bge", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0)) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* bgt disp9 */
void
OP_58F ()
{
  unsigned int psw;
  int op0;

  trace_input ("bgt", OP_COND_BR, 0);
  op0 = SEXT9 (OP[0]);
  psw = State.sregs[5];
  
  if ((((psw & PSW_Z) != 0)
       || (((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0))) == 0)
    State.pc += op0;
  else
    State.pc += 2;
  trace_output (OP_COND_BR);
}

/* jmp [reg1] */
void
OP_60 ()
{
  /* interp.c will bump this by +2, so correct for it here.  */
  trace_input ("jmp", OP_REG, 0);
  State.pc = State.regs[OP[0]] - 2;
  trace_output (OP_REG);
}

/* jarl disp22, reg */
void
OP_780 ()
{
  unsigned int op0, opc;
  int temp;

  trace_input ("jarl", OP_JUMP, 0);
  temp = SEXT22 (OP[0]);
  op0 = temp;
  opc = State.pc;

  State.pc += temp;

  /* Gross.  jarl X,r0 is really jr and doesn't save its result.  */
  if (OP[1] != 0)
    State.regs[OP[1]] = opc + 4;
  trace_output (OP_JUMP);
}

/* add reg, reg */
void
OP_1C0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  trace_input ("add", OP_REG_REG, 0);
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
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		     | (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_REG_REG);
}

/* add sign_extend(imm5), reg */
void
OP_240 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;
  int temp;

  trace_input ("add", OP_IMM_REG, 0);

  /* Compute the result.  */
  temp = SEXT5 (OP[0]);
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
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_IMM_REG);
}

/* addi sign_extend(imm16), reg, reg */
void
OP_600 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;
  int temp;

  trace_input ("addi", OP_IMM_REG_REG, 0);

  /* Compute the result.  */
  temp = SEXT16 (OP[0]);
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
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_IMM_REG_REG);
}

/* sub reg1, reg2 */
void
OP_1A0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  trace_input ("sub", OP_REG_REG, 0);
  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op1 - op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 < op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_REG_REG);
}

/* subr reg1, reg2 */
void
OP_180 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  trace_input ("subr", OP_REG_REG, 0);
  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 - op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op0 < op1);
  ov = ((op0 & 0x80000000) != (op1 & 0x80000000)
	&& (op0 & 0x80000000) != (result & 0x80000000));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_REG_REG);
}

/* mulh reg1, reg2 */
void
OP_E0 ()
{
  trace_input ("mulh", OP_REG_REG, 0);
  State.regs[OP[1]] = ((State.regs[OP[1]] & 0xffff)
		       * (State.regs[OP[0]] & 0xffff));
  trace_output (OP_REG_REG);
}

/* mulh sign_extend(imm5), reg2

   Condition codes */
void
OP_2E0 ()
{
  int value = SEXT5 (OP[0]);
 
  trace_input ("mulh", OP_IMM_REG, 0);
  State.regs[OP[1]] = (State.regs[OP[1]] & 0xffff) * value;
  trace_output (OP_IMM_REG);
}

/* mulhi imm16, reg1, reg2 */
void
OP_6E0 ()
{
  int value = OP[0] & 0xffff;

  trace_input ("mulhi", OP_IMM_REG_REG, 0);
  State.regs[OP[2]] = (State.regs[OP[1]] & 0xffff) * value;
  trace_output (OP_IMM_REG_REG);
}

/* divh reg1, reg2 */
void
OP_40 ()
{
  unsigned int op0, op1, result, ov, s, z;
  int temp;

  trace_input ("divh", OP_REG_REG, 0);

  /* Compute the result.  */
  temp = SEXT16 (State.regs[OP[0]]);
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
    {
      result = 0x0;
      ov = 1;
    }

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (ov ? PSW_OV : 0));
  trace_output (OP_REG_REG);
}

/* cmp reg, reg */
void
OP_1E0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;

  trace_input ("cmp", OP_REG_REG_CMP, 0);
  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op1 - op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 < op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));

  /* Set condition codes.  */
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_REG_REG_CMP);
}

/* cmp sign_extend(imm5), reg */
void
OP_260 ()
{
  unsigned int op0, op1, result, z, s, cy, ov;
  int temp;

  /* Compute the result.  */
  trace_input ("cmp", OP_IMM_REG_CMP, 0);
  temp = SEXT5 (OP[0]);
  op0 = temp;
  op1 = State.regs[OP[1]];
  result = op1 - op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 < op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));

  /* Set condition codes.  */
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0));
  trace_output (OP_IMM_REG_CMP);
}

/* setf cccc,reg2 */
void
OP_7E0 ()
{
  /* Hack alert.  We turn off a bit in op0 since we really only
     wanted 4 bits.  */
  unsigned int op0, psw, result = 0;

  trace_input ("setf", OP_EX1, 0);
  op0 = OP[0] & 0xf;
  psw = State.sregs[5];

  switch (op0)
    {
      case 0x0:
	result = ((psw & PSW_OV) != 0);
	break;
      case 0x1:
	result = ((psw & PSW_CY) != 0);
	break;
      case 0x2:
	result = ((psw & PSW_Z) != 0);
	break;
      case 0x3:
	result = ((((psw & PSW_CY) != 0) | ((psw & PSW_Z) != 0)) != 0);
	break;
      case 0x4:
	result = ((psw & PSW_S) != 0);
	break;
      case 0x5:
	result = 1;
	break;
      case 0x6:
	result = ((((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0)) != 0);
	break;
      case 0x7:
	result = (((((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0))
		  || ((psw & PSW_Z) != 0)) != 0);
	break;
      case 0x8:
	result = ((psw & PSW_OV) == 0);
	break;
      case 0x9:
	result = ((psw & PSW_CY) == 0);
	break;
      case 0xa:
	result = ((psw & PSW_Z) == 0);
	break;
      case 0xb:
	result = ((((psw & PSW_CY) != 0) | ((psw & PSW_Z) != 0)) == 0);
	break;
      case 0xc:
	result = ((psw & PSW_S) == 0);
	break;
      case 0xd:
	result = ((psw & PSW_SAT) != 0);
	break;
      case 0xe:
	result = ((((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0)) == 0);
	break;
      case 0xf:
	result = (((((psw & PSW_S) != 0) ^ ((psw & PSW_OV) != 0))
		  || ((psw & PSW_Z) != 0)) == 0);
	break;
    }
  
  State.regs[OP[1]] = result;
  trace_output (OP_EX1);
}

/* satadd reg,reg */
void
OP_C0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov, sat;

  trace_input ("satadd", OP_REG_REG, 0);
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
  sat = ov;

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0)
		| (sat ? PSW_SAT : 0));

  /* Handle saturated results.  */
  if (sat && s)
    State.regs[OP[1]] = 0x80000000;
  else if (sat)
    State.regs[OP[1]] = 0x7fffffff;
  trace_output (OP_REG_REG);
}

/* satadd sign_extend(imm5), reg */
void
OP_220 ()
{
  unsigned int op0, op1, result, z, s, cy, ov, sat;

  int temp;

  trace_input ("satadd", OP_IMM_REG, 0);

  /* Compute the result.  */
  temp = SEXT5 (OP[0]);
  op0 = temp;
  op1 = State.regs[OP[1]];
  result = op0 + op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < op0 || result < op1);
  ov = ((op0 & 0x80000000) == (op1 & 0x80000000)
	&& (op0 & 0x80000000) != (result & 0x80000000));
  sat = ov;

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0)
		| (sat ? PSW_SAT : 0));

  /* Handle saturated results.  */
  if (sat && s)
    State.regs[OP[1]] = 0x80000000;
  else if (sat)
    State.regs[OP[1]] = 0x7fffffff;
  trace_output (OP_IMM_REG);
}

/* satsub reg1, reg2 */
void
OP_A0 ()
{
  unsigned int op0, op1, result, z, s, cy, ov, sat;

  trace_input ("satsub", OP_REG_REG, 0);

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op1 - op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 < op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));
  sat = ov;

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0)
		| (sat ? PSW_SAT : 0));

  /* Handle saturated results.  */
  if (sat && s)
    State.regs[OP[1]] = 0x80000000;
  else if (sat)
    State.regs[OP[1]] = 0x7fffffff;
  trace_output (OP_REG_REG);
}

/* satsubi sign_extend(imm16), reg */
void
OP_660 ()
{
  unsigned int op0, op1, result, z, s, cy, ov, sat;
  int temp;

  trace_input ("satsubi", OP_IMM_REG, 0);

  /* Compute the result.  */
  temp = SEXT16 (OP[0]);
  op0 = temp;
  op1 = State.regs[OP[1]];
  result = op1 - op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 < op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));
  sat = ov;

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0)
		| (sat ? PSW_SAT : 0));

  /* Handle saturated results.  */
  if (sat && s)
    State.regs[OP[1]] = 0x80000000;
  else if (sat)
    State.regs[OP[1]] = 0x7fffffff;
  trace_output (OP_IMM_REG);
}

/* satsubr reg,reg */
void
OP_80 ()
{
  unsigned int op0, op1, result, z, s, cy, ov, sat;

  trace_input ("satsubr", OP_REG_REG, 0);

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 - op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (result < op0);
  ov = ((op1 & 0x80000000) != (op0 & 0x80000000)
	&& (op1 & 0x80000000) != (result & 0x80000000));
  sat = ov;

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_CY | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0) | (ov ? PSW_OV : 0)
		| (sat ? PSW_SAT : 0));

  /* Handle saturated results.  */
  if (sat && s)
    State.regs[OP[1]] = 0x80000000;
  else if (sat)
    State.regs[OP[1]] = 0x7fffffff;
  trace_output (OP_REG_REG);
}

/* tst reg,reg */
void
OP_160 ()
{
  unsigned int op0, op1, result, z, s;

  trace_input ("tst", OP_REG_REG_CMP, 0);

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 & op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the condition codes.  */
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_REG_REG_CMP);
}

/* mov reg, reg */
void
OP_0 ()
{
  trace_input ("mov", OP_REG_REG_MOVE, 0);
  State.regs[OP[1]] = State.regs[OP[0]];
  trace_output (OP_REG_REG_MOVE);
}

/* mov sign_extend(imm5), reg */
void
OP_200 ()
{
  int value = SEXT5 (OP[0]);
 
  trace_input ("mov", OP_IMM_REG_MOVE, 0);
  State.regs[OP[1]] = value;
  trace_output (OP_IMM_REG_MOVE);
}

/* movea sign_extend(imm16), reg, reg  */

void
OP_620 ()
{
  int value = SEXT16 (OP[0]);

  trace_input ("movea", OP_IMM_REG_REG, 0);
  State.regs[OP[2]] = State.regs[OP[1]] + value;
  trace_output (OP_IMM_REG_REG);
}

/* movhi imm16, reg, reg */
void
OP_640 ()
{
  uint32 value = (OP[0] & 0xffff) << 16;

  trace_input ("movhi", OP_UIMM_REG_REG, 16);
  State.regs[OP[2]] = State.regs[OP[1]] + value;
  trace_output (OP_UIMM_REG_REG);
}

/* sar zero_extend(imm5),reg1 */
void
OP_2A0 ()
{
  unsigned int op0, op1, result, z, s, cy;

  trace_input ("sar", OP_IMM_REG, 0);
  op0 = OP[0] & 0x1f;
  op1 = State.regs[OP[1]];
  result = (signed)op1 >> op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 & (1 << (op0 - 1)));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV | PSW_CY);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0));
  trace_output (OP_IMM_REG);
}

/* sar reg1, reg2 */
void
OP_A007E0 ()
{
  unsigned int op0, op1, result, z, s, cy;

  trace_input ("sar", OP_REG_REG, 0);
  op0 = State.regs[OP[0]] & 0x1f;
  op1 = State.regs[OP[1]];
  result = (signed)op1 >> op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 & (1 << (op0 - 1)));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV | PSW_CY);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0));
  trace_output (OP_REG_REG);
}

/* shl zero_extend(imm5),reg1 */
void
OP_2C0 ()
{
  unsigned int op0, op1, result, z, s, cy;

  trace_input ("shl", OP_IMM_REG, 0);
  op0 = OP[0] & 0x1f;
  op1 = State.regs[OP[1]];
  result = op1 << op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 & (1 << (32 - op0)));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV | PSW_CY);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0));
  trace_output (OP_IMM_REG);
}

/* shl reg1, reg2 */
void
OP_C007E0 ()
{
  unsigned int op0, op1, result, z, s, cy;

  trace_input ("shl", OP_REG_REG, 0);
  op0 = State.regs[OP[0]] & 0x1f;
  op1 = State.regs[OP[1]];
  result = op1 << op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 & (1 << (32 - op0)));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV | PSW_CY);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0));
  trace_output (OP_REG_REG);
}

/* shr zero_extend(imm5),reg1 */
void
OP_280 ()
{
  unsigned int op0, op1, result, z, s, cy;

  trace_input ("shr", OP_IMM_REG, 0);
  op0 = OP[0] & 0x1f;
  op1 = State.regs[OP[1]];
  result = op1 >> op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 & (1 << (op0 - 1)));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV | PSW_CY);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0));
  trace_output (OP_IMM_REG);
}

/* shr reg1, reg2 */
void
OP_8007E0 ()
{
  unsigned int op0, op1, result, z, s, cy;

  trace_input ("shr", OP_REG_REG, 0);
  op0 = State.regs[OP[0]] & 0x1f;
  op1 = State.regs[OP[1]];
  result = op1 >> op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);
  cy = (op1 & (1 << (op0 - 1)));

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV | PSW_CY);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0)
		| (cy ? PSW_CY : 0));
  trace_output (OP_REG_REG);
}

/* or reg, reg */
void
OP_100 ()
{
  unsigned int op0, op1, result, z, s;

  trace_input ("or", OP_REG_REG, 0);

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 | op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_REG_REG);
}

/* ori zero_extend(imm16), reg, reg */
void
OP_680 ()
{
  unsigned int op0, op1, result, z, s;

  trace_input ("ori", OP_UIMM_REG_REG, 0);
  op0 = OP[0] & 0xffff;
  op1 = State.regs[OP[1]];
  result = op0 | op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_UIMM_REG_REG);
}

/* and reg, reg */
void
OP_140 ()
{
  unsigned int op0, op1, result, z, s;

  trace_input ("and", OP_REG_REG, 0);

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 & op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_REG_REG);
}

/* andi zero_extend(imm16), reg, reg */
void
OP_6C0 ()
{
  unsigned int op0, op1, result, z;

  trace_input ("andi", OP_UIMM_REG_REG, 0);
  op0 = OP[0] & 0xffff;
  op1 = State.regs[OP[1]];
  result = op0 & op1;

  /* Compute the condition codes.  */
  z = (result == 0);

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= (z ? PSW_Z : 0);
  trace_output (OP_UIMM_REG_REG);
}

/* xor reg, reg */
void
OP_120 ()
{
  unsigned int op0, op1, result, z, s;

  trace_input ("xor", OP_REG_REG, 0);

  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  op1 = State.regs[OP[1]];
  result = op0 ^ op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_REG_REG);
}

/* xori zero_extend(imm16), reg, reg */
void
OP_6A0 ()
{
  unsigned int op0, op1, result, z, s;

  trace_input ("xori", OP_UIMM_REG_REG, 0);
  op0 = OP[0] & 0xffff;
  op1 = State.regs[OP[1]];
  result = op0 ^ op1;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[2]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_UIMM_REG_REG);
}

/* not reg1, reg2 */
void
OP_20 ()
{
  unsigned int op0, result, z, s;

  trace_input ("not", OP_REG_REG_MOVE, 0);
  /* Compute the result.  */
  op0 = State.regs[OP[0]];
  result = ~op0;

  /* Compute the condition codes.  */
  z = (result == 0);
  s = (result & 0x80000000);

  /* Store the result and condition codes.  */
  State.regs[OP[1]] = result;
  State.sregs[5] &= ~(PSW_Z | PSW_S | PSW_OV);
  State.sregs[5] |= ((z ? PSW_Z : 0) | (s ? PSW_S : 0));
  trace_output (OP_REG_REG_MOVE);
}

/* set1 */
void
OP_7C0 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("set1", OP_BIT, 0);
  op0 = State.regs[OP[0]];
  op1 = OP[1] & 0x7;
  temp = SEXT16 (OP[2]);
  op2 = temp;
  temp = get_byte (State.mem + op0 + op2);
  State.sregs[5] &= ~PSW_Z;
  if ((temp & (1 << op1)) == 0)
    State.sregs[5] |= PSW_Z;
  temp |= (1 << op1);
  put_byte (State.mem + op0 + op2, temp);
  trace_output (OP_BIT);
}

/* not1 */
void
OP_47C0 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("not1", OP_BIT, 0);
  op0 = State.regs[OP[0]];
  op1 = OP[1] & 0x7;
  temp = SEXT16 (OP[2]);
  op2 = temp;
  temp = get_byte (State.mem + op0 + op2);
  State.sregs[5] &= ~PSW_Z;
  if ((temp & (1 << op1)) == 0)
    State.sregs[5] |= PSW_Z;
  temp ^= (1 << op1);
  put_byte (State.mem + op0 + op2, temp);
  trace_output (OP_BIT);
}

/* clr1 */
void
OP_87C0 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("clr1", OP_BIT, 0);
  op0 = State.regs[OP[0]];
  op1 = OP[1] & 0x7;
  temp = SEXT16 (OP[2]);
  op2 = temp;
  temp = get_byte (State.mem + op0 + op2);
  State.sregs[5] &= ~PSW_Z;
  if ((temp & (1 << op1)) == 0)
    State.sregs[5] |= PSW_Z;
  temp &= ~(1 << op1);
  put_byte (State.mem + op0 + op2, temp);
  trace_output (OP_BIT);
}

/* tst1 */
void
OP_C7C0 ()
{
  unsigned int op0, op1, op2;
  int temp;

  trace_input ("tst1", OP_BIT, 0);
  op0 = State.regs[OP[0]];
  op1 = OP[1] & 0x7;
  temp = SEXT16 (OP[2]);
  op2 = temp;
  temp = get_byte (State.mem + op0 + op2);
  State.sregs[5] &= ~PSW_Z;
  if ((temp & (1 << op1)) == 0)
    State.sregs[5] |= PSW_Z;
  trace_output (OP_BIT);
}

/* di */
void
OP_16007E0 ()
{
  trace_input ("di", OP_NONE, 0);
  State.sregs[5] |= PSW_ID;
  trace_output (OP_NONE);
}

/* ei */
void
OP_16087E0 ()
{
  trace_input ("ei", OP_NONE, 0);
  State.sregs[5] &= ~PSW_ID;
  trace_output (OP_NONE);
}

/* halt, not supported */
void
OP_12007E0 ()
{
  trace_input ("halt", OP_NONE, 0);
  State.exception = SIGQUIT;
  trace_output (OP_NONE);
}

/* reti, not supported */
void
OP_14007E0 ()
{
  trace_input ("reti", OP_NONE, 0);
  trace_output (OP_NONE);
  abort ();
}

/* trap, not supportd */
void
OP_10007E0 ()
{
  extern int errno;

  trace_input ("trap", OP_TRAP, 0);
  trace_output (OP_TRAP);

  /* Trap 0 is used for simulating low-level I/O */

  if (OP[0] == 0)
    {
      int save_errno = errno;	
      errno = 0;

/* Registers passed to trap 0 */

#define FUNC   State.regs[6]	/* function number, return value */
#define PARM1  State.regs[7]	/* optional parm 1 */
#define PARM2  State.regs[8]	/* optional parm 2 */
#define PARM3  State.regs[9]	/* optional parm 3 */

/* Registers set by trap 0 */

#define RETVAL State.regs[10]	/* return value */
#define RETERR State.regs[11]	/* return error code */

/* Turn a pointer in a register into a pointer into real memory. */

#define MEMPTR(x) ((char *)((x) + State.mem))


      switch (FUNC)
	{
#if !defined(__GO32__) && !defined(_WIN32)
	case SYS_fork:
	  RETVAL = fork ();
	  break;
	case SYS_execve:
	  RETVAL = execve (MEMPTR (PARM1), (char **) MEMPTR (PARM2),
			   (char **)MEMPTR (PARM3));
	  break;
	case SYS_execv:
	  RETVAL = execve (MEMPTR (PARM1), (char **) MEMPTR (PARM2), NULL);
	  break;
#if 0
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

	case SYS_wait:
	  {
	    int status;

	    RETVAL = wait (&status);
	    SW (PARM1, status);
	  }
	  break;
#endif
#endif

	case SYS_read:
	  RETVAL = v850_callback->read (v850_callback, PARM1, MEMPTR (PARM2),
					PARM3);
	  break;
	case SYS_write:
	  if (PARM1 == 1)
	    RETVAL = (int)v850_callback->write_stdout (v850_callback,
				 		       MEMPTR (PARM2), PARM3);
	  else
	    RETVAL = (int)v850_callback->write (v850_callback, PARM1,
						MEMPTR (PARM2), PARM3);
	  break;
	case SYS_lseek:
	  RETVAL = v850_callback->lseek (v850_callback, PARM1, PARM2, PARM3);
	  break;
	case SYS_close:
	  RETVAL = v850_callback->close (v850_callback, PARM1);
	  break;
	case SYS_open:
	  RETVAL = v850_callback->open (v850_callback, MEMPTR (PARM1), PARM2);
	  break;
	case SYS_exit:
	  /* EXIT - caller can look in PARM1 to work out the 
	     reason */
	  if (PARM1 == 0xdead || PARM1 == 0x1)
	    State.exception = SIGABRT;
	  else
	    State.exception = SIGQUIT;
	  break;

#if 0
	case SYS_stat:	/* added at hmsi */
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
#endif
	  break;

	case SYS_chown:
	  RETVAL = chown (MEMPTR (PARM1), PARM2, PARM3);
	  break;
	case SYS_chmod:
	  RETVAL = chmod (MEMPTR (PARM1), PARM2);
	  break;
	case SYS_utime:
	  /* Cast the second argument to void *, to avoid type mismatch
	     if a prototype is present.  */
	  RETVAL = utime (MEMPTR (PARM1), (void *) MEMPTR (PARM2));
	  break;
	default:
	  abort ();
	}
      RETERR = errno;
      errno = save_errno;
    }
  else if (OP[0] == 1 )
    {
      char *fstr = State.regs[2] + State.mem;
      puts (fstr);
    }
}

/* ldsr, reg,reg */
void
OP_2007E0 ()
{
  unsigned int op0;

  trace_input ("ldsr", OP_LDSR, 0);
  op0 = State.regs[OP[0]];
  State.sregs[OP[1]] = op0;
  trace_output (OP_LDSR);
}

/* stsr, not supported */
void
OP_4007E0 ()
{
  unsigned int op0;

  trace_input ("stsr", OP_STSR, 0);
  op0 = State.sregs[OP[1]];
  State.regs[OP[0]] = op0;
  trace_output (OP_STSR);
}
