/* OBSOLETE /* Program to stuff files into a specially prepared space in kdb. */
/* OBSOLETE    Copyright 1986, 1989, 1991, 1992, 1995, 2000 */
/* OBSOLETE    Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Written 13-Mar-86 by David Bridgham. */ */
/* OBSOLETE  */
/* OBSOLETE #include <stdio.h> */
/* OBSOLETE #include <a.out.h> */
/* OBSOLETE #include <sys/types.h> */
/* OBSOLETE #include "gdb_stat.h" */
/* OBSOLETE #include <sys/file.h> */
/* OBSOLETE #include <varargs.h> */
/* OBSOLETE  */
/* OBSOLETE main (argc, argv) */
/* OBSOLETE      int argc; */
/* OBSOLETE      char *argv[]; */
/* OBSOLETE { */
/* OBSOLETE   register char *cp; */
/* OBSOLETE   char *outfile; */
/* OBSOLETE   register int i; */
/* OBSOLETE   int offset; */
/* OBSOLETE   int out_fd, in_fd; */
/* OBSOLETE   struct stat stat_buf; */
/* OBSOLETE   int size, pad; */
/* OBSOLETE   char buf[1024]; */
/* OBSOLETE   static char zeros[4] = */
/* OBSOLETE   {0}; */
/* OBSOLETE  */
/* OBSOLETE   if (argc < 4) */
/* OBSOLETE     err ("Not enough arguments\nUsage: %s -o kdb file1 file2 ...\n", */
/* OBSOLETE 	 argv[0]); */
/* OBSOLETE  */
/* OBSOLETE   outfile = 0; */
/* OBSOLETE   for (i = 1; i < argc; i++) */
/* OBSOLETE     { */
/* OBSOLETE       if (STREQ (argv[i], "-o")) */
/* OBSOLETE 	outfile = argv[++i]; */
/* OBSOLETE     } */
/* OBSOLETE   if (outfile == 0) */
/* OBSOLETE     err ("Output file not specified\n"); */
/* OBSOLETE  */
/* OBSOLETE   offset = get_offset (outfile, "_heap"); */
/* OBSOLETE  */
/* OBSOLETE   out_fd = open (outfile, O_WRONLY); */
/* OBSOLETE   if (out_fd < 0) */
/* OBSOLETE     err ("Error opening %s for write: %s\n", outfile, strerror (errno)); */
/* OBSOLETE   if (lseek (out_fd, offset, 0) < 0) */
/* OBSOLETE     err ("Error seeking to heap in %s: %s\n", outfile, strerror (errno)); */
/* OBSOLETE  */
/* OBSOLETE   /* For each file listed on the command line, write it into the */
/* OBSOLETE    * 'heap' of the output file.  Make sure to skip the arguments */
/* OBSOLETE    * that name the output file. */ */
/* OBSOLETE   for (i = 1; i < argc; i++) */
/* OBSOLETE     { */
/* OBSOLETE       if (STREQ (argv[i], "-o")) */
/* OBSOLETE 	continue; */
/* OBSOLETE       if ((in_fd = open (argv[i], O_RDONLY)) < 0) */
/* OBSOLETE 	err ("Error opening %s for read: %s\n", argv[i], */
/* OBSOLETE 	     strerror (errno)); */
/* OBSOLETE       if (fstat (in_fd, &stat_buf) < 0) */
/* OBSOLETE 	err ("Error stat'ing %s: %s\n", argv[i], strerror (errno)); */
/* OBSOLETE       size = strlen (argv[i]); */
/* OBSOLETE       pad = 4 - (size & 3); */
/* OBSOLETE       size += pad + stat_buf.st_size + sizeof (int); */
/* OBSOLETE       write (out_fd, &size, sizeof (int)); */
/* OBSOLETE       write (out_fd, argv[i], strlen (argv[i])); */
/* OBSOLETE       write (out_fd, zeros, pad); */
/* OBSOLETE       while ((size = read (in_fd, buf, sizeof (buf))) > 0) */
/* OBSOLETE 	write (out_fd, buf, size); */
/* OBSOLETE       close (in_fd); */
/* OBSOLETE     } */
/* OBSOLETE   size = 0; */
/* OBSOLETE   write (out_fd, &size, sizeof (int)); */
/* OBSOLETE   close (out_fd); */
/* OBSOLETE   return (0); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Read symbol table from file and returns the offset into the file */
/* OBSOLETE  * where symbol sym_name is located.  If error, print message and */
/* OBSOLETE  * exit. */ */
/* OBSOLETE get_offset (char *file, char *sym_name) */
/* OBSOLETE { */
/* OBSOLETE   int f; */
/* OBSOLETE   struct exec file_hdr; */
/* OBSOLETE   struct nlist *symbol_table; */
/* OBSOLETE   int size; */
/* OBSOLETE   char *strings; */
/* OBSOLETE  */
/* OBSOLETE   f = open (file, O_RDONLY); */
/* OBSOLETE   if (f < 0) */
/* OBSOLETE     err ("Error opening %s: %s\n", file, strerror (errno)); */
/* OBSOLETE   if (read (f, &file_hdr, sizeof (file_hdr)) < 0) */
/* OBSOLETE     err ("Error reading exec structure: %s\n", strerror (errno)); */
/* OBSOLETE   if (N_BADMAG (file_hdr)) */
/* OBSOLETE     err ("File %s not an a.out file\n", file); */
/* OBSOLETE  */
/* OBSOLETE   /* read in symbol table */ */
/* OBSOLETE   if ((symbol_table = (struct nlist *) malloc (file_hdr.a_syms)) == 0) */
/* OBSOLETE     err ("Couldn't allocate space for symbol table\n"); */
/* OBSOLETE   if (lseek (f, N_SYMOFF (file_hdr), 0) == -1) */
/* OBSOLETE     err ("lseek error: %s\n", strerror (errno)); */
/* OBSOLETE   if (read (f, symbol_table, file_hdr.a_syms) == -1) */
/* OBSOLETE     err ("Error reading symbol table from %s: %s\n", file, */
/* OBSOLETE 	 strerror (errno)); */
/* OBSOLETE  */
/* OBSOLETE   /* read in string table */ */
/* OBSOLETE   if (read (f, &size, 4) == -1) */
/* OBSOLETE     err ("reading string table size: %s\n", strerror (errno)); */
/* OBSOLETE   if ((strings = (char *) malloc (size)) == 0) */
/* OBSOLETE     err ("Couldn't allocate memory for string table\n"); */
/* OBSOLETE   if (read (f, strings, size - 4) == -1) */
/* OBSOLETE     err ("reading string table: %s\n", strerror (errno)); */
/* OBSOLETE  */
/* OBSOLETE   /* Find the core address at which the first byte of kdb text segment */
/* OBSOLETE      should be loaded into core when kdb is run.  */ */
/* OBSOLETE   origin = find_symbol ("_etext", symbol_table, file_hdr.a_syms, strings) */
/* OBSOLETE     - file_hdr.a_text; */
/* OBSOLETE   /* Find the core address at which the heap will appear.  */ */
/* OBSOLETE   coreaddr = find_symbol (sym_name, symbol_table, file_hdr.a_syms, strings); */
/* OBSOLETE   /* Return address in file of the heap data space.  */ */
/* OBSOLETE   return (N_TXTOFF (file_hdr) + core_addr - origin); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE find_symbol (char *sym_name, struct nlist *symbol_table, int length, */
/* OBSOLETE 	     char *strings) */
/* OBSOLETE { */
/* OBSOLETE   register struct nlist *sym; */
/* OBSOLETE  */
/* OBSOLETE   /* Find symbol in question */ */
/* OBSOLETE   for (sym = symbol_table; */
/* OBSOLETE        sym != (struct nlist *) ((char *) symbol_table + length); */
/* OBSOLETE        sym++) */
/* OBSOLETE     { */
/* OBSOLETE       if ((sym->n_type & N_TYPE) != N_DATA) */
/* OBSOLETE 	continue; */
/* OBSOLETE       if (sym->n_un.n_strx == 0) */
/* OBSOLETE 	continue; */
/* OBSOLETE       if (STREQ (sym_name, strings + sym->n_un.n_strx - 4)) */
/* OBSOLETE 	return sym->n_value; */
/* OBSOLETE     } */
/* OBSOLETE   err ("Data symbol %s not found in %s\n", sym_name, file); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* VARARGS */ */
/* OBSOLETE void */
/* OBSOLETE err (va_alist) */
/* OBSOLETE      va_dcl */
/* OBSOLETE { */
/* OBSOLETE   va_list args; */
/* OBSOLETE   char *string; */
/* OBSOLETE  */
/* OBSOLETE   va_start (args); */
/* OBSOLETE   string = va_arg (args, char *); */
/* OBSOLETE   vfprintf (gdb_stderr, string, args); */
/* OBSOLETE   va_end (args); */
/* OBSOLETE   exit (-1); */
/* OBSOLETE } */
