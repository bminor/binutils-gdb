/* Machine independent variables that describe the core file under GDB.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

/* File names of core file and executable file.  */

extern char *corefile;
extern char *execfile;

/* Descriptors on which core file and executable file are open.
   Note that the execchan is closed when an inferior is created
   and reopened if the inferior dies or is killed.  */

extern int corechan;
extern int execchan;

/* Last modification time of executable file.
   Also used in source.c to compare against mtime of a source file.  */

extern int exec_mtime;

/* Virtual addresses of bounds of the two areas of memory in the core file.  */

extern CORE_ADDR data_start;
extern CORE_ADDR data_end;
extern CORE_ADDR stack_start;
extern CORE_ADDR stack_end;

/* Virtual addresses of bounds of two areas of memory in the exec file.
   Note that the data area in the exec file is used only when there is no core file.  */

extern CORE_ADDR text_start;
extern CORE_ADDR text_end;

extern CORE_ADDR exec_data_start;
extern CORE_ADDR exec_data_end;

/* Address in executable file of start of text area data.  */

extern int text_offset;

/* Address in executable file of start of data area data.  */

extern int exec_data_offset;

/* Address in core file of start of data area data.  */

extern int data_offset;

/* Address in core file of start of stack area data.  */

extern int stack_offset;
