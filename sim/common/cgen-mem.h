/* Memory ops header for CGEN-based simulators.
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
   Contributed by Cygnus Solutions.

This file is part of the GNU Simulators.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef CGEN_MEM_H
#define CGEN_MEM_H

#ifdef MEMOPS_DEFINE_INLINE
#define MEMOPS_INLINE
#else
#define MEMOPS_INLINE extern inline
#endif

/* Memory read support.  */

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
#define DECLARE_GETMEM(mode, size) \
MEMOPS_INLINE mode \
XCONCAT2 (GETMEM,mode) (SIM_CPU *cpu, IADDR pc, ADDR a) \
{ \
  PROFILE_COUNT_READ (cpu, a, XCONCAT2 (MODE_,mode)); \
  /* Don't read anything into "unaligned" here.  Bad name choice.  */\
  return XCONCAT2 (sim_core_read_unaligned_,size) (cpu, pc, read_map, a); \
}
#else
#define DECLARE_GETMEM(mode, size) \
extern mode XCONCAT2 (GETMEM,mode) (SIM_CPU *, IADDR, ADDR);
#endif

DECLARE_GETMEM (QI, 1)
DECLARE_GETMEM (UQI, 1)
DECLARE_GETMEM (HI, 2)
DECLARE_GETMEM (UHI, 2)
DECLARE_GETMEM (SI, 4)
DECLARE_GETMEM (USI, 4)
DECLARE_GETMEM (DI, 8)
DECLARE_GETMEM (UDI, 8)

#undef DECLARE_GETMEM

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
#define DECLARE_GETMEM(mode, size) \
MEMOPS_INLINE mode \
XCONCAT2 (GETMEM,mode) (SIM_CPU *cpu, IADDR pc, ADDR a) \
{ \
  PROFILE_COUNT_READ (cpu, a, XCONCAT2 (MODE_,mode)); \
  /* Don't read anything into "unaligned" here.  Bad name choice.  */\
  return XCONCAT2 (sim_core_read_unaligned_,size) (cpu, pc, read_map, a); \
}
#else
#define DECLARE_GETMEM(mode, size) \
extern mode XCONCAT2 (GETMEM,mode) (SIM_CPU *, IADDR, ADDR);
#endif

DECLARE_GETMEM (SF, 4)
DECLARE_GETMEM (DF, 8)
/*DECLARE_GETMEM (TF, 16)*/

#undef DECLARE_GETMEM

/* Memory write support.  */

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
#define DECLARE_SETMEM(mode, size) \
MEMOPS_INLINE void \
XCONCAT2 (SETMEM,mode) (SIM_CPU *cpu, IADDR pc, ADDR a, mode val) \
{ \
  PROFILE_COUNT_WRITE (cpu, a, XCONCAT2 (MODE_,mode)); \
  /* Don't read anything into "unaligned" here.  Bad name choice.  */ \
  XCONCAT2 (sim_core_write_unaligned_,size) (cpu, pc, write_map, a, val); \
}
#else
#define DECLARE_SETMEM(mode, size) \
extern void XCONCAT2 (SETMEM,mode) (SIM_CPU *, IADDR, ADDR, mode);
#endif

DECLARE_SETMEM (QI, 1)
DECLARE_SETMEM (UQI, 1)
DECLARE_SETMEM (HI, 2)
DECLARE_SETMEM (UHI, 2)
DECLARE_SETMEM (SI, 4)
DECLARE_SETMEM (USI, 4)
DECLARE_SETMEM (DI, 8)
DECLARE_SETMEM (UDI, 8)

/*
DECLARE_SETMEM (SF, 4)
DECLARE_SETMEM (DF, 8)
DECLARE_SETMEM (TF, 16)
*/

#undef DECLARE_SETMEM

/* Instruction read support.  */

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
#define DECLARE_GETIMEM(mode, size) \
MEMOPS_INLINE mode \
XCONCAT2 (GETIMEM,mode) (SIM_CPU *cpu, IADDR a) \
{ \
  /*PROFILE_COUNT_READ (cpu, a, XCONCAT2 (MODE_,mode));*/ \
  /* Don't read anything into "unaligned" here.  Bad name choice.  */\
  return XCONCAT2 (sim_core_read_unaligned_,size) (cpu, a, exec_map, a); \
}
#else
#define DECLARE_GETIMEM(mode, size) \
extern mode XCONCAT2 (GETIMEM,mode) (SIM_CPU *, ADDR);
#endif

DECLARE_GETIMEM (UQI, 1)
DECLARE_GETIMEM (UHI, 2)
DECLARE_GETIMEM (USI, 4)
DECLARE_GETIMEM (UDI, 8)

#undef DECLARE_GETIMEM

/* GETT<mode>: translate target value at P to host value.
   This needn't be very efficient (i.e. can call memcpy) as this is
   only used when interfacing with the outside world (e.g. gdb).  */

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
#define DECLARE_GETT(mode, size) \
MEMOPS_INLINE mode \
XCONCAT2 (GETT,mode) (unsigned char *p) \
{ \
  mode tmp; \
  memcpy (&tmp, p, sizeof (mode)); \
  return XCONCAT2 (T2H_,size) (tmp); \
}
#else
#define DECLARE_GETT(mode, size) \
extern mode XCONCAT2 (GETT,mode) (unsigned char *);
#endif

DECLARE_GETT (QI, 1)
DECLARE_GETT (UQI, 1)
DECLARE_GETT (HI, 2)
DECLARE_GETT (UHI, 2)
DECLARE_GETT (SI, 4)
DECLARE_GETT (USI, 4)
DECLARE_GETT (DI, 8)
DECLARE_GETT (UDI, 8)

/*
DECLARE_GETT (SF, 4)
DECLARE_GETT (DF, 8)
DECLARE_GETT (TF, 16)
*/

#undef DECLARE_GETT

/* SETT<mode>: translate host value to target value and store at P.
   This needn't be very efficient (i.e. can call memcpy) as this is
   only used when interfacing with the outside world (e.g. gdb).  */

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
#define DECLARE_SETT(mode, size) \
MEMOPS_INLINE void \
XCONCAT2 (SETT,mode) (unsigned char *buf, mode val) \
{ \
  mode tmp; \
  tmp = XCONCAT2 (H2T_,size) (val); \
  memcpy (buf, &tmp, sizeof (mode)); \
}
#else
#define DECLARE_SETT(mode, size) \
extern mode XCONCAT2 (GETT,mode) (unsigned char *, mode);
#endif

DECLARE_SETT (QI, 1)
DECLARE_SETT (UQI, 1)
DECLARE_SETT (HI, 2)
DECLARE_SETT (UHI, 2)
DECLARE_SETT (SI, 4)
DECLARE_SETT (USI, 4)
DECLARE_SETT (DI, 8)
DECLARE_SETT (UDI, 8)

/*
DECLARE_SETT (SF, 4)
DECLARE_SETT (DF, 8)
DECLARE_SETT (TF, 16)
*/

#undef DECLARE_SETT

#endif /* CGEN_MEM_H */
