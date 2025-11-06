/* CTF archive files.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

   This file is part of libctf.

   libctf is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not see
   <http://www.gnu.org/licenses/>.  */

#include <ctf-impl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>
#include "ctf-endian.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_MMAP
#include <sys/mman.h>
#endif

/* Note on datatypes: the datatype known outside this file as ctf_archive_t
   is here known as struct ctf_archive_internal, to emphasise its role as a
   wrapper with no on-disk representation.

   The on-disk structure is struct ctf_archive.  */

static off_t arc_write_one_ctf (ctf_dict_t * f, int fd, size_t threshold);
static off_t arc_write_one (int fd, const void *item, size_t size, int align);
static int ctf_arc_value_write (int fd, const void *, size_t, uint64_t *start_off);
static int ctf_arc_flip_modents (ctf_archive_modent_t *modent, uint64_t els,
		      unsigned char *ents, uint64_t base, size_t arc_len,
		      ctf_error_t *errp);
static int ctf_arc_range_check_hdr (struct ctf_archive_internal *arci, size_t,
				    ctf_error_t *errp);
static int ctf_arc_range_check_modents (ctf_archive_modent_t *modent,
					struct ctf_archive *arc_hdr,
					unsigned char *arc_bytes,
					uint64_t contents_base,
					size_t contents_els, size_t arc_len,
					int fixup_v1, ctf_error_t *errp);
static ctf_dict_t *ctf_dict_open_by_offset (const struct ctf_archive_internal *,
					    const ctf_sect_t *symsect,
					    const ctf_sect_t *strsect,
					    size_t offset,
					    int little_endian_symtab, ctf_error_t *errp);
static int sort_modent_by_name (const void *one, const void *two, void *n);
static void ctf_arc_close_internal (struct ctf_archive *arc);
static void *arc_mmap_header (int fd, size_t headersz);
static void *arc_mmap_file (int fd, size_t size);
static int arc_mmap_writeout (int fd, void *header, size_t headersz,
			      const char **errmsg);
static int arc_mmap_unmap (void *header, size_t headersz, const char **errmsg);
static int ctf_arc_import_parent (const struct ctf_archive_internal *arci,
				  ctf_dict_t *fp, ctf_error_t *errp);

/* Flag to indicate "symbol not present" in ctf_archive_internal.ctfi_symdicts
   and ctfi_symnamedicts.  Never initialized.  */
static ctf_dict_t enosym;

/* Prepare to serialize everything.  Members of archives have dependencies on
   each other, because the strtabs and type IDs of children depend on the
   parent: so we have to work over the archive as a whole to prepare for final
   serialization.

   Returns zero on success, or an errno, or an ECTF_* value.

   Updates the first dict in the archive with the errno value.  */

static ctf_error_t
ctf_arc_preserialize (ctf_dict_t **ctf_dicts, ssize_t ctf_dict_cnt,
		      size_t threshold)
{
  uint64_t old_parent_strlen, all_strlens = 0;
  ssize_t i;
  ctf_error_t err;

  ctf_dprintf ("Preserializing dicts.\n");

  /* Preserialize everything, doing everything but strtab generation and things
     that depend on that.  */
  for (i = 0; i < ctf_dict_cnt; i++)
    if (ctf_preserialize (ctf_dicts[i], threshold != (size_t) -1
			  || ctf_dict_cnt > 1) < 0)
      goto err;

  ctf_dprintf ("Deduplicating strings.\n");

  for (i = 0; i < ctf_dict_cnt; i++)
    all_strlens += ctf_dicts[i]->ctf_str[0].cts_len
      + ctf_dicts[i]->ctf_str_prov_len;

  /* If linking, deduplicate strings against the children in every dict that has
     any.  (String deduplication is not yet implemented for non-linked dicts.)  */
  for (i = 0; i < ctf_dict_cnt; i++)
    if (ctf_dicts[i]->ctf_flags & LCTF_LINKING && ctf_dicts[i]->ctf_link_outputs)
      {
	old_parent_strlen = ctf_dicts[i]->ctf_str[0].cts_len
	  + ctf_dicts[i]->ctf_str_prov_len;

	if (ctf_dedup_strings (ctf_dicts[i]) < 0)
	  goto err;

	ctf_dprintf ("Deduplicated strings in archive member %zi: "
		     "original parent strlen: %zu; original lengths: %zu; "
		     "final length: %zu.\n", i, (size_t) old_parent_strlen,
		     (size_t) all_strlens,
		     (size_t) ctf_dicts[i]->ctf_str_prov_len);
      }

  return 0;

 err:
  err = ctf_errno (ctf_dicts[i]);
  ctf_err_copy (ctf_dicts[0], ctf_dicts[i]);
  for (i--; i >= 0; i--)
    ctf_depreserialize (ctf_dicts[i]);
  return err;
}

/* Write out a CTF archive to the start of the file referenced by the passed-in
   fd.  The entries in CTF_DICTS are referenced by name: the names are passed in
   the names array, which must have CTF_DICTS entries.

   Returns 0 on success, or an errno, or an ECTF_* value.  */
ctf_error_t
ctf_arc_write_fd (int fd, ctf_dict_t **ctf_dicts, size_t ctf_dict_cnt,
		  const char **names, size_t threshold)
{
  const char *errmsg;
  struct ctf_archive *archdr;
  size_t i;
  char dummy = 0;
  size_t headersz;
  ssize_t namesz;
  size_t ctf_startoffs;		/* Start of the section we are working over.  */
  char *nametbl = NULL;		/* The name table.  */
  int multiple_parents = 0;
  const char *known_parent = NULL;
  ctf_error_t err;
  size_t prop_off = 0;
  ctf_archive_modent_t *modent;

  /* Prepare by serializing everything.  Done first because it allocates a lot
     of space and thus is more likely to fail.  */
  if (ctf_dict_cnt > 0 &&
      (err = ctf_arc_preserialize (ctf_dicts, ctf_dict_cnt, threshold)) < 0)
    return err;

  ctf_dprintf ("Writing CTF archive with %lu files\n",
	       (unsigned long) ctf_dict_cnt);

  /* Figure out the size of the mmap()ed header, including the
     ctf_archive_modent array immediately following the header itself.  We
     assume that all of this needs no padding: a likely assumption, given
     that it's all made up of uint64_t's.  */
  headersz = sizeof (struct ctf_archive)
    + (ctf_dict_cnt * sizeof (uint64_t) * 2);
  ctf_dprintf ("headersz is %lu\n", (unsigned long) headersz);

  /* From now on we work in two pieces: an mmap()ed region from zero up to the
     headersz, and a region updated via write() starting after that, containing
     all the tables.  Platforms that do not support mmap() just use write().  */
  ctf_startoffs = headersz;
  if (lseek (fd, ctf_startoffs - 1, SEEK_SET) < 0)
    {
      errmsg = N_("ctf_arc_write(): cannot extend file while writing");
      goto err;
    }

  if (write (fd, &dummy, 1) < 0)
    {
      errmsg = N_("ctf_arc_write(): cannot extend file while writing");
      goto err;
    }

  if ((archdr = arc_mmap_header (fd, headersz)) == NULL)
    {
      errmsg = N_("ctf_arc_write(): cannot mmap");
      goto err;
    }

  /* Fill in everything we can, which is everything other than the name
     table and shared properties table offsets.  */
  archdr->magic = CTFA_MAGIC;
  archdr->ndicts = ctf_dict_cnt;
  archdr->ctfs = ctf_startoffs;
  archdr->nprops = 0;			/* Updated later.  */
  archdr->propents = 0;			/* Updated later.  */
  archdr->modents = sizeof (struct ctf_archive);

  /* We could validate that all CTF files have the same data model, but
     since any reasonable construction process will be building things of
     only one bitness anyway, this is pretty pointless, so just use the
     model of the first CTF file for all of them.  (It *is* valid to
     create an empty archive: the value of model is irrelevant in
     this case, but we must be sure not to dereference uninitialized
     memory.)  */

  if (ctf_dict_cnt > 0)
    archdr->model = ctf_getmodel (ctf_dicts[0]);

  /* Now write out the CTFs: ctf_archive_modent array via the mapping,
     ctfs via write().  The names themselves have not been written yet: we
     track them in a local strtab until the time is right, and sort the
     modents array after construction.

     We also keep track of the parent of each dict, so we can easily tell if
     all dicts have a consistent parent (or none).  (This is usually the
     case, e.g. for all archives resulting from a ctf_link() operation.)

     The name table is not sorted.  */

  for (i = 0, namesz = 0; i < archdr->ndicts; i++)
    namesz += strlen (names[i]) + 1;

  nametbl = malloc (namesz);
  if (nametbl == NULL)
    {
      errmsg = N_("ctf_arc_write(): error writing named CTF to archive");
      goto err_unmap;
    }

  for (i = 0, namesz = 0,
       modent = (ctf_archive_modent_t *) ((char *) archdr
					  + sizeof (struct ctf_archive));
       i < archdr->ndicts; i++)
    {
      off_t off;

      strcpy (&nametbl[namesz], names[i]);

      off = arc_write_one_ctf (ctf_dicts[i], fd, threshold);
      if (off == -1)
	{
	  errmsg = N_("ctf_arc_write(): cannot write CTF file to archive");
	  goto err_free;
	}

      modent->name = namesz;
      modent->contents = off - ctf_startoffs;
      namesz += strlen (names[i]) + 1;
      modent++;

      if (!multiple_parents)
	{
	  if (!known_parent)
	    known_parent = ctf_dicts[i]->ctf_parent_name;
	  else
	    if (ctf_dicts[i]->ctf_parent_name
		&& strcmp (known_parent, ctf_dicts[i]->ctf_parent_name) != 0)
	      multiple_parents = 1;
	}
    }

  ctf_qsort_r ((ctf_archive_modent_t *) ((char *) archdr
					 + sizeof (struct ctf_archive)),
	       archdr->ndicts,
	       sizeof (ctf_archive_modent_t), sort_modent_by_name,
	       nametbl);

  /* Properties.  We have only one property so far, defined only if we have
     exactly one parent in common across all dicts.  */
  if (!multiple_parents && known_parent)
    {
      char *new_nametbl;

      archdr->nprops++;
      if ((new_nametbl = realloc (nametbl, namesz
				  + strlen ("parent_name") + 1)) == NULL)
	{
	  errmsg = N_("ctf_arc_write(): error allocating properties");
	  goto err_free;
	}
      nametbl = new_nametbl;
      strcpy (&nametbl[namesz], "parent_name");
      prop_off = namesz;
      namesz += strlen ("parent_name") + 1;
    }

   /* Now the name table.  */

  if (ctf_arc_value_write (fd, nametbl, namesz, &archdr->names) < 0)
    {
      errmsg = N_("ctf_arc_write(): cannot write name table to archive");
      goto err_free;
    }
  free (nametbl);

  /* Now the properties.  There is only one currently: the parent name,
     which means the sorted nature of the name table is trivial.  (When
     multiple properties are written out, only the last will be
     aligned.)  */

  if (!multiple_parents && known_parent)
    {
      ctf_archive_modent_t props;
      off_t prop_values;
      uint64_t propents;

      memset (&props, 0, sizeof (ctf_archive_modent_t));

      props.name = prop_off;
      props.contents = 0;

      if ((prop_values = arc_write_one (fd, known_parent,
					strlen (known_parent) + 1, 1)) < 0
	  || (ctf_arc_value_write (fd, &props, sizeof (ctf_archive_modent_t),
				   &propents) < 0))
	{
	  /* Something went wrong: just blank out the props and keep going.  */
	  archdr->prop_values = 0;
	  archdr->propents = 0;
	}
      else
	{
	  archdr->prop_values = prop_values;
	  archdr->propents = propents;
	}
    }

  if (arc_mmap_writeout (fd, archdr, headersz, &errmsg) < 0)
    goto err_unmap;
  if (arc_mmap_unmap (archdr, headersz, &errmsg) < 0)
    goto err;
  return 0;

err_free:
  free (nametbl);
err_unmap:
  arc_mmap_unmap (archdr, headersz, NULL);
err:
  /* We report errors into the first file in the archive, if any: if this is a
     zero-file archive, put it in the open-errors stream for lack of anywhere
     else for it to go.  */
  ctf_err_warn (ctf_dict_cnt > 0 ? ctf_dicts[0] : NULL, 0, errno, "%s",
		gettext (errmsg));
  return errno;
}

/* Write out a CTF archive.  The entries in CTF_DICTS are referenced by name:
   the names are passed in the names array, which must have CTF_DICTS entries.

   If the filename is NULL, create a temporary file and return a pointer to it.

   Returns 0 on success, or an errno, or an ECTF_* value.  */
ctf_error_t
ctf_arc_write (const char *file, ctf_dict_t **ctf_dicts, size_t ctf_dict_cnt,
	       const char **names, size_t threshold)
{
  ctf_error_t err;
  int fd;

  if ((fd = open (file, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0666)) < 0)
    {
      ctf_err_warn (ctf_dict_cnt > 0 ? ctf_dicts[0] : NULL, 0, errno,
		    _("ctf_arc_write(): cannot create %s"), file);
      return errno;
    }

  err = ctf_arc_write_fd (fd, ctf_dicts, ctf_dict_cnt, names, threshold);
  if (err)
    goto err_close;

  if ((err = close (fd)) < 0)
    ctf_err_warn (ctf_dict_cnt > 0 ? ctf_dicts[0] : NULL, 0, errno,
		  _("ctf_arc_write(): cannot close after writing to archive"));
  goto err;

 err_close:
  (void) close (fd);
 err:
  if (err < 0)
    unlink (file);

  return err;
}

/* Write one CTF dict out.  Return the file position of the written file (or
   rather, of the file-size uint64_t that precedes it): negative return is a
   negative errno or ctf_errno value.  On error, the file position may no
   longer be at the end of the file, but if it is, it will be at an 8-byte
   aligned offset.  Sets errno on error.  */
static off_t
arc_write_one_ctf (ctf_dict_t *fp, int fd, size_t threshold)
{
  uint64_t off, ctfsz = 0;
  off_t end_off;

  /* This zero write turns into the size in a moment...  */
  if (ctf_arc_value_write (fd, &ctfsz, sizeof (ctfsz), &off) < 0)
    return -1;

  if (ctf_write_thresholded (fp, fd, threshold) != 0)
    {
      errno = fp->ctf_errno;
      return -1;
    }

  if ((end_off = lseek (fd, 0, SEEK_CUR)) < 0)
    return -1;
  ctfsz = end_off - (off + sizeof (ctfsz));

  if ((lseek (fd, off, SEEK_SET)) < 0)
    return -1;

  /* ... here.  */
  if (ctf_arc_value_write (fd, &ctfsz, sizeof (ctfsz), &off) < 0)
    return -1;

  /* Align the end byte and move there.  */

  end_off = LCTF_ALIGN_OFFS (end_off, 8);
  if ((lseek (fd, end_off, SEEK_SET)) < 0)
    return -1;

  return (off_t) off;
}

/* Write one size-prepended thing out.  On error, the file position may no
   longer be at the end of the file.  Sets errno on error.  */

static off_t
arc_write_one (int fd, const void *item, size_t size, int align)
{
  uint64_t sz64 = (uint64_t) size;
  uint64_t off;

  if (ctf_arc_value_write (fd, &sz64, sizeof (sz64), &off) < 0)
    return -1;

  if (ctf_arc_value_write (fd, item, size, NULL) < 0)
    return -1;

  if (align)
    {
      off_t end_off;

      if ((end_off = lseek (fd, 0, SEEK_CUR)) < 0)
	return -1;

      end_off = LCTF_ALIGN_OFFS (end_off, 8);
      if ((lseek (fd, end_off, SEEK_SET)) < 0)
	return -1;
    }

  return (off_t) off;
}

/* Write out one value in the archive.  Sets errno and returns -1 on
   error.  */

static int
ctf_arc_value_write (int fd, const void *tbl_, size_t tblsz,
		     uint64_t *start_off)
{
  off_t off;
  const unsigned char *tbl = (const unsigned char *) tbl_;

  if ((off = lseek (fd, 0, SEEK_CUR)) < 0)
    return -1;

  if (start_off)
    *start_off = off;

  while (tblsz > 0)
    {
      ssize_t len;

      if ((len = write (fd, tbl, tblsz)) < 0)
	{
	  *start_off = 0;
	  return -1;
	}
      tblsz -= len;
      tbl += len;
    }
  return 0;
}

/* qsort() function to sort the array of ctf_archive_modents_t into
   ascending name order.  */
static int
sort_modent_by_name (const void *one, const void *two, void *n)
{
  const ctf_archive_modent_t *a = one;
  const ctf_archive_modent_t *b = two;
  char *nametbl = n;

  return strcmp (&nametbl[a->name], &nametbl[b->name]);
}

/* bsearch_r() function to search for a given name in the sorted array of struct
   ctf_archive_modents.  */
static int
search_modent_by_name (const void *key, const void *ent, void *arg)
{
  const char *k = key;
  const ctf_archive_modent_t *v = ent;
  const struct ctf_archive_internal *arci;
  const char *search_nametbl;

  arci = (const struct ctf_archive_internal *) arg;

  search_nametbl = (char *) arci->ctfi_archive + arci->ctfi_hdr->names;
  return strcmp (k, &search_nametbl[v->name]);
}

/* Byteswap an archive (but not its members) if necessary.  After this,
   the entire archive is in native-endian byte order.  */

static int
ctf_arc_flip_archive (struct ctf_archive_internal *arci, size_t arc_len,
		      ctf_error_t *errp)
{
  struct ctf_archive *hdr = arci->ctfi_hdr;
  int needs_flipping = 0;
  ctf_archive_modent_t *modent;
  unsigned char *arc_bytes = arci->ctfi_archive;
  unsigned char *ents;

  if (bswap_64 (hdr->magic) == CTFA_MAGIC)
    needs_flipping = 1;
  else if (arci->ctfi_archive_v1
	   && bswap_64 (hdr->magic) == CTFA_V1_MAGIC)
    needs_flipping = 1;

  if (!needs_flipping)
    return 0;

  /* Headers.  Some headers are v2-only.  The layout has already been
     adjusted to be v2-compatible.  */

  swap_thing (hdr->magic);
  swap_thing (hdr->model);
  swap_thing (hdr->ndicts);
  swap_thing (hdr->names);
  swap_thing (hdr->ctfs);

  if (!arci->ctfi_archive_v1)
    {
      swap_thing (hdr->nprops);
      swap_thing (hdr->prop_values);
      swap_thing (hdr->modents);
      swap_thing (hdr->propents);
    }

  /* Swap the tables and the sizes of things therein.

     modents for v1 is populated by ctf_new_archive_internal, below.

     We must range-check first to be sure that the modent arrays are not out
     of range.  */

  if (ctf_arc_range_check_hdr (arci, arc_len, errp) < 0)
    return -1;					/* errp is set for us.  */

  modent = (ctf_archive_modent_t *) (arc_bytes + arci->ctfi_hdr->modents);
  ents = (unsigned char *) (arc_bytes + arci->ctfi_hdr->ctfs);
  if (ctf_arc_flip_modents (modent, arci->ctfi_hdr->ndicts, ents,
			    arci->ctfi_hdr->ctfs, arc_len, errp) < 0)
    return -1;					/* errp is set for us.  */

  modent = (ctf_archive_modent_t *) (arc_bytes + arci->ctfi_hdr->propents);
  ents = (unsigned char *) (arc_bytes + arci->ctfi_hdr->prop_values);
  if (ctf_arc_flip_modents (modent, arci->ctfi_hdr->nprops, ents,
			    arci->ctfi_hdr->prop_values, arc_len, errp) < 0)
    return -1;					/* errp is set for us.  */

  return 0;
}

/* Byteswap a modent table with offsets rooted at BASE, including the size
   entries preceding the elements themselves.  */

static int
ctf_arc_flip_modents (ctf_archive_modent_t *modent, uint64_t els,
		      unsigned char *ents, uint64_t base, size_t arc_len,
		      ctf_error_t *errp)
{
  uint64_t i;

  for (i = 0; i < els; i++)
    {
      uint64_t *ctf_size;

      swap_thing (modent[i].name);
      swap_thing (modent[i].contents);

      if (base + modent[i].contents + sizeof (uint64_t) > arc_len)
	{
	  ctf_err_warn (NULL, 0, EOVERFLOW,
			"CTF archive overflow in content offset for member %" PRIu64
		       " of %zi + %zi", i, base, modent[i].contents);
	  ctf_set_open_errno (errp, EOVERFLOW);
	  return -1;
	}

      ctf_size = (uint64_t *) (ents + modent[i].contents);
      swap_thing (*ctf_size);
    }

  return 0;
}

/* Range- and overlap-check the archive header.  At this stage, only the
   modent and name offsets, and overlaps of the starts of tables, are
   checkable.  The rest gets checked further down, in
   ctf_arc_range_check.  */

static int
ctf_arc_range_check_hdr (struct ctf_archive_internal *arci, size_t arc_len,
			 ctf_error_t *errp)
{
  const char *err;
  uint64_t ndict_end, nprop_end;

  ndict_end = arci->ctfi_hdr->modents +
    (sizeof (ctf_archive_modent_t) * arci->ctfi_hdr->ndicts);

  if (ndict_end > arc_len)
    {
      ctf_err_warn (NULL, 0, EOVERFLOW, "CTF archive overflow: archive is %zi bytes, but ctfs end at %zi + (%zi * %zi) = %zi",
		   arc_len, arci->ctfi_hdr->modents,
		   sizeof (ctf_archive_modent_t),
		   arci->ctfi_hdr->ndicts, ndict_end);

      ctf_set_open_errno (errp, EOVERFLOW);
      return -1;
    }

  if ((arci->ctfi_hdr->modents < arci->ctfi_hdr->names
       && ndict_end > arci->ctfi_hdr->names)
      || (arci->ctfi_hdr->modents < arci->ctfi_hdr->ctfs
	  && ndict_end > arci->ctfi_hdr->ctfs)
      || (arci->ctfi_hdr->modents < arci->ctfi_hdr->prop_values
	  && ndict_end > arci->ctfi_hdr->prop_values)
      || (arci->ctfi_hdr->modents < arci->ctfi_hdr->propents
	  && ndict_end > arci->ctfi_hdr->propents)
      || arci->ctfi_hdr->names == arci->ctfi_hdr->ctfs
      || (arci->ctfi_hdr->names == arci->ctfi_hdr->prop_values
	  && arci->ctfi_hdr->prop_values != 0)
      || arci->ctfi_hdr->names == arci->ctfi_hdr->modents
      || (arci->ctfi_hdr->names == arci->ctfi_hdr->propents
	  && arci->ctfi_hdr->propents != 0)
      || (arci->ctfi_hdr->ctfs == arci->ctfi_hdr->prop_values
	  && arci->ctfi_hdr->prop_values != 0)
      || arci->ctfi_hdr->ctfs == arci->ctfi_hdr->modents
      || (arci->ctfi_hdr->ctfs == arci->ctfi_hdr->propents
	  && arci->ctfi_hdr->propents != 0)
      || (arci->ctfi_hdr->prop_values != 0
	  && arci->ctfi_hdr->prop_values == arci->ctfi_hdr->modents
	  && arci->ctfi_hdr->prop_values == arci->ctfi_hdr->propents)
      || (arci->ctfi_hdr->propents != 0
	  && arci->ctfi_hdr->modents == arci->ctfi_hdr->propents))
    {
      err = "ctf table";
      goto err;
    }

  nprop_end = arci->ctfi_hdr->propents +
    (sizeof (ctf_archive_modent_t) * arci->ctfi_hdr->nprops);

  if (nprop_end > arc_len)
    {
      err = "props table";
      goto err;
    }

  if (arci->ctfi_hdr->names > arc_len)
    {
      err = "name table";
      goto err;
    }

  if (arci->ctfi_hdr->ctfs > arc_len)
    {
      err = "member table";
      goto err;
    }

  return 0;

 err:
  ctf_err_warn (NULL, 0, EOVERFLOW, "CTF archive overflow: overlapping %s in archive",
	       err);
  ctf_set_open_errno (errp, EOVERFLOW);
  return -1;
}

/* Range check the archive modent tables.  By this stage the tables are all
   in native endianness.  */

static int
ctf_arc_range_check (struct ctf_archive_internal *arci, size_t arc_len,
		     ctf_error_t *errp)
{
  unsigned char *arc_bytes = arci->ctfi_archive;
  ctf_archive_modent_t *modents;

  if (ctf_arc_range_check_hdr (arci, arc_len, errp) < 0)
    return -1;					/* errno is set for us.  */

  modents = (ctf_archive_modent_t *) (arc_bytes + arci->ctfi_hdr->modents);

  if (ctf_arc_range_check_modents (modents, arci->ctfi_hdr, arc_bytes,
				   arci->ctfi_hdr->ctfs,
				   arci->ctfi_hdr->ndicts, arc_len,
				   arci->ctfi_archive_v1, errp) < 0)
    return -1;					/* errno is set for us.  */

  modents = (ctf_archive_modent_t *) (arc_bytes + arci->ctfi_hdr->propents);

  if (ctf_arc_range_check_modents (modents, arci->ctfi_hdr, arc_bytes,
				   arci->ctfi_hdr->prop_values,
				   arci->ctfi_hdr->nprops, arc_len,
				   0, errp) < 0)
    return -1;					/* errno is set for us.  */

  return 0;
}

/* Find the closest section to BASE that is located after it.  If none, the
   archive length is returned.  */

static uint64_t
ctf_arc_closest_section (struct ctf_archive *arc_hdr, uint64_t base,
			 size_t arc_len)
{
  uint64_t closest = arc_len;

  if (arc_hdr->names > base
      && arc_hdr->ctfs < closest)
    closest = arc_hdr->names;

  if (arc_hdr->ctfs > base
      && arc_hdr->ctfs < closest)
    closest = arc_hdr->ctfs;

  if (arc_hdr->prop_values > base
      && arc_hdr->prop_values < closest)
    closest = arc_hdr->prop_values;

  if (arc_hdr->modents > base
      && arc_hdr->modents < closest)
    closest = arc_hdr->modents;

  if (arc_hdr->propents > base
      && arc_hdr->propents < closest)
    closest = arc_hdr->propents;

  return closest;
}

/* Range-check a single modent array.  */

static int
ctf_arc_range_check_modents (ctf_archive_modent_t *modent,
			     struct ctf_archive *arc_hdr,
			     unsigned char *arc_bytes,
			     uint64_t contents_base, size_t contents_els,
			     size_t arc_len, int fixup_v1, ctf_error_t *errp)
{
  uint64_t i;
  char *names = (char *) arc_bytes + arc_hdr->names;
  uint64_t name_base = arc_hdr->names;
  unsigned char *contents = (unsigned char *) arc_bytes + contents_base;
  size_t closest_names_offset, closest_contents_offset;

  /* Figure out the offset of the thing that is closest to, but after the
     end of, the names section, or the end of the file if none.  */

  closest_names_offset = arc_len;

  closest_names_offset = ctf_arc_closest_section (arc_hdr, name_base, arc_len);
  closest_contents_offset = ctf_arc_closest_section (arc_hdr, contents_base,
						     arc_len);

  for (i = 0; i < contents_els; i++)
    {
      uint64_t name_off = modent[i].name + name_base;
      uint64_t contents_off = modent[i].contents + contents_base;
      ssize_t space_left;
      uint64_t *contents_size;

      /* We already checked for modent table overflow and overlap, but we
	 cannot check for name table overlap except member-by-member.  We
	 have to check the name offset first to make sure that strlen()ing
	 the string is safe, then check that.  */

      if (name_off > closest_names_offset
	  || (contents_off + sizeof (uint64_t) > closest_contents_offset))
	goto err;

      space_left = closest_names_offset - name_off;

      if (space_left < 0)
	goto err;

      if (strnlen (&names[modent[i].name], space_left)
	  == (size_t) space_left)
	goto err;

      /* Checking the contents offset is simpler: we already checked that
	 the actual size didn't overflow, so now we just need to make sure
	 that the entire dict (or, depending on the call, property value)
	 fits.  */

      contents_size = (uint64_t *) (contents + modent[i].contents);

      /* If this was a v1 archive, the size is actually wrong: it includes
	 the size of the size uint64_t itself, so all archive opens opened
	 one uint64_t too much.  Fix this up, since the values are otherwise
	 valid.  If the ctf_size is zero in v1, ironically, we know this is
	 an underflow! */

      if (fixup_v1)
	{
	  if (*contents_size < sizeof (uint64_t))
	    goto err;
	  *contents_size -= sizeof (uint64_t);
	}

      if (contents_off + sizeof (uint64_t) + *contents_size
	  > closest_contents_offset)
	goto err;
    }

  return 0;

err:
  ctf_err_warn (NULL, 0, EOVERFLOW, "CTF archive overflow: modent array element %" PRIu64
		" overflow/overlap", i);
  ctf_set_open_errno (errp, EOVERFLOW);
  return -1;
}

/* Make a new struct ctf_archive_internal wrapper for a ctf_archive or a
   ctf_dict: endian-swap the archive header as necessary, and check all its
   offsets for validity.  Close ARC and/or FP on error.  Arrange to free or
   unmap the SYMSECT or STRSECT, as needed, on close.  */

struct ctf_archive_internal *
ctf_new_archive_internal (int is_archive, int is_v1, int unmap_on_close,
			  struct ctf_archive *arc, size_t arc_len,
			  ctf_dict_t *fp, const ctf_sect_t *symsect,
			  const ctf_sect_t *strsect,
			  ctf_error_t *errp)
{
  struct ctf_archive_internal *arci = NULL;

  if ((arci = calloc (1, sizeof (struct ctf_archive_internal))) == NULL)
    goto err;

  ctf_set_open_errno (errp, 0);

  arci->ctfi_is_archive = is_archive;
  arci->ctfi_archive_v1 = is_v1;

  if (is_archive)
    {
      arci->ctfi_archive = (unsigned char *) arc;
      if (is_v1)
	arci->ctfi_hdr_len = sizeof (struct ctf_archive_v1);
      else
	arci->ctfi_hdr_len = sizeof (struct ctf_archive);

      if (arc_len < arci->ctfi_hdr_len)
	{
	  errno = EOVERFLOW;
	  ctf_err_warn (NULL, 0, EOVERFLOW, "CTF archive underflow: archive is %zi bytes, shorter than the archive header length of %zi bytes\n",
			arc_len, arci->ctfi_hdr_len);
	  goto err;
	}

      if ((arci->ctfi_hdr = malloc (sizeof (struct ctf_archive))) == NULL)
	goto err;

      /* Upgrade the v1 header if needed.  */

      if (!is_v1)
	memcpy (arci->ctfi_hdr, arc, sizeof (struct ctf_archive));
      else
	{
	  struct ctf_archive_v1 *v1hdr = (struct ctf_archive_v1 *) arc;

	  memset (arci->ctfi_hdr, 0, sizeof (struct ctf_archive));
	  arci->ctfi_hdr->magic = v1hdr->magic;
	  arci->ctfi_hdr->model = v1hdr->model;
	  arci->ctfi_hdr->ndicts = v1hdr->ndicts;
	  arci->ctfi_hdr->names = v1hdr->names;
	  arci->ctfi_hdr->ctfs = v1hdr->ctfs;
	  arci->ctfi_hdr->modents = sizeof (struct ctf_archive_v1);
	}

      if (ctf_arc_flip_archive (arci, arc_len, errp) < 0)
	goto err_set;

      if (ctf_arc_range_check (arci, arc_len, errp) < 0)
	goto err_set;
    }
  else
    arci->ctfi_dict = fp;
  if (symsect)
     memcpy (&arci->ctfi_symsect, symsect, sizeof (struct ctf_sect));
  if (strsect)
     memcpy (&arci->ctfi_strsect, strsect, sizeof (struct ctf_sect));
  arci->ctfi_free_symsect = 0;
  arci->ctfi_free_strsect = 0;
  arci->ctfi_unmap_on_close = unmap_on_close;
  arci->ctfi_symsect_little_endian = -1;

  return arci;

 err:
  ctf_set_open_errno (errp, errno);
 err_set:
  if (is_archive)
    {
      if (unmap_on_close)
	ctf_arc_close_internal (arc);
    }
  else
    ctf_dict_close (fp);
  if (arci)
    {
      free (arci->ctfi_hdr);
      free (arci);
    }
  return NULL;
}

/* Set the symbol-table endianness of an archive (defaulting the symtab
   endianness of all ctf_file_t's opened from that archive).  */
void
ctf_arc_symsect_endianness (struct ctf_archive_internal *arci, int little_endian)
{
  arci->ctfi_symsect_little_endian = !!little_endian;
  if (!arci->ctfi_is_archive)
    ctf_symsect_endianness (arci->ctfi_dict, arci->ctfi_symsect_little_endian);
}

/* Get the CTF preamble from data in a buffer, which may be either an archive or
   a CTF dict.  If multiple dicts are present in an archive, the preamble comes
   from an arbitrary dict.  The preamble is a pointer into the ctfsect passed
   in.  Returns NULL if called on non-v1 archives.  (Backward-compatibility
   only.)  */

const ctf_preamble_t *
ctf_arc_bufpreamble_v1 (const ctf_sect_t *ctfsect)
{
  if (ctfsect->cts_data == NULL
      || ctfsect->cts_size <= sizeof (uint64_t))
    {
      errno = EOVERFLOW;
      return NULL;
    }

  if (le64toh ((*(uint64_t *) ctfsect->cts_data)) == CTFA_V1_MAGIC)
    {
      struct ctf_archive_v1 *arc = (struct ctf_archive_v1 *) ctfsect->cts_data;
      return (const ctf_preamble_t *) ((char *) arc + le64toh (arc->ctfs)
				       + sizeof (uint64_t));
    }

  return NULL;
}

/* Open a CTF archive or dictionary from data in a buffer (which the caller must
   preserve until ctf_arc_close() time).  Returns the archive, or NULL and an
   error in *err (if not NULL).  */
ctf_archive_t *
ctf_arc_bufopen (const ctf_sect_t *ctfsect, const ctf_sect_t *symsect,
		 const ctf_sect_t *strsect, ctf_error_t *errp)
{
  struct ctf_archive *arc = NULL;
  int is_archive, is_v1 = 0;
  ctf_dict_t *fp = NULL;

  if (ctfsect->cts_data != NULL
      && ctfsect->cts_size > sizeof (uint64_t)
      && (*(uint64_t *) ctfsect->cts_data == CTFA_MAGIC
	  || bswap_64 ((*(uint64_t *) ctfsect->cts_data)) == CTFA_MAGIC
	  || le64toh ((*(uint64_t *) ctfsect->cts_data)) == CTFA_V1_MAGIC))
    {
      /* The archive is mmappable, so this operation is trivial.

	 This buffer is nonmodifiable, so the trick involving mmapping only
	 part of it and storing the length in the magic number is not
	 applicable: so record this fact in the archive-wrapper header.  (We
	 cannot record it in the archive, because the archive may very well
	 be a read-only mapping.  unmap_on_close is left unset in the
	 ctf_new_archive_internal call so that we don't try to unmap it
	 ourselves.)  */

      is_archive = 1;
      arc = (struct ctf_archive *) ctfsect->cts_data;

      if (le64toh ((*(uint64_t *) ctfsect->cts_data)) == CTFA_V1_MAGIC)
	is_v1 = 1;
    }
  else
    {
      is_archive = 0;
      if ((fp = ctf_bufopen (ctfsect, symsect, strsect, errp)) == NULL)
	{
	  ctf_err_warn (NULL, 0, *errp, _("ctf_arc_bufopen(): cannot open CTF"));
	  return NULL;
	}
    }
  return ctf_new_archive_internal (is_archive, is_v1, 0, arc, ctfsect->cts_size,
				   fp, symsect, strsect, errp);
}

/* Open a CTF archive from a given file.  Returns the archive (wrapper), or
   NULL and an error in *err (if not NULL).  Only archives are supported,
   not dicts (because our only caller already handles those itself).  */
struct ctf_archive_internal *
ctf_arc_open_internal (const char *filename, ctf_error_t *errp)
{
  const char *errmsg;
  int fd, is_v1 = 0;
  struct stat s;
  struct ctf_archive *arc;		/* (Actually the whole file.)  */

  libctf_init_debug();
  if ((fd = open (filename, O_RDONLY)) < 0)
    {
      errmsg = N_("ctf_arc_open(): cannot open %s");
      goto err;
    }
  if (fstat (fd, &s) < 0)
    {
      errmsg = N_("ctf_arc_open(): cannot stat %s");
      goto err_close;
    }

  /* This will fail if the file is too big -- e.g. > 4GiB on 32-bit
     platforms -- and thus free us from having to do integer-overflow checks
     elsewhere.  */
  if ((arc = arc_mmap_file (fd, s.st_size)) == NULL)
    {
      errmsg = N_("ctf_arc_open(): cannot read in %s");
      goto err_close;
    }

  if (arc->magic != CTFA_MAGIC && bswap_64 (arc->magic) != CTFA_MAGIC
      && le64toh (arc->magic) != CTFA_V1_MAGIC)
    {
      errmsg = N_("ctf_arc_open(): %s: invalid magic number");
      errno = ECTF_FMT;
      goto err_unmap;
    }

  if (le64toh (arc->magic) == CTFA_V1_MAGIC)
    is_v1 = 1;

  /* This horrible hack lets us know how much to unmap when the file is
     closed.  (We no longer need the magic number, and the mapping
     is private.)  */
  arc->magic = s.st_size;
  close (fd);

  return ctf_new_archive_internal (1, is_v1, 1, arc, s.st_size, NULL,
				   NULL, NULL, errp);

err_unmap:
  arc_mmap_unmap (arc, s.st_size, NULL);
err_close:
  close (fd);
err:
  if (errp)
    *errp = errno;
  ctf_err_warn (NULL, 0, errno, gettext (errmsg), filename);
  return NULL;
}

/* Close an archive.  */
static void
ctf_arc_close_internal (struct ctf_archive *arc)
{
  if (arc == NULL)
    return;

  /* See the comment in ctf_arc_open().  */
  arc_mmap_unmap (arc, arc->magic, NULL);
}

/* Public entry point: close an archive (via its wrapper), or CTF dict.  */
void
ctf_arc_close (struct ctf_archive_internal *arci)
{
  if (arci == NULL)
    return;

  if (arci->ctfi_is_archive)
    {
      if (arci->ctfi_unmap_on_close)
	ctf_arc_close_internal ((struct ctf_archive *) arci->ctfi_archive);
    }
  else
    ctf_dict_close (arci->ctfi_dict);
  free (arci->ctfi_hdr);
  free (arci->ctfi_symdicts);
  free (arci->ctfi_symnamedicts);
  ctf_dynhash_destroy (arci->ctfi_dicts);
  if (arci->ctfi_free_symsect)
    free ((void *) arci->ctfi_symsect.cts_data);
  if (arci->ctfi_free_strsect)
    free ((void *) arci->ctfi_strsect.cts_data);
  free (arci->ctfi_data);
  if (arci->ctfi_bfd_close)
    arci->ctfi_bfd_close (arci);
  free (arci);
}

/* Return the ctf_dict_t with the given name, or NULL if none, setting 'err' if
   non-NULL.  A name of NULL means to open the default file.  */
static ctf_dict_t *
ctf_dict_open_internal (const struct ctf_archive_internal *arci,
			const ctf_sect_t *symsect,
			const ctf_sect_t *strsect,
			const char *name, int little_endian_symtab,
			ctf_error_t *errp)
{
  ctf_archive_modent_t *modent;

  if (name == NULL)
    name = _CTF_SECTION;		 /* The default name.  */

  ctf_dprintf ("ctf_dict_open_internal(%s): opening\n", name);

  modent = (ctf_archive_modent_t *) (arci->ctfi_archive
				     + arci->ctfi_hdr->modents);

  modent = bsearch_r (name, modent, arci->ctfi_hdr->ndicts,
		      sizeof (ctf_archive_modent_t),
		      search_modent_by_name, (void *) arci);

  /* This is actually a common case and normal operation: no error
     debug output.  */
  if (modent == NULL)
    {
      if (errp)
	*errp = ECTF_ARNNAME;
      return NULL;
    }

  return ctf_dict_open_by_offset (arci, symsect, strsect, modent->contents,
				  little_endian_symtab, errp);
}

/* Return the ctf_dict_t with the given name, or NULL if none, setting 'err' if
   non-NULL.  A name of NULL means to open the default file.

   Public entry point.  */
ctf_dict_t *
ctf_dict_open (const struct ctf_archive_internal *arci, const char *name,
	       ctf_error_t *errp)
{
  const ctf_sect_t *symsect = &arci->ctfi_symsect;
  const ctf_sect_t *strsect = &arci->ctfi_strsect;

  if (errp)
    *errp = 0;

  if (symsect->cts_name == NULL)
    symsect = NULL;
  if (strsect->cts_name == NULL)
    strsect = NULL;

  if (arci->ctfi_is_archive)
    {
      ctf_dict_t *fp;
      fp = ctf_dict_open_internal (arci, symsect, strsect, name,
				   arci->ctfi_symsect_little_endian,
				   errp);
      if (fp)
	{
	  fp->ctf_archive = (struct ctf_archive_internal *) arci;
	  if (ctf_arc_import_parent (arci, fp, errp) < 0)
	    {
	      ctf_dict_close (fp);
	      return NULL;
	    }
	}
      return fp;
    }

  if ((name != NULL) && (strcmp (name, _CTF_SECTION) != 0))
    {
      if (errp)
	*errp = ECTF_ARNNAME;
      return NULL;
    }
  arci->ctfi_dict->ctf_archive = (struct ctf_archive_internal *) arci;

  /* Bump the refcount so that the user can ctf_dict_close() it.  */
  arci->ctfi_dict->ctf_refcnt++;
  return arci->ctfi_dict;
}

static void
ctf_cached_dict_close (void *fp)
{
  ctf_dict_close ((ctf_dict_t *) fp);
}

/* Return the ctf_dict_t with the given name and cache it in the archive's
   ctfi_dicts.  If this is the first cached dict, designate it the
   crossdict_cache.  */
static ctf_dict_t *
ctf_dict_open_cached (struct ctf_archive_internal *arci, const char *name,
		      ctf_error_t *errp)
{
  ctf_dict_t *fp;
  char *dupname = NULL;

  /* Just return from the cache if possible.  */
  if (arci->ctfi_dicts
      && ((fp = ctf_dynhash_lookup (arci->ctfi_dicts, name)) != NULL))
    {
      fp->ctf_refcnt++;
      return fp;
    }

  /* Not yet cached: open it.  */
  if ((fp = ctf_dict_open (arci, name, errp)) == NULL)
    goto err;

  if ((dupname = strdup (name)) == NULL)
    goto oom;

  if (arci->ctfi_dicts == NULL)
    if ((arci->ctfi_dicts
	 = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			       free, ctf_cached_dict_close)) == NULL)
      goto oom;

  if (ctf_dynhash_insert (arci->ctfi_dicts, dupname, fp) < 0)
    goto oom;
  fp->ctf_refcnt++;

  if (arci->ctfi_crossdict_cache == NULL)
    arci->ctfi_crossdict_cache = fp;

  /* If this archive has multiple members, and this is a parent, pretend
     that we have opened at least one child.  This forces type and string
     allocations in the parent to use provisional IDs, permitting you to
     import children into it even if you modify the parent before you import
     any.  */
  if (arci->ctfi_is_archive && arci->ctfi_hdr->ndicts > 1
      && !(fp->ctf_flags & LCTF_CHILD))
    {
      ctf_dprintf ("archived parent: max children bumped.\n");
      fp->ctf_max_children++;
    }

  return fp;

 oom:
  if (errp)
    *errp = ENOMEM;
 err:
  ctf_dict_close (fp);
  free (dupname);
  return NULL;
}

/* Flush any caches the CTF archive may have open.  */
void
ctf_arc_flush_caches (struct ctf_archive_internal *arci)
{
  free (arci->ctfi_symdicts);
  ctf_dynhash_destroy (arci->ctfi_symnamedicts);
  ctf_dynhash_destroy (arci->ctfi_dicts);
  arci->ctfi_symdicts = NULL;
  arci->ctfi_symnamedicts = NULL;
  arci->ctfi_dicts = NULL;
  arci->ctfi_crossdict_cache = NULL;
}

/* Return the ctf_dict_t at the given ctfs-relative offset, or NULL if
   none, setting 'err' if non-NULL.  */
static ctf_dict_t *
ctf_dict_open_by_offset (const struct ctf_archive_internal *arci,
			 const ctf_sect_t *symsect,
			 const ctf_sect_t *strsect, size_t offset,
			 int little_endian_symtab,
			 ctf_error_t *errp)
{
  ctf_sect_t ctfsect;
  ctf_dict_t *fp;

  ctf_dprintf ("ctf_dict_open_by_offset(%lu): opening\n", (unsigned long) offset);

  memset (&ctfsect, 0, sizeof (ctf_sect_t));

  offset += arci->ctfi_hdr->ctfs;

  ctfsect.cts_name = _CTF_SECTION;
  ctfsect.cts_size = *((uint64_t *) (arci->ctfi_archive + offset));
  ctfsect.cts_entsize = 1;
  ctfsect.cts_data = (void *) (arci->ctfi_archive + offset + sizeof (uint64_t));

  fp = ctf_bufopen (&ctfsect, symsect, strsect, errp);
  if (fp)
    {
      ctf_setmodel (fp, arci->ctfi_hdr->model);
      if (little_endian_symtab >= 0)
	ctf_symsect_endianness (fp, little_endian_symtab);
    }
  return fp;
}

/* Backward compatibility.  */
ctf_dict_t *
ctf_arc_open_by_name (const ctf_archive_t *arci, const char *name,
		      int *errp)
{
  return ctf_dict_open (arci, name, (ctf_error_t *) errp);
}

/* Get a property value from the shared properties table of an archive,
   given a name, or NULL.  */

static const char *
ctf_arc_get_property (const struct ctf_archive_internal *arci, const char *prop)
{
  ctf_archive_modent_t *modent;
  uint64_t *size;

  if (!arci->ctfi_archive)
    return NULL;

  if (arci->ctfi_hdr->propents == 0
      || arci->ctfi_hdr->prop_values == 0)
    return NULL;

  modent = (ctf_archive_modent_t *) (arci->ctfi_archive
				     + arci->ctfi_hdr->modents);

  modent = bsearch_r (prop, modent, arci->ctfi_hdr->nprops,
		      sizeof (ctf_archive_modent_t),
		      search_modent_by_name, (void *) arci);

  if (modent == NULL)
    return NULL;

  /* Currently, all property values we use are strings: we can exploit this
     and skip the size, with one special case for no value at all meaning
     the null string.  */

  size = (uint64_t *) (arci->ctfi_archive
		       + arci->ctfi_hdr->propents
		       + modent->contents);
  if (*size == 0)
    return "";

  return (char *) (arci->ctfi_archive
		   + arci->ctfi_hdr->propents
		   + modent->contents
		   + sizeof (uint64_t));
}

/* Import the parent into a ctf archive, if this is a child, the parent is not
   already set, and a suitable archive member exists.  No error is raised if
   this is not possible: this is just a best-effort helper operation to give
   people useful dicts to start with.  */
static int
ctf_arc_import_parent (const struct ctf_archive_internal *arci, ctf_dict_t *fp,
		       ctf_error_t *errp)
{
  if ((fp->ctf_flags & LCTF_CHILD) && !fp->ctf_parent)
    {
      ctf_error_t err = 0;
      ctf_dict_t *parent;
      const char *parent_name = fp->ctf_parent_name;

      /* If no parent is set, and this is an archive, check the parent name
	 in the properties table.  (v1 archives will not have one, but v1
	 archives will only ever contain CTF dicts, which will always have
	 a parent name set if they need one.  This contingency is for BTF,
	 which can only appear in v2 archives.)

	 There may not be a parent name set even in this case: in the
	 (admittedly pathological) case of archives which contain a mix of
	 parents and some BTF children, the parent name will not be set
	 because no consistent name exists.  In this case,
	 ctf_archive_next's callers will simply have to import the parent
	 themselves.  Such archives cannot be created by ctf_link, so
	 callers are only expected to need this in conjunction with
	 special-case archive construction tools, and general-purpose
	 callers don't need to worry.  */

      if (!parent_name && arci->ctfi_archive)
	parent_name = ctf_arc_get_property (arci, "parent_name");

      parent = ctf_dict_open_cached ((ctf_archive_t *) arci, parent_name, &err);
      if (errp)
	*errp = err;

      if (parent)
	{
	  if (ctf_import (fp, parent) < 0)
	    ctf_err_warn (NULL, 1, ctf_errno (fp),
			  "ctf_arc_import_parent: cannot import: %s",
			  ctf_errmsg (ctf_errno (fp)));
	  ctf_dict_close (parent);
	}
      else if (err != ECTF_ARNNAME)
	return -1;				/* errno is set for us.  */
    }
  return 0;
}

/* Return the number of members in an archive.  */
size_t
ctf_archive_count (const struct ctf_archive_internal *arci)
{
  if (!arci->ctfi_is_archive)
    return 1;

  return arci->ctfi_hdr->ndicts;
}

/* Look up a symbol in an archive by name or index (if the name is set, a lookup
   by name is done).  Return the dict in the archive that the symbol is found
   in, and (optionally) the ctf_id_t of the symbol in that dict (so you don't
   have to look it up yourself).  The dict is cached, so repeated lookups are
   nearly free.

   As usual, you should ctf_dict_close() the returned dict once you are done
   with it.

   Returns NULL on error, and an error in errp (if set).  */

static ctf_dict_t *
ctf_arc_lookup_sym_or_name (struct ctf_archive_internal *arci, unsigned long symidx,
			    const char *symname, ctf_id_t *typep, ctf_error_t *errp)
{
  ctf_dict_t *fp;
  void *fpkey;
  ctf_id_t type;

  /* The usual non-archive-transparent-wrapper special case.  */
  if (!arci->ctfi_is_archive)
    {
      if (!symname)
	{
	  if ((type = ctf_lookup_by_symbol (arci->ctfi_dict, symidx)) == CTF_ERR)
	    {
	      if (errp)
		*errp = ctf_errno (arci->ctfi_dict);
	      return NULL;
	    }
	}
      else
	{
	  if ((type = ctf_lookup_by_symbol_name (arci->ctfi_dict,
						 symname)) == CTF_ERR)
	    {
	      if (errp)
		*errp = ctf_errno (arci->ctfi_dict);
	      return NULL;
	    }
	}
      if (typep)
	*typep = type;
      arci->ctfi_dict->ctf_refcnt++;
      return arci->ctfi_dict;
    }

  if (arci->ctfi_symsect.cts_name == NULL
      || arci->ctfi_symsect.cts_data == NULL
      || arci->ctfi_symsect.cts_size == 0
      || arci->ctfi_symsect.cts_entsize == 0)
    {
      if (errp)
	*errp = ECTF_NOSYMTAB;
      return NULL;
    }

  /* Make enough space for all possible symbol indexes, if not already done.  We
     cache the originating dictionary of all symbols.  The dict links are weak,
     to the dictionaries cached in ctfi_dicts: their refcnts are *not* bumped.
     We also cache similar mappings for symbol names: these are ordinary
     dynhashes, with weak links to dicts.  */

  if (!arci->ctfi_symdicts)
    {
      if ((arci->ctfi_symdicts = calloc (arci->ctfi_symsect.cts_size
					 / arci->ctfi_symsect.cts_entsize,
					 sizeof (ctf_dict_t *))) == NULL)
	{
	  if (errp)
	    *errp = ENOMEM;
	  return NULL;
	}
    }
  if (!arci->ctfi_symnamedicts)
    {
      if ((arci->ctfi_symnamedicts = ctf_dynhash_create (ctf_hash_string,
							 ctf_hash_eq_string,
							 free, NULL)) == NULL)
	{
	  if (errp)
	    *errp = ENOMEM;
	  return NULL;
	}
    }

  /* Perhaps the dict in which we found a previous lookup is cached.  If it's
     supposed to be cached but we don't find it, pretend it was always not
     found: this should never happen, but shouldn't be allowed to cause trouble
     if it does.  */

  if ((symname && ctf_dynhash_lookup_kv (arci->ctfi_symnamedicts,
					 symname, NULL, &fpkey))
      || (!symname && arci->ctfi_symdicts[symidx] != NULL))
    {
      if (symname)
	fp = (ctf_dict_t *) fpkey;
      else
	fp = arci->ctfi_symdicts[symidx];

      if (fp == &enosym)
	goto no_sym;

      if (symname)
	{
	  if ((type = ctf_lookup_by_symbol_name (fp, symname)) == CTF_ERR)
	    goto cache_no_sym;
	}
      else
	{
	  if ((type = ctf_lookup_by_symbol (fp, symidx)) == CTF_ERR)
	    goto cache_no_sym;
	}

      if (typep)
	*typep = type;
      fp->ctf_refcnt++;
      return fp;
    }

  /* Not cached: find it and cache it.  We must track open errors ourselves even
     if our caller doesn't, to be able to distinguish no-error end-of-iteration
     from open errors.  */

  ctf_error_t local_err;
  ctf_error_t *local_errp;
  ctf_next_t *i = NULL;
  const char *name;

  if (errp)
    local_errp = errp;
  else
    local_errp = &local_err;

  while ((fp = ctf_archive_next (arci, &i, &name, 0, local_errp)) != NULL)
    {
      if (!symname)
	{
	  if ((type = ctf_lookup_by_symbol (fp, symidx)) != CTF_ERR)
	    arci->ctfi_symdicts[symidx] = fp;
	}
      else
	{
	  if ((type = ctf_lookup_by_symbol_name (fp, symname)) != CTF_ERR)
	    {
	      char *tmp;
	      /* No error checking, as above.  */
	      if ((tmp = strdup (symname)) != NULL)
		ctf_dynhash_insert (arci->ctfi_symnamedicts, tmp, fp);
	    }
	}

      if (type != CTF_ERR)
	{
	  if (typep)
	    *typep = type;
	  ctf_next_destroy (i);
	  return fp;
	}
      if (ctf_errno (fp) != ECTF_NOTYPEDAT)
	{
	  if (errp)
	    *errp = ctf_errno (fp);
	  ctf_dict_close (fp);
	  ctf_next_destroy (i);
	  return NULL;				/* errno is set for us.  */
	}
      ctf_dict_close (fp);
    }
  if (*local_errp != ECTF_NEXT_END)
    return NULL;

  /* Don't leak end-of-iteration to the caller.  */
  *local_errp = 0;

 cache_no_sym:
  if (!symname)
    arci->ctfi_symdicts[symidx] = &enosym;
  else
    {
      char *tmp;

      /* No error checking: if caching fails, there is only a slight performance
	 impact.  */
      if ((tmp = strdup (symname)) != NULL)
	if (ctf_dynhash_insert (arci->ctfi_symnamedicts, tmp, &enosym) < 0)
	  free (tmp);
    }

 no_sym:
  if (errp)
    *errp = ECTF_NOTYPEDAT;
  if (typep)
    *typep = CTF_ERR;
  return NULL;
}

/* The public API for looking up a symbol by index.  */
ctf_dict_t *
ctf_arc_lookup_symbol (struct ctf_archive_internal *arci, unsigned long symidx,
		       ctf_id_t *typep, ctf_error_t *errp)
{
  return ctf_arc_lookup_sym_or_name (arci, symidx, NULL, typep, errp);
}

/* The public API for looking up a symbol by name. */

ctf_dict_t *
ctf_arc_lookup_symbol_name (struct ctf_archive_internal *arci, const char *symname,
			    ctf_id_t *typep, ctf_error_t *errp)
{
  return ctf_arc_lookup_sym_or_name (arci, 0, symname, typep, errp);
}

/* Return all enumeration constants with a given NAME across all dicts in an
   archive, similar to ctf_lookup_enumerator_next.  The DICT is cached, so
   opening costs are paid only once, but (unlike ctf_arc_lookup_symbol*
   above) the results of the iterations are not cached.  dict and errp are
   not optional.  */

ctf_id_t
ctf_arc_lookup_enumerator_next (struct ctf_archive_internal *arci,
				const char *name, ctf_next_t **it,
				int64_t *enum_value, ctf_dict_t **dict,
				ctf_error_t *errp)
{
  ctf_next_t *i = *it;
  ctf_id_t type;
  int opened_this_time = 0;
  ctf_error_t err;

  /* We have two nested iterators in here: ctn_next tracks archives, while
     within it ctn_next_inner tracks enumerators within an archive.  We
     keep track of the dict by simply reusing the passed-in arg: if it's
     changed by the caller, the caller will get an ECTF_WRONGFP error,
     so this is quite safe and means we don't have to track the arc and fp
     simultaneously in the ctf_next_t.  */

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	{
	  err = ENOMEM;
	  goto err;
	}
      i->ctn_iter_fun = (void (*) (void)) ctf_arc_lookup_enumerator_next;
      i->cu.ctn_arc = arci;
      *it = i;
    }

  if ((void (*) (void)) ctf_arc_lookup_enumerator_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto err;
    }

  if (arci != i->cu.ctn_arc)
    {
      err = ECTF_NEXT_WRONGFP;
      goto err;
    }

  /* Prevent any earlier end-of-iteration on this dict from confusing the
     test below.  */
  if (i->ctn_next != NULL)
    ctf_set_errno (*dict, 0);

  do
    {
      /* At end of one dict, or not started any iterations yet?
	 Traverse to next dict.  If we never returned this dict to the
	 caller, close it ourselves: the caller will never see it and cannot
	 do so.  */

      if (i->ctn_next == NULL || ctf_errno (*dict) == ECTF_NEXT_END)
	{
	  if (opened_this_time)
	    {
	      ctf_dict_close (*dict);
	      *dict = NULL;
	      opened_this_time = 0;
	    }

	  *dict = ctf_archive_next (arci, &i->ctn_next, NULL, 0, &err);
	  if (!*dict)
	    goto err;
	  opened_this_time = 1;
	}

      type = ctf_lookup_enumerator_next (*dict, name, &i->ctn_next_inner,
					 enum_value);
    }
  while (type == CTF_ERR && ctf_errno (*dict) == ECTF_NEXT_END);

  if (type == CTF_ERR)
    {
      err = ctf_errno (*dict);
      goto err;
    }

  /* If this dict is being reused from the previous iteration, bump its
     refcnt: the caller is going to close it and has no idea that we didn't
     open it this time round.  */
  if (!opened_this_time)
    (*dict)->ctf_refcnt++;

  return type;

 err:						/* Also ECTF_NEXT_END. */
  if (opened_this_time)
    {
      ctf_dict_close (*dict);
      *dict = NULL;
    }

  ctf_next_destroy (i);
  *it = NULL;
  if (errp)
    *errp = err;
  return CTF_ERR;
}

/* Raw iteration over all CTF files in an archive: public entry point.

   Returns -EINVAL if not supported for this sort of archive.  */

const char *
ctf_archive_raw_next (const struct ctf_archive_internal *arci, ctf_next_t **it,
		      const void **contents, size_t *len, ctf_error_t *errp)
{
  ctf_next_t *i = *it;
  size_t name_off, contents_off;
  const void *local_contents;
  ctf_archive_modent_t *modent;
  const char *nametbl;
  ctf_error_t err;

  if (!arci->ctfi_is_archive || !arci->ctfi_archive)
    {
      if (errp)
	*errp = EINVAL;
      return NULL;				/* Not supported.  */
    }

  if (!i)
    {
      if ((i = ctf_next_create()) == NULL)
	{
	  if (errp)
	    *errp = ENOMEM;
	  return NULL;
	}
      i->cu.ctn_arc = arci;
      i->ctn_iter_fun = (void (*) (void)) ctf_archive_raw_next;
      i->ctn_size = arci->ctfi_hdr->ndicts;
      i->ctn_n = 0;
      *it = i;
    }

  if ((void (*) (void)) ctf_archive_raw_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (arci != i->cu.ctn_arc)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  if (i->ctn_n >= arci->ctfi_hdr->ndicts)
    {
      err = ECTF_NEXT_END;
      goto end;
    }

  modent = (ctf_archive_modent_t *) (arci->ctfi_archive
				     + arci->ctfi_hdr->modents);
  nametbl = (const char *) arci->ctfi_archive + arci->ctfi_hdr->names;

  name_off = modent[i->ctn_n].name;
  contents_off = modent[i->ctn_n].contents;

  local_contents = arci->ctfi_archive + arci->ctfi_hdr->ctfs + contents_off;
  if (contents)
    *contents = local_contents;
  if (len)
    *len = *((uint64_t *) local_contents);

  i->ctn_n++;

  return &nametbl[name_off];

 end:
  ctf_next_destroy (i);
  *it = NULL;
  if (errp)
    *errp = err;
  return NULL;
}

/* Iterate over all CTF files in an archive, returning each dict in turn as a
   ctf_dict_t, and NULL on error or end of iteration.  It is the caller's
   responsibility to close it.  Parent dicts may be skipped.

   The archive member is cached for rapid return on future calls.

   We identify parents by name rather than by flag value: for now, with the
   linker only emitting parents named _CTF_SECTION, this works well enough.  */

ctf_dict_t *
ctf_archive_next (const struct ctf_archive_internal *arci, ctf_next_t **it,
		  const char **name, int skip_parent, ctf_error_t *errp)
{
  ctf_dict_t *f;
  ctf_next_t *i = *it;
  ctf_archive_modent_t *modent;
  const char *nametbl;
  const char *name_;
  ctf_error_t err;

  if (!i)
    {
      if ((i = ctf_next_create()) == NULL)
	{
	  if (errp)
	    *errp = ENOMEM;
	  return NULL;
	}
      i->cu.ctn_arc = arci;
      i->ctn_iter_fun = (void (*) (void)) ctf_archive_next;
      *it = i;
    }

  if ((void (*) (void)) ctf_archive_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (arci != i->cu.ctn_arc)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  /* Iteration is made a bit more complex by the need to handle ctf_dict_t's
     transparently wrapped in a single-member archive.  These are parents: if
     skip_parent is on, they are skipped and the iterator terminates
     immediately.  */

  if (!arci->ctfi_is_archive && i->ctn_n == 0)
    {
      i->ctn_n++;
      if (!skip_parent)
	{
	  arci->ctfi_dict->ctf_refcnt++;
	  if (name)
	    *name = _CTF_SECTION;
	  return arci->ctfi_dict;
	}
    }

  /* The loop keeps going when skip_parent is on as long as the member we find
     is the parent (i.e. at most two iterations, but possibly an early return if
     *all* we have is a parent).  */

  do
    {
      if ((!arci->ctfi_is_archive) || (i->ctn_n >= arci->ctfi_hdr->ndicts))
	{
	  err = ECTF_NEXT_END;
	  goto end;
	}

      modent = (ctf_archive_modent_t *) ((char *) arci->ctfi_archive
				     + arci->ctfi_hdr->modents);
      nametbl = (const char *) arci->ctfi_archive + arci->ctfi_hdr->names;
      name_ = &nametbl[modent[i->ctn_n].name];
      i->ctn_n++;
    }
  while (skip_parent && strcmp (name_, _CTF_SECTION) == 0);

  if (name)
    *name = name_;

  f = ctf_dict_open_cached ((ctf_archive_t *) arci, name_, errp);
  return f;

 end:
  ctf_next_destroy (i);
  *it = NULL;
  if (errp)
    *errp = err;
  return NULL;
}

#ifdef HAVE_MMAP
/* Map the header in.  Only used on new, empty files.  */
static void *arc_mmap_header (int fd, size_t headersz)
{
  void *hdr;
  if ((hdr = mmap (NULL, headersz, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		   0)) == MAP_FAILED)
    return NULL;
  return hdr;
}

/* mmap() the whole file, for reading only.  (Map it writably, but privately: we
   need to modify the region, but don't need anyone else to see the
   modifications.)  */
static void *arc_mmap_file (int fd, size_t size)
{
  void *arc;
  if ((arc = mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
		   fd, 0)) == MAP_FAILED)
    return NULL;
  return arc;
}

/* Persist the header to disk.  */
static int arc_mmap_writeout (int fd _libctf_unused_, void *header,
			      size_t headersz, const char **errmsg)
{
    if (msync (header, headersz, MS_ASYNC) < 0)
    {
      if (errmsg)
	*errmsg = N_("arc_mmap_writeout(): cannot sync after writing "
		     "to %s: %s");
      return -1;
    }
    return 0;
}

/* Unmap the region.  */
static int arc_mmap_unmap (void *header, size_t headersz, const char **errmsg)
{
  if (munmap (header, headersz) < 0)
    {
      if (errmsg)
	*errmsg = N_("arc_mmap_munmap(): cannot unmap after writing "
		     "to %s: %s");
      return -1;
    }
    return 0;
}
#else
/* Map the header in.  Only used on new, empty files.  */
static void *arc_mmap_header (int fd _libctf_unused_, size_t headersz)
{
  void *hdr;
  if ((hdr = malloc (headersz)) == NULL)
    return NULL;
  return hdr;
}

/* Pull in the whole file, for reading only.  We assume the current file
   position is at the start of the file.  */
static void *arc_mmap_file (int fd, size_t size)
{
  char *data;

  if ((data = malloc (size)) == NULL)
    return NULL;

  if (ctf_pread (fd, data, size, 0) < 0)
    {
      free (data);
      return NULL;
    }
  return data;
}

/* Persist the header to disk.  */
static int arc_mmap_writeout (int fd, void *header, size_t headersz,
			      const char **errmsg)
{
  ssize_t len;
  char *data = (char *) header;
  ssize_t count = headersz;

  if ((lseek (fd, 0, SEEK_SET)) < 0)
    {
      if (errmsg)
	*errmsg = N_("arc_mmap_writeout(): cannot seek while writing header to "
		     "%s: %s");
      return -1;
    }

  while (headersz > 0)
    {
      if ((len = write (fd, data, count)) < 0)
	{
	  if (errmsg)
	    *errmsg = N_("arc_mmap_writeout(): cannot write header to %s: %s");
	  return len;
	}
      if (len == EINTR)
	continue;

      if (len == 0)				/* EOF.  */
	break;

      count -= len;
      data += len;
    }
  return 0;
}

/* Unmap the region.  */
static int arc_mmap_unmap (void *header, size_t headersz _libctf_unused_,
			   const char **errmsg _libctf_unused_)
{
  free (header);
  return 0;
}
#endif
