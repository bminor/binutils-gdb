/* Declarations for debug printing functions.

   Copyright (C) 2014-2021 Free Software Foundation, Inc.

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

/* Nesting depth of scoped_debug_start_end objects.  */

extern int debug_print_depth;

/* Print a message on construction and destruction, to denote the start and end
   of an operation.  Increment DEBUG_PRINT_DEPTH on construction and decrement
   it on destruction, such that nested debug statements will be printed with
   an indent and appear "inside" this one.  */

struct scoped_debug_start_end
{
  /* DEBUG_ENABLED is a reference to a variable that indicates whether debugging
     is enabled, so if the debug statements should be printed.  Is is read
     separately at construction and destruction, such that the start statement
     could be printed but not the end statement, or vice-versa.

     MODULE and FUNC are forwarded to debug_prefixed_printf.

     START_MSG and END_MSG are the statements to print on construction and
     destruction, respectively.  */

  scoped_debug_start_end (bool &debug_enabled, const char *module,
			  const char *func, const char *start_msg,
			  const char *end_msg)
    : m_debug_enabled (debug_enabled),
      m_module (module),
      m_func (func),
      m_end_msg (end_msg)
  {
    if (m_debug_enabled)
      {
	debug_prefixed_printf (m_module, m_func, "%s", start_msg);
	++debug_print_depth;
	m_must_decrement_print_depth = true;
      }
  }

  DISABLE_COPY_AND_ASSIGN (scoped_debug_start_end);

  ~scoped_debug_start_end ()
  {
    if (m_must_decrement_print_depth)
      {
	gdb_assert (debug_print_depth > 0);
	--debug_print_depth;
      }

    if (m_debug_enabled)
      {
	debug_prefixed_printf (m_module, m_func, "%s", m_end_msg);
      }
  }

private:
  bool &m_debug_enabled;
  const char *m_module;
  const char *m_func;
  const char *m_end_msg;

  /* This is used to handle the case where debugging is enabled during
     construction but not during destruction, or vice-versa.  We want to make
     sure there are as many increments are there are decrements.  */

  bool m_must_decrement_print_depth = false;
};

/* Helper to define a module-specific start/end debug macro.  */

#define scoped_debug_start_end(debug_enabled, module, msg) \
  scoped_debug_start_end CONCAT(scoped_debug_start_end, __LINE__) \
    (debug_enabled, module, __func__, "start: " msg, "end: " msg)

/* Helper to define a module-specific enter/exit debug macro.  This is a special
   case of `scoped_debug_start_end` where the start and end messages are "enter"
   and "exit", to denote entry and exit of a function.  */

#define scoped_debug_enter_exit(debug_enabled, module) \
  scoped_debug_start_end CONCAT(scoped_debug_start_end, __LINE__) \
    (debug_enabled, module, __func__, "enter", "exit")

#endif /* COMMON_COMMON_DEBUG_H */
