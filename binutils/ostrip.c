/* strip certain symbols from a rel file.
   Copyright (C) 1986, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "getopt.h"

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

enum strip_action {
  strip_undef,
  strip_all,			/* strip all symbols */
  strip_debug,			/* strip all debugger symbols */
};

/* Which symbols to remove. */
enum strip_action strip_symbols;

enum locals_action {
  locals_undef,
  locals_start_L,		/* discard locals starting with L */
  locals_all,			/* discard all locals */
};

/* Which local symbols to remove. */
enum locals_action discard_locals;

/* The name this program was run with. */
char *program_name;

struct option long_options[] = {
  {"strip-all", 0, 0, 's'},
  {"strip-debug", 0, 0, 'S'},
  {"discard-all", 0, 0, 'x'},
  {"discard-locals", 0, 0, 'X'},
  {0, 0, 0, 0},
};

static char *target = NULL;

static int fatal_error;

extern char *malloc();
extern char *mktemp();
extern char *realloc();
extern char *strcpy();
extern int exit();
extern int fprintf();
extern int free();
extern int getpid();
extern int kill();
extern int perror();
extern int sprintf();
extern int unlink();

#ifdef __STDC__
static int strip_bfd(bfd *ibfd, bfd *obfd);
static int strip_file(char *filetostrip);
static void usage(void);
#else
static int strip_bfd();
static int strip_file();
static void usage();
#endif /* __STDC__ */
static void copy_sections ();
static void setup_sections ();

int main(argc, argv)
char **argv;
int argc;
{
	int ind;
	int c;
	program_name = argv[0];
	
	strip_symbols = strip_undef;	/* default is to strip everything.  */
	discard_locals = locals_undef;
	
	while ((c = getopt_long (argc, argv, "gsST:xX", long_options, &ind)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 's':
			strip_symbols = strip_all;
			break;
		case 'g':
		case 'S':
			strip_symbols = strip_debug;
			break;
		case 'T':
			target = optarg;
			break;
		case 'x':
			discard_locals = locals_all;
			break;
		case 'X':
			discard_locals = locals_start_L;
			break;
		default:
			usage ();
		} /* switch on option */
	} /* for each option */
	
	if (strip_symbols == strip_undef && discard_locals == locals_undef) {
		strip_symbols = strip_all;
	} /* Default is to strip all symbols.  */
	
	
	if (argc == optind) {
		return(strip_file("a.out"));
	} else {
		int retval = 0;

		for ( ; optind < argc; ++optind) {
			retval &= strip_file(argv[optind]);
		} /* for each file to strip */

		return(retval);
	} /* if no arguments given */

} /* main() */

static int delayed_signal;

void delay_signal(signo)
int signo;
{
	delayed_signal = signo;
	signal(signo, delay_signal);
} /* delay_signal() */

static int sigint_handled = 0;
static int sighup_handled = 0;
static int sigterm_handled = 0;

void handle_sigs() {
	/* Effectively defer handling of asynchronous kill signals.  */
	delayed_signal = 0;
	
	if (signal (SIGINT, SIG_IGN) != SIG_IGN) {
		sigint_handled = 1;
		signal(SIGINT, delay_signal);
	} /* if not ignored */
	
	if (signal (SIGHUP, SIG_IGN) != SIG_IGN) {
		sighup_handled = 1;
		signal(SIGHUP, delay_signal);
	} /* if not ignored */
	
	if (signal (SIGTERM, SIG_IGN) != SIG_IGN) {
		sigterm_handled = 1;
		signal(SIGTERM, delay_signal);
	} /* if not ignored */
	
	return;
} /* handle_sigs() */

void unhandle_sigs() {
	/* Effectively undefer handling.  */
	if (sigint_handled)
	    signal (SIGINT, SIG_DFL);
	if (sighup_handled)
	    signal (SIGHUP, SIG_DFL);
	if (sigterm_handled)
	    signal (SIGTERM, SIG_DFL);
	
	/* Handle any signal that came in while they were deferred.  */
	if (delayed_signal)
	    kill (getpid (), delayed_signal);
	
	return;
} /* unhandle_sigs() */

static int strip_file(filetostrip)
char *filetostrip;
{
	bfd *ibfd;
	bfd *obfd;
	char tmpfilename[] = "stXXXXXX";

	if ((ibfd = bfd_openr(filetostrip, (char *)NULL)) == NULL) {
		bfd_perror(filetostrip);
		return(1);
	} /* on error opening input */
	
	obfd = bfd_openw(mktemp(tmpfilename),
			 target? target: bfd_get_target (ibfd));
	if (obfd == NULL) {
		bfd_perror(tmpfilename);

		if (bfd_close(ibfd) == false) {
			bfd_perror(bfd_get_filename(ibfd));
		} /* on close error */

		return(1);
	} /* on error opening output */

	handle_sigs();

	if (bfd_check_format(ibfd, bfd_object) != false) {
		if (bfd_set_format(obfd, bfd_get_format(ibfd)) != false) {
			if (!strip_bfd(ibfd, obfd)) {
				/* success */

				if (bfd_close(ibfd) == false) {
					bfd_perror(bfd_get_filename(ibfd));
				} /* on close error */

				if (bfd_close(obfd) == false) {
					bfd_perror(bfd_get_filename(obfd));
				} /* on close error */

				rename(tmpfilename, filetostrip);
				unhandle_sigs();
				return(0);
			} /* strip_bfd prints it's own failing messages */
		} else {
			bfd_perror(filetostrip);
		} /* can't set format */
	} else {
		/* not an object file */
		(void) fprintf(stderr, "File %s has format 0x%x that will not be stripped.\n",
			       filetostrip, (unsigned) bfd_get_format(ibfd));
	} /* if anything fails along the way */
	

	if (bfd_close(ibfd) == false) {
		bfd_perror(bfd_get_filename(ibfd));
	} /* on close error */

	if (bfd_close(obfd) == false) {
		bfd_perror(bfd_get_filename(obfd));
	} /* on close error */
	
	if (unlink(tmpfilename)) {
		perror(tmpfilename);
	} /* on error */
	
	unhandle_sigs();

	return(1);
} /* strip_file() */


boolean
bfd_set_start_address (abfd, new_address)
     bfd *abfd;
     bfd_vma new_address;
{
  bfd_get_start_address (abfd) = new_address;
  return true;
}



static int
strip_bfd(ibfd, obfd)
	bfd *ibfd;
	bfd *obfd;
{
	if (bfd_set_start_address(obfd, bfd_get_start_address(ibfd)) == false
	    || bfd_set_file_flags(obfd, bfd_get_file_flags(ibfd) & ~(HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS)) == false
	    || bfd_set_start_address(obfd, bfd_get_start_address(ibfd)) == false) {
		bfd_perror(bfd_get_filename(ibfd));
		return(1);
	} /* on error setting file attributes */

      /* bfd mandates that all output sections be created and sizes set before
	 any output is done.  Thus, we traverse all sections twice.  */

	fatal_error = 0;
	bfd_map_over_sections (ibfd, setup_sections, (void *)obfd);
	if (!fatal_error)
		bfd_map_over_sections (ibfd, copy_sections,  (void *)obfd);
	return fatal_error;
}

static void
setup_sections(ibfd, isection, obfd)
bfd *ibfd;
sec_ptr isection;
bfd *obfd;
{
	sec_ptr osection;
	char *err;

	do {
	    err = "making";
	    osection = bfd_make_section(obfd, bfd_section_name(ibfd, isection));
	    if (osection == NULL)
		break;
	    err = "size";
	    if (!bfd_set_section_size(obfd, osection, 
		 bfd_section_size(ibfd, isection)))
		break;
	    err = "vma";
	    if (!bfd_set_section_vma(obfd, osection,
		 bfd_section_vma(ibfd, isection)))
		break;
	    err = "alignment";
	    if (!bfd_set_section_alignment(obfd, osection,
		 bfd_section_alignment(ibfd, isection)))
		break;
	    err = "flags";
	    if (!bfd_set_section_flags(obfd, osection,
		 bfd_get_section_flags(ibfd, isection)))
		break;
	    return;
	} while (0);

	(void) fprintf(stderr, "file \"%s\", section \"%s\": error in %s: ",
		       bfd_get_filename(ibfd),
		       bfd_section_name(ibfd, isection),
		       err);

	bfd_perror("");
	fatal_error = 1;
}

static void
copy_sections(ibfd, isection, obfd)
bfd *ibfd;
sec_ptr isection;
bfd *obfd;
{
	static char *memhunk = NULL;
	static unsigned memhunksize = 0;

	sec_ptr osection;
	unsigned long size;
	flagword iflg;
	char *temp;

	osection = bfd_get_section_by_name (obfd,
					    bfd_section_name(ibfd, isection));

	size = bfd_section_size(ibfd, isection);
	iflg = bfd_get_section_flags(ibfd, isection);

	/* either:
	   we don't need any memory because there's nothing in this section,
	   we had no memory so we got some,
	   we had some memory but not enough so we got more,
	   or we fail to allocat. */

	if (size == 0)
		return;

	if (memhunk == NULL) {
		memhunk = malloc (size);
		memhunksize = size;
	}

	if (size > memhunksize) {
		temp = realloc (memhunk, size);
		memhunksize = size;
		if (!temp)	/* If realloc failed, blow away our mem */
			free (memhunk);
		memhunk = temp;
	}

	if (memhunk == NULL) {
		/* failed to allocate or reallocate */
		/* FIXME, we should just copy in pieces. */
		(void) fprintf(stderr,
	   "Could not allocate %lu bytes in which to copy section.\n", size);
		return;
	}

	/* now we have enough memory */
	
	if (!bfd_get_section_contents(ibfd, isection, memhunk, 0, size)) {
		bfd_perror(bfd_get_filename(ibfd));
		fatal_error = 1;
		return;
	}
	if (!bfd_set_section_contents(obfd, osection, memhunk, 0, size)) {
		bfd_perror(bfd_get_filename(obfd));
		fatal_error = 1;
		return;
	}
}

void
usage ()
{
  fprintf (stderr, "\
Usage: %s [-gsxSX] [+strip-all] [+strip-debug] [+discard-all]\n\
       [+discard-locals] file...\n", program_name);
  exit (1);
}

/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of strip.c */
