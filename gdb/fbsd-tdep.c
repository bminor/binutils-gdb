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
fbsd_get_syscall_number (struct gdbarch *gdbarch,
			 ptid_t ptid)
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
