/* Basic, host-specific, and target-specific definitions for GDB.
   Copyright (C) 1986, 1989, 1991 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (DEFS_H)
#define DEFS_H

/* An address in the program being debugged.  Host byte order.  */
typedef unsigned int CORE_ADDR;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/* The character C++ uses to build identifiers that must be unique from
   the program's identifiers (such as $this and $$vptr).  */
#define CPLUS_MARKER '$'	/* May be overridden to '.' for SysV */

extern int errno;			/* System call error return status */

extern int quit_flag;
extern int immediate_quit;
extern void quit ();

#define QUIT { if (quit_flag) quit (); }

/* Notes on classes: class_alias is for alias commands which are not
   abbreviations of the original command.  */

enum command_class
{
  /* Special args to help_list */
  all_classes = -2, all_commands = -1,
  /* Classes of commands */
  no_class = -1, class_run = 0, class_vars, class_stack,
  class_files, class_support, class_info, class_breakpoint,
  class_alias, class_obscure, class_user
};

/* the cleanup list records things that have to be undone
   if an error happens (descriptors to be closed, memory to be freed, etc.)
   Each link in the chain records a function to call and an
   argument to give it.

   Use make_cleanup to add an element to the cleanup chain.
   Use do_cleanups to do all cleanup actions back to a given
   point in the chain.  Use discard_cleanups to remove cleanups
   from the chain back to a given point, not doing them.  */

struct cleanup
{
  struct cleanup *next;
  void (*function) ();
  int arg;
};

/* From utils.c.  */
extern void do_cleanups ();
extern void discard_cleanups ();
extern struct cleanup *make_cleanup ();
extern struct cleanup *save_cleanups ();
extern void restore_cleanups ();
extern void free_current_contents ();
extern int myread ();
extern int query ();
extern void wrap_here (
#ifdef __STDC__
		       char *
#endif
		       );
extern void reinitialize_more_filter ();
extern void fputs_filtered ();
extern void puts_filtered ();
extern void fprintf_filtered ();
extern void printf_filtered ();
extern void print_spaces ();
extern void print_spaces_filtered ();
extern char *n_spaces ();
extern void printchar ();
extern void fprint_symbol ();
extern void fputs_demangled ();
extern void perror_with_name ();
extern void print_sys_errmsg ();

/* From printcmd.c */
extern void print_address_symbolic ();
extern void print_address ();

/* From source.c */
void mod_path (
#ifdef __STDC__
	       char *, char **
#endif
	       );

/* From readline (but not in any readline .h files).  */
extern char *tilde_expand ();

/* Structure for saved commands lines
   (for breakpoints, defined commands, etc).  */

struct command_line
{
  struct command_line *next;
  char *line;
};

extern struct command_line *read_command_lines ();
extern void free_command_lines ();

/* String containing the current directory (what getwd would return).  */

char *current_directory;

/* Default radixes for input and output.  Only some values supported.  */
extern unsigned input_radix;
extern unsigned output_radix;

/* Baud rate specified for communication with serial target systems.  */
char *baud_rate;

/* Languages represented in the symbol table and elsewhere. */

enum language 
{
   language_unknown, 		/* Language not known */
   language_auto,		/* Placeholder for automatic setting */
   language_c, 			/* C */
   language_cplus, 		/* C++ */
   language_m2			/* Modula-2 */
};

/* Return a format string for printf that will print a number in the local
   (language-specific) hexadecimal format.  Result is static and is
   overwritten by the next call.  local_hex_format_custom takes printf
   options like "08" or "l" (to produce e.g. %08x or %lx).  */

#define local_hex_format() (current_language->la_hex_format)
char *local_hex_format_custom();		/* language.c */

/* Return a string that contains a number formatted in the local
   (language-specific) hexadecimal format.  Result is static and is
   overwritten by the next call.  local_hex_string_custom takes printf
   options like "08" or "l".  */

char *local_hex_string ();			/* language.c */
char *local_hex_string_custom ();		/* language.c */

/* Host machine definition.  This will be a symlink to one of the
   xm-*.h files, built by the `configure' script.  */

#include "xm.h"

/*
 * Allow things in gdb to be declared "const".  If compiling ANSI, it
 * just works.  If compiling with gcc but non-ansi, redefine to __const__.
 * If non-ansi, non-gcc, then eliminate "const" entirely, making those
 * objects be read-write rather than read-only.
 */

#ifndef const
#ifndef __STDC__
# ifdef __GNUC__
#  define const __const__
# else
#  define const /*nothing*/
# endif /* GNUC */
#endif /* STDC */
#endif /* const */

#ifndef volatile
#ifndef __STDC__
# ifdef __GNUC__
#  define volatile __volatile__
# else
#  define volatile /*nothing*/
# endif /* GNUC */
#endif /* STDC */
#endif /* volatile */

/* Defaults for system-wide constants (if not defined by xm.h, we fake it).  */

#if !defined (UINT_MAX)
#define UINT_MAX 0xffffffff
#endif

#if !defined (LONG_MAX)
#define LONG_MAX 0x7fffffff
#endif

#if !defined (INT_MAX)
#define INT_MAX 0x7fffffff
#endif

#if !defined (INT_MIN)
/* Two's complement, 32 bit.  */
#define INT_MIN -0x80000000
#endif

/* Number of bits in a char or unsigned char for the target machine.
   Just like CHAR_BIT in <limits.h> but describes the target machine.  */
#if !defined (TARGET_CHAR_BIT)
#define TARGET_CHAR_BIT 8
#endif

/* Number of bits in a short or unsigned short for the target machine. */
#if !defined (TARGET_SHORT_BIT)
#define TARGET_SHORT_BIT (sizeof (short) * TARGET_CHAR_BIT)
#endif

/* Number of bits in an int or unsigned int for the target machine. */
#if !defined (TARGET_INT_BIT)
#define TARGET_INT_BIT (sizeof (int) * TARGET_CHAR_BIT)
#endif

/* Number of bits in a long or unsigned long for the target machine. */
#if !defined (TARGET_LONG_BIT)
#define TARGET_LONG_BIT (sizeof (long) * TARGET_CHAR_BIT)
#endif

/* Number of bits in a long long or unsigned long long for the target machine. */
#if !defined (TARGET_LONG_LONG_BIT)
#define TARGET_LONG_LONG_BIT (2 * TARGET_LONG_BIT)
#endif

/* Number of bits in a float for the target machine. */
#if !defined (TARGET_FLOAT_BIT)
#define TARGET_FLOAT_BIT (sizeof (float) * TARGET_CHAR_BIT)
#endif

/* Number of bits in a double for the target machine. */
#if !defined (TARGET_DOUBLE_BIT)
#define TARGET_DOUBLE_BIT (sizeof (double) * TARGET_CHAR_BIT)
#endif

/* Number of bits in a long double for the target machine. */
#if !defined (TARGET_LONG_DOUBLE_BIT)
#define TARGET_LONG_DOUBLE_BIT (2 * TARGET_DOUBLE_BIT)
#endif

/* Number of bits in a "complex" for the target machine. */
#if !defined (TARGET_COMPLEX_BIT)
#define TARGET_COMPLEX_BIT (2 * TARGET_FLOAT_BIT)
#endif

/* Number of bits in a "double complex" for the target machine. */
#if !defined (TARGET_DOUBLE_COMPLEX_BIT)
#define TARGET_DOUBLE_COMPLEX_BIT (2 * TARGET_DOUBLE_BIT)
#endif

/* Convert a LONGEST to an int.  This is used in contexts (e.g. number
   of arguments to a function, number in a value history, register
   number, etc.) where the value must not be larger than can fit
   in an int.  */
#if !defined (longest_to_int)
#if defined (LONG_LONG)
#define longest_to_int(x) (((x) > INT_MAX || (x) < INT_MIN) \
			   ? error ("Value out of range.") : (int) (x))
#else /* No LONG_LONG.  */
/* Assume sizeof (int) == sizeof (long).  */
#define longest_to_int(x) ((int) (x))
#endif /* No LONG_LONG.  */
#endif /* No longest_to_int.  */

/* Assorted functions we can declare, now that const and volatile are 
   defined.  */
extern char *savestring ();
extern char *strsave ();
extern char *concat ();
#ifdef __STDC__
extern void *xmalloc (), *xrealloc ();
#else
extern char *xmalloc (), *xrealloc ();
#endif
extern void free ();
extern int parse_escape ();
extern char *reg_names[];
/* Indicate that these routines do not return to the caller.  */
extern volatile void error(), fatal();
extern void warning_setup(), warning();

/* Various possibilities for alloca.  */
#ifndef alloca
# ifdef __GNUC__
#  define alloca __builtin_alloca
# else
#  ifdef sparc
#   include <alloca.h>
#  endif
   extern char *alloca ();
# endif
#endif

/* TARGET_BYTE_ORDER and HOST_BYTE_ORDER should be defined to one of these.  */

#if !defined (BIG_ENDIAN)
#define BIG_ENDIAN 4321
#endif

#if !defined (LITTLE_ENDIAN)
#define LITTLE_ENDIAN 1234
#endif

/* Target-system-dependent parameters for GDB.

   The standard thing is to include defs.h.  However, files that are
   specific to a particular target can define TM_FILE_OVERRIDE before
   including defs.h, then can include any particular tm-file they desire.  */

/* Target machine definition.  This will be a symlink to one of the
   tm-*.h files, built by the `configure' script.  */

#ifndef TM_FILE_OVERRIDE
#include "tm.h"
#endif

/* The bit byte-order has to do just with numbering of bits in
   debugging symbols and such.  Conceptually, it's quite separate
   from byte/word byte order.  */

#if !defined (BITS_BIG_ENDIAN)
#if TARGET_BYTE_ORDER == BIG_ENDIAN
#define BITS_BIG_ENDIAN 1
#endif /* Big endian.  */

#if TARGET_BYTE_ORDER == LITTLE_ENDIAN
#define BITS_BIG_ENDIAN 0
#endif /* Little endian.  */
#endif /* BITS_BIG_ENDIAN not defined.  */

/* Swap LEN bytes at BUFFER between target and host byte-order.  */
#if TARGET_BYTE_ORDER == HOST_BYTE_ORDER
#define SWAP_TARGET_AND_HOST(buffer,len)
#else /* Target and host byte order differ.  */
#define SWAP_TARGET_AND_HOST(buffer,len) \
  {	       	       	       	       	       	       	       	       	 \
    char tmp;								 \
    char *p = (char *)(buffer);						 \
    char *q = ((char *)(buffer)) + len - 1;		   		 \
    for (; p < q; p++, q--)				 		 \
      {									 \
        tmp = *q;							 \
        *q = *p;							 \
        *p = tmp;							 \
      }									 \
  }
#endif /* Target and host byte order differ.  */

/* On some machines there are bits in addresses which are not really
   part of the address, but are used by the kernel, the hardware, etc.
   for special purposes.  ADDR_BITS_REMOVE takes out any such bits
   so we get a "real" address such as one would find in a symbol
   table.  ADDR_BITS_SET sets those bits the way the system wants
   them.  */
#if !defined (ADDR_BITS_REMOVE)
#define ADDR_BITS_REMOVE(addr) (addr)
#define ADDR_BITS_SET(addr) (addr)
#endif /* No ADDR_BITS_REMOVE.  */

#if !defined (SYS_SIGLIST_MISSING)
#define SYS_SIGLIST_MISSING defined (USG)
#endif /* No SYS_SIGLIST_MISSING */

#endif /* no DEFS_H */
