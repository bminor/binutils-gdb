/* GDB_FILE - a generic STDIO like output stream.
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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

#ifndef GDB_FILE_H
#define GDB_FILE_H

struct gdb_file;

/* Create a generic gdb_file object with null methods. */

extern struct gdb_file *gdb_file_new (void);

/* Override methods used by specific implementations of a GDB_FILE
   object. */

typedef void (gdb_file_flush_ftype) (struct gdb_file * stream);
extern void set_gdb_file_flush (struct gdb_file *stream, gdb_file_flush_ftype * flush);

/* NOTE: Both fputs and write methods are available. Default
   implementations that mapping one onto the other are included. */
typedef void (gdb_file_write_ftype) (struct gdb_file * stream, const char *buf, long length_buf);
extern void set_gdb_file_write (struct gdb_file *stream, gdb_file_write_ftype *fputs);

typedef void (gdb_file_fputs_ftype) (const char *, struct gdb_file * stream);
extern void set_gdb_file_fputs (struct gdb_file *stream, gdb_file_fputs_ftype * fputs);

typedef int (gdb_file_isatty_ftype) (struct gdb_file * stream);
extern void set_gdb_file_isatty (struct gdb_file *stream, gdb_file_isatty_ftype * isatty);

typedef void (gdb_file_rewind_ftype) (struct gdb_file * stream);
extern void set_gdb_file_rewind (struct gdb_file *stream, gdb_file_rewind_ftype * rewind);

typedef void (gdb_file_put_method_ftype) (void *object, const char *buffer, long length_buffer);
typedef void (gdb_file_put_ftype) (struct gdb_file *stream, gdb_file_put_method_ftype * method, void *context);
extern void set_gdb_file_put (struct gdb_file *stream, gdb_file_put_ftype * put);

typedef void (gdb_file_delete_ftype) (struct gdb_file * stream);
extern void set_gdb_file_data (struct gdb_file *stream, void *data, gdb_file_delete_ftype * delete);

extern void *gdb_file_data (struct gdb_file *file);


extern void gdb_flush (struct gdb_file *);

extern void gdb_file_delete (struct gdb_file *stream);

extern void gdb_file_rewind (struct gdb_file *stream);

extern int gdb_file_isatty (struct gdb_file *);

extern void gdb_file_write (struct gdb_file *file, const char *buf, long length_buf);

/* NOTE: copies left to right */
extern void gdb_file_put (struct gdb_file *src, gdb_file_put_method_ftype *write, void *dest);

/* Returns a freshly allocated buffer containing the entire contents
   of FILE (as determined by gdb_file_put()) with a NUL character
   appended.  LENGTH is set to the size of the buffer minus that
   appended NUL. */
extern char *gdb_file_xstrdup (struct gdb_file *file, long *length);



/* Create/open a memory based file. Can be used as a scratch buffer
   for collecting output. */
extern struct gdb_file *mem_fileopen (void);



/* Open/create a an STDIO based GDB_FILE using the already open FILE. */
extern struct gdb_file *stdio_fileopen (FILE *file);

/* Open NAME returning an STDIO based GDB_FILE. */
extern GDB_FILE *gdb_fopen (char *name, char *mode);

#endif
