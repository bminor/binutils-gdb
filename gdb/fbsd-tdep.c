/* Target-dependent code for FreeBSD, architecture-independent.

   Copyright (C) 2002-2016 Free Software Foundation, Inc.

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
#include "gdbcore.h"
#include "inferior.h"
#include "regcache.h"
#include "regset.h"
#include "gdbthread.h"

#include "elf-bfd.h"
#include "fbsd-tdep.h"


/* This is how we want PTIDs from core files to be printed.  */

static char *
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
  char sectionstr[32];

  if (ptid_get_lwp (thr->ptid) != 0)
    {
      /* FreeBSD includes a NT_FREEBSD_THRMISC note for each thread
	 whose contents are defined by a "struct thrmisc" declared in
	 <sys/procfs.h> on FreeBSD.  The per-thread name is stored as
	 a null-terminated string as the first member of the
	 structure.  Rather than define the full structure here, just
	 extract the null-terminated name from the start of the
	 note.  */
      xsnprintf (sectionstr, sizeof sectionstr, ".thrmisc/%ld",
		ptid_get_lwp (thr->ptid));
      section = bfd_get_section_by_name (core_bfd, sectionstr);
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

static int
find_signalled_thread (struct thread_info *info, void *data)
{
  if (info->suspend.stop_signal != GDB_SIGNAL_0
      && ptid_get_pid (info->ptid) == ptid_get_pid (inferior_ptid))
    return 1;

  return 0;
}

static enum gdb_signal
find_stop_signal (void)
{
  struct thread_info *info =
    iterate_over_threads (find_signalled_thread, NULL);

  if (info)
    return info->suspend.stop_signal;
  else
    return GDB_SIGNAL_0;
}

struct fbsd_collect_regset_section_cb_data
{
  const struct regcache *regcache;
  bfd *obfd;
  char *note_data;
  int *note_size;
};

static void
fbsd_collect_regset_section_cb (const char *sect_name, int size,
				const struct regset *regset,
				const char *human_name, void *cb_data)
{
  char *buf;
  struct fbsd_collect_regset_section_cb_data *data
    = (struct fbsd_collect_regset_section_cb_data *) cb_data;

  gdb_assert (regset->collect_regset);

  buf = (char *) xmalloc (size);
  regset->collect_regset (regset, data->regcache, -1, buf, size);

  /* PRSTATUS still needs to be treated specially.  */
  if (strcmp (sect_name, ".reg") == 0)
    data->note_data = (char *) elfcore_write_prstatus
      (data->obfd, data->note_data, data->note_size,
       ptid_get_pid (inferior_ptid), find_stop_signal (), buf);
  else
    data->note_data = (char *) elfcore_write_register_note
      (data->obfd, data->note_data, data->note_size,
       sect_name, buf, size);
  xfree (buf);
}

/* Create appropriate note sections for a corefile, returning them in
   allocated memory.  */

static char *
fbsd_make_corefile_notes (struct gdbarch *gdbarch, bfd *obfd, int *note_size)
{
  struct regcache *regcache = get_current_regcache ();
  char *note_data;
  Elf_Internal_Ehdr *i_ehdrp;
  struct fbsd_collect_regset_section_cb_data data;

  /* Put a "FreeBSD" label in the ELF header.  */
  i_ehdrp = elf_elfheader (obfd);
  i_ehdrp->e_ident[EI_OSABI] = ELFOSABI_FREEBSD;

  gdb_assert (gdbarch_iterate_over_regset_sections_p (gdbarch));

  data.regcache = regcache;
  data.obfd = obfd;
  data.note_data = NULL;
  data.note_size = note_size;
  target_fetch_registers (regcache, -1);
  gdbarch_iterate_over_regset_sections (gdbarch,
					fbsd_collect_regset_section_cb,
					&data, regcache);
  note_data = data.note_data;

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

  return note_data;
}

/* To be called from GDB_OSABI_FREEBSD_ELF handlers. */

void
fbsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  set_gdbarch_core_pid_to_str (gdbarch, fbsd_core_pid_to_str);
  set_gdbarch_core_thread_name (gdbarch, fbsd_core_thread_name);
  set_gdbarch_make_corefile_notes (gdbarch, fbsd_make_corefile_notes);
}
