/* strip.c -- strip certain symbols from a rel file.
   Copyright (C) 1986, 1990, 1991 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* BUGS: When there's not enough memory, this should do the copy
   in pieces rather than just fail as it does now */

#include "bfd.h"
#include "sysdep.h"
#include "getopt.h"
#include <signal.h>

/* Various program options */

int show_version = 0;

/* Which symbols to remove. */
enum strip_action {
  strip_undef,
  strip_all,			/* strip all symbols */
  strip_debug,			/* strip all debugger symbols */
} strip_symbols;

/* Which local symbols to remove. */
enum {
  locals_undef,
  locals_start_L,		/* discard locals starting with L */
  locals_all,			/* discard all locals */
} discard_locals;

extern char *mktemp();

/* IMPORTS */
extern char *program_version;
extern char *program_name;
extern char *target;
extern char *xmalloc();

PROTO(static boolean, strip_file, (char *filetostrip));
PROTO(static void, copy_sections, (bfd *ibfd, sec_ptr isection, bfd *obfd));
PROTO(static void, setup_sections, (bfd *ibfd, sec_ptr isection, bfd *obfd));

/** main, etc */

static void
usage ()
{
  fprintf (stderr, "strip %s\nUsage: %s [-gsxSX] files ...\n",
	   program_version, program_name);
  exit (1);
}

struct option long_options[] = {{"strip-all", 0, 0, 's'},
				{"strip-debug", 0, 0, 'S'},
				{"discard-all", 0, 0, 'x'},
				{"discard-locals", 0, 0, 'X'},
				{0, 0, 0, 0}
				};

int
main (argc, argv)
     char **argv;
     int argc;
{
  int ind;
  int c;
  program_name = argv[0];
	
  bfd_init();

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
    }
  }

  /* Default is to strip all symbols: */
  if (strip_symbols == strip_undef && discard_locals == locals_undef) {
    strip_symbols = strip_all;
  }

  /* OK, all options now parsed.  If no filename specified, do a.out. */
  if (optind == argc) return !strip_file ("a.out");
  
  /* We were given several filenames to do: */
  while (optind < argc)
    if (!strip_file (argv[optind++])) return 1;

  return 0;
}

/** Hack signals */

/* Why does strip need to do this, and anyway, if it does shouldn't this be
   handled by bfd? */

static int delayed_signal;

static int sigint_handled = 0;
static int sighup_handled = 0;
static int sigterm_handled = 0;

void
delay_signal (signo)
     int signo;
{
  delayed_signal = signo;
  signal (signo, delay_signal);
}

/* Effectively defer handling of asynchronous kill signals.  */
void
handle_sigs ()			/* puff puff */
{
  delayed_signal = 0;
	
  if (signal (SIGINT, SIG_IGN) != SIG_IGN) {
    sigint_handled = 1;
    signal (SIGINT, delay_signal);
  }
	
  if (signal (SIGHUP, SIG_IGN) != SIG_IGN) {
    sighup_handled = 1;
    signal (SIGHUP, delay_signal);
  }
	
  if (signal (SIGTERM, SIG_IGN) != SIG_IGN) {
    sigterm_handled = 1;
    signal (SIGTERM, delay_signal);
  }
	
  return;
}

/* Effectively undefer handling.  */
void
unhandle_sigs ()		/* put them down */
{
  if (sigint_handled) signal (SIGINT, SIG_DFL);

  if (sighup_handled) signal (SIGHUP, SIG_DFL);

  if (sigterm_handled) signal (SIGTERM, SIG_DFL);
	
  /* Handle any signal that came in while they were deferred.  */
  if (delayed_signal)
    kill (getpid (), delayed_signal);
	
  return;
}


static boolean
strip_file (filetostrip)
     char *filetostrip;
{
  static char template[] = "stXXXXXX";
  char *slash;
  char *tmpname;
  bfd *ibfd;
  bfd *obfd;

  ibfd = bfd_openr (filetostrip, target);

  if (ibfd == NULL) bfd_fatal (filetostrip);

  handle_sigs ();		/* light up */

  if (!bfd_check_format (ibfd, bfd_object)) {
    fprintf (stderr, "Can't strip %s file %s.\n",
	     bfd_format_string (bfd_get_format (ibfd)), filetostrip);
    exit (1);
  }

  slash = strrchr( filetostrip, '/' );
  if ( slash ){
    *slash = 0;
    tmpname = xmalloc( strlen(filetostrip) + sizeof(template) + 1 );
    strcpy( tmpname, filetostrip );
    strcat( tmpname, "/" );
    strcat( tmpname, template );
    mktemp( tmpname );
    *slash = '/';
  } else {
    tmpname = xmalloc( sizeof(template) );
    strcpy( tmpname, template );
    mktemp( tmpname );
  }

  obfd = bfd_openw (mktemp(tmpname), (target ? target : bfd_get_target (ibfd)));
  if (obfd == NULL) bfd_fatal (tmpname);

  if (!bfd_set_format (obfd, bfd_get_format (ibfd)))
    bfd_fatal (tmpname);


  if ((bfd_set_start_address (obfd, bfd_get_start_address (ibfd)) == false) ||
      (bfd_set_file_flags (obfd, (bfd_get_file_flags (ibfd) &
				  ~(HAS_LINENO | HAS_DEBUG | HAS_SYMS |
				    HAS_LOCALS))) == false) ||
      bfd_set_start_address (obfd, bfd_get_start_address (ibfd)) == false)
    bfd_fatal (bfd_get_filename (ibfd));

   /* Copy architecture of input file to output file */
   if (!bfd_set_arch_mach (obfd, bfd_get_architecture (ibfd),
 			       bfd_get_machine (ibfd))) {
     fprintf(stderr, "Output file cannot represent architecture %s",
 	bfd_printable_arch_mach (bfd_get_architecture(ibfd),
 				 bfd_get_machine (ibfd)));
   }
 

  /* bfd mandates that all output sections be created and sizes set before
     any output is done.  Thus, we traverse all sections twice.  */
  bfd_map_over_sections (ibfd, setup_sections, (void *)obfd);
  bfd_map_over_sections (ibfd, copy_sections, (void *)obfd);

  if (!bfd_close (obfd)) bfd_fatal (filetostrip);
  if (!bfd_close (ibfd)) bfd_fatal (filetostrip);

  rename(tmpname, filetostrip);
  free(tmpname);

  unhandle_sigs();

  return true;
}

/** Actually do the work */
static void
setup_sections (ibfd, isection, obfd)
     bfd *ibfd;
     sec_ptr isection;
     bfd *obfd;
{
  sec_ptr osection;
  char *err;

  osection = bfd_make_section (obfd, bfd_section_name (ibfd, isection));
  if (osection == NULL) {
      err = "making";
      goto loser;
    }

  if (!bfd_set_section_size(obfd, osection, bfd_section_size(ibfd, isection))) {
     err = "size";
     goto loser;
  }

  if (!bfd_set_section_vma (obfd, osection, bfd_section_vma (ibfd, isection))) {
     err = "vma";
     goto loser;
  }

  if (bfd_set_section_alignment (obfd, osection,
			     bfd_section_alignment (ibfd, isection))
      != true) {
	  err = "alignment";
	  goto loser;
  } /* on error, I presume. */

  if (!bfd_set_section_flags (obfd, osection,
			      bfd_get_section_flags (ibfd, isection))) {
     err = "flags";
     goto loser;
  }

  /* All went well */
  return;
  
 loser:
  fprintf (stderr, "%s: file \"%s\", section \"%s\": error in %s: %s\n",
	   program_name,
	   bfd_get_filename (ibfd), bfd_section_name (ibfd, isection),
	   err, bfd_errmsg (bfd_error));
  exit (1);
}

static void
copy_sections (ibfd, isection, obfd)
     bfd *ibfd;
     sec_ptr isection;
     bfd *obfd;
{
  static unsigned char *memhunk = NULL;
  static unsigned memhunksize = 0;

  sec_ptr osection;
  unsigned long size;
  flagword iflg;
  unsigned char *temp;

  osection = bfd_get_section_by_name (obfd, bfd_section_name (ibfd, isection));

  size = bfd_section_size (ibfd, isection);
  iflg = bfd_get_section_flags (ibfd, isection);

  /* either:
     we don't need any memory because there's nothing in this section,
     we had no memory so we got some,
     we had some memory but not enough so we got more,
     or we fail to allocat. */

  if (size == 0)
     return;

  if ((iflg & SEC_HAS_CONTENTS) == 0)
      return;

  if (memhunk == NULL) {
     memhunk = (unsigned char *) xmalloc (size);
     memhunksize = size;
  }

  if (size > memhunksize) {
     temp = (unsigned char *) xrealloc ((char *) memhunk, size);
     memhunksize = size;
     memhunk = temp;
  }

  /* now we have enough memory, just do it: */
  if (!bfd_get_section_contents (ibfd, isection, memhunk, 0, size))
     bfd_fatal (bfd_get_filename (ibfd));

  if (!bfd_set_section_contents (obfd, osection, memhunk, 0, size))
     bfd_fatal (bfd_get_filename (obfd));
}
