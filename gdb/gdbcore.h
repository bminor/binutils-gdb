/* Machine independent variables that describe the core file under GDB.
   Copyright (C) 1986, 1987, 1989, 1990  Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Interface routines for core, executable, etc.  */

#include "bfd.h"		/* Binary File Description */

/* Return the name of the executable file as a string.
   ERR nonzero means get error if there is none specified;
   otherwise return 0 in that case.  */
char *get_exec_file ();

/* Nonzero if there is a core file.  */
int have_core_file_p ();

/* Read "memory data" from whatever target or inferior we have. 
   Returns zero if successful, errno value if not.  EIO is used
   for address out of bounds.  If breakpoints are inserted, returns
   shadow contents, not the breakpoints themselves.  From breakpoint.c.  */
int read_memory_nobpt ();

/* Report a memory error with error().  */

void memory_error ();

/* Like target_read_memory, but report an error if can't read.  */
void read_memory ();

/* Read an integer from debugged memory, given address and number of bytes.  */
long read_memory_integer ();

void write_memory (
#ifdef __STDC__
		   CORE_ADDR, char *, int
#endif
		   );

/* Hook for `exec_file_command' command to call.  */

extern void (*exec_file_display_hook) ();
   
/* Binary File Diddlers for the exec and core files */
extern bfd *core_bfd;
extern bfd *exec_bfd;

void core_file_command ();
void exec_file_command ();
void validate_files ();
unsigned int register_addr ();
int xfer_core_file ();
void fetch_core_registers ();
void registers_fetched ();

#if !defined (KERNEL_U_ADDR)
extern CORE_ADDR kernel_u_addr;
#define KERNEL_U_ADDR kernel_u_addr
#endif

/* Struct section_table maps address ranges to file sections.  It is
   mostly used with BFD files, but can be used without (e.g. for handling
   raw disks, or files not in formats handled by BFD).  */

struct section_table {
  CORE_ADDR addr;		/* Lowest address in section */
  CORE_ADDR endaddr;		/* 1+highest address in section */
  sec_ptr   sec_ptr;		/* BFD section pointer */
};

/* Builds a section table, given args BFD, SECTABLE_PTR, SECEND_PTR.
   Returns 0 if OK, 1 on error.  */

int build_section_table ();	
