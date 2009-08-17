/* Target-dependent code for GNU/Linux x86-64.

   Copyright (C) 2001, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by Jiri Smid, SuSE Labs.

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

#include "defs.h"
#include "arch-utils.h"
#include "frame.h"
#include "gdbcore.h"
#include "regcache.h"
#include "osabi.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "reggroups.h"
#include "amd64-linux-tdep.h"
#include "linux-tdep.h"

#include "gdb_string.h"

#include "amd64-tdep.h"
#include "solib-svr4.h"

#include "record.h"
#include "linux-record.h"

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register cache layout.  */

/* From <sys/reg.h>.  */
static int amd64_linux_gregset_reg_offset[] =
{
  10 * 8,			/* %rax */
  5 * 8,			/* %rbx */
  11 * 8,			/* %rcx */
  12 * 8,			/* %rdx */
  13 * 8,			/* %rsi */
  14 * 8,			/* %rdi */
  4 * 8,			/* %rbp */
  19 * 8,			/* %rsp */
  9 * 8,			/* %r8 ... */
  8 * 8,
  7 * 8,
  6 * 8,
  3 * 8,
  2 * 8,
  1 * 8,
  0 * 8,			/* ... %r15 */
  16 * 8,			/* %rip */
  18 * 8,			/* %eflags */
  17 * 8,			/* %cs */
  20 * 8,			/* %ss */
  23 * 8,			/* %ds */
  24 * 8,			/* %es */
  25 * 8,			/* %fs */
  26 * 8			/* %gs */
};


/* Support for signal handlers.  */

#define LINUX_SIGTRAMP_INSN0	0x48	/* mov $NNNNNNNN, %rax */
#define LINUX_SIGTRAMP_OFFSET0	0
#define LINUX_SIGTRAMP_INSN1	0x0f	/* syscall */
#define LINUX_SIGTRAMP_OFFSET1	7

static const gdb_byte linux_sigtramp_code[] =
{
  /* mov $__NR_rt_sigreturn, %rax */
  LINUX_SIGTRAMP_INSN0, 0xc7, 0xc0, 0x0f, 0x00, 0x00, 0x00,
  /* syscall */
  LINUX_SIGTRAMP_INSN1, 0x05
};

#define LINUX_SIGTRAMP_LEN (sizeof linux_sigtramp_code)

/* If PC is in a sigtramp routine, return the address of the start of
   the routine.  Otherwise, return 0.  */

static CORE_ADDR
amd64_linux_sigtramp_start (struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  gdb_byte buf[LINUX_SIGTRAMP_LEN];

  /* We only recognize a signal trampoline if PC is at the start of
     one of the two instructions.  We optimize for finding the PC at
     the start, as will be the case when the trampoline is not the
     first frame on the stack.  We assume that in the case where the
     PC is not at the start of the instruction sequence, there will be
     a few trailing readable bytes on the stack.  */

  if (!safe_frame_unwind_memory (this_frame, pc, buf, sizeof buf))
    return 0;

  if (buf[0] != LINUX_SIGTRAMP_INSN0)
    {
      if (buf[0] != LINUX_SIGTRAMP_INSN1)
	return 0;

      pc -= LINUX_SIGTRAMP_OFFSET1;
      if (!safe_frame_unwind_memory (this_frame, pc, buf, sizeof buf))
	return 0;
    }

  if (memcmp (buf, linux_sigtramp_code, LINUX_SIGTRAMP_LEN) != 0)
    return 0;

  return pc;
}

/* Return whether THIS_FRAME corresponds to a GNU/Linux sigtramp
   routine.  */

static int
amd64_linux_sigtramp_p (struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);

  /* If we have NAME, we can optimize the search.  The trampoline is
     named __restore_rt.  However, it isn't dynamically exported from
     the shared C library, so the trampoline may appear to be part of
     the preceding function.  This should always be sigaction,
     __sigaction, or __libc_sigaction (all aliases to the same
     function).  */
  if (name == NULL || strstr (name, "sigaction") != NULL)
    return (amd64_linux_sigtramp_start (this_frame) != 0);

  return (strcmp ("__restore_rt", name) == 0);
}

/* Offset to struct sigcontext in ucontext, from <asm/ucontext.h>.  */
#define AMD64_LINUX_UCONTEXT_SIGCONTEXT_OFFSET 40

/* Assuming THIS_FRAME is a GNU/Linux sigtramp routine, return the
   address of the associated sigcontext structure.  */

static CORE_ADDR
amd64_linux_sigcontext_addr (struct frame_info *this_frame)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  CORE_ADDR sp;
  gdb_byte buf[8];

  get_frame_register (this_frame, AMD64_RSP_REGNUM, buf);
  sp = extract_unsigned_integer (buf, 8, byte_order);

  /* The sigcontext structure is part of the user context.  A pointer
     to the user context is passed as the third argument to the signal
     handler, i.e. in %rdx.  Unfortunately %rdx isn't preserved across
     function calls so we can't use it.  Fortunately the user context
     is part of the signal frame and the unwound %rsp directly points
     at it.  */
  return sp + AMD64_LINUX_UCONTEXT_SIGCONTEXT_OFFSET;
}


/* From <asm/sigcontext.h>.  */
static int amd64_linux_sc_reg_offset[] =
{
  13 * 8,			/* %rax */
  11 * 8,			/* %rbx */
  14 * 8,			/* %rcx */
  12 * 8,			/* %rdx */
  9 * 8,			/* %rsi */
  8 * 8,			/* %rdi */
  10 * 8,			/* %rbp */
  15 * 8,			/* %rsp */
  0 * 8,			/* %r8 */
  1 * 8,			/* %r9 */
  2 * 8,			/* %r10 */
  3 * 8,			/* %r11 */
  4 * 8,			/* %r12 */
  5 * 8,			/* %r13 */
  6 * 8,			/* %r14 */
  7 * 8,			/* %r15 */
  16 * 8,			/* %rip */
  17 * 8,			/* %eflags */

  /* FIXME: kettenis/2002030531: The registers %cs, %fs and %gs are
     available in `struct sigcontext'.  However, they only occupy two
     bytes instead of four, which makes using them here rather
     difficult.  Leave them out for now.  */
  -1,				/* %cs */
  -1,				/* %ss */
  -1,				/* %ds */
  -1,				/* %es */
  -1,				/* %fs */
  -1				/* %gs */
};

/* Replacement register functions which know about %orig_rax.  */

static const char *
amd64_linux_register_name (struct gdbarch *gdbarch, int reg)
{
  if (reg == AMD64_LINUX_ORIG_RAX_REGNUM)
    return "orig_rax";

  return amd64_register_name (gdbarch, reg);
}

static struct type *
amd64_linux_register_type (struct gdbarch *gdbarch, int reg)
{
  if (reg == AMD64_LINUX_ORIG_RAX_REGNUM)
    return builtin_type (gdbarch)->builtin_int64;

  return amd64_register_type (gdbarch, reg);
}

static int
amd64_linux_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
				 struct reggroup *group)
{ 
  if (regnum == AMD64_LINUX_ORIG_RAX_REGNUM)
    return (group == system_reggroup
            || group == save_reggroup
            || group == restore_reggroup);
  return default_register_reggroup_p (gdbarch, regnum, group);
}

/* Set the program counter for process PTID to PC.  */

static void
amd64_linux_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  regcache_cooked_write_unsigned (regcache, AMD64_RIP_REGNUM, pc);

  /* We must be careful with modifying the program counter.  If we
     just interrupted a system call, the kernel might try to restart
     it when we resume the inferior.  On restarting the system call,
     the kernel will try backing up the program counter even though it
     no longer points at the system call.  This typically results in a
     SIGSEGV or SIGILL.  We can prevent this by writing `-1' in the
     "orig_rax" pseudo-register.

     Note that "orig_rax" is saved when setting up a dummy call frame.
     This means that it is properly restored when that frame is
     popped, and that the interrupted system call will be restarted
     when we resume the inferior on return from a function call from
     within GDB.  In all other cases the system call will not be
     restarted.  */
  regcache_cooked_write_unsigned (regcache, AMD64_LINUX_ORIG_RAX_REGNUM, -1);
}

/* Parse the arguments of current system call instruction and record
   the values of the registers and memory that will be changed into
   "record_arch_list".  This instruction is "syscall".

   Return -1 if something wrong.  */

static struct linux_record_tdep amd64_linux_record_tdep;

#define RECORD_ARCH_GET_FS	0x1003
#define RECORD_ARCH_GET_GS	0x1004

static int
amd64_linux_syscall_record (struct regcache *regcache)
{
  int ret, num = -1;
  ULONGEST tmpulongest;

  regcache_raw_read_unsigned (regcache, AMD64_RAX_REGNUM, &tmpulongest);

  /* Convert tmpulongest to number in record_linux_system_call.  */
  switch (tmpulongest)
    {
      /* sys_read */
    case 0:
      num = 3;
      break;
      /* sys_write */
    case 1:
      num = 4;
      break;
      /* sys_open */
    case 2:
      num = 5;
      break;
      /* sys_close */
    case 3:
      num = 6;
      break;
      /* sys_newstat */
    case 4:
      num = 106;
      break;
      /* sys_newfstat */
    case 5:
      num = 108;
      break;
      /* sys_newlstat */
    case 6:
      num = 107;
      break;
      /* sys_poll */
    case 7:
      num = 168;
      break;
      /* sys_lseek */
    case 8:
      num = 19;
      break;
      /* sys_mmap */
    case 9:
      num = 192;
      break;
      /* sys_mprotect */
    case 10:
      num = 125;
      break;
      /* sys_munmap */
    case 11:
      num = 91;
      break;
      /* sys_brk */
    case 12:
      num = 45;
      break;
      /* sys_rt_sigaction */
    case 13:
      num = 174;
      break;
      /* sys_rt_sigprocmask */
    case 14:
      num = 175;
      break;
      /* sys_rt_sigreturn */
    case 15:
      num = 173;
      break;
      /* sys_ioctl */
    case 16:
      num = 54;
      break;
      /* sys_pread64 */
    case 17:
      num = 180;
      break;
      /* sys_pwrite64 */
    case 18:
      num = 181;
      break;
      /* sys_readv */
    case 19:
      num = 145;
      break;
      /* sys_writev */
    case 20:
      num = 146;
      break;
      /* sys_access */
    case 21:
      num = 33;
      break;
      /* sys_pipe */
    case 22:
      num = 42;
      break;
      /* sys_select */
    case 23:
      num = 142;
      break;
      /* sys_sched_yield */
    case 24:
      num = 158;
      break;
      /* sys_mremap */
    case 25:
      num = 163;
      break;
      /* sys_msync */
    case 26:
      num = 144;
      break;
      /* sys_mincore */
    case 27:
      num = 218;
      break;
      /* sys_madvise */
    case 28:
      num = 219;
      break;
      /* sys_shmget */
    case 29:
      num = 520;
      break;
      /* sys_shmat */
    case 30:
      num = 521;
      break;
      /* sys_shmctl */
    case 31:
      num = 522;
      break;
      /* sys_dup */
    case 32:
      num = 41;
      break;
      /* sys_dup2 */
    case 33:
      num = 63;
      break;
      /* sys_pause */
    case 34:
      num = 29;
      break;
      /* sys_nanosleep */
    case 35:
      num = 162;
      break;
      /* sys_getitimer */
    case 36:
      num = 105;
      break;
      /* sys_alarm */
    case 37:
      num = 27;
      break;
      /* sys_setitimer */
    case 38:
      num = 104;
      break;
      /* sys_getpid */
    case 39:
      num = 20;
      break;
      /* sys_sendfile64 */
    case 40:
      num = 239;
      break;
      /* sys_socket */
    case 41:
      num = 500;
      break;
      /* sys_connect */
    case 42:
      num = 501;
      break;
      /* sys_accept */
    case 43:
      num = 502;
      break;
      /* sys_sendto */
    case 44:
      num = 503;
      break;
      /* sys_recvfrom */
    case 45:
      num = 504;
      break;
      /* sys_sendmsg */
    case 46:
      num = 505;
      break;
      /* sys_recvmsg */
    case 47:
      num = 506;
      break;
      /* sys_shutdown */
    case 48:
      num = 507;
      break;
      /* sys_bind */
    case 49:
      num = 508;
      break;
      /* sys_listen */
    case 50:
      num = 509;
      break;
      /* sys_getsockname */
    case 51:
      num = 510;
      break;
      /* sys_getpeername */
    case 52:
      num = 511;
      break;
      /* sys_socketpair */
    case 53:
      num = 512;
      break;
      /* sys_setsockopt */
    case 54:
      num = 513;
      break;
      /* sys_getsockopt */
    case 55:
      num = 514;
      break;
      /* sys_clone */
    case 56:
      num = 120;
      break;
      /* sys_fork */
    case 57:
      num = 2;
      break;
      /* sys_vfork */
    case 58:
      num = 190;
      break;
      /* sys_execve */
    case 59:
      num = 11;
      break;
      /* sys_exit */
    case 60:
      num = 1;
      break;
      /* sys_wait4 */
    case 61:
      num = 114;
      break;
      /* sys_kill */
    case 62:
      num = 37;
      break;
      /* sys_uname */
    case 63:
      num = 109;
      break;
      /* sys_semget */
    case 64:
      num = 523;
      break;
      /* sys_semop */
    case 65:
      num = 524;
      break;
      /* sys_semctl */
    case 66:
      num = 525;
      break;
      /* sys_shmdt */
    case 67:
      num = 527;
      break;
      /* sys_msgget */
    case 68:
      num = 528;
      break;
      /* sys_msgsnd */
    case 69:
      num = 529;
      break;
      /* sys_msgrcv */
    case 70:
      num = 530;
      break;
      /* sys_msgctl */
    case 71:
      num = 531;
      break;
      /* sys_fcntl */
    case 72:
      num = 55;
      break;
      /* sys_flock */
    case 73:
      num = 143;
      break;
      /* sys_fsync */
    case 74:
      num = 118;
      break;
      /* sys_fdatasync */
    case 75:
      num = 148;
      break;
      /* sys_truncate */
    case 76:
      num = 92;
      break;
      /* sys_ftruncate */
    case 77:
      num = 93;
      break;
      /* sys_getdents */
    case 78:
      num = 141;
      break;
      /* sys_getcwd */
    case 79:
      num = 183;
      break;
      /* sys_chdir */
    case 80:
      num = 12;
      break;
      /* sys_fchdir */
    case 81:
      num = 133;
      break;
      /* sys_rename */
    case 82:
      num = 38;
      break;
      /* sys_mkdir */
    case 83:
      num = 39;
      break;
      /* sys_rmdir */
    case 84:
      num = 40;
      break;
      /* sys_creat */
    case 85:
      num = 8;
      break;
      /* sys_link */
    case 86:
      num = 9;
      break;
      /* sys_unlink */
    case 87:
      num = 10;
      break;
      /* sys_symlink */
    case 88:
      num = 83;
      break;
      /* sys_readlink */
    case 89:
      num = 85;
      break;
      /* sys_chmod */
    case 90:
      num = 15;
      break;
      /* sys_fchmod */
    case 91:
      num = 94;
      break;
      /* sys_chown */
    case 92:
      num = 212;
      break;
      /* sys_fchown */
    case 93:
      num = 207;
      break;
      /* sys_lchown */
    case 94:
      num = 198;
      break;
      /* sys_umask */
    case 95:
      num = 60;
      break;
      /* sys_gettimeofday */
    case 96:
      num = 78;
      break;
      /* sys_getrlimit */
    case 97:
      num = 191;
      break;
      /* sys_getrusage */
    case 98:
      num = 77;
      break;
      /* sys_sysinfo */
    case 99:
      num = 116;
      break;
      /* sys_times */
    case 100:
      num = 43;
      break;
      /* sys_ptrace */
    case 101:
      num = 26;
      break;
      /* sys_getuid */
    case 102:
      num = 199;
      break;
      /* sys_syslog */
    case 103:
      num = 103;
      break;
      /* sys_getgid */
    case 104:
      num = 200;
      break;
      /* sys_setuid */
    case 105:
      num = 213;
      break;
      /* sys_setgid */
    case 106:
      num = 214;
      break;
      /* sys_geteuid */
    case 107:
      num = 201;
      break;
      /* sys_getegid */
    case 108:
      num = 202;
      break;
      /* sys_setpgid */
    case 109:
      num = 57;
      break;
      /* sys_getppid */
    case 110:
      num = 64;
      break;
      /* sys_getpgrp */
    case 111:
      num = 65;
      break;
      /* sys_setsid */
    case 112:
      num = 66;
      break;
      /* sys_setreuid */
    case 113:
      num = 203;
      break;
      /* sys_setregid */
    case 114:
      num = 204;
      break;
      /* sys_getgroups */
    case 115:
      num = 205;
      break;
      /* sys_setgroups */
    case 116:
      num = 206;
      break;
      /* sys_setresuid */
    case 117:
      num = 208;
      break;
      /* sys_getresuid */
    case 118:
      num = 209;
      break;
      /* sys_setresgid */
    case 119:
      num = 210;
      break;
      /* sys_getresgid */
    case 120:
      num = 211;
      break;
      /* sys_getpgid */
    case 121:
      num = 132;
      break;
      /* sys_setfsuid */
    case 122:
      num = 215;
      break;
      /* sys_setfsgid */
    case 123:
      num = 216;
      break;
      /* sys_getsid */
    case 124:
      num = 147;
      break;
      /* sys_capget */
    case 125:
      num = 184;
      break;
      /* sys_capset */
    case 126:
      num = 185;
      break;
      /* sys_rt_sigpending */
    case 127:
      num = 176;
      break;
      /* sys_rt_sigtimedwait */
    case 128:
      num = 177;
      break;
      /* sys_rt_sigqueueinfo */
    case 129:
      num = 178;
      break;
      /* sys_rt_sigsuspend */
    case 130:
      num = 179;
      break;
      /* sys_sigaltstack */
    case 131:
      num = 186;
      break;
      /* sys_utime */
    case 132:
      num = 30;
      break;
      /* sys_mknod */
    case 133:
      num = 14;
      break;
      /* sys_personality */
    case 135:
      num = 136;
      break;
      /* sys_ustat */
    case 136:
      num = 62;
      break;
      /* sys_statfs */
    case 137:
      num = 99;
      break;
      /* sys_fstatfs */
    case 138:
      num = 100;
      break;
      /* sys_sysfs */
    case 139:
      num = 135;
      break;
      /* sys_getpriority */
    case 140:
      num = 96;
      break;
      /* sys_setpriority */
    case 141:
      num = 97;
      break;
      /* sys_sched_setparam */
    case 142:
      num = 154;
      break;
      /* sys_sched_getparam */
    case 143:
      num = 155;
      break;
      /* sys_sched_setscheduler */
    case 144:
      num = 156;
      break;
      /* sys_sched_getscheduler */
    case 145:
      num = 157;
      break;
      /* sys_sched_get_priority_max */
    case 146:
      num = 159;
      break;
      /* sys_sched_get_priority_min */
    case 147:
      num = 160;
      break;
      /* sys_sched_rr_get_interval */
    case 148:
      num = 161;
      break;
      /* sys_mlock */
    case 149:
      num = 150;
      break;
      /* sys_munlock */
    case 150:
      num = 151;
      break;
      /* sys_mlockall */
    case 151:
      num = 152;
      break;
      /* sys_munlockall */
    case 152:
      num = 153;
      break;
      /* sys_vhangup */
    case 153:
      num = 111;
      break;
      /* sys_modify_ldt */
    case 154:
      num = 123;
      break;
      /* sys_pivot_root */
    case 155:
      num = 217;
      break;
      /* sys_sysctl */
    case 156:
      num = 149;
      break;
      /* sys_prctl */
    case 157:
      num = 172;
      break;
      /* sys_arch_prctl */
    case 158:
      regcache_raw_read_unsigned (regcache, amd64_linux_record_tdep.arg3,
                                  &tmpulongest);
      if (tmpulongest == RECORD_ARCH_GET_FS
          || tmpulongest == RECORD_ARCH_GET_GS)
        {
          regcache_raw_read_unsigned (regcache, amd64_linux_record_tdep.arg2,
                                      &tmpulongest);
          if (record_arch_list_add_mem ((CORE_ADDR) tmpulongest,
                                        amd64_linux_record_tdep.size_ulong))
            return -1;
        }
      break;
      /* sys_adjtimex */
    case 159:
      num = 124;
      break;
      /* sys_setrlimit */
    case 160:
      num = 75;
      break;
      /* sys_chroot */
    case 161:
      num = 61;
      break;
      /* sys_sync */
    case 162:
      num = 36;
      break;
      /* sys_acct */
    case 163:
      num = 51;
      break;
      /* sys_settimeofday */
    case 164:
      num = 79;
      break;
      /* sys_mount */
    case 165:
      num = 21;
      break;
      /* sys_umount */
    case 166:
      num = 52;
      break;
      /* sys_swapon */
    case 167:
      num = 87;
      break;
      /* sys_swapoff */
    case 168:
      num = 115;
      break;
      /* sys_reboot */
    case 169:
      num = 88;
      break;
      /* sys_sethostname */
    case 170:
      num = 74;
      break;
      /* sys_setdomainname */
    case 171:
      num = 121;
      break;
      /* sys_iopl */
    case 172:
      num = 110;
      break;
      /* sys_ioperm */
    case 173:
      num = 101;
      break;
      /* sys_init_module */
    case 175:
      num = 128;
      break;
      /* sys_delete_module */
    case 176:
      num = 129;
      break;
      /* sys_quotactl */
    case 179:
      num = 131;
      break;
      /* sys_nfsservctl */
    case 180:
      num = 169;
      break;
      /* sys_gettid */
    case 186:
      num = 224;
      break;
      /* sys_readahead */
    case 187:
      num = 225;
      break;
      /* sys_setxattr */
    case 188:
      num = 226;
      break;
      /* sys_lsetxattr */
    case 189:
      num = 227;
      break;
      /* sys_fsetxattr */
    case 190:
      num = 228;
      break;
      /* sys_getxattr */
    case 191:
      num = 229;
      break;
      /* sys_lgetxattr */
    case 192:
      num = 230;
      break;
      /* sys_fgetxattr */
    case 193:
      num = 231;
      break;
      /* sys_listxattr */
    case 194:
      num = 232;
      break;
      /* sys_llistxattr */
    case 195:
      num = 233;
      break;
      /* sys_flistxattr */
    case 196:
      num = 234;
      break;
      /* sys_removexattr */
    case 197:
      num = 235;
      break;
      /* sys_lremovexattr */
    case 198:
      num = 236;
      break;
      /* sys_fremovexattr */
    case 199:
      num = 237;
      break;
      /* sys_tkill */
    case 200:
      num = 238;
      break;
      /* sys_time */
    case 201:
      num = 13;
      break;
      /* sys_futex */
    case 202:
      num = 240;
      break;
      /* sys_sched_setaffinity */
    case 203:
      num = 241;
      break;
      /* sys_sched_getaffinity */
    case 204:
      num = 242;
      break;
      /* sys_io_setup */
    case 206:
      num = 245;
      break;
      /* sys_io_destroy */
    case 207:
      num = 246;
      break;
      /* sys_io_getevents */
    case 208:
      num = 247;
      break;
      /* sys_io_submit */
    case 209:
      num = 248;
      break;
      /* sys_io_cancel */
    case 210:
      num = 249;
      break;
      /* sys_lookup_dcookie */
    case 212:
      num = 253;
      break;
      /* sys_epoll_create */
    case 213:
      num = 254;
      break;
      /* sys_remap_file_pages */
    case 216:
      num = 257;
      break;
      /* sys_getdents64 */
    case 217:
      num = 220;
      break;
      /* sys_set_tid_address */
    case 218:
      num = 258;
      break;
      /* sys_restart_syscall */
    case 219:
      num = 0;
      break;
      /* sys_semtimedop */
    case 220:
      num = 532;
      break;
      /* sys_fadvise64 */
    case 221:
      num = 250;
      break;
      /* sys_timer_create */
    case 222:
      num = 259;
      break;
      /* sys_timer_settime */
    case 223:
      num = 260;
      break;
      /* sys_timer_gettime */
    case 224:
      num = 261;
      break;
      /* sys_timer_getoverrun */
    case 225:
      num = 262;
      break;
      /* sys_timer_delete */
    case 226:
      num = 263;
      break;
      /* sys_clock_settime */
    case 227:
      num = 264;
      break;
      /* sys_clock_gettime */
    case 228:
      num = 265;
      break;
      /* sys_clock_getres */
    case 229:
      num = 266;
      break;
      /* sys_clock_nanosleep */
    case 230:
      num = 267;
      break;
      /* sys_exit_group */
    case 231:
      num = 252;
      break;
      /* sys_epoll_wait */
    case 232:
      num = 256;
      break;
      /* sys_epoll_ctl */
    case 233:
      num = 255;
      break;
      /* sys_tgkill */
    case 234:
      num = 270;
      break;
      /* sys_utimes */
    case 235:
      num = 271;
      break;
      /* sys_mbind */
    case 237:
      num = 274;
      break;
      /* sys_set_mempolicy */
    case 238:
      num = 276;
      break;
      /* sys_get_mempolicy */
    case 239:
      num = 275;
      break;
      /* sys_mq_open */
    case 240:
      num = 277;
      break;
      /* sys_mq_unlink */
    case 241:
      num = 278;
      break;
      /* sys_mq_timedsend */
    case 242:
      num = 279;
      break;
      /* sys_mq_timedreceive */
    case 243:
      num = 280;
      break;
      /* sys_mq_notify */
    case 244:
      num = 281;
      break;
      /* sys_mq_getsetattr */
    case 245:
      num = 282;
      break;
      /* sys_kexec_load */
    case 246:
      num = 283;
      break;
      /* sys_waitid */
    case 247:
      num = 284;
      break;
      /* sys_add_key */
    case 248:
      num = 286;
      break;
      /* sys_request_key */
    case 249:
      num = 287;
      break;
      /* sys_keyctl */
    case 250:
      num = 288;
      break;
      /* sys_ioprio_set */
    case 251:
      num = 289;
      break;
      /* sys_ioprio_get */
    case 252:
      num = 290;
      break;
      /* sys_inotify_init */
    case 253:
      num = 291;
      break;
      /* sys_inotify_add_watch */
    case 254:
      num = 292;
      break;
      /* sys_inotify_rm_watch */
    case 255:
      num = 293;
      break;
      /* sys_migrate_pages */
    case 256:
      num = 294;
      break;
      /* sys_openat */
    case 257:
      num = 295;
      break;
      /* sys_mkdirat */
    case 258:
      num = 296;
      break;
      /* sys_mknodat */
    case 259:
      num = 297;
      break;
      /* sys_fchownat */
    case 260:
      num = 298;
      break;
      /* sys_futimesat */
    case 261:
      num = 299;
      break;
      /* sys_newfstatat */
    case 262:
      num = 540;
      break;
      /* sys_unlinkat */
    case 263:
      num = 301;
      break;
      /* sys_renameat */
    case 264:
      num = 302;
      break;
      /* sys_linkat */
    case 265:
      num = 303;
      break;
      /* sys_symlinkat */
    case 266:
      num = 304;
      break;
      /* sys_readlinkat */
    case 267:
      num = 305;
      break;
      /* sys_fchmodat */
    case 268:
      num = 306;
      break;
      /* sys_faccessat */
    case 269:
      num = 307;
      break;
      /* sys_pselect6 */
    case 270:
      num = 308;
      break;
      /* sys_ppoll */
    case 271:
      num = 309;
      break;
      /* sys_unshare */
    case 272:
      num = 310;
      break;
      /* sys_set_robust_list */
    case 273:
      num = 311;
      break;
      /* sys_get_robust_list */
    case 274:
      num = 312;
      break;
      /* sys_splice */
    case 275:
      num = 313;
      break;
      /* sys_tee */
    case 276:
      num = 315;
      break;
      /* sys_sync_file_range */
    case 277:
      num = 314;
      break;
      /* sys_vmsplice */
    case 278:
      num = 316;
      break;
      /* sys_move_pages */
    case 279:
      num = 317;
      break;
    default:
      printf_unfiltered (_("Process record and replay target doesn't "
                           "support syscall number %d\n"), (int) tmpulongest);
      return -1;
      break;
    }

  if (num >= 0)
    {
      ret = record_linux_system_call (num, regcache,
                                      &amd64_linux_record_tdep);
      if (ret)
        return ret;
    }

  /* Record the return value of the system call.  */
  if (record_arch_list_add_reg (regcache, AMD64_RCX_REGNUM))
    return -1;
  if (record_arch_list_add_reg (regcache, AMD64_R11_REGNUM))
    return -1;


  return 0;
}

static void
amd64_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  tdep->gregset_reg_offset = amd64_linux_gregset_reg_offset;
  tdep->gregset_num_regs = ARRAY_SIZE (amd64_linux_gregset_reg_offset);
  tdep->sizeof_gregset = 27 * 8;

  amd64_init_abi (info, gdbarch);

  tdep->sigtramp_p = amd64_linux_sigtramp_p;
  tdep->sigcontext_addr = amd64_linux_sigcontext_addr;
  tdep->sc_reg_offset = amd64_linux_sc_reg_offset;
  tdep->sc_num_regs = ARRAY_SIZE (amd64_linux_sc_reg_offset);

  /* GNU/Linux uses SVR4-style shared libraries.  */
  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, svr4_lp64_fetch_link_map_offsets);

  /* Add the %orig_rax register used for syscall restarting.  */
  set_gdbarch_write_pc (gdbarch, amd64_linux_write_pc);
  set_gdbarch_num_regs (gdbarch, AMD64_LINUX_NUM_REGS);
  set_gdbarch_register_name (gdbarch, amd64_linux_register_name);
  set_gdbarch_register_type (gdbarch, amd64_linux_register_type);
  set_gdbarch_register_reggroup_p (gdbarch, amd64_linux_register_reggroup_p);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
                                             svr4_fetch_objfile_link_map);

  /* GNU/Linux uses SVR4-style shared libraries.  */
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* Displaced stepping.  */
  set_gdbarch_displaced_step_copy_insn (gdbarch,
                                        amd64_displaced_step_copy_insn);
  set_gdbarch_displaced_step_fixup (gdbarch, amd64_displaced_step_fixup);
  set_gdbarch_displaced_step_free_closure (gdbarch,
                                           simple_displaced_step_free_closure);
  set_gdbarch_displaced_step_location (gdbarch,
                                       displaced_step_at_entry_point);

  set_gdbarch_get_siginfo_type (gdbarch, linux_get_siginfo_type);

  set_gdbarch_process_record (gdbarch, i386_process_record);

  /* Initialize the amd64_linux_record_tdep.  */
  /* These values are the size of the type that will be used in a system
     call.  They are obtained from Linux Kernel source.  */
  amd64_linux_record_tdep.size_pointer
    = gdbarch_ptr_bit (gdbarch) / TARGET_CHAR_BIT;
  amd64_linux_record_tdep.size__old_kernel_stat = 32;
  amd64_linux_record_tdep.size_tms = 32;
  amd64_linux_record_tdep.size_loff_t = 8;
  amd64_linux_record_tdep.size_flock = 32;
  amd64_linux_record_tdep.size_oldold_utsname = 45;
  amd64_linux_record_tdep.size_ustat = 32;
  /* ADM64 doesn't need this size because it doesn't have sys_sigaction
     but sys_rt_sigaction.  */
  amd64_linux_record_tdep.size_old_sigaction = 152;
  /* ADM64 doesn't need this size because it doesn't have sys_sigpending
     but sys_rt_sigpending.  */
  amd64_linux_record_tdep.size_old_sigset_t = 128;
  amd64_linux_record_tdep.size_rlimit = 16;
  amd64_linux_record_tdep.size_rusage = 144;
  amd64_linux_record_tdep.size_timeval = 16;
  amd64_linux_record_tdep.size_timezone = 8;
  /* ADM64 doesn't need this size because it doesn't have sys_getgroups16
     but sys_getgroups.  */
  amd64_linux_record_tdep.size_old_gid_t = 2;
  /* ADM64 doesn't need this size because it doesn't have sys_getresuid16
     but sys_getresuid.  */
  amd64_linux_record_tdep.size_old_uid_t = 2;
  amd64_linux_record_tdep.size_fd_set = 128;
  amd64_linux_record_tdep.size_dirent = 280;
  amd64_linux_record_tdep.size_dirent64 = 280;
  amd64_linux_record_tdep.size_statfs = 120;
  amd64_linux_record_tdep.size_statfs64 = 120;
  amd64_linux_record_tdep.size_sockaddr = 16;
  amd64_linux_record_tdep.size_int
    = gdbarch_int_bit (gdbarch) / TARGET_CHAR_BIT;
  amd64_linux_record_tdep.size_long
    = gdbarch_long_bit (gdbarch) / TARGET_CHAR_BIT;
  amd64_linux_record_tdep.size_ulong
    = gdbarch_long_bit (gdbarch) / TARGET_CHAR_BIT;
  amd64_linux_record_tdep.size_msghdr = 56;
  amd64_linux_record_tdep.size_itimerval = 32;
  amd64_linux_record_tdep.size_stat = 144;
  amd64_linux_record_tdep.size_old_utsname = 325;
  amd64_linux_record_tdep.size_sysinfo = 112;
  amd64_linux_record_tdep.size_msqid_ds = 120;
  amd64_linux_record_tdep.size_shmid_ds = 112;
  amd64_linux_record_tdep.size_new_utsname = 390;
  amd64_linux_record_tdep.size_timex = 208;
  amd64_linux_record_tdep.size_mem_dqinfo = 24;
  amd64_linux_record_tdep.size_if_dqblk = 72;
  amd64_linux_record_tdep.size_fs_quota_stat = 80;
  amd64_linux_record_tdep.size_timespec = 16;
  amd64_linux_record_tdep.size_pollfd = 8;
  amd64_linux_record_tdep.size_NFS_FHSIZE = 32;
  amd64_linux_record_tdep.size_knfsd_fh = 132;
  amd64_linux_record_tdep.size_TASK_COMM_LEN = 16;
  amd64_linux_record_tdep.size_sigaction = 152;
  amd64_linux_record_tdep.size_sigset_t = 128;
  amd64_linux_record_tdep.size_siginfo_t = 128;
  amd64_linux_record_tdep.size_cap_user_data_t = 8;
  amd64_linux_record_tdep.size_stack_t = 24;
  amd64_linux_record_tdep.size_off_t = 8;
  amd64_linux_record_tdep.size_stat64 = 144;
  amd64_linux_record_tdep.size_gid_t = 4;
  amd64_linux_record_tdep.size_uid_t = 4;
  amd64_linux_record_tdep.size_PAGE_SIZE = 4096;
  amd64_linux_record_tdep.size_flock64 = 32;
  amd64_linux_record_tdep.size_user_desc = 16;
  amd64_linux_record_tdep.size_io_event = 32;
  amd64_linux_record_tdep.size_iocb = 64;
  amd64_linux_record_tdep.size_epoll_event = 12;
  amd64_linux_record_tdep.size_itimerspec = 32;
  amd64_linux_record_tdep.size_mq_attr = 64;
  amd64_linux_record_tdep.size_siginfo = 128;
  amd64_linux_record_tdep.size_termios = 60;
  amd64_linux_record_tdep.size_termios2 = 44;
  amd64_linux_record_tdep.size_pid_t = 4;
  amd64_linux_record_tdep.size_winsize = 8;
  amd64_linux_record_tdep.size_serial_struct = 72;
  amd64_linux_record_tdep.size_serial_icounter_struct = 80;
  amd64_linux_record_tdep.size_hayes_esp_config = 12;
  amd64_linux_record_tdep.size_size_t = 8;
  amd64_linux_record_tdep.size_iovec = 16;

  /* These values are the second argument of system call "sys_ioctl".
     They are obtained from Linux Kernel source.  */
  amd64_linux_record_tdep.ioctl_TCGETS = 0x5401;
  amd64_linux_record_tdep.ioctl_TCSETS = 0x5402;
  amd64_linux_record_tdep.ioctl_TCSETSW = 0x5403;
  amd64_linux_record_tdep.ioctl_TCSETSF = 0x5404;
  amd64_linux_record_tdep.ioctl_TCGETA = 0x5405;
  amd64_linux_record_tdep.ioctl_TCSETA = 0x5406;
  amd64_linux_record_tdep.ioctl_TCSETAW = 0x5407;
  amd64_linux_record_tdep.ioctl_TCSETAF = 0x5408;
  amd64_linux_record_tdep.ioctl_TCSBRK = 0x5409;
  amd64_linux_record_tdep.ioctl_TCXONC = 0x540A;
  amd64_linux_record_tdep.ioctl_TCFLSH = 0x540B;
  amd64_linux_record_tdep.ioctl_TIOCEXCL = 0x540C;
  amd64_linux_record_tdep.ioctl_TIOCNXCL = 0x540D;
  amd64_linux_record_tdep.ioctl_TIOCSCTTY = 0x540E;
  amd64_linux_record_tdep.ioctl_TIOCGPGRP = 0x540F;
  amd64_linux_record_tdep.ioctl_TIOCSPGRP = 0x5410;
  amd64_linux_record_tdep.ioctl_TIOCOUTQ = 0x5411;
  amd64_linux_record_tdep.ioctl_TIOCSTI = 0x5412;
  amd64_linux_record_tdep.ioctl_TIOCGWINSZ = 0x5413;
  amd64_linux_record_tdep.ioctl_TIOCSWINSZ = 0x5414;
  amd64_linux_record_tdep.ioctl_TIOCMGET = 0x5415;
  amd64_linux_record_tdep.ioctl_TIOCMBIS = 0x5416;
  amd64_linux_record_tdep.ioctl_TIOCMBIC = 0x5417;
  amd64_linux_record_tdep.ioctl_TIOCMSET = 0x5418;
  amd64_linux_record_tdep.ioctl_TIOCGSOFTCAR = 0x5419;
  amd64_linux_record_tdep.ioctl_TIOCSSOFTCAR = 0x541A;
  amd64_linux_record_tdep.ioctl_FIONREAD = 0x541B;
  amd64_linux_record_tdep.ioctl_TIOCINQ
    = amd64_linux_record_tdep.ioctl_FIONREAD;
  amd64_linux_record_tdep.ioctl_TIOCLINUX = 0x541C;
  amd64_linux_record_tdep.ioctl_TIOCCONS = 0x541D;
  amd64_linux_record_tdep.ioctl_TIOCGSERIAL = 0x541E;
  amd64_linux_record_tdep.ioctl_TIOCSSERIAL = 0x541F;
  amd64_linux_record_tdep.ioctl_TIOCPKT = 0x5420;
  amd64_linux_record_tdep.ioctl_FIONBIO = 0x5421;
  amd64_linux_record_tdep.ioctl_TIOCNOTTY = 0x5422;
  amd64_linux_record_tdep.ioctl_TIOCSETD = 0x5423;
  amd64_linux_record_tdep.ioctl_TIOCGETD = 0x5424;
  amd64_linux_record_tdep.ioctl_TCSBRKP = 0x5425;
  amd64_linux_record_tdep.ioctl_TIOCTTYGSTRUCT = 0x5426;
  amd64_linux_record_tdep.ioctl_TIOCSBRK = 0x5427;
  amd64_linux_record_tdep.ioctl_TIOCCBRK = 0x5428;
  amd64_linux_record_tdep.ioctl_TIOCGSID = 0x5429;
  amd64_linux_record_tdep.ioctl_TCGETS2 = 0x802c542a;
  amd64_linux_record_tdep.ioctl_TCSETS2 = 0x402c542b;
  amd64_linux_record_tdep.ioctl_TCSETSW2 = 0x402c542c;
  amd64_linux_record_tdep.ioctl_TCSETSF2 = 0x402c542d;
  amd64_linux_record_tdep.ioctl_TIOCGPTN = 0x80045430;
  amd64_linux_record_tdep.ioctl_TIOCSPTLCK = 0x40045431;
  amd64_linux_record_tdep.ioctl_FIONCLEX = 0x5450;
  amd64_linux_record_tdep.ioctl_FIOCLEX = 0x5451;
  amd64_linux_record_tdep.ioctl_FIOASYNC = 0x5452;
  amd64_linux_record_tdep.ioctl_TIOCSERCONFIG = 0x5453;
  amd64_linux_record_tdep.ioctl_TIOCSERGWILD = 0x5454;
  amd64_linux_record_tdep.ioctl_TIOCSERSWILD = 0x5455;
  amd64_linux_record_tdep.ioctl_TIOCGLCKTRMIOS = 0x5456;
  amd64_linux_record_tdep.ioctl_TIOCSLCKTRMIOS = 0x5457;
  amd64_linux_record_tdep.ioctl_TIOCSERGSTRUCT = 0x5458;
  amd64_linux_record_tdep.ioctl_TIOCSERGETLSR = 0x5459;
  amd64_linux_record_tdep.ioctl_TIOCSERGETMULTI = 0x545A;
  amd64_linux_record_tdep.ioctl_TIOCSERSETMULTI = 0x545B;
  amd64_linux_record_tdep.ioctl_TIOCMIWAIT = 0x545C;
  amd64_linux_record_tdep.ioctl_TIOCGICOUNT = 0x545D;
  amd64_linux_record_tdep.ioctl_TIOCGHAYESESP = 0x545E;
  amd64_linux_record_tdep.ioctl_TIOCSHAYESESP = 0x545F;
  amd64_linux_record_tdep.ioctl_FIOQSIZE = 0x5460;

  /* These values are the second argument of system call "sys_fcntl"
     and "sys_fcntl64".  They are obtained from Linux Kernel source.  */
  amd64_linux_record_tdep.fcntl_F_GETLK = 5;
  amd64_linux_record_tdep.fcntl_F_GETLK64 = 12;
  amd64_linux_record_tdep.fcntl_F_SETLK64 = 13;
  amd64_linux_record_tdep.fcntl_F_SETLKW64 = 14;

  amd64_linux_record_tdep.arg1 = AMD64_RDI_REGNUM;
  amd64_linux_record_tdep.arg2 = AMD64_RSI_REGNUM;
  amd64_linux_record_tdep.arg3 = AMD64_RDX_REGNUM;
  amd64_linux_record_tdep.arg4 = AMD64_R10_REGNUM;
  amd64_linux_record_tdep.arg5 = AMD64_R8_REGNUM;
  amd64_linux_record_tdep.arg6 = AMD64_R9_REGNUM;

  tdep->i386_syscall_record = amd64_linux_syscall_record;
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_amd64_linux_tdep (void);

void
_initialize_amd64_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64,
			  GDB_OSABI_LINUX, amd64_linux_init_abi);
}
