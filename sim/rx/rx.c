/* rx.c --- opcode semantics for stand-alone RX simulator.

Copyright (C) 2008, 2009 Free Software Foundation, Inc.
Contributed by Red Hat, Inc.

This file is part of the GNU simulators.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "opcode/rx.h"
#include "cpu.h"
#include "mem.h"
#include "syscalls.h"
#include "fpu.h"
#include "err.h"

#define tprintf if (trace) printf

jmp_buf decode_jmp_buf;
unsigned int rx_cycles = 0;

static int size2bytes[] = {
  4, 1, 1, 1, 2, 2, 2, 3, 4
};

typedef struct {
  unsigned long dpc;
} RX_Data;

#define rx_abort() _rx_abort(__FILE__, __LINE__)
static void
_rx_abort (const char *file, int line)
{
  if (strrchr (file, '/'))
    file = strrchr (file, '/') + 1;
  fprintf(stderr, "abort at %s:%d\n", file, line);
  abort();
}

static int
rx_get_byte (void *vdata)
{
  int saved_trace = trace;
  unsigned char rv;

  if (trace == 1)
    trace = 0;

  RX_Data *rx_data = (RX_Data *)vdata;
  if (rx_big_endian)
    /* See load.c for an explanation of this.  */
    rv = mem_get_pc (rx_data->dpc ^ 3);
  else
    rv = mem_get_pc (rx_data->dpc);
  rx_data->dpc ++;
  trace = saved_trace;
  return rv;
}

static int
get_op (RX_Opcode_Decoded *rd, int i)
{
  RX_Opcode_Operand *o = rd->op + i;
  int addr, rv = 0;

  switch (o->type)
    {
    case RX_Operand_None:
      rx_abort ();

    case RX_Operand_Immediate:	/* #addend */
      return o->addend;

    case RX_Operand_Register:	/* Rn */
      rv = get_reg (o->reg);
      break;

    case RX_Operand_Predec:	/* [-Rn] */
      put_reg (o->reg, get_reg (o->reg) - size2bytes[o->size]);
      /* fall through */
    case RX_Operand_Postinc:	/* [Rn+] */
    case RX_Operand_Indirect:	/* [Rn + addend] */

      addr = get_reg (o->reg) + o->addend;
      switch (o->size)
	{
	case RX_AnySize:
	  rx_abort ();

	case RX_Byte: /* undefined extension */
	case RX_UByte:
	case RX_SByte:
	  rv = mem_get_qi (addr);
	  break;

	case RX_Word: /* undefined extension */
	case RX_UWord:
	case RX_SWord:
	  rv = mem_get_hi (addr);
	  break;

	case RX_3Byte:
	  rv = mem_get_psi (addr);
	  break;

	case RX_Long:
	  rv = mem_get_si (addr);
	  break;
	}

      if (o->type == RX_Operand_Postinc)
	put_reg (o->reg, get_reg (o->reg) + size2bytes[o->size]);

      break;

    case RX_Operand_Condition:	/* eq, gtu, etc */
      return condition_true (o->reg);

    case RX_Operand_Flag:	/* [UIOSZC] */
      return (regs.r_psw & (1 << o->reg)) ? 1 : 0;
    }

  /* if we've gotten here, we need to clip/extend the value according
     to the size.  */
  switch (o->size)
    {
    case RX_AnySize:
      rx_abort ();

    case RX_Byte: /* undefined extension */
      rv |= 0xdeadbe00; /* keep them honest */
      break;

    case RX_UByte:
      rv &= 0xff;
      break;

    case RX_SByte:
      rv = sign_ext (rv, 8);
      break;

    case RX_Word: /* undefined extension */
      rv |= 0xdead0000; /* keep them honest */
      break;

    case RX_UWord:
      rv &=  0xffff;
      break;

    case RX_SWord:
      rv = sign_ext (rv, 16);
      break;

    case RX_3Byte:
      rv &= 0xffffff;
      break;

    case RX_Long:
      break;
    }
  return rv;
}

static void
put_op (RX_Opcode_Decoded *rd, int i, int v)
{
  RX_Opcode_Operand *o = rd->op + i;
  int addr;

  switch (o->size)
    {
    case RX_AnySize:
      if (o->type != RX_Operand_Register)
	rx_abort ();
      break;

    case RX_Byte: /* undefined extension */
      v |= 0xdeadbe00; /* keep them honest */
      break;

    case RX_UByte:
      v &= 0xff;
      break;

    case RX_SByte:
      v = sign_ext (v, 8);
      break;

    case RX_Word: /* undefined extension */
      v |= 0xdead0000; /* keep them honest */
      break;

    case RX_UWord:
      v &=  0xffff;
      break;

    case RX_SWord:
      v = sign_ext (v, 16);
      break;

    case RX_3Byte:
      v &= 0xffffff;
      break;

    case RX_Long:
      break;
    }

  switch (o->type)
    {
    case RX_Operand_None:
      /* Opcodes like TST and CMP use this.  */
      break;

    case RX_Operand_Immediate:	/* #addend */
    case RX_Operand_Condition:	/* eq, gtu, etc */
      rx_abort ();

    case RX_Operand_Register:	/* Rn */
      put_reg (o->reg, v);
      break;

    case RX_Operand_Predec:	/* [-Rn] */
      put_reg (o->reg, get_reg (o->reg) - size2bytes[o->size]);
      /* fall through */
    case RX_Operand_Postinc:	/* [Rn+] */
    case RX_Operand_Indirect:	/* [Rn + addend] */

      addr = get_reg (o->reg) + o->addend;
      switch (o->size)
	{
	case RX_AnySize:
	  rx_abort ();

	case RX_Byte: /* undefined extension */
	case RX_UByte:
	case RX_SByte:
	  mem_put_qi (addr, v);
	  break;

	case RX_Word: /* undefined extension */
	case RX_UWord:
	case RX_SWord:
	  mem_put_hi (addr, v);
	  break;

	case RX_3Byte:
	  mem_put_psi (addr, v);
	  break;

	case RX_Long:
	  mem_put_si (addr, v);
	  break;
	}

      if (o->type == RX_Operand_Postinc)
	put_reg (o->reg, get_reg (o->reg) + size2bytes[o->size]);

      break;

    case RX_Operand_Flag:	/* [UIOSZC] */
      if (v)
	regs.r_psw |= (1 << o->reg);
      else
	regs.r_psw &= ~(1 << o->reg);
      break;
    }
}

#define PD(x) put_op (&opcode, 0, x)
#define PS(x) put_op (&opcode, 1, x)
#define PS2(x) put_op (&opcode, 2, x)
#define GD() get_op (&opcode, 0)
#define GS() get_op (&opcode, 1)
#define GS2() get_op (&opcode, 2)
#define DSZ() size2bytes[opcode.op[0].size]
#define SSZ() size2bytes[opcode.op[0].size]
#define S2SZ() size2bytes[opcode.op[0].size]

/* "Universal" sources.  */
#define US1() ((opcode.op[2].type == RX_Operand_None) ? GD() : GS())
#define US2() ((opcode.op[2].type == RX_Operand_None) ? GS() : GS2())

static void
push(int val)
{
  int rsp = get_reg (sp);
  rsp -= 4;
  put_reg (sp, rsp);
  mem_put_si (rsp, val);
}

/* Just like the above, but tag the memory as "pushed pc" so if anyone
   tries to write to it, it will cause an error.  */
static void
pushpc(int val)
{
  int rsp = get_reg (sp);
  rsp -= 4;
  put_reg (sp, rsp);
  mem_put_si (rsp, val);
  mem_set_content_range (rsp, rsp+3, MC_PUSHED_PC);
}

static int
pop()
{
  int rv;
  int rsp = get_reg (sp);
  rv = mem_get_si (rsp);
  rsp += 4;
  put_reg (sp, rsp);
  return rv;
}

static int
poppc()
{
  int rv;
  int rsp = get_reg (sp);
  if (mem_get_content_type (rsp) != MC_PUSHED_PC)
    execution_error (SIM_ERR_CORRUPT_STACK, rsp);
  rv = mem_get_si (rsp);
  mem_set_content_range (rsp, rsp+3, MC_UNINIT);
  rsp += 4;
  put_reg (sp, rsp);
  return rv;
}

#define MATH_OP(vop,c)				\
{ \
  uma = US1(); \
  umb = US2(); \
  ll = (unsigned long long) uma vop (unsigned long long) umb vop c; \
  tprintf ("0x%x " #vop " 0x%x " #vop " 0x%x = 0x%llx\n", uma, umb, c, ll); \
  ma = sign_ext (uma, DSZ() * 8);					\
  mb = sign_ext (umb, DSZ() * 8);					\
  sll = (long long) ma vop (long long) mb vop c; \
  tprintf ("%d " #vop " %d " #vop " %d = %lld\n", ma, mb, c, sll); \
  set_oszc (sll, DSZ(), (long long) ll > ((1 vop 1) ? (long long) b2mask[DSZ()] : (long long) -1)); \
  PD (sll); \
}

#define LOGIC_OP(vop) \
{ \
  ma = US1(); \
  mb = US2(); \
  v = ma vop mb; \
  tprintf("0x%x " #vop " 0x%x = 0x%x\n", ma, mb, v); \
  set_sz (v, DSZ()); \
  PD(v); \
}

#define SHIFT_OP(val, type, count, OP, carry_mask)	\
{ \
  int i, c=0; \
  val = (type)US1();				\
  count = US2(); \
  tprintf("%lld " #OP " %d\n", val, count); \
  for (i = 0; i < count; i ++) \
    { \
      c = val & carry_mask; \
      val OP 1; \
    } \
  if (count) \
    set_oszc (val, 4, c); \
  PD (val); \
}

typedef union {
  int i;
  float f;
} FloatInt;

static inline int
float2int (float f)
{
  FloatInt fi;
  fi.f = f;
  return fi.i;
}

static inline float
int2float (int i)
{
  FloatInt fi;
  fi.i = i;
  return fi.f;
}

static int
fop_fadd (fp_t s1, fp_t s2, fp_t *d)
{
  *d = rxfp_add (s1, s2);
  return 1;
}

static int
fop_fmul (fp_t s1, fp_t s2, fp_t *d)
{
  *d = rxfp_mul (s1, s2);
  return 1;
}

static int
fop_fdiv (fp_t s1, fp_t s2, fp_t *d)
{
  *d = rxfp_div (s1, s2);
  return 1;
}

static int
fop_fsub (fp_t s1, fp_t s2, fp_t *d)
{
  *d = rxfp_sub (s1, s2);
  return 1;
}

#define FPPENDING() (regs.r_fpsw & (FPSWBITS_CE | (FPSWBITS_FMASK & (regs.r_fpsw << FPSW_EFSH))))
#define FPCLEAR() regs.r_fpsw &= FPSWBITS_CLEAR
#define FPCHECK() \
  if (FPPENDING()) \
    return do_fp_exception (opcode_pc)

#define FLOAT_OP(func) \
{ \
  int do_store;   \
  fp_t fa, fb, fc; \
  FPCLEAR(); \
  fa = GD (); \
  fb = GS (); \
  do_store = fop_##func (fa, fb, &fc); \
  tprintf("%g " #func " %g = %g %08x\n", int2float(fa), int2float(fb), int2float(fc), fc); \
  FPCHECK(); \
  if (do_store) \
    PD (fc);	\
  mb = 0; \
  if ((fc & 0x80000000UL) != 0) \
    mb |= FLAGBIT_S; \
  if ((fc & 0x7fffffffUL) == 0)			\
    mb |= FLAGBIT_Z; \
  set_flags (FLAGBIT_S | FLAGBIT_Z, mb); \
}

#define carry (FLAG_C ? 1 : 0)

static struct {
  unsigned long vaddr;
  const char *str;
  int signal;
} exception_info[] = {
  { 0xFFFFFFD0UL, "priviledged opcode", SIGILL },
  { 0xFFFFFFD4UL, "access violation", SIGSEGV },
  { 0xFFFFFFDCUL, "undefined opcode", SIGILL },
  { 0xFFFFFFE4UL, "floating point", SIGFPE }
};
#define EX_PRIVILEDGED	0
#define EX_ACCESS	1
#define EX_UNDEFINED	2
#define EX_FLOATING	3
#define EXCEPTION(n)  \
  return generate_exception (n, opcode_pc)

#define PRIVILEDGED() \
  if (FLAG_PM) \
    EXCEPTION (EX_PRIVILEDGED)

static int
generate_exception (unsigned long type, SI opcode_pc)
{
  SI old_psw, old_pc, new_pc;

  new_pc = mem_get_si (exception_info[type].vaddr);
  /* 0x00020000 is the value used to initialise the known
     exception vectors (see rx.ld), but it is a reserved
     area of memory so do not try to access it, and if the
     value has not been changed by the program then the
     vector has not been installed.  */
  if (new_pc == 0 || new_pc == 0x00020000)
    {
      if (rx_in_gdb)
	return RX_MAKE_STOPPED (exception_info[type].signal);

      fprintf(stderr, "Unhandled %s exception at pc = %#lx\n",
	      exception_info[type].str, (unsigned long) opcode_pc);
      if (type == EX_FLOATING)
	{
	  int mask = FPPENDING ();
	  fprintf (stderr, "Pending FP exceptions:");
	  if (mask & FPSWBITS_FV)
	    fprintf(stderr, " Invalid");
	  if (mask & FPSWBITS_FO)
	    fprintf(stderr, " Overflow");
	  if (mask & FPSWBITS_FZ)
	    fprintf(stderr, " Division-by-zero");
	  if (mask & FPSWBITS_FU)
	    fprintf(stderr, " Underflow");
	  if (mask & FPSWBITS_FX)
	    fprintf(stderr, " Inexact");
	  if (mask & FPSWBITS_CE)
	    fprintf(stderr, " Unimplemented");
	  fprintf(stderr, "\n");
	}
      return RX_MAKE_EXITED (1);
    }

  tprintf ("Triggering %s exception\n", exception_info[type].str);

  old_psw = regs.r_psw;
  regs.r_psw &= ~ (FLAGBIT_I | FLAGBIT_U | FLAGBIT_PM);
  old_pc = opcode_pc;
  regs.r_pc = new_pc;
  pushpc (old_psw);
  pushpc (old_pc);
  return RX_MAKE_STEPPED ();
}

void
generate_access_exception (void)
{
  int rv;

  rv = generate_exception (EX_ACCESS, regs.r_pc);
  if (RX_EXITED (rv))
    longjmp (decode_jmp_buf, rv);
}

static int
do_fp_exception (unsigned long opcode_pc)
{
  while (FPPENDING())
    EXCEPTION (EX_FLOATING);
  return RX_MAKE_STEPPED ();
}

int
decode_opcode ()
{
  unsigned int uma=0, umb=0;
  int ma=0, mb=0;
  int opcode_size, v;
  unsigned long long ll;
  long long sll;
  unsigned long opcode_pc;
  RX_Data rx_data;
  RX_Opcode_Decoded opcode;
  int rv;

  if ((rv = setjmp (decode_jmp_buf)))
    return rv;

  rx_cycles ++;

  rx_data.dpc = opcode_pc = regs.r_pc;
  opcode_size = rx_decode_opcode (opcode_pc, &opcode, rx_get_byte, &rx_data);
  regs.r_pc += opcode_size;

  rx_flagmask = opcode.flags_s;
  rx_flagand = ~(int)opcode.flags_0;
  rx_flagor = opcode.flags_1;

  switch (opcode.id)
    {
    case RXO_abs:
      sll = GS ();
      tprintf("|%lld| = ", sll);
      if (sll < 0)
	sll = -sll;
      tprintf("%lld\n", sll);
      PD (sll);
      set_osz (sll, 4);
      break;

    case RXO_adc:
      MATH_OP (+,carry);
      break;

    case RXO_add:
      MATH_OP (+,0);
      break;

    case RXO_and:
      LOGIC_OP (&);
      break;

    case RXO_bclr:
      ma = GD ();
      mb = GS ();
      if (opcode.op[0].type == RX_Operand_Register)
	mb &= 0x1f;
      else
	mb &= 0x07;
      ma &= ~(1 << mb);
      PD (ma);
      break;

    case RXO_bmcc:
      ma = GD ();
      mb = GS ();
      if (opcode.op[0].type == RX_Operand_Register)
	mb &= 0x1f;
      else
	mb &= 0x07;
      if (GS2 ())
	ma |= (1 << mb);
      else
	ma &= ~(1 << mb);
      PD (ma);
      break;

    case RXO_bnot:
      ma = GD ();
      mb = GS ();
      if (opcode.op[0].type == RX_Operand_Register)
	mb &= 0x1f;
      else
	mb &= 0x07;
      ma ^= (1 << mb);
      PD (ma);
      break;

    case RXO_branch:
      if (GS())
	regs.r_pc = GD();
      break;

    case RXO_branchrel:
      if (GS())
	regs.r_pc += GD();
      break;

    case RXO_brk:
      {
	int old_psw = regs.r_psw;
	if (rx_in_gdb)
	  return RX_MAKE_HIT_BREAK ();
	if (regs.r_intb == 0)
	  {
	    tprintf("BREAK hit, no vector table.\n");
	    return RX_MAKE_EXITED(1);
	  }
	regs.r_psw &= ~(FLAGBIT_I | FLAGBIT_U | FLAGBIT_PM);
	pushpc (old_psw);
	pushpc (regs.r_pc);
	regs.r_pc = mem_get_si (regs.r_intb);
      }
      break;

    case RXO_bset:
      ma = GD ();
      mb = GS ();
      if (opcode.op[0].type == RX_Operand_Register)
	mb &= 0x1f;
      else
	mb &= 0x07;
      ma |= (1 << mb);
      PD (ma);
      break;

    case RXO_btst:
      ma = GS ();
      mb = GS2 ();
      if (opcode.op[0].type == RX_Operand_Register)
	mb &= 0x1f;
      else
	mb &= 0x07;
      umb = ma & (1 << mb);
      set_zc (! umb, umb);
      break;

    case RXO_clrpsw:
      v = 1 << opcode.op[0].reg;
      if (FLAG_PM
	  && (v == FLAGBIT_I
	      || v == FLAGBIT_U))
	break;
      regs.r_psw &= ~v;
      break;

    case RXO_div: /* d = d / s */
      ma = GS();
      mb = GD();
      tprintf("%d / %d = ", mb, ma);
      if (ma == 0 || (ma == -1 && (unsigned int) mb == 0x80000000))
	{
	  tprintf("#NAN\n");
	  set_flags (FLAGBIT_O, FLAGBIT_O);
	}
      else
	{
	  v = mb/ma;
	  tprintf("%d\n", v);
	  set_flags (FLAGBIT_O, 0);
	  PD (v);
	}
      break;

    case RXO_divu: /* d = d / s */
      uma = GS();
      umb = GD();
      tprintf("%u / %u = ", umb, uma);
      if (uma == 0)
	{
	  tprintf("#NAN\n");
	  set_flags (FLAGBIT_O, FLAGBIT_O);
	}
      else
	{
	  v = umb / uma;
	  tprintf("%u\n", v);
	  set_flags (FLAGBIT_O, 0);
	  PD (v);
	}
      break;

    case RXO_ediv:
      ma = GS();
      mb = GD();
      tprintf("%d / %d = ", mb, ma);
      if (ma == 0 || (ma == -1 && (unsigned int) mb == 0x80000000))
	{
	  tprintf("#NAN\n");
	  set_flags (FLAGBIT_O, FLAGBIT_O);
	}
      else
	{
	  v = mb/ma;
	  mb = mb%ma;
	  tprintf("%d, rem %d\n", v, mb);
	  set_flags (FLAGBIT_O, 0);
	  PD (v);
	  opcode.op[0].reg ++;
	  PD (mb);
	}
      break;

    case RXO_edivu:
      uma = GS();
      umb = GD();
      tprintf("%u / %u = ", umb, uma);
      if (uma == 0)
	{
	  tprintf("#NAN\n");
	  set_flags (FLAGBIT_O, FLAGBIT_O);
	}
      else
	{
	  v = umb/uma;
	  umb = umb%uma;
	  tprintf("%u, rem %u\n", v, umb);
	  set_flags (FLAGBIT_O, 0);
	  PD (v);
	  opcode.op[0].reg ++;
	  PD (umb);
	}
      break;

    case RXO_emul:
      ma = GD ();
      mb = GS ();
      sll = (long long)ma * (long long)mb;
      tprintf("%d * %d = %lld\n", ma, mb, sll);
      PD (sll);
      opcode.op[0].reg ++;
      PD (sll >> 32);
      break;

    case RXO_emulu:
      uma = GD ();
      umb = GS ();
      ll = (long long)uma * (long long)umb;
      tprintf("%#x * %#x = %#llx\n", uma, umb, ll);
      PD (ll);
      opcode.op[0].reg ++;
      PD (ll >> 32);
      break;

    case RXO_fadd:
      FLOAT_OP (fadd);
      break;

    case RXO_fcmp:
      ma = GD();
      mb = GS();
      FPCLEAR ();
      rxfp_cmp (ma, mb);
      FPCHECK ();
      break;

    case RXO_fdiv:
      FLOAT_OP (fdiv);
      break;

    case RXO_fmul:
      FLOAT_OP (fmul);
      break;

    case RXO_rtfi:
      PRIVILEDGED ();
      regs.r_psw = regs.r_bpsw;
      regs.r_pc = regs.r_bpc;
      break;

    case RXO_fsub:
      FLOAT_OP (fsub);
      break;

    case RXO_ftoi:
      ma = GS ();
      FPCLEAR ();
      mb = rxfp_ftoi (ma, FPRM_ZERO);
      FPCHECK ();
      PD (mb);
      tprintf("(int) %g = %d\n", int2float(ma), mb);
      set_sz (mb, 4);
      break;

    case RXO_int:
      v = GS ();
      if (v == 255)
	{
	  return rx_syscall (regs.r[5]);
	}
      else
	{
	  int old_psw = regs.r_psw;
	  regs.r_psw &= ~(FLAGBIT_I | FLAGBIT_U | FLAGBIT_PM);
	  pushpc (old_psw);
	  pushpc (regs.r_pc);
	  regs.r_pc = mem_get_si (regs.r_intb + 4 * v);
	}
      break;

    case RXO_itof:
      ma = GS ();
      FPCLEAR ();
      mb = rxfp_itof (ma, regs.r_fpsw);
      FPCHECK ();
      tprintf("(float) %d = %x\n", ma, mb);
      PD (mb);
      set_sz (ma, 4);
      break;

    case RXO_jsr:
    case RXO_jsrrel:
      v = GD ();
      pushpc (get_reg (pc));
      if (opcode.id == RXO_jsrrel)
	v += regs.r_pc;
      put_reg (pc, v);
      break;

    case RXO_machi:
      ll = (long long)(signed short)(GS() >> 16) * (long long)(signed short)(GS2 () >> 16);
      ll <<= 16;
      put_reg64 (acc64, ll + regs.r_acc);
      break;

    case RXO_maclo:
      ll = (long long)(signed short)(GS()) * (long long)(signed short)(GS2 ());
      ll <<= 16;
      put_reg64 (acc64, ll + regs.r_acc);
      break;

    case RXO_max:
      ma = GD();
      mb = GS();
      if (ma > mb)
	PD (ma);
      else
	PD (mb);
      break;

    case RXO_min:
      ma = GD();
      mb = GS();
      if (ma < mb)
	PD (ma);
      else
	PD (mb);
      break;

    case RXO_mov:
      v = GS ();
      if (opcode.op[0].type == RX_Operand_Register
	  && opcode.op[0].reg == 16 /* PSW */)
	{
	  /* Special case, LDC and POPC can't ever modify PM.  */
	  int pm = regs.r_psw & FLAGBIT_PM;
	  v &= ~ FLAGBIT_PM;
	  v |= pm;
	  if (pm)
	    {
	      v &= ~ (FLAGBIT_I | FLAGBIT_U | FLAGBITS_IPL);
	      v |= pm;
	    }
	}
      if (FLAG_PM)
	{
	  /* various things can't be changed in user mode.  */
	  if (opcode.op[0].type == RX_Operand_Register)
	    if (opcode.op[0].reg == 32)
	      {
		v &= ~ (FLAGBIT_I | FLAGBIT_U | FLAGBITS_IPL);
		v |= regs.r_psw & (FLAGBIT_I | FLAGBIT_U | FLAGBITS_IPL);
	      }
	  if (opcode.op[0].reg == 34 /* ISP */
	      || opcode.op[0].reg == 37 /* BPSW */
	      || opcode.op[0].reg == 39 /* INTB */
	      || opcode.op[0].reg == 38 /* VCT */)
	    /* These are ignored.  */
	    break;
	}
      PD (v);
      set_sz (v, DSZ());
      break;

    case RXO_movbi:
      /* We cheat to save on code duplication. */
      regs.r_temp = (get_reg (opcode.op[1].reg) * size2bytes[opcode.size]
		     + get_reg (opcode.op[2].reg));
      opcode.op[1].reg = r_temp_idx;
      opcode.op[1].type = RX_Operand_Indirect;
      opcode.op[1].addend = 0;
      PD (GS ());
      break;

    case RXO_movbir:
      /* We cheat to save on code duplication. */
      regs.r_temp = (get_reg (opcode.op[1].reg) * size2bytes[opcode.size]
		     + get_reg (opcode.op[2].reg));
      opcode.op[1].reg = r_temp_idx;
      opcode.op[1].type = RX_Operand_Indirect;
      opcode.op[1].addend = 0;
      PS (GD ());
      break;

    case RXO_mul:
      ll = (unsigned long long) US1() * (unsigned long long) US2();
      PD(ll);
      break;

    case RXO_mulhi:
      ll = (long long)(signed short)(GS() >> 16) * (long long)(signed short)(GS2 () >> 16);
      ll <<= 16;
      put_reg64 (acc64, ll);
      break;

    case RXO_mullo:
      ll = (long long)(signed short)(GS()) * (long long)(signed short)(GS2 ());
      ll <<= 16;
      put_reg64 (acc64, ll);
      break;

    case RXO_mvfachi:
      PD (get_reg (acchi));
      break;

    case RXO_mvfaclo:
      PD (get_reg (acclo));
      break;

    case RXO_mvfacmi:
      PD (get_reg (accmi));
      break;

    case RXO_mvtachi:
      put_reg (acchi, GS ());
      break;

    case RXO_mvtaclo:
      put_reg (acclo, GS ());
      break;

    case RXO_mvtipl:
      regs.r_psw &= ~ FLAGBITS_IPL;
      regs.r_psw |= (GS () << FLAGSHIFT_IPL) & FLAGBITS_IPL;
      break;

    case RXO_nop:
      break;

    case RXO_or:
      LOGIC_OP (|);
      break;

    case RXO_popm:
      /* POPM cannot pop R0 (sp).  */
      if (opcode.op[1].reg == 0 || opcode.op[2].reg == 0)
	EXCEPTION (EX_UNDEFINED);
      if (opcode.op[1].reg >= opcode.op[2].reg)
	{
	  regs.r_pc = opcode_pc;
	  return RX_MAKE_STOPPED (SIGILL);
	}
      for (v = opcode.op[1].reg; v <= opcode.op[2].reg; v++)
	put_reg (v, pop ());
      break;

    case RXO_pusha:
      push (get_reg (opcode.op[1].reg) + opcode.op[1].addend);
      break;

    case RXO_pushm:
      /* PUSHM cannot push R0 (sp).  */
      if (opcode.op[1].reg == 0 || opcode.op[2].reg == 0)
	EXCEPTION (EX_UNDEFINED);
      if (opcode.op[1].reg >= opcode.op[2].reg)
	{
	  regs.r_pc = opcode_pc;
	  return RX_MAKE_STOPPED (SIGILL);
	}
      for (v = opcode.op[2].reg; v >= opcode.op[1].reg; v--)
	push (get_reg (v));
      break;

    case RXO_racw:
      ll = get_reg64 (acc64) << GS ();
      ll += 0x80000000ULL;
      if ((signed long long)ll > (signed long long)0x00007fff00000000ULL)
	ll = 0x00007fff00000000ULL;
      else if ((signed long long)ll < (signed long long)0xffff800000000000ULL)
	ll = 0xffff800000000000ULL;
      else
	ll &= 0xffffffff00000000ULL;
      put_reg64 (acc64, ll);
      break;

    case RXO_rte:
      PRIVILEDGED ();
      regs.r_pc = poppc ();
      regs.r_psw = poppc ();
      if (FLAG_PM)
	regs.r_psw |= FLAGBIT_U;
      break;

    case RXO_revl:
      uma = GS ();
      umb = (((uma >> 24) & 0xff)
	     | ((uma >> 8) & 0xff00)
	     | ((uma << 8) & 0xff0000)
	     | ((uma << 24) & 0xff000000UL));
      PD (umb);
      break;

    case RXO_revw:
      uma = GS ();
      umb = (((uma >> 8) & 0x00ff00ff)
	     | ((uma << 8) & 0xff00ff00UL));
      PD (umb);
      break;

    case RXO_rmpa:
      while (regs.r[3] != 0)
	{
	  long long tmp;

	  switch (opcode.size)
	    {
	    case RX_Long:
	      ma = mem_get_si (regs.r[1]);
	      mb = mem_get_si (regs.r[2]);
	      regs.r[1] += 4;
	      regs.r[2] += 4;
	      break;
	    case RX_Word:
	      ma = sign_ext (mem_get_hi (regs.r[1]), 16);
	      mb = sign_ext (mem_get_hi (regs.r[2]), 16);
	      regs.r[1] += 2;
	      regs.r[2] += 2;
	      break;
	    case RX_Byte:
	      ma = sign_ext (mem_get_qi (regs.r[1]), 8);
	      mb = sign_ext (mem_get_qi (regs.r[2]), 8);
	      regs.r[1] += 1;
	      regs.r[2] += 1;
	      break;
	    default:
	      abort ();
	    }
	  /* We do the multiply as a signed value.  */
	  sll = (long long)ma * (long long)mb;
	  tprintf("        %016llx = %d * %d\n", sll, ma, mb);
	  /* but we do the sum as unsigned, while sign extending the operands.  */
	  tmp = regs.r[4] + (sll & 0xffffffffUL);
	  regs.r[4] = tmp & 0xffffffffUL;
	  tmp >>= 32;
	  sll >>= 32;
	  tmp += regs.r[5] + (sll & 0xffffffffUL);
	  regs.r[5] = tmp & 0xffffffffUL;
	  tmp >>= 32;
	  sll >>= 32;
	  tmp += regs.r[6] + (sll & 0xffffffffUL);
	  regs.r[6] = tmp & 0xffffffffUL;
	  tprintf("%08lx\033[36m%08lx\033[0m%08lx\n",
		  (unsigned long) regs.r[6],
		  (unsigned long) regs.r[5],
		  (unsigned long) regs.r[4]);

	  regs.r[3] --;
	}
      if (regs.r[6] & 0x00008000)
	regs.r[6] |= 0xffff0000UL;
      else
	regs.r[6] &= 0x0000ffff;
      ma = (regs.r[6] & 0x80000000UL) ? FLAGBIT_S : 0;
      if (regs.r[6] != 0 && regs.r[6] != 0xffffffffUL)
	set_flags (FLAGBIT_O|FLAGBIT_S, ma | FLAGBIT_O);
      else
	set_flags (FLAGBIT_O|FLAGBIT_S, ma);
      break;

    case RXO_rolc:
      v = GD ();
      ma = v & 0x80000000UL;
      v <<= 1;
      v |= carry;
      set_szc (v, 4, ma);
      PD (v);
      break;

    case RXO_rorc:
      uma = GD ();
      mb = uma & 1;
      uma >>= 1;
      uma |= (carry ? 0x80000000UL : 0);
      set_szc (uma, 4, mb);
      PD (uma);
      break;

    case RXO_rotl:
      mb = GS ();
      uma = GD ();
      if (mb)
	{
	  uma = (uma << mb) | (uma >> (32-mb));
	  mb = uma & 1;
	}
      set_szc (uma, 4, mb);
      PD (uma);
      break;

    case RXO_rotr:
      mb = GS ();
      uma = GD ();
      if (mb)
	{
	  uma = (uma >> mb) | (uma << (32-mb));
	  mb = uma & 0x80000000;
	}
      set_szc (uma, 4, mb);
      PD (uma);
      break;

    case RXO_round:
      ma = GS ();
      FPCLEAR ();
      mb = rxfp_ftoi (ma, regs.r_fpsw);
      FPCHECK ();
      PD (mb);
      tprintf("(int) %g = %d\n", int2float(ma), mb);
      set_sz (mb, 4);
      break;

    case RXO_rts:
      regs.r_pc = poppc ();
      break;

    case RXO_rtsd:
      if (opcode.op[2].type == RX_Operand_Register)
	{
	  int i;
	  /* RTSD cannot pop R0 (sp).  */
	  put_reg (0, get_reg (0) + GS() - (opcode.op[0].reg-opcode.op[2].reg+1)*4);
	  if (opcode.op[2].reg == 0)
	    EXCEPTION (EX_UNDEFINED);
	  for (i = opcode.op[2].reg; i <= opcode.op[0].reg; i ++)
	    put_reg (i, pop ());
	}
      else
	put_reg (0, get_reg (0) + GS());
      put_reg (pc, poppc ());
      break;

    case RXO_sat:
      if (FLAG_O && FLAG_S)
	PD (0x7fffffffUL);
      else if (FLAG_O && ! FLAG_S)
	PD (0x80000000UL);
      break;

    case RXO_sbb:
      MATH_OP (-, ! carry);
      break;

    case RXO_sccnd:
      if (GS())
	PD (1);
      else
	PD (0);
      break;

    case RXO_scmpu:
      while (regs.r[3] != 0)
	{
	  uma = mem_get_qi (regs.r[1] ++);
	  umb = mem_get_qi (regs.r[2] ++);
	  regs.r[3] --;
	  if (uma != umb || uma == 0)
	    break;
	}
      if (uma == umb)
	set_zc (1, 1);
      else
	set_zc (0, ((int)uma - (int)umb) >= 0);
      break;

    case RXO_setpsw:
      v = 1 << opcode.op[0].reg;
      if (FLAG_PM
	  && (v == FLAGBIT_I
	      || v == FLAGBIT_U))
	break;
      regs.r_psw |= v;
      break;

    case RXO_smovb:
      while (regs.r[3])
	{
	  uma = mem_get_qi (regs.r[2] --);
	  mem_put_qi (regs.r[1]--, uma);
	  regs.r[3] --;
	}
      break;

    case RXO_smovf:
      while (regs.r[3])
	{
	  uma = mem_get_qi (regs.r[2] ++);
	  mem_put_qi (regs.r[1]++, uma);
	  regs.r[3] --;
	}
      break;

    case RXO_smovu:
      while (regs.r[3] != 0)
	{
	  uma = mem_get_qi (regs.r[2] ++);
	  mem_put_qi (regs.r[1]++, uma);
	  regs.r[3] --;
	  if (uma == 0)
	    break;
	}
      break;

    case RXO_shar: /* d = ma >> mb */
      SHIFT_OP (sll, int, mb, >>=, 1);
      break;

    case RXO_shll: /* d = ma << mb */
      SHIFT_OP (ll, int, mb, <<=, 0x80000000UL);
      break;

    case RXO_shlr: /* d = ma >> mb */
      SHIFT_OP (ll, unsigned int, mb, >>=, 1);
      break;

    case RXO_sstr:
      switch (opcode.size)
	{
	case RX_Long:
	  while (regs.r[3] != 0)
	    {
	      mem_put_si (regs.r[1], regs.r[2]);
	      regs.r[1] += 4;
	      regs.r[3] --;
	    }
	  break;
	case RX_Word:
	  while (regs.r[3] != 0)
	    {
	      mem_put_hi (regs.r[1], regs.r[2]);
	      regs.r[1] += 2;
	      regs.r[3] --;
	    }
	  break;
	case RX_Byte:
	  while (regs.r[3] != 0)
	    {
	      mem_put_qi (regs.r[1], regs.r[2]);
	      regs.r[1] ++;
	      regs.r[3] --;
	    }
	  break;
	default:
	  abort ();
	}
      break;

    case RXO_stcc:
      if (GS2())
	PD (GS ());
      break;

    case RXO_stop:
      PRIVILEDGED ();
      regs.r_psw |= FLAGBIT_I;
      return RX_MAKE_STOPPED(0);

    case RXO_sub:
      MATH_OP (-, 0);
      break;

    case RXO_suntil:
      if (regs.r[3] == 0)
	break;
      switch (opcode.size)
	{
	case RX_Long:
	  uma = get_reg (2);
	  while (regs.r[3] != 0)
	    {
	      regs.r[3] --;
	      umb = mem_get_si (get_reg (1));
	      regs.r[1] += 4;
	      if (umb == uma)
		break;
	    }
	  break;
	case RX_Word:
	  uma = get_reg (2) & 0xffff;
	  while (regs.r[3] != 0)
	    {
	      regs.r[3] --;
	      umb = mem_get_hi (get_reg (1));
	      regs.r[1] += 2;
	      if (umb == uma)
		break;
	    }
	  break;
	case RX_Byte:
	  uma = get_reg (2) & 0xff;
	  while (regs.r[3] != 0)
	    {
	      regs.r[3] --;
	      umb = mem_get_qi (regs.r[1]);
	      regs.r[1] += 1;
	      if (umb == uma)
		break;
	    }
	  break;
	default:
	  abort();
	}
      if (uma == umb)
	set_zc (1, 1);
      else
	set_zc (0, ((int)uma - (int)umb) >= 0);
      break;

    case RXO_swhile:
      if (regs.r[3] == 0)
	break;
      switch (opcode.size)
	{
	case RX_Long:
	  uma = get_reg (2);
	  while (regs.r[3] != 0)
	    {
	      regs.r[3] --;
	      umb = mem_get_si (get_reg (1));
	      regs.r[1] += 4;
	      if (umb != uma)
		break;
	    }
	  break;
	case RX_Word:
	  uma = get_reg (2) & 0xffff;
	  while (regs.r[3] != 0)
	    {
	      regs.r[3] --;
	      umb = mem_get_hi (get_reg (1));
	      regs.r[1] += 2;
	      if (umb != uma)
		break;
	    }
	  break;
	case RX_Byte:
	  uma = get_reg (2) & 0xff;
	  while (regs.r[3] != 0)
	    {
	      regs.r[3] --;
	      umb = mem_get_qi (regs.r[1]);
	      regs.r[1] += 1;
	      if (umb != uma)
		break;
	    }
	  break;
	default:
	  abort();
	}
      if (uma == umb)
	set_zc (1, 1);
      else
	set_zc (0, ((int)uma - (int)umb) >= 0);
      break;

    case RXO_wait:
      PRIVILEDGED ();
      regs.r_psw |= FLAGBIT_I;
      return RX_MAKE_STOPPED(0);

    case RXO_xchg:
      v = GS (); /* This is the memory operand, if any.  */
      PS (GD ()); /* and this may change the address register.  */
      PD (v);
      break;

    case RXO_xor:
      LOGIC_OP (^);
      break;

    default:
      EXCEPTION (EX_UNDEFINED);
    }

  return RX_MAKE_STEPPED ();
}
