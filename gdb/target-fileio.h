/* Target File-I/O communications

   Copyright (C) 2003, 2007, 2008 Free Software Foundation, Inc.

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


#ifndef TARGET_FILEIO_H
#define TARGET_FILEIO_H

struct cmd_list_element;

struct file_io_operations {
    int (*read_bytes) (CORE_ADDR memaddr, gdb_byte *myaddr, int len);
    int (*write_bytes)(CORE_ADDR memaddr, gdb_byte *myaddr, int len);
    void (*reply)(int retcode, int error);
    void (*set_ctrl_c_signal_handler)(void);
};


#define ioerror()                 reply(-1,      FILEIO_EIO)
#define badfd()                   reply(-1,      FILEIO_EBADF)
#define return_errno(retcode)     reply(retcode, ((retcode) < 0) ? target_fileio_errno_to_target (errno) : 0)
#define return_success(retcode)   reply(retcode, 0)


/* Unified interface to target fileio */
extern void target_fileio_request (char *buf, struct file_io_operations *operations);

/* Cleanup any target fileio state.  */
extern void target_fileio_reset (void);

extern void initialize_target_fileio (
  struct cmd_list_element *set_cmdlist,
  struct cmd_list_element *show_cmdlist);


extern int target_fio_no_longjmp;

#endif
