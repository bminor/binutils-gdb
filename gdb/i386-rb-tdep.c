/* Intel 386 target-dependent stuff for the codetap target.

   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996,
   1997, 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdb_string.h"
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "objfiles.h"
#include "target.h"
#include "floatformat.h"
#include "symfile.h"
#include "symtab.h"
#include "gdbcmd.h"
#include "command.h"
#include "arch-utils.h"
#include "regcache.h"
#include "doublest.h"
#include "value.h"
#include "gdb_assert.h"
#include "reggroups.h"

#include "i386-tdep.h"
#include "i387-tdep.h"

#include <ctype.h>

/* System registers.  */

static struct type *builtin_type_i386_gdtr (void);
static struct type *builtin_type_i386_ldtr (void);
static struct type *builtin_type_i386_cr0 (void);
static struct type *builtin_type_i386_cr2 (void);
static struct type *builtin_type_i386_cr3 (void);
static struct type *builtin_type_i386_cr4 (void);
static struct type *builtin_type_i386_msr (void);

struct i386_sys_reg
{
  const char *name;
  struct type *(*type) (void);
};

struct i386_sys_reg i386_sys_regs[] = {
  { "cr0", builtin_type_i386_cr0 },
  { "cr2", builtin_type_i386_cr2 },
  { "cr3", builtin_type_i386_cr3 },
  { "cr4", builtin_type_i386_cr4 },
  { "gdtr", builtin_type_i386_gdtr },
  { "idtr", builtin_type_i386_gdtr },
  { "ldtr", builtin_type_i386_ldtr },
  { "tr", builtin_type_i386_ldtr },
  { "", builtin_type_i386_msr }
};
static const int num_sys_regs = (sizeof i386_sys_regs
				 / sizeof i386_sys_regs[0]);

static int
regnum_to_sysnum (int regnum)
{
  return regnum - I386_SSE_NUM_REGS;
}

static int
sys_regnum_p (int regnum)
{
  int sysnum = regnum_to_sysnum (regnum);
  return (sysnum >= 0 && sysnum < num_sys_regs);
}

static int
msr_regnum ()
{
  /* The last SYSREG is special, it is used to access Red Boot's MSR
     registers.  Find the regnum of the first SYSREG by reversing the
     effects of regnum_to_sysregnum.  */
  return (-regnum_to_sysnum (0)) + num_sys_regs - 1;
}

/* Red Boot's MSR registers.  They are accessed using a single real
   MSR register.  */

struct i386_msr_reg
{
  const char *name;
  long address;  /* MSRs have a cardinal pnum.  */
};

struct i386_msr_reg i386_msr_regs[] = {
  { "P5_MC_ADDR", 0 },	/* Machine-Check Error Address */
  { "P5_MC_TYPE", 1 },	/* Machine-Check Error Type */
  { "TSC", 16 },		/* Time Stamp Counter */
  { "BBL_CR_OVRD", 23 },	/* Platform type for microcode updates */
  { "APICBASE", 27 },	/* APIC Base Address */
  { "EBL_CR_POWERON", 42 },	/* Power On Configuration Register */
  { "TEST_CTL", 51 },	/* Test Control Register */
  { "BIOS_UPDT_TRIG", 121 }, /* BIOS Update Trigger Register */
  { "BBL_CR_D0", 136 },	/* L2 Chunk 0 data register D[63:0] */
  { "BBL_CR_D1", 137 },	/* L2 Chunk 1 data register D[63:0] */
  { "BBL_CR_D2", 138 },	/* L2 Chunk 2 data register D[63:0] */
  { "BBL_CR_D3", 139 },	/* L2 Chunk 3 data register D[63:0] */
  { "BIOS_SIGN", 139 },	/* BIOS Update Signature Register */
  { "PERFCT0", 193 },	/* Performance Counter 0 */
  { "PERFCT1", 194 },	/* Performance Counter 1 */
  { "MTRRcap", 254 },	/* MTR Information */
  { "BBL_CR_ADDR", 278 },	/* L2 Address Register  */
  { "BBL_CR_DECC", 280 },	/* L2 Data ECC Register */
  { "BBL_CR_CTL", 281 },	/* L2 Control Register */
  { "BBL_CR_TRIG", 282 },	/* L2 Trigger Register */
  { "BBL_CR_BUSY", 283 },	/* L2 Busy Register */
  { "BBL_CR_CTL3", 286 },	/* L2 Control Register 3 */
  { "MCG_CAP", 377 },	/* Machine-Check Architecture Implementation */
  { "MCG_STATUS", 378 },	/* Machine-Check Status */
  { "MCG_CTL", 379 },	/* Machine-Check Control */
  { "EVNTSEL0", 390 },	/* Event Select 0 */
  { "EVNTSEL1", 391 },	/* Event Select 1 */
  { "DEBUGCTLMSR", 473 },	/* Debug Control */
  { "LASTBRANCHFROMIP", 475 }, /* LastBranchFromIP */
  { "LASTBRANCTOIP", 476 },	/* LastBranchToIP */
  { "LASTINTFROMIP", 477 },	/* LastExceptionFromIP */
  { "LASTINTTOIP", 478 },	/* LastExceptionToIP */
  { "ROB_CR_BKUPTMPDR6", 480 }, /*  */
  { "MTRRphysBase0", 512 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask0", 513 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase1", 514 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask1", 515 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase2", 516 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask2", 517 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase3", 518 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask3", 519 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase4", 520 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask4", 521 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase5", 522 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask5", 523 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase6", 524 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask6", 525 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRphysBase7", 526 },	/* Variable Range MTRR Base Address and Type */
  { "MTRRphysMask7", 527 },	/* Variable Range MTRR Address Range Mask */
  { "MTRRfix64K_0000", 592 }, /* Fixed Range MTRR */
  { "MTRRfix16K_8000", 600 }, /* Fixed Range MTRR */
  { "MTRRfix16K_a000", 601 }, /* Fixed Range MTRR */
  { "MTRRfix4K_c000", 616 },  /* Fixed Range MTRR */
  { "MTRRfix4K_c800", 617 },  /* Fixed Range MTRR */
  { "MTRRfix4K_d000", 618 },  /* Fixed Range MTRR */
  { "MTRRfix4K_d800", 619 },  /* Fixed Range MTRR */
  { "MTRRfix4K_e000", 620 },  /* Fixed Range MTRR */
  { "MTRRfix4K_e800", 621 },  /* Fixed Range MTRR */
  { "MTRRfix4K_f000", 622 },  /* Fixed Range MTRR */
  { "MTRRfix4K_f800", 623 },  /* Fixed Range MTRR */
  { "MTRRdefType", 767 },	/* Default Properties of non-MTRR memory */
  { "MC0_CTL", 1024 },		/* Bank 0 Machine-Check Error Reporting Control */
  { "MC0_STATUS", 1025 },	/* Bank 0 Machine-Check Error Reporting Status */
  { "MC0_ADDR", 1026 },	/* Bank 0 Machine-Check Error Reporting Mask */
  { "MC1_CTL", 1028 },		/* Bank 1 Machine-Check Error Reporting Control */
  { "MC1_STATUS", 1029 },	/* Bank 1 Machine-Check Error Reporting Status */
  { "MC1_ADDR", 1030 },	/* Bank 1 Machine-Check Error Reporting Mask */
  { "MC2_CTL", 1032 },		/* Bank 2 Machine-Check Error Reporting Control */
  { "MC2_STATUS", 1033 },	/* Bank 2 Machine-Check Error Reporting Status */
  { "MC2_ADDR", 1034 },	/* Bank 2 Machine-Check Error Reporting Mask */
  { "MC4_CTL", 1036 },		/* Bank 4 Machine-Check Error Reporting Control */
  { "MC4_STATUS", 1037 },	/* Bank 4 Machine-Check Error Reporting Status */
  { "MC4_ADDR", 1038 },	/* Bank 4 Machine-Check Error Reporting Mask */
  { "MC3_CTL", 1040 },		/* Bank 3 Machine-Check Error Reporting Control */
  { "MC3_STATUS", 1041 },	/* Bank 3 Machine-Check Error Reporting Status */
  { "MC3_ADDR", 1042 }		/* Bank 3 Machine-Check Error Reporting Mask */
};
static const long num_msr_regs = (sizeof i386_msr_regs
				  / sizeof i386_msr_regs[0]);
/* FIXME: cagney/2002-08-28: Just ``know'' that there are 8 MSR regs.  */
#define NUM_I386_MMX_REGS 8 /*MAGIC*/

static int
regnum_to_msrnum (int regnum)
{
  return regnum - (NUM_REGS + NUM_I386_MMX_REGS);
}

static int
msr_regnum_p (int regnum)
{
  int msrnum = regnum_to_msrnum (regnum);
  return (msrnum >= 0 && msrnum < num_msr_regs);
}

/* Query the target for selected MSR, if available.  This also tells
   the target which MSR we are interested in.  */

static int
i386_query_msr_regnum (int regnum)
{
  int size = 0;
  char *buf;
  char *rets;
  char *qstr;
  struct cleanup *saved_cleanups = make_cleanup (null_cleanup, NULL);

  gdb_assert (msr_regnum_p (regnum));

  /* Get the current buffer size. */
  if (current_target.to_query == NULL)
    return -1;
  size = 0;
  target_query ('M', "SR", NULL, &size);

  buf = xmalloc (size);
  make_cleanup (xfree, buf);

  /* Now query the MSR info. */
  xasprintf (&qstr, "SR,%lx,%s",
	     i386_msr_regs[regnum_to_msrnum (regnum)].address,
	     i386_msr_regs[regnum_to_msrnum (regnum)].name);
  make_cleanup (xfree, qstr);

  if (target_query ('M', qstr, buf, &size) < 0)
    return -1;

  if ((strlen (buf) == 0)
      || (strcmp (buf, "INVALID") == 0))
    {
      do_cleanups (saved_cleanups);
      return -1;
    }

  do_cleanups (saved_cleanups);
  return 0;
}

/* Return the name of register REG.  */

const char *
i386_rb_register_name (int regnum)
{
  if (sys_regnum_p (regnum))
    return i386_sys_regs[regnum_to_sysnum (regnum)].name;
  if (msr_regnum_p (regnum))
    return i386_msr_regs[regnum_to_msrnum (regnum)].name;
  return i386_register_name (regnum);
}

static struct type *
builtin_type_i386_gdtr (void)
{
  /* Construct a type for the GDTR and IDTR MM registers.  The type
     we're building is this: */
#if 0
  struct builtin_type_i386_gdtr
  {
    uint32_t base;
    uint16_t lim;
  };
#endif

  static struct type *t;
  struct field *f;

  if (t != NULL)
    return t;

  f = (struct field *) xmalloc (sizeof (*f) * 2);
  memset (f, 0, sizeof (*f) * 2);

  FIELD_TYPE (f[0])= builtin_type_uint32;
  FIELD_NAME (f[0])= "base";
  FIELD_BITPOS (f[0]) = 0;

  FIELD_TYPE (f[1])= builtin_type_uint16;
  FIELD_NAME (f[1])= "lim";
  FIELD_BITPOS (f[1]) = 32;

  /* Build a struct type with those fields.  */
  t = init_type (TYPE_CODE_STRUCT, 6, 0, 0, 0);
  TYPE_NFIELDS (t) = 2;
  TYPE_FIELDS (t) = f;
  TYPE_TAG_NAME (t) = "builtin_type_i386_gdtr";

  return t;
}

static struct type *
builtin_type_i386_ldtr (void)
{
  /* Construct a type for the LDTR and TR MM registers.  The type
     we're building is this: */
#if 0
  struct builtin_type_i386_ldtr
  {
    uint32_t base;
    uint16_t lim;
    struct
    {
      unsigned int index:13;
      unsigned int TI:1;
      unsigned int RPL:2;
    } sel;
  }; */
#endif

  static struct type *t;	/* The type we are creating. */
  struct type *ts;	/* The type of selector part. */
  struct field *f;	/* Fields of the main type struct. */
  struct field *fs;	/* Fields of the selector part. */

  if (t != NULL)
    return t;

  /* First, make a type for the selector part. */
  fs = (struct field *) xmalloc (sizeof (*fs) * 3);
  memset (fs, 0, sizeof (*fs) * 3);

  /* Note that we reverse the order so we see the bit fields from
     left to right, as in a manual figure. */

  FIELD_TYPE (fs[0]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[0]) = "index";
  FIELD_BITPOS (fs[0]) = 3;
  FIELD_BITSIZE (fs[0]) = 13;

  FIELD_TYPE (fs[1]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[1]) = "TI";
  FIELD_BITPOS (fs[1]) = 2;
  FIELD_BITSIZE (fs[1]) = 1;

  FIELD_TYPE (fs[2]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[2]) = "RPL";
  FIELD_BITPOS (fs[2]) = 0;
  FIELD_BITSIZE (fs[2]) = 2;

  /* Build a struct type with those fields.  */
  ts = init_type (TYPE_CODE_STRUCT, 2, 0, 0, 0);
  TYPE_NFIELDS (ts) = 3;
  TYPE_FIELDS (ts) = fs;
  TYPE_TAG_NAME (ts) = "builtin_type_i386_ldtr_selector";

  /* Now build the main type. */
  f = (struct field *) xmalloc (sizeof (*f) * 3);
  memset (f, 0, sizeof (*f) * 3);

  FIELD_TYPE (f[0])= builtin_type_uint32;
  FIELD_NAME (f[0])= "base";
  FIELD_BITPOS (f[0]) = 0;

  FIELD_TYPE (f[1]) = builtin_type_uint16;
  FIELD_NAME (f[1]) = "lim";
  FIELD_BITPOS (f[1]) = 32;

  FIELD_TYPE (f[2])= ts;
  FIELD_NAME (f[2])= "sel";
  FIELD_BITPOS (f[2]) = 48;

  /* Build a struct type with those fields.  */
  t = init_type (TYPE_CODE_STRUCT, 8, 0, 0, 0);
  TYPE_NFIELDS (t) = 3;
  TYPE_FIELDS (t) = f;
  TYPE_TAG_NAME (t) = "builtin_type_i386_ldtr";

  return t;
}

static struct type *
builtin_type_i386_cr0 (void)
{
  /* Construct a type for the CR0 register.  The type we're building
     is this: */
#if 0
  union builtin_type_i386_cr0
  {
    void * cr0;
    struct __builtin_i386_cr0_bits
    {
      unsigned PG:1;
      unsigned CD:1;
      unsigned NW:1;
      /* We skip 10 reserved bits here.  */
      unsigned AM:1;
      /* We skip 1 reserved bit here.  */
      unsigned WP:1;
      /* We skip 10 reserved bits here.  */
      unsigned NE:1;
      unsigned ET:1;
      unsigned TS:1;
      unsigned EM:1;
      unsigned MP:1;
      unsigned PE:1;
    } bits;
  };
#endif
  /* Note that we cannot create a structure like that in C because we
     could not skip the reserved bits and the order would be the
     reverse of what we want (we want to see bit names from left to
     right, as in a manual figure).  */

  static struct type *t;	/* The type we are creating. */
  struct type *tp;	/* The type of the pointer part. */
  struct type *tb;	/* The type of the bitfields part. */
  struct field *fu;	/* Fields of the union. */
  struct field *fs;	/* Fields of the struct. */

  if (t != NULL)
    return t;

  /* Create fields for each group of bits. */
  fs = (struct field *) xmalloc (sizeof (*fs) * 11);
  memset (fs, 0, sizeof (*fs) * 11);

  /* Note that we reverse the order of the fields so they are printed
     as we would see then in a manual figure, left to right. */

  FIELD_TYPE (fs[10]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[10]) = "PE";
  FIELD_BITPOS (fs[10]) = 0;
  FIELD_BITSIZE (fs[10]) = 1;

  FIELD_TYPE (fs[9]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[9]) = "MP";
  FIELD_BITPOS (fs[9]) = 1;
  FIELD_BITSIZE (fs[9]) = 1;

  FIELD_TYPE (fs[8]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[8]) = "EM";
  FIELD_BITPOS (fs[8]) = 2;
  FIELD_BITSIZE (fs[8]) = 1;

  FIELD_TYPE (fs[7]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[7]) = "TS";
  FIELD_BITPOS (fs[7]) = 3;
  FIELD_BITSIZE (fs[7]) = 1;

  FIELD_TYPE (fs[6]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[6]) = "ET";
  FIELD_BITPOS (fs[6]) = 4;
  FIELD_BITSIZE (fs[6]) = 1;

  FIELD_TYPE (fs[5]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[5]) = "NE";
  FIELD_BITPOS (fs[5]) = 5;
  FIELD_BITSIZE (fs[5]) = 1;

  FIELD_TYPE (fs[4]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[4]) = "WP";
  FIELD_BITPOS (fs[4]) = 16;
  FIELD_BITSIZE (fs[4]) = 1;

  FIELD_TYPE (fs[3]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[3]) = "AM";
  FIELD_BITPOS (fs[3]) = 18;
  FIELD_BITSIZE (fs[3]) = 1;

  FIELD_TYPE (fs[2]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[2]) = "NW";
  FIELD_BITPOS (fs[2]) = 29;
  FIELD_BITSIZE (fs[2]) = 1;

  FIELD_TYPE (fs[1]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[1]) = "CD";
  FIELD_BITPOS (fs[1]) = 30;
  FIELD_BITSIZE (fs[1]) = 1;

  FIELD_TYPE (fs[0]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[0]) = "PG";
  FIELD_BITPOS (fs[0]) = 31;
  FIELD_BITSIZE (fs[0]) = 1;

  /* Build a struct type with these bitfields.  */
  tb = init_type (TYPE_CODE_STRUCT, 4, 0, 0, 0);
  TYPE_NFIELDS (tb) = 11;
  TYPE_FIELDS (tb) = fs;
  TYPE_TAG_NAME (tb) = "builtin_type_i386_cr0_bits";

  /* Create the pointer part. */
  tp = make_pointer_type (builtin_type_void, NULL);

  /* Now make our type as the union of the pointer and the bitfield parts. */
  fu = (struct field *) xmalloc (sizeof (*fu) * 2);
  memset (fu, 0, sizeof (*fu) * 2);

  FIELD_TYPE (fu[0]) = tp;
  FIELD_NAME (fu[0]) = "cr0";

  FIELD_TYPE (fu[1]) = tb;
  FIELD_NAME (fu[1]) = "bits";

  /* Build a union type with those fields.  */
  t = init_type (TYPE_CODE_UNION, 4, 0, 0, 0);
  TYPE_NFIELDS (t) = 2;
  TYPE_FIELDS (t) = fu;
  TYPE_TAG_NAME (t) = "builtin_type_i386_cr0";

  return t;
}

static struct type *
builtin_type_i386_cr2 (void)
{
  return builtin_type_uint32;
}

static struct type *
builtin_type_i386_cr3 (void)
{
  /* Construct a type for the PDBR (CR3).  The type we're building is
     this: */
#if 0
  union builtin_type_i386_cr3
  {
    void * cr3;
    struct builtin_type_i386_pdbr
    {
      unsigned int PDbase:20;
      /* We skip 7 reserved bits here.  */
      unsigned int PCD:1;
      unsigned int PWT:1;
      /* We ignore 3 reserved bits here.  */
    };
  };
#endif

  struct type *t;     /* The type we are creating. */
  struct type *tp;    /* The type of the pointer part. */
  struct type *tb;    /* The type of the bitfields part. */
  struct field *fu;   /* Fields of the union. */
  struct field *fs;   /* Fields of the struct. */

  fs = (struct field *) xmalloc (sizeof (*fs) * 3);
  memset (fs, 0, sizeof (*fs) * 3);

  FIELD_TYPE (fs[0])= builtin_type_unsigned_int;
  FIELD_NAME (fs[0])= "PDbase";
  FIELD_BITPOS (fs[0]) = 12;
  FIELD_BITSIZE (fs[0]) = 20;

  FIELD_TYPE (fs[1])= builtin_type_unsigned_int;
  FIELD_NAME (fs[1])= "PCD";
  FIELD_BITPOS (fs[1]) = 4;
  FIELD_BITSIZE (fs[1]) = 1;

  FIELD_TYPE (fs[2])= builtin_type_unsigned_int;
  FIELD_NAME (fs[2])= "PWT";
  FIELD_BITPOS (fs[2]) = 3;
  FIELD_BITSIZE (fs[2]) = 1;

  /* Build a struct type with those fields.  */
  tb = init_type (TYPE_CODE_STRUCT, 4, 0, 0, 0);
  TYPE_NFIELDS (tb) = 3;
  TYPE_FIELDS (tb) = fs;
  TYPE_TAG_NAME (tb) = "builtin_type_i386_cr3_bits";

  /* Create the pointer part. */
  tp = make_pointer_type (builtin_type_void, NULL);

  /* Now make our type as the union of the pointer and the bitfield parts. */
  fu = (struct field *) xmalloc (sizeof (*fu) * 2);
  memset (fu, 0, sizeof (*fu) * 2);

  FIELD_TYPE (fu[0]) = tp;
  FIELD_NAME (fu[0]) = "cr3";

  FIELD_TYPE (fu[1]) = tb;
  FIELD_NAME (fu[1]) = "bits";

  /* Build a union type with those fields.  */
  t = init_type (TYPE_CODE_UNION, 4, 0, 0, 0);
  TYPE_NFIELDS (t) = 2;
  TYPE_FIELDS (t) = fu;
  TYPE_TAG_NAME (t) = "builtin_type_i386_cr3";

  return t;
}

static struct type *
builtin_type_i386_cr4 (void)
{
  /* Construct a type for the CR4 register.  The type we're building
     is this: */
#if 0
  union builtin_type_i386_cr4
  {
    void * cr0;
    struct __builtin_i386_cr4_bits {
      /* We skip the 21 reserved bits here.  */
      unsigned OSXMMEXCPT:1;
      unsigned OSFXSR:1;
      unsigned PCE:1;
      unsigned PGE:1;
      unsigned MCE:1;
      unsigned PAE:1;
      unsigned PSE:1;
      unsigned DE:1;
      unsigned TSD:1;
      unsigned PVI:1;
      unsigned VME:1;
    } bits;
  };
#endif
  /* Note that we cannot create a structure like that in C because we
     could not skip the reserved bits and the order would be the
     reverse of what we want (we want to see bit names from left to
     right, as in a manual figure).  */

  struct type *t;	/* The type we are creating. */
  struct type *tp;	/* The type of the pointer part. */
  struct type *tb;	/* The type of the bitfields part. */
  struct field *fu;	/* Fields of the union. */
  struct field *fs;	/* Fields of the struct. */

  /* Create fields for each group of bits. */
  fs = (struct field *) xmalloc (sizeof (*fs) * 11);
  memset (fs, 0, sizeof (*fs) * 11);

  /* Note that we reverse the order of the fields so they are printed
     as we would see then in a manual figure, left to right. */

  FIELD_TYPE (fs[10]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[10]) = "VME";
  FIELD_BITPOS (fs[10]) = 0;
  FIELD_BITSIZE (fs[10]) = 1;

  FIELD_TYPE (fs[9]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[9]) = "PVI";
  FIELD_BITPOS (fs[9]) = 1;
  FIELD_BITSIZE (fs[9]) = 1;

  FIELD_TYPE (fs[8]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[8]) = "TSD";
  FIELD_BITPOS (fs[8]) = 2;
  FIELD_BITSIZE (fs[8]) = 1;

  FIELD_TYPE (fs[7]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[7]) = "DE";
  FIELD_BITPOS (fs[7]) = 3;
  FIELD_BITSIZE (fs[7]) = 1;

  FIELD_TYPE (fs[6]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[6]) = "PSE";
  FIELD_BITPOS (fs[6]) = 4;
  FIELD_BITSIZE (fs[6]) = 1;

  FIELD_TYPE (fs[5]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[5]) = "PAE";
  FIELD_BITPOS (fs[5]) = 5;
  FIELD_BITSIZE (fs[5]) = 1;

  FIELD_TYPE (fs[4]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[4]) = "MCE";
  FIELD_BITPOS (fs[4]) = 6;
  FIELD_BITSIZE (fs[4]) = 1;

  FIELD_TYPE (fs[3]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[3]) = "PGE";
  FIELD_BITPOS (fs[3]) = 7;
  FIELD_BITSIZE (fs[3]) = 1;

  FIELD_TYPE (fs[2]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[2]) = "PCE";
  FIELD_BITPOS (fs[2]) = 8;
  FIELD_BITSIZE (fs[2]) = 1;

  FIELD_TYPE (fs[1]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[1]) = "OSFXSR";
  FIELD_BITPOS (fs[1]) = 9;
  FIELD_BITSIZE (fs[1]) = 1;

  FIELD_TYPE (fs[0]) = builtin_type_unsigned_int;
  FIELD_NAME (fs[0]) = "OSXMMEXCPT";
  FIELD_BITPOS (fs[0]) = 10;
  FIELD_BITSIZE (fs[0]) = 1;

  /* Build a struct type with these bitfields.  */
  tb = init_type (TYPE_CODE_STRUCT, 4, 0, 0, 0);
  TYPE_NFIELDS (tb) = 11;
  TYPE_FIELDS (tb) = fs;
  TYPE_TAG_NAME (tb) = "builtin_type_i386_cr4_bits";

  /* Create the pointer part. */
  tp = make_pointer_type (builtin_type_void, NULL);

  /* Now make our type as the union of the pointer and the bitfield parts. */
  fu = (struct field *) xmalloc (sizeof (*fu) * 2);
  memset (fu, 0, sizeof (*fu) * 2);

  FIELD_TYPE (fu[0]) = tp;
  FIELD_NAME (fu[0]) = "cr4";

  FIELD_TYPE (fu[1]) = tb;
  FIELD_NAME (fu[1]) = "bits";

  /* Build a union type with those fields.  */
  t = init_type (TYPE_CODE_UNION, 4, 0, 0, 0);
  TYPE_NFIELDS (t) = 2;
  TYPE_FIELDS (t) = fu;
  TYPE_TAG_NAME (t) = "builtin_type_i386_cr4";

  return t;
}

static struct type *
builtin_type_i386_msr (void)
{
  return builtin_type_uint64;
}

/* Return the GDB type object for the "standard" data type of data in
   register REGNUM.  Perhaps %esi and %edi should go here, but
   potentially they could be used for things other than address.  */

struct type *
i386_rb_register_virtual_type (int regnum)
{
  if (sys_regnum_p (regnum))
    return i386_sys_regs[regnum_to_sysnum (regnum)].type ();
  if (msr_regnum_p (regnum))
    return builtin_type_i386_msr ();
  return i386_register_virtual_type (regnum);
}

static struct reggroup *msr_reggroup;

static int
i386_rb_register_regroup_p (struct gdbarch *gdbarch, int regnum,
			    struct reggroup *group)
{
  if (group == system_reggroup)
    return sys_regnum_p (regnum);
  if (group == msr_reggroup)
    return msr_regnum_p (regnum);
  if (sys_regnum_p (regnum) || msr_regnum_p (regnum))
    return 0;
  return i386_register_reggroup_p (gdbarch, regnum, group);
}

int
i386_rb_gpacket_upper_bound_hack (void)
{
  return I386_SSE_NUM_REGS;
}

int
i386_rb_next_abi_register (struct gdbarch *gdbarch, int regnum)
{
  if (regnum < 0)
    return 0;
  regnum++;
  if (regnum >= I386_SSE_NUM_REGS)
    return -1;
  return regnum;
}

/* Map registers onto their real counterparts.  Note these functions
   need sibling changes in the code that does the same thing for
   registers on a frame.  */

static void
i386_msr_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
			int regnum, void *buf)
{
  /* ULGH.  */
  gdb_assert (msr_regnum_p (regnum));
  if (i386_query_msr_regnum (regnum) == 0)
    {
      /* Invalidate the MSR in the register cache forcing a re-read.  */
      register_valid[msr_regnum ()] = 0;
      regcache_raw_read (regcache, msr_regnum (), buf);
    }
}

static void
i386_rb_pseudo_register_read (struct gdbarch *gdbarch,
			      struct regcache *regcache,
			      int regnum, void *buf)
{
  if (msr_regnum_p (regnum))
    i386_msr_register_read (gdbarch, regcache, regnum, buf);
  else
    i386_pseudo_register_read (gdbarch, regcache, regnum, buf);
}

static void
i386_msr_register_write (struct gdbarch *gdbarch,
			 struct regcache *regcache, int regnum,
			 const void *buf)
{
  /* ULGH.  */
  gdb_assert (msr_regnum_p (regnum));
  if (i386_query_msr_regnum (regnum) == 0)
    {
      regcache_raw_write (regcache, msr_regnum (), buf);
    }
}

static void
i386_rb_pseudo_register_write (struct gdbarch *gdbarch,
			       struct regcache *regcache,
			       int regnum, const void *buf)
{
  if (msr_regnum_p (regnum))
    i386_msr_register_write (gdbarch, regcache, regnum, buf);
  else
    i386_pseudo_register_write (gdbarch, regcache, regnum, buf);
}

static void
i386_rb_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Red Boot uses ELF.  */
  i386_elf_init_abi (info, gdbarch);

  /* We support the SSE registers on Red Boot  */
  tdep->num_xmm_regs = I386_NUM_XREGS - 1;

  set_gdbarch_register_bytes (gdbarch, (I386_SSE_SIZEOF_REGS
					+ num_sys_regs * 4));

  /* Since we have the extra system and MSR registers, we need to
     adjust a few things.  */

  set_gdbarch_num_regs (gdbarch, I386_SSE_NUM_REGS + num_sys_regs);
  set_gdbarch_register_name (gdbarch, i386_rb_register_name);
  set_gdbarch_register_virtual_type (gdbarch, i386_rb_register_virtual_type);

  set_gdbarch_num_pseudo_regs (gdbarch, NUM_I386_MMX_REGS + num_msr_regs);
  set_gdbarch_pseudo_register_read (gdbarch, i386_rb_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, i386_rb_pseudo_register_write);

  set_gdbarch_next_cooked_register_to_save (gdbarch, i386_rb_next_abi_register);
  set_gdbarch_next_cooked_register_to_restore (gdbarch, i386_rb_next_abi_register);

  set_gdbarch_register_reggroup_p (gdbarch, i386_rb_register_regroup_p);
  reggroup_add (gdbarch, msr_reggroup);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_i386_tdep (void);

void
_initialize_i386_rb_tdep (void)
{
  /* Ulgh, claim the default.  */
  gdbarch_register_osabi (bfd_arch_i386, GDB_OSABI_REDBOOT,
			  i386_rb_init_abi);

  msr_reggroup = reggroup_new ("msr");
}
