/* arsup.c - Archive support for MRI compatibility */

/*   Copyright (C) 1992 Free Software Foundation, Inc.

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


/* Contributed by Steve Chamberlain
   		  sac@cygnus.com

This file looks after requests from arparse.y, to provide the MRI
style librarian command syntax + 1 word LIST

*/

#include "bfd.h"
#include "arsup.h"
#include <sysdep.h>
#include "bucomm.h"
extern bfd *inarch;
extern int verbose;



void
DEFUN(map_over_list,(function, list),
      void            (*function) () AND
      struct list           *list)

{
  bfd            *head;

  if (list == 0) {
    for (head = inarch->next; head; head = head->next){
      function(head);
    }
  }
  else {
    /*
      This may appear to be a baroque way of accomplishing what we want.
      however we have to iterate over the filenames in order to notice where
      a filename is requested but does not exist in the archive.  Ditto
      mapping over each file each time -- we want to hack multiple
      references.
      */
    struct list *ptr = list;

    for (ptr = list; ptr; ptr=ptr->next)
    {
      boolean         found = false;
      bfd *prev = inarch;
      for (head = inarch->next; head; head = head->next) 
      {
	if ((head->filename != NULL) &&
	    (!strcmp(ptr->name, head->filename))) 
	{
	  found = true;
	  function(head, prev);

	}
	prev = head;
      }
      if (!found)
       fprintf(stderr, "No entry %s in archive.\n", ptr->name);
    }
  }
}


FILE *outfile;
void
DEFUN(ar_directory_doer,(abfd),
      bfd *abfd)
{
    print_arelt_descr(outfile, abfd, verbose);
}

void
DEFUN(ar_directory,(ar_name, list, output),
      char *ar_name AND
      struct list *list AND
      char *output)
{
  open_inarch(ar_name);
  if (output)  {
    outfile = fopen(output,"w");
    if (outfile == 0) {
      outfile =stdout;
      fprintf(stderr,"Can't open file %s\n", output);
      output = 0;
    }
  }
  else 
   outfile = stdout;

  map_over_list(ar_directory_doer, list);
  bfd_close(inarch);
  if (output)
   fclose(outfile);
}

void
DEFUN_VOID(prompt)
{
  extern int interactive;
  if (interactive) 
  {
    printf("AR >");
    fflush(stdout); 
  }
}

void
DEFUN_VOID(maybequit)
{
if (!interactive) 
 exit(9);
}


bfd *obfd;
char *real_name ; 
void
DEFUN(ar_open,(name, t),
      char *name AND
      int t)

{
  char *tname = malloc(strlen(name)+10);
  real_name = name;
  sprintf(tname, "%s-tmp", name);
  obfd = bfd_openw(tname, NULL);

  if (!obfd) {
    fprintf(stderr,"%s: Can't open output archive %s\n", program_name,
	    tname);

    maybequit();
  }
  else {
    if (!t) {
      bfd **ptr;
      bfd *element;
      bfd *ibfd;
      ibfd = bfd_openr(name, NULL);
      if (bfd_check_format(ibfd, bfd_archive) != true) {
	fprintf(stderr,"%s: file %s is not an archive\n", program_name,
		name);
	maybequit();
	return;
      }
      ptr = &(obfd->archive_head);
      element = bfd_openr_next_archived_file(ibfd, NULL);

      while (element) {
	*ptr = element;
	ptr = &element->next;
	element = bfd_openr_next_archived_file(ibfd, element);
      }
    }

    bfd_set_format(obfd, bfd_archive);

    obfd->has_armap = 1;
  }
}


void
DEFUN(ar_addlib_doer, (abfd, prev),
      bfd *abfd AND
      bfd *prev)
{
  /* Add this module to the output bfd */
  
  prev->next = abfd->next;
  abfd->next = obfd->archive_head;
  obfd->archive_head = abfd;
}

void
DEFUN(ar_addlib, (name, list),
      char *name AND
      struct list *list)
{
  if (!obfd) {
    fprintf(stderr, "%s: no output archive specified yet\n", program_name);
    maybequit();
  }
  else {
    if (open_inarch(name) ) {
      map_over_list(ar_addlib_doer, list);
    }
    /* Don't close the bfd, since it will make the elements disasppear */
  }
}



void
DEFUN(ar_addmod, (list),
      struct list *list)
{
  if (!obfd) {
    fprintf(stderr, "%s: no open output archive\n", program_name);
    maybequit();
  }
  else 
  {
    while (list) {
      bfd *abfd = bfd_openr(list->name, NULL);
      if (!abfd)  {
	fprintf(stderr,"%s: can't open file %s\n", program_name,
		list->name);
	maybequit();
      }
      else {
	abfd->next = obfd->archive_head;
	obfd->archive_head = abfd;
      }
      list = list->next;
    }
  }
}



void
DEFUN_VOID(ar_clear)
{
if (obfd) 
 obfd->archive_head = 0;
}

void
DEFUN(ar_delete, (list),
      struct list *list)
{
  if (!obfd) {
    fprintf(stderr, "%s: no open output archive\n", program_name);
    maybequit();
  }
  else 
  {
    while (list) {
      /* Find this name in the archive */
      bfd *member = obfd->archive_head;
      bfd **prev = &(obfd->archive_head);
      int found = 0;
      while (member) {
	if (strcmp(member->filename, list->name) == 0) {
	  *prev = member->next;
	  found = 1;
	}
	else {
	  prev = &(member->next);
	}
	  member = member->next;
      }
      if (!found)  {
	fprintf(stderr,"%s: can't find module file %s\n", program_name,
		list->name);
	maybequit();
      }
      list = list->next;
    }
  }
}


void
DEFUN_VOID(ar_save)
{

  if (!obfd) {
    fprintf(stderr, "%s: no open output archive\n", program_name);
    maybequit();
  }
  else {
    bfd_close(obfd);
    unlink(real_name);
    link(obfd->filename, real_name);
    unlink(obfd->filename);
    obfd = 0;
  }
}



void
DEFUN(ar_replace, (list),
      struct list *list)
{
  if (!obfd) {
    fprintf(stderr, "%s: no open output archive\n", program_name);
    maybequit();
  }
  else 
  {
    while (list) {
      /* Find this name in the archive */
      bfd *member = obfd->archive_head;
      bfd **prev = &(obfd->archive_head);
      int found = 0;
      while (member) 
      {
	if (strcmp(member->filename, list->name) == 0) 
	{
	  /* Found the one to replace */
	  bfd *abfd = bfd_openr(list->name, 0);
	  if (!abfd) 
	  {
	    fprintf(stderr, "%s: can't open file %s\n", program_name, list->name);
	    maybequit();
	  }
	  else {
	    *prev = abfd;
	    abfd->next = member->next;
	    found = 1;
	  }
	}
	else {
	  prev = &(member->next);
	}
	member = member->next;
      }
      if (!found)  {
	bfd *abfd = bfd_openr(list->name, 0);
	fprintf(stderr,"%s: can't find module file %s\n", program_name,
		list->name);
	if (!abfd) 
	{
	  fprintf(stderr, "%s: can't open file %s\n", program_name, list->name);
	  maybequit();
	}
	else 
	{
	  *prev = abfd;
	}
      }

    list = list->next;
    }
  }
}

/* And I added this one */
void
DEFUN_VOID(ar_list)
{
  if (!obfd) 
  {
    fprintf(stderr, "%s: no open output archive\n", program_name);
    maybequit();
  }
  else {
    bfd *abfd;
    outfile = stdout;
    verbose =1 ;
    printf("Current open archive is %s\n", obfd->filename);
    for (abfd = obfd->archive_head;
	 abfd != (bfd *)NULL;
	 abfd = abfd->next) 
    {
      ar_directory_doer(abfd);
    }
  }
}


void
DEFUN(ar_extract,(list),
      struct list *list)
{
  if (!obfd) 
  {

    fprintf(stderr, "%s: no open  archive\n", program_name);
    maybequit();
  }
  else 
  {
    while (list) {
      /* Find this name in the archive */
      bfd *member = obfd->archive_head;
      int found = 0;
      while (member && !found) 
      {
	if (strcmp(member->filename, list->name) == 0) 
	{
	  extract_file(member);
	  found = 1;
	  }

	member = member->next;
      }
      if (!found)  {
	bfd *abfd = bfd_openr(list->name, 0);
	fprintf(stderr,"%s: can't find module file %s\n", program_name,
		list->name);

      }
      list = list->next;
    }
  }
}
