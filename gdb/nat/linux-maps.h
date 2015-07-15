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

#ifndef NAT_LINUX_MAPS_H
#define NAT_LINUX_MAPS_H

extern void
  read_mapping (const char *line,
		ULONGEST *addr, ULONGEST *endaddr,
		const char **permissions, size_t *permissions_len,
		ULONGEST *offset,
		const char **device, size_t *device_len,
		ULONGEST *inode,
		const char **filename);

/* Callback function for linux_find_memory_regions_full.  If it returns
   non-zero linux_find_memory_regions_full returns immediately with that
   value.  */

typedef int linux_find_memory_region_ftype (ULONGEST vaddr, ULONGEST size,
					    ULONGEST offset, ULONGEST inode,
					    int read, int write,
					    int exec, int modified,
					    const char *filename,
					    void *data);

/* This enum represents the values that the user can choose when
   informing the Linux kernel about which memory mappings will be
   dumped in a corefile.  They are described in the file
   Documentation/filesystems/proc.txt, inside the Linux kernel
   tree.  */

enum filterflags
  {
    COREFILTER_ANON_PRIVATE = 1 << 0,
    COREFILTER_ANON_SHARED = 1 << 1,
    COREFILTER_MAPPED_PRIVATE = 1 << 2,
    COREFILTER_MAPPED_SHARED = 1 << 3,
    COREFILTER_ELF_HEADERS = 1 << 4,
    COREFILTER_HUGETLB_PRIVATE = 1 << 5,
    COREFILTER_HUGETLB_SHARED = 1 << 6,
  };

extern int
  linux_find_memory_regions_full (pid_t pid, enum filterflags filterflags,
				  linux_find_memory_region_ftype *func,
				  void *func_data);

#endif /* NAT_LINUX_MAPS_H */
