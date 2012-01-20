/* Target-dependent code for GNU/Linux, architecture independent.

   Copyright (C) 2009-2012 Free Software Foundation, Inc.

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
#include "gdbtypes.h"
#include "linux-tdep.h"
#include "auxv.h"
#include "target.h"
#include "elf/common.h"
#include "inferior.h"
#include "cli/cli-utils.h"

#include <ctype.h>

static struct gdbarch_data *linux_gdbarch_data_handle;

struct linux_gdbarch_data
  {
    struct type *siginfo_type;
  };

static void *
init_linux_gdbarch_data (struct gdbarch *gdbarch)
{
  return GDBARCH_OBSTACK_ZALLOC (gdbarch, struct linux_gdbarch_data);
}

static struct linux_gdbarch_data *
get_linux_gdbarch_data (struct gdbarch *gdbarch)
{
  return gdbarch_data (gdbarch, linux_gdbarch_data_handle);
}

/* This function is suitable for architectures that don't
   extend/override the standard siginfo structure.  */

struct type *
linux_get_siginfo_type (struct gdbarch *gdbarch)
{
  struct linux_gdbarch_data *linux_gdbarch_data;
  struct type *int_type, *uint_type, *long_type, *void_ptr_type;
  struct type *uid_type, *pid_type;
  struct type *sigval_type, *clock_type;
  struct type *siginfo_type, *sifields_type;
  struct type *type;

  linux_gdbarch_data = get_linux_gdbarch_data (gdbarch);
  if (linux_gdbarch_data->siginfo_type != NULL)
    return linux_gdbarch_data->siginfo_type;

  int_type = arch_integer_type (gdbarch, gdbarch_int_bit (gdbarch),
			 	0, "int");
  uint_type = arch_integer_type (gdbarch, gdbarch_int_bit (gdbarch),
				 1, "unsigned int");
  long_type = arch_integer_type (gdbarch, gdbarch_long_bit (gdbarch),
				 0, "long");
  void_ptr_type = lookup_pointer_type (builtin_type (gdbarch)->builtin_void);

  /* sival_t */
  sigval_type = arch_composite_type (gdbarch, NULL, TYPE_CODE_UNION);
  TYPE_NAME (sigval_type) = xstrdup ("sigval_t");
  append_composite_type_field (sigval_type, "sival_int", int_type);
  append_composite_type_field (sigval_type, "sival_ptr", void_ptr_type);

  /* __pid_t */
  pid_type = arch_type (gdbarch, TYPE_CODE_TYPEDEF,
			TYPE_LENGTH (int_type), "__pid_t");
  TYPE_TARGET_TYPE (pid_type) = int_type;
  TYPE_TARGET_STUB (pid_type) = 1;

  /* __uid_t */
  uid_type = arch_type (gdbarch, TYPE_CODE_TYPEDEF,
			TYPE_LENGTH (uint_type), "__uid_t");
  TYPE_TARGET_TYPE (uid_type) = uint_type;
  TYPE_TARGET_STUB (uid_type) = 1;

  /* __clock_t */
  clock_type = arch_type (gdbarch, TYPE_CODE_TYPEDEF,
			  TYPE_LENGTH (long_type), "__clock_t");
  TYPE_TARGET_TYPE (clock_type) = long_type;
  TYPE_TARGET_STUB (clock_type) = 1;

  /* _sifields */
  sifields_type = arch_composite_type (gdbarch, NULL, TYPE_CODE_UNION);

  {
    const int si_max_size = 128;
    int si_pad_size;
    int size_of_int = gdbarch_int_bit (gdbarch) / HOST_CHAR_BIT;

    /* _pad */
    if (gdbarch_ptr_bit (gdbarch) == 64)
      si_pad_size = (si_max_size / size_of_int) - 4;
    else
      si_pad_size = (si_max_size / size_of_int) - 3;
    append_composite_type_field (sifields_type, "_pad",
				 init_vector_type (int_type, si_pad_size));
  }

  /* _kill */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_pid", pid_type);
  append_composite_type_field (type, "si_uid", uid_type);
  append_composite_type_field (sifields_type, "_kill", type);

  /* _timer */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_tid", int_type);
  append_composite_type_field (type, "si_overrun", int_type);
  append_composite_type_field (type, "si_sigval", sigval_type);
  append_composite_type_field (sifields_type, "_timer", type);

  /* _rt */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_pid", pid_type);
  append_composite_type_field (type, "si_uid", uid_type);
  append_composite_type_field (type, "si_sigval", sigval_type);
  append_composite_type_field (sifields_type, "_rt", type);

  /* _sigchld */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_pid", pid_type);
  append_composite_type_field (type, "si_uid", uid_type);
  append_composite_type_field (type, "si_status", int_type);
  append_composite_type_field (type, "si_utime", clock_type);
  append_composite_type_field (type, "si_stime", clock_type);
  append_composite_type_field (sifields_type, "_sigchld", type);

  /* _sigfault */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_addr", void_ptr_type);
  append_composite_type_field (sifields_type, "_sigfault", type);

  /* _sigpoll */
  type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_band", long_type);
  append_composite_type_field (type, "si_fd", int_type);
  append_composite_type_field (sifields_type, "_sigpoll", type);

  /* struct siginfo */
  siginfo_type = arch_composite_type (gdbarch, NULL, TYPE_CODE_STRUCT);
  TYPE_NAME (siginfo_type) = xstrdup ("siginfo");
  append_composite_type_field (siginfo_type, "si_signo", int_type);
  append_composite_type_field (siginfo_type, "si_errno", int_type);
  append_composite_type_field (siginfo_type, "si_code", int_type);
  append_composite_type_field_aligned (siginfo_type,
				       "_sifields", sifields_type,
				       TYPE_LENGTH (long_type));

  linux_gdbarch_data->siginfo_type = siginfo_type;

  return siginfo_type;
}

int
linux_has_shared_address_space (void)
{
  /* Determine whether we are running on uClinux or normal Linux
     kernel.  */
  CORE_ADDR dummy;
  int target_is_uclinux;

  target_is_uclinux
    = (target_auxv_search (&current_target, AT_NULL, &dummy) > 0
       && target_auxv_search (&current_target, AT_PAGESZ, &dummy) == 0);

  return target_is_uclinux;
}

/* This is how we want PTIDs from core files to be printed.  */

static char *
linux_core_pid_to_str (struct gdbarch *gdbarch, ptid_t ptid)
{
  static char buf[80];

  if (ptid_get_lwp (ptid) != 0)
    {
      snprintf (buf, sizeof (buf), "LWP %ld", ptid_get_lwp (ptid));
      return buf;
    }

  return normal_pid_to_str (ptid);
}

/* Service function for corefiles and info proc.  */

static void
read_mapping (const char *line,
	      ULONGEST *addr, ULONGEST *endaddr,
	      const char **permissions, size_t *permissions_len,
	      ULONGEST *offset,
              const char **device, size_t *device_len,
	      ULONGEST *inode,
	      const char **filename)
{
  const char *p = line;

  *addr = strtoulst (p, &p, 16);
  if (*p == '-')
    p++;
  *endaddr = strtoulst (p, &p, 16);

  while (*p && isspace (*p))
    p++;
  *permissions = p;
  while (*p && !isspace (*p))
    p++;
  *permissions_len = p - *permissions;

  *offset = strtoulst (p, &p, 16);

  while (*p && isspace (*p))
    p++;
  *device = p;
  while (*p && !isspace (*p))
    p++;
  *device_len = p - *device;

  *inode = strtoulst (p, &p, 10);

  while (*p && isspace (*p))
    p++;
  *filename = p;
}

/* Implement the "info proc" command.  */

static void
linux_info_proc (struct gdbarch *gdbarch, char *args,
		 enum info_proc_what what)
{
  /* A long is used for pid instead of an int to avoid a loss of precision
     compiler warning from the output of strtoul.  */
  long pid;
  int cmdline_f = (what == IP_MINIMAL || what == IP_CMDLINE || what == IP_ALL);
  int cwd_f = (what == IP_MINIMAL || what == IP_CWD || what == IP_ALL);
  int exe_f = (what == IP_MINIMAL || what == IP_EXE || what == IP_ALL);
  int mappings_f = (what == IP_MAPPINGS || what == IP_ALL);
  int status_f = (what == IP_STATUS || what == IP_ALL);
  int stat_f = (what == IP_STAT || what == IP_ALL);
  char filename[100];
  gdb_byte *data;
  int target_errno;

  if (args && isdigit (args[0]))
    pid = strtoul (args, &args, 10);
  else
    {
      if (!target_has_execution)
	error (_("No current process: you must name one."));
      if (current_inferior ()->fake_pid_p)
	error (_("Can't determine the current process's PID: you must name one."));

      pid = current_inferior ()->pid;
    }

  args = skip_spaces (args);
  if (args && args[0])
    error (_("Too many parameters: %s"), args);

  printf_filtered (_("process %ld\n"), pid);
  if (cmdline_f)
    {
      xsnprintf (filename, sizeof filename, "/proc/%ld/cmdline", pid);
      data = target_fileio_read_stralloc (filename);
      if (data)
	{
	  struct cleanup *cleanup = make_cleanup (xfree, data);
          printf_filtered ("cmdline = '%s'\n", data);
	  do_cleanups (cleanup);
	}
      else
	warning (_("unable to open /proc file '%s'"), filename);
    }
  if (cwd_f)
    {
      xsnprintf (filename, sizeof filename, "/proc/%ld/cwd", pid);
      data = target_fileio_readlink (filename, &target_errno);
      if (data)
	{
	  struct cleanup *cleanup = make_cleanup (xfree, data);
          printf_filtered ("cwd = '%s'\n", data);
	  do_cleanups (cleanup);
	}
      else
	warning (_("unable to read link '%s'"), filename);
    }
  if (exe_f)
    {
      xsnprintf (filename, sizeof filename, "/proc/%ld/exe", pid);
      data = target_fileio_readlink (filename, &target_errno);
      if (data)
	{
	  struct cleanup *cleanup = make_cleanup (xfree, data);
          printf_filtered ("exe = '%s'\n", data);
	  do_cleanups (cleanup);
	}
      else
	warning (_("unable to read link '%s'"), filename);
    }
  if (mappings_f)
    {
      xsnprintf (filename, sizeof filename, "/proc/%ld/maps", pid);
      data = target_fileio_read_stralloc (filename);
      if (data)
	{
	  struct cleanup *cleanup = make_cleanup (xfree, data);
	  char *line;

	  printf_filtered (_("Mapped address spaces:\n\n"));
	  if (gdbarch_addr_bit (gdbarch) == 32)
	    {
	      printf_filtered ("\t%10s %10s %10s %10s %s\n",
			   "Start Addr",
			   "  End Addr",
			   "      Size", "    Offset", "objfile");
            }
	  else
            {
	      printf_filtered ("  %18s %18s %10s %10s %s\n",
			   "Start Addr",
			   "  End Addr",
			   "      Size", "    Offset", "objfile");
	    }

	  for (line = strtok (data, "\n"); line; line = strtok (NULL, "\n"))
	    {
	      ULONGEST addr, endaddr, offset, inode;
	      const char *permissions, *device, *filename;
	      size_t permissions_len, device_len;

	      read_mapping (line, &addr, &endaddr,
			    &permissions, &permissions_len,
			    &offset, &device, &device_len,
			    &inode, &filename);

	      if (gdbarch_addr_bit (gdbarch) == 32)
	        {
	          printf_filtered ("\t%10s %10s %10s %10s %s\n",
				   paddress (gdbarch, addr),
				   paddress (gdbarch, endaddr),
				   hex_string (endaddr - addr),
				   hex_string (offset),
				   *filename? filename : "");
		}
	      else
	        {
	          printf_filtered ("  %18s %18s %10s %10s %s\n",
				   paddress (gdbarch, addr),
				   paddress (gdbarch, endaddr),
				   hex_string (endaddr - addr),
				   hex_string (offset),
				   *filename? filename : "");
	        }
	    }

	  do_cleanups (cleanup);
	}
      else
	warning (_("unable to open /proc file '%s'"), filename);
    }
  if (status_f)
    {
      xsnprintf (filename, sizeof filename, "/proc/%ld/status", pid);
      data = target_fileio_read_stralloc (filename);
      if (data)
	{
	  struct cleanup *cleanup = make_cleanup (xfree, data);
          puts_filtered (data);
	  do_cleanups (cleanup);
	}
      else
	warning (_("unable to open /proc file '%s'"), filename);
    }
  if (stat_f)
    {
      xsnprintf (filename, sizeof filename, "/proc/%ld/stat", pid);
      data = target_fileio_read_stralloc (filename);
      if (data)
	{
	  struct cleanup *cleanup = make_cleanup (xfree, data);
	  const char *p = data;
	  const char *ep;
	  ULONGEST val;

	  printf_filtered (_("Process: %s\n"),
			   pulongest (strtoulst (p, &p, 10)));

	  while (*p && isspace (*p))
	    p++;
	  if (*p == '(' && (ep = strchr (p, ')')) != NULL)
	    {
	      printf_filtered ("Exec file: %.*s\n", (int) (ep - p - 1), p + 1);
	      p = ep + 1;
	    }

	  while (*p && isspace (*p))
	    p++;
	  if (*p)
	    printf_filtered (_("State: %c\n"), *p++);

	  if (*p)
	    printf_filtered (_("Parent process: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Process group: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Session id: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("TTY: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("TTY owner process group: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));

	  if (*p)
	    printf_filtered (_("Flags: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Minor faults (no memory page): %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Minor faults, children: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Major faults (memory page faults): %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Major faults, children: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("utime: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("stime: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("utime, children: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("stime, children: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("jiffies remaining in current "
			       "time slice: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("'nice' value: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("jiffies until next timeout: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("jiffies until next SIGALRM: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("start time (jiffies since "
			       "system boot): %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Virtual memory size: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Resident set size: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("rlim: %s\n"),
			     pulongest (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Start of text: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("End of text: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Start of stack: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
#if 0	/* Don't know how architecture-dependent the rest is...
	   Anyway the signal bitmap info is available from "status".  */
	  if (*p)
	    printf_filtered (_("Kernel stack pointer: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Kernel instr pointer: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Pending signals bitmap: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Blocked signals bitmap: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Ignored signals bitmap: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("Catched signals bitmap: %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
	  if (*p)
	    printf_filtered (_("wchan (system call): %s\n"),
			     hex_string (strtoulst (p, &p, 10)));
#endif
	  do_cleanups (cleanup);
	}
      else
	warning (_("unable to open /proc file '%s'"), filename);
    }
}

/* To be called from the various GDB_OSABI_LINUX handlers for the
   various GNU/Linux architectures and machine types.  */

void
linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  set_gdbarch_core_pid_to_str (gdbarch, linux_core_pid_to_str);
  set_gdbarch_info_proc (gdbarch, linux_info_proc);
}

void
_initialize_linux_tdep (void)
{
  linux_gdbarch_data_handle =
    gdbarch_data_register_post_init (init_linux_gdbarch_data);
}
