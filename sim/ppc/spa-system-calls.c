/*
 * Copyright (C) 1991 Gordon Irlam.  All rights reserved.
 */

/*
 * Definition of system calls for sparc trace generator.
 */


/*
 * Imported declarations.
 */

#include "system_calls.h"


/*
 * Table containing system calls, and their parameter profile.
 */

call_desc system_calls[] = {
    /*   0 */ {"syscall", {fmt_dec, fmt_unknown}, fmt_dec},
    /*   1 */ {"_exit", {fmt_dec}, fmt_none},
    /*   2 */ {"fork", {fmt_none}, fmt_dec},
    /*   3 */ {"read", {fmt_fd, fmt_post_data, FMT_SIZE}, fmt_post_data_size},
    /*   4 */ {"write", {fmt_fd, fmt_data, fmt_data_size}, FMT_SIZE},
    /*   5 */ {"open", {fmt_string, fmt_open_flags, FMT_FLAGS}, fmt_fd},
    /*   6 */ {"close", {fmt_fd}, FMT_STATUS},
    /*   7 */ {"wait4", {fmt_dec, fmt_ptr, FMT_FLAGS, fmt_ptr}, fmt_dec},
    /*   8 */ {"creat", {fmt_string, FMT_FLAGS}, fmt_fd},
    /*   9 */ {"link", {fmt_string, fmt_string}, FMT_STATUS},
    /*  10 */ {"unlink", {fmt_string}, FMT_STATUS},
    /*  11 */ {"_unknown_11[\"old execv\"]", {fmt_unknown}, fmt_unknown},
              /*
               * execv is now a library routine which calls execve, although
               * Sun have not officially declared execv obsolete.
               */
    /*  12 */ {"chdir", {fmt_string}, FMT_STATUS},
    /*  13 */ {"_unknown_13[\"old time\"]", {fmt_unknown}, fmt_unknown},
    /*  14 */ {"mknod", {fmt_string, FMT_FLAGS, FMT_FLAGS}, FMT_STATUS},
    /*  15 */ {"chmod", {fmt_string, FMT_FLAGS}, FMT_STATUS},
    /*  16 */ {"chown", {fmt_string, fmt_dec, fmt_dec}, FMT_STATUS},
    /*  17 */ {"_brk", {fmt_ptr}, FMT_STATUS},
    /*  18 */ {"_unknown_18[\"old stat\"]", {fmt_unknown}, fmt_unknown},
    /*  19 */ {"lseek", {fmt_fd, FMT_SIZE, fmt_dec}, FMT_SIZE},
    /*  20 */ {"getpid", {fmt_none}, fmt_dec},
    /*  21 */ {"_unknown_21", {fmt_unknown}, fmt_unknown},
    /*  22 */ {"umount[\"System V\"]", {fmt_string}, FMT_STATUS},
    /*  23 */ {"_unknown_23[\"old setuid\"]", {fmt_unknown}, fmt_unknown},
    /*  24 */ {"getuid", {fmt_none}, fmt_dec},
    /*  25 */ {"_unknown_25[\"old System V stime\"]",
               {fmt_unknown},
               fmt_unknown},
    /*  26 */ {"ptrace",
               {fmt_dec, fmt_dec, fmt_ptr, fmt_dec, fmt_ptr},
               fmt_dec},
    /*  27 */ {"_unknown_27[\"old alarm\"]", {fmt_unknown}, fmt_unknown},
    /*  28 */ {"_unknown_28[\"old fstat\"]", {fmt_unknown}, fmt_unknown},
    /*  29 */ {"_unknown_29[\"old pause\"]", {fmt_unknown}, fmt_unknown},
    /*  30 */ {"_unknown_30[\"old utime\"]", {fmt_unknown}, fmt_unknown},
    /*  31 */ {"_unknown_31", {fmt_unknown}, fmt_unknown},
    /*  32 */ {"_unknown_32", {fmt_unknown}, fmt_unknown},
    /*  33 */ {"access", {fmt_string, FMT_FLAGS}, FMT_STATUS},
    /*  34 */ {"_unknown_34[\"old nice\"]", {fmt_unknown}, fmt_unknown},
    /*  35 */ {"_unknown_35[\"old ftime\"]", {fmt_unknown}, fmt_unknown},
    /*  36 */ {"sync", {fmt_none}, fmt_none},
    /*  37 */ {"kill", {fmt_dec, fmt_signal}, FMT_STATUS},
    /*  38 */ {"stat", {fmt_string, fmt_ptr}, FMT_STATUS},
    /*  39 */ {"_unknown_39[\"old setpgrp\"]", {fmt_unknown}, fmt_unknown},
    /*  40 */ {"lstat", {fmt_string, fmt_ptr}, FMT_STATUS},
    /*  41 */ {"dup", {fmt_fd}, fmt_fd},
              /*
               * Sun sometimes claim dup has 2 parameters.
               */
    /*  42 */ {"pipe", {fmt_ptr}, FMT_STATUS},
    /*  43 */ {"_unknown_43[\"old times\"]", {fmt_unknown}, fmt_unknown},
    /*  44 */ {"profil", {fmt_ptr, FMT_SIZE, fmt_ptr, fmt_dec}, FMT_STATUS},
    /*  45 */ {"_unknown_45", {fmt_unknown}, fmt_unknown},
    /*  46 */ {"_unknown_46[\"old setgid\"]", {fmt_unknown}, fmt_unknown},
    /*  47 */ {"getgid", {fmt_none}, fmt_dec},
    /*  48 */ {"_unknown_48[\"old signal\"]", {fmt_unknown}, fmt_unknown},
    /*  49 */ {"_unknown_49", {fmt_unknown}, fmt_unknown},
    /*  50 */ {"_unknown_50", {fmt_unknown}, fmt_unknown},
    /*  51 */ {"acct", {fmt_string}, FMT_STATUS},
    /*  52 */ {"_unknown_52", {fmt_unknown}, fmt_unknown},
    /*  53 */ {"mctl", {fmt_ptr, FMT_SIZE, fmt_dec, FMT_FLAGS}, FMT_STATUS},
    /*  54 */ {"ioctl", {fmt_fd, FMT_FLAGS, fmt_ptr}, fmt_dec},
    /*  55 */ {"reboot", {FMT_FLAGS, fmt_string}, FMT_STATUS},
    /*  56 */ {"_unknown_56[\"old wait3\"]", {fmt_unknown}, fmt_unknown},
    /*  57 */ {"symlink", {fmt_string, fmt_string}, FMT_STATUS},
    /*  58 */ {"readlink",
               {fmt_string, fmt_post_data, FMT_SIZE},
               fmt_post_data_size},
    /*  59 */ {"execve",
               {fmt_string, fmt_string_array, fmt_string_array},
               FMT_STATUS},
    /*  60 */ {"umask", {FMT_FLAGS}, FMT_FLAGS},
    /*  61 */ {"chroot", {fmt_string}, FMT_STATUS},
    /*  62 */ {"fstat", {fmt_fd, fmt_ptr}, FMT_STATUS},
    /*  63 */ {"_unknown_63", {fmt_unknown}, fmt_unknown},
    /*  64 */ {"getpagesize", {fmt_none}, FMT_SIZE},
    /*  65 */ {"_unknown_65[\"old msync\"]", {fmt_unknown}, fmt_unknown},
              /*
               * msync is now a library routine which calls mctl, although
               * Sun have not officially declared msync obsolete.
               */
    /*  66 */ {"vfork", {fmt_none}, fmt_dec},
    /*  67 */ {"_unknown_67[\"old vread\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think vread can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /*  68 */ {"_unknown_68[\"old vwrite\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think vwrite can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /*  69 */ {"_unknown_69[\"old brk\"]", {fmt_unknown}, fmt_unknown},
              /*
               * Also referred to as sbrk.  I don't think it can be generated
               * by the standard libararies, although Sun have not officially
               * declared it obsolete.
               */
    /*  70 */ {"_unknown_70[\"old sstk\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think sstk can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /*  71 */ {"mmap",
               {fmt_ptr, fmt_post_data_size, FMT_FLAGS, FMT_FLAGS, fmt_fd,
                FMT_SIZE},
               fmt_post_data},
    /*  72 */ {"vadvise", {fmt_dec}, FMT_STATUS},
              /*
               * vadvise is currently still a valid system call, although Sun
               * have said it is likely to disappear in the future.
               */
    /*  73 */ {"munmap", {fmt_ptr, FMT_SIZE}, FMT_STATUS},
    /*  74 */ {"mprotect", {fmt_ptr, FMT_SIZE, FMT_FLAGS}, FMT_STATUS},
    /*  75 */ {"_unknown_75[\"old madvise\"]", {fmt_unknown}, fmt_unknown},
              /*
               * madvise is now a library routine which calls mctl, although
               * Sun have not officially declared madvise obsolete.
               */
    /*  76 */ {"vhangup", {fmt_none}, FMT_STATUS},
              /*
               * Sun sometimes claim vhangup has 1 parameter.
               */
    /*  77 */ {"_unknown_77[\"old vlimit\"]", {fmt_unknown}, fmt_unknown},
    /*  78 */ {"mincore", {fmt_ptr, FMT_SIZE, fmt_ptr}, FMT_STATUS},
    /*  79 */ {"getgroups", {fmt_dec, fmt_ptr}, fmt_dec},
    /*  80 */ {"setgroups", {fmt_dec, fmt_ptr}, FMT_STATUS},
    /*  81 */ {"getpgrp", {fmt_dec}, fmt_dec},
    /*  82 */ {"setpgrp", {fmt_dec, fmt_dec}, FMT_STATUS},
    /*  83 */ {"setitimer", {fmt_dec, fmt_ptr, fmt_ptr}, FMT_STATUS},
    /*  84 */ {"_unknown_84[\"old wait\"]", {fmt_unknown}, fmt_unknown},
              /*
               * wait is now a library routine which calls wait4, although Sun
               * have not officially declared wait obsolete.
               */
    /*  85 */ {"swapon", {fmt_string}, FMT_STATUS},
    /*  86 */ {"getitimer", {fmt_dec, fmt_ptr}, FMT_STATUS},
    /*  87 */ {"gethostname", {fmt_post_string, FMT_SIZE}, FMT_STATUS},
    /*  88 */ {"sethostname", {fmt_data, fmt_data_size}, FMT_STATUS},
    /*  89 */ {"getdtablesize", {fmt_none}, fmt_dec},
    /*  90 */ {"dup2", {fmt_fd, fmt_dec}, fmt_fd},
    /*  91 */ {"_unknown_91[\"old getdopt\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think getdopt can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /*  92 */ {"fcntl", {fmt_fd, fmt_dec, fmt_dec}, fmt_dec},
    /*  93 */ {"select",
               {fmt_dec, fmt_ptr, fmt_ptr, fmt_ptr, fmt_ptr},
               fmt_dec},
    /*  94 */ {"_unknown_94[\"old setdopt\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think setdopt can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /*  95 */ {"fsync", {fmt_fd}, FMT_STATUS},
    /*  96 */ {"setpriority", {fmt_dec, fmt_dec, fmt_dec}, FMT_STATUS},
    /*  97 */ {"socket", {fmt_dec, fmt_dec, fmt_dec}, fmt_fd},
    /*  98 */ {"connect", {fmt_fd, fmt_ptr, FMT_SIZE}, FMT_STATUS},
    /*  99 */ {"accept", {fmt_fd, fmt_ptr, fmt_ptr}, fmt_fd},
    /* 100 */ {"getpriority", {fmt_dec, fmt_dec}, fmt_dec},
    /* 101 */ {"send", {fmt_fd, fmt_data, fmt_data_size, FMT_FLAGS}, FMT_SIZE},
    /* 102 */ {"recv",
               {fmt_fd, fmt_post_data, FMT_SIZE, FMT_FLAGS},
               fmt_post_data_size},
    /* 103 */ {"_unknown_103", {fmt_unknown}, fmt_unknown},
    /* 104 */ {"bind", {fmt_fd, fmt_ptr, FMT_SIZE}, FMT_STATUS},
    /* 105 */ {"setsockopt",
               {fmt_fd, fmt_dec, fmt_dec, fmt_ptr, FMT_SIZE},
               FMT_STATUS},
    /* 106 */ {"listen", {fmt_fd, fmt_dec}, FMT_STATUS},
    /* 107 */ {"_unknown_107[\"old vtimes\"]", {fmt_unknown}, fmt_unknown},
    /* 108 */ {"_sigvec", {fmt_signal, fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 109 */ {"sigblock", {fmt_hex}, fmt_hex},
    /* 110 */ {"sigsetmask", {fmt_hex}, fmt_hex},
    /* 111 */ {"sigpause", {fmt_hex}, FMT_STATUS},
    /* 112 */ {"sigstack", {fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 113 */ {"recvmsg", {fmt_fd, fmt_ptr, FMT_FLAGS}, FMT_SIZE},
    /* 114 */ {"sendmsg", {fmt_fd, fmt_ptr, FMT_FLAGS}, FMT_SIZE},
    /* 115 */ {"_unknown_115[\"vtrace\"]",
               {fmt_dec, fmt_hex, fmt_hex},
               fmt_unknown},
              /*
               * I am unsure of the parameters for vtrace.
               */
    /* 116 */ {"gettimeofday", {fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 117 */ {"getrusage", {fmt_dec, fmt_ptr}, FMT_STATUS},
    /* 118 */ {"getsockopt",
               {fmt_fd, fmt_dec, fmt_dec, fmt_ptr, fmt_ptr},
               FMT_STATUS},
    /* 119 */ {"_unknown_119", {fmt_unknown}, fmt_unknown},
    /* 120 */ {"readv", {fmt_fd, fmt_ptr, fmt_dec}, FMT_SIZE},
    /* 121 */ {"writev", {fmt_fd, fmt_ptr, fmt_dec}, FMT_SIZE},
    /* 122 */ {"settimeofday", {fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 123 */ {"fchown", {fmt_fd, fmt_dec, fmt_dec}, FMT_STATUS},
    /* 124 */ {"fchmod", {fmt_fd, FMT_FLAGS}, FMT_STATUS},
    /* 125 */ {"recvfrom",
               {fmt_fd, fmt_post_data, FMT_SIZE, FMT_FLAGS, fmt_ptr, fmt_ptr},
               fmt_post_data_size},
    /* 126 */ {"setreuid", {fmt_dec, fmt_dec}, FMT_STATUS},
    /* 127 */ {"setregid", {fmt_dec, fmt_dec}, FMT_STATUS},
    /* 128 */ {"rename", {fmt_string, fmt_string}, FMT_STATUS},
    /* 129 */ {"truncate", {fmt_string, FMT_SIZE}, FMT_STATUS},
    /* 130 */ {"ftruncate", {fmt_fd, FMT_SIZE}, FMT_STATUS},
    /* 131 */ {"flock", {fmt_fd, FMT_FLAGS}, FMT_STATUS},
    /* 132 */ {"_unknown_132", {fmt_unknown}, fmt_unknown},
    /* 133 */ {"sendto",
               {fmt_fd, fmt_data, fmt_data_size, FMT_FLAGS, fmt_ptr, FMT_SIZE},
               FMT_SIZE},
    /* 134 */ {"shutdown", {fmt_fd, fmt_dec}, FMT_STATUS},
    /* 135 */ {"socketpair", {fmt_dec, fmt_dec, fmt_dec, fmt_ptr}, FMT_STATUS},
              /*
               * Sun sometimes claim socketpair has 5 parameters.
               */
    /* 136 */ {"mkdir", {fmt_string, FMT_FLAGS}, FMT_STATUS},
    /* 137 */ {"rmdir", {fmt_string}, FMT_STATUS},
    /* 138 */ {"utimes", {fmt_string, fmt_ptr}, FMT_STATUS},
    /* 139 */ {"_sigcleanup", {fmt_ptr}, FMT_STATUS},
    /* 140 */ {"adjtime", {fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 141 */ {"getpeername", {fmt_fd, fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 142 */ {"gethostid", {fmt_none}, fmt_hex},
              /*
               * Sun sometimes claim gethostid has 2 parameters.
               */
    /* 143 */ {"_unknown_143", {fmt_unknown}, fmt_unknown},
    /* 144 */ {"getrlimit", {fmt_dec, fmt_ptr}, FMT_STATUS},
    /* 145 */ {"setrlimit", {fmt_dec, fmt_ptr}, FMT_STATUS},
    /* 146 */ {"killpg", {fmt_dec, fmt_signal}, FMT_STATUS},
    /* 147 */ {"_unknown_147", {fmt_unknown}, fmt_unknown},
    /* 148 */ {"_unknown_148[\"old quota\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think quota can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /* 149 */ {"_unknown_149[\"old qquota\"]", {fmt_unknown}, fmt_unknown},
              /*
               * I don't think qquota can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /* 150 */ {"getsockname", {fmt_fd, fmt_ptr, fmt_ptr}, FMT_STATUS},
    /* 151 */ {"getmsg", {fmt_fd, fmt_ptr, fmt_ptr, fmt_ptr}, fmt_dec},
    /* 152 */ {"putmsg", {fmt_fd, fmt_ptr, fmt_ptr, FMT_FLAGS}, FMT_STATUS},
    /* 153 */ {"poll", {fmt_ptr, fmt_dec, fmt_dec}, fmt_dec},
    /* 154 */ {"_unknown_154", {fmt_unknown}, fmt_unknown},
    /* 155 */ {"nfssvc", {fmt_fd}, FMT_STATUS},
    /* 156 */ {"_unknown_156[\"old getdirentries\"]",
               {fmt_unknown},
               fmt_unknown},
              /*
               * I don't think getdirentries can be generated by the standard
               * libararies, although Sun have not officially declared it
               * obsolete.
               */
    /* 157 */ {"statfs", {fmt_string, fmt_ptr}, FMT_STATUS},
    /* 158 */ {"fstatfs", {fmt_fd, fmt_ptr}, FMT_STATUS},
    /* 159 */ {"unmount", {fmt_string}, FMT_STATUS},
    /* 160 */ {"async_daemon", {fmt_none}, fmt_none},
    /* 161 */ {"nfs_getfh", {fmt_hex, fmt_hex}, fmt_unknown},
              /*
               * I am unsure of the parameters for nfs_getfh.
               */
    /* 162 */ {"getdomainname", {fmt_post_string, FMT_SIZE}, FMT_STATUS},
    /* 163 */ {"setdomainname", {fmt_data, fmt_data_size}, FMT_STATUS},
    /* 164 */ {"rtschedule",
               {fmt_hex, fmt_hex, fmt_hex, fmt_hex, fmt_hex},
               fmt_unknown},
              /*
               * I am unsure of the parameters for rtschedule.
               */
    /* 165 */ {"quotactl",
               {fmt_dec, fmt_string, fmt_dec, fmt_ptr},
               FMT_STATUS},
    /* 166 */ {"_exportfs", {fmt_string, fmt_ptr}, FMT_STATUS},
    /* 167 */ {"mount",
               {fmt_string, fmt_string, FMT_FLAGS, fmt_ptr},
               FMT_STATUS},
    /* 168 */ {"ustat", {fmt_hex, fmt_ptr}, FMT_STATUS},
    /* 169 */ {"_semsys",
               {fmt_dec, fmt_hex, fmt_hex, fmt_hex, fmt_hex},
               fmt_dec},
    /* 170 */ {"_msgsys",
               {fmt_dec, fmt_hex, fmt_hex, fmt_hex, fmt_hex, fmt_hex},
               fmt_dec},
    /* 171 */ {"_shmsys", {fmt_dec, fmt_hex, fmt_hex, fmt_hex}, fmt_dec},
    /* 172 */ {"_auditsys", {fmt_dec, fmt_hex, fmt_hex, fmt_hex}, fmt_dec},
    /* 173 */ {"_rfssys",
               {fmt_dec, fmt_hex, fmt_hex, fmt_hex, fmt_hex},
               fmt_dec},
    /* 174 */ {"getdents",
               {fmt_fd, fmt_post_data, FMT_SIZE},
               fmt_post_data_size},
    /* 175 */ {"_setsid", {fmt_dec}, fmt_dec},
    /* 176 */ {"fchdir", {fmt_fd}, FMT_STATUS},
    /* 177 */ {"fchroot", {fmt_fd}, FMT_STATUS},
    /* 178 */ {"vpixsys", {fmt_hex, fmt_hex}, fmt_unknown},
              /*
               * I am unsure of the parameters for vpixsys.
               */
    /* 179 */ {"aioread",
               {fmt_fd, fmt_ptr, FMT_SIZE, FMT_SIZE, fmt_dec, fmt_ptr},
               FMT_STATUS},
    /* 180 */ {"aiowrite",
               {fmt_fd, fmt_data, fmt_data_size, FMT_SIZE, fmt_dec, fmt_ptr},
               FMT_STATUS},
    /* 181 */ {"aiowait", {fmt_ptr}, fmt_ptr},
    /* 182 */ {"aiocancel", {fmt_ptr}, FMT_STATUS},
    /* 183 */ {"sigpending", {fmt_ptr}, FMT_STATUS},
    /* 184 */ {"_unknown_184", {fmt_unknown}, fmt_unknown},
    /* 185 */ {"setpgid", {fmt_dec, fmt_dec}, FMT_STATUS},
    /* 186 */ {"_pathconf", {fmt_string, fmt_dec}, fmt_dec},
    /* 187 */ {"fpathconf", {fmt_fd, fmt_dec}, fmt_dec},
    /* 188 */ {"sysconf", {fmt_dec}, fmt_dec},
    /* 189 */ {"uname", {fmt_ptr}, FMT_STATUS}
    /*
     * Next 8 system calls are for loadable system calls.  Not declared since
     * they are likely to change from one O/S release to the next.
     */
};

int no_system_calls = sizeof(system_calls) / sizeof(call_desc);

call_desc system_call_open_simple =
    /*   5 */ {"open", {fmt_string, fmt_open_flags}, fmt_fd};


/*
 * Table containing signal names.
 */

char *signal_names[] = {
    /*  0 */ "0",
    /*  1 */ "SIGHUP",
    /*  2 */ "SIGINT",
    /*  3 */ "SIGQUIT",
    /*  4 */ "SIGILL",
    /*  5 */ "SIGTRAP",
    /*  6 */ "SIGABRT",
    /*  7 */ "SIGEMT",
    /*  8 */ "SIGFPE",
    /*  9 */ "SIGKILL",
    /* 10 */ "SIGBUS",
    /* 11 */ "SIGSEGV",
    /* 12 */ "SIGSYS",
    /* 13 */ "SIGPIPE",
    /* 14 */ "SIGALRM",
    /* 15 */ "SIGTERM",
    /* 16 */ "SIGURG",
    /* 17 */ "SIGSTOP",
    /* 18 */ "SIGTSTP",
    /* 19 */ "SIGCONT",
    /* 20 */ "SIGCHLD",
    /* 21 */ "SIGTTIN",
    /* 22 */ "SIGTTOU",
    /* 23 */ "SIGIO",
    /* 24 */ "SIGXCPU",
    /* 25 */ "SIGXFSZ",
    /* 26 */ "SIGVTALRM",
    /* 27 */ "SIGPROF",
    /* 28 */ "SIGWINCH",
    /* 29 */ "SIGLOST",
    /* 30 */ "SIGUSR1",
    /* 31 */ "SIGUSR2"
};

int no_signal_names = sizeof(signal_names) / sizeof(char *);


/*
 * Table containing error messages.
 */

char *error_names[] = {
    /*  0 */ "0",
    /*  1 */ "EPERM",
    /*  2 */ "ENOENT",
    /*  3 */ "ESRCH",
    /*  4 */ "EINTR",
    /*  5 */ "EIO",
    /*  6 */ "ENXIO",
    /*  7 */ "E2BIG",
    /*  8 */ "ENOEXEC",
    /*  9 */ "EBADF",
    /* 10 */ "ECHILD",
    /* 11 */ "EAGAIN",
    /* 12 */ "ENOMEM",
    /* 13 */ "EACCES",
    /* 14 */ "EFAULT",
    /* 15 */ "ENOTBLK",
    /* 16 */ "EBUSY",
    /* 17 */ "EEXIST",
    /* 18 */ "EXDEV",
    /* 19 */ "ENODEV",
    /* 20 */ "ENOTDIR",
    /* 21 */ "EISDIR",
    /* 22 */ "EINVAL",
    /* 23 */ "ENFILE",
    /* 24 */ "EMFILE",
    /* 25 */ "ENOTTY",
    /* 26 */ "ETXTBSY",
    /* 27 */ "EFBIG",
    /* 28 */ "ENOSPC",
    /* 29 */ "ESPIPE",
    /* 30 */ "EROFS",
    /* 31 */ "EMLINK",
    /* 32 */ "EPIPE",
    /* 33 */ "EDOM",
    /* 34 */ "ERANGE",
    /* 35 */ "EWOULDBLOCK",
    /* 36 */ "EINPROGRESS",
    /* 37 */ "EALREADY",
    /* 38 */ "ENOTSOCK",
    /* 39 */ "EDESTADDRREQ",
    /* 40 */ "EMSGSIZE",
    /* 41 */ "EPROTOTYPE",
    /* 42 */ "ENOPROTOOPT",
    /* 43 */ "EPROTONOSUPPORT",
    /* 44 */ "ESOCKTNOSUPPORT",
    /* 45 */ "EOPNOTSUPP",
    /* 46 */ "EPFNOSUPPORT",
    /* 47 */ "EAFNOSUPPORT",
    /* 48 */ "EADDRINUSE",
    /* 49 */ "EADDRNOTAVAIL",
    /* 50 */ "ENETDOWN",
    /* 51 */ "ENETUNREACH",
    /* 52 */ "ENETRESET",
    /* 53 */ "ECONNABORTED",
    /* 54 */ "ECONNRESET",
    /* 55 */ "ENOBUFS",
    /* 56 */ "EISCONN",
    /* 57 */ "ENOTCONN",
    /* 58 */ "ESHUTDOWN",
    /* 59 */ "ETOOMANYREFS",
    /* 60 */ "ETIMEDOUT",
    /* 61 */ "ECONNREFUSED",
    /* 62 */ "ELOOP",
    /* 63 */ "ENAMETOOLONG",
    /* 64 */ "EHOSTDOWN",
    /* 65 */ "EHOSTUNREACH",
    /* 66 */ "ENOTEMPTY",
    /* 67 */ "EPROCLIM",
    /* 68 */ "EUSERS",
    /* 69 */ "EDQUOT",
    /* 70 */ "ESTALE",
    /* 71 */ "EREMOTE",
    /* 72 */ "ENOSTR",
    /* 73 */ "ETIME",
    /* 74 */ "ENOSR",
    /* 75 */ "ENOMSG",
    /* 76 */ "EBADMSG",
    /* 77 */ "EIDRM",
    /* 78 */ "EDEADLK",
    /* 79 */ "ENOLCK",
    /* 80 */ "ENONET",
    /* 81 */ "ERREMOTE",
    /* 82 */ "ENOLINK",
    /* 83 */ "EADV",
    /* 84 */ "ESRMNT",
    /* 85 */ "ECOMM",
    /* 86 */ "EPROTO",
    /* 87 */ "EMULTIHOP",
    /* 88 */ "EDOTDOT",
    /* 89 */ "EREMCHG",
    /* 90 */ "ENOSYS"
};

int no_error_names = sizeof(error_names) / sizeof(char *);
 
