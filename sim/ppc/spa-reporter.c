/*
 * Copyright (C) 1991 Gordon Irlam.  All rights reserved.
 */

/*
 * Sparc trace generator.
 *
 * Generate a Sparc address trace.
 *
 *     Report system calls.
 *
 *     We want to display the system call and the return value at the same time
 *     (so that other output does not appear between the two) but also want to
 *     identify system calls that block without having to wait for them to
 *     return.  Whenever a system call is performed we store the name of the
 *     call and the parameters.  If we don't see a return within a certain time
 *     period we display the call regardless, and assume it has blocked.
 */


/*
 * Imported declarations.
 */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <machine/trap.h>

/*
 * sigcleanup is not defined in a system header file.
 */
#define SYS_sigcleanup 139

#include "prototype.h"
#include "error.h"
#include "spy.h"
#include "system_calls.h"


/*
 * Forward declarations.
 */

PROTOTYPE(void report_trap,
          (int pid, void *addr, int trap, int g1, syscall_params *params));
PROTOTYPE(void report_trap_result, (int pid, int error, int o0, int o1));
PROTOTYPE(void display_trap_msg, (void));
PROTOTYPE(void delayed_trap_msg, (void));
PROTOTYPE(void discard_trap_msg, (void));
PROTOTYPE(int copy_memory, (int pid, void *addr, int size, char *data));
PROTOTYPE(char *snarf_string, (int pid, void *addr));
PROTOTYPE(char *snarf_data, (int pid, void *addr, int size));
PROTOTYPE(char *format_value,
          (int pid, fmt_type format, unsigned long value, int opt));
PROTOTYPE(int printable_data, (char *data, int size));
PROTOTYPE(char *print_string, (char *data, int size));


/*
 * Global definitions.
 */

static char *trap_msg = NULL;
static fmt_type result_format;
static int no_return;
static fmt_type post_fmt;
static unsigned long post_value;
static int post_size;


/*
 * Report the occurence of the specified trap.
 */

void report_trap(pid, addr, trap, g1, params_addr)
    int pid;
    void *addr;
    int trap;
    int g1;
    syscall_params *params_addr;
{
    syscall_params params;
    call_desc *call;
    int i;
    fmt_type arg_format;
    char *arg_str;

    /*
     * Display any previous trap message that is still pending (it might have
     * been a trap that did not return a value, and so has not yet been
     * displayed).
     */

    display_trap_msg();

    /*
     * Read the parameters, and construct a string describing the system call.
     */

    ensure(ptrace(PTRACE_READDATA, pid,
                  (char *) params_addr, sizeof(syscall_params),
                  (char *) params) != -1);

    no_return = 0;

    if (trap != T_SOFTWARE_TRAP) {

        /*
         * Not a system call trap.
         */

        no_return = 1;

        ensure((trap_msg = malloc(17 + 20 + 1)) != NULL);
        sprintf(trap_msg, "0x%08lx: trap %d", (unsigned long) addr, trap);

        result_format = fmt_unknown;
    } if ((g1 < 0) || (g1 >= no_system_calls)) {

        /*
         * An unknown system call.
         */

        ensure((trap_msg = malloc(21 + 20 + 1)) != NULL);
        sprintf(trap_msg, "0x%08lx: _unknown_%d(",
                (unsigned long) addr, g1);

        arg_str = format_value(pid, fmt_unknown, params[0], 0);
        ensure((trap_msg = realloc(trap_msg, strlen(trap_msg)
                                             + strlen(arg_str) + 1 + 1))
               != NULL);
        sprintf(trap_msg + sizeof(trap_msg), "%s)", arg_str);
        free(arg_str);

        result_format = fmt_unknown;
    } else {

        /*
         * A known system call.
         */

        call = &system_calls[g1];
        switch (g1) {
            case SYS_open :
                if (!(params[1] & O_CREAT)) {
                    call = &system_call_open_simple;
                }
                break;
            case SYS_exit :
            case SYS_execve :
            case SYS_sigcleanup :
                no_return = 1;
                break;
            default :
                break;
        }

        ensure((trap_msg = malloc(13 + strlen(call->name) + 1 + 1))
               != NULL);
        sprintf(trap_msg, "0x%08lx: %s(",
                (unsigned long) addr, call->name);

        /*
         * Display each of the arguments.
         */

        for (i = 0; i < NO_PARAMS; i++) {
            if ((arg_format = call->arg[i]) == fmt_none) {
                break;
            }
            if (i > 0) {
                strcat(trap_msg, ", ");
            }
            if (arg_format == fmt_data) {
                assert(((i + 1) < NO_PARAMS) &&
                       (call->arg[i + 1] == fmt_data_size));
                arg_str = format_value(pid, arg_format,
                                       params[i], (int) params[i + 1]);
            } else {
                arg_str = format_value(pid, arg_format, params[i], 0);
            }
            ensure((trap_msg = realloc(trap_msg, strlen(trap_msg) +
                                                 strlen(arg_str) + 2 + 1))
                   != NULL);
            strcat(trap_msg, arg_str);
            free(arg_str);
        }

        strcat(trap_msg, ")");

        result_format = call->result;
    }

    /*
     * Set alarm so that name of call will be displayed even if it blocks.
     */

    alarm((unsigned int) 1);
}


/*
 * Report the value returned as a result of the most recent trap.
 */

void report_trap_result(pid, error, o0, o1)
    int pid;
    int error;
    int o0;
    int o1;
{
    char *result, *eno, *emsg, *addr;

    /*
     * Turn off alarm used to ensure we print the call promptly - we are about
     * to print it now.
     */

    alarm((unsigned int) 0);

    /*
     * See if previous call blocked.
     */

    if (trap_msg == NULL) {
        ensure((trap_msg = strdup("            [previous call]")) != NULL);
    }

    /*
     * Work out error message (if any) to be printed following return value.
     */

    if (error) {
        eno = format_value(pid, fmt_error, o0, 0);
        ensure((emsg = malloc(9 + strlen(eno) + 1)) != NULL);
        sprintf(emsg, " [error %s]", eno);
        free(eno);
        o0 = -1;
        post_fmt = fmt_none;
    } else {
        ensure((emsg = strdup("")) != NULL);
    }

    /*
     * Print out all the details of the system call.
     */

    if (result_format == fmt_none) {
        ensure(fprintf(msgfile, "%s: %s%s\n", trace_progname, trap_msg, emsg)
               != EOF);
    } else {
        result = format_value(pid, result_format, o0, 0);
        ensure(fprintf(msgfile, "%s: %s -> %s%s\n",
                       trace_progname, trap_msg, result, emsg) != EOF);
        free(result);
    }

    free(emsg);

    /*
     * Display any string or buffer modified by the system call if required.
     * And providing it can be displayed as a (non-null) string.
     */

    if (post_fmt != fmt_none) {
        result = format_value(pid, post_fmt, post_value, post_size);
        if ((result[0] == '"') && (strlen(result) > 2)) {
            addr = format_value(pid, fmt_ptr, post_value, 0);
            ensure(fprintf(msgfile, "%s: %s:     %s\n",
                           trace_progname, addr, result) != EOF);
            free(addr);
        }
        free(result);
        post_fmt = fmt_none;
    }

    free(trap_msg);
    trap_msg = NULL;
}


/*
 * Report any trap messages that haven't been reported yet.
 */

void display_trap_msg() {

    /*
     * Clear the alarm - we are about to print the message.
     */

    alarm((unsigned int) 0);

    if (trap_msg != NULL) {
        ensure(fprintf(msgfile, "%s: %s\n", trace_progname, trap_msg) != EOF);
        free(trap_msg);
        trap_msg = NULL;
    }
}


/*
 * Report the completion of a trap message as being delayed.
 *
 * This routine is invoked when a SIGALRM is received.
 */

void delayed_trap_msg() {

    assert(trap_msg != NULL);

    /*
     * If the call was not expected to return a value, think nothing of it,
     * otherwise assume the call has blocked.
     */

    ensure(fprintf(msgfile, "%s: %s%s\n",
                   trace_progname, trap_msg, (no_return ? "" : " [pending]"))
           != EOF);
    free(trap_msg);
    trap_msg = NULL;
}


/*
 * Discard any pending trap messages.
 *
 * This routine is used by the child of a fork to discard the fork system call
 * record.
 */

void discard_trap_msg() {

    trap_msg = NULL;
}


/*
 * Attempt to copy size bytes from the target process to data.  The number of
 * bytes successfully copied is returned.
 */

int copy_memory(pid, addr, size, data)
    int pid;
    void *addr;
    int size;
    char *data;
{
    int lo, hi, try;

    assert(size >= 0);

    /*
     * Common cases first.
     */

    if (ptrace(PTRACE_READDATA, pid, (char *) addr, size, data) != -1) {
        return size;
    } else if (ptrace(PTRACE_READDATA, pid, (char *) addr, 1, data) == -1) {
        return 0;
    }

    /*
     * Binary search.
     */

    lo = 1;
    hi = size - 1;

    while (lo < hi) {
        try = (lo + hi + 1) / 2;
        if (ptrace(PTRACE_READDATA, pid, (char *) addr, try, data) != -1) {
            lo = try;
        } else {
            hi = try - 1;
        }
    }

    ensure(ptrace(PTRACE_READDATA, pid, (char *) addr, lo, data) != -1);

    return lo;
}


/*
 * Create a string representing the contents of the indicated null termintated
 * region of memory.
 */

char *snarf_string(pid, addr)
    int pid;
    void *addr;
{
    char data[STRING_SIZE_LIMIT + 1];
    int size, len;
    char *result = NULL;
    int too_long = 0;

    size = copy_memory(pid, addr, STRING_SIZE_LIMIT, data);
    data[size] = '\0';
    len = strlen(data);
    too_long = (len == STRING_SIZE_LIMIT);
    if ((len < size) || too_long) {
        if (printable_data(data, len)) {
            result = print_string(data, len);
            if (too_long) {
                ensure((result = realloc(result, strlen(result) + 2 + 1))
                       != NULL);
                strcat(result, "..");
            }
        }
    }

    return result;
}


/*
 * Create a string representing the contents of the indicated length delimited
 * region of memory.
 */

char *snarf_data(pid, addr, size)
    int pid;
    void *addr;
    int size;
{
    char data[DATA_SIZE_LIMIT];
    char *result = NULL;
    int too_long = 0;

    if (size > DATA_SIZE_LIMIT) {
        size = DATA_SIZE_LIMIT;
        too_long = 1;
    }
    if ((size >= 0) && (copy_memory(pid, addr, size, data) == size)) {
        if (printable_data(data, size)) {
            result = print_string(data, size);
            if (too_long) {
                ensure((result = realloc(result, strlen(result) + 2 + 1))
                       != NULL);
                strcat(result, "..");
            }
        }
    }

    return result;
}


/*
 * Create a string representing the contents of the indicated null termintated
 * array of pointers to null terminated regions of memory.
 */

char *snarf_string_array(pid, addr)
    int pid;
    void *addr;
{
    char *data[ARRAY_SIZE_LIMIT + 1];
    int size, len, i;
    char *result = NULL;
    char *s;
    int too_long = 0;

    size = copy_memory(pid, addr, ARRAY_SIZE_LIMIT * sizeof(char *),
                       (char *) data) / sizeof(char *);
    data[size] = NULL;
    for (len = 0; data[len] != NULL; len++) {
    }
    too_long = (len == ARRAY_SIZE_LIMIT);
    if ((len < size) || too_long) {
        ensure((result = strdup("{")) != NULL);
        for (i = 0; i < len; i++) {
            if (i > 0) {
                strcat(result, ", ");
            }
            s = format_value(pid, fmt_string, (unsigned long) data[i], 0);
            ensure((result = realloc(result,
                                     strlen(result) + strlen(s) + 2 + 5 + 1))
                   != NULL);
            strcat(result, s);
        }
        if (too_long) {
            strcat(result, ", ..");
        }
        strcat(result, "}");
    }

    return result;
}


/*
 * Return a string containing a value printed in a specific format.  Opt is a
 * second optional parameter currently only used to contain the size to be used
 * with fmt_data.
 */

char *format_value(pid, format, value, opt)
    int pid;
    fmt_type format;
    unsigned long value;
    int opt;
{
    char *str;
    int sig, error;

    /*
     * See if we are meant to hang on to the value for later use.
     */

    switch (format) {

        case fmt_post_string :
            post_fmt = fmt_string ;
            post_value = value;
            format = fmt_ptr;
            break;

        case fmt_post_data :
            post_fmt = fmt_data;
            post_value = value;
            format = fmt_ptr;
            break;

        case fmt_data_size :
            format = FMT_SIZE;
            break;

        case fmt_post_data_size :
            post_size = (int) value;
            format = FMT_SIZE;
            break;

        default :
            break;
    }

    /*
     * Display the value.
     */

    switch (format) {

        case fmt_dec :

            ensure((str = malloc(20 + 1)) != NULL);
            sprintf(str, "%d", (int) value);
            break;

        case fmt_hex :

            ensure((str = malloc(2 + 20 + 1)) != NULL);
            sprintf(str, "0x%lx", value);
            break;

        case fmt_ptr :

            if (value == 0) {
                ensure((str = strdup("NULL")) != NULL);
            } else {
                ensure((str = malloc(10 + 1)) != NULL);
                sprintf(str, "0x%08lx", value);
            }
            break;

        case fmt_fd :

            ensure((str = malloc(2 + 20 + 1)) != NULL);
            sprintf(str, "fd%d", (int) value);
            break;

        case fmt_signal :

            sig = (int) value;
            if ((sig < 0) || (sig >= no_signal_names)) {
                ensure((str = malloc(20 + 1)) != NULL);
                sprintf(str, "%d", sig);
            } else {
                ensure((str = strdup(signal_names[sig])) != NULL);
            }
            break;

        case fmt_error :

            error = (int) value;
            if ((error < 0) || (error >= no_error_names)) {
                ensure((str = malloc(20 + 1)) != NULL);
                sprintf(str, "%d", error);
            } else {
                ensure((str = strdup(error_names[error])) != NULL);
            }
            break;

        case fmt_open_flags :

            ensure((str = malloc(8 + 3 + 20 + 1)) != NULL);
            switch (value & 3) {
                case O_RDONLY :
                    sprintf(str, "O_RDONLY");
                    value -= O_RDONLY;
                    break;
                case O_WRONLY :
                    sprintf(str, "O_WRONLY");
                    value -= O_WRONLY;
                    break;
                case O_RDWR :
                    sprintf(str, "O_RDWR");
                    value -= O_RDWR;
                    break;
                default :
                    sprintf(str, "0x%lx", value);
                    value = 0;
                    break;
            }
            if (value != 0) {
                sprintf(str + strlen(str), "|0x%lx", value);
            }
            break;

        case fmt_unknown :

            ensure((str = strdup("..")) != NULL);
            break;

        case fmt_string :

            if ((str = snarf_string(pid, (void *) value)) == NULL) {
                str = format_value(pid, fmt_ptr, value, 0);
            }
            break;

        case fmt_data :

            if ((str = snarf_data(pid, (void *) value, opt)) == NULL) {
                str = format_value(pid, fmt_ptr, value, 0);
            }
            break;

        case fmt_string_array :

            if ((str = snarf_string_array(pid, (void *) value)) == NULL) {
                str = format_value(pid, fmt_ptr, value, 0);
            }
            break;

        default :

            diagnose("Unexpected display format");
            break;
    }

    return str;
}


/*
 * Determine whether size bytes of data are printable.
 */

int printable_data(data, size)
    char *data;
    int size;
{
    int i;

    for (i = 0; i < size; i++) {

        if (!(isprint(data[i]))) {

            switch (data[i]) {

                case '\0' :
                case '\t' :
                case '\n' :
                case '\f' :
                case '\r' :
                    break;

                default :
                    return 0;
                    break;
            }
        }
    }

    return 1;
}


/*
 * Create a string representing size bytes of data.
 */

char *print_string(data, size)
    char *data;
    int size;
{
    char *str, *s;
    int i;

    assert(size >= 0);

    ensure((str = malloc(1 + size * 2 + 1 + 1)) != NULL);
    s = str;

    *(s++) = '"';

    for (i = 0; i < size; i++) {

        if ((!(isprint(data[i]))) || (data[i] == '"') || (data[i] == '\\')) {

            *(s++) = '\\';

            switch (data[i]) {
                case '\0' :
                    *(s++) = '0';
                    break;
                case '\t' :
                    *(s++) = 't';
                    break;
                case '\n' :
                    *(s++) = 'n';
                    break;
                case '\f' :
                    *(s++) = 'f';
                    break;
                case '\r' :
                    *(s++) = 'r';
                    break;
                case '"' :
                case '\\' :
                    *(s++) = data[i];
                    break;
                default :
                    diagnose("Attempted to display illegal character");
            }
        } else {

            *(s++) = data[i];
        }
    }

    *(s++) = '"';
    *s = '\0';

    return str;
}
