/* Memory ops header for CGEN-based simlators.

This file is machine generated.

Copyright (C) 1996, 1997 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

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
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef CGEN_MEM_OPS_H
#define CGEN_MEM_OPS_H

#ifdef MEMOPS_DEFINE_INLINE
#define MEMOPS_INLINE
#else
#define MEMOPS_INLINE extern inline
#endif

/* Only used in this file.  */
typedef unsigned char *ptr;

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE QI
GETTQI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return p[0];
  else
    return p[0];
}
#else
extern QI GETTQI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE HI
GETTHI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return ((p[0] << 8) | p[1]);
  else
    return ((p[1] << 8) | p[0]);
}
#else
extern HI GETTHI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE SI
GETTSI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
  else
    return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}
#else
extern SI GETTSI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE DI
GETTDI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return MAKEDI ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3], (p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7]);
  else
    return MAKEDI ((p[7] << 24) | (p[6] << 16) | (p[5] << 8) | p[4], (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}
#else
extern DI GETTDI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE UQI
GETTUQI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return p[0];
  else
    return p[0];
}
#else
extern UQI GETTUQI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE UHI
GETTUHI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return ((p[0] << 8) | p[1]);
  else
    return ((p[1] << 8) | p[0]);
}
#else
extern UHI GETTUHI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE USI
GETTUSI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
  else
    return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}
#else
extern USI GETTUSI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE UDI
GETTUDI (ptr p)
{
  if (TARGET_BIG_ENDIAN)
    return MAKEDI ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3], (p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7]);
  else
    return MAKEDI ((p[7] << 24) | (p[6] << 16) | (p[5] << 8) | p[4], (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}
#else
extern UDI GETTUDI (ptr);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTQI (ptr p, QI val)
{
  if (TARGET_BIG_ENDIAN)
    do { p[0] = val; } while (0);
else
    do { p[0] = val; } while (0);
}
#else
extern void SETTQI (ptr, QI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTHI (ptr p, HI val)
{
  if (TARGET_BIG_ENDIAN)
    do { p[0] = val >> 8; p[1] = val; } while (0);
else
    do { p[1] = val >> 8; p[0] = val; } while (0);
}
#else
extern void SETTHI (ptr, HI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTSI (ptr p, SI val)
{
  if (TARGET_BIG_ENDIAN)
    do { p[0] = val >> 24; p[1] = val >> 16; p[2] = val >> 8; p[3] = val; } while (0);
else
    do { p[3] = val >> 24; p[2] = val >> 16; p[1] = val >> 8; p[0] = val; } while (0);
}
#else
extern void SETTSI (ptr, SI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTDI (ptr p, DI val)
{
  if (TARGET_BIG_ENDIAN)
    do { SI t = GETHIDI (val); p[0] = t >> 24; p[1] = t >> 16; p[2] = t >> 8; p[3] = t; t = GETLODI (val); p[4] = t >> 24; p[5] = t >> 16; p[6] = t >> 8; p[7] = t; } while (0);
else
    do { SI t = GETHIDI (val); p[7] = t >> 24; p[6] = t >> 16; p[5] = t >> 8; p[4] = t; t = GETLODI (val); p[3] = t >> 24; p[2] = t >> 16; p[1] = t >> 8; p[0] = t; } while (0);
}
#else
extern void SETTDI (ptr, DI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTUQI (ptr p, UQI val)
{
  if (TARGET_BIG_ENDIAN)
    do { p[0] = val; } while (0);
else
    do { p[0] = val; } while (0);
}
#else
extern void SETTUQI (ptr, UQI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTUHI (ptr p, UHI val)
{
  if (TARGET_BIG_ENDIAN)
    do { p[0] = val >> 8; p[1] = val; } while (0);
else
    do { p[1] = val >> 8; p[0] = val; } while (0);
}
#else
extern void SETTUHI (ptr, UHI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTUSI (ptr p, USI val)
{
  if (TARGET_BIG_ENDIAN)
    do { p[0] = val >> 24; p[1] = val >> 16; p[2] = val >> 8; p[3] = val; } while (0);
else
    do { p[3] = val >> 24; p[2] = val >> 16; p[1] = val >> 8; p[0] = val; } while (0);
}
#else
extern void SETTUSI (ptr, USI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETTUDI (ptr p, UDI val)
{
  if (TARGET_BIG_ENDIAN)
    do { SI t = GETHIDI (val); p[0] = t >> 24; p[1] = t >> 16; p[2] = t >> 8; p[3] = t; t = GETLODI (val); p[4] = t >> 24; p[5] = t >> 16; p[6] = t >> 8; p[7] = t; } while (0);
else
    do { SI t = GETHIDI (val); p[7] = t >> 24; p[6] = t >> 16; p[5] = t >> 8; p[4] = t; t = GETLODI (val); p[3] = t >> 24; p[2] = t >> 16; p[1] = t >> 8; p[0] = t; } while (0);
}
#else
extern void SETTUDI (ptr, UDI);
#endif


/* FIXME: Need to merge with sim-core.  */
/* FIXME: Don't perform >= 4, text section checks if OEA.  */
#ifndef MEM_CHECK_READ
#define MEM_CHECK_READ(addr, type) \
     ((addr) >= 4 /*&& (addr) < STATE_MEM_SIZE (current_state)*/)
#endif
#ifndef MEM_CHECK_WRITE
#define MEM_CHECK_WRITE(addr, type) \
     ((addr) >= 4 /*&& (addr) < STATE_MEM_SIZE (current_state)*/ \
      && ((addr) >= STATE_TEXT_END (current_state) \
	  || (addr) < STATE_TEXT_START (current_state)))
#endif
#ifndef MEM_CHECK_ALIGNMENT
#define MEM_CHECK_ALIGNMENT(addr, type) \
     (((addr) & (sizeof (type) - 1)) == 0)
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE QI
GETMEMQI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, QI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, QI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_QI);
  return sim_core_read_aligned_1 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern QI GETMEMQI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE HI
GETMEMHI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, HI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, HI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_HI);
  return sim_core_read_aligned_2 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern HI GETMEMHI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE SI
GETMEMSI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, SI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, SI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_SI);
  return sim_core_read_aligned_4 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern SI GETMEMSI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE DI
GETMEMDI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, DI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, DI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_DI);
  return sim_core_read_aligned_8 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern DI GETMEMDI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE UQI
GETMEMUQI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, UQI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, UQI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_UQI);
  return sim_core_read_aligned_1 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern UQI GETMEMUQI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE UHI
GETMEMUHI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, UHI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, UHI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_UHI);
  return sim_core_read_aligned_2 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern UHI GETMEMUHI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE USI
GETMEMUSI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, USI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, USI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_USI);
  return sim_core_read_aligned_4 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern USI GETMEMUSI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE UDI
GETMEMUDI (SIM_CPU *cpu, ADDR a)
{
  if (! MEM_CHECK_READ (a, UDI))
    { engine_signal (cpu, SIM_SIGSEGV); }
  if (! MEM_CHECK_ALIGNMENT (a, UDI))
    { engine_signal (cpu, SIM_SIGBUS); }
  PROFILE_COUNT_READ (cpu, a, MODE_UDI);
  return sim_core_read_aligned_8 (cpu, NULL_CIA, sim_core_read_map, a);
}
#else
extern UDI GETMEMUDI (SIM_CPU *, ADDR);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMQI (SIM_CPU *cpu, ADDR a, QI val)
{
  if (! MEM_CHECK_WRITE (a, QI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, QI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_QI);
  sim_core_write_aligned_1 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMQI (SIM_CPU *, ADDR, QI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMHI (SIM_CPU *cpu, ADDR a, HI val)
{
  if (! MEM_CHECK_WRITE (a, HI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, HI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_HI);
  sim_core_write_aligned_2 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMHI (SIM_CPU *, ADDR, HI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMSI (SIM_CPU *cpu, ADDR a, SI val)
{
  if (! MEM_CHECK_WRITE (a, SI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, SI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_SI);
  sim_core_write_aligned_4 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMSI (SIM_CPU *, ADDR, SI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMDI (SIM_CPU *cpu, ADDR a, DI val)
{
  if (! MEM_CHECK_WRITE (a, DI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, DI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_DI);
  sim_core_write_aligned_8 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMDI (SIM_CPU *, ADDR, DI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMUQI (SIM_CPU *cpu, ADDR a, UQI val)
{
  if (! MEM_CHECK_WRITE (a, UQI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, UQI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_UQI);
  sim_core_write_aligned_1 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMUQI (SIM_CPU *, ADDR, UQI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMUHI (SIM_CPU *cpu, ADDR a, UHI val)
{
  if (! MEM_CHECK_WRITE (a, UHI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, UHI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_UHI);
  sim_core_write_aligned_2 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMUHI (SIM_CPU *, ADDR, UHI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMUSI (SIM_CPU *cpu, ADDR a, USI val)
{
  if (! MEM_CHECK_WRITE (a, USI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, USI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_USI);
  sim_core_write_aligned_4 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMUSI (SIM_CPU *, ADDR, USI);
#endif

#if defined (__GNUC__) || defined (MEMOPS_DEFINE_INLINE)
MEMOPS_INLINE void
SETMEMUDI (SIM_CPU *cpu, ADDR a, UDI val)
{
  if (! MEM_CHECK_WRITE (a, UDI))
    { engine_signal (cpu, SIM_SIGSEGV); return; }
  if (! MEM_CHECK_ALIGNMENT (a, UDI))
    { engine_signal (cpu, SIM_SIGBUS); return; }
  PROFILE_COUNT_WRITE (cpu, a, MODE_UDI);
  sim_core_write_aligned_8 (cpu, NULL_CIA, sim_core_read_map, a, val);
}
#else
extern void SETMEMUDI (SIM_CPU *, ADDR, UDI);
#endif

#endif /* MEM_OPS_H */
