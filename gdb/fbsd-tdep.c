/* Target-dependent code for FreeBSD, architecture-independent.

   Copyright (C) 2002-2018 Free Software Foundation, Inc.

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
#include "auxv.h"
#include "gdbcore.h"
#include "inferior.h"
#include "regcache.h"
#include "regset.h"
#include "gdbthread.h"
#include "xml-syscall.h"

#include "elf-bfd.h"
#include "fbsd-tdep.h"


/* FreeBSD kernels 12.0 and later include a copy of the
   'ptrace_lwpinfo' structure returned by the PT_LWPINFO ptrace
   operation in an ELF core note (NT_FREEBSD_PTLWPINFO) for each LWP.
   The constants below define the offset of field members and flags in
   this structure used by methods in this file.  Note that the
   'ptrace_lwpinfo' struct in the note is preceded by a 4 byte integer
   containing the size of the structure.  */

#define	LWPINFO_OFFSET		0x4

/* Offsets in ptrace_lwpinfo.  */
#define	LWPINFO_PL_FLAGS	0x8
#define	LWPINFO64_PL_SIGINFO	0x30
#define	LWPINFO32_PL_SIGINFO	0x2c

/* Flags in pl_flags.  */
#define	PL_FLAG_SI	0x20	/* siginfo is valid */

/* Sizes of siginfo_t.	*/
#define	SIZE64_SIGINFO_T	80
#define	SIZE32_SIGINFO_T	64

/* Offsets in data structure used in NT_FREEBSD_PROCSTAT_VMMAP core
   dump notes.  See <sys/user.h> for the definition of struct
   kinfo_vmentry.  This data structure should have the same layout on
   all architectures.  */

#define	KVE_STRUCTSIZE		0x0
#define	KVE_START		0x8
#define	KVE_END			0x10
#define	KVE_OFFSET		0x18
#define	KVE_FLAGS		0x2c
#define	KVE_PROTECTION		0x38
#define	KVE_PATH		0x88

/* Flags in the 'kve_protection' field in struct kinfo_vmentry.  These
   match the KVME_PROT_* constants in <sys/user.h>.  */

#define	KINFO_VME_PROT_READ	0x00000001
#define	KINFO_VME_PROT_WRITE	0x00000002
#define	KINFO_VME_PROT_EXEC	0x00000004

/* Flags in the 'kve_flags' field in struct kinfo_vmentry.  These
   match the KVME_FLAG_* constants in <sys/user.h>.  */

#define	KINFO_VME_FLAG_COW		0x00000001
#define	KINFO_VME_FLAG_NEEDS_COPY	0x00000002
#define	KINFO_VME_FLAG_NOCOREDUMP	0x00000004
#define	KINFO_VME_FLAG_SUPER		0x00000008
#define	KINFO_VME_FLAG_GROWS_UP		0x00000010
#define	KINFO_VME_FLAG_GROWS_DOWN	0x00000020

/* Offsets in data structure used in NT_FREEBSD_PROCSTAT_FILES core
   dump notes.  See <sys/user.h> for the definition of struct
   kinfo_file.  This data structure should have the same layout on all
   architectures.  */

#define	KF_STRUCTSIZE		0x0
#define	KF_TYPE			0x4
#define	KF_FD			0x8
#define	KF_PATH			0x170

/* Constants for the 'kf_type' field in struct kinfo_file.  These
   match the KF_TYPE_* constants in <sys/user.h>.  */

#define	KINFO_FILE_TYPE_VNODE	1

/* Special values for the 'kf_fd' field in struct kinfo_file.  These
   match the KF_FD_TYPE_* constants in <sys/user.h>.  */

#define	KINFO_FILE_FD_TYPE_CWD	-1
#define	KINFO_FILE_FD_TYPE_TEXT	-5

/* Number of 32-bit words in a signal set.  This matches _SIG_WORDS in
   <sys/_sigset.h> and is the same value on all architectures.  */

#define	SIG_WORDS		4

/* Offsets in data structure used in NT_FREEBSD_PROCSTAT_PROC core
   dump notes.  See <sys/user.h> for the definition of struct
   kinfo_proc.  This data structure has different layouts on different
   architectures mostly due to ILP32 vs LP64.  However, FreeBSD/i386
   uses a 32-bit time_t while all other architectures use a 64-bit
   time_t.

   The core dump note actually contains one kinfo_proc structure for
   each thread, but all of the process-wide data can be obtained from
   the first structure.  One result of this note's format is that some
   of the process-wide status available in the native target method
   from the kern.proc.pid.<pid> sysctl such as ki_stat and ki_siglist
   is not available from a core dump.  Instead, the per-thread data
   structures contain the value of these fields for individual
   threads.  */

struct kinfo_proc_layout
{
  /* Offsets of struct kinfo_proc members.  */
  int ki_layout;
  int ki_pid;
  int ki_ppid;
  int ki_pgid;
  int ki_tpgid;
  int ki_sid;
  int ki_tdev_freebsd11;
  int ki_sigignore;
  int ki_sigcatch;
  int ki_uid;
  int ki_ruid;
  int ki_svuid;
  int ki_rgid;
  int ki_svgid;
  int ki_ngroups;
  int ki_groups;
  int ki_size;
  int ki_rssize;
  int ki_tsize;
  int ki_dsize;
  int ki_ssize;
  int ki_start;
  int ki_nice;
  int ki_comm;
  int ki_tdev;
  int ki_rusage;
  int ki_rusage_ch;

  /* Offsets of struct rusage members.  */
  int ru_utime;
  int ru_stime;
  int ru_maxrss;
  int ru_minflt;
  int ru_majflt;
};

const struct kinfo_proc_layout kinfo_proc_layout_32 =
  {
    .ki_layout = 0x4,
    .ki_pid = 0x28,
    .ki_ppid = 0x2c,
    .ki_pgid = 0x30,
    .ki_tpgid = 0x34,
    .ki_sid = 0x38,
    .ki_tdev_freebsd11 = 0x44,
    .ki_sigignore = 0x68,
    .ki_sigcatch = 0x78,
    .ki_uid = 0x88,
    .ki_ruid = 0x8c,
    .ki_svuid = 0x90,
    .ki_rgid = 0x94,
    .ki_svgid = 0x98,
    .ki_ngroups = 0x9c,
    .ki_groups = 0xa0,
    .ki_size = 0xe0,
    .ki_rssize = 0xe4,
    .ki_tsize = 0xec,
    .ki_dsize = 0xf0,
    .ki_ssize = 0xf4,
    .ki_start = 0x118,
    .ki_nice = 0x145,
    .ki_comm = 0x17f,
    .ki_tdev = 0x1f0,
    .ki_rusage = 0x220,
    .ki_rusage_ch = 0x278,

    .ru_utime = 0x0,
    .ru_stime = 0x10,
    .ru_maxrss = 0x20,
    .ru_minflt = 0x30,
    .ru_majflt = 0x34,
  };

const struct kinfo_proc_layout kinfo_proc_layout_i386 =
  {
    .ki_layout = 0x4,
    .ki_pid = 0x28,
    .ki_ppid = 0x2c,
    .ki_pgid = 0x30,
    .ki_tpgid = 0x34,
    .ki_sid = 0x38,
    .ki_tdev_freebsd11 = 0x44,
    .ki_sigignore = 0x68,
    .ki_sigcatch = 0x78,
    .ki_uid = 0x88,
    .ki_ruid = 0x8c,
    .ki_svuid = 0x90,
    .ki_rgid = 0x94,
    .ki_svgid = 0x98,
    .ki_ngroups = 0x9c,
    .ki_groups = 0xa0,
    .ki_size = 0xe0,
    .ki_rssize = 0xe4,
    .ki_tsize = 0xec,
    .ki_dsize = 0xf0,
    .ki_ssize = 0xf4,
    .ki_start = 0x118,
    .ki_nice = 0x135,
    .ki_comm = 0x16f,
    .ki_tdev = 0x1e0,
    .ki_rusage = 0x210,
    .ki_rusage_ch = 0x258,

    .ru_utime = 0x0,
    .ru_stime = 0x8,
    .ru_maxrss = 0x10,
    .ru_minflt = 0x20,
    .ru_majflt = 0x24,
  };

const struct kinfo_proc_layout kinfo_proc_layout_64 =
  {
    .ki_layout = 0x4,
    .ki_pid = 0x48,
    .ki_ppid = 0x4c,
    .ki_pgid = 0x50,
    .ki_tpgid = 0x54,
    .ki_sid = 0x58,
    .ki_tdev_freebsd11 = 0x64,
    .ki_sigignore = 0x88,
    .ki_sigcatch = 0x98,
    .ki_uid = 0xa8,
    .ki_ruid = 0xac,
    .ki_svuid = 0xb0,
    .ki_rgid = 0xb4,
    .ki_svgid = 0xb8,
    .ki_ngroups = 0xbc,
    .ki_groups = 0xc0,
    .ki_size = 0x100,
    .ki_rssize = 0x108,
    .ki_tsize = 0x118,
    .ki_dsize = 0x120,
    .ki_ssize = 0x128,
    .ki_start = 0x150,
    .ki_nice = 0x185,
    .ki_comm = 0x1bf,
    .ki_tdev = 0x230,
    .ki_rusage = 0x260,
    .ki_rusage_ch = 0x2f0,

    .ru_utime = 0x0,
    .ru_stime = 0x10,
    .ru_maxrss = 0x20,
    .ru_minflt = 0x40,
    .ru_majflt = 0x48,
  };

static struct gdbarch_data *fbsd_gdbarch_data_handle;

struct fbsd_gdbarch_data
  {
    struct type *siginfo_type;
  };

static void *
init_fbsd_gdbarch_data (struct gdbarch *gdbarch)
{
  return GDBARCH_OBSTACK_ZALLOC (gdbarch, struct fbsd_gdbarch_data);
}

static struct fbsd_gdbarch_data *
get_fbsd_gdbarch_data (struct gdbarch *gdbarch)
{
  return ((struct fbsd_gdbarch_data *)
	  gdbarch_data (gdbarch, fbsd_gdbarch_data_handle));
}

/* This is how we want PTIDs from core files to be printed.  */

static const char *
fbsd_core_pid_to_str (struct gdbarch *gdbarch, ptid_t ptid)
{
  static char buf[80];

  if (ptid_get_lwp (ptid) != 0)
    {
      xsnprintf (buf, sizeof buf, "LWP %ld", ptid_get_lwp (ptid));
      return buf;
    }

  return normal_pid_to_str (ptid);
}

/* Extract the name assigned to a thread from a core.  Returns the
   string in a static buffer.  */

static const char *
fbsd_core_thread_name (struct gdbarch *gdbarch, struct thread_info *thr)
{
  static char buf[80];
  struct bfd_section *section;
  bfd_size_type size;

  if (ptid_get_lwp (thr->ptid) != 0)
    {
      /* FreeBSD includes a NT_FREEBSD_THRMISC note for each thread
	 whose contents are defined by a "struct thrmisc" declared in
	 <sys/procfs.h> on FreeBSD.  The per-thread name is stored as
	 a null-terminated string as the first member of the
	 structure.  Rather than define the full structure here, just
	 extract the null-terminated name from the start of the
	 note.  */
      thread_section_name section_name (".thrmisc", thr->ptid);

      section = bfd_get_section_by_name (core_bfd, section_name.c_str ());
      if (section != NULL && bfd_section_size (core_bfd, section) > 0)
	{
	  /* Truncate the name if it is longer than "buf".  */
	  size = bfd_section_size (core_bfd, section);
	  if (size > sizeof buf - 1)
	    size = sizeof buf - 1;
	  if (bfd_get_section_contents (core_bfd, section, buf, (file_ptr) 0,
					size)
	      && buf[0] != '\0')
	    {
	      buf[size] = '\0';

	      /* Note that each thread will report the process command
		 as its thread name instead of an empty name if a name
		 has not been set explicitly.  Return a NULL name in
		 that case.  */
	      if (strcmp (buf, elf_tdata (core_bfd)->core->program) != 0)
		return buf;
	    }
	}
    }

  return NULL;
}

/* Implement the "core_xfer_siginfo" gdbarch method.  */

static LONGEST
fbsd_core_xfer_siginfo (struct gdbarch *gdbarch, gdb_byte *readbuf,
			ULONGEST offset, ULONGEST len)
{
  size_t siginfo_size;

  if (gdbarch_long_bit (gdbarch) == 32)
    siginfo_size = SIZE32_SIGINFO_T;
  else
    siginfo_size = SIZE64_SIGINFO_T;
  if (offset > siginfo_size)
    return -1;

  thread_section_name section_name (".note.freebsdcore.lwpinfo", inferior_ptid);
  asection *section = bfd_get_section_by_name (core_bfd, section_name.c_str ());
  if (section == NULL)
    return -1;

  gdb_byte buf[4];
  if (!bfd_get_section_contents (core_bfd, section, buf,
				 LWPINFO_OFFSET + LWPINFO_PL_FLAGS, 4))
    return -1;

  int pl_flags = extract_signed_integer (buf, 4, gdbarch_byte_order (gdbarch));
  if (!(pl_flags & PL_FLAG_SI))
    return -1;

  if (offset + len > siginfo_size)
    len = siginfo_size - offset;

  ULONGEST siginfo_offset;
  if (gdbarch_long_bit (gdbarch) == 32)
    siginfo_offset = LWPINFO_OFFSET + LWPINFO32_PL_SIGINFO;
  else
    siginfo_offset = LWPINFO_OFFSET + LWPINFO64_PL_SIGINFO;

  if (!bfd_get_section_contents (core_bfd, section, readbuf,
				 siginfo_offset + offset, len))
    return -1;

  return len;
}

static int
find_signalled_thread (struct thread_info *info, void *data)
{
  if (info->suspend.stop_signal != GDB_SIGNAL_0
      && ptid_get_pid (info->ptid) == ptid_get_pid (inferior_ptid))
    return 1;

  return 0;
}

/* Structure for passing information from
   fbsd_collect_thread_registers via an iterator to
   fbsd_collect_regset_section_cb. */

struct fbsd_collect_regset_section_cb_data
{
  const struct regcache *regcache;
  bfd *obfd;
  char *note_data;
  int *note_size;
  unsigned long lwp;
  enum gdb_signal stop_signal;
  int abort_iteration;
};

static void
fbsd_collect_regset_section_cb (const char *sect_name, int size,
				const struct regset *regset,
				const char *human_name, void *cb_data)
{
  char *buf;
  struct fbsd_collect_regset_section_cb_data *data
    = (struct fbsd_collect_regset_section_cb_data *) cb_data;

  if (data->abort_iteration)
    return;

  gdb_assert (regset->collect_regset);

  buf = (char *) xmalloc (size);
  regset->collect_regset (regset, data->regcache, -1, buf, size);

  /* PRSTATUS still needs to be treated specially.  */
  if (strcmp (sect_name, ".reg") == 0)
    data->note_data = (char *) elfcore_write_prstatus
      (data->obfd, data->note_data, data->note_size, data->lwp,
       gdb_signal_to_host (data->stop_signal), buf);
  else
    data->note_data = (char *) elfcore_write_register_note
      (data->obfd, data->note_data, data->note_size,
       sect_name, buf, size);
  xfree (buf);

  if (data->note_data == NULL)
    data->abort_iteration = 1;
}

/* Records the thread's register state for the corefile note
   section.  */

static char *
fbsd_collect_thread_registers (const struct regcache *regcache,
			       ptid_t ptid, bfd *obfd,
			       char *note_data, int *note_size,
			       enum gdb_signal stop_signal)
{
  struct gdbarch *gdbarch = regcache->arch ();
  struct fbsd_collect_regset_section_cb_data data;

  data.regcache = regcache;
  data.obfd = obfd;
  data.note_data = note_data;
  data.note_size = note_size;
  data.stop_signal = stop_signal;
  data.abort_iteration = 0;
  data.lwp = ptid_get_lwp (ptid);

  gdbarch_iterate_over_regset_sections (gdbarch,
					fbsd_collect_regset_section_cb,
					&data, regcache);
  return data.note_data;
}

struct fbsd_corefile_thread_data
{
  struct gdbarch *gdbarch;
  bfd *obfd;
  char *note_data;
  int *note_size;
  enum gdb_signal stop_signal;
};

/* Records the thread's register state for the corefile note
   section.  */

static void
fbsd_corefile_thread (struct thread_info *info,
		      struct fbsd_corefile_thread_data *args)
{
  struct regcache *regcache;

  regcache = get_thread_arch_regcache (info->ptid, args->gdbarch);

  target_fetch_registers (regcache, -1);

  args->note_data = fbsd_collect_thread_registers
    (regcache, info->ptid, args->obfd, args->note_data,
     args->note_size, args->stop_signal);
}

/* Create appropriate note sections for a corefile, returning them in
   allocated memory.  */

static char *
fbsd_make_corefile_notes (struct gdbarch *gdbarch, bfd *obfd, int *note_size)
{
  struct fbsd_corefile_thread_data thread_args;
  char *note_data = NULL;
  Elf_Internal_Ehdr *i_ehdrp;
  struct thread_info *curr_thr, *signalled_thr, *thr;

  /* Put a "FreeBSD" label in the ELF header.  */
  i_ehdrp = elf_elfheader (obfd);
  i_ehdrp->e_ident[EI_OSABI] = ELFOSABI_FREEBSD;

  gdb_assert (gdbarch_iterate_over_regset_sections_p (gdbarch));

  if (get_exec_file (0))
    {
      const char *fname = lbasename (get_exec_file (0));
      char *psargs = xstrdup (fname);

      if (get_inferior_args ())
	psargs = reconcat (psargs, psargs, " ", get_inferior_args (),
			   (char *) NULL);

      note_data = elfcore_write_prpsinfo (obfd, note_data, note_size,
					  fname, psargs);
    }

  /* Thread register information.  */
  TRY
    {
      update_thread_list ();
    }
  CATCH (e, RETURN_MASK_ERROR)
    {
      exception_print (gdb_stderr, e);
    }
  END_CATCH

  /* Like the kernel, prefer dumping the signalled thread first.
     "First thread" is what tools use to infer the signalled thread.
     In case there's more than one signalled thread, prefer the
     current thread, if it is signalled.  */
  curr_thr = inferior_thread ();
  if (curr_thr->suspend.stop_signal != GDB_SIGNAL_0)
    signalled_thr = curr_thr;
  else
    {
      signalled_thr = iterate_over_threads (find_signalled_thread, NULL);
      if (signalled_thr == NULL)
	signalled_thr = curr_thr;
    }

  thread_args.gdbarch = gdbarch;
  thread_args.obfd = obfd;
  thread_args.note_data = note_data;
  thread_args.note_size = note_size;
  thread_args.stop_signal = signalled_thr->suspend.stop_signal;

  fbsd_corefile_thread (signalled_thr, &thread_args);
  ALL_NON_EXITED_THREADS (thr)
    {
      if (thr == signalled_thr)
	continue;
      if (ptid_get_pid (thr->ptid) != ptid_get_pid (inferior_ptid))
	continue;

      fbsd_corefile_thread (thr, &thread_args);
    }

  note_data = thread_args.note_data;

  return note_data;
}

/* Helper function to generate mappings flags for a single VM map
   entry in 'info proc mappings'.  */

const char *
fbsd_vm_map_entry_flags (int kve_flags, int kve_protection)
{
  static char vm_flags[9];

  vm_flags[0] = (kve_protection & KINFO_VME_PROT_READ) ? 'r' : '-';
  vm_flags[1] = (kve_protection & KINFO_VME_PROT_WRITE) ? 'w' : '-';
  vm_flags[2] = (kve_protection & KINFO_VME_PROT_EXEC) ? 'x' : '-';
  vm_flags[3] = ' ';
  vm_flags[4] = (kve_flags & KINFO_VME_FLAG_COW) ? 'C' : '-';
  vm_flags[5] = (kve_flags & KINFO_VME_FLAG_NEEDS_COPY) ? 'N' : '-';
  vm_flags[6] = (kve_flags & KINFO_VME_FLAG_SUPER) ? 'S' : '-';
  vm_flags[7] = (kve_flags & KINFO_VME_FLAG_GROWS_UP) ? 'U'
    : (kve_flags & KINFO_VME_FLAG_GROWS_DOWN) ? 'D' : '-';
  vm_flags[8] = '\0';

  return vm_flags;
}

/* Implement "info proc mappings" for a corefile.  */

static void
fbsd_core_info_proc_mappings (struct gdbarch *gdbarch)
{
  asection *section;
  unsigned char *descdata, *descend;
  size_t note_size;

  section = bfd_get_section_by_name (core_bfd, ".note.freebsdcore.vmmap");
  if (section == NULL)
    {
      warning (_("unable to find mappings in core file"));
      return;
    }

  note_size = bfd_get_section_size (section);
  if (note_size < 4)
    error (_("malformed core note - too short for header"));

  gdb::def_vector<unsigned char> contents (note_size);
  if (!bfd_get_section_contents (core_bfd, section, contents.data (),
				 0, note_size))
    error (_("could not get core note contents"));

  descdata = contents.data ();
  descend = descdata + note_size;

  /* Skip over the structure size.  */
  descdata += 4;

  printf_filtered (_("Mapped address spaces:\n\n"));
  if (gdbarch_addr_bit (gdbarch) == 64)
    {
      printf_filtered ("  %18s %18s %10s %10s %9s %s\n",
		       "Start Addr",
		       "  End Addr",
		       "      Size", "    Offset", "Flags  ", "File");
    }
  else
    {
      printf_filtered ("\t%10s %10s %10s %10s %9s %s\n",
		       "Start Addr",
		       "  End Addr",
		       "      Size", "    Offset", "Flags  ", "File");
    }

  while (descdata + KVE_PATH < descend)
    {
      ULONGEST start, end, offset, flags, prot, structsize;

      structsize = bfd_get_32 (core_bfd, descdata + KVE_STRUCTSIZE);
      if (structsize < KVE_PATH)
	error (_("malformed core note - vmmap entry too small"));

      start = bfd_get_64 (core_bfd, descdata + KVE_START);
      end = bfd_get_64 (core_bfd, descdata + KVE_END);
      offset = bfd_get_64 (core_bfd, descdata + KVE_OFFSET);
      flags = bfd_get_32 (core_bfd, descdata + KVE_FLAGS);
      prot = bfd_get_32 (core_bfd, descdata + KVE_PROTECTION);
      if (gdbarch_addr_bit (gdbarch) == 64)
	{
	  printf_filtered ("  %18s %18s %10s %10s %9s %s\n",
			   paddress (gdbarch, start),
			   paddress (gdbarch, end),
			   hex_string (end - start),
			   hex_string (offset),
			   fbsd_vm_map_entry_flags (flags, prot),
			   descdata + KVE_PATH);
	}
      else
	{
	  printf_filtered ("\t%10s %10s %10s %10s %9s %s\n",
			   paddress (gdbarch, start),
			   paddress (gdbarch, end),
			   hex_string (end - start),
			   hex_string (offset),
			   fbsd_vm_map_entry_flags (flags, prot),
			   descdata + KVE_PATH);
	}

      descdata += structsize;
    }
}

/* Fetch the pathname of a vnode for a single file descriptor from the
   file table core note.  */

static gdb::unique_xmalloc_ptr<char>
fbsd_core_vnode_path (struct gdbarch *gdbarch, int fd)
{
  asection *section;
  unsigned char *descdata, *descend;
  size_t note_size;

  section = bfd_get_section_by_name (core_bfd, ".note.freebsdcore.files");
  if (section == NULL)
    return nullptr;

  note_size = bfd_get_section_size (section);
  if (note_size < 4)
    error (_("malformed core note - too short for header"));

  gdb::def_vector<unsigned char> contents (note_size);
  if (!bfd_get_section_contents (core_bfd, section, contents.data (),
				 0, note_size))
    error (_("could not get core note contents"));

  descdata = contents.data ();
  descend = descdata + note_size;

  /* Skip over the structure size.  */
  descdata += 4;

  while (descdata + KVE_PATH < descend)
    {
      ULONGEST structsize;

      structsize = bfd_get_32 (core_bfd, descdata + KF_STRUCTSIZE);
      if (structsize < KVE_PATH)
	error (_("malformed core note - vmmap entry too small"));

      if (bfd_get_32 (core_bfd, descdata + KF_TYPE) == KINFO_FILE_TYPE_VNODE
	  && bfd_get_signed_32 (core_bfd, descdata + KF_FD) == fd)
	{
	  char *path = (char *) descdata + KF_PATH;
	  return gdb::unique_xmalloc_ptr<char> (xstrdup (path));
	}

      descdata += structsize;
    }
  return nullptr;
}

/* Helper function to read a struct timeval.  */

static void
fbsd_core_fetch_timeval (struct gdbarch *gdbarch, unsigned char *data,
			 LONGEST &sec, ULONGEST &usec)
{
  if (gdbarch_addr_bit (gdbarch) == 64)
    {
      sec = bfd_get_signed_64 (core_bfd, data);
      usec = bfd_get_64 (core_bfd, data + 8);
    }
  else if (bfd_get_arch (core_bfd) == bfd_arch_i386)
    {
      sec = bfd_get_signed_32 (core_bfd, data);
      usec = bfd_get_32 (core_bfd, data + 4);
    }
  else
    {
      sec = bfd_get_signed_64 (core_bfd, data);
      usec = bfd_get_32 (core_bfd, data + 8);
    }
}

/* Print out the contents of a signal set.  */

static void
fbsd_print_sigset (const char *descr, unsigned char *sigset)
{
  printf_filtered ("%s: ", descr);
  for (int i = 0; i < SIG_WORDS; i++)
    printf_filtered ("%08x ",
		     (unsigned int) bfd_get_32 (core_bfd, sigset + i * 4));
  printf_filtered ("\n");
}

/* Implement "info proc status" for a corefile.  */

static void
fbsd_core_info_proc_status (struct gdbarch *gdbarch)
{
  const struct kinfo_proc_layout *kp;
  asection *section;
  const char *state;
  unsigned char *descdata;
  int addr_bit, long_bit;
  size_t note_size;
  ULONGEST value;
  LONGEST sec;

  section = bfd_get_section_by_name (core_bfd, ".note.freebsdcore.proc");
  if (section == NULL)
    {
      warning (_("unable to find process info in core file"));
      return;
    }

  addr_bit = gdbarch_addr_bit (gdbarch);
  if (addr_bit == 64)
    kp = &kinfo_proc_layout_64;
  else if (bfd_get_arch (core_bfd) == bfd_arch_i386)
    kp = &kinfo_proc_layout_i386;
  else
    kp = &kinfo_proc_layout_32;
  long_bit = gdbarch_long_bit (gdbarch);

  /*
   * Ensure that the note is large enough for all of the fields fetched
   * by this function.  In particular, the note must contain the 32-bit
   * structure size, then it must be long enough to access the last
   * field used (ki_rusage_ch.ru_majflt) which is the size of a long.
   */
  note_size = bfd_get_section_size (section);
  if (note_size < (4 + kp->ki_rusage_ch + kp->ru_majflt
		   + long_bit / TARGET_CHAR_BIT))
    error (_("malformed core note - too short"));

  gdb::def_vector<unsigned char> contents (note_size);
  if (!bfd_get_section_contents (core_bfd, section, contents.data (),
				 0, note_size))
    error (_("could not get core note contents"));

  descdata = contents.data ();

  /* Skip over the structure size.  */
  descdata += 4;

  /* Verify 'ki_layout' is 0.  */
  if (bfd_get_32 (core_bfd, descdata + kp->ki_layout) != 0)
    {
      warning (_("unsupported process information in core file"));
      return;
    }

  printf_filtered ("Name: %.19s\n", descdata + kp->ki_comm);
  printf_filtered ("Process ID: %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_pid)));
  printf_filtered ("Parent process: %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_ppid)));
  printf_filtered ("Process group: %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_pgid)));
  printf_filtered ("Session id: %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_sid)));

  /* FreeBSD 12.0 and later store a 64-bit dev_t at 'ki_tdev'.  Older
     kernels store a 32-bit dev_t at 'ki_tdev_freebsd11'.  In older
     kernels the 64-bit 'ki_tdev' field is in a reserved section of
     the structure that is cleared to zero.  Assume that a zero value
     in ki_tdev indicates a core dump from an older kernel and use the
     value in 'ki_tdev_freebsd11' instead.  */
  value = bfd_get_64 (core_bfd, descdata + kp->ki_tdev);
  if (value == 0)
    value = bfd_get_32 (core_bfd, descdata + kp->ki_tdev_freebsd11);
  printf_filtered ("TTY: %s\n", pulongest (value));
  printf_filtered ("TTY owner process group: %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_tpgid)));
  printf_filtered ("User IDs (real, effective, saved): %s %s %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_ruid)),
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_uid)),
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_svuid)));
  printf_filtered ("Group IDs (real, effective, saved): %s %s %s\n",
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_rgid)),
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_groups)),
		   pulongest (bfd_get_32 (core_bfd, descdata + kp->ki_svgid)));
  printf_filtered ("Groups: ");
  uint16_t ngroups = bfd_get_16 (core_bfd, descdata + kp->ki_ngroups);
  for (int i = 0; i < ngroups; i++)
    printf_filtered ("%s ",
		     pulongest (bfd_get_32 (core_bfd,
					    descdata + kp->ki_groups + i * 4)));
  printf_filtered ("\n");
  value = bfd_get (long_bit, core_bfd,
		   descdata + kp->ki_rusage + kp->ru_minflt);
  printf_filtered ("Minor faults (no memory page): %s\n", pulongest (value));
  value = bfd_get (long_bit, core_bfd,
		   descdata + kp->ki_rusage_ch + kp->ru_minflt);
  printf_filtered ("Minor faults, children: %s\n", pulongest (value));
  value = bfd_get (long_bit, core_bfd,
		   descdata + kp->ki_rusage + kp->ru_majflt);
  printf_filtered ("Major faults (memory page faults): %s\n",
		   pulongest (value));
  value = bfd_get (long_bit, core_bfd,
		   descdata + kp->ki_rusage_ch + kp->ru_majflt);
  printf_filtered ("Major faults, children: %s\n", pulongest (value));
  fbsd_core_fetch_timeval (gdbarch,
			   descdata + kp->ki_rusage + kp->ru_utime,
			   sec, value);
  printf_filtered ("utime: %s.%06d\n", plongest (sec), (int) value);
  fbsd_core_fetch_timeval (gdbarch,
			   descdata + kp->ki_rusage + kp->ru_stime,
			   sec, value);
  printf_filtered ("stime: %s.%06d\n", plongest (sec), (int) value);
  fbsd_core_fetch_timeval (gdbarch,
			   descdata + kp->ki_rusage_ch + kp->ru_utime,
			   sec, value);
  printf_filtered ("utime, children: %s.%06d\n", plongest (sec), (int) value);
  fbsd_core_fetch_timeval (gdbarch,
			   descdata + kp->ki_rusage_ch + kp->ru_stime,
			   sec, value);
  printf_filtered ("stime, children: %s.%06d\n", plongest (sec), (int) value);
  printf_filtered ("'nice' value: %d\n",
		   bfd_get_signed_8 (core_bfd, descdata + kp->ki_nice));
  fbsd_core_fetch_timeval (gdbarch, descdata + kp->ki_start, sec, value);
  printf_filtered ("Start time: %s.%06d\n", plongest (sec), (int) value);
  printf_filtered ("Virtual memory size: %s kB\n",
		   pulongest (bfd_get (addr_bit, core_bfd,
				       descdata + kp->ki_size) / 1024));
  printf_filtered ("Data size: %s pages\n",
		   pulongest (bfd_get (addr_bit, core_bfd,
				       descdata + kp->ki_dsize)));
  printf_filtered ("Stack size: %s pages\n",
		   pulongest (bfd_get (addr_bit, core_bfd,
				       descdata + kp->ki_ssize)));
  printf_filtered ("Text size: %s pages\n",
		   pulongest (bfd_get (addr_bit, core_bfd,
				       descdata + kp->ki_tsize)));
  printf_filtered ("Resident set size: %s pages\n",
		   pulongest (bfd_get (addr_bit, core_bfd,
				       descdata + kp->ki_rssize)));
  printf_filtered ("Maximum RSS: %s pages\n",
		   pulongest (bfd_get (long_bit, core_bfd,
				       descdata + kp->ki_rusage
				       + kp->ru_maxrss)));
  fbsd_print_sigset ("Ignored Signals", descdata + kp->ki_sigignore);
  fbsd_print_sigset ("Caught Signals", descdata + kp->ki_sigcatch);
}

/* Implement the "core_info_proc" gdbarch method.  */

static void
fbsd_core_info_proc (struct gdbarch *gdbarch, const char *args,
		     enum info_proc_what what)
{
  bool do_cmdline = false;
  bool do_cwd = false;
  bool do_exe = false;
  bool do_mappings = false;
  bool do_status = false;
  int pid;

  switch (what)
    {
    case IP_MINIMAL:
      do_cmdline = true;
      do_cwd = true;
      do_exe = true;
      break;
    case IP_MAPPINGS:
      do_mappings = true;
      break;
    case IP_STATUS:
    case IP_STAT:
      do_status = true;
      break;
    case IP_CMDLINE:
      do_cmdline = true;
      break;
    case IP_EXE:
      do_exe = true;
      break;
    case IP_CWD:
      do_cwd = true;
      break;
    case IP_ALL:
      do_cmdline = true;
      do_cwd = true;
      do_exe = true;
      do_mappings = true;
      do_status = true;
      break;
    default:
      return;
    }

  pid = bfd_core_file_pid (core_bfd);
  if (pid != 0)
    printf_filtered (_("process %d\n"), pid);

  if (do_cmdline)
    {
      const char *cmdline;

      cmdline = bfd_core_file_failing_command (core_bfd);
      if (cmdline)
	printf_filtered ("cmdline = '%s'\n", cmdline);
      else
	warning (_("Command line unavailable"));
    }
  if (do_cwd)
    {
      gdb::unique_xmalloc_ptr<char> cwd =
	fbsd_core_vnode_path (gdbarch, KINFO_FILE_FD_TYPE_CWD);
      if (cwd)
	printf_filtered ("cwd = '%s'\n", cwd.get ());
      else
	warning (_("unable to read current working directory"));
    }
  if (do_exe)
    {
      gdb::unique_xmalloc_ptr<char> exe =
	fbsd_core_vnode_path (gdbarch, KINFO_FILE_FD_TYPE_TEXT);
      if (exe)
	printf_filtered ("exe = '%s'\n", exe.get ());
      else
	warning (_("unable to read executable path name"));
    }
  if (do_mappings)
    fbsd_core_info_proc_mappings (gdbarch);
  if (do_status)
    fbsd_core_info_proc_status (gdbarch);
}

/* Print descriptions of FreeBSD-specific AUXV entries to FILE.  */

static void
fbsd_print_auxv_entry (struct gdbarch *gdbarch, struct ui_file *file,
		       CORE_ADDR type, CORE_ADDR val)
{
  const char *name;
  const char *description;
  enum auxv_format format;

  switch (type)
    {
#define _TAGNAME(tag) #tag
#define TAGNAME(tag) _TAGNAME(AT_##tag)
#define TAG(tag, text, kind) \
      case AT_FREEBSD_##tag: name = TAGNAME(tag); description = text; format = kind; break
      TAG (EXECPATH, _("Executable path"), AUXV_FORMAT_STR);
      TAG (CANARY, _("Canary for SSP"), AUXV_FORMAT_HEX);
      TAG (CANARYLEN, ("Length of the SSP canary"), AUXV_FORMAT_DEC);
      TAG (OSRELDATE, _("OSRELDATE"), AUXV_FORMAT_DEC);
      TAG (NCPUS, _("Number of CPUs"), AUXV_FORMAT_DEC);
      TAG (PAGESIZES, _("Pagesizes"), AUXV_FORMAT_HEX);
      TAG (PAGESIZESLEN, _("Number of pagesizes"), AUXV_FORMAT_DEC);
      TAG (TIMEKEEP, _("Pointer to timehands"), AUXV_FORMAT_HEX);
      TAG (STACKPROT, _("Initial stack protection"), AUXV_FORMAT_HEX);
      TAG (EHDRFLAGS, _("ELF header e_flags"), AUXV_FORMAT_HEX);
      TAG (HWCAP, _("Machine-dependent CPU capability hints"), AUXV_FORMAT_HEX);
    default:
      default_print_auxv_entry (gdbarch, file, type, val);
      return;
    }

  fprint_auxv_entry (file, name, description, format, type, val);
}

/* Implement the "get_siginfo_type" gdbarch method.  */

static struct type *
fbsd_get_siginfo_type (struct gdbarch *gdbarch)
{
  struct fbsd_gdbarch_data *fbsd_gdbarch_data;
  struct type *int_type, *int32_type, *uint32_type, *long_type, *void_ptr_type;
  struct type *uid_type, *pid_type;
  struct type *sigval_type, *reason_type;
  struct type *siginfo_type;
  struct type *type;

  fbsd_gdbarch_data = get_fbsd_gdbarch_data (gdbarch);
  if (fbsd_gdbarch_data->siginfo_type != NULL)
    return fbsd_gdbarch_data->siginfo_type;

  int_type = arch_integer_type (gdbarch, gdbarch_int_bit (gdbarch),
				0, "int");
  int32_type = arch_integer_type (gdbarch, 32, 0, "int32_t");
  uint32_type = arch_integer_type (gdbarch, 32, 1, "uint32_t");
  long_type = arch_integer_type (gdbarch, gdbarch_long_bit (gdbarch),
				 0, "long");
  void_ptr_type = lookup_pointer_type (builtin_type (gdbarch)->builtin_void);

  /* union sigval */
  sigval_type = arch_composite_type (gdbarch, NULL, TYPE_CODE_UNION);
  TYPE_NAME (sigval_type) = xstrdup ("sigval");
  append_composite_type_field (sigval_type, "sival_int", int_type);
  append_composite_type_field (sigval_type, "sival_ptr", void_ptr_type);

  /* __pid_t */
  pid_type = arch_type (gdbarch, TYPE_CODE_TYPEDEF,
			TYPE_LENGTH (int32_type) * TARGET_CHAR_BIT, "__pid_t");
  TYPE_TARGET_TYPE (pid_type) = int32_type;
  TYPE_TARGET_STUB (pid_type) = 1;

  /* __uid_t */
  uid_type = arch_type (gdbarch, TYPE_CODE_TYPEDEF,
			TYPE_LENGTH (uint32_type) * TARGET_CHAR_BIT,
			"__uid_t");
  TYPE_TARGET_TYPE (uid_type) = uint32_type;
  TYPE_TARGET_STUB (uid_type) = 1;

  /* _reason */
  reason_type = arch_composite_type (gdbarch, NULL, TYPE_CODE_UNION);

  /* _fault */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_trapno", int_type);
  append_composite_type_field (reason_type, "_fault", type);

  /* _timer */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_timerid", int_type);
  append_composite_type_field (type, "si_overrun", int_type);
  append_composite_type_field (reason_type, "_timer", type);

  /* _mesgq */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_mqd", int_type);
  append_composite_type_field (reason_type, "_mesgq", type);

  /* _poll */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_band", long_type);
  append_composite_type_field (reason_type, "_poll", type);

  /* __spare__ */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "__spare1__", long_type);
  append_composite_type_field (type, "__spare2__",
			       init_vector_type (int_type, 7));
  append_composite_type_field (reason_type, "__spare__", type);

  /* struct siginfo */
  siginfo_type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  TYPE_NAME (siginfo_type) = xstrdup ("siginfo");
  append_composite_type_field (siginfo_type, "si_signo", int_type);
  append_composite_type_field (siginfo_type, "si_errno", int_type);
  append_composite_type_field (siginfo_type, "si_code", int_type);
  append_composite_type_field (siginfo_type, "si_pid", pid_type);
  append_composite_type_field (siginfo_type, "si_uid", uid_type);
  append_composite_type_field (siginfo_type, "si_status", int_type);
  append_composite_type_field (siginfo_type, "si_addr", void_ptr_type);
  append_composite_type_field (siginfo_type, "si_value", sigval_type);
  append_composite_type_field (siginfo_type, "_reason", reason_type);

  fbsd_gdbarch_data->siginfo_type = siginfo_type;

  return siginfo_type;
}

/* Implement the "get_syscall_number" gdbarch method.  */

static LONGEST
fbsd_get_syscall_number (struct gdbarch *gdbarch, thread_info *thread)
{

  /* FreeBSD doesn't use gdbarch_get_syscall_number since FreeBSD
     native targets fetch the system call number from the
     'pl_syscall_code' member of struct ptrace_lwpinfo in fbsd_wait.
     However, system call catching requires this function to be
     set.  */

  internal_error (__FILE__, __LINE__, _("fbsd_get_sycall_number called"));
}

/* To be called from GDB_OSABI_FREEBSD handlers. */

void
fbsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  set_gdbarch_core_pid_to_str (gdbarch, fbsd_core_pid_to_str);
  set_gdbarch_core_thread_name (gdbarch, fbsd_core_thread_name);
  set_gdbarch_core_xfer_siginfo (gdbarch, fbsd_core_xfer_siginfo);
  set_gdbarch_make_corefile_notes (gdbarch, fbsd_make_corefile_notes);
  set_gdbarch_core_info_proc (gdbarch, fbsd_core_info_proc);
  set_gdbarch_print_auxv_entry (gdbarch, fbsd_print_auxv_entry);
  set_gdbarch_get_siginfo_type (gdbarch, fbsd_get_siginfo_type);

  /* `catch syscall' */
  set_xml_syscall_file_name (gdbarch, "syscalls/freebsd.xml");
  set_gdbarch_get_syscall_number (gdbarch, fbsd_get_syscall_number);
}

void
_initialize_fbsd_tdep (void)
{
  fbsd_gdbarch_data_handle =
    gdbarch_data_register_post_init (init_fbsd_gdbarch_data);
}
