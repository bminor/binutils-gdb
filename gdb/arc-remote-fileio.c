/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Author:
      Richard Stuckey <richard.stuckey@arc.com>

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

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module implements facilities for intercepting I/O (and other)     */
/*     operations attempted on an ARC target and performing them on the host, */
/*     using a RPC (Remote Procedure Call) mechanism.                         */
/*                                                                            */
/* Mechanism:                                                                 */
/*     When interception is enabled, this module sets a breakpoint at the     */
/*     entry point of each operation which is to be intercepted; it finds the */
/*     entry points of named routines in the C runtime library code by using  */
/*     gdb's symbol lookup facilities.                                        */
/*                                                                            */
/*     When a breakpoint is triggered on the target, the target monitoring    */
/*     loop calls the function 'arc_check_interception_breakpoint' to check   */
/*     whether the breakpoint triggered is an interception breakpoint; this   */
/*     function will return a code indicating either                          */
/*                                                                            */
/*       a) the breakpoint is an interception breakpoint, the interception    */
/*          has been performed and execution of the target program should be  */
/*          resumed; or                                                       */
/*                                                                            */
/*       b) the breakpoint is an interception breakpoint, but the intercepted */
/*          routine is 'exit' and execution should not be resumed; or         */
/*                                                                            */
/*       c) the breakpoint is not an interception breakpoint, so execution    */
/*          should not be resumed and the trigger should be reported to gdb.  */
/*                                                                            */
/*     In case a), this module then reads the routine's parameters from the   */
/*     target's registers, performs whatever conversions are required, and    */
/*     constructs a gdb RSP File I/O extension 'F' message which it passes to */
/*     the gdb target_fileio module, which performs the requested operation   */
/*     on the host machine.                                                   */
/*                                                                            */
/*     The target_fileio module is passed a set of operations which allow it  */
/*     to read data from target memory, write data to target memory, and      */
/*     return a result value (and possibly a error code) to the intercepted   */
/*     routine.  The result value is written into the target's R0 register;   */
/*     the error code (if any) is written into the location of the 'errno'    */
/*     variable.                                                              */
/*                                                                            */
/*     Finally, this module copies the routine return address from the BLINK  */
/*     register to the PC register - this ensures that when execution of the  */
/*     target is resumed, control returns to the code after the call to the   */
/*     intercepted routine.                                                   */
/*                                                                            */
/* Notes:                                                                     */
/*     1) the set of routines to be intercepted, and the parameters to these  */
/*        routines, is defined by a table (see below) - so it is simple to    */
/*        add more routines to the set;                                       */
/*                                                                            */
/*     2) the 'open' syscall (see man open(2)) has a 'flags' parameter which  */
/*        is a bit mask; unfortunately, the bits differ in meaning between    */
/*        the host and the elf-32 target program, so the parameter must be    */
/*        converted before it can be passed to the target_fileio module;      */
/*                                                                            */
/*     3) the 'fstat' syscall (see man fstat(2)) has an out parameter which   */
/*        is a 'struct stat' structure (i.e. the address of such a structure  */
/*        is a parameter to the syscall): the target_fileio module writes the */
/*        data of the structure to that location in target memory; however,   */
/*        the structure does not have the same layout on host and target, so  */
/*        the structure must be converted before it can be written to the     */
/*        target;                                                             */
/*                                                                            */
/*     4) the interception breakpoints are not handled by the core gdb        */
/*        breakpoint mechanism; hence, they are not listed by the 'info break'*/
/*        command, and can not be (accidentally) deleted by the user; though  */
/*        they could be handled by gdb, that would require the introduction   */
/*        of a new "invisible" breakpoint type, and hence more changes to     */
/*        supposedly generic code;                                            */
/*                                                                            */
/*     5) it would be more elegant (from one perspective) to intecept these   */
/*        operations by placing a breakpoint at the interrupt vector location */
/*        of the 'swi' (SoftWare Interrupt) handler: only one breakpoint      */
/*        would then be required, and all syscalls would be intercepted;      */
/*        however, this module would then have to simulate a "return from     */
/*        exception" in order to resume target execution, which would be more */
/*        complex than the "restart at return address" method currently used. */
/*                                                                            */
/* Restrictions:                                                              */
/*     1) this module places s/w breakpoints at the entry points; therefore,  */
/*        the mechanism will not work with programs that are loaded into read */
/*        -only memory;                                                       */
/*                                                                            */
/*     2) this mechanism will probably not work if the user sets breakpoints  */
/*        on the entry points of the intercepted routines - there will be a   */
/*        conflict!                                                           */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <stdio.h>
#include <string.h>
#include <signal.h>

/* gdb header files */
#include "defs.h"
#include "symtab.h"
#include "frame.h"
#include "block.h"
#include "target.h"
#include "target-fileio.h"
#include "exceptions.h"
#include "gdb/fileio.h"

/* ARC header files */
#include "arc-remote-fileio.h"
#include "config/arc/tm-embed.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

#define MAX_SYSCALL_PARAMS    4

/* These are the intercepted routines.  */
typedef enum
{
    READ_CALL,
    WRITE_CALL,
    OPEN_CALL,
    CLOSE_CALL,
    LSEEK_CALL,
    FSTAT_CALL,
    GETTIMEOFDAY_CALL,
    EXIT_CALL
} SystemCall;


struct lib_function
{
    SystemCall            call;
    const char           *name;
    const char           *format;
    Boolean               bp_is_set;
    unsigned int          param_count;
    ARC_RegisterNumber    param_register[MAX_SYSCALL_PARAMS];
    struct bp_target_info breakpoint;
};


/* This structure defines a memory image of the 'stat' structure as it is
   represented on the ARC target.  */
struct arc_stat
{
    ARC_Byte fst_dev    [4];
    ARC_Byte fst_ino    [4];
    ARC_Byte fst_mode   [2];
    ARC_Byte fst_nlink  [2];
    ARC_Byte fst_uid    [2];
    ARC_Byte fst_gid    [2];
    ARC_Byte fst_rdev   [4];
    ARC_Byte fst_size   [4];
    ARC_Byte fst_blksize[4];
    ARC_Byte fst_blocks [4];
    ARC_Byte fst_atime  [8];
    ARC_Byte fst_mtime  [8];
    ARC_Byte fst_ctime  [8];
};


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

/* The %x specifiers in the format strings in this table correspond to the
   parameters to the intercepted functions; the register number of the target
   register in which the parameter is passed is given by the corresponding
   entry in the param_register array.

   N.B. the special value of SL as the number of the register in which a
        parameter is passed indicates that the preceding parameter was the
        address of a string, and the length of that string (including the
        terminating NUL) is required here.

        F<n> indicates that the parameter in register <n> is a word of flag
        bits which must be handled specially!

        X indicates that the array element is not used.  */

#define SL   1000
#define F2   1002
#define X    9999

static struct lib_function functions[] =
{
    { READ_CALL,         "_read_r",         "Fread,%x,%x,%x",       FALSE, 3, {1, 2,   3, X} },
    { WRITE_CALL,        "_write_r",        "Fwrite,%x,%x,%x",      FALSE, 3, {1, 2,   3, X} },
    { OPEN_CALL,         "_open_r",         "Fopen,%x/%x,%x,%x",    FALSE, 4, {1, SL, F2, 3} },
    { CLOSE_CALL,        "_close_r",        "Fclose,%x",            FALSE, 1, {1, X,   X, X} },
    { LSEEK_CALL,        "_lseek_r",        "Flseek,%x,%x,%x",      FALSE, 3, {1, 2,   3, X} },
    { FSTAT_CALL,        "_fstat_r",        "Ffstat,%x,%x",         FALSE, 2, {1, 2,   X, X} },
    { GETTIMEOFDAY_CALL, "_gettimeofday_r", "Fgettimeofday,%x,%x",  FALSE, 2, {1, 2,   X, X} },
    { EXIT_CALL,         "_exit_r",         NULL,                   FALSE, 1, {1, X,   X, X} }
};


/* A pointer to the set of target operations for the current target.  */
static TargetOperations *target_operations;

/* TRUE if the operation currently being intercepted has NOT been interrupted
   by the user typing a Ctrl-C.  */
static Boolean not_interrupted;

/* For the Ctrl-C signal handler.  */
static void (*old_signal_handler) (int);


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* Read a number of bytes of data from the given address in target memory.  */

static int
read_bytes (CORE_ADDR memaddr, gdb_byte *myaddr, int len)
{
    DEBUG("reading %d bytes from %x\n", len, (unsigned int) memaddr);

    return (int) target_read (&current_target, TARGET_OBJECT_MEMORY, NULL, myaddr, memaddr, len);
}


/* Read a number of bytes of data from the given address in target memory.  */

static int
write_bytes (CORE_ADDR memaddr, gdb_byte *myaddr, int len)
{
    DEBUG("writing %d bytes to %x\n", len, (unsigned int) memaddr);

    return (int) target_write (&current_target, TARGET_OBJECT_MEMORY, NULL, myaddr, memaddr, len);
}


/* Perform the reply to the intercepted operation: set up the result of the call
   and the error code (if any) so that the intercepted operation receives them
   just as though the operation had really been performed upon the target.  */

static void
reply (int retcode, int error)
{
    /* Ignore any Ctrl-Cs while performing the reply.  */
    (void) signal (SIGINT, SIG_IGN);

    DEBUG("reply: retcode = %d, error = %d\n", retcode, error);

    /* If an error has occurred.  */
    if (retcode == -1)
    {
        ARC_RegisterContents errno_address;

        if (error == FILEIO_EINTR)
        {
            DEBUG("*** interrupted by user!\n");
            /* Set the global flag so that it can be tested later.  */
            not_interrupted = FALSE;
            return;
        }

        /* Read the address of the 'errno' variable from R0.  */
        if (target_operations->read_core_register(0, &errno_address, TRUE))
            /* Write the error number into the 'errno' variable.  */
            (void) write_bytes((CORE_ADDR) errno_address, (gdb_byte*) &error, BYTES_IN_WORD);
    }

    /* Write the return code into the function result register R0.  */
    (void) target_operations->write_core_register(0, (ARC_RegisterContents) retcode, TRUE);
}


/* Copy a number of bytes of data from one buffer to another.  Note that the
   buffers are not necessarily of the same size. Perform endianess byte-swapping
   if necessary.  */

static void
copy_bytes (char     *from, size_t from_size,
            ARC_Byte *to,   size_t to_size,
            Boolean   target_is_big_endian)
{
    /* We can not copy more data than we have been given in the source buffer,
       or for which there is room in the destination buffer.  */
    unsigned int bytes = (unsigned int) ((from_size > to_size) ? to_size : from_size);
    unsigned int i;

    /* N.B. 1) the fio_stat structure created by target-fileio.c has the values
               in big-endian byte order; so if the ARC target is little-endian
               we must reverse the order;

            2) the fields in the fio_stat structure may be smaller (or larger!)
               than the corresponding fields in the ARC target structure - so we
               copy the *least* significant bytes of the fields, on the grounds
               that the most significant bytes are probably just sign-extensions!  */
    for (i = 0; i < bytes; i++)
        to[i] = (ARC_Byte) ((target_is_big_endian) ? from[i]
                                                   : from[from_size - 1 - i]);
}


/* Write a 'stat' structure to the target at a given address in memory.
   'myaddr' points to a fio_stat structure created by the target-fileio module;
   this structure is meant for use in the RSP protocol, and is designed for
   independence of host/target systems - therefore, we must create an equivalent
   structure which is ARC-specific, and write that structure to the target.

   Return the number of bytes of data written.  */

static int
write_fstat (CORE_ADDR memaddr, gdb_byte *myaddr, int len)
{
    Boolean          target_is_big_endian = (gdbarch_byte_order (current_gdbarch) == BFD_ENDIAN_BIG);
    struct fio_stat *fst                  = (struct fio_stat*) myaddr;
    struct arc_stat  ast;

    memset(&ast, 0, sizeof(ast));

#define COPY(from, to)   copy_bytes(from, sizeof(from), to, sizeof(to), target_is_big_endian)

    COPY(fst->fst_dev,     ast.fst_dev);
    COPY(fst->fst_ino,     ast.fst_ino);
    COPY(fst->fst_mode,    ast.fst_mode);
    COPY(fst->fst_nlink,   ast.fst_nlink);
    COPY(fst->fst_uid,     ast.fst_uid);
    COPY(fst->fst_gid,     ast.fst_gid);
    COPY(fst->fst_rdev,    ast.fst_rdev);
    COPY(fst->fst_size,    ast.fst_size);
    COPY(fst->fst_blksize, ast.fst_blksize);
    COPY(fst->fst_blocks,  ast.fst_blocks);
    COPY(fst->fst_atime,   ast.fst_atime);
    COPY(fst->fst_mtime,   ast.fst_mtime);
    COPY(fst->fst_ctime,   ast.fst_ctime);

    return write_bytes(memaddr, (gdb_byte*) &ast, (int) sizeof(ast));
}


/* Find the address of the entry point of the given routine in the target program.
   Return 0 if the entry point can not be found, or is not a function.  */

static CORE_ADDR
findEntryPoint (const char *routine)
{
    CORE_ADDR      entry = 0;
    int            is_a_field_of_this;
    struct symbol *sym = lookup_symbol (routine,
                                        get_selected_block (0),
                                        VAR_DOMAIN,
                                        &is_a_field_of_this,
                                        (struct symtab **) NULL);
    if (sym)
    {
        if (SYMBOL_CLASS (sym) == LOC_BLOCK)
            entry = BLOCK_START (SYMBOL_BLOCK_VALUE (sym));
        else
            warning(_("%s is not a function"), routine);
    }
    else
        warning(_("can not find entry point of function %s"), routine);

    return entry;
}


/* Insert a s/w breakpoint in the target program code at the entry point of the
   given library function.  */

static void
insert_breakpoint (struct lib_function *f)
{
    if (!f->bp_is_set)
    {
        if (arc_elf32_insert_breakpoint(&f->breakpoint) == 0)
            f->bp_is_set = TRUE;
        else
            warning(_("can not set breakpoint at entrypoint of %s"), f->name);
    }
}


/* Remove a s/w breakpoint from the target program code at the entry point of the
   given library function.  */

static void
remove_breakpoint (struct lib_function *f)
{
    if (f->bp_is_set)
    {
        if (arc_elf32_remove_breakpoint(&f->breakpoint) == 0)
            f->bp_is_set = FALSE;
        else
            warning(_("can not unset breakpoint at entrypoint of %s"), f->name);
    }
}


/* This function handles any Ctrl-C typed by the user whilst the interception of
   an operation is in progress.  */

static void
Ctrl_C_signal_handler (int signo)
{
    /* Ignore any more Ctrl-Cs.  */
    (void) signal (SIGINT, SIG_IGN);

    /* We must use the gdb exception mechanism since the target_fileio_request
       function calls catch_exceptions, and if we do something else (like a long
       jump) here, gdb's cleanup list would be left in an inconsistent state!  */
    DEBUG("*** throwing RETURN_QUIT...\n");
    deprecated_throw_reason (RETURN_QUIT);
}


/* This function is called from the gdb target-fileio module: it sets up this
   module's handler for Ctrl-C interrupts.  */

static void
set_Ctrl_C_signal_handler (void)
{
    old_signal_handler = signal (SIGINT, Ctrl_C_signal_handler);
}


/* This function finds the length of a C string stored in target memory at the
   given address.  */

static unsigned int
find_string_length (ARC_Address address)
{
    unsigned int length = 0;

    while (TRUE)
    {
        gdb_byte buf[65];
        int      bytes = read_bytes((CORE_ADDR) address,
                                    buf,
                                    (int) sizeof(buf) - 1);
        int      i;

        for (i = 0; i < bytes; i++)
            if (buf[i] == (gdb_byte) '\0')
                return (unsigned int) (length + i + 1);

        address += bytes;
        length  += bytes;
    }
}


/* Convert flags to target syscall to what they "should" be!  */

static ARC_RegisterContents
convert_flags (ARC_RegisterContents flags)
{
    ARC_RegisterContents result = flags;

/* See gcc/src/newlib/libc/sys/arc/sys/fcntl.h */

/* The following values have been changed for uclibc compatibility.  */
#define _FAPPEND        0x0400  /* append (writes guaranteed at the end) */
#define _FASYNC         0x2000  /* signal pgrp when data ready */
#define _FCREAT         0x0040  /* open with file create */
#define _FTRUNC         0x0200  /* open with truncation */
#define _FEXCL          0x0080  /* error on open if file exists */
#define _FSYNC          0x1000  /* do all writes synchronously */
#define _FNONBLOCK      0x0800  /* non blocking I/O (POSIX style) */

#define REMOVE(flag)    if (flags & _F ## flag) result &= ~ _F        ## flag
#define ADD(flag)       if (flags & _F ## flag) result |=   FILEIO_O_ ## flag

    /* N.B. all "old" bits most be removed from the result word before all
            "new" bits are added, in case the old and new sets intersect!  */
    REMOVE(APPEND);
//  REMOVE(ASYNC);     // no equivalent flag in gdb/fileio.h
    REMOVE(CREAT);
    REMOVE(TRUNC);
    REMOVE(EXCL);
//  REMOVE(SYNC);      // no equivalent flag in gdb/fileio.h
//  REMOVE(NONBLOCK);  // no equivalent flag in gdb/fileio.h
    ADD(APPEND);
//  ADD(ASYNC);        // no equivalent flag in gdb/fileio.h
    ADD(CREAT);
    ADD(TRUNC);
    ADD(EXCL);
//  ADD(SYNC);         // no equivalent flag in gdb/fileio.h
//  ADD(NONBLOCK);     // no equivalent flag in gdb/fileio.h

    return result;
}


/* Perform the interception of the given library function.
   Return TRUE if the interception is completed successfully,
          FALSE if it is interrupted by the user.  */

static Boolean
perform_interception (struct lib_function *f)
{
    ARC_RegisterContents params [MAX_SYSCALL_PARAMS];
    char                 request[MAX_SYSCALL_PARAMS * 9 + 40];
    unsigned int         i;

    /* These operations allow the target_fileio module to read data from target
       memory, write data to target memory, and return a result value (and
       possibly a error code) to the intercepted routine.

       N.B. if the syscsall is 'fstat', we pass a special write function
            which converts the 'struct stat' structure to target layout before
            writing it to target memory.  */
    struct file_io_operations io_operations =
    {
        read_bytes,
        (f->call == FSTAT_CALL) ? write_fstat : write_bytes,
        reply,
        set_Ctrl_C_signal_handler
    };

    /* Evaluate the parameters to be passed to the RPC request.  */
    for (i = 0; i < f->param_count; i++)
    {
        ARC_RegisterNumber reg = f->param_register[i];

        if (reg == SL)
            params[i] = find_string_length((ARC_Address) params[i - 1]);
        else if (reg == F2)
        {
            ARC_RegisterContents flags;

            (void) target_operations->read_core_register(2, &flags, TRUE);
            params[i] = convert_flags(flags);
        }
        else
            (void) target_operations->read_core_register(reg, &params[i], TRUE);
    }

    /* Do not close the target program's stdin, stdout or stderr streams on the
       host: it is possible that the program may be re-loaded and re-run on the
       target in the same debugging session (so re-initializing its I/O system)
       so it may try to read/write those streams again - instead, just tell the
       target that the close succeeded.  */
    if (f->call == CLOSE_CALL)
    {
        int fd = (int) params[0];

        if (fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO)
        {
            reply(0, 0);
            DEBUG("*** RPC close of stream %d ignored\n", fd);
            return TRUE;
        }
    }

    /* Parameters which are extra to those required by the format will simply be
       ignored.  */
    (void) snprintf(request, sizeof(request), f->format,
                    params[0], params[1], params[2], params[3]);

    DEBUG("RPC request: %s\n", request);

    /* the interception might be interrupted by the user typing Ctrl-C whilst
       the interception is in progress; if that happens, this flag will be set
       to FALSE.  */
    not_interrupted = TRUE;

    /* Make the RPC request.  */
    target_fileio_request(request, &io_operations);

    (void) signal (SIGINT, old_signal_handler);

    /* If the call was not interrupted, the interception has been performed.  */
    return not_interrupted;
}


/* -------------------------------------------------------------------------- */
/*                         externally visible functions                       */
/* -------------------------------------------------------------------------- */

/* Set the state of the I/O interception mechanism:
      ON   : set breakpoints on all the functions to be intercepted
      OFF  : clear breakpoints from all the intercepted functions
      RESET: mark the breakpoints as not being set (if a new program has been
             downloaded to the target, the s/w breakpoints in the old program
             have been lost, and so should not be removed).  */

void
arc_set_IO_interception (TargetOperations *operations,
                         InterceptionState state)
{
    unsigned int i;

    DEBUG("*** interception: %s\n", (state == INTERCEPTION_RESET) ? "RESET" :
                                    (state == INTERCEPTION_ON)    ? "ON"    :
                                                                    "OFF");

    target_operations = operations;

    for (i = 0; i < ELEMENTS_IN_ARRAY(functions); i++)
    {
        struct lib_function* f = &functions[i];

        switch (state)
        {
            case INTERCEPTION_RESET:
                f->bp_is_set = FALSE;
                break;

            case INTERCEPTION_ON:
                /* Set a breakpoint on the entry point of the function.  */
                f->breakpoint.placed_address = findEntryPoint(f->name);

                if (f->breakpoint.placed_address != 0)
                {
                    DEBUG("intercept 0x%08X : %s\n", (unsigned int) f->breakpoint.placed_address, f->name);
                    insert_breakpoint(f);
                }
                break;

            case INTERCEPTION_OFF:
                if (f->breakpoint.placed_address != 0)
                {
                    remove_breakpoint(f);
                    f->breakpoint.placed_address = 0;
                }
                break;
        }
    }
}


/* This function is called when the execution of the target program has been
   halted by a breakpoint trigger. It checks whether the breakpoint that has
   been triggered is at the entry point of an intercepted function, and, if so,
   performs the required interception.

   Returns:
      INTERCEPTION_RESUME    : interception has been performed, execution should be resumed
      INTERCEPTION_HALT      : the program is halted (no interception has been performed)
      INTERCEPTION_EXIT      : the program has exited
      INTERCEPTION_INTERRUPT : the interception has been interrupted by the user

  If the program has exited, the 'exitcode' parameter is set to the program's exit code.  */

InterceptionAction
arc_check_interception_breakpoint (int *exitcode)
{
    ARC_RegisterContents pc;

    ENTERMSG;

    *exitcode = 0;

    /* Get the current execution point from the PCL, rather than the PC - this
       gives the same result on both ARC700 and ARC600 targets.  */
    if (target_operations->read_core_register(ARC_PCL_REGNUM, &pc, TRUE))
    {
        unsigned int i;

        DEBUG("checking for interception at 0x%08X\n", pc);

        /* Look at each of the intercepted operations.  */
        for (i = 0; i < ELEMENTS_IN_ARRAY(functions); i++)
        {
            struct lib_function *f = &functions[i];

            if (f->breakpoint.placed_address == (CORE_ADDR) pc)
            {
                DEBUG("intercepted function %s\n", f->name);

                if (f->call == EXIT_CALL)
                {
                    ARC_RegisterContents code;

                    /* The exit code is in parameter register R1.  */
                    if (target_operations->read_core_register(1, &code, TRUE))
                        *exitcode = (int) code;

                    return INTERCEPTION_EXIT;
                }
                else
                {
                    /* If the interception is performed.  */
                    if (perform_interception(f))
                    {
                        ARC_RegisterContents blink;

                        /* Copy BLINK to PC, so that when execution is re-started,
                           control will return to the point after the call of the
                           intercepted function.  */
                        if (target_operations->read_core_register      (ARC_BLINK_REGNUM, &blink, TRUE) &&
                            target_operations->write_auxiliary_register(arc_pc_regnum,     blink, TRUE))
                        {
                            DEBUG("copied BLINK (%x) to PC (was %x)\n", blink, pc);
                            return INTERCEPTION_RESUME;
                        }

                        /* If we couldn't set PC, fall through.  */
                    }
                    else
                    {
                        /* The interception has been interrupted by a Ctrl-C
                           from the user - do not change the PC, as we want
                           execution to resume at the same point in the code,
                           so that the I/O request will be performed (and
                           intercepted) again: this e.g. allows the user to
                           break into a program that is in a tight loop doing
                           reads or writes.  */
                        return INTERCEPTION_INTERRUPT;
                    }
                }
            }
        }
    }

    return INTERCEPTION_HALT;
}

/******************************************************************************/
