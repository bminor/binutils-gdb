/* Definitions for PRPSINFO and PRSTATUS structures under ELF on GNU/Linux.
   Copyright (C) 2013-2016 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef ELF_LINUX_CORE_H
#define ELF_LINUX_CORE_H

/* The PRPSINFO structures defined below are used by most
   architectures, although some of them define their own versions
   (like e.g., PPC).  */

/* External 32-bit structure for PRPSINFO.  This structure is
   ABI-defined, thus we choose to use char arrays here in order to
   avoid dealing with different types in different architectures.

   This structure will ultimately be written in the corefile's note
   section, as the PRPSINFO.  */

struct elf_external_linux_prpsinfo32
  {
    char pr_state;			/* Numeric process state.  */
    char pr_sname;			/* Char for pr_state.  */
    char pr_zomb;			/* Zombie.  */
    char pr_nice;			/* Nice val.  */
    char pr_flag[4];			/* Flags.  */
    char pr_uid[2];
    char pr_gid[2];
    char pr_pid[4];
    char pr_ppid[4];
    char pr_pgrp[4];
    char pr_sid[4];
    char pr_fname[16];			/* Filename of executable.  */
    char pr_psargs[80];			/* Initial part of arg list.  */
  };

/* Helper function to copy an elf_internal_linux_prpsinfo in host
   endian to an elf_external_linux_prpsinfo32 in target endian.  */

static inline void
swap_linux_prpsinfo32_out (bfd *obfd,
			   const struct elf_internal_linux_prpsinfo *from,
			   struct elf_external_linux_prpsinfo32 *to)
{
  bfd_put_8 (obfd, from->pr_state, &to->pr_state);
  bfd_put_8 (obfd, from->pr_sname, &to->pr_sname);
  bfd_put_8 (obfd, from->pr_zomb, &to->pr_zomb);
  bfd_put_8 (obfd, from->pr_nice, &to->pr_nice);
  bfd_put_32 (obfd, from->pr_flag, to->pr_flag);
  bfd_put_16 (obfd, from->pr_uid, to->pr_uid);
  bfd_put_16 (obfd, from->pr_gid, to->pr_gid);
  bfd_put_32 (obfd, from->pr_pid, to->pr_pid);
  bfd_put_32 (obfd, from->pr_ppid, to->pr_ppid);
  bfd_put_32 (obfd, from->pr_pgrp, to->pr_pgrp);
  bfd_put_32 (obfd, from->pr_sid, to->pr_sid);
  strncpy (to->pr_fname, from->pr_fname, sizeof (to->pr_fname));
  strncpy (to->pr_psargs, from->pr_psargs, sizeof (to->pr_psargs));
}

/* External 64-bit structure for PRPSINFO.  This structure is
   ABI-defined, thus we choose to use char arrays here in order to
   avoid dealing with different types in different architectures.

   This structure will ultimately be written in the corefile's note
   section, as the PRPSINFO.  */

struct elf_external_linux_prpsinfo64
  {
    char pr_state;			/* Numeric process state.  */
    char pr_sname;			/* Char for pr_state.  */
    char pr_zomb;			/* Zombie.  */
    char pr_nice;			/* Nice val.  */
    char pr_flag[8];			/* Flags.  */
    char gap[4];
    char pr_uid[4];
    char pr_gid[4];
    char pr_pid[4];
    char pr_ppid[4];
    char pr_pgrp[4];
    char pr_sid[4];
    char pr_fname[16];			/* Filename of executable.  */
    char pr_psargs[80];			/* Initial part of arg list.  */
  };

/* Helper function to copy an elf_internal_linux_prpsinfo in host
   endian to an elf_external_linux_prpsinfo64 in target endian.  */

static inline void
swap_linux_prpsinfo64_out (bfd *obfd,
			   const struct elf_internal_linux_prpsinfo *from,
			   struct elf_external_linux_prpsinfo64 *to)
{
  bfd_put_8 (obfd, from->pr_state, &to->pr_state);
  bfd_put_8 (obfd, from->pr_sname, &to->pr_sname);
  bfd_put_8 (obfd, from->pr_zomb, &to->pr_zomb);
  bfd_put_8 (obfd, from->pr_nice, &to->pr_nice);
  bfd_put_64 (obfd, from->pr_flag, to->pr_flag);
  bfd_put_32 (obfd, from->pr_uid, to->pr_uid);
  bfd_put_32 (obfd, from->pr_gid, to->pr_gid);
  bfd_put_32 (obfd, from->pr_pid, to->pr_pid);
  bfd_put_32 (obfd, from->pr_ppid, to->pr_ppid);
  bfd_put_32 (obfd, from->pr_pgrp, to->pr_pgrp);
  bfd_put_32 (obfd, from->pr_sid, to->pr_sid);
  strncpy (to->pr_fname, from->pr_fname, sizeof (to->pr_fname));
  strncpy (to->pr_psargs, from->pr_psargs, sizeof (to->pr_psargs));
}

/* PRSTATUS structs.  */

struct elf_external_linux_prstatus32
{
  char pr_info[12];
  char pr_cursig[2];
  char pad[2];
  char pr_sigpend[4];
  char pr_sighold[4];
  char pr_pid[4];
  char pr_ppid[4];
  char pr_pgrp[4];
  char pr_sid[4];
  char pr_utime[8];
  char pr_stime[8];
  char pr_cutime[8];
  char pr_cstime[8];
  /* Variable size "char pr_reg[]" goes here.  */
  char pr_fpvalid[4];
};

struct elf_external_linux_prstatus64
{
  char pr_info[12];
  char pr_cursig[2];
  char pad[2];
  char pr_sigpend[8];
  char pr_sighold[8];
  char pr_pid[4];
  char pr_ppid[4];
  char pr_pgrp[4];
  char pr_sid[4];
  char pr_utime[16];
  char pr_stime[16];
  char pr_cutime[16];
  char pr_cstime[16];
  /* Variable size "char pr_reg[]" goes here.  */
  char pr_fpvalid[4];
  char pad2[4];
};

/* Helper function to copy an elf_internal_linux_prpstatus
   to an elf_external_linux_prpsinfo32 in target endian.
   FROM->pr_reg is already in target endian.  */

static inline void
swap_linux_prstatus32_out (bfd *obfd,
			   const struct elf_internal_linux_prstatus *from,
			   void *to)
{
  struct elf_external_linux_prstatus32 *dest;

  dest = (struct elf_external_linux_prstatus32 *) to;
  bfd_put_32 (obfd, from->pr_info.si_signo, dest->pr_info + 0);
  bfd_put_32 (obfd, from->pr_info.si_code, dest->pr_info + 4);
  bfd_put_32 (obfd, from->pr_info.si_errno, dest->pr_info + 8);
  bfd_put_16 (obfd, from->pr_cursig, dest->pr_cursig);
  memset (dest->pad, 0, sizeof (dest->pad));
  bfd_put_32 (obfd, from->pr_sigpend, dest->pr_sigpend);
  bfd_put_32 (obfd, from->pr_sighold, dest->pr_sighold);
  bfd_put_32 (obfd, from->pr_pid, dest->pr_pid);
  bfd_put_32 (obfd, from->pr_ppid, dest->pr_ppid);
  bfd_put_32 (obfd, from->pr_pgrp, dest->pr_pgrp);
  bfd_put_32 (obfd, from->pr_sid, dest->pr_sid);
  bfd_put_32 (obfd, from->pr_utime.tv_sec, dest->pr_utime + 0);
  bfd_put_32 (obfd, from->pr_utime.tv_usec, dest->pr_utime + 4);
  bfd_put_32 (obfd, from->pr_stime.tv_sec, dest->pr_stime + 0);
  bfd_put_32 (obfd, from->pr_stime.tv_usec, dest->pr_stime + 4);
  bfd_put_32 (obfd, from->pr_cutime.tv_sec, dest->pr_cutime + 0);
  bfd_put_32 (obfd, from->pr_cutime.tv_usec, dest->pr_cutime + 4);
  bfd_put_32 (obfd, from->pr_cstime.tv_sec, dest->pr_cstime + 0);
  bfd_put_32 (obfd, from->pr_cstime.tv_usec, dest->pr_cstime + 4);
  memcpy (dest->pr_fpvalid, from->pr_reg, from->pr_reg_size);
  bfd_put_32 (obfd, from->pr_fpvalid, dest->pr_fpvalid + from->pr_reg_size);
}

/* Helper function to copy an elf_internal_linux_prpstatus
   to an elf_external_linux_prpsinfo64 in target endian.
   FROM->pr_reg is already in target endian.  */

static inline void
swap_linux_prstatus64_out (bfd *obfd,
			   const struct elf_internal_linux_prstatus *from,
			   void *to)
{
  struct elf_external_linux_prstatus64 *dest;

  dest = (struct elf_external_linux_prstatus64 *) to;
  bfd_put_32 (obfd, from->pr_info.si_signo, dest->pr_info + 0);
  bfd_put_32 (obfd, from->pr_info.si_code, dest->pr_info + 4);
  bfd_put_32 (obfd, from->pr_info.si_errno, dest->pr_info + 8);
  bfd_put_16 (obfd, from->pr_cursig, dest->pr_cursig);
  memset (dest->pad, 0, sizeof (dest->pad));
  bfd_put_64 (obfd, from->pr_sigpend, dest->pr_sigpend);
  bfd_put_64 (obfd, from->pr_sighold, dest->pr_sighold);
  bfd_put_32 (obfd, from->pr_pid, dest->pr_pid);
  bfd_put_32 (obfd, from->pr_ppid, dest->pr_ppid);
  bfd_put_32 (obfd, from->pr_pgrp, dest->pr_pgrp);
  bfd_put_32 (obfd, from->pr_sid, dest->pr_sid);
  bfd_put_64 (obfd, from->pr_utime.tv_sec, dest->pr_utime + 0);
  bfd_put_64 (obfd, from->pr_utime.tv_usec, dest->pr_utime + 8);
  bfd_put_64 (obfd, from->pr_stime.tv_sec, dest->pr_stime + 0);
  bfd_put_64 (obfd, from->pr_stime.tv_usec, dest->pr_stime + 8);
  bfd_put_64 (obfd, from->pr_cutime.tv_sec, dest->pr_cutime + 0);
  bfd_put_64 (obfd, from->pr_cutime.tv_usec, dest->pr_cutime + 8);
  bfd_put_64 (obfd, from->pr_cstime.tv_sec, dest->pr_cstime + 0);
  bfd_put_64 (obfd, from->pr_cstime.tv_usec, dest->pr_cstime + 8);
  memcpy (dest->pr_fpvalid, from->pr_reg, from->pr_reg_size);
  bfd_put_32 (obfd, from->pr_fpvalid, dest->pr_fpvalid + from->pr_reg_size);
  memset (dest->pad2 + from->pr_reg_size, 0, sizeof (dest->pad2));
}

#endif
