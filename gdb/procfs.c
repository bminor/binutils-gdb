/* Machine independent support for SVR4 /proc (process file system) for GDB.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support.

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


/*			N  O  T  E  S

For information on the details of using /proc consult section proc(4)
in the UNIX System V Release 4 System Administrator's Reference Manual.

The general register and floating point register sets are manipulated by
separate ioctl's.  This file makes the assumption that if FP0_REGNUM is
defined, then support for the floating point register set is desired,
regardless of whether or not the actual target has floating point hardware.

 */


#include "defs.h"

#ifdef USE_PROC_FS	/* Entire file goes away if not using /proc */

#include <time.h>
#include <sys/procfs.h>
#include <fcntl.h>
#include <errno.h>

#include "inferior.h"
#include "target.h"
#include "signame.h"

#define MAX_SYSCALLS	256	/* Maximum number of syscalls for table */

#ifndef PROC_NAME_FMT
#define PROC_NAME_FMT "/proc/%d"
#endif

#if 1	/* FIXME: Gross and ugly hack to resolve coredep.c global */
CORE_ADDR kernel_u_addr;
#endif

#ifdef BROKEN_SIGINFO_H		/* Workaround broken SGS <sys/siginfo.h> */
#undef si_pid
#define si_pid _data._proc.pid
#undef si_uid
#define si_uid _data._proc._pdata._kill.uid
#endif /* BROKEN_SIGINFO_H */

/*  All access to the inferior, either one started by gdb or one that has
    been attached to, is controlled by an instance of a procinfo structure,
    defined below.  Since gdb currently only handles one inferior at a time,
    the procinfo structure for the inferior is statically allocated and
    only one exists at any given time.  There is a separate procinfo
    structure for use by the "info proc" command, so that we can print
    useful information about any random process without interfering with
    the inferior's procinfo information. */

struct procinfo {
  int valid;			/* Nonzero if pid, fd, & pathname are valid */
  int pid;			/* Process ID of inferior */
  int fd;			/* File descriptor for /proc entry */
  char *pathname;		/* Pathname to /proc entry */
  int was_stopped;		/* Nonzero if was stopped prior to attach */
  int nopass_next_sigstop;	/* Don't pass a sigstop on next resume */
  prrun_t prrun;		/* Control state when it is run */
  prstatus_t prstatus;		/* Current process status info */
  gregset_t gregset;		/* General register set */
  fpregset_t fpregset;		/* Floating point register set */
  fltset_t fltset;		/* Current traced hardware fault set */
  sigset_t trace;		/* Current traced signal set */
  sysset_t exitset;		/* Current traced system call exit set */
  sysset_t entryset;		/* Current traced system call entry set */
  fltset_t saved_fltset;	/* Saved traced hardware fault set */
  sigset_t saved_trace;		/* Saved traced signal set */
  sigset_t saved_sighold;	/* Saved held signal set */
  sysset_t saved_exitset;	/* Saved traced system call exit set */
  sysset_t saved_entryset;	/* Saved traced system call entry set */
};

static struct procinfo pi;	/* Inferior's process information */

/*  Much of the information used in the /proc interface, particularly for
    printing status information, is kept as tables of structures of the
    following form.  These tables can be used to map numeric values to
    their symbolic names and to a string that describes their specific use. */

struct trans {
  int value;			/* The numeric value */
  char *name;			/* The equivalent symbolic value */
  char *desc;			/* Short description of value */
};

/*  Translate bits in the pr_flags member of the prstatus structure, into the
    names and desc information. */

static struct trans pr_flag_table[] =
{
#if defined (PR_STOPPED)
  PR_STOPPED, "PR_STOPPED", "Process is stopped",
#endif
#if defined (PR_ISTOP)
  PR_ISTOP, "PR_ISTOP", "Stopped on an event of interest",
#endif
#if defined (PR_DSTOP)
  PR_DSTOP, "PR_DSTOP", "A stop directive is in effect",
#endif
#if defined (PR_ASLEEP)
  PR_ASLEEP, "PR_ASLEEP", "Sleeping in an interruptible system call",
#endif
#if defined (PR_FORK)
  PR_FORK, "PR_FORK", "Inherit-on-fork is in effect",
#endif
#if defined (PR_RLC)
  PR_RLC, "PR_RLC", "Run-on-last-close is in effect",
#endif
#if defined (PR_PTRACE)
  PR_PTRACE, "PR_PTRACE", "Process is being controlled by ptrace",
#endif
#if defined (PR_PCINVAL)
  PR_PCINVAL, "PR_PCINVAL", "PC refers to an invalid virtual address",
#endif
#if defined (PR_ISSYS)
  PR_ISSYS, "PR_ISSYS", "Is a system process",
#endif
#if defined (PR_STEP)
  PR_STEP, "PR_STEP", "Process has single step pending",
#endif
#if defined (PR_KLC)
  PR_KLC, "PR_KLC", "Kill-on-last-close is in effect",
#endif
#if defined (PR_ASYNC)
  PR_ASYNC, "PR_ASYNC", "Asynchronous stop is in effect",
#endif
#if defined (PR_PCOMPAT)
  PR_PCOMPAT, "PR_PCOMPAT", "Ptrace compatibility mode in effect",
#endif
 0, NULL, NULL
};

/*  Translate values in the pr_why field of the prstatus struct. */

static struct trans pr_why_table[] =
{
#if defined (PR_REQUESTED)
 PR_REQUESTED, "PR_REQUESTED", "Directed to stop via PIOCSTOP/PIOCWSTOP",
#endif
#if defined (PR_SIGNALLED)
 PR_SIGNALLED, "PR_SIGNALLED", "Receipt of a traced signal",
#endif
#if defined (PR_FAULTED)
 PR_FAULTED, "PR_FAULTED", "Incurred a traced hardware fault",
#endif
#if defined (PR_SYSENTRY)
 PR_SYSENTRY, "PR_SYSENTRY", "Entry to a traced system call",
#endif
#if defined (PR_SYSEXIT)
 PR_SYSEXIT, "PR_SYSEXIT", "Exit from a traced system call",
#endif
#if defined (PR_JOBCONTROL)
 PR_JOBCONTROL, "PR_JOBCONTROL", "Default job control stop signal action",
#endif
#if defined (PR_SUSPENDED)
 PR_SUSPENDED, "PR_SUSPENDED", "Process suspended",
#endif
 0, NULL, NULL
};

/*  Hardware fault translation table. */

static struct trans faults_table[] =
{
#if defined (FLTILL)
 FLTILL, "FLTILL", "Illegal instruction",
#endif
#if defined (FLTPRIV)
 FLTPRIV, "FLTPRIV", "Privileged instruction",
#endif
#if defined (FLTBPT)
 FLTBPT, "FLTBPT", "Breakpoint trap",
#endif
#if defined (FLTTRACE)
 FLTTRACE, "FLTTRACE", "Trace trap",
#endif
#if defined (FLTACCESS)
 FLTACCESS, "FLTACCESS", "Memory access fault",
#endif
#if defined (FLTBOUNDS)
 FLTBOUNDS, "FLTBOUNDS", "Memory bounds violation",
#endif
#if defined (FLTIOVF)
 FLTIOVF, "FLTIOVF", "Integer overflow",
#endif
#if defined (FLTIZDIV)
 FLTIZDIV, "FLTIZDIV", "Integer zero divide",
#endif
#if defined (FLTFPE)
 FLTFPE, "FLTFPE", "Floating-point exception",
#endif
#if defined (FLTSTACK)
 FLTSTACK, "FLTSTACK", "Unrecoverable stack fault",
#endif
#if defined (FLTPAGE)
 FLTPAGE, "FLTPAGE", "Recoverable page fault",
#endif
 0, NULL, NULL
};

/* Translation table for signal generation information.  See UNIX System
   V Release 4 Programmer's Reference Manual, siginfo(5).  */

static struct sigcode {
  int signo;
  int code;
  char *codename;
  char *desc;
} siginfo_table[] = {
#if defined (SIGILL) && defined (ILL_ILLOPC)
  SIGILL, ILL_ILLOPC, "ILL_ILLOPC", "Illegal opcode",
#endif
#if defined (SIGILL) && defined (ILL_ILLOPN)
  SIGILL, ILL_ILLOPN, "ILL_ILLOPN", "Illegal operand",
#endif
#if defined (SIGILL) && defined (ILL_ILLADR)
  SIGILL, ILL_ILLADR, "ILL_ILLADR", "Illegal addressing mode",
#endif
#if defined (SIGILL) && defined (ILL_ILLTRP)
  SIGILL, ILL_ILLTRP, "ILL_ILLTRP", "Illegal trap",
#endif
#if defined (SIGILL) && defined (ILL_PRVOPC)
  SIGILL, ILL_PRVOPC, "ILL_PRVOPC", "Privileged opcode",
#endif
#if defined (SIGILL) && defined (ILL_PRVREG)
  SIGILL, ILL_PRVREG, "ILL_PRVREG", "Privileged register",
#endif
#if defined (SIGILL) && defined (ILL_COPROC)
  SIGILL, ILL_COPROC, "ILL_COPROC", "Coprocessor error",
#endif
#if defined (SIGILL) && defined (ILL_BADSTK)
  SIGILL, ILL_BADSTK, "ILL_BADSTK", "Internal stack error",
#endif
#if defined (SIGFPE) && defined (FPE_INTDIV)
  SIGFPE, FPE_INTDIV, "FPE_INTDIV", "Integer divide by zero",
#endif
#if defined (SIGFPE) && defined (FPE_INTOVF)
  SIGFPE, FPE_INTOVF, "FPE_INTOVF", "Integer overflow",
#endif
#if defined (SIGFPE) && defined (FPE_FLTDIV)
  SIGFPE, FPE_FLTDIV, "FPE_FLTDIV", "Floating point divide by zero",
#endif
#if defined (SIGFPE) && defined (FPE_FLTOVF)
  SIGFPE, FPE_FLTOVF, "FPE_FLTOVF", "Floating point overflow",
#endif
#if defined (SIGFPE) && defined (FPE_FLTUND)
  SIGFPE, FPE_FLTUND, "FPE_FLTUND", "Floating point underflow",
#endif
#if defined (SIGFPE) && defined (FPE_FLTRES)
  SIGFPE, FPE_FLTRES, "FPE_FLTRES", "Floating point inexact result",
#endif
#if defined (SIGFPE) && defined (FPE_FLTINV)
  SIGFPE, FPE_FLTINV, "FPE_FLTINV", "Invalid floating point operation",
#endif
#if defined (SIGFPE) && defined (FPE_FLTSUB)
  SIGFPE, FPE_FLTSUB, "FPE_FLTSUB", "Subscript out of range",
#endif
#if defined (SIGSEGV) && defined (SEGV_MAPERR)
  SIGSEGV, SEGV_MAPERR, "SEGV_MAPERR", "Address not mapped to object",
#endif
#if defined (SIGSEGV) && defined (SEGV_ACCERR)
  SIGSEGV, SEGV_ACCERR, "SEGV_ACCERR", "Invalid permissions for object",
#endif
#if defined (SIGBUS) && defined (BUS_ADRALN)
  SIGBUS, BUS_ADRALN, "BUS_ADRALN", "Invalid address alignment",
#endif
#if defined (SIGBUS) && defined (BUS_ADRERR)
  SIGBUS, BUS_ADRERR, "BUS_ADRERR", "Non-existent physical address",
#endif
#if defined (SIGBUS) && defined (BUS_OBJERR)
  SIGBUS, BUS_OBJERR, "BUS_OBJERR", "Object specific hardware error",
#endif
#if defined (SIGTRAP) && defined (TRAP_BRKPT)
  SIGTRAP, TRAP_BRKPT, "TRAP_BRKPT", "Process breakpoint",
#endif
#if defined (SIGTRAP) && defined (TRAP_TRACE)
  SIGTRAP, TRAP_TRACE, "TRAP_TRACE", "Process trace trap",
#endif
#if defined (SIGCLD) && defined (CLD_EXITED)
  SIGCLD, CLD_EXITED, "CLD_EXITED", "Child has exited",
#endif
#if defined (SIGCLD) && defined (CLD_KILLED)
  SIGCLD, CLD_KILLED, "CLD_KILLED", "Child was killed",
#endif
#if defined (SIGCLD) && defined (CLD_DUMPED)
  SIGCLD, CLD_DUMPED, "CLD_DUMPED", "Child has terminated abnormally",
#endif
#if defined (SIGCLD) && defined (CLD_TRAPPED)
  SIGCLD, CLD_TRAPPED, "CLD_TRAPPED", "Traced child has trapped",
#endif
#if defined (SIGCLD) && defined (CLD_STOPPED)
  SIGCLD, CLD_STOPPED, "CLD_STOPPED", "Child has stopped",
#endif
#if defined (SIGCLD) && defined (CLD_CONTINUED)
  SIGCLD, CLD_CONTINUED, "CLD_CONTINUED", "Stopped child had continued",
#endif
#if defined (SIGPOLL) && defined (POLL_IN)
  SIGPOLL, POLL_IN, "POLL_IN", "Input input available",
#endif
#if defined (SIGPOLL) && defined (POLL_OUT)
  SIGPOLL, POLL_OUT, "POLL_OUT", "Output buffers available",
#endif
#if defined (SIGPOLL) && defined (POLL_MSG)
  SIGPOLL, POLL_MSG, "POLL_MSG", "Input message available",
#endif
#if defined (SIGPOLL) && defined (POLL_ERR)
  SIGPOLL, POLL_ERR, "POLL_ERR", "I/O error",
#endif
#if defined (SIGPOLL) && defined (POLL_PRI)
  SIGPOLL, POLL_PRI, "POLL_PRI", "High priority input available",
#endif
#if defined (SIGPOLL) && defined (POLL_HUP)
  SIGPOLL, POLL_HUP, "POLL_HUP", "Device disconnected",
#endif
  0, 0, NULL, NULL
};

/* Translation table for errno values.  See intro(2) in most UNIX systems
   Programmers Reference Manuals.

   Note that some systems provide a function (strerror) that returns the
   error message string, or a global variable that is the base address of the
   array of character pointers.  Perhaps we should try to make use of these
   provided strings if they are present, but at least this is more portable.
   (FIXME?) */

static struct trans errno_table[] =
{
#if defined (EPERM)
  EPERM, "EPERM", "Not super-user",
#endif
#if defined (ENOENT)
  ENOENT, "ENOENT", "No such file or directory",
#endif
#if defined (ESRCH)
  ESRCH, "ESRCH", "No such process",
#endif
#if defined (EINTR)
  EINTR, "EINTR", "Interrupted system call",
#endif
#if defined (EIO)
  EIO, "EIO", "I/O error",
#endif
#if defined (ENXIO)
  ENXIO, "ENXIO", "No such device or address",
#endif
#if defined (E2BIG)
  E2BIG, "E2BIG", "Arg list too long",
#endif
#if defined (ENOEXEC)
  ENOEXEC, "ENOEXEC", "Exec format error",
#endif
#if defined (EBADF)
  EBADF, "EBADF", "Bad file number",
#endif
#if defined (ECHILD)
  ECHILD, "ECHILD", "No child process",
#endif
#if defined (EAGAIN)
  EAGAIN, "EAGAIN", "No more processes",
#endif
#if defined (ENOMEM)
  ENOMEM, "ENOMEM", "Not enough space",
#endif
#if defined (EACCES)
  EACCES, "EACCES", "Permission denied",
#endif
#if defined (EFAULT)
  EFAULT, "EFAULT", "Bad address",
#endif
#if defined (ENOTBLK)
  ENOTBLK, "ENOTBLK", "Block device required",
#endif
#if defined (EBUSY)
  EBUSY, "EBUSY", "Device busy",
#endif
#if defined (EEXIST)
  EEXIST, "EEXIST", "File exists",
#endif
#if defined (EXDEV)
  EXDEV, "EXDEV", "Cross-device link",
#endif
#if defined (ENODEV)
  ENODEV, "ENODEV", "No such device",
#endif
#if defined (ENOTDIR)
  ENOTDIR, "ENOTDIR", "Not a directory",
#endif
#if defined (EISDIR)
  EISDIR, "EISDIR", "Is a directory",
#endif
#if defined (EINVAL)
  EINVAL, "EINVAL", "Invalid argument",
#endif
#if defined (ENFILE)
  ENFILE, "ENFILE", "File table overflow",
#endif
#if defined (EMFILE)
  EMFILE, "EMFILE", "Too many open files",
#endif
#if defined (ENOTTY)
  ENOTTY, "ENOTTY", "Not a typewriter",
#endif
#if defined (ETXTBSY)
  ETXTBSY, "ETXTBSY", "Text file busy",
#endif
#if defined (EFBIG)
  EFBIG, "EFBIG", "File too large",
#endif
#if defined (ENOSPC)
  ENOSPC, "ENOSPC", "No space left on device",
#endif
#if defined (ESPIPE)
  ESPIPE, "ESPIPE", "Illegal seek",
#endif
#if defined (EROFS)
  EROFS, "EROFS", "Read only file system",
#endif
#if defined (EMLINK)
  EMLINK, "EMLINK", "Too many links",
#endif
#if defined (EPIPE)
  EPIPE, "EPIPE", "Broken pipe",
#endif
#if defined (EDOM)
  EDOM, "EDOM", "Math argument out of domain of func",
#endif
#if defined (ERANGE)
  ERANGE, "ERANGE", "Math result not representable",
#endif
#if defined (ENOMSG)
  ENOMSG, "ENOMSG", "No message of desired type",
#endif
#if defined (EIDRM)
  EIDRM, "EIDRM", "Identifier removed",
#endif
#if defined (ECHRNG)
  ECHRNG, "ECHRNG", "Channel number out of range",
#endif
#if defined (EL2NSYNC)
  EL2NSYNC, "EL2NSYNC", "Level 2 not synchronized",
#endif
#if defined (EL3HLT)
  EL3HLT, "EL3HLT", "Level 3 halted",
#endif
#if defined (EL3RST)
  EL3RST, "EL3RST", "Level 3 reset",
#endif
#if defined (ELNRNG)
  ELNRNG, "ELNRNG", "Link number out of range",
#endif
#if defined (EUNATCH)
  EUNATCH, "EUNATCH", "Protocol driver not attached",
#endif
#if defined (ENOCSI)
  ENOCSI, "ENOCSI", "No CSI structure available",
#endif
#if defined (EL2HLT)
  EL2HLT, "EL2HLT", "Level 2 halted",
#endif
#if defined (EDEADLK)
  EDEADLK, "EDEADLK", "Deadlock condition",
#endif
#if defined (ENOLCK)
  ENOLCK, "ENOLCK", "No record locks available",
#endif
#if defined (EBADE)
  EBADE, "EBADE", "Invalid exchange",
#endif
#if defined (EBADR)
  EBADR, "EBADR", "Invalid request descriptor",
#endif
#if defined (EXFULL)
  EXFULL, "EXFULL", "Exchange full",
#endif
#if defined (ENOANO)
  ENOANO, "ENOANO", "No anode",
#endif
#if defined (EBADRQC)
  EBADRQC, "EBADRQC", "Invalid request code",
#endif
#if defined (EBADSLT)
  EBADSLT, "EBADSLT", "Invalid slot",
#endif
#if defined (EDEADLOCK)
  EDEADLOCK, "EDEADLOCK", "File locking deadlock error",
#endif
#if defined (EBFONT)
  EBFONT, "EBFONT", "Bad font file fmt",
#endif
#if defined (ENOSTR)
  ENOSTR, "ENOSTR", "Device not a stream",
#endif
#if defined (ENODATA)
  ENODATA, "ENODATA", "No data available",
#endif
#if defined (ETIME)
  ETIME, "ETIME", "Timer expired",
#endif
#if defined (ENOSR)
  ENOSR, "ENOSR", "Out of streams resources",
#endif
#if defined (ENONET)
  ENONET, "ENONET", "Machine is not on the network",
#endif
#if defined (ENOPKG)
  ENOPKG, "ENOPKG", "Package not installed",
#endif
#if defined (EREMOTE)
  EREMOTE, "EREMOTE", "Object is remote",
#endif
#if defined (ENOLINK)
  ENOLINK, "ENOLINK", "Link has been severed",
#endif
#if defined (EADV)
  EADV, "EADV", "Advertise error",
#endif
#if defined (ESRMNT)
  ESRMNT, "ESRMNT", "Srmount error",
#endif
#if defined (ECOMM)
  ECOMM, "ECOMM", "Communication error on send",
#endif
#if defined (EPROTO)
  EPROTO, "EPROTO", "Protocol error",
#endif
#if defined (EMULTIHOP)
  EMULTIHOP, "EMULTIHOP", "Multihop attempted",
#endif
#if defined (EDOTDOT)
  EDOTDOT, "EDOTDOT", "RFS specific error",
#endif
#if defined (EBADMSG)
  EBADMSG, "EBADMSG", "Not a data message",
#endif
#if defined (ENAMETOOLONG)
  ENAMETOOLONG, "ENAMETOOLONG", "File name too long",
#endif
#if defined (EOVERFLOW)
  EOVERFLOW, "EOVERFLOW", "Value too large for defined data type",
#endif
#if defined (ENOTUNIQ)
  ENOTUNIQ, "ENOTUNIQ", "Name not unique on network",
#endif
#if defined (EBADFD)
  EBADFD, "EBADFD", "File descriptor in bad state",
#endif
#if defined (EREMCHG)
  EREMCHG, "EREMCHG", "Remote address changed",
#endif
#if defined (ELIBACC)
  ELIBACC, "ELIBACC", "Cannot access a needed shared library",
#endif
#if defined (ELIBBAD)
  ELIBBAD, "ELIBBAD", "Accessing a corrupted shared library",
#endif
#if defined (ELIBSCN)
  ELIBSCN, "ELIBSCN", ".lib section in a.out corrupted",
#endif
#if defined (ELIBMAX)
  ELIBMAX, "ELIBMAX", "Attempting to link in too many shared libraries",
#endif
#if defined (ELIBEXEC)
  ELIBEXEC, "ELIBEXEC", "Cannot exec a shared library directly",
#endif
#if defined (EILSEQ)
  EILSEQ, "EILSEQ", "Illegal byte sequence",
#endif
#if defined (ENOSYS)
  ENOSYS, "ENOSYS", "Operation not applicable",
#endif
#if defined (ELOOP)
  ELOOP, "ELOOP", "Too many symbolic links encountered",
#endif
#if defined (ERESTART)
  ERESTART, "ERESTART", "Interrupted system call should be restarted",
#endif
#if defined (ESTRPIPE)
  ESTRPIPE, "ESTRPIPE", "Streams pipe error",
#endif
#if defined (ENOTEMPTY)
  ENOTEMPTY, "ENOTEMPTY", "Directory not empty",
#endif
#if defined (EUSERS)
  EUSERS, "EUSERS", "Too many users",
#endif
#if defined (ENOTSOCK)
  ENOTSOCK, "ENOTSOCK", "Socket operation on non-socket",
#endif
#if defined (EDESTADDRREQ)
  EDESTADDRREQ, "EDESTADDRREQ", "Destination address required",
#endif
#if defined (EMSGSIZE)
  EMSGSIZE, "EMSGSIZE", "Message too long",
#endif
#if defined (EPROTOTYPE)
  EPROTOTYPE, "EPROTOTYPE", "Protocol wrong type for socket",
#endif
#if defined (ENOPROTOOPT)
  ENOPROTOOPT, "ENOPROTOOPT", "Protocol not available",
#endif
#if defined (EPROTONOSUPPORT)
  EPROTONOSUPPORT, "EPROTONOSUPPORT", "Protocol not supported",
#endif
#if defined (ESOCKTNOSUPPORT)
  ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", "Socket type not supported",
#endif
#if defined (EOPNOTSUPP)
  EOPNOTSUPP, "EOPNOTSUPP", "Operation not supported on transport endpoint ",
#endif
#if defined (EPFNOSUPPORT)
  EPFNOSUPPORT, "EPFNOSUPPORT", "Protocol family not supported",
#endif
#if defined (EAFNOSUPPORT)
  EAFNOSUPPORT, "EAFNOSUPPORT", "Address family not supported by protocol",
#endif
#if defined (EADDRINUSE)
  EADDRINUSE, "EADDRINUSE", "Address already in use",
#endif
#if defined (EADDRNOTAVAIL)
  EADDRNOTAVAIL, "EADDRNOTAVAIL","Cannot assign requested address",
#endif
#if defined (ENETDOWN)
  ENETDOWN, "ENETDOWN", "Network is down",
#endif
#if defined (ENETUNREACH)
  ENETUNREACH, "ENETUNREACH", "Network is unreachable",
#endif
#if defined (ENETRESET)
  ENETRESET, "ENETRESET", "Network dropped connection because of reset",
#endif
#if defined (ECONNABORTED)
  ECONNABORTED, "ECONNABORTED", "Software caused connection abort",
#endif
#if defined (ECONNRESET)
  ECONNRESET, "ECONNRESET", "Connection reset by peer",
#endif
#if defined (ENOBUFS)
  ENOBUFS, "ENOBUFS", "No buffer space available",
#endif
#if defined (EISCONN)
  EISCONN, "EISCONN", "Transport endpoint is already connected",
#endif
#if defined (ENOTCONN)
  ENOTCONN, "ENOTCONN", "Transport endpoint is not connected",
#endif
#if defined (ESHUTDOWN)
  ESHUTDOWN, "ESHUTDOWN", "Cannot send after transport endpoint shutdown",
#endif
#if defined (ETOOMANYREFS)
  ETOOMANYREFS, "ETOOMANYREFS", "Too many references: cannot splice",
#endif
#if defined (ETIMEDOUT)
  ETIMEDOUT, "ETIMEDOUT", "Connection timed out",
#endif
#if defined (ECONNREFUSED)
  ECONNREFUSED, "ECONNREFUSED", "Connection refused",
#endif
#if defined (EHOSTDOWN)
  EHOSTDOWN, "EHOSTDOWN", "Host is down",
#endif
#if defined (EHOSTUNREACH)
  EHOSTUNREACH, "EHOSTUNREACH", "No route to host",
#endif
#if defined (EWOULDBLOCK)
  EWOULDBLOCK, "EWOULDBLOCK", "Operation already in progress",
#endif
#if defined (EINPROGRESS)
  EINPROGRESS, "EINPROGRESS", "Operation now in progress",
#endif
#if defined (ESTALE)
  ESTALE, "ESTALE", "Stale NFS file handle",
#endif
#if defined (EUCLEAN)
  EUCLEAN, "EUCLEAN", "Structure needs cleaning",
#endif
#if defined (ENOTNAM)
  ENOTNAM, "ENOTNAM", "Not a XENIX named type file",
#endif
#if defined (ENAVAIL)
  ENAVAIL, "ENAVAIL", "No XENIX semaphores available",
#endif
#if defined (EISNAM)
  EISNAM, "EISNAM", "Is a named type file",
#endif
#if defined (EREMOTEIO)
  EREMOTEIO, "EREMOTEIO", "Remote I/O error",
#endif
 0, NULL, NULL
};

static char *syscall_table[MAX_SYSCALLS];

/* Prototypes for local functions */

static void
set_proc_siginfo PARAMS ((struct procinfo *, int));

static void
init_syscall_table PARAMS ((void));

static char *
syscallname PARAMS ((int));

static char *
signalname PARAMS ((int));

static int
proc_address_to_fd PARAMS ((CORE_ADDR, int));

static int
open_proc_file PARAMS ((int, struct procinfo *));

static void
close_proc_file PARAMS ((struct procinfo *));

static void
unconditionally_kill_inferior PARAMS ((void));

static void
proc_init_failed PARAMS ((char *));

static void
info_proc PARAMS ((char *, int));

static void
info_proc_flags PARAMS ((struct procinfo *, int));

static void
info_proc_stop PARAMS ((struct procinfo *, int));

static void
info_proc_siginfo PARAMS ((struct procinfo *, int));

static void
info_proc_syscalls PARAMS ((struct procinfo *, int));

static void
info_proc_mappings PARAMS ((struct procinfo *, int));

static void
info_proc_signals PARAMS ((struct procinfo *, int));

static void
info_proc_faults PARAMS ((struct procinfo *, int));

static char *
mappingflags PARAMS ((long));

static char *
lookupname PARAMS ((struct trans *, unsigned int, char *));

static char *
lookupdesc PARAMS ((struct trans *, unsigned int));

/* External function prototypes that can't be easily included in any
   header file because the args are typedefs in system include files. */

extern void
supply_gregset PARAMS ((gregset_t *));

extern void
fill_gregset PARAMS ((gregset_t *, int));

extern void
supply_fpregset PARAMS ((fpregset_t *));

extern void
fill_fpregset PARAMS ((fpregset_t *, int));

/*

LOCAL FUNCTION

	lookupdesc -- translate a value to a summary desc string

SYNOPSIS

	static char *lookupdesc (struct trans *transp, unsigned int val);

DESCRIPTION
	
	Given a pointer to a translation table and a value to be translated,
	lookup the desc string and return it.
 */

static char *
lookupdesc (transp, val)
     struct trans *transp;
     unsigned int val;
{
  char *desc;
  
  for (desc = NULL; transp -> name != NULL; transp++)
    {
      if (transp -> value == val)
	{
	  desc = transp -> desc;
	  break;
	}
    }

  /* Didn't find a translation for the specified value, set a default one. */

  if (desc == NULL)
    {
      desc = "Unknown";
    }
  return (desc);
}

/*

LOCAL FUNCTION

	lookupname -- translate a value to symbolic name

SYNOPSIS

	static char *lookupname (struct trans *transp, unsigned int val,
				 char *prefix);

DESCRIPTION
	
	Given a pointer to a translation table, a value to be translated,
	and a default prefix to return if the value can't be translated,
	match the value with one of the translation table entries and
	return a pointer to the symbolic name.

	If no match is found it just returns the value as a printable string,
	with the given prefix.  The previous such value, if any, is freed
	at this time.
 */

static char *
lookupname (transp, val, prefix)
     struct trans *transp;
     unsigned int val;
     char *prefix;
{
  static char *locbuf;
  char *name;
  
  for (name = NULL; transp -> name != NULL; transp++)
    {
      if (transp -> value == val)
	{
	  name = transp -> name;
	  break;
	}
    }

  /* Didn't find a translation for the specified value, build a default
     one using the specified prefix and return it.  The lifetime of
     the value is only until the next one is needed. */

  if (name == NULL)
    {
      if (locbuf != NULL)
	{
	  free (locbuf);
	}
      locbuf = xmalloc (strlen (prefix) + 16);
      (void) sprintf (locbuf, "%s %u", prefix, val);
      name = locbuf;
    }
  return (name);
}

static char *
sigcodename (sip)
     siginfo_t *sip;
{
  struct sigcode *scp;
  char *name = NULL;
  static char locbuf[32];
  
  for (scp = siginfo_table; scp -> codename != NULL; scp++)
    {
      if ((scp -> signo == sip -> si_signo) &&
	  (scp -> code == sip -> si_code))
	{
	  name = scp -> codename;
	  break;
	}
    }
  if (name == NULL)
    {
      (void) sprintf (locbuf, "sigcode %u", sip -> si_signo);
      name = locbuf;
    }
  return (name);
}

static char *sigcodedesc (sip)
     siginfo_t *sip;
{
  struct sigcode *scp;
  char *desc = NULL;
  
  for (scp = siginfo_table; scp -> codename != NULL; scp++)
    {
      if ((scp -> signo == sip -> si_signo) &&
	  (scp -> code == sip -> si_code))
	{
	  desc = scp -> desc;
	  break;
	}
    }
  if (desc == NULL)
    {
      desc = "Unrecognized signal or trap use";
    }
  return (desc);
}

/*

LOCAL FUNCTION

	syscallname - translate a system call number into a system call name

SYNOPSIS

	char *syscallname (int syscallnum)

DESCRIPTION

	Given a system call number, translate it into the printable name
	of a system call, or into "syscall <num>" if it is an unknown
	number.
 */

static char *
syscallname (syscallnum)
     int syscallnum;
{
  static char locbuf[32];
  char *rtnval;
  
  if (syscallnum >= 0 && syscallnum < MAX_SYSCALLS)
    {
      rtnval = syscall_table[syscallnum];
    }
  else
    {
      (void) sprintf (locbuf, "syscall %u", syscallnum);
      rtnval = locbuf;
    }
  return (rtnval);
}

/*

LOCAL FUNCTION

	init_syscall_table - initialize syscall translation table

SYNOPSIS

	void init_syscall_table (void)

DESCRIPTION

	Dynamically initialize the translation table to convert system
	call numbers into printable system call names.  Done once per
	gdb run, on initialization.

NOTES

	This is awfully ugly, but preprocessor tricks to make it prettier
	tend to be nonportable.
 */

static void
init_syscall_table ()
{
  int syscallnum;
  
#if defined (SYS_exit)
  syscall_table[SYS_exit] = "exit";
#endif
#if defined (SYS_fork)
  syscall_table[SYS_fork] = "fork";
#endif
#if defined (SYS_read)
  syscall_table[SYS_read] = "read";
#endif
#if defined (SYS_write)
  syscall_table[SYS_write] = "write";
#endif
#if defined (SYS_open)
  syscall_table[SYS_open] = "open";
#endif
#if defined (SYS_close)
  syscall_table[SYS_close] = "close";
#endif
#if defined (SYS_wait)
  syscall_table[SYS_wait] = "wait";
#endif
#if defined (SYS_creat)
  syscall_table[SYS_creat] = "creat";
#endif
#if defined (SYS_link)
  syscall_table[SYS_link] = "link";
#endif
#if defined (SYS_unlink)
  syscall_table[SYS_unlink] = "unlink";
#endif
#if defined (SYS_exec)
  syscall_table[SYS_exec] = "exec";
#endif
#if defined (SYS_execv)
  syscall_table[SYS_execv] = "execv";
#endif
#if defined (SYS_execve)
  syscall_table[SYS_execve] = "execve";
#endif
#if defined (SYS_chdir)
  syscall_table[SYS_chdir] = "chdir";
#endif
#if defined (SYS_time)
  syscall_table[SYS_time] = "time";
#endif
#if defined (SYS_mknod)
  syscall_table[SYS_mknod] = "mknod";
#endif
#if defined (SYS_chmod)
  syscall_table[SYS_chmod] = "chmod";
#endif
#if defined (SYS_chown)
  syscall_table[SYS_chown] = "chown";
#endif
#if defined (SYS_brk)
  syscall_table[SYS_brk] = "brk";
#endif
#if defined (SYS_stat)
  syscall_table[SYS_stat] = "stat";
#endif
#if defined (SYS_lseek)
  syscall_table[SYS_lseek] = "lseek";
#endif
#if defined (SYS_getpid)
  syscall_table[SYS_getpid] = "getpid";
#endif
#if defined (SYS_mount)
  syscall_table[SYS_mount] = "mount";
#endif
#if defined (SYS_umount)
  syscall_table[SYS_umount] = "umount";
#endif
#if defined (SYS_setuid)
  syscall_table[SYS_setuid] = "setuid";
#endif
#if defined (SYS_getuid)
  syscall_table[SYS_getuid] = "getuid";
#endif
#if defined (SYS_stime)
  syscall_table[SYS_stime] = "stime";
#endif
#if defined (SYS_ptrace)
  syscall_table[SYS_ptrace] = "ptrace";
#endif
#if defined (SYS_alarm)
  syscall_table[SYS_alarm] = "alarm";
#endif
#if defined (SYS_fstat)
  syscall_table[SYS_fstat] = "fstat";
#endif
#if defined (SYS_pause)
  syscall_table[SYS_pause] = "pause";
#endif
#if defined (SYS_utime)
  syscall_table[SYS_utime] = "utime";
#endif
#if defined (SYS_stty)
  syscall_table[SYS_stty] = "stty";
#endif
#if defined (SYS_gtty)
  syscall_table[SYS_gtty] = "gtty";
#endif
#if defined (SYS_access)
  syscall_table[SYS_access] = "access";
#endif
#if defined (SYS_nice)
  syscall_table[SYS_nice] = "nice";
#endif
#if defined (SYS_statfs)
  syscall_table[SYS_statfs] = "statfs";
#endif
#if defined (SYS_sync)
  syscall_table[SYS_sync] = "sync";
#endif
#if defined (SYS_kill)
  syscall_table[SYS_kill] = "kill";
#endif
#if defined (SYS_fstatfs)
  syscall_table[SYS_fstatfs] = "fstatfs";
#endif
#if defined (SYS_pgrpsys)
  syscall_table[SYS_pgrpsys] = "pgrpsys";
#endif
#if defined (SYS_xenix)
  syscall_table[SYS_xenix] = "xenix";
#endif
#if defined (SYS_dup)
  syscall_table[SYS_dup] = "dup";
#endif
#if defined (SYS_pipe)
  syscall_table[SYS_pipe] = "pipe";
#endif
#if defined (SYS_times)
  syscall_table[SYS_times] = "times";
#endif
#if defined (SYS_profil)
  syscall_table[SYS_profil] = "profil";
#endif
#if defined (SYS_plock)
  syscall_table[SYS_plock] = "plock";
#endif
#if defined (SYS_setgid)
  syscall_table[SYS_setgid] = "setgid";
#endif
#if defined (SYS_getgid)
  syscall_table[SYS_getgid] = "getgid";
#endif
#if defined (SYS_signal)
  syscall_table[SYS_signal] = "signal";
#endif
#if defined (SYS_msgsys)
  syscall_table[SYS_msgsys] = "msgsys";
#endif
#if defined (SYS_sys3b)
  syscall_table[SYS_sys3b] = "sys3b";
#endif
#if defined (SYS_acct)
  syscall_table[SYS_acct] = "acct";
#endif
#if defined (SYS_shmsys)
  syscall_table[SYS_shmsys] = "shmsys";
#endif
#if defined (SYS_semsys)
  syscall_table[SYS_semsys] = "semsys";
#endif
#if defined (SYS_ioctl)
  syscall_table[SYS_ioctl] = "ioctl";
#endif
#if defined (SYS_uadmin)
  syscall_table[SYS_uadmin] = "uadmin";
#endif
#if defined (SYS_utssys)
  syscall_table[SYS_utssys] = "utssys";
#endif
#if defined (SYS_fsync)
  syscall_table[SYS_fsync] = "fsync";
#endif
#if defined (SYS_umask)
  syscall_table[SYS_umask] = "umask";
#endif
#if defined (SYS_chroot)
  syscall_table[SYS_chroot] = "chroot";
#endif
#if defined (SYS_fcntl)
  syscall_table[SYS_fcntl] = "fcntl";
#endif
#if defined (SYS_ulimit)
  syscall_table[SYS_ulimit] = "ulimit";
#endif
#if defined (SYS_rfsys)
  syscall_table[SYS_rfsys] = "rfsys";
#endif
#if defined (SYS_rmdir)
  syscall_table[SYS_rmdir] = "rmdir";
#endif
#if defined (SYS_mkdir)
  syscall_table[SYS_mkdir] = "mkdir";
#endif
#if defined (SYS_getdents)
  syscall_table[SYS_getdents] = "getdents";
#endif
#if defined (SYS_sysfs)
  syscall_table[SYS_sysfs] = "sysfs";
#endif
#if defined (SYS_getmsg)
  syscall_table[SYS_getmsg] = "getmsg";
#endif
#if defined (SYS_putmsg)
  syscall_table[SYS_putmsg] = "putmsg";
#endif
#if defined (SYS_poll)
  syscall_table[SYS_poll] = "poll";
#endif
#if defined (SYS_lstat)
  syscall_table[SYS_lstat] = "lstat";
#endif
#if defined (SYS_symlink)
  syscall_table[SYS_symlink] = "symlink";
#endif
#if defined (SYS_readlink)
  syscall_table[SYS_readlink] = "readlink";
#endif
#if defined (SYS_setgroups)
  syscall_table[SYS_setgroups] = "setgroups";
#endif
#if defined (SYS_getgroups)
  syscall_table[SYS_getgroups] = "getgroups";
#endif
#if defined (SYS_fchmod)
  syscall_table[SYS_fchmod] = "fchmod";
#endif
#if defined (SYS_fchown)
  syscall_table[SYS_fchown] = "fchown";
#endif
#if defined (SYS_sigprocmask)
  syscall_table[SYS_sigprocmask] = "sigprocmask";
#endif
#if defined (SYS_sigsuspend)
  syscall_table[SYS_sigsuspend] = "sigsuspend";
#endif
#if defined (SYS_sigaltstack)
  syscall_table[SYS_sigaltstack] = "sigaltstack";
#endif
#if defined (SYS_sigaction)
  syscall_table[SYS_sigaction] = "sigaction";
#endif
#if defined (SYS_sigpending)
  syscall_table[SYS_sigpending] = "sigpending";
#endif
#if defined (SYS_context)
  syscall_table[SYS_context] = "context";
#endif
#if defined (SYS_evsys)
  syscall_table[SYS_evsys] = "evsys";
#endif
#if defined (SYS_evtrapret)
  syscall_table[SYS_evtrapret] = "evtrapret";
#endif
#if defined (SYS_statvfs)
  syscall_table[SYS_statvfs] = "statvfs";
#endif
#if defined (SYS_fstatvfs)
  syscall_table[SYS_fstatvfs] = "fstatvfs";
#endif
#if defined (SYS_nfssys)
  syscall_table[SYS_nfssys] = "nfssys";
#endif
#if defined (SYS_waitsys)
  syscall_table[SYS_waitsys] = "waitsys";
#endif
#if defined (SYS_sigsendsys)
  syscall_table[SYS_sigsendsys] = "sigsendsys";
#endif
#if defined (SYS_hrtsys)
  syscall_table[SYS_hrtsys] = "hrtsys";
#endif
#if defined (SYS_acancel)
  syscall_table[SYS_acancel] = "acancel";
#endif
#if defined (SYS_async)
  syscall_table[SYS_async] = "async";
#endif
#if defined (SYS_priocntlsys)
  syscall_table[SYS_priocntlsys] = "priocntlsys";
#endif
#if defined (SYS_pathconf)
  syscall_table[SYS_pathconf] = "pathconf";
#endif
#if defined (SYS_mincore)
  syscall_table[SYS_mincore] = "mincore";
#endif
#if defined (SYS_mmap)
  syscall_table[SYS_mmap] = "mmap";
#endif
#if defined (SYS_mprotect)
  syscall_table[SYS_mprotect] = "mprotect";
#endif
#if defined (SYS_munmap)
  syscall_table[SYS_munmap] = "munmap";
#endif
#if defined (SYS_fpathconf)
  syscall_table[SYS_fpathconf] = "fpathconf";
#endif
#if defined (SYS_vfork)
  syscall_table[SYS_vfork] = "vfork";
#endif
#if defined (SYS_fchdir)
  syscall_table[SYS_fchdir] = "fchdir";
#endif
#if defined (SYS_readv)
  syscall_table[SYS_readv] = "readv";
#endif
#if defined (SYS_writev)
  syscall_table[SYS_writev] = "writev";
#endif
#if defined (SYS_xstat)
  syscall_table[SYS_xstat] = "xstat";
#endif
#if defined (SYS_lxstat)
  syscall_table[SYS_lxstat] = "lxstat";
#endif
#if defined (SYS_fxstat)
  syscall_table[SYS_fxstat] = "fxstat";
#endif
#if defined (SYS_xmknod)
  syscall_table[SYS_xmknod] = "xmknod";
#endif
#if defined (SYS_clocal)
  syscall_table[SYS_clocal] = "clocal";
#endif
#if defined (SYS_setrlimit)
  syscall_table[SYS_setrlimit] = "setrlimit";
#endif
#if defined (SYS_getrlimit)
  syscall_table[SYS_getrlimit] = "getrlimit";
#endif
#if defined (SYS_lchown)
  syscall_table[SYS_lchown] = "lchown";
#endif
#if defined (SYS_memcntl)
  syscall_table[SYS_memcntl] = "memcntl";
#endif
#if defined (SYS_getpmsg)
  syscall_table[SYS_getpmsg] = "getpmsg";
#endif
#if defined (SYS_putpmsg)
  syscall_table[SYS_putpmsg] = "putpmsg";
#endif
#if defined (SYS_rename)
  syscall_table[SYS_rename] = "rename";
#endif
#if defined (SYS_uname)
  syscall_table[SYS_uname] = "uname";
#endif
#if defined (SYS_setegid)
  syscall_table[SYS_setegid] = "setegid";
#endif
#if defined (SYS_sysconfig)
  syscall_table[SYS_sysconfig] = "sysconfig";
#endif
#if defined (SYS_adjtime)
  syscall_table[SYS_adjtime] = "adjtime";
#endif
#if defined (SYS_systeminfo)
  syscall_table[SYS_systeminfo] = "systeminfo";
#endif
#if defined (SYS_seteuid)
  syscall_table[SYS_seteuid] = "seteuid";
#endif
}

/*

GLOBAL FUNCTION

	ptrace -- override library version to force errors for /proc version

SYNOPSIS

	int ptrace (int request, int pid, int arg3, int arg4)

DESCRIPTION

	When gdb is configured to use /proc, it should not be calling
	or otherwise attempting to use ptrace.  In order to catch errors
	where use of /proc is configured, but some routine is still calling
	ptrace, we provide a local version of a function with that name
	that does nothing but issue an error message.
*/

int
ptrace (request, pid, arg3, arg4)
     int request;
     int pid;
     int arg3;
     int arg4;
{
  error ("internal error - there is a call to ptrace() somewhere");
  /*NOTREACHED*/
}

/*

GLOBAL FUNCTION

	kill_inferior_fast -- kill inferior while gdb is exiting

SYNOPSIS

	void kill_inferior_fast (void)

DESCRIPTION

	This is used when GDB is exiting.  It gives less chance of error.

NOTES

	Don't attempt to kill attached inferiors since we may be called
	when gdb is in the process of aborting, and killing the attached
	inferior may be very anti-social.  This is particularly true if we
	were attached just so we could use the /proc facilities to get
	detailed information about it's status.

*/

void
kill_inferior_fast ()
{
  if (inferior_pid != 0 && !attach_flag)
    {
      unconditionally_kill_inferior ();
    }
}

/*

GLOBAL FUNCTION

	kill_inferior - kill any currently inferior

SYNOPSIS

	void kill_inferior (void)

DESCRIPTION

	Kill any current inferior.

NOTES

	Kills even attached inferiors.  Presumably the user has already
	been prompted that the inferior is an attached one rather than
	one started by gdb.  (FIXME?)

*/

void
kill_inferior ()
{
  if (inferior_pid != 0)
    {
      unconditionally_kill_inferior ();
      target_mourn_inferior ();
    }
}

/*

LOCAL FUNCTION

	unconditionally_kill_inferior - terminate the inferior

SYNOPSIS

	static void unconditionally_kill_inferior (void)

DESCRIPTION

	Kill the current inferior.  Should not be called until it
	is at least tested that there is an inferior.

NOTE

	A possibly useful enhancement would be to first try sending
	the inferior a terminate signal, politely asking it to commit
	suicide, before we murder it.

*/

static void
unconditionally_kill_inferior ()
{
  int signo;
  
  signo = SIGKILL;
  (void) ioctl (pi.fd, PIOCKILL, &signo);
  close_proc_file (&pi);
  wait ((int *) 0);
}

/*

GLOBAL FUNCTION

	child_xfer_memory -- copy data to or from inferior memory space

SYNOPSIS

	int child_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len,
		int dowrite, struct target_ops target)

DESCRIPTION

	Copy LEN bytes to/from inferior's memory starting at MEMADDR
	from/to debugger memory starting at MYADDR.  Copy from inferior
	if DOWRITE is zero or to inferior if DOWRITE is nonzero.
  
	Returns the length copied, which is either the LEN argument or
	zero.  This xfer function does not do partial moves, since child_ops
	doesn't allow memory operations to cross below us in the target stack
	anyway.

NOTES

	The /proc interface makes this an almost trivial task.
 */


int
child_xfer_memory (memaddr, myaddr, len, dowrite, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int dowrite;
     struct target_ops *target; /* ignored */
{
  int nbytes = 0;

  if (lseek (pi.fd, (off_t) memaddr, 0) == (off_t) memaddr)
    {
      if (dowrite)
	{
	  nbytes = write (pi.fd, myaddr, len);
	}
      else
	{
	  nbytes = read (pi.fd, myaddr, len);
	}
      if (nbytes < 0)
	{
	  nbytes = 0;
	}
    }
  return (nbytes);
}

/*

GLOBAL FUNCTION

	store_inferior_registers -- copy register values back to inferior

SYNOPSIS

	void store_inferior_registers (int regno)

DESCRIPTION

	Store our current register values back into the inferior.  If
	REGNO is -1 then store all the register, otherwise store just
	the value specified by REGNO.

NOTES

	If we are storing only a single register, we first have to get all
	the current values from the process, overwrite the desired register
	in the gregset with the one we want from gdb's registers, and then
	send the whole set back to the process.  For writing all the
	registers, all we have to do is generate the gregset and send it to
	the process.

	Also note that the process has to be stopped on an event of interest
	for this to work, which basically means that it has to have been
	run under the control of one of the other /proc ioctl calls and not
	ptrace.  Since we don't use ptrace anyway, we don't worry about this
	fine point, but it is worth noting for future reference.

	Gdb is confused about what this function is supposed to return.
	Some versions return a value, others return nothing.  Some are
	declared to return a value and actually return nothing.  Gdb ignores
	anything returned.  (FIXME)

 */

void
store_inferior_registers (regno)
     int regno;
{
  if (regno != -1)
    {
      (void) ioctl (pi.fd, PIOCGREG, &pi.gregset);
    }
  fill_gregset (&pi.gregset, regno);
  (void) ioctl (pi.fd, PIOCSREG, &pi.gregset);

#if defined (FP0_REGNUM)

  /* Now repeat everything using the floating point register set, if the
     target has floating point hardware. Since we ignore the returned value,
     we'll never know whether it worked or not anyway. */

  if (regno != -1)
    {
      (void) ioctl (pi.fd, PIOCGFPREG, &pi.fpregset);
    }
  fill_fpregset (&pi.fpregset, regno);
  (void) ioctl (pi.fd, PIOCSFPREG, &pi.fpregset);

#endif	/* FP0_REGNUM */

}

/*

GLOBAL FUNCTION

	inferior_proc_init - initialize access to a /proc entry

SYNOPSIS

	void inferior_proc_init (int pid)

DESCRIPTION

	When gdb starts an inferior, this function is called in the parent
	process immediately after the fork.  It waits for the child to stop
	on the return from the exec system call (the child itself takes care
	of ensuring that this is set up), then sets up the set of signals
	and faults that are to be traced.

NOTES

	If proc_init_failed ever gets called, control returns to the command
	processing loop via the standard error handling code.

 */

void
inferior_proc_init (pid)
     int pid;
{
  if (!open_proc_file (pid, &pi))
    {
      proc_init_failed ("can't open process file");
    }
  else
    {
      (void) memset ((char *) &pi.prrun, 0, sizeof (pi.prrun));
      prfillset (&pi.prrun.pr_trace);
      proc_signal_handling_change ();
      prfillset (&pi.prrun.pr_fault);
      prdelset (&pi.prrun.pr_fault, FLTPAGE);
      if (ioctl (pi.fd, PIOCWSTOP, &pi.prstatus) < 0)
	{
	  proc_init_failed ("PIOCWSTOP failed");
	}
      else if (ioctl (pi.fd, PIOCSFAULT, &pi.prrun.pr_fault) < 0)
	{
	  proc_init_failed ("PIOCSFAULT failed");
	}
    }
}

/*

GLOBAL FUNCTION

	proc_signal_handling_change

SYNOPSIS

	void proc_signal_handling_change (void);

DESCRIPTION

	When the user changes the state of gdb's signal handling via the
	"handle" command, this function gets called to see if any change
	in the /proc interface is required.  It is also called internally
	by other /proc interface functions to initialize the state of
	the traced signal set.

	One thing it does is that signals for which the state is "nostop",
	"noprint", and "pass", have their trace bits reset in the pr_trace
	field, so that they are no longer traced.  This allows them to be
	delivered directly to the inferior without the debugger ever being
	involved.
 */

void
proc_signal_handling_change ()
{
  int signo;

  if (pi.valid)
    {
      for (signo = 0; signo < NSIG; signo++)
	{
	  if (signal_stop_state (signo) == 0 &&
	      signal_print_state (signo) == 0 &&
	      signal_pass_state (signo) == 1)
	    {
	      prdelset (&pi.prrun.pr_trace, signo);
	    }
	  else
	    {
	      praddset (&pi.prrun.pr_trace, signo);
	    }
	}
      if (ioctl (pi.fd, PIOCSTRACE, &pi.prrun.pr_trace))
	{
	  print_sys_errmsg ("PIOCSTRACE failed", errno);
	}
    }
}

/*

GLOBAL FUNCTION

	proc_set_exec_trap -- arrange for exec'd child to halt at startup

SYNOPSIS

	void proc_set_exec_trap (void)

DESCRIPTION

	This function is called in the child process when starting up
	an inferior, prior to doing the exec of the actual inferior.
	It sets the child process's exitset to make exit from the exec
	system call an event of interest to stop on, and then simply
	returns.  The child does the exec, the system call returns, and
	the child stops at the first instruction, ready for the gdb
	parent process to take control of it.

NOTE

	We need to use all local variables since the child may be sharing
	it's data space with the parent, if vfork was used rather than
	fork.

	Also note that we want to turn off the inherit-on-fork flag in
	the child process so that any grand-children start with all
	tracing flags cleared.
 */

void
proc_set_exec_trap ()
{
  sysset_t exitset;
  auto char procname[32];
  int fd;
  
  (void) sprintf (procname, PROC_NAME_FMT, getpid ());
  if ((fd = open (procname, O_RDWR)) < 0)
    {
      perror (procname);
      fflush (stderr);
      _exit (127);
    }
  premptyset (&exitset);

  /* GW: Rationale...
     Not all systems with /proc have all the exec* syscalls with the same
     names.  On the SGI, for example, there is no SYS_exec, but there
     *is* a SYS_execv.  So, we try to account for that. */

#ifdef SYS_exec
  praddset (&exitset, SYS_exec);
#endif
#ifdef SYS_execve
  praddset (&exitset, SYS_execve);
#endif
#ifdef SYS_execv
  praddset(&exitset, SYS_execv);
#endif

  if (ioctl (fd, PIOCSEXIT, &exitset) < 0)
    {
      perror (procname);
      fflush (stderr);
      _exit (127);
    }

  /* Turn off inherit-on-fork flag so that all grand-children of gdb
     start with tracing flags cleared. */

#if defined (PIOCRESET)	/* New method */
  {
      long pr_flags;
      pr_flags = PR_FORK;
      (void) ioctl (fd, PIOCRESET, &pr_flags);
  }
#else
#if defined (PIOCRFORK)	/* Original method */
  (void) ioctl (fd, PIOCRFORK, NULL);
#endif
#endif
}

/*

GLOBAL FUNCTION

	proc_iterate_over_mappings -- call function for every mapped space

SYNOPSIS

	int proc_iterate_over_mappings (int (*func)())

DESCRIPTION

	Given a pointer to a function, call that function for every
	mapped address space, passing it an open file descriptor for
	the file corresponding to that mapped address space (if any)
	and the base address of the mapped space.  Quit when we hit
	the end of the mappings or the function returns nonzero.
 */

int
proc_iterate_over_mappings (func)
     int (*func) PARAMS ((int, CORE_ADDR));
{
  int nmap;
  int fd;
  int funcstat = 0;
  struct prmap *prmaps;
  struct prmap *prmap;
  CORE_ADDR baseaddr = 0;

  if (pi.valid && (ioctl (pi.fd, PIOCNMAP, &nmap) == 0))
    {
      prmaps = (struct prmap *) alloca ((nmap + 1) * sizeof (*prmaps));
      if (ioctl (pi.fd, PIOCMAP, prmaps) == 0)
	{
	  for (prmap = prmaps; prmap -> pr_size && funcstat == 0; ++prmap)
	    {
	      fd = proc_address_to_fd ((CORE_ADDR) prmap -> pr_vaddr, 0);
	      funcstat = (*func) (fd, (CORE_ADDR) prmap -> pr_vaddr);
	      close (fd);
	    }
	}
    }
  return (funcstat);
}

/*

GLOBAL FUNCTION

	proc_base_address -- find base address for segment containing address

SYNOPSIS

	CORE_ADDR proc_base_address (CORE_ADDR addr)

DESCRIPTION

	Given an address of a location in the inferior, find and return
	the base address of the mapped segment containing that address.

	This is used for example, by the shared library support code,
	where we have the pc value for some location in the shared library
	where we are stopped, and need to know the base address of the
	segment containing that address.
*/


#if 0	/* Currently unused */

CORE_ADDR
proc_base_address (addr)
     CORE_ADDR addr;
{
  int nmap;
  struct prmap *prmaps;
  struct prmap *prmap;
  CORE_ADDR baseaddr = 0;

  if (pi.valid && (ioctl (pi.fd, PIOCNMAP, &nmap) == 0))
    {
      prmaps = (struct prmap *) alloca ((nmap + 1) * sizeof (*prmaps));
      if (ioctl (pi.fd, PIOCMAP, prmaps) == 0)
	{
	  for (prmap = prmaps; prmap -> pr_size; ++prmap)
	    {
	      if ((prmap -> pr_vaddr <= (caddr_t) addr) &&
		  (prmap -> pr_vaddr + prmap -> pr_size > (caddr_t) addr))
		{
		  baseaddr = (CORE_ADDR) prmap -> pr_vaddr;
		  break;
		}
	    }
	}
    }
  return (baseaddr);
}

#endif	/* 0 */

/*

LOCAL FUNCTION

	proc_address_to_fd -- return open fd for file mapped to address

SYNOPSIS

	int proc_address_to_fd (CORE_ADDR addr, complain)

DESCRIPTION

	Given an address in the current inferior's address space, use the
	/proc interface to find an open file descriptor for the file that
	this address was mapped in from.  Return -1 if there is no current
	inferior.  Print a warning message if there is an inferior but
	the address corresponds to no file (IE a bogus address).

*/

static int
proc_address_to_fd (addr, complain)
     CORE_ADDR addr;
     int complain;
{
  int fd = -1;

  if (pi.valid)
    {
      if ((fd = ioctl (pi.fd, PIOCOPENM, (caddr_t *) &addr)) < 0)
	{
	  if (complain)
	    {
	      print_sys_errmsg (pi.pathname, errno);
	      warning ("can't find mapped file for address 0x%x", addr);
	    }
	}
    }
  return (fd);
}


#ifdef ATTACH_DETACH

/*

GLOBAL FUNCTION

	attach -- attach to an already existing process

SYNOPSIS

	int attach (int pid)

DESCRIPTION

	Attach to an already existing process with the specified process
	id.  If the process is not already stopped, query whether to
	stop it or not.

NOTES

	The option of stopping at attach time is specific to the /proc
	versions of gdb.  Versions using ptrace force the attachee
	to stop.

*/

int
attach (pid)
     int pid;
{
  if (!open_proc_file (pid, &pi))
    {
      perror_with_name (pi.pathname);
      /* NOTREACHED */
    }
  
  /*  Get current status of process and if it is not already stopped,
      then stop it.  Remember whether or not it was stopped when we first
      examined it. */
  
  if (ioctl (pi.fd, PIOCSTATUS, &pi.prstatus) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      close_proc_file (&pi);
      error ("PIOCSTATUS failed");
    }
  if (pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP))
    {
      pi.was_stopped = 1;
    }
  else
    {
      pi.was_stopped = 0;
      if (query ("Process is currently running, stop it? "))
	{
	  if (ioctl (pi.fd, PIOCSTOP, &pi.prstatus) < 0)
	    {
	      print_sys_errmsg (pi.pathname, errno);
	      close_proc_file (&pi);
	      error ("PIOCSTOP failed");
	    }
	  pi.nopass_next_sigstop = 1;
	}
      else
	{
	  printf ("Ok, gdb will wait for process %u to stop.\n", pid);
	}
    }
  
  /*  Remember some things about the inferior that we will, or might, change
      so that we can restore them when we detach. */
  
  (void) ioctl (pi.fd, PIOCGTRACE, &pi.saved_trace);
  (void) ioctl (pi.fd, PIOCGHOLD, &pi.saved_sighold);
  (void) ioctl (pi.fd, PIOCGFAULT, &pi.saved_fltset);
  (void) ioctl (pi.fd, PIOCGENTRY, &pi.saved_entryset);
  (void) ioctl (pi.fd, PIOCGEXIT, &pi.saved_exitset);
  
  /* Set up trace and fault sets, as gdb expects them. */
  
  (void) memset (&pi.prrun, 0, sizeof (pi.prrun));
  prfillset (&pi.prrun.pr_trace);
  proc_signal_handling_change ();
  prfillset (&pi.prrun.pr_fault);
  prdelset (&pi.prrun.pr_fault, FLTPAGE);
  if (ioctl (pi.fd, PIOCSFAULT, &pi.prrun.pr_fault))
    {
      print_sys_errmsg ("PIOCSFAULT failed", errno);
    }
  if (ioctl (pi.fd, PIOCSTRACE, &pi.prrun.pr_trace))
    {
      print_sys_errmsg ("PIOCSTRACE failed", errno);
    }
  attach_flag = 1;
  return (pid);
}

/*

GLOBAL FUNCTION

	detach -- detach from an attached-to process

SYNOPSIS

	void detach (int signal)

DESCRIPTION

	Detach from the current attachee.

	If signal is non-zero, the attachee is started running again and sent
	the specified signal.

	If signal is zero and the attachee was not already stopped when we
	attached to it, then we make it runnable again when we detach.

	Otherwise, we query whether or not to make the attachee runnable
	again, since we may simply want to leave it in the state it was in
	when we attached.

	We report any problems, but do not consider them errors, since we
	MUST detach even if some things don't seem to go right.  This may not
	be the ideal situation.  (FIXME).
 */

void
detach (signal)
     int signal;
{
  if (signal)
    {
      set_proc_siginfo (&pi, signal);
    }
  if (ioctl (pi.fd, PIOCSEXIT, &pi.saved_exitset) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSEXIT failed.\n");
    }
  if (ioctl (pi.fd, PIOCSENTRY, &pi.saved_entryset) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSENTRY failed.\n");
    }
  if (ioctl (pi.fd, PIOCSTRACE, &pi.saved_trace) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSTRACE failed.\n");
    }
  if (ioctl (pi.fd, PIOCSHOLD, &pi.saved_sighold) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOSCHOLD failed.\n");
    }
  if (ioctl (pi.fd, PIOCSFAULT, &pi.saved_fltset) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSFAULT failed.\n");
    }
  if (ioctl (pi.fd, PIOCSTATUS, &pi.prstatus) < 0)
    {
      print_sys_errmsg (pi.pathname, errno);
      printf ("PIOCSTATUS failed.\n");
    }
  else
    {
      if (signal || (pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP)))
	{
	  if (signal || !pi.was_stopped ||
	      query ("Was stopped when attached, make it runnable again? "))
	    {
	      (void) memset (&pi.prrun, 0, sizeof (pi.prrun));
	      pi.prrun.pr_flags = PRCFAULT;
	      if (ioctl (pi.fd, PIOCRUN, &pi.prrun))
		{
		  print_sys_errmsg (pi.pathname, errno);
		  printf ("PIOCRUN failed.\n");
		}
	    }
	}
    }
  close_proc_file (&pi);
  attach_flag = 0;
}

#endif	/* ATTACH_DETACH */

/*

GLOBAL FUNCTION

	proc_wait -- emulate wait() as much as possible

SYNOPSIS

	int proc_wait (int *statloc)

DESCRIPTION

	Try to emulate wait() as much as possible.  Not sure why we can't
	just use wait(), but it seems to have problems when applied to a
	process being controlled with the /proc interface.

NOTES

	We have a race problem here with no obvious solution.  We need to let
	the inferior run until it stops on an event of interest, which means
	that we need to use the PIOCWSTOP ioctl.  However, we cannot use this
	ioctl if the process is already stopped on something that is not an
	event of interest, or the call will hang indefinitely.  Thus we first
	use PIOCSTATUS to see if the process is not stopped.  If not, then we
	use PIOCWSTOP.  But during the window between the two, if the process
	stops for any reason that is not an event of interest (such as a job
	control signal) then gdb will hang.  One possible workaround is to set
	an alarm to wake up every minute of so and check to see if the process
	is still running, and if so, then reissue the PIOCWSTOP.  But this is
	a real kludge, so has not been implemented.  FIXME: investigate
	alternatives.

	FIXME:  Investigate why wait() seems to have problems with programs
	being control by /proc routines.

 */

int
proc_wait (statloc)
     int *statloc;
{
  short what;
  short why;
  int statval = 0;
  int checkerr = 0;
  int rtnval = -1;
  
  if (ioctl (pi.fd, PIOCSTATUS, &pi.prstatus) < 0)
    {
      checkerr++;
    }
  else if (!(pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP)))
    {
      if (ioctl (pi.fd, PIOCWSTOP, &pi.prstatus) < 0)
	{
	  checkerr++;
	}
    }    
  if (checkerr)
    {
      if (errno == ENOENT)
	{
	  rtnval = wait (&statval);
	  if (rtnval != inferior_pid)
	    {
	      error ("PIOCWSTOP, wait failed, returned %d", rtnval);
	      /* NOTREACHED */
	    }
	}
      else
	{
	  print_sys_errmsg (pi.pathname, errno);
	  error ("PIOCSTATUS or PIOCWSTOP failed.");
	  /* NOTREACHED */
	}
    }
  else if (pi.prstatus.pr_flags & (PR_STOPPED | PR_ISTOP))
    {
      rtnval = pi.prstatus.pr_pid;
      why = pi.prstatus.pr_why;
      what = pi.prstatus.pr_what;
      if (why == PR_SIGNALLED)
	{
	  statval = (what << 8) | 0177;
	}
      else if ((why == PR_SYSEXIT)
	       &&
	       (
#ifdef SYS_exec
		what == SYS_exec
#else
		0 == 0
#endif
#ifdef SYS_execve
		|| what == SYS_execve
#endif
#ifdef SYS_execv
		|| what == SYS_execv
#endif
		))
	{
	  statval = (SIGTRAP << 8) | 0177;
	}
      else if (why == PR_REQUESTED)
	{
	  statval = (SIGSTOP << 8) | 0177;
	}
      else if (why == PR_JOBCONTROL)
	{
	  statval = (what << 8) | 0177;
	}
      else if (why == PR_FAULTED)
	{
	  switch (what)
	    {
	    case FLTPRIV:
	    case FLTILL:
	      statval = (SIGILL << 8) | 0177;
	      break;
	    case FLTBPT:
	    case FLTTRACE:
	      statval = (SIGTRAP << 8) | 0177;
	      break;
	    case FLTSTACK:
	    case FLTACCESS:
	    case FLTBOUNDS:
	      statval = (SIGSEGV << 8) | 0177;
	      break;
	    case FLTIOVF:
	    case FLTIZDIV:
	    case FLTFPE:
	      statval = (SIGFPE << 8) | 0177;
	      break;
	    case FLTPAGE:		/* Recoverable page fault */
	    default:
	      rtnval = -1;
	      error ("PIOCWSTOP, unknown why %d, what %d", why, what);
	      /* NOTREACHED */
	    }
	}
      else
	{
	  rtnval = -1;
	  error ("PIOCWSTOP, unknown why %d, what %d", why, what);
	  /* NOTREACHED */
	}
    }
  else
    {
      error ("PIOCWSTOP, stopped for unknown/unhandled reason, flags %#x", 
	     pi.prstatus.pr_flags);
	  /* NOTREACHED */
    }
  if (statloc)
    {
      *statloc = statval;
    }
  return (rtnval);
}

/*

LOCAL FUNCTION

	set_proc_siginfo - set a process's current signal info

SYNOPSIS

	void set_proc_siginfo (struct procinfo *pip, int signo);

DESCRIPTION

	Given a pointer to a process info struct in PIP and a signal number
	in SIGNO, set the process's current signal and its associated signal
	information.  The signal will be delivered to the process immediately
	after execution is resumed, even if it is being held.  In addition,
	this particular delivery will not cause another PR_SIGNALLED stop
	even if the signal is being traced.

	If we are not delivering the same signal that the prstatus siginfo
	struct contains information about, then synthesize a siginfo struct
	to match the signal we are doing to deliver, make it of the type
	"generated by a user process", and send this synthesized copy.  When
	used to set the inferior's signal state, this will be required if we
	are not currently stopped because of a traced signal, or if we decide
	to continue with a different signal.

	Note that when continuing the inferior from a stop due to receipt
	of a traced signal, we either have set PRCSIG to clear the existing
	signal, or we have to call this function to do a PIOCSSIG with either
	the existing siginfo struct from pr_info, or one we have synthesized
	appropriately for the signal we want to deliver.  Otherwise if the
	signal is still being traced, the inferior will immediately stop
	again.

	See siginfo(5) for more details.
*/

static void
set_proc_siginfo (pip, signo)
     struct procinfo *pip;
     int signo;
{
  struct siginfo newsiginfo;
  struct siginfo *sip;

  if (pip -> valid)
    {
      if (signo == pip -> prstatus.pr_info.si_signo)
	{
	  sip = &pip -> prstatus.pr_info;
	}
      else
	{
	  (void) memset ((char *) &newsiginfo, 0, sizeof (newsiginfo));
	  sip = &newsiginfo;
	  sip -> si_signo = signo;
	  sip -> si_code = 0;
	  sip -> si_errno = 0;
	  sip -> si_pid = getpid ();
	  sip -> si_uid = getuid ();
	}
      if (ioctl (pip -> fd, PIOCSSIG, sip) < 0)
	{
	  print_sys_errmsg (pip -> pathname, errno);
	  warning ("PIOCSSIG failed");
	}
    }
}

/*

GLOBAL FUNCTION

	child_resume -- resume execution of the inferior process

SYNOPSIS

	void child_resume (int step, int signo)

DESCRIPTION

	Resume execution of the inferior process.  If STEP is nozero, then
	just single step it.  If SIGNAL is nonzero, restart it with that
	signal activated.

NOTE

	It may not be absolutely necessary to specify the PC value for
	restarting, but to be safe we use the value that gdb considers
	to be current.  One case where this might be necessary is if the
	user explicitly changes the PC value that gdb considers to be
	current.  FIXME:  Investigate if this is necessary or not.

	When attaching to a child process, if we forced it to stop with
	a PIOCSTOP, then we will have set the nopass_next_sigstop flag.
	Upon resuming the first time after such a stop, we explicitly
	inhibit sending it another SIGSTOP, which would be the normal
	result of default signal handling.  One potential drawback to
	this is that we will also ignore any attempt to by the user
	to explicitly continue after the attach with a SIGSTOP.  Ultimately
	this problem should be dealt with by making the routines that
	deal with the inferior a little smarter, and possibly even allow
	an inferior to continue running at the same time as gdb.  (FIXME?)
 */

void
child_resume (step, signo)
     int step;
     int signo;
{
  errno = 0;
  pi.prrun.pr_flags = PRSVADDR | PRSTRACE | PRSFAULT | PRCFAULT;
  pi.prrun.pr_vaddr = (caddr_t) *(int *) &registers[REGISTER_BYTE (PC_REGNUM)];
  if (signo && !(signo == SIGSTOP && pi.nopass_next_sigstop))
    {
      set_proc_siginfo (&pi, signo);
    }
  else
    {
      pi.prrun.pr_flags |= PRCSIG;
    }
  pi.nopass_next_sigstop = 0;
  if (step)
    {
      pi.prrun.pr_flags |= PRSTEP;
    }
  if (ioctl (pi.fd, PIOCRUN, &pi.prrun) != 0)
    {
      perror_with_name (pi.pathname);
      /* NOTREACHED */
    }
}

/*

GLOBAL FUNCTION

	fetch_inferior_registers -- fetch current registers from inferior

SYNOPSIS

	void fetch_inferior_registers (int regno)

DESCRIPTION

	Read the current values of the inferior's registers, both the
	general register set and floating point registers (if supported)
	and update gdb's idea of their current values.

*/

void
fetch_inferior_registers (regno)
     int regno;
{
  if (ioctl (pi.fd, PIOCGREG, &pi.gregset) != -1)
    {
      supply_gregset (&pi.gregset);
    }
#if defined (FP0_REGNUM)
  if (ioctl (pi.fd, PIOCGFPREG, &pi.fpregset) != -1)
    {
      supply_fpregset (&pi.fpregset);
    }
#endif
}

/*

GLOBAL FUNCTION

	fetch_core_registers -- fetch current registers from core file data

SYNOPSIS

	void fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
				   int which, unsigned in reg_addr)

DESCRIPTION

	Read the values of either the general register set (WHICH equals 0)
	or the floating point register set (WHICH equals 2) from the core
	file data (pointed to by CORE_REG_SECT), and update gdb's idea of
	their current values.  The CORE_REG_SIZE parameter is ignored.

NOTES

	Use the indicated sizes to validate the gregset and fpregset
	structures.
*/

void
fetch_core_registers (core_reg_sect, core_reg_size, which, reg_addr)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     unsigned int reg_addr;	/* Unused in this version */
{

  if (which == 0)
    {
      if (core_reg_size != sizeof (pi.gregset))
	{
	  warning ("wrong size gregset struct in core file");
	}
      else
	{
	  (void) memcpy ((char *) &pi.gregset, core_reg_sect,
			 sizeof (pi.gregset));
	  supply_gregset (&pi.gregset);
	}
    }
  else if (which == 2)
    {
      if (core_reg_size != sizeof (pi.fpregset))
	{
	  warning ("wrong size fpregset struct in core file");
	}
      else
	{
	  (void) memcpy ((char *) &pi.fpregset, core_reg_sect,
			 sizeof (pi.fpregset));
#if defined (FP0_REGNUM)
	  supply_fpregset (&pi.fpregset);
#endif
	}
    }
}

/*

LOCAL FUNCTION

	proc_init_failed - called whenever /proc access initialization fails

SYNOPSIS

	static void proc_init_failed (char *why)

DESCRIPTION

	This function is called whenever initialization of access to a /proc
	entry fails.  It prints a suitable error message, does some cleanup,
	and then invokes the standard error processing routine which dumps
	us back into the command loop.
 */

static void
proc_init_failed (why)
     char *why;
{
  print_sys_errmsg (pi.pathname, errno);
  (void) kill (pi.pid, SIGKILL);
  close_proc_file (&pi);
  error (why);
  /* NOTREACHED */
}

/*

LOCAL FUNCTION

	close_proc_file - close any currently open /proc entry

SYNOPSIS

	static void close_proc_file (struct procinfo *pip)

DESCRIPTION

	Close any currently open /proc entry and mark the process information
	entry as invalid.  In order to ensure that we don't try to reuse any
	stale information, the pid, fd, and pathnames are explicitly
	invalidated, which may be overkill.

 */

static void
close_proc_file (pip)
     struct procinfo *pip;
{
  pip -> pid = 0;
  if (pip -> valid)
    {
      (void) close (pip -> fd);
    }
  pip -> fd = -1;
  if (pip -> pathname)
    {
      free (pip -> pathname);
      pip -> pathname = NULL;
    }
  pip -> valid = 0;
}

/*

LOCAL FUNCTION

	open_proc_file - open a /proc entry for a given process id

SYNOPSIS

	static int open_proc_file (pid, struct procinfo *pip)

DESCRIPTION

	Given a process id, close the existing open /proc entry (if any)
	and open one for the new process id.  Once it is open, then
	mark the local process information structure as valid, which
	guarantees that the pid, fd, and pathname fields match an open
	/proc entry.  Returns zero if the open fails, nonzero otherwise.

	Note that the pathname is left intact, even when the open fails,
	so that callers can use it to construct meaningful error messages
	rather than just "file open failed".
 */

static int
open_proc_file (pid, pip)
     int pid;
     struct procinfo *pip;
{
  pip -> valid = 0;
  if (pip -> valid)
    {
      (void) close (pip -> fd);
    }
  if (pip -> pathname == NULL)
    {
      pip -> pathname = xmalloc (32);
    }
  sprintf (pip -> pathname, PROC_NAME_FMT, pid);
  if ((pip -> fd = open (pip -> pathname, O_RDWR)) >= 0)
    {
      pip -> valid = 1;
      pip -> pid = pid;
    }
  return (pip -> valid);
}

static char *
mappingflags (flags)
     long flags;
{
  static char asciiflags[8];
  
  strcpy (asciiflags, "-------");
#if defined (MA_PHYS)
  if (flags & MA_PHYS)   asciiflags[0] = 'd';
#endif
  if (flags & MA_STACK)  asciiflags[1] = 's';
  if (flags & MA_BREAK)  asciiflags[2] = 'b';
  if (flags & MA_SHARED) asciiflags[3] = 's';
  if (flags & MA_READ)   asciiflags[4] = 'r';
  if (flags & MA_WRITE)  asciiflags[5] = 'w';
  if (flags & MA_EXEC)   asciiflags[6] = 'x';
  return (asciiflags);
}

static void
info_proc_flags (pip, summary)
     struct procinfo *pip;
     int summary;
{
  struct trans *transp;

  printf_filtered ("%-32s", "Process status flags:");
  if (!summary)
    {
      printf_filtered ("\n\n");
    }
  for (transp = pr_flag_table; transp -> name != NULL; transp++)
    {
      if (pip -> prstatus.pr_flags & transp -> value)
	{
	  if (summary)
	    {
	      printf_filtered ("%s ", transp -> name);
	    }
	  else
	    {
	      printf_filtered ("\t%-16s %s.\n", transp -> name, transp -> desc);
	    }
	}
    }
  printf_filtered ("\n");
}

static void
info_proc_stop (pip, summary)
     struct procinfo *pip;
     int summary;
{
  struct trans *transp;
  int why;
  int what;

  why = pip -> prstatus.pr_why;
  what = pip -> prstatus.pr_what;

  if (pip -> prstatus.pr_flags & PR_STOPPED)
    {
      printf_filtered ("%-32s", "Reason for stopping:");
      if (!summary)
	{
	  printf_filtered ("\n\n");
	}
      for (transp = pr_why_table; transp -> name != NULL; transp++)
	{
	  if (why == transp -> value)
	    {
	      if (summary)
		{
		  printf_filtered ("%s ", transp -> name);
		}
	      else
		{
		  printf_filtered ("\t%-16s %s.\n",
				   transp -> name, transp -> desc);
		}
	      break;
	    }
	}
      
      /* Use the pr_why field to determine what the pr_what field means, and
	 print more information. */
      
      switch (why)
	{
	  case PR_REQUESTED:
	    /* pr_what is unused for this case */
	    break;
	  case PR_JOBCONTROL:
	  case PR_SIGNALLED:
	    if (summary)
	      {
		printf_filtered ("%s ", signalname (what));
	      }
	    else
	      {
		printf_filtered ("\t%-16s %s.\n", signalname (what),
				 sys_siglist[what]);
	      }
	    break;
	  case PR_SYSENTRY:
	    if (summary)
	      {
		printf_filtered ("%s ", syscallname (what));
	      }
	    else
	      {
		printf_filtered ("\t%-16s %s.\n", syscallname (what),
				 "Entered this system call");
	      }
	    break;
	  case PR_SYSEXIT:
	    if (summary)
	      {
		printf_filtered ("%s ", syscallname (what));
	      }
	    else
	      {
		printf_filtered ("\t%-16s %s.\n", syscallname (what),
				 "Returned from this system call");
	      }
	    break;
	  case PR_FAULTED:
	    if (summary)
	      {
		printf_filtered ("%s ",
				 lookupname (faults_table, what, "fault"));
	      }
	    else
	      {
		printf_filtered ("\t%-16s %s.\n",
				 lookupname (faults_table, what, "fault"),
				 lookupdesc (faults_table, what));
	      }
	    break;
	  }
      printf_filtered ("\n");
    }
}

static void
info_proc_siginfo (pip, summary)
     struct procinfo *pip;
     int summary;
{
  struct siginfo *sip;

  if ((pip -> prstatus.pr_flags & PR_STOPPED) &&
      (pip -> prstatus.pr_why == PR_SIGNALLED ||
       pip -> prstatus.pr_why == PR_FAULTED))
    {
      printf_filtered ("%-32s", "Additional signal/fault info:");
      sip = &pip -> prstatus.pr_info;
      if (summary)
	{
	  printf_filtered ("%s ", signalname (sip -> si_signo));
	  if (sip -> si_errno > 0)
	    {
	      printf_filtered ("%s ", lookupname (errno_table,
						  sip -> si_errno, "errno"));
	    }
	  if (sip -> si_code <= 0)
	    {
	      printf_filtered ("sent by pid %d, uid %d ", sip -> si_pid,
			       sip -> si_uid);
	    }
	  else
	    {
	      printf_filtered ("%s ", sigcodename (sip));
	      if ((sip -> si_signo == SIGILL) ||
		  (sip -> si_signo == SIGFPE) ||
		  (sip -> si_signo == SIGSEGV) ||
		  (sip -> si_signo == SIGBUS))
		{
		  printf_filtered ("addr=%#x ", sip -> si_addr);
		}
	      else if ((sip -> si_signo == SIGCHLD))
		{
		  printf_filtered ("child pid %u, status %u ",
				   sip -> si_pid,
				   sip -> si_status);
		}
	      else if ((sip -> si_signo == SIGPOLL))
		{
		  printf_filtered ("band %u ", sip -> si_band);
		}
	    }
	}
      else
	{
	  printf_filtered ("\n\n");
	  printf_filtered ("\t%-16s %s.\n", signalname (sip -> si_signo),
			   sys_siglist[sip -> si_signo]);
	  if (sip -> si_errno > 0)
	    {
	      printf_filtered ("\t%-16s %s.\n",
			       lookupname (errno_table,
					   sip -> si_errno, "errno"),
			       lookupdesc (errno_table, sip -> si_errno));
	    }
	  if (sip -> si_code <= 0)
	    {
	      printf_filtered ("\t%-16u %s\n", sip -> si_pid,
			       "PID of process sending signal");
	      printf_filtered ("\t%-16u %s\n", sip -> si_uid,
			       "UID of process sending signal");
	    }
	  else
	    {
	      printf_filtered ("\t%-16s %s.\n", sigcodename (sip),
			       sigcodedesc (sip));
	      if ((sip -> si_signo == SIGILL) ||
		  (sip -> si_signo == SIGFPE))
		{
		  printf_filtered ("\t%-16#x %s.\n", sip -> si_addr,
				   "Address of faulting instruction");
		}
	      else if ((sip -> si_signo == SIGSEGV) ||
		       (sip -> si_signo == SIGBUS))
		{
		  printf_filtered ("\t%-16#x %s.\n", sip -> si_addr,
				   "Address of faulting memory reference");
		}
	      else if ((sip -> si_signo == SIGCHLD))
		{
		  printf_filtered ("\t%-16u %s.\n", sip -> si_pid,
				   "Child process ID");
		  printf_filtered ("\t%-16u %s.\n", sip -> si_status,
				   "Child process exit value or signal");
		}
	      else if ((sip -> si_signo == SIGPOLL))
		{
		  printf_filtered ("\t%-16u %s.\n", sip -> si_band,
				   "Band event for POLL_{IN,OUT,MSG}");
		}
	    }
	}
      printf_filtered ("\n");
    }
}

static void
info_proc_syscalls (pip, summary)
     struct procinfo *pip;
     int summary;
{
  int syscallnum;

  if (!summary)
    {

#if 0	/* FIXME:  Needs to use gdb-wide configured info about system calls. */
      if (pip -> prstatus.pr_flags & PR_ASLEEP)
	{
	  int syscallnum = pip -> prstatus.pr_reg[R_D0];
	  if (summary)
	    {
	      printf_filtered ("%-32s", "Sleeping in system call:");
	      printf_filtered ("%s", syscallname (syscallnum));
	    }
	  else
	    {
	      printf_filtered ("Sleeping in system call '%s'.\n",
			       syscallname (syscallnum));
	    }
	}
#endif

      if (ioctl (pip -> fd, PIOCGENTRY, &pip -> entryset) < 0)
	{
	  print_sys_errmsg (pip -> pathname, errno);
	  error ("PIOCGENTRY failed");
	}
      
      if (ioctl (pip -> fd, PIOCGEXIT, &pip -> exitset) < 0)
	{
	  print_sys_errmsg (pip -> pathname, errno);
	  error ("PIOCGEXIT failed");
	}
      
      printf_filtered ("System call tracing information:\n\n");
      
      printf_filtered ("\t%-12s %-8s %-8s\n",
		       "System call",
		       "Entry",
		       "Exit");
      for (syscallnum = 0; syscallnum < MAX_SYSCALLS; syscallnum++)
	{
	  QUIT;
	  if (syscall_table[syscallnum] != NULL)
	    {
	      printf_filtered ("\t%-12s ", syscall_table[syscallnum]);
	      printf_filtered ("%-8s ",
			       prismember (&pip -> entryset, syscallnum)
			       ? "on" : "off");
	      printf_filtered ("%-8s ",
			       prismember (&pip -> exitset, syscallnum)
			       ? "on" : "off");
	      printf_filtered ("\n");
	    }
	  }
      printf_filtered ("\n");
    }
}

static char *
signalname (signo)
     int signo;
{
  char *abbrev;
  static char locbuf[32];

  abbrev = sig_abbrev (signo);
  if (abbrev == NULL)
    {
      sprintf (locbuf, "signal %d", signo);
    }
  else
    {
      sprintf (locbuf, "SIG%s (%d)", abbrev, signo);
    }
  return (locbuf);
}

static void
info_proc_signals (pip, summary)
     struct procinfo *pip;
     int summary;
{
  int signo;

  if (!summary)
    {
      if (ioctl (pip -> fd, PIOCGTRACE, &pip -> trace) < 0)
	{
	  print_sys_errmsg (pip -> pathname, errno);
	  error ("PIOCGTRACE failed");
	}
      
      printf_filtered ("Disposition of signals:\n\n");
      printf_filtered ("\t%-15s %-8s %-8s %-8s  %s\n\n",
		       "Signal", "Trace", "Hold", "Pending", "Description");
      for (signo = 0; signo < NSIG; signo++)
	{
	  QUIT;
	  printf_filtered ("\t%-15s ", signalname (signo));
	  printf_filtered ("%-8s ",
			   prismember (&pip -> trace, signo)
			   ? "on" : "off");
	  printf_filtered ("%-8s ",
			   prismember (&pip -> prstatus.pr_sighold, signo)
			   ? "on" : "off");
	  printf_filtered ("%-8s ",
			   prismember (&pip -> prstatus.pr_sigpend, signo)
			   ? "yes" : "no");
	  printf_filtered (" %s\n", sys_siglist[signo]);
	}
      printf_filtered ("\n");
    }
}

static void
info_proc_faults (pip, summary)
     struct procinfo *pip;
     int summary;
{
  struct trans *transp;

  if (!summary)
    {
      if (ioctl (pip -> fd, PIOCGFAULT, &pip -> fltset) < 0)
	{
	  print_sys_errmsg (pip -> pathname, errno);
	  error ("PIOCGFAULT failed");
	}
      
      printf_filtered ("Current traced hardware fault set:\n\n");
      printf_filtered ("\t%-12s %-8s\n", "Fault", "Trace");

      for (transp = faults_table; transp -> name != NULL; transp++)
	{
	  QUIT;
	  printf_filtered ("\t%-12s ", transp -> name);
	  printf_filtered ("%-8s", prismember (&pip -> fltset, transp -> value)
			   ? "on" : "off");
	  printf_filtered ("\n");
	}
      printf_filtered ("\n");
    }
}

static void
info_proc_mappings (pip, summary)
     struct procinfo *pip;
     int summary;
{
  int nmap;
  struct prmap *prmaps;
  struct prmap *prmap;

  if (!summary)
    {
      printf_filtered ("Mapped address spaces:\n\n");
      printf_filtered ("\t%10s %10s %10s %10s %7s\n",
		       "Start Addr",
		       "  End Addr",
		       "      Size",
		       "    Offset",
		       "Flags");
      if (ioctl (pip -> fd, PIOCNMAP, &nmap) == 0)
	{
	  prmaps = (struct prmap *) alloca ((nmap + 1) * sizeof (*prmaps));
	  if (ioctl (pip -> fd, PIOCMAP, prmaps) == 0)
	    {
	      for (prmap = prmaps; prmap -> pr_size; ++prmap)
		{
		  printf_filtered ("\t%#10x %#10x %#10x %#10x %7s\n",
				   prmap -> pr_vaddr,
				   prmap -> pr_vaddr + prmap -> pr_size - 1,
				   prmap -> pr_size,
				   prmap -> pr_off,
				   mappingflags (prmap -> pr_mflags));
		}
	    }
	}
      printf_filtered ("\n");
    }
}

/*

LOCAL FUNCTION

	info_proc -- implement the "info proc" command

SYNOPSIS

	void info_proc (char *args, int from_tty)

DESCRIPTION

	Implement gdb's "info proc" command by using the /proc interface
	to print status information about any currently running process.

	Examples of the use of "info proc" are:

	info proc		(prints summary info for current inferior)
	info proc 123		(prints summary info for process with pid 123)
	info proc mappings	(prints address mappings)
	info proc times		(prints process/children times)
	info proc id		(prints pid, ppid, gid, sid, etc)
	info proc status	(prints general process state info)
	info proc signals	(prints info about signal handling)
	info proc all		(prints all info)

 */

static void
info_proc (args, from_tty)
     char *args;
     int from_tty;
{
  int pid;
  struct procinfo pii;
  struct procinfo *pip;
  struct cleanup *old_chain;
  char *nexttok;
  char **argv;
  int argsize;
  int summary = 1;
  int flags = 0;
  int syscalls = 0;
  int signals = 0;
  int faults = 0;
  int mappings = 0;
  int times = 0;
  int id = 0;
  int status = 0;
  int all = 0;

  old_chain = make_cleanup (null_cleanup, 0);

  /* Default to using the current inferior if no pid specified */

  pip = &pi;

  if (args != NULL)
    {
      if ((argv = buildargv (args)) == NULL)
	{
	  nomem (0);
	}
      make_cleanup (freeargv, (char *) argv);

      while (*argv != NULL)
	{
	  argsize = strlen (*argv);
	  if (argsize >= 1 && strncmp (*argv, "all", argsize) == 0)
	    {
	      summary = 0;
	      all = 1;
	    }
	  else if (argsize >= 2 && strncmp (*argv, "faults", argsize) == 0)
	    {
	      summary = 0;
	      faults = 1;
	    }
	  else if (argsize >= 2 && strncmp (*argv, "flags", argsize) == 0)
	    {
	      summary = 0;
	      flags = 1;
	    }
	  else if (argsize >= 1 && strncmp (*argv, "id", argsize) == 0)
	    {
	      summary = 0;
	      id = 1;
	    }
	  else if (argsize >= 1 && strncmp (*argv, "mappings", argsize) == 0)
	    {
	      summary = 0;
	      mappings = 1;
	    }
	  else if (argsize >= 2 && strncmp (*argv, "signals", argsize) == 0)
	    {
	      summary = 0;
	      signals = 1;
	    }
	  else if (argsize >= 2 && strncmp (*argv, "status", argsize) == 0)
	    {
	      summary = 0;
	      status = 1;
	    }
	  else if (argsize >= 2 && strncmp (*argv, "syscalls", argsize) == 0)
	    {
	      summary = 0;
	      syscalls = 1;
	    }
	  else if (argsize >= 1 && strncmp (*argv, "times", argsize) == 0)
	    {
	      summary = 0;
	      times = 1;
	    }
	  else if ((pii.pid = atoi (*argv)) > 0)
	    {
	      pid = pii.pid;
	      pip = &pii;
	      (void) memset (&pii, 0, sizeof (pii));
	      if (!open_proc_file (pid, pip))
		{
		  perror_with_name (pip -> pathname);
		  /* NOTREACHED */
		}
	      make_cleanup (close_proc_file, pip);
	    }
	  else if (**argv != '\000')
	    {
	      error ("Unrecognized or ambiguous keyword `%s'.", *argv);
	    }
	  argv++;
	}
    }

  /* If we don't have a valid open process at this point, then we have no
     inferior or didn't specify a specific pid. */

  if (!pip -> valid)
    {
      error ("No process.  Run an inferior or specify an explicit pid.");
    }
  if (ioctl (pip -> fd, PIOCSTATUS, &(pip -> prstatus)) < 0)
    {
      print_sys_errmsg (pip -> pathname, errno);
      error ("PIOCSTATUS failed");
    }

  /* Print verbose information of the requested type(s), or just a summary
     of the information for all types. */

  printf_filtered ("\nInformation for %s:\n\n", pip -> pathname);
  if (summary || all || flags)
    {
      info_proc_flags (pip, summary);
    }
  if (summary || all)
    {
      info_proc_stop (pip, summary);
    }
  if (summary || all || signals || faults)
    {
      info_proc_siginfo (pip, summary);
    }
  if (summary || all || syscalls)
    {
      info_proc_syscalls (pip, summary);
    }
  if (summary || all || mappings)
    {
      info_proc_mappings (pip, summary);
    }
  if (summary || all || signals)
    {
      info_proc_signals (pip, summary);
    }
  if (summary || all || faults)
    {
      info_proc_faults (pip, summary);
    }
  printf_filtered ("\n");

  /* All done, deal with closing any temporary process info structure,
     freeing temporary memory , etc. */

  do_cleanups (old_chain);
}

/*

GLOBAL FUNCTION

	_initialize_proc_fs -- initialize the process file system stuff

SYNOPSIS

	void _initialize_proc_fs (void)

DESCRIPTION

	Do required initializations during gdb startup for using the
	/proc file system interface.

*/

static char *proc_desc =
"Show process status information using /proc entry.\n\
Specify process id or use current inferior by default.\n\
Specify keywords for detailed information; default is summary.\n\
Keywords are: `all', `faults', `flags', `id', `mappings', `signals',\n\
`status', `syscalls', and `times'.\n\
Unambiguous abbreviations may be used.";

void
_initialize_proc_fs ()
{
  add_info ("proc", info_proc, proc_desc);
  init_syscall_table ();
}

#endif	/* USE_PROC_FS */
