/* ar.c - Archive modify and extract.
   Copyright 1991, 1992, 1994 Free Software Foundation, Inc.

This file is part of GNU Binutils.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
   Bugs: should use getopt the way tar does (complete w/optional -) and
   should have long options too. GNU ar used to check file against filesystem
   in quick_update and replace operations (would check mtime). Doesn't warn
   when name truncated. No way to specify pos_end. Error messages should be
   more consistant.
*/
#include "bfd.h"
#include "sysdep.h"
#include "bucomm.h"
#include "aout/ar.h"
#include "libbfd.h"
#include "arsup.h"
#include <stdio.h>
#ifdef POSIX_UTIME
#include <utime.h>
#else /* ! POSIX_UTIME */
#ifdef	USE_UTIME
#include <time.h>
#else /* ! USE_UTIME */
#include <sys/time.h>
#endif /* ! USE_UTIME */
#endif /* ! POSIX_UTIME */
#include <errno.h>
#ifndef errno
extern int errno;
#endif
#define BUFSIZE 8192

#ifdef __GO32___
#define EXT_NAME_LEN 3 /* bufflen of addition to name if it's MS-DOS */
#else
#define EXT_NAME_LEN 6 /* ditto for *NIX */
#endif

/* Kludge declaration from BFD!  This is ugly!  FIXME!  XXX */

struct ar_hdr *
bfd_special_undocumented_glue PARAMS ((bfd *abfd, char *filename));

/* Forward declarations */

static void
print_contents PARAMS ((bfd * member));

static void
delete_members PARAMS ((char **files_to_delete));

static void
do_quick_append PARAMS ((char *archive_filename, char **files_to_append));

static void
move_members PARAMS ((char **files_to_move));

static void
replace_members PARAMS ((char **files_to_replace));

static void
print_descr PARAMS ((bfd * abfd));

static void
ranlib_only PARAMS ((char *archname));

/** Globals and flags */

bfd            *inarch;		/* The input arch we're manipulating */

int mri_mode;
/* This flag distinguishes between ar and ranlib:
   1 means this is 'ranlib'; 0 means this is 'ar'.
   -1 means if we should use argv[0] to decide. */
extern int is_ranlib;
/* Nonzero means don't warn about creating the archive file if necessary.  */
int             silent_create = 0;
/* Nonzero means describe each action performed.  */
int             verbose = 0;
/* Nonzero means preserve dates of members when extracting them.  */
int             preserve_dates = 0;
/*
   Nonzero means don't replace existing members whose dates are more recent
   than the corresponding files.
*/
int             newer_only = 0;

/* Controls the writing of an archive symbol table (in BSD: a __.SYMDEF
   member).  -1 means we've been explicitly asked to not write a symbol table;
   +1 means we've been explictly asked to write it;
   0 is the default.
   Traditionally, the default in BSD has been to not write the table.
   However, for Posix.2 compliance the default is now to write a symbol table
   if any of the members are object files. */
int write_armap = 0;

/*
   Nonzero means it's the name of an existing member; position new or moved
   files with respect to this one.
*/
char           *posname = NULL;
/*
   Sez how to use `posname': pos_before means position before that member.
   pos_after means position after that member. pos_end means always at end.
   pos_default means default appropriately. For the latter two, `posname'
   should also be zero.
*/
enum pos {
    pos_default, pos_before, pos_after, pos_end
}               postype = pos_default;

int interactive = 0;
void
DEFUN_VOID(mri_emul)
{
  interactive = isatty(fileno(stdin)) ;
  yyparse();
}

/*
   If count is 0, then function is called once on each entry. if nonzero,
   count is the length of the files chain; function is called on each entry
   whose name matches one in files
*/
void
DEFUN(map_over_members,(function, files, count),
      void            (*function) () AND
      char          **files AND
      int             count)
{
  bfd            *head;

  if (count == 0) {
    for (head = inarch->next; head; head = head->next)
     function(head);
    return;
  }
  /*
    This may appear to be a baroque way of accomplishing what we want.
    however we have to iterate over the filenames in order to notice where
    a filename is requested but does not exist in the archive.  Ditto
    mapping over each file each time -- we want to hack multiple
    references.
    */

  for (; count > 0; files++, count--) {
    boolean         found = false;
    for (head = inarch->next; head; head = head->next)
    {
      if (head->filename == NULL)
      {
	/* Some archive formats don't get the filenames filled in
	   'till the elements are opened */
	struct stat buf;
	bfd_stat_arch_elt(head, &buf);
      }
      if ((head->filename != NULL) &&
	  (!strcmp(*files, head->filename))) {
	found = true;
	function(head);
      }
    }
    if (!found)
     fprintf(stderr, "no entry %s in archive\n", *files);
  }
}


boolean operation_alters_arch = false;

extern char *program_version;

void
do_show_version ()
{
  printf ("GNU %s version %s\n", program_name, program_version);
  exit (0);
}

void
usage ()
{
  if (is_ranlib == 0)
    fprintf(stderr, "\
Usage: %s [-]{dmpqrtx}[abcilosuvV] [member-name] archive-file file...\n\
       %s -M [<mri-script]\n",
	    program_name, program_name);
  else
    fprintf(stderr, "\
Usage: %s [-vV] archive\n", program_name);
  exit(1);
}

/*
   The option parsing should be in its own function.  It will be when I have
   getopt working.
*/
int
main(argc, argv)
    int             argc;
    char          **argv;
{
  char           *arg_ptr;
  char            c;
  enum {
    none = 0, delete, replace, print_table,
    print_files, extract, move, quick_append
  }               operation = none;
  int             arg_index;
  char          **files;
  char           *inarch_filename;
  char           *temp;
  int		 show_version;

  bfd_init();
  show_version = 0;

  program_name = argv[0];

  temp = strrchr(program_name, '/');
  if (temp == (char *) NULL)
   temp = program_name;		/* shouldn't happen, but... */
  else
   ++temp;
  if (is_ranlib > 0 || (is_ranlib < 0 && strcmp(temp, "ranlib") == 0)) {
    is_ranlib = 1;
    if (argc < 2 || argc > 3)
      usage ();
    arg_ptr = argv[1];
    if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "-v") == 0)
      do_show_version();
    ranlib_only(arg_ptr);
  }
  else
    is_ranlib = 0;

  if (argc == 2 && strcmp(argv[1],"-M") == 0) {
    mri_emul();
    exit(0);
  }

  if (argc < 2)
    usage ();

  arg_ptr = argv[1];

  if (*arg_ptr == '-')
   ++arg_ptr;			/* compatibility */

  while ((c = *arg_ptr++) != '\0') {
    switch (c) {
     case 'd':
     case 'm':
     case 'p':
     case 'q':
     case 'r':
     case 't':
     case 'x':
      if (operation != none)
       fatal("two different operation options specified");
      switch (c) {
       case 'd':
	operation = delete;
	operation_alters_arch = true;
	break;
       case 'm':
	operation = move;
	operation_alters_arch = true;
	break;
       case 'p':
	operation = print_files;
	break;
       case 'q':
	operation = quick_append;
	operation_alters_arch = true;
	break;
       case 'r':
	operation = replace;
	operation_alters_arch = true;
	break;
       case 't':
	operation = print_table;
	break;
       case 'x':
	operation = extract;
	break;
      }
     case 'l':
      break;
     case 'c':
      silent_create = 1;
      break;
     case 'o':
      preserve_dates = 1;
      break;
     case 'V':
      show_version = true;
      break;
     case 's':
      write_armap = 1;
      break;
     case 'u':
      newer_only = 1;
      break;
     case 'v':
      verbose = 1;
      break;
     case 'a':
      postype = pos_after;
      break;
     case 'b':
      postype = pos_before;
      break;
     case 'i':
      postype = pos_before;
      break;
     case 'M':
      mri_mode = 1;
      break;
     default:
      fprintf(stderr, "%s: illegal option -- %c\n", program_name, c);
      usage ();
    }
  }

  if (show_version)
    do_show_version();

  if (argc < 3)
    usage ();

  if (mri_mode) {
    mri_emul();
  }
  else {
    if ((operation == none || operation == print_table) 
	&& write_armap == 1)
      ranlib_only(argv[2]);

    if (operation == none)
      fatal("no operation specified");

    if (newer_only && operation != replace)
      fatal("`u' is only meaningful with the `r' option.");

    arg_index = 2;

    if (postype != pos_default)
      posname = argv[arg_index++];

    inarch_filename = argv[arg_index++];

    files = arg_index < argc ? argv + arg_index : NULL;

    if (operation == quick_append)
      {
	/* Note that quick appending to a non-existent archive creates it,
	   even if there are no files to append. */
	do_quick_append(inarch_filename, files);
	exit(0);
      }

    open_inarch(inarch_filename);

    switch (operation) {

     case print_table:
      map_over_members(print_descr, files, argc - 3);
      break;

     case print_files:
      map_over_members(print_contents, files, argc - 3);
      break;

     case extract:
      map_over_members(extract_file, files, argc - 3);
      break;

     case delete:
      if (files != NULL)
       delete_members(files);
      break;

     case move:
      if (files != NULL)
       move_members(files);
      break;

     case replace:
      if (files != NULL || write_armap > 0)
       replace_members(files);
      break;

      /* Shouldn't happen! */
     default:
      fprintf(stderr, "%s: internal error -- this option not implemented\n",
	      program_name);
      exit (1);
    }
  }
  return (0);
}				/* main() */

static
char *normalize(file)
char *file;
{
    char *    filename = strrchr(file, '/');
    if (filename != (char *)NULL) {
	filename ++;
    }
    else {
	filename = file;
    }
    return filename;
}

int 
open_inarch(archive_filename)
    char           *archive_filename;
{
    bfd           **last_one;
    bfd            *next_one;
    struct stat     sbuf;
    bfd_error = no_error;

    if (stat(archive_filename, &sbuf) != 0) {

#ifndef __GO32__

/* KLUDGE ALERT! Temporary fix until I figger why
 * stat() is wrong ... think it's buried in GO32's IDT
 * - Jax
 */
	if (errno != ENOENT)
	    bfd_fatal(archive_filename);
#endif

	if (!operation_alters_arch) {
	  fprintf (stderr, "%s: ", program_name);
	  perror (archive_filename);
	  maybequit();
	  return 0;
	}	

	/* This routine is one way to forcibly create the archive. */

	do_quick_append(archive_filename, 0);
    }

    inarch = bfd_openr(archive_filename, NULL);
    if (inarch == NULL) {

      bloser:

        fprintf (stderr, "%s: ", program_name);
	bfd_perror(archive_filename);
	exit(1);
    }

    if (bfd_check_format(inarch, bfd_archive) != true)
	fatal("%s is not an archive", archive_filename);
    last_one = &(inarch->next);
    /* Read all the contents right away, regardless. */
    for (next_one = bfd_openr_next_archived_file(inarch, NULL);
	 next_one;
	 next_one = bfd_openr_next_archived_file(inarch, next_one)) {
	*last_one = next_one;
	last_one = &next_one->next;
    }
    *last_one = (bfd *) NULL;
    if (bfd_error != no_more_archived_files)
	goto bloser;
    return 1;
}





static void
print_contents(abfd)
    bfd            *abfd;
{
    int             ncopied = 0;
    struct stat     buf;
    long            size;
    if (bfd_stat_arch_elt(abfd, &buf) != 0)
	fatal("internal stat error on %s", bfd_get_filename (abfd));

    if (verbose)
	printf("\n<member %s>\n\n", bfd_get_filename (abfd));

    bfd_seek(abfd, 0, SEEK_SET);

    size = buf.st_size;
    while (ncopied < size) {
	char            cbuf[BUFSIZE];
	int             nread;
	int             tocopy = size - ncopied;
	if (tocopy > BUFSIZE)
	    tocopy = BUFSIZE;

	nread = bfd_read(cbuf, 1, tocopy, abfd);	/* oops -- broke
							   abstraction!  */

	if (nread != tocopy)
	    fatal("%s is not a valid archive", abfd->my_archive->filename);
	fwrite(cbuf, 1, nread, stdout);
	ncopied += tocopy;
    }
}


/*
   Extract a member of the archive into its own file.

We defer opening the new file until after we have read a BUFSIZ chunk of the
   old one, since we know we have just read the archive header for the old
   one.  Since most members are shorter than BUFSIZ, this means we will read
   the old header, read the old data, write a new inode for the new file, and
   write the new data, and be done. This 'optimization' is what comes from
   sitting next to a bare disk and hearing it every time it seeks.  -- Gnu
   Gilmore
*/

void
extract_file(abfd)
    bfd            *abfd;
{
    FILE           *ostream;
    char            cbuf[BUFSIZE];
    int             nread,
                    tocopy;
    int             ncopied = 0;
    long            size;
    struct stat     buf;
    if (bfd_stat_arch_elt(abfd, &buf) != 0)
	fatal("internal stat error on %s", bfd_get_filename (abfd));
    size = buf.st_size;

    if (verbose)
	printf("x - %s\n", bfd_get_filename (abfd));

    bfd_seek(abfd, 0, SEEK_SET);

    ostream = 0;
    if (size == 0) {
      /* Seems like an abstraction violation, eh?  Well it's OK! */
      ostream = fopen(bfd_get_filename (abfd), FOPEN_WB);
      if (!ostream) {
	perror(bfd_get_filename (abfd));
	exit(1);
      }
    } else
    while (ncopied < size) {
	tocopy = size - ncopied;
	if (tocopy > BUFSIZE)
	    tocopy = BUFSIZE;

	nread = bfd_read(cbuf, 1, tocopy, abfd);
	if (nread != tocopy)
	    fatal("%s is not a valid archive", abfd->my_archive->filename);

	/* See comment above; this saves disk arm motion */
	if (!ostream) {
	    /* Seems like an abstraction violation, eh?  Well it's OK! */
	    ostream = fopen(bfd_get_filename (abfd), FOPEN_WB);
	    if (!ostream) {
		perror(bfd_get_filename (abfd));
		exit(1);
	    }
	}
	fwrite(cbuf, 1, nread, ostream);
	ncopied += tocopy;
    }

    fclose(ostream);
    chmod(bfd_get_filename (abfd), buf.st_mode);

    if (preserve_dates) {
#ifdef POSIX_UTIME
        struct utimbuf	tb;
	tb.actime = buf.st_mtime;
	tb.modtime = buf.st_mtime;
	utime(bfd_get_filename (abfd), &tb);	/* FIXME check result */
#else /* ! POSIX_UTIME */
#ifdef USE_UTIME
	long            tb[2];
	tb[0] = buf.st_mtime;
	tb[1] = buf.st_mtime;
	utime(bfd_get_filename (abfd), tb);	/* FIXME check result */
#else /* ! USE_UTIME */
	struct timeval  tv[2];
	tv[0].tv_sec = buf.st_mtime;
	tv[0].tv_usec = 0;
	tv[1].tv_sec = buf.st_mtime;
	tv[1].tv_usec = 0;
	utimes(bfd_get_filename (abfd), tv);	/* FIXME check result */
#endif /* ! USE_UTIME */
#endif /* ! POSIX_UTIME */
    }
}


/* Just do it quickly; don't worry about dups, armap, or anything like that */

static void
do_quick_append(archive_filename, files_to_append)
    char           *archive_filename;
    char          **files_to_append;

{
    FILE           *ofile,
                   *ifile;
    char            buf[BUFSIZE];
    long            tocopy,
                    thistime;
    bfd            *temp;
    struct stat     sbuf;
    boolean         newfile = false;
    bfd_error = no_error;

    if (stat(archive_filename, &sbuf) != 0) {

#ifndef __GO32__

/* KLUDGE ALERT! Temporary fix until I figger why
 * stat() is wrong ... think it's buried in GO32's IDT
 * - Jax
 */

	if (errno != ENOENT)
	    bfd_fatal(archive_filename);
#endif

	newfile = true;
    }


    ofile = fopen(archive_filename, FOPEN_AUB);
    if (ofile == NULL) {
	perror(program_name);
	exit(1);
    }

    temp = bfd_openr(archive_filename, NULL);
    if (temp == NULL) {
        fprintf (stderr, "%s: ", program_name);
	bfd_perror(archive_filename);
	exit(1);
    }
    if (newfile == false) {
	if (bfd_check_format(temp, bfd_archive) != true)
	    fatal("%s is not an archive", archive_filename);
    }
    else {
	fwrite(ARMAG, 1, SARMAG, ofile);
	if (!silent_create)
	    fprintf(stderr, "%s: creating %s\n",
		    program_name, archive_filename);
    }

    /* assume it's an achive, go straight to the end, sans $200 */
    fseek(ofile, 0, 2);

    for (; files_to_append && *files_to_append; ++files_to_append) {
	struct ar_hdr  *hdr = bfd_special_undocumented_glue(temp, *files_to_append);
	if (hdr == NULL) {
	    fprintf (stderr, "%s: ", program_name);
	    bfd_perror(*files_to_append);
	    exit(1);
	}

	BFD_SEND(temp, _bfd_truncate_arname, (temp, *files_to_append, (char *) hdr));

	ifile = fopen(*files_to_append, FOPEN_RB);
	if (ifile == NULL)
	  {
	    bfd_perror(program_name);
	  }

	if (stat(*files_to_append, &sbuf) != 0)
	  {
	    fprintf (stderr, "%s: ", program_name);
	    bfd_perror(*files_to_append);
	  }

	tocopy = sbuf.st_size;

	/* XXX should do error-checking! */
	fwrite(hdr, 1, sizeof(struct ar_hdr), ofile);


	while (tocopy > 0) {
	    thistime = tocopy;
	    if (thistime > BUFSIZE)
		thistime = BUFSIZE;
	    fread(buf, 1, thistime, ifile);
	    fwrite(buf, 1, thistime, ofile);
	    tocopy -= thistime;
	}
	fclose(ifile);
	if ((sbuf.st_size % 2) == 1)
	    putc('\n', ofile);
    }
    fclose(ofile);
    bfd_close(temp);
}


void
write_archive()
{
    bfd            *obfd;
    int             namelen = strlen(inarch->filename);
    char           *new_name = xmalloc(namelen + EXT_NAME_LEN);
    bfd            *contents_head = inarch->next;

	strcpy(new_name, inarch->filename);

#ifdef __GO32__	/* avoid long .extensions for MS-DOS */
	strcpy(new_name + namelen, "-a");
#else
	strcpy(new_name + namelen, "-art");
#endif

	obfd = bfd_openw(new_name,
		 /* FIXME: violates abstraction; need a better protocol */
			 (inarch->xvec ? bfd_get_target(inarch) : NULL));

	if (obfd == NULL)
	    bfd_fatal(inarch->filename);

	bfd_set_format(obfd, bfd_archive);

	/* Request writing the archive symbol table unless we've
	   been explicitly requested not to. */
	obfd->has_armap = write_armap >= 0;

	if (bfd_set_archive_head(obfd, contents_head) != true)
	    bfd_fatal(inarch->filename);

	if (!bfd_close(obfd))
	    bfd_fatal(inarch->filename);

	/* We don't care if this fails, we might be creating the
	   archive */
	(void) unlink(inarch->filename);

	if (rename(new_name, inarch->filename) != 0)
	    bfd_fatal(inarch->filename);
}



/*
   returns a pointer to the pointer to the entry which should be rplacd'd
   into when altering.  default_pos should be how to interpret pos_default,
   and should be a pos value.
*/

bfd **
get_pos_bfd(contents, default_pos)
    bfd           **contents;
    enum pos        default_pos;
{
    bfd           **after_bfd = contents;
    enum pos        realpos = (postype == pos_default ? default_pos : postype);

    if (realpos == pos_end) {
	while (*after_bfd)
	    after_bfd = &((*after_bfd)->next);
    }
    else {
	for ( ; *after_bfd; after_bfd = &(*after_bfd)->next)
	    if (!strcmp((*after_bfd)->filename, posname)) {
		if (realpos == pos_after)
		    after_bfd = &(*after_bfd)->next;
		break;
	}
    }
    return after_bfd;
}


static void
delete_members(files_to_delete)
    char          **files_to_delete;
{
    bfd           **current_ptr_ptr;
    boolean         found;
    boolean         something_changed = false;
    for (; *files_to_delete != NULL; ++files_to_delete) {
	/*
	   In a.out systems, the armap is optional.  It's also called
	   __.SYMDEF.  So if the user asked to delete it, we should remember
	   that fact. This isn't quite right for COFF systems (where
	   __.SYMDEF might be regular member), but it's very unlikely
	   to be a problem.  FIXME */

	if (!strcmp(*files_to_delete, "__.SYMDEF")) {
	    inarch->has_armap = false;
	    write_armap = -1;
	    continue;
	}

	found = false;
	current_ptr_ptr = &(inarch->next);
	while (*current_ptr_ptr) {
	    if (strcmp(*files_to_delete, (*current_ptr_ptr)->filename) == 0) {
		found = true;
		something_changed = true;
		if (verbose)
		    printf("d - %s\n",
			   *files_to_delete);
		*current_ptr_ptr = ((*current_ptr_ptr)->next);
		goto next_file;

	    }
	    else {
		current_ptr_ptr = &((*current_ptr_ptr)->next);
	    }
	}

	if (verbose && found == false) {
	    printf("No member named `%s'\n", *files_to_delete);
	}
next_file:;

    }

    if (something_changed == true) {
	write_archive();
    }
}


/* Reposition existing members within an archive */

static void
move_members(files_to_move)
    char          **files_to_move;
{
    bfd           **after_bfd;	/* New entries go after this one */
    bfd           **current_ptr_ptr;	/* cdr pointer into contents */




    for (; *files_to_move; ++files_to_move) {
	current_ptr_ptr = &(inarch->next);
	while (*current_ptr_ptr) {
	    bfd            *current_ptr = *current_ptr_ptr;
	    if (strcmp(normalize(*files_to_move), current_ptr->filename) == 0) {
		/*
		   Move this file to the end of the list - first cut from
		   where it is.
		*/
		*current_ptr_ptr = current_ptr->next;

		/* Now glue to end */
		after_bfd = get_pos_bfd(&inarch->next, pos_end);
		*after_bfd = current_ptr;
		current_ptr->next = (bfd *) NULL;

		if (verbose)
		    printf("m - %s\n", *files_to_move);

		goto next_file;
	    }
	    current_ptr_ptr = &((*current_ptr_ptr)->next);
	}
	fprintf(stderr, "%s: no entry %s in archive %s!\n",
		program_name, *files_to_move, inarch->filename);
	exit(1);
next_file:;
    }

    write_archive();
}


/* Ought to default to replacing in place, but this is existing practice! */

static void
replace_members(files_to_move)
    char          **files_to_move;
{
    bfd           **after_bfd;	/* New entries go after this one */
    bfd            *current;
    bfd           **current_ptr;
    bfd            *temp;

    while (files_to_move && *files_to_move) {
	current_ptr = &inarch->next;
	while (*current_ptr) {
	    current = *current_ptr;
	    
	    if (!strcmp(normalize(*files_to_move), current->filename)) {
		if (newer_only) {
		    struct stat     fsbuf,
		                    asbuf;

		    if (current->arelt_data == NULL) {
		      /* This can only happen if you specify a file on the
			 command line more than once. */
		      fprintf (stderr, "%s: duplicate file specified: %s -- skipping\n", program_name, *files_to_move);
		      goto next_file;
		    }

		    if (stat(*files_to_move, &fsbuf) != 0) {
			if (errno != ENOENT)
			    bfd_fatal(*files_to_move);
			goto next_file;
		    }
		    if (bfd_stat_arch_elt(current, &asbuf) != 0)
			fatal("internal stat error on %s", current->filename);

		    if (fsbuf.st_mtime <= asbuf.st_mtime)
			goto next_file;
		}

		/* snip out this entry from the chain */
		*current_ptr = current->next;

		after_bfd = get_pos_bfd(&inarch->next, pos_end);
		temp = *after_bfd;
		*after_bfd = bfd_openr(*files_to_move, NULL);
		if (*after_bfd == (bfd *) NULL) {
		    fprintf(stderr, "%s: ", program_name);
		    bfd_perror (*files_to_move);
		    exit(1);
		}
		(*after_bfd)->next = temp;

		if (verbose) {
		    printf("%c - %s\n", (postype == pos_after ? 'r' : 'a'),
			   *files_to_move);
		}
		goto next_file;
	    }
	    current_ptr = &(current->next);
	}

	/* It isn't in there, so add to end */

	after_bfd = get_pos_bfd(&inarch->next, pos_end);
	temp = *after_bfd;
	*after_bfd = bfd_openr(*files_to_move, NULL);
	if (*after_bfd == (bfd *) NULL) {
	  fprintf(stderr, "%s: ", program_name);
	  bfd_perror (*files_to_move);
	  exit(1);
	}
	if (verbose) {
	    printf("c - %s\n", *files_to_move);
	}

	(*after_bfd)->next = temp;

next_file:;

	files_to_move++;
    }


    write_archive();
}

static void
ranlib_only(archname)
    char           *archname;
{
    write_armap = 1;
    open_inarch(archname);
    write_archive();
    exit(0);
}



/* Things which are interesting to map over all or some of the files: */

static void
print_descr(abfd)
    bfd            *abfd;
{
    print_arelt_descr(stdout,abfd, verbose);
}
