/* copy.c -- copy object file from input to output, optionally massaging it.
   Copyright (C) 1991 Free Software Foundation, Inc.

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

#include "bfd.h"
#include "sysdep.h"
#include "bucomm.h"

asymbol       **sympp;
char           *input_target = NULL;
char           *output_target = NULL;
char           *input_filename = NULL;
char           *output_filename = NULL;


static void     setup_sections();
static void     copy_sections();
static boolean verbose;

/* This flag distinguishes between strip and copy:
   1 means this is 'strip'; 0 means this is 'copy'.
   -1 means if we should use argv[0] to decide. */
extern int is_strip;

/* IMPORTS */
extern char    *program_name;
extern char    *program_version;


static
void            
usage()
{
    fprintf(stderr,
    "Usage %s [-S][-s srcfmt] [-d dtfmt] [-b bothfmts] infile [outfile] [-vV]\n",
	    program_name);
    exit(1);
}

static
void            
strip_usage()
{
    fprintf(stderr, "Usage %s [-vV] filename ...\n", program_name);
    exit(1);
}


/* Create a temp file in the same directory as supplied */
static
char *
make_tempname(filename)
char *filename;
{
    static char template[] = "stXXXXXX";
    char *tmpname;
    char *      slash = strrchr( filename, '/' );
    if (slash != (char *)NULL){
	*slash = 0;
	tmpname = xmalloc(strlen(filename) + sizeof(template) + 1 );
	strcpy(tmpname, filename);
	strcat(tmpname, "/" );
	strcat(tmpname, template);
	mktemp(tmpname );
	*slash = '/';
    } else {
	tmpname = xmalloc(sizeof(template));
	strcpy(tmpname, template);
	mktemp(tmpname);
    }
    return tmpname;
}

/*
   All the symbols have been read in and point to their owning input section.
   They have been relocated to that they are all relative to the base of
   their owning section. On the way out, all the symbols will be relocated to
   their new location in the output file, through some complex sums.

*/
static void
mangle_sections(ibfd, obfd)
    bfd            *ibfd;
    bfd            *obfd;
{
    asection       *current = ibfd->sections;
    for (; current != NULL; current = current->next) {
	current->output_section = bfd_get_section_by_name(obfd, current->name);
	current->output_offset = 0;
    }
}

static 
void
copy_object(ibfd, obfd)
bfd *ibfd;
bfd *obfd;
{

    unsigned int symcount;


    if (!bfd_set_format(obfd, bfd_get_format(ibfd)))
	bfd_fatal(output_filename);


    if (verbose)
	printf("copy from %s(%s) to %s(%s)\n",
	       ibfd->filename, ibfd->xvec->name,
	       obfd->filename, obfd->xvec->name);

    if ((bfd_set_start_address(obfd, bfd_get_start_address(ibfd)) == false) 
	||
	(bfd_set_file_flags(obfd, (bfd_get_file_flags(ibfd) &
				   (HAS_LINENO | HAS_DEBUG |
				    HAS_RELOC | HAS_SYMS | D_PAGED |
				     HAS_LOCALS))) == false)) {
      bfd_fatal(bfd_get_filename(ibfd));
    }

    /* Copy architecture of input file to output file */
    if (!bfd_set_arch_mach(obfd, bfd_get_arch(ibfd),
			   bfd_get_mach(ibfd))) {
	fprintf(stderr, "Output file cannot represent architecture %s\n",
		bfd_printable_arch_mach(bfd_get_arch(ibfd),
					bfd_get_mach(ibfd)));
    }
    if (!bfd_set_format(obfd, bfd_get_format(ibfd)))
	{
	    bfd_fatal(ibfd->filename);
	}

    sympp = (asymbol **) xmalloc(get_symtab_upper_bound(ibfd));
    symcount = bfd_canonicalize_symtab(ibfd, sympp);

    bfd_set_symtab(obfd, sympp, is_strip ? 0 : symcount);
    
    /*
      bfd mandates that all output sections be created and sizes set before
      any output is done.  Thus, we traverse all sections twice.
      */
    bfd_map_over_sections(ibfd, setup_sections, (void *) obfd);
    bfd_map_over_sections(ibfd, copy_sections, (void *) obfd);
    mangle_sections(ibfd, obfd);
}
static
char *
cat(a,b,c)
char *a;
char *b;
char *c;
{
    int size = strlen(a) + strlen(b) + strlen(c);
    char *r = xmalloc(size+1);
    strcpy(r,a);
    strcat(r,b);
    strcat(r,c);
    return r;
}

static void 
copy_archive(ibfd, obfd)
bfd *ibfd;
bfd *obfd;
{
    bfd **ptr = &obfd->archive_head;
    bfd *this_element;
    /* Read each archive element in turn from the input, copy the
       contents to a temp file, and keep the temp file handle */
    char *dir = cat("./#",make_tempname(""),"cd");

    /* Make a temp directory to hold the contents */
    mkdir(dir,0777);
    obfd->has_armap = ibfd->has_armap;
    this_element = bfd_openr_next_archived_file(ibfd, NULL);
    ibfd->archive_head = this_element;
    while (this_element != (bfd *)NULL) {

	/* Create an output file for this member */
	char *output_name = cat(dir, "/",this_element->filename);
	bfd *output_bfd = bfd_openw(output_name, output_target);

	if (!bfd_set_format(obfd, bfd_get_format(ibfd)))
	    bfd_fatal(output_filename);

	if (output_bfd == (bfd *)NULL) {
	    bfd_fatal(output_name);
	}
	if (bfd_check_format(this_element, bfd_object) == true) {
	    copy_object(this_element, output_bfd);
	}

	bfd_close(output_bfd);
	/* Now open the newly output file and attatch to our list */
	output_bfd = bfd_openr(output_name, output_target);
	/* Mark it for deletion */

	*ptr = output_bfd;

	ptr = &output_bfd->next;
	this_element->next = bfd_openr_next_archived_file(ibfd, this_element);
	this_element = this_element->next;

    }
    *ptr = (bfd *)NULL;

    if (!bfd_close(obfd))
	bfd_fatal(output_filename);

    /* Now delete all the files that we opened.
       Construct their names again, unfortunately, but so what;
       we're about to exit anyway. */
    for (this_element = ibfd->archive_head;
	 this_element != (bfd *)NULL;
	 this_element = this_element->next) 
	{
	unlink(cat(dir,"/",this_element->filename));
    }
    rmdir(dir);
    if (!bfd_close(ibfd))
	bfd_fatal(input_filename);

}

static
void
copy_file(input_filename, output_filename)
    char           *input_filename;
    char           *output_filename;
{
  bfd            *ibfd;

  ibfd = bfd_openr(input_filename, input_target);
  if (ibfd == NULL)
    bfd_fatal(input_filename);

  if (bfd_check_format(ibfd, bfd_object)) {
    bfd * obfd = bfd_openw(output_filename, output_target);
    if (obfd == NULL)
      bfd_fatal(output_filename);

    copy_object(ibfd, obfd);

    if (ibfd->flags & EXEC_P)
	obfd->flags |= EXEC_P;
    if (!bfd_close(obfd))
      bfd_fatal(output_filename);

    if (!bfd_close(ibfd))
      bfd_fatal(input_filename);
  }
  else if (bfd_check_format(ibfd, bfd_archive)) {
    bfd * obfd = bfd_openw(output_filename, output_target);
    if (obfd == NULL)
      bfd_fatal(output_filename);
    copy_archive(ibfd, obfd);
  }
}



/** Actually do the work */
static void
setup_sections(ibfd, isection, obfd)
    bfd            *ibfd;
    sec_ptr         isection;
    bfd            *obfd;
{
    sec_ptr         osection;
    char           *err;

    osection = bfd_get_section_by_name(obfd, bfd_section_name(ibfd, isection));
    if (osection == NULL) {
	osection = bfd_make_section(obfd, bfd_section_name(ibfd, isection));
	if (osection == NULL) {
	    err = "making";
	    goto loser;
	}
    }

    if (!bfd_set_section_size(obfd,
			      osection,
			      bfd_section_size(ibfd, isection))) {
	err = "size";
	goto loser;
    }

    if (bfd_set_section_vma(obfd,
			    osection,
			    bfd_section_vma(ibfd, isection))
	== false) {
	err = "vma";
	goto loser;
    }				/* on error */

    if (bfd_set_section_alignment(obfd,
				  osection,
				  bfd_section_alignment(ibfd, isection))
	== false) {
	err = "alignment";
	goto loser;
    }				/* on error */

    if (!bfd_set_section_flags(obfd, osection,
			       bfd_get_section_flags(ibfd, isection))) {
	err = "flags";
	goto loser;
    }

    /* All went well */
    return;

loser:
    fprintf(stderr, "%s: file \"%s\", section \"%s\": error in %s: %s\n",
	    program_name,
	    bfd_get_filename(ibfd), bfd_section_name(ibfd, isection),
	    err, bfd_errmsg(bfd_error));
    exit(1);
}				/* setup_sections() */

/*
Copy all the section related data from an input section
to an output section

If stripping then don't copy any relocation info
*/
static void
copy_sections(ibfd, isection, obfd)
    bfd            *ibfd;
    sec_ptr         isection;
    bfd            *obfd;
{

  arelent       **relpp;
  int             relcount;
  sec_ptr         osection;
  bfd_size_type   size;
  osection = bfd_get_section_by_name(obfd,
				     bfd_section_name(ibfd, isection));

  size = bfd_get_section_size_before_reloc(isection);

  if (size == 0)
    return;

  if (is_strip || bfd_get_reloc_upper_bound(ibfd, isection) == 0) 
    {
      bfd_set_reloc(obfd, osection, (arelent **)NULL, 0);
    } 
  else 
    {
      relpp = (arelent **) xmalloc(bfd_get_reloc_upper_bound(ibfd, isection));
      relcount = bfd_canonicalize_reloc(ibfd, isection, relpp, sympp);
      bfd_set_reloc(obfd, osection, relpp, relcount);
    }

  isection->_cooked_size = isection->_raw_size;
  isection->reloc_done =true;
  

  if (bfd_get_section_flags(ibfd, isection) & SEC_HAS_CONTENTS) 
    {
      PTR memhunk = (PTR) xmalloc((unsigned)size);

      if (!bfd_get_section_contents(ibfd, isection, memhunk, (file_ptr) 0, size))
	bfd_fatal(bfd_get_filename(ibfd));

      if (!bfd_set_section_contents(obfd, osection, memhunk, (file_ptr)0, size))
	bfd_fatal(bfd_get_filename(obfd));
      free(memhunk);
    }


}
int
main(argc, argv)
    int             argc;
    char           *argv[];
{
  int             i;
  int		  show_version = 0;
  program_name = argv[0];

  bfd_init();

  if (is_strip < 0) {
      i = strlen (program_name);
      is_strip = (i >= 5 && strcmp(program_name+i-5,"strip"));
  }

  if (is_strip)
    {
      for (i = 1; i < argc; i++) 
        {
          if (argv[i][0] != '-')
	    break;
	  if (argv[i][1] == '-') {
	    i++;
	    break;
	  }
	  switch (argv[i][1]) {
	    case 'V':
	      show_version = true;
	      break;
	    case 'v':
	      verbose = true;
	      show_version = true;
	      break;

	      /* undocumented for now */
	    case 'd':
	      i++;
	      output_target = argv[i];
	      break;
	    default:
	      strip_usage();
	  }
        }
      if (show_version)
	printf ("%s version %s\n", program_name, program_version);
      for ( ; i < argc; i++) {
	    char *tmpname = make_tempname(argv[i]);
	    copy_file(argv[i], tmpname);
	    rename(tmpname, argv[i]);
      }
      return 0;
    }

  for (i = 1; i < argc; i++) 
    {
      if (argv[i][0] == '-') {
	switch (argv[i][1]) {
	case 'V':
	  show_version = true;
	  break;
	case 'v':
	  show_version = true;
	  verbose = true;
	  break;
	case 'b':
	  i++;
	  input_target = output_target = argv[i];
	  break;
	case 'S':
	  is_strip = 1;
	  break;
	case 's':
	  i++;
	  input_target = argv[i];
	  break;
	case 'd':
	  i++;
	  output_target = argv[i];
	  break;
	default:
	  usage();
	}
      }
      else {
	if (input_filename) {
	  output_filename = argv[i];
	}
	else {
	  input_filename = argv[i];
	}
      }
    }

  if (show_version)
    printf ("%s version %s\n", program_name, program_version);

  if (input_filename == (char *) NULL)
    usage();

  if (output_target == (char *) NULL)
    output_target = input_target;

  /* If there is no  destination file then create a temp and rename
     the result into the input */

  if (output_filename == (char *)NULL) {
    char *	tmpname = make_tempname(input_filename);
    copy_file(input_filename, tmpname);
    output_filename = input_filename;
    rename(tmpname, input_filename);
  }
  else {
    copy_file(input_filename, output_filename);
  }
  return 0;
}
