/* Linux-specific methods for using the /proc file system.
   Copyright 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "inferior.h"
#include <sys/param.h>	/* for MAXPATHLEN */
#include <sys/procfs.h>
#include "gregset.h"	/* for gregset */
#include "gdbcore.h"	/* for get_exec_file */
#include "gdbthread.h"	/* for struct thread_info etc. */
#include "elf-bfd.h"

/* Function: child_pid_to_exec_file
 *
 * Accepts an integer pid
 * Returns a string representing a file that can be opened
 * to get the symbols for the child process.
 */

char *
child_pid_to_exec_file (int pid)
{
  char *name1, *name2;

  name1 = xmalloc (MAXPATHLEN);
  name2 = xmalloc (MAXPATHLEN);
  make_cleanup (xfree, name1);
  make_cleanup (xfree, name2);
  memset (name2, 0, MAXPATHLEN);

  sprintf (name1, "/proc/%d/exe", pid);
  if (readlink (name1, name2, MAXPATHLEN) > 0)
    return name2;
  else
    return name1;
}

/* Function: linux_find_memory_regions
 *
 * Fills the "to_find_memory_regions" target vector.
 * Lists the memory regions in the inferior for a corefile.
 */

static int
linux_find_memory_regions (int (*func) (CORE_ADDR, 
					unsigned long,
					int, int, int,
					void *), 
			   void *obfd)
{
  long long pid = PIDGET (inferior_ptid);
  char procfilename[MAXPATHLEN];
  FILE *procfile;
  long long addr, endaddr, size, offset, inode;
  char perms[8], dev[8], filename[MAXPATHLEN];
  int read, write, exec;
  int ret;

  /* Compose the filename for the /proc memory map, and open it. */
  sprintf (procfilename, "/proc/%lld/maps", pid);
  if ((procfile = fopen (procfilename, "r")) == NULL)
    error ("Could not open %s\n", procfilename);

  if (info_verbose)
    fprintf_filtered (gdb_stdout, 
		      "Reading memory regions from %s\n", procfilename);

  /* Read the first memory segment descriptor from the maps file.  */
  ret = fscanf (procfile, "%llx-%llx %s %llx %s %llx", 
		&addr, &endaddr, perms, &offset, dev, &inode);
  if (inode)
    fscanf (procfile, " %s\n", filename);
  else
    {
      filename[0] = '\0';
      fscanf (procfile, "\n");
    }

  /* Now iterate until end-of-file. */
  while (ret > 0 && ret != EOF)
    {
      size = endaddr - addr;

      /* Get the segment's permissions.  */
      read  = (strchr (perms, 'r') != 0);
      write = (strchr (perms, 'w') != 0);
      exec  = (strchr (perms, 'x') != 0);

      if (info_verbose)
	{
	  fprintf_filtered (gdb_stdout, 
			    "Save segment, %lld bytes at 0x%s (%c%c%c)", 
			    size, paddr_nz (addr), 
			    read  ? 'r' : ' ', 
			    write ? 'w' : ' ',
			    exec  ? 'x' : ' ');
	  if (filename && filename[0])
	    fprintf_filtered (gdb_stdout, 
			      " for %s", filename);
	  fprintf_filtered (gdb_stdout, "\n");
	}

      /* Invoke the callback function to create the corefile segment. */
      func (addr, size, read, write, exec, obfd);

      /* Read the next memory region.  */
      filename[0] = '\0';
      ret = fscanf (procfile, "%llx-%llx %s %llx %s %llx", 
		    &addr, &endaddr, perms, &offset, dev, &inode);
      if (inode)
	fscanf (procfile, " %s\n", filename);
      else
	{
	  filename[0] = '\0';
	  fscanf (procfile, "\n");
	}
    }

  fclose (procfile);
  return 0;
}

/* Function: linux_do_thread_registers
 *
 * Records the thread's register state for the corefile note section.
 */

static char *
linux_do_thread_registers (bfd *obfd, ptid_t ptid, 
			   char *note_data, int *note_size)
{
  gdb_gregset_t gregs;
  gdb_fpregset_t fpregs;
  unsigned long merged_pid = ptid_get_tid (ptid) << 16 | ptid_get_pid (ptid);

  fill_gregset (&gregs, -1);
  note_data = (char *) elfcore_write_prstatus (obfd, 
					       note_data, 
					       note_size, 
					       merged_pid, 
					       stop_signal, 
					       &gregs);

  fill_fpregset (&fpregs, -1);
  note_data = (char *) elfcore_write_prfpreg (obfd, 
					      note_data, 
					      note_size, 
					      &fpregs, 
					      sizeof (fpregs));
  return note_data;
}

struct linux_corefile_thread_data {
  bfd  *obfd;
  char *note_data;
  int  *note_size;
};

/* Function: linux_corefile_thread_callback
 * 
 * Called by gdbthread.c once per thread.
 * Records the thread's register state for the corefile note section.
 */

static int
linux_corefile_thread_callback (struct thread_info *ti, void *data)
{
  struct linux_corefile_thread_data *args = data;
  ptid_t saved_ptid = inferior_ptid;

  inferior_ptid = ti->ptid;
  registers_changed ();
  target_fetch_registers (-1);	/* FIXME should not be necessary; 
				   fill_gregset should do it automatically. */
  args->note_data = linux_do_thread_registers (args->obfd, 
					       ti->ptid, 
					       args->note_data, 
					       args->note_size);
  inferior_ptid = saved_ptid;
  registers_changed ();
  target_fetch_registers (-1);	/* FIXME should not be necessary; 
				   fill_gregset should do it automatically. */
  return 0;
}

/* Function: linux_make_note_section
 *
 * Fills the "to_make_corefile_note" target vector.
 * Builds the note section for a corefile, and returns it 
 * in a malloc buffer. 
 */

static char *
linux_make_note_section (bfd *obfd, int *note_size)
{
  struct linux_corefile_thread_data thread_args;
  struct cleanup *old_chain;
  char fname[16] = {'\0'};
  char psargs[80] = {'\0'};
  char *note_data = NULL;
  ptid_t current_ptid = inferior_ptid;

  if (get_exec_file (0))
    {
      strncpy (fname, strrchr (get_exec_file (0), '/') + 1, sizeof (fname));
      strncpy (psargs, get_exec_file (0), 
	       sizeof (psargs));
      if (get_inferior_args ())
	{
	  strncat (psargs, " ", 
		   sizeof (psargs) - strlen (psargs));
	  strncat (psargs, get_inferior_args (), 
		   sizeof (psargs) - strlen (psargs));
	}
      note_data = (char *) elfcore_write_prpsinfo (obfd, 
						   note_data, 
						   note_size, 
						   fname, 
						   psargs);
    }

  /* Dump information for threads.  */
  thread_args.obfd = obfd;
  thread_args.note_data = note_data;
  thread_args.note_size = note_size;
  iterate_over_threads (linux_corefile_thread_callback, &thread_args);
  if (thread_args.note_data == note_data)
    {
      /* iterate_over_threads didn't come up with any threads;
	 just use inferior_ptid. */
      note_data = linux_do_thread_registers (obfd, inferior_ptid, 
					     note_data, note_size);
    }
  else
    {
      note_data = thread_args.note_data;
    }

  make_cleanup (xfree, note_data);
  return note_data;
}

void
_initialize_linux_proc (void)
{
  extern void inftarg_set_find_memory_regions ();
  extern void inftarg_set_make_corefile_notes ();

  inftarg_set_find_memory_regions (linux_find_memory_regions);
  inftarg_set_make_corefile_notes (linux_make_note_section);
}
