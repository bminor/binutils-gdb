/* Linux-specific memory maps manipulation routines.
   Copyright (C) 2015 Free Software Foundation, Inc.

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

#include "common-defs.h"
#include "linux-maps.h"
#include <ctype.h>
#include "target/target-utils.h"
#include "gdb_regex.h"
#include "target/target.h"

/* This struct is used to map flags found in the "VmFlags:" field (in
   the /proc/<PID>/smaps file).  */

struct smaps_vmflags
  {
    /* Zero if this structure has not been initialized yet.  It
       probably means that the Linux kernel being used does not emit
       the "VmFlags:" field on "/proc/PID/smaps".  */

    unsigned int initialized_p : 1;

    /* Memory mapped I/O area (VM_IO, "io").  */

    unsigned int io_page : 1;

    /* Area uses huge TLB pages (VM_HUGETLB, "ht").  */

    unsigned int uses_huge_tlb : 1;

    /* Do not include this memory region on the coredump (VM_DONTDUMP, "dd").  */

    unsigned int exclude_coredump : 1;

    /* Is this a MAP_SHARED mapping (VM_SHARED, "sh").  */

    unsigned int shared_mapping : 1;
  };

/* Service function for corefiles and info proc.  */

void
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

  p = skip_spaces_const (p);
  *permissions = p;
  while (*p && !isspace (*p))
    p++;
  *permissions_len = p - *permissions;

  *offset = strtoulst (p, &p, 16);

  p = skip_spaces_const (p);
  *device = p;
  while (*p && !isspace (*p))
    p++;
  *device_len = p - *device;

  *inode = strtoulst (p, &p, 10);

  p = skip_spaces_const (p);
  *filename = p;
}

/* Helper function to decode the "VmFlags" field in /proc/PID/smaps.

   This function was based on the documentation found on
   <Documentation/filesystems/proc.txt>, on the Linux kernel.

   Linux kernels before commit
   834f82e2aa9a8ede94b17b656329f850c1471514 (3.10) do not have this
   field on smaps.  */

static void
decode_vmflags (char *p, struct smaps_vmflags *v)
{
  char *saveptr = NULL;
  const char *s;

  v->initialized_p = 1;
  p = skip_to_space (p);
  p = skip_spaces (p);

  for (s = strtok_r (p, " ", &saveptr);
       s != NULL;
       s = strtok_r (NULL, " ", &saveptr))
    {
      if (strcmp (s, "io") == 0)
	v->io_page = 1;
      else if (strcmp (s, "ht") == 0)
	v->uses_huge_tlb = 1;
      else if (strcmp (s, "dd") == 0)
	v->exclude_coredump = 1;
      else if (strcmp (s, "sh") == 0)
	v->shared_mapping = 1;
    }
}

/* Return 1 if the memory mapping is anonymous, 0 otherwise.

   FILENAME is the name of the file present in the first line of the
   memory mapping, in the "/proc/PID/smaps" output.  For example, if
   the first line is:

   7fd0ca877000-7fd0d0da0000 r--p 00000000 fd:02 2100770   /path/to/file

   Then FILENAME will be "/path/to/file".  */

static int
mapping_is_anonymous_p (const char *filename)
{
  static regex_t dev_zero_regex, shmem_file_regex, file_deleted_regex;
  static int init_regex_p = 0;

  if (!init_regex_p)
    {
      struct cleanup *c = make_cleanup (null_cleanup, NULL);

      /* Let's be pessimistic and assume there will be an error while
	 compiling the regex'es.  */
      init_regex_p = -1;

      /* DEV_ZERO_REGEX matches "/dev/zero" filenames (with or
	 without the "(deleted)" string in the end).  We know for
	 sure, based on the Linux kernel code, that memory mappings
	 whose associated filename is "/dev/zero" are guaranteed to be
	 MAP_ANONYMOUS.  */
      compile_rx_or_error (&dev_zero_regex, "^/dev/zero\\( (deleted)\\)\\?$",
			   _("Could not compile regex to match /dev/zero "
			     "filename"));
      /* SHMEM_FILE_REGEX matches "/SYSV%08x" filenames (with or
	 without the "(deleted)" string in the end).  These filenames
	 refer to shared memory (shmem), and memory mappings
	 associated with them are MAP_ANONYMOUS as well.  */
      compile_rx_or_error (&shmem_file_regex,
			   "^/\\?SYSV[0-9a-fA-F]\\{8\\}\\( (deleted)\\)\\?$",
			   _("Could not compile regex to match shmem "
			     "filenames"));
      /* FILE_DELETED_REGEX is a heuristic we use to try to mimic the
	 Linux kernel's 'n_link == 0' code, which is responsible to
	 decide if it is dealing with a 'MAP_SHARED | MAP_ANONYMOUS'
	 mapping.  In other words, if FILE_DELETED_REGEX matches, it
	 does not necessarily mean that we are dealing with an
	 anonymous shared mapping.  However, there is no easy way to
	 detect this currently, so this is the best approximation we
	 have.

	 As a result, GDB will dump readonly pages of deleted
	 executables when using the default value of coredump_filter
	 (0x33), while the Linux kernel will not dump those pages.
	 But we can live with that.  */
      compile_rx_or_error (&file_deleted_regex, " (deleted)$",
			   _("Could not compile regex to match "
			     "'<file> (deleted)'"));
      /* We will never release these regexes, so just discard the
	 cleanups.  */
      discard_cleanups (c);

      /* If we reached this point, then everything succeeded.  */
      init_regex_p = 1;
    }

  if (init_regex_p == -1)
    {
      const char deleted[] = " (deleted)";
      size_t del_len = sizeof (deleted) - 1;
      size_t filename_len = strlen (filename);

      /* There was an error while compiling the regex'es above.  In
	 order to try to give some reliable information to the caller,
	 we just try to find the string " (deleted)" in the filename.
	 If we managed to find it, then we assume the mapping is
	 anonymous.  */
      return (filename_len >= del_len
	      && strcmp (filename + filename_len - del_len, deleted) == 0);
    }

  if (*filename == '\0'
      || regexec (&dev_zero_regex, filename, 0, NULL, 0) == 0
      || regexec (&shmem_file_regex, filename, 0, NULL, 0) == 0
      || regexec (&file_deleted_regex, filename, 0, NULL, 0) == 0)
    return 1;

  return 0;
}

/* Return 0 if the memory mapping (which is related to FILTERFLAGS, V,
   MAYBE_PRIVATE_P, and MAPPING_ANONYMOUS_P) should not be dumped, or
   greater than 0 if it should.

   In a nutshell, this is the logic that we follow in order to decide
   if a mapping should be dumped or not.

   - If the mapping is associated to a file whose name ends with
     " (deleted)", or if the file is "/dev/zero", or if it is
     "/SYSV%08x" (shared memory), or if there is no file associated
     with it, or if the AnonHugePages: or the Anonymous: fields in the
     /proc/PID/smaps have contents, then GDB considers this mapping to
     be anonymous.  Otherwise, GDB considers this mapping to be a
     file-backed mapping (because there will be a file associated with
     it).
 
     It is worth mentioning that, from all those checks described
     above, the most fragile is the one to see if the file name ends
     with " (deleted)".  This does not necessarily mean that the
     mapping is anonymous, because the deleted file associated with
     the mapping may have been a hard link to another file, for
     example.  The Linux kernel checks to see if "i_nlink == 0", but
     GDB cannot easily (and normally) do this check (iff running as
     root, it could find the mapping in /proc/PID/map_files/ and
     determine whether there still are other hard links to the
     inode/file).  Therefore, we made a compromise here, and we assume
     that if the file name ends with " (deleted)", then the mapping is
     indeed anonymous.  FWIW, this is something the Linux kernel could
     do better: expose this information in a more direct way.
 
   - If we see the flag "sh" in the "VmFlags:" field (in
     /proc/PID/smaps), then certainly the memory mapping is shared
     (VM_SHARED).  If we have access to the VmFlags, and we don't see
     the "sh" there, then certainly the mapping is private.  However,
     Linux kernels before commit
     834f82e2aa9a8ede94b17b656329f850c1471514 (3.10) do not have the
     "VmFlags:" field; in that case, we use another heuristic: if we
     see 'p' in the permission flags, then we assume that the mapping
     is private, even though the presence of the 's' flag there would
     mean VM_MAYSHARE, which means the mapping could still be private.
     This should work OK enough, however.  */

static int
dump_mapping_p (enum filterflags filterflags, const struct smaps_vmflags *v,
		int maybe_private_p, int mapping_anon_p, int mapping_file_p,
		const char *filename)
{
  /* Initially, we trust in what we received from our caller.  This
     value may not be very precise (i.e., it was probably gathered
     from the permission line in the /proc/PID/smaps list, which
     actually refers to VM_MAYSHARE, and not VM_SHARED), but it is
     what we have until we take a look at the "VmFlags:" field
     (assuming that the version of the Linux kernel being used
     supports it, of course).  */
  int private_p = maybe_private_p;

  /* We always dump vDSO and vsyscall mappings, because it's likely that
     there'll be no file to read the contents from at core load time.
     The kernel does the same.  */
  if (strcmp ("[vdso]", filename) == 0
      || strcmp ("[vsyscall]", filename) == 0)
    return 1;

  if (v->initialized_p)
    {
      /* We never dump I/O mappings.  */
      if (v->io_page)
	return 0;

      /* Check if we should exclude this mapping.  */
      if (v->exclude_coredump)
	return 0;

      /* Update our notion of whether this mapping is shared or
	 private based on a trustworthy value.  */
      private_p = !v->shared_mapping;

      /* HugeTLB checking.  */
      if (v->uses_huge_tlb)
	{
	  if ((private_p && (filterflags & COREFILTER_HUGETLB_PRIVATE))
	      || (!private_p && (filterflags & COREFILTER_HUGETLB_SHARED)))
	    return 1;

	  return 0;
	}
    }

  if (private_p)
    {
      if (mapping_anon_p && mapping_file_p)
	{
	  /* This is a special situation.  It can happen when we see a
	     mapping that is file-backed, but that contains anonymous
	     pages.  */
	  return ((filterflags & COREFILTER_ANON_PRIVATE) != 0
		  || (filterflags & COREFILTER_MAPPED_PRIVATE) != 0);
	}
      else if (mapping_anon_p)
	return (filterflags & COREFILTER_ANON_PRIVATE) != 0;
      else
	return (filterflags & COREFILTER_MAPPED_PRIVATE) != 0;
    }
  else
    {
      if (mapping_anon_p && mapping_file_p)
	{
	  /* This is a special situation.  It can happen when we see a
	     mapping that is file-backed, but that contains anonymous
	     pages.  */
	  return ((filterflags & COREFILTER_ANON_SHARED) != 0
		  || (filterflags & COREFILTER_MAPPED_SHARED) != 0);
	}
      else if (mapping_anon_p)
	return (filterflags & COREFILTER_ANON_SHARED) != 0;
      else
	return (filterflags & COREFILTER_MAPPED_SHARED) != 0;
    }
}

/* List memory regions in the inferior PID matched to FILTERFLAGS for
   a corefile.  Call FUNC with FUNC_DATA for each such region.  Return
   immediately with the value returned by FUNC if it is non-zero.
   *MEMORY_TO_FREE_PTR should be registered to be freed automatically if
   called FUNC throws an exception.  MEMORY_TO_FREE_PTR can be also
   passed as NULL if it is not used.  Return -1 if error occurs, 0 if
   all memory regions have been processed or return the value from FUNC
   if FUNC returns non-zero.  */

int
linux_find_memory_regions_full (pid_t pid, enum filterflags filterflags,
				linux_find_memory_region_ftype *func,
				void *func_data)
{
  char mapsfilename[100];
  char *data;

  xsnprintf (mapsfilename, sizeof mapsfilename, "/proc/%d/smaps", pid);
  data = target_fileio_read_stralloc (NULL, mapsfilename);
  if (data == NULL)
    {
      /* Older Linux kernels did not support /proc/PID/smaps.  */
      xsnprintf (mapsfilename, sizeof mapsfilename, "/proc/%d/maps", pid);
      data = target_fileio_read_stralloc (NULL, mapsfilename);
    }

  if (data != NULL)
    {
      struct cleanup *cleanup = make_cleanup (xfree, data);
      char *line, *t;
      int retval = 0;

      line = strtok_r (data, "\n", &t);
      while (line != NULL)
	{
	  ULONGEST addr, endaddr, offset, inode;
	  const char *permissions, *device, *filename;
	  struct smaps_vmflags v;
	  size_t permissions_len, device_len;
	  int read, write, exec, priv;
	  int has_anonymous = 0;
	  int should_dump_p = 0;
	  int mapping_anon_p;
	  int mapping_file_p;

	  memset (&v, 0, sizeof (v));
	  read_mapping (line, &addr, &endaddr, &permissions, &permissions_len,
			&offset, &device, &device_len, &inode, &filename);
	  mapping_anon_p = mapping_is_anonymous_p (filename);
	  /* If the mapping is not anonymous, then we can consider it
	     to be file-backed.  These two states (anonymous or
	     file-backed) seem to be exclusive, but they can actually
	     coexist.  For example, if a file-backed mapping has
	     "Anonymous:" pages (see more below), then the Linux
	     kernel will dump this mapping when the user specified
	     that she only wants anonymous mappings in the corefile
	     (*even* when she explicitly disabled the dumping of
	     file-backed mappings).  */
	  mapping_file_p = !mapping_anon_p;

	  /* Decode permissions.  */
	  read = (memchr (permissions, 'r', permissions_len) != 0);
	  write = (memchr (permissions, 'w', permissions_len) != 0);
	  exec = (memchr (permissions, 'x', permissions_len) != 0);
	  /* 'private' here actually means VM_MAYSHARE, and not
	     VM_SHARED.  In order to know if a mapping is really
	     private or not, we must check the flag "sh" in the
	     VmFlags field.  This is done by decode_vmflags.  However,
	     if we are using a Linux kernel released before the commit
	     834f82e2aa9a8ede94b17b656329f850c1471514 (3.10), we will
	     not have the VmFlags there.  In this case, there is
	     really no way to know if we are dealing with VM_SHARED,
	     so we just assume that VM_MAYSHARE is enough.  */
	  priv = memchr (permissions, 'p', permissions_len) != 0;

	  /* Try to detect if region should be dumped by parsing smaps
	     counters.  */
	  for (line = strtok_r (NULL, "\n", &t);
	       line != NULL && line[0] >= 'A' && line[0] <= 'Z';
	       line = strtok_r (NULL, "\n", &t))
	    {
	      char keyword[64 + 1];

	      if (sscanf (line, "%64s", keyword) != 1)
		{
		  warning (_("Error parsing {s,}maps file '%s'"), mapsfilename);
		  break;
		}

	      if (strcmp (keyword, "Anonymous:") == 0)
		{
		  /* Older Linux kernels did not support the
		     "Anonymous:" counter.  Check it here.  */
		  has_anonymous = 1;
		}
	      else if (strcmp (keyword, "VmFlags:") == 0)
		decode_vmflags (line, &v);

	      if (strcmp (keyword, "AnonHugePages:") == 0
		  || strcmp (keyword, "Anonymous:") == 0)
		{
		  unsigned long number;

		  if (sscanf (line, "%*s%lu", &number) != 1)
		    {
		      warning (_("Error parsing {s,}maps file '%s' number"),
			       mapsfilename);
		      break;
		    }
		  if (number > 0)
		    {
		      /* Even if we are dealing with a file-backed
			 mapping, if it contains anonymous pages we
			 consider it to be *also* an anonymous
			 mapping, because this is what the Linux
			 kernel does:

			 // Dump segments that have been written to.
			 if (vma->anon_vma && FILTER(ANON_PRIVATE))
			 	goto whole;

			 Note that if the mapping is already marked as
			 file-backed (i.e., mapping_file_p is
			 non-zero), then this is a special case, and
			 this mapping will be dumped either when the
			 user wants to dump file-backed *or* anonymous
			 mappings.  */
		      mapping_anon_p = 1;
		    }
		}
	    }

	  if (has_anonymous)
	    should_dump_p = dump_mapping_p (filterflags, &v, priv,
					    mapping_anon_p, mapping_file_p,
					    filename);
	  else
	    {
	      /* Older Linux kernels did not support the "Anonymous:" counter.
		 If it is missing, we can't be sure - dump all the pages.  */
	      should_dump_p = 1;
	    }

	  /* Invoke the callback function to create the corefile segment.  */
	  if (should_dump_p)
	    retval = func (addr, endaddr - addr, offset, inode,
			   read, write, exec,
			   1, /* MODIFIED is true because we want to dump the
				 mapping.  */
			   filename, func_data);
	  if (retval != 0)
	    break;
	}

      do_cleanups (cleanup);
      return retval;
    }

  return -1;
}
