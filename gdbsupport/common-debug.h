/* Declarations for debug printing functions.

   Copyright (C) 2014-2020 Free Software Foundation, Inc.

   This file is part of GDB.

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

#ifndef COMMON_COMMON_DEBUG_H
#define COMMON_COMMON_DEBUG_H

/* Set to true to enable debugging of hardware breakpoint/
   watchpoint support code.  */

extern bool show_debug_regs;

/* Print a formatted message to the appropriate channel for
   debugging output for the client.  */

extern void debug_printf (const char *format, ...)
     ATTRIBUTE_PRINTF (1, 2);

/* Print a formatted message to the appropriate channel for
   debugging output for the client.  This function must be
   provided by the client.  */

extern void debug_vprintf (const char *format, va_list ap)
     ATTRIBUTE_PRINTF (1, 0);

/* Print a debug statement prefixed with the module and function name, and
   with a newline at the end.  */

extern void ATTRIBUTE_PRINTF (3, 4) debug_prefixed_printf
  (const char *module, const char *func, const char *format, ...);

/* Print a debug statement prefixed with the module and function name, and
   with a newline at the end.  */

extern void ATTRIBUTE_PRINTF (3, 0) debug_prefixed_vprintf
  (const char *module, const char *func, const char *format, va_list args);


/* Helper to define "_debug_print" macros.

   DEBUG_ENABLED_COND is an expression that evaluates to true if the debugging
   statement is enabled and should be printed.

   The other arguments, as well as the name of the current function, are
   forwarded to debug_prefixed_printf.  */

#define debug_prefixed_printf_cond(debug_enabled_cond, module, fmt, ...) \
  do \
    { \
      if (debug_enabled_cond) \
	debug_prefixed_printf (module, __func__, fmt, ##__VA_ARGS__); \
    } \
  while (0)

#endif /* COMMON_COMMON_DEBUG_H */
