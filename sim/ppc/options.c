/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    */

#ifndef _OPTIONS_C_
#define _OPTIONS_C_

#include "cpu.h"
#include "options.h"

STATIC_INLINE const char *
options_byte_order (int order)
{
  switch (order) {
  case 0:		return "0";
  case BIG_ENDIAN:	return "BIG_ENDIAN";
  case LITTLE_ENDIAN:	return "LITTLE_ENDIAN";
  }

  return "UNKNOWN";
}

STATIC_INLINE const char *
options_env (int env)
{
  switch (env) {
  case OPERATING_ENVIRONMENT:	return "OPERATING";
  case VIRTUAL_ENVIRONMENT:	return "VIRTUAL";
  case USER_ENVIRONMENT:	return "USER";
  case 0:			return "0";
  }

  return "UNKNOWN";
}

STATIC_INLINE const char *
options_align (int align)
{
  switch (align) {
  case NONSTRICT_ALIGNMENT:	return "NONSTRICT";
  case STRICT_ALIGNMENT:	return "STRICT";
  case 0:			return "0";
  }

  return "UNKNOWN";
}

STATIC_INLINE const char *
options_float (int float_type)
{
  switch (float_type) {
  case SOFT_FLOATING_POINT:	return "SOFTWARE";
  case HARD_FLOATING_POINT:	return "HARDWARE";
  }

  return "UNKNOWN";
}

STATIC_INLINE const char *
options_mon (int mon)
{
  switch (mon) {
  case MONITOR_INSTRUCTION_ISSUE|MONITOR_LOAD_STORE_UNIT:	return "ALL";
  case MONITOR_INSTRUCTION_ISSUE:				return "INSTRUCTION";
  case MONITOR_LOAD_STORE_UNIT:					return "MEMORY";
  case 0:							return "0";
  }

  return "UNKNOWN";
}

INLINE_OPTIONS void
print_options (void)
{
#if defined(_GNUC_) && defined(__VERSION__)
  printf_filtered ("Compiled by GCC %s on %s %s\n", __VERSION__, __DATE__, __TIME__);
#else
  printf_filtered ("Compiled on %s %s\n", __DATE__, __TIME__);
#endif

  printf_filtered ("WITH_HOST_BYTE_ORDER     = %s\n", options_byte_order (WITH_HOST_BYTE_ORDER));
  printf_filtered ("WITH_TARGET_BYTE_ORDER   = %s\n", options_byte_order (WITH_TARGET_BYTE_ORDER));
  printf_filtered ("WITH_XOR_ENDIAN          = %d\n", WITH_XOR_ENDIAN);
  printf_filtered ("WITH_BSWAP               = %d\n", WITH_BSWAP);
  printf_filtered ("WITH_SMP                 = %d\n", WITH_SMP);
  printf_filtered ("WITH_HOST_WORD_BITSIZE   = %d\n", WITH_HOST_WORD_BITSIZE);
  printf_filtered ("WITH_TARGET_WORD_BITSIZE = %d\n", WITH_TARGET_WORD_BITSIZE);
  printf_filtered ("WITH_ENVIRONMENT         = %s\n", options_env(WITH_ENVIRONMENT));
  printf_filtered ("WITH_EVENTS              = %d\n", WITH_EVENTS);
  printf_filtered ("WITH_TIME_BASE           = %d\n", WITH_TIME_BASE);
  printf_filtered ("WITH_CALLBACK_MEMORY     = %d\n", WITH_CALLBACK_MEMORY);
  printf_filtered ("WITH_ALIGNMENT           = %s\n", options_align (WITH_ALIGNMENT));
  printf_filtered ("WITH_FLOATING_POINT      = %s\n", options_float (WITH_FLOATING_POINT));
  printf_filtered ("WITH_TRACE               = %d\n", WITH_TRACE);
  printf_filtered ("WITH_ASSERT              = %d\n", WITH_ASSERT);
  printf_filtered ("WITH_MON                 = %s\n", options_mon (WITH_MON));
  printf_filtered ("WITH_FUNCTION_UNIT       = %d\n", WITH_FUNCTION_UNIT);
  printf_filtered ("WITH_DEFAULT_MODEL       = %s\n", model_name[WITH_DEFAULT_MODEL]);
  printf_filtered ("WITH_MODEL               = %s\n", model_name[WITH_MODEL]);
  printf_filtered ("WITH_RESERVED_BITS       = %d\n", WITH_RESERVED_BITS);
  printf_filtered ("DEFAULT_INLINE           = %d\n", DEFAULT_INLINE);
  printf_filtered ("SIM_ENDIAN_INLINE        = %d\n", SIM_ENDIAN_INLINE);
  printf_filtered ("BITS_INLINE              = %d\n", BITS_INLINE);
  printf_filtered ("CPU_INLINE               = %d\n", CPU_INLINE);
  printf_filtered ("VM_INLINE                = %d\n", VM_INLINE);
  printf_filtered ("CORE_INLINE              = %d\n", CORE_INLINE);
  printf_filtered ("EVENTS_INLINE            = %d\n", EVENTS_INLINE);
  printf_filtered ("MON_INLINE               = %d\n", MON_INLINE);
  printf_filtered ("INTERRUPTS_INLINE        = %d\n", INTERRUPTS_INLINE);
  printf_filtered ("REGISTERS_INLINE         = %d\n", REGISTERS_INLINE);
  printf_filtered ("DEVICE_TREE_INLINE       = %d\n", DEVICE_TREE_INLINE);
  printf_filtered ("DEVICES_INLINE           = %d\n", DEVICES_INLINE);
  printf_filtered ("SPREG_INLINE             = %d\n", SPREG_INLINE);
  printf_filtered ("SEMANTICS_INLINE         = %d\n", SEMANTICS_INLINE);
  printf_filtered ("IDECODE_INLINE           = %d\n", IDECODE_INLINE);
  printf_filtered ("FUNCTION_UNIT_INLINE     = %d\n", FUNCTION_UNIT_INLINE);
  printf_filtered ("OPTIONS_INLINE           = %d\n", FUNCTION_UNIT_INLINE);

#ifdef OPCODE_RULES
  printf_filtered ("OPCODE rules             = %s\n", OPCODE_RULES);
#endif

#ifdef IGEN_FLAGS
  printf_filtered ("IGEN_FLAGS               = %s\n", IGEN_FLAGS);
#endif

#ifdef DGEN_FLAGS
  printf_filtered ("DGEN_FLAGS               = %s\n", DGEN_FLAGS);
#endif
}

#endif /* _OPTIONS_C_ */
