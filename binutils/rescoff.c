/* resrc.c -- read and write Windows rc files.
   Copyright 1997 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* This file contains function that read and write Windows resources
   in COFF files.  */

#include "bfd.h"
#include "bucomm.h"
#include "libiberty.h"
#include "windres.h"

/* In order to use the address of a resource data entry, we need to
   get the image base of the file.  Right now we extract it from
   internal BFD information.  FIXME.  */

#include "coff/internal.h"
#include "libcoff.h"

/* Information we extract from the file.  */

struct coff_file_info
{
  /* File name.  */
  const char *filename;
  /* Data read from the file.  */
  const bfd_byte *data;
  /* End of data read from file.  */
  const bfd_byte *data_end;
  /* Address of the resource section minus the image base of the file.  */
  bfd_vma secaddr;
  /* Non-zero if the file is big endian.  */
  int big_endian;
};

/* A resource directory table in a COFF file.  */

struct extern_res_directory
{
  /* Characteristics.  */
  bfd_byte characteristics[4];
  /* Time stamp.  */
  bfd_byte time[4];
  /* Major version number.  */
  bfd_byte major[2];
  /* Minor version number.  */
  bfd_byte minor[2];
  /* Number of named directory entries.  */
  bfd_byte name_count[2];
  /* Number of directory entries with IDs.  */
  bfd_byte id_count[2];
};

/* A resource directory entry in a COFF file.  */

struct extern_res_entry
{
  /* Name or ID.  */
  bfd_byte name[4];
  /* Address of resource entry or subdirectory.  */
  bfd_byte rva[4];
};

/* A resource data entry in a COFF file.  */

struct extern_res_data
{
  /* Address of resource data.  This is apparently a file relative
     address, rather than a section offset.  */
  bfd_byte rva[4];
  /* Size of resource data.  */
  bfd_byte size[4];
  /* Code page.  */
  bfd_byte codepage[4];
  /* Reserved.  */
  bfd_byte reserved[4];
};

/* Macros to swap in values.  */

#define get_16(fi, s) ((fi)->big_endian ? bfd_getb16 (s) : bfd_getl16 (s))
#define get_32(fi, s) ((fi)->big_endian ? bfd_getb32 (s) : bfd_getl32 (s))

/* Local functions.  */

static void overrun PARAMS ((const struct coff_file_info *, const char *));
static struct res_directory *read_coff_res_dir
  PARAMS ((const bfd_byte *, const struct coff_file_info *));
static struct res_resource *read_coff_data_entry
  PARAMS ((const bfd_byte *, const struct coff_file_info *));

/* Read the resources in a COFF file.  */

struct res_directory *
read_coff_rsrc (filename, target)
     const char *filename;
     const char *target;
{
  bfd *abfd;
  char **matching;
  asection *sec;
  bfd_size_type size;
  bfd_byte *data;
  struct coff_file_info finfo;

  abfd = bfd_openr (filename, target);
  if (abfd == NULL)
    bfd_fatal (filename);

  if (! bfd_check_format_matches (abfd, bfd_object, &matching))
    {
      bfd_nonfatal (bfd_get_filename (abfd));
      if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
	list_matching_formats (matching);
      xexit (1);
    }

  sec = bfd_get_section_by_name (abfd, ".rsrc");
  if (sec == NULL)
    {
      fprintf (stderr, "%s: %s: no resource section\n", program_name,
	       filename);
      xexit (1);
    }

  size = bfd_section_size (abfd, sec);
  data = (bfd_byte *) xmalloc (size);

  if (! bfd_get_section_contents (abfd, sec, data, 0, size))
    bfd_fatal ("can't read resource section");

  finfo.filename = filename;
  finfo.data = data;
  finfo.data_end = data + size;
  finfo.secaddr = (bfd_get_section_vma (abfd, sec)
		   - pe_data (abfd)->pe_opthdr.ImageBase);
  finfo.big_endian = bfd_big_endian (abfd);

  bfd_close (abfd);

  /* Now just read in the top level resource directory.  Note that we
     don't free data, since we create resource entries that point into
     it.  If we ever want to free up the resource information we read,
     this will have to be cleaned up.  */

  return read_coff_res_dir (data, &finfo);
}

/* Give an error if we are out of bounds.  */

static void
overrun (finfo, msg)
     const struct coff_file_info *finfo;
     const char *msg;
{
  fatal ("%s: %s: address out of bounds", finfo->filename, msg);
}

/* Read a resource directory.  */

static struct res_directory *
read_coff_res_dir (data, finfo)
     const bfd_byte *data;
     const struct coff_file_info *finfo;
{
  const struct extern_res_directory *erd;
  struct res_directory *rd;
  int name_count, id_count, i;
  struct res_entry **pp;
  const struct extern_res_entry *ere;

  if (finfo->data_end - data < sizeof (struct extern_res_directory))
    overrun (finfo, "directory");

  erd = (const struct extern_res_directory *) data;

  rd = (struct res_directory *) xmalloc (sizeof *rd);
  rd->characteristics = get_32 (finfo, erd->characteristics);
  rd->time = get_32 (finfo, erd->time);
  rd->major = get_16 (finfo, erd->major);
  rd->minor = get_16 (finfo, erd->minor);
  rd->entries = NULL;

  name_count = get_16 (finfo, erd->name_count);
  id_count = get_16 (finfo, erd->id_count);

  pp = &rd->entries;

  /* The resource directory entries immediately follow the directory
     table.  */
  ere = (const struct extern_res_entry *) (erd + 1);

  for (i = 0; i < name_count; i++, ere++)
    {
      unsigned long name, rva;
      struct res_entry *re;
      const bfd_byte *ers;
      int length, j;

      if ((const bfd_byte *) ere >= finfo->data_end)
	overrun (finfo, "named directory entry");

      name = get_32 (finfo, ere->name);
      rva = get_32 (finfo, ere->rva);

      /* For some reason the high bit in NAME is set.  */
      name &=~ 0x80000000;

      if (name > finfo->data_end - finfo->data)
	overrun (finfo, "directory entry name");

      ers = finfo->data + name;

      re = (struct res_entry *) xmalloc (sizeof *re);
      re->next = NULL;
      re->id.named = 1;
      length = get_16 (finfo, ers);
      re->id.u.n.length = length;
      re->id.u.n.name = ((unsigned short *)
			 xmalloc (length * sizeof (unsigned short)));
      for (j = 0; j < length; j++)
	re->id.u.n.name[j] = get_16 (finfo, ers + j * 2 + 2);

      if ((rva & 0x80000000) != 0)
	{
	  rva &=~ 0x80000000;
	  if (rva >= finfo->data_end - finfo->data)
	    overrun (finfo, "named subdirectory");
	  re->subdir = 1;
	  re->u.dir = read_coff_res_dir (finfo->data + rva, finfo);
	}
      else
	{
	  if (rva >= finfo->data_end - finfo->data)
	    overrun (finfo, "named resource");
	  re->subdir = 0;
	  re->u.res = read_coff_data_entry (finfo->data + rva, finfo);
	}

      *pp = re;
      pp = &re->next;
    }

  for (i = 0; i < id_count; i++, ere++)
    {
      unsigned long name, rva;
      struct res_entry *re;

      if ((const bfd_byte *) ere >= finfo->data_end)
	overrun (finfo, "ID directory entry");

      name = get_32 (finfo, ere->name);
      rva = get_32 (finfo, ere->rva);

      re = (struct res_entry *) xmalloc (sizeof *re);
      re->next = NULL;
      re->id.named = 0;
      re->id.u.id = name;

      if ((rva & 0x80000000) != 0)
	{
	  rva &=~ 0x80000000;
	  if (rva >= finfo->data_end - finfo->data)
	    overrun (finfo, "ID subdirectory");
	  re->subdir = 1;
	  re->u.dir = read_coff_res_dir (finfo->data + rva, finfo);
	}
      else
	{
	  if (rva >= finfo->data_end - finfo->data)
	    overrun (finfo, "ID resource");
	  re->subdir = 0;
	  re->u.res = read_coff_data_entry (finfo->data + rva, finfo);
	}

      *pp = re;
      pp = &re->next;
    }

  return rd;
}

/* Read a resource data entry.  */

static struct res_resource *
read_coff_data_entry (data, finfo)
     const bfd_byte *data;
     const struct coff_file_info *finfo;
{
  const struct extern_res_data *erd;
  struct res_resource *r;
  unsigned long size, rva;
  const bfd_byte *resdata;

  if (finfo->data_end - data < sizeof (struct extern_res_data))
    overrun (finfo, "data entry");

  erd = (const struct extern_res_data *) data;

  r = (struct res_resource *) xmalloc (sizeof *r);
  memset (&r->res_info, 0, sizeof (struct res_res_info));
  r->coff_info.codepage = get_32 (finfo, erd->codepage);
  r->coff_info.reserved = get_32 (finfo, erd->reserved);

  size = get_32 (finfo, erd->size);
  rva = get_32 (finfo, erd->rva);
  if (rva < finfo->secaddr
      || rva - finfo->secaddr >= finfo->data_end - finfo->data)
    overrun (finfo, "resource data");

  resdata = finfo->data + (rva - finfo->secaddr);

  r->type = RES_TYPE_USERDATA;
  r->u.userdata = ((struct rcdata_data *)
		   xmalloc (sizeof (struct rcdata_data)));
  r->u.userdata->first = ((struct rcdata_item *)
			  xmalloc (sizeof (struct rcdata_item)));
  r->u.userdata->last = r->u.userdata->first;
  r->u.userdata->first->next = NULL;
  r->u.userdata->first->type = RCDATA_BUFFER;
  r->u.userdata->first->u.buffer.length = size;
  r->u.userdata->first->u.buffer.data = (unsigned char *) resdata;

  return r;
}
