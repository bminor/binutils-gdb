/*
 * Copyright (C) 1991 Gordon Irlam.  All rights reserved.
 */

/*
 * Declaration of system calls for sparc trace generator.
 */


#if !defined(SYSCALLS_H)
#define SYSCALLS_H 1

/*
 * Imported declarations.
 */

#include "agent_msg.h"


/*
 * Declaration of table containing system calls, and their parameter profile.
 */

/*
 * Words, such as the parameters and results of system calls, are capable of
 * being displayed in a number of different formats.
 *
 * fmt_none - indicates the absense of further arguments, functions that don't
 *            return a value, etc.
 *
 * The function format_value(..) can be used to display a word in one of the
 * following formats.
 *
 * fmt_dec - a signed decimal number : 0, 21, -1
 * fmt_hex - a unsigned hex number : 0x0, 0x15, 0xffffffff
 * fmt_ptr - a pointer : NULL, 0x00000015, 0xffffffff
 * fmt_fd - a file descriptor : fd0, fd15, fd-1
 * fmt_signal - the name of a signal : 0, SIGTTIN, -1
 * fmt_error - the name of an error : 0, EISDIR, -1
 * fmt_open_flags - the flags to open : O_RDONLY, O_WRONLY|0x14, 0xffffffff
 * fmt_unknown - representation unknown : .., .., ..
 * fmt_string - if the null termintated string at word is printable displays
 *              the string within quotes, otherwise displays like fmt_ptr
 * fmt_post_string - displays like fmt_ptr, value of word is also saved,
 *                   following the system call a printable string exists at
 *                   address word the address and the string will be displayed
 * fmt_data - only permitted in argument lists, next argument must be
 *            format_data_size, if printable data exists at word having length
 *            specified by the next argument it is printed, otherwise displays
 *            like fmt_ptr
 * fmt_data_size - displays like FMT_SIZE
 * fmt_post_data - displays like fmt_ptr, value of word is also saved,
 *                 following call if a printable length delimited string exists
 *                 it will be displayed
 * fmt_post_data_size - displays like FMT_SIZE, value is saved for use as
 *                      length for fmt_post_data display format
 * fmt_string_array - word is the address of a null terminted array of strings
 *                    to be printed if possible
 *
 * Unlike the string formats which typically represent filenames it is not so
 * important that length delimited data be fully displayed.  When printable,
 * it will be truncate much more harshly than the string formats.
 *
 * Only one item can be pending for display at the end of a system call.
 *
 * At a later date this program may be extended to display length delimited
 * data as a hex dump if it is not printable.
 *
 * The following macros are employed to make it easy to alter how a whole
 * class of values is displayed by changing their definition.
 *
 *     FMT_STATUS - function calls that return 0 on success and -1 on error
 *     FMT_FLAGS - bit field objects
 *     FMT_SIZE - length of an object in bytes
 */
typedef enum fmt_type {fmt_none = 0, fmt_dec, fmt_hex, fmt_ptr, fmt_fd,
    fmt_signal, fmt_error, fmt_open_flags, fmt_unknown, fmt_string,
    fmt_post_string, fmt_data, fmt_post_data, fmt_data_size,
    fmt_post_data_size, fmt_string_array} fmt_type;

#define FMT_STATUS fmt_none
#define FMT_FLAGS  fmt_hex
#define FMT_SIZE   fmt_dec

typedef struct _spa_call_desc {
    char *name;
    fmt_type arg[NO_PARAMS];
    fmt_type result;
} spa_call_desc;

extern spa_call_desc spa_system_calls[];

extern int no_system_calls;

extern spa_call_desc spa_system_call_open_simple;

#define SPA_DATA_SIZE_LIMIT    20
#define SPA_STRING_SIZE_LIMIT 201
#define SPA_ARRAY_SIZE_LIMIT   21


/*
 * Declaration of table containing signal names.
 */

extern char *spa_signal_names[];

extern int spa_no_signal_names;


/*
 * Declaration of table containing error messages.
 */

char *spa_error_names[];

extern int spa_no_error_names;

#endif
