/* CTF file creation.
   Copyright (C) 2019 Free Software Foundation, Inc.

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
#include <sys/param.h>
#include <assert.h>
#include <string.h>
#include <zlib.h>

/* To create an empty CTF container, we just declare a zeroed header and call
   ctf_bufopen() on it.  If ctf_bufopen succeeds, we mark the new container r/w
   and initialize the dynamic members.  We set dtvstrlen to 1 to reserve the
   first byte of the string table for a \0 byte, and we start assigning type
   IDs at 1 because type ID 0 is used as a sentinel and a not-found
   indicator.  */

ctf_file_t *
ctf_create (int *errp)
{
  static const ctf_header_t hdr = { .cth_preamble = { CTF_MAGIC, CTF_VERSION, 0 } };

  ctf_dynhash_t *dthash;
  ctf_dynhash_t *dvhash;
  ctf_dynhash_t *dtbyname;
  ctf_sect_t cts;
  ctf_file_t *fp;

  libctf_init_debug();
  dthash = ctf_dynhash_create (ctf_hash_integer, ctf_hash_eq_integer,
			       NULL, NULL);
  if (dthash == NULL)
    {
      ctf_set_open_errno (errp, EAGAIN);
      goto err;
    }

  dvhash = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			       NULL, NULL);
  if (dvhash == NULL)
    {
      ctf_set_open_errno (errp, EAGAIN);
      goto err_dt;
    }

  dtbyname = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
				 free, NULL);
  if (dtbyname == NULL)
    {
      ctf_set_open_errno (errp, EAGAIN);
      goto err_dv;
    }

  cts.cts_name = _CTF_SECTION;
  cts.cts_type = SHT_PROGBITS;
  cts.cts_flags = 0;
  cts.cts_data = &hdr;
  cts.cts_size = sizeof (hdr);
  cts.cts_entsize = 1;
  cts.cts_offset = 0;

  if ((fp = ctf_bufopen (&cts, NULL, NULL, errp)) == NULL)
      goto err_dtbyname;

  fp->ctf_flags |= LCTF_RDWR;
  fp->ctf_dtbyname = dtbyname;
  fp->ctf_dthash = dthash;
  fp->ctf_dvhash = dvhash;
  fp->ctf_dtvstrlen = 1;
  fp->ctf_dtnextid = 1;
  fp->ctf_dtoldid = 0;
  fp->ctf_snapshots = 0;
  fp->ctf_snapshot_lu = 0;

  return fp;

 err_dtbyname:
  ctf_dynhash_destroy (dtbyname);
 err_dv:
  ctf_dynhash_destroy (dvhash);
 err_dt:
  ctf_dynhash_destroy (dthash);
 err:
  return NULL;
}

static unsigned char *
ctf_copy_smembers (ctf_dtdef_t *dtd, uint32_t soff, unsigned char *t)
{
  ctf_dmdef_t *dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
  ctf_member_t ctm;

  for (; dmd != NULL; dmd = ctf_list_next (dmd))
    {
      if (dmd->dmd_name)
	{
	  ctm.ctm_name = soff;
	  soff += strlen (dmd->dmd_name) + 1;
	}
      else
	ctm.ctm_name = 0;

      ctm.ctm_type = (uint32_t) dmd->dmd_type;
      ctm.ctm_offset = (uint32_t) dmd->dmd_offset;

      memcpy (t, &ctm, sizeof (ctm));
      t += sizeof (ctm);
    }

  return t;
}

static unsigned char *
ctf_copy_lmembers (ctf_dtdef_t *dtd, uint32_t soff, unsigned char *t)
{
  ctf_dmdef_t *dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
  ctf_lmember_t ctlm;

  for (; dmd != NULL; dmd = ctf_list_next (dmd))
    {
      if (dmd->dmd_name)
	{
	  ctlm.ctlm_name = soff;
	  soff += strlen (dmd->dmd_name) + 1;
	}
      else
	ctlm.ctlm_name = 0;

      ctlm.ctlm_type = (uint32_t) dmd->dmd_type;
      ctlm.ctlm_offsethi = CTF_OFFSET_TO_LMEMHI (dmd->dmd_offset);
      ctlm.ctlm_offsetlo = CTF_OFFSET_TO_LMEMLO (dmd->dmd_offset);

      memcpy (t, &ctlm, sizeof (ctlm));
      t += sizeof (ctlm);
    }

  return t;
}

static unsigned char *
ctf_copy_emembers (ctf_dtdef_t *dtd, uint32_t soff, unsigned char *t)
{
  ctf_dmdef_t *dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
  ctf_enum_t cte;

  for (; dmd != NULL; dmd = ctf_list_next (dmd))
    {
      cte.cte_name = soff;
      cte.cte_value = dmd->dmd_value;
      soff += strlen (dmd->dmd_name) + 1;
      memcpy (t, &cte, sizeof (cte));
      t += sizeof (cte);
    }

  return t;
}

static unsigned char *
ctf_copy_membnames (ctf_dtdef_t *dtd, unsigned char *s)
{
  ctf_dmdef_t *dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
  size_t len;

  for (; dmd != NULL; dmd = ctf_list_next (dmd))
    {
      if (dmd->dmd_name == NULL)
	continue;			/* Skip anonymous members.  */
      len = strlen (dmd->dmd_name) + 1;
      memcpy (s, dmd->dmd_name, len);
      s += len;
    }

  return s;
}

/* Sort a newly-constructed static variable array.  */

static int
ctf_sort_var (const void *one_, const void *two_, void *strtab_)
{
  const ctf_varent_t *one = one_;
  const ctf_varent_t *two = two_;
  const char *strtab = strtab_;
  const char *n1 = strtab + CTF_NAME_OFFSET (one->ctv_name);
  const char *n2 = strtab + CTF_NAME_OFFSET (two->ctv_name);

  return (strcmp (n1, n2));
}

/* If the specified CTF container is writable and has been modified, reload this
   container with the updated type definitions.  In order to make this code and
   the rest of libctf as simple as possible, we perform updates by taking the
   dynamic type definitions and creating an in-memory CTF file containing the
   definitions, and then call ctf_simple_open() on it.  This not only leverages
   ctf_simple_open(), but also avoids having to bifurcate the rest of the library
   code with different lookup paths for static and dynamic type definitions.  We
   are therefore optimizing greatly for lookup over update, which we assume will
   be an uncommon operation.  We perform one extra trick here for the benefit of
   callers and to keep our code simple: ctf_simple_open() will return a new
   ctf_file_t, but we want to keep the fp constant for the caller, so after
   ctf_simple_open() returns, we use memcpy to swap the interior of the old and
   new ctf_file_t's, and then free the old.  */
int
ctf_update (ctf_file_t *fp)
{
  ctf_file_t ofp, *nfp;
  ctf_header_t hdr;
  ctf_dtdef_t *dtd;
  ctf_dvdef_t *dvd;
  ctf_varent_t *dvarents;

  unsigned char *s, *s0, *t;
  unsigned long i;
  size_t buf_size, type_size, nvars;
  void *buf;
  int err;

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  /* Update required?  */
  if (!(fp->ctf_flags & LCTF_DIRTY))
    return 0;

  /* Fill in an initial CTF header.  We will leave the label, object,
     and function sections empty and only output a header, type section,
     and string table.  The type section begins at a 4-byte aligned
     boundary past the CTF header itself (at relative offset zero).  */

  memset (&hdr, 0, sizeof (hdr));
  hdr.cth_magic = CTF_MAGIC;
  hdr.cth_version = CTF_VERSION;

  if (fp->ctf_flags & LCTF_CHILD)
    hdr.cth_parname = 1;		/* parname added just below.  */

  /* Iterate through the dynamic type definition list and compute the
     size of the CTF type section we will need to generate.  */

  for (type_size = 0, dtd = ctf_list_next (&fp->ctf_dtdefs);
       dtd != NULL; dtd = ctf_list_next (dtd))
    {
      uint32_t kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);
      uint32_t vlen = LCTF_INFO_VLEN (fp, dtd->dtd_data.ctt_info);

      if (dtd->dtd_data.ctt_size != CTF_LSIZE_SENT)
	type_size += sizeof (ctf_stype_t);
      else
	type_size += sizeof (ctf_type_t);

      switch (kind)
	{
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  type_size += sizeof (uint32_t);
	  break;
	case CTF_K_ARRAY:
	  type_size += sizeof (ctf_array_t);
	  break;
	case CTF_K_SLICE:
	  type_size += sizeof (ctf_slice_t);
	  break;
	case CTF_K_FUNCTION:
	  type_size += sizeof (uint32_t) * (vlen + (vlen & 1));
	  break;
	case CTF_K_STRUCT:
	case CTF_K_UNION:
	  if (dtd->dtd_data.ctt_size < CTF_LSTRUCT_THRESH)
	    type_size += sizeof (ctf_member_t) * vlen;
	  else
	    type_size += sizeof (ctf_lmember_t) * vlen;
	  break;
	case CTF_K_ENUM:
	  type_size += sizeof (ctf_enum_t) * vlen;
	  break;
	}
    }

  /* Computing the number of entries in the CTF variable section is much
     simpler.  */

  for (nvars = 0, dvd = ctf_list_next (&fp->ctf_dvdefs);
       dvd != NULL; dvd = ctf_list_next (dvd), nvars++);

  /* Fill in the string table and type offset and size, compute the size
     of the entire CTF buffer we need, and then allocate a new buffer and
     memcpy the finished header to the start of the buffer.  */

  hdr.cth_typeoff = hdr.cth_varoff + (nvars * sizeof (ctf_varent_t));
  hdr.cth_stroff = hdr.cth_typeoff + type_size;
  hdr.cth_strlen = fp->ctf_dtvstrlen;
  if (fp->ctf_parname != NULL)
    hdr.cth_strlen += strlen (fp->ctf_parname) + 1;

  buf_size = sizeof (ctf_header_t) + hdr.cth_stroff + hdr.cth_strlen;

  if ((buf = ctf_data_alloc (buf_size)) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  memcpy (buf, &hdr, sizeof (ctf_header_t));
  t = (unsigned char *) buf + sizeof (ctf_header_t) + hdr.cth_varoff;
  s = s0 = (unsigned char *) buf + sizeof (ctf_header_t) + hdr.cth_stroff;

  s[0] = '\0';
  s++;

  if (fp->ctf_parname != NULL)
    {
      memcpy (s, fp->ctf_parname, strlen (fp->ctf_parname) + 1);
      s += strlen (fp->ctf_parname) + 1;
    }

  /* Work over the variable list, translating everything into
     ctf_varent_t's and filling out the string table, then sort the buffer
     of ctf_varent_t's.  */

  dvarents = (ctf_varent_t *) t;
  for (i = 0, dvd = ctf_list_next (&fp->ctf_dvdefs); dvd != NULL;
       dvd = ctf_list_next (dvd), i++)
    {
      ctf_varent_t *var = &dvarents[i];
      size_t len = strlen (dvd->dvd_name) + 1;

      var->ctv_name = (uint32_t) (s - s0);
      var->ctv_type = dvd->dvd_type;
      memcpy (s, dvd->dvd_name, len);
      s += len;
    }
  assert (i == nvars);

  qsort_r (dvarents, nvars, sizeof (ctf_varent_t), ctf_sort_var, s0);
  t += sizeof (ctf_varent_t) * nvars;

  assert (t == (unsigned char *) buf + sizeof (ctf_header_t) + hdr.cth_typeoff);

  /* We now take a final lap through the dynamic type definition list and
     copy the appropriate type records and strings to the output buffer.  */

  for (dtd = ctf_list_next (&fp->ctf_dtdefs);
       dtd != NULL; dtd = ctf_list_next (dtd))
    {

      uint32_t kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);
      uint32_t vlen = LCTF_INFO_VLEN (fp, dtd->dtd_data.ctt_info);

      ctf_array_t cta;
      uint32_t encoding;
      size_t len;

      if (dtd->dtd_name != NULL)
	{
	  dtd->dtd_data.ctt_name = (uint32_t) (s - s0);
	  len = strlen (dtd->dtd_name) + 1;
	  memcpy (s, dtd->dtd_name, len);
	  s += len;
	}
      else
	dtd->dtd_data.ctt_name = 0;

      if (dtd->dtd_data.ctt_size != CTF_LSIZE_SENT)
	len = sizeof (ctf_stype_t);
      else
	len = sizeof (ctf_type_t);

      memcpy (t, &dtd->dtd_data, len);
      t += len;

      switch (kind)
	{
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  if (kind == CTF_K_INTEGER)
	    {
	      encoding = CTF_INT_DATA (dtd->dtd_u.dtu_enc.cte_format,
				       dtd->dtd_u.dtu_enc.cte_offset,
				       dtd->dtd_u.dtu_enc.cte_bits);
	    }
	  else
	    {
	      encoding = CTF_FP_DATA (dtd->dtd_u.dtu_enc.cte_format,
				      dtd->dtd_u.dtu_enc.cte_offset,
				      dtd->dtd_u.dtu_enc.cte_bits);
	    }
	  memcpy (t, &encoding, sizeof (encoding));
	  t += sizeof (encoding);
	  break;

	case CTF_K_SLICE:
	  memcpy (t, &dtd->dtd_u.dtu_slice, sizeof (struct ctf_slice));
	  t += sizeof (struct ctf_slice);
	  break;

	case CTF_K_ARRAY:
	  cta.cta_contents = (uint32_t) dtd->dtd_u.dtu_arr.ctr_contents;
	  cta.cta_index = (uint32_t) dtd->dtd_u.dtu_arr.ctr_index;
	  cta.cta_nelems = dtd->dtd_u.dtu_arr.ctr_nelems;
	  memcpy (t, &cta, sizeof (cta));
	  t += sizeof (cta);
	  break;

	case CTF_K_FUNCTION:
	  {
	    uint32_t *argv = (uint32_t *) (uintptr_t) t;
	    uint32_t argc;

	    for (argc = 0; argc < vlen; argc++)
	      *argv++ = (uint32_t) dtd->dtd_u.dtu_argv[argc];

	    if (vlen & 1)
	      *argv++ = 0;	/* Pad to 4-byte boundary.  */

	    t = (unsigned char *) argv;
	    break;
	  }

	case CTF_K_STRUCT:
	case CTF_K_UNION:
	  if (dtd->dtd_data.ctt_size < CTF_LSTRUCT_THRESH)
	    t = ctf_copy_smembers (dtd, (uint32_t) (s - s0), t);
	  else
	    t = ctf_copy_lmembers (dtd, (uint32_t) (s - s0), t);
	  s = ctf_copy_membnames (dtd, s);
	  break;

	case CTF_K_ENUM:
	  t = ctf_copy_emembers (dtd, (uint32_t) (s - s0), t);
	  s = ctf_copy_membnames (dtd, s);
	  break;
	}
    }
  assert (t == (unsigned char *) buf + sizeof (ctf_header_t) + hdr.cth_stroff);

  /* Finally, we are ready to ctf_simple_open() the new container.  If this
     is successful, we then switch nfp and fp and free the old container.  */

  ctf_data_protect (buf, buf_size);

  if ((nfp = ctf_simple_open (buf, buf_size, NULL, 0, 0, NULL, 0, &err)) == NULL)
    {
      ctf_data_free (buf, buf_size);
      return (ctf_set_errno (fp, err));
    }

  (void) ctf_setmodel (nfp, ctf_getmodel (fp));
  (void) ctf_import (nfp, fp->ctf_parent);

  nfp->ctf_refcnt = fp->ctf_refcnt;
  nfp->ctf_flags |= fp->ctf_flags & ~LCTF_DIRTY;
  nfp->ctf_data.cts_data = NULL;	/* Force ctf_data_free() on close.  */
  nfp->ctf_dthash = fp->ctf_dthash;
  nfp->ctf_dtdefs = fp->ctf_dtdefs;
  nfp->ctf_dtbyname = fp->ctf_dtbyname;
  nfp->ctf_dvhash = fp->ctf_dvhash;
  nfp->ctf_dvdefs = fp->ctf_dvdefs;
  nfp->ctf_dtvstrlen = fp->ctf_dtvstrlen;
  nfp->ctf_dtnextid = fp->ctf_dtnextid;
  nfp->ctf_dtoldid = fp->ctf_dtnextid - 1;
  nfp->ctf_snapshots = fp->ctf_snapshots + 1;
  nfp->ctf_specific = fp->ctf_specific;

  nfp->ctf_snapshot_lu = fp->ctf_snapshots;

  fp->ctf_dtbyname = NULL;
  fp->ctf_dthash = NULL;
  memset (&fp->ctf_dtdefs, 0, sizeof (ctf_list_t));

  fp->ctf_dvhash = NULL;
  memset (&fp->ctf_dvdefs, 0, sizeof (ctf_list_t));

  memcpy (&ofp, fp, sizeof (ctf_file_t));
  memcpy (fp, nfp, sizeof (ctf_file_t));
  memcpy (nfp, &ofp, sizeof (ctf_file_t));

  /* Initialize the ctf_lookup_by_name top-level dictionary.  We keep an
     array of type name prefixes and the corresponding ctf_dynhash to use.
     NOTE: This code must be kept in sync with the code in ctf_bufopen().  */

  fp->ctf_lookups[0].ctl_hash = fp->ctf_structs;
  fp->ctf_lookups[1].ctl_hash = fp->ctf_unions;
  fp->ctf_lookups[2].ctl_hash = fp->ctf_enums;
  fp->ctf_lookups[3].ctl_hash = fp->ctf_names;

  nfp->ctf_refcnt = 1;		/* Force nfp to be freed.  */
  ctf_file_close (nfp);

  return 0;
}

static char *
ctf_prefixed_name (int kind, const char *name)
{
  char *prefixed;

  switch (kind)
    {
    case CTF_K_STRUCT:
      prefixed = ctf_strdup ("struct ");
      break;
    case CTF_K_UNION:
      prefixed = ctf_strdup ("union ");
      break;
    case CTF_K_ENUM:
      prefixed = ctf_strdup ("enum ");
      break;
    default:
      prefixed = ctf_strdup ("");
    }

  prefixed = ctf_str_append (prefixed, name);
  return prefixed;
}

void
ctf_dtd_insert (ctf_file_t *fp, ctf_dtdef_t *dtd)
{
  ctf_dynhash_insert (fp->ctf_dthash, (void *) dtd->dtd_type, dtd);
  ctf_list_append (&fp->ctf_dtdefs, dtd);
  if (dtd->dtd_name)
    {
      int kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);
      ctf_dynhash_insert (fp->ctf_dtbyname, ctf_prefixed_name (kind,
							       dtd->dtd_name),
			  dtd);
    }
}

void
ctf_dtd_delete (ctf_file_t *fp, ctf_dtdef_t *dtd)
{
  ctf_dmdef_t *dmd, *nmd;
  int kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);

  ctf_dynhash_remove (fp->ctf_dthash, (void *) dtd->dtd_type);

  switch (kind)
    {
    case CTF_K_STRUCT:
    case CTF_K_UNION:
    case CTF_K_ENUM:
      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
	   dmd != NULL; dmd = nmd)
	{
	  if (dmd->dmd_name != NULL)
	    {
	      fp->ctf_dtvstrlen -= strlen (dmd->dmd_name) + 1;
	      ctf_free (dmd->dmd_name);
	    }
	  nmd = ctf_list_next (dmd);
	  ctf_free (dmd);
	}
      break;
    case CTF_K_FUNCTION:
      ctf_free (dtd->dtd_u.dtu_argv);
      break;
    }

  if (dtd->dtd_name)
    {
      char *name;

      name = ctf_prefixed_name (kind, dtd->dtd_name);
      ctf_dynhash_remove (fp->ctf_dtbyname, name);
      free (name);

      fp->ctf_dtvstrlen -= strlen (dtd->dtd_name) + 1;
      ctf_free (dtd->dtd_name);
    }

  ctf_list_delete (&fp->ctf_dtdefs, dtd);
  ctf_free (dtd);
}

ctf_dtdef_t *
ctf_dtd_lookup (const ctf_file_t *fp, ctf_id_t type)
{
  return (ctf_dtdef_t *) ctf_dynhash_lookup (fp->ctf_dthash, (void *) type);
}

static ctf_id_t
ctf_dtd_lookup_type_by_name (ctf_file_t *fp, int kind, const char *name)
{
  ctf_dtdef_t *dtd;
  char *decorated = ctf_prefixed_name (kind, name);

  dtd = (ctf_dtdef_t *) ctf_dynhash_lookup (fp->ctf_dtbyname, decorated);
  free (decorated);

  if (dtd)
    return dtd->dtd_type;

  return 0;
}

ctf_dtdef_t *
ctf_dynamic_type (const ctf_file_t *fp, ctf_id_t id)
{
  ctf_id_t idx;

  if ((fp->ctf_flags & LCTF_CHILD) && LCTF_TYPE_ISPARENT (fp, id))
    fp = fp->ctf_parent;

  idx = LCTF_TYPE_TO_INDEX(fp, id);

  if (((unsigned long) idx > fp->ctf_typemax) &&
      ((unsigned long) idx < fp->ctf_dtnextid))
    return ctf_dtd_lookup (fp, id);
  return NULL;
}

void
ctf_dvd_insert (ctf_file_t *fp, ctf_dvdef_t *dvd)
{
  ctf_dynhash_insert (fp->ctf_dvhash, dvd->dvd_name, dvd);
  ctf_list_append (&fp->ctf_dvdefs, dvd);
}

void
ctf_dvd_delete (ctf_file_t *fp, ctf_dvdef_t *dvd)
{
  ctf_dynhash_remove (fp->ctf_dvhash, dvd->dvd_name);

  fp->ctf_dtvstrlen -= strlen (dvd->dvd_name) + 1;
  ctf_free (dvd->dvd_name);

  ctf_list_delete (&fp->ctf_dvdefs, dvd);
  ctf_free (dvd);
}

ctf_dvdef_t *
ctf_dvd_lookup (const ctf_file_t *fp, const char *name)
{
  return (ctf_dvdef_t *) ctf_dynhash_lookup (fp->ctf_dvhash, name);
}

/* Discard all of the dynamic type definitions and variable definitions that
   have been added to the container since the last call to ctf_update().  We
   locate such types by scanning the dtd list and deleting elements that have
   type IDs greater than ctf_dtoldid, which is set by ctf_update(), above, and
   by scanning the variable list and deleting elements that have update IDs
   equal to the current value of the last-update snapshot count (indicating that
   they were added after the most recent call to ctf_update()).  */
int
ctf_discard (ctf_file_t *fp)
{
  ctf_snapshot_id_t last_update =
    { fp->ctf_dtoldid,
      fp->ctf_snapshot_lu + 1 };

  /* Update required?  */
  if (!(fp->ctf_flags & LCTF_DIRTY))
    return 0;

  return (ctf_rollback (fp, last_update));
}

ctf_snapshot_id_t
ctf_snapshot (ctf_file_t *fp)
{
  ctf_snapshot_id_t snapid;
  snapid.dtd_id = fp->ctf_dtnextid - 1;
  snapid.snapshot_id = fp->ctf_snapshots++;
  return snapid;
}

/* Like ctf_discard(), only discards everything after a particular ID.  */
int
ctf_rollback (ctf_file_t *fp, ctf_snapshot_id_t id)
{
  ctf_dtdef_t *dtd, *ntd;
  ctf_dvdef_t *dvd, *nvd;

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (fp->ctf_dtoldid > id.dtd_id)
    return (ctf_set_errno (fp, ECTF_OVERROLLBACK));

  if (fp->ctf_snapshot_lu >= id.snapshot_id)
    return (ctf_set_errno (fp, ECTF_OVERROLLBACK));

  for (dtd = ctf_list_next (&fp->ctf_dtdefs); dtd != NULL; dtd = ntd)
    {
      ntd = ctf_list_next (dtd);

      if (LCTF_TYPE_TO_INDEX (fp, dtd->dtd_type) <= id.dtd_id)
	continue;

      ctf_dtd_delete (fp, dtd);
    }

  for (dvd = ctf_list_next (&fp->ctf_dvdefs); dvd != NULL; dvd = nvd)
    {
      nvd = ctf_list_next (dvd);

      if (dvd->dvd_snapshots <= id.snapshot_id)
	continue;

      ctf_dvd_delete (fp, dvd);
    }

  fp->ctf_dtnextid = id.dtd_id + 1;
  fp->ctf_snapshots = id.snapshot_id;

  if (fp->ctf_snapshots == fp->ctf_snapshot_lu)
    fp->ctf_flags &= ~LCTF_DIRTY;

  return 0;
}

static ctf_id_t
ctf_add_generic (ctf_file_t *fp, uint32_t flag, const char *name,
		 ctf_dtdef_t **rp)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  char *s = NULL;

  if (flag != CTF_ADD_NONROOT && flag != CTF_ADD_ROOT)
    return (ctf_set_errno (fp, EINVAL));

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (LCTF_INDEX_TO_TYPE (fp, fp->ctf_dtnextid, 1) > CTF_MAX_TYPE)
    return (ctf_set_errno (fp, ECTF_FULL));

  if (LCTF_INDEX_TO_TYPE (fp, fp->ctf_dtnextid, 1) == CTF_MAX_PTYPE)
    return (ctf_set_errno (fp, ECTF_FULL));

  if ((dtd = ctf_alloc (sizeof (ctf_dtdef_t))) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  if (name != NULL && (s = ctf_strdup (name)) == NULL)
    {
      ctf_free (dtd);
      return (ctf_set_errno (fp, EAGAIN));
    }

  type = fp->ctf_dtnextid++;
  type = LCTF_INDEX_TO_TYPE (fp, type, (fp->ctf_flags & LCTF_CHILD));

  memset (dtd, 0, sizeof (ctf_dtdef_t));
  dtd->dtd_name = s;
  dtd->dtd_type = type;

  if (s != NULL)
    fp->ctf_dtvstrlen += strlen (s) + 1;

  ctf_dtd_insert (fp, dtd);
  fp->ctf_flags |= LCTF_DIRTY;

  *rp = dtd;
  return type;
}

/* When encoding integer sizes, we want to convert a byte count in the range
   1-8 to the closest power of 2 (e.g. 3->4, 5->8, etc).  The clp2() function
   is a clever implementation from "Hacker's Delight" by Henry Warren, Jr.  */
static size_t
clp2 (size_t x)
{
  x--;

  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);

  return (x + 1);
}

static ctf_id_t
ctf_add_encoded (ctf_file_t *fp, uint32_t flag,
		 const char *name, const ctf_encoding_t *ep, uint32_t kind)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;

  if (ep == NULL)
    return (ctf_set_errno (fp, EINVAL));

  if ((type = ctf_add_generic (fp, flag, name, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (kind, flag, 0);
  dtd->dtd_data.ctt_size = clp2 (P2ROUNDUP (ep->cte_bits, NBBY) / NBBY);
  dtd->dtd_u.dtu_enc = *ep;

  return type;
}

static ctf_id_t
ctf_add_reftype (ctf_file_t *fp, uint32_t flag, ctf_id_t ref, uint32_t kind)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  ctf_file_t *tmp = fp;

  if (ref == CTF_ERR || ref < 0 || ref > CTF_MAX_TYPE)
    return (ctf_set_errno (fp, EINVAL));

  if (ctf_lookup_by_id (&tmp, ref) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((type = ctf_add_generic (fp, flag, NULL, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (kind, flag, 0);
  dtd->dtd_data.ctt_type = (uint32_t) ref;

  return type;
}

ctf_id_t
ctf_add_slice (ctf_file_t *fp, uint32_t flag, ctf_id_t ref,
	       const ctf_encoding_t *ep)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  int kind;
  const ctf_type_t *tp;
  ctf_file_t *tmp = fp;

  if (ep == NULL)
    return (ctf_set_errno (fp, EINVAL));

  if ((ep->cte_bits > 255) || (ep->cte_offset > 255))
    return (ctf_set_errno (fp, ECTF_SLICEOVERFLOW));

  if (ref == CTF_ERR || ref < 0 || ref > CTF_MAX_TYPE)
    return (ctf_set_errno (fp, EINVAL));

  if ((tp = ctf_lookup_by_id (&tmp, ref)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  kind = ctf_type_kind_unsliced (tmp, ref);
  if ((kind != CTF_K_INTEGER) && (kind != CTF_K_FLOAT) &&
      (kind != CTF_K_ENUM))
    return (ctf_set_errno (fp, ECTF_NOTINTFP));

  if ((type = ctf_add_generic (fp, flag, NULL, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_SLICE, flag, 0);
  dtd->dtd_data.ctt_size = clp2 (P2ROUNDUP (ep->cte_bits, NBBY) / NBBY);
  dtd->dtd_u.dtu_slice.cts_type = ref;
  dtd->dtd_u.dtu_slice.cts_bits = ep->cte_bits;
  dtd->dtd_u.dtu_slice.cts_offset = ep->cte_offset;

  return type;
}

ctf_id_t
ctf_add_integer (ctf_file_t *fp, uint32_t flag,
		 const char *name, const ctf_encoding_t *ep)
{
  return (ctf_add_encoded (fp, flag, name, ep, CTF_K_INTEGER));
}

ctf_id_t
ctf_add_float (ctf_file_t *fp, uint32_t flag,
	       const char *name, const ctf_encoding_t *ep)
{
  return (ctf_add_encoded (fp, flag, name, ep, CTF_K_FLOAT));
}

ctf_id_t
ctf_add_pointer (ctf_file_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_POINTER));
}

ctf_id_t
ctf_add_array (ctf_file_t *fp, uint32_t flag, const ctf_arinfo_t *arp)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  ctf_file_t *tmp = fp;

  if (arp == NULL)
    return (ctf_set_errno (fp, EINVAL));

  if (ctf_lookup_by_id (&tmp, arp->ctr_contents) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  tmp = fp;
  if (ctf_lookup_by_id (&tmp, arp->ctr_index) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((type = ctf_add_generic (fp, flag, NULL, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_ARRAY, flag, 0);
  dtd->dtd_data.ctt_size = 0;
  dtd->dtd_u.dtu_arr = *arp;

  return type;
}

int
ctf_set_array (ctf_file_t *fp, ctf_id_t type, const ctf_arinfo_t *arp)
{
  ctf_dtdef_t *dtd = ctf_dtd_lookup (fp, type);

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (dtd == NULL
      || LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info) != CTF_K_ARRAY)
    return (ctf_set_errno (fp, ECTF_BADID));

  fp->ctf_flags |= LCTF_DIRTY;
  dtd->dtd_u.dtu_arr = *arp;

  return 0;
}

ctf_id_t
ctf_add_function (ctf_file_t *fp, uint32_t flag,
		  const ctf_funcinfo_t *ctc, const ctf_id_t *argv)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  uint32_t vlen;
  ctf_id_t *vdat = NULL;
  ctf_file_t *tmp = fp;
  size_t i;

  if (ctc == NULL || (ctc->ctc_flags & ~CTF_FUNC_VARARG) != 0
      || (ctc->ctc_argc != 0 && argv == NULL))
    return (ctf_set_errno (fp, EINVAL));

  vlen = ctc->ctc_argc;
  if (ctc->ctc_flags & CTF_FUNC_VARARG)
    vlen++;	       /* Add trailing zero to indicate varargs (see below).  */

  if (ctf_lookup_by_id (&tmp, ctc->ctc_return) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  for (i = 0; i < ctc->ctc_argc; i++)
    {
      tmp = fp;
      if (ctf_lookup_by_id (&tmp, argv[i]) == NULL)
	return CTF_ERR;		/* errno is set for us.  */
    }

  if (vlen > CTF_MAX_VLEN)
    return (ctf_set_errno (fp, EOVERFLOW));

  if (vlen != 0 && (vdat = ctf_alloc (sizeof (ctf_id_t) * vlen)) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  if ((type = ctf_add_generic (fp, flag, NULL, &dtd)) == CTF_ERR)
    {
      ctf_free (vdat);
      return CTF_ERR;		   /* errno is set for us.  */
    }

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_FUNCTION, flag, vlen);
  dtd->dtd_data.ctt_type = (uint32_t) ctc->ctc_return;

  memcpy (vdat, argv, sizeof (ctf_id_t) * ctc->ctc_argc);
  if (ctc->ctc_flags & CTF_FUNC_VARARG)
    vdat[vlen - 1] = 0;		   /* Add trailing zero to indicate varargs.  */
  dtd->dtd_u.dtu_argv = vdat;

  return type;
}

ctf_id_t
ctf_add_struct_sized (ctf_file_t *fp, uint32_t flag, const char *name,
		      size_t size)
{
  ctf_hash_t *hp = fp->ctf_structs;
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;

  /* Promote forwards to structs.  */

  if (name != NULL)
    {
      type = ctf_hash_lookup_type (hp, fp, name);
      if (type == 0)
	type = ctf_dtd_lookup_type_by_name (fp, CTF_K_STRUCT, name);
    }

  if (type != 0 && ctf_type_kind (fp, type) == CTF_K_FORWARD)
    dtd = ctf_dtd_lookup (fp, type);
  else if ((type = ctf_add_generic (fp, flag, name, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_STRUCT, flag, 0);

  if (size > CTF_MAX_SIZE)
    {
      dtd->dtd_data.ctt_size = CTF_LSIZE_SENT;
      dtd->dtd_data.ctt_lsizehi = CTF_SIZE_TO_LSIZE_HI (size);
      dtd->dtd_data.ctt_lsizelo = CTF_SIZE_TO_LSIZE_LO (size);
    }
  else
    dtd->dtd_data.ctt_size = (uint32_t) size;

  return type;
}

ctf_id_t
ctf_add_struct (ctf_file_t *fp, uint32_t flag, const char *name)
{
  return (ctf_add_struct_sized (fp, flag, name, 0));
}

ctf_id_t
ctf_add_union_sized (ctf_file_t *fp, uint32_t flag, const char *name,
		     size_t size)
{
  ctf_hash_t *hp = fp->ctf_unions;
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;

  /* Promote forwards to unions.  */
  if (name != NULL)
    {
      type = ctf_hash_lookup_type (hp, fp, name);
      if (type == 0)
	type = ctf_dtd_lookup_type_by_name (fp, CTF_K_UNION, name);
    }

  if (type != 0 && ctf_type_kind (fp, type) == CTF_K_FORWARD)
    dtd = ctf_dtd_lookup (fp, type);
  else if ((type = ctf_add_generic (fp, flag, name, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_UNION, flag, 0);

  if (size > CTF_MAX_SIZE)
    {
      dtd->dtd_data.ctt_size = CTF_LSIZE_SENT;
      dtd->dtd_data.ctt_lsizehi = CTF_SIZE_TO_LSIZE_HI (size);
      dtd->dtd_data.ctt_lsizelo = CTF_SIZE_TO_LSIZE_LO (size);
    }
  else
    dtd->dtd_data.ctt_size = (uint32_t) size;

  return type;
}

ctf_id_t
ctf_add_union (ctf_file_t *fp, uint32_t flag, const char *name)
{
  return (ctf_add_union_sized (fp, flag, name, 0));
}

ctf_id_t
ctf_add_enum (ctf_file_t *fp, uint32_t flag, const char *name)
{
  ctf_hash_t *hp = fp->ctf_enums;
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;

  /* Promote forwards to enums.  */
  if (name != NULL)
    {
      type = ctf_hash_lookup_type (hp, fp, name);
      if (type == 0)
	type = ctf_dtd_lookup_type_by_name (fp, CTF_K_ENUM, name);
    }

  if (type != 0 && ctf_type_kind (fp, type) == CTF_K_FORWARD)
    dtd = ctf_dtd_lookup (fp, type);
  else if ((type = ctf_add_generic (fp, flag, name, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_ENUM, flag, 0);
  dtd->dtd_data.ctt_size = fp->ctf_dmodel->ctd_int;

  return type;
}

ctf_id_t
ctf_add_enum_encoded (ctf_file_t *fp, uint32_t flag, const char *name,
		      const ctf_encoding_t *ep)
{
  ctf_hash_t *hp = fp->ctf_enums;
  ctf_id_t type = 0;

  /* First, create the enum if need be, using most of the same machinery as
     ctf_add_enum(), to ensure that we do not allow things past that are not
     enums or forwards to them.  (This includes other slices: you cannot slice a
     slice, which would be a useless thing to do anyway.)  */

  if (name != NULL)
    {
      type = ctf_hash_lookup_type (hp, fp, name);
      if (type == 0)
	type = ctf_dtd_lookup_type_by_name (fp, CTF_K_ENUM, name);
    }

  if (type != 0)
    {
      if ((ctf_type_kind (fp, type) != CTF_K_FORWARD) &&
	  (ctf_type_kind_unsliced (fp, type) != CTF_K_ENUM))
	return (ctf_set_errno (fp, ECTF_NOTINTFP));
    }
  else if ((type = ctf_add_enum (fp, flag, name)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  /* Now attach a suitable slice to it.  */

  return ctf_add_slice (fp, flag, type, ep);
}

ctf_id_t
ctf_add_forward (ctf_file_t *fp, uint32_t flag, const char *name,
		 uint32_t kind)
{
  ctf_hash_t *hp;
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;

  switch (kind)
    {
    case CTF_K_STRUCT:
      hp = fp->ctf_structs;
      break;
    case CTF_K_UNION:
      hp = fp->ctf_unions;
      break;
    case CTF_K_ENUM:
      hp = fp->ctf_enums;
      break;
    default:
      return (ctf_set_errno (fp, ECTF_NOTSUE));
    }

  /* If the type is already defined or exists as a forward tag, just
     return the ctf_id_t of the existing definition.  */

  if (name != NULL)
    {
      if (((type = ctf_hash_lookup_type (hp, fp, name)) != 0)
	  || (type = ctf_dtd_lookup_type_by_name (fp, kind, name)) != 0)
	return type;
    }

  if ((type = ctf_add_generic (fp, flag, name, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_FORWARD, flag, 0);
  dtd->dtd_data.ctt_type = kind;

  return type;
}

ctf_id_t
ctf_add_typedef (ctf_file_t *fp, uint32_t flag, const char *name,
		 ctf_id_t ref)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  ctf_file_t *tmp = fp;

  if (ref == CTF_ERR || ref < 0 || ref > CTF_MAX_TYPE)
    return (ctf_set_errno (fp, EINVAL));

  if (ctf_lookup_by_id (&tmp, ref) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((type = ctf_add_generic (fp, flag, name, &dtd)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (CTF_K_TYPEDEF, flag, 0);
  dtd->dtd_data.ctt_type = (uint32_t) ref;

  return type;
}

ctf_id_t
ctf_add_volatile (ctf_file_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_VOLATILE));
}

ctf_id_t
ctf_add_const (ctf_file_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_CONST));
}

ctf_id_t
ctf_add_restrict (ctf_file_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_RESTRICT));
}

int
ctf_add_enumerator (ctf_file_t *fp, ctf_id_t enid, const char *name,
		    int value)
{
  ctf_dtdef_t *dtd = ctf_dtd_lookup (fp, enid);
  ctf_dmdef_t *dmd;

  uint32_t kind, vlen, root;
  char *s;

  if (name == NULL)
    return (ctf_set_errno (fp, EINVAL));

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (dtd == NULL)
    return (ctf_set_errno (fp, ECTF_BADID));

  kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);
  root = LCTF_INFO_ISROOT (fp, dtd->dtd_data.ctt_info);
  vlen = LCTF_INFO_VLEN (fp, dtd->dtd_data.ctt_info);

  if (kind != CTF_K_ENUM)
    return (ctf_set_errno (fp, ECTF_NOTENUM));

  if (vlen == CTF_MAX_VLEN)
    return (ctf_set_errno (fp, ECTF_DTFULL));

  for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
       dmd != NULL; dmd = ctf_list_next (dmd))
    {
      if (strcmp (dmd->dmd_name, name) == 0)
	return (ctf_set_errno (fp, ECTF_DUPLICATE));
    }

  if ((dmd = ctf_alloc (sizeof (ctf_dmdef_t))) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  if ((s = ctf_strdup (name)) == NULL)
    {
      ctf_free (dmd);
      return (ctf_set_errno (fp, EAGAIN));
    }

  dmd->dmd_name = s;
  dmd->dmd_type = CTF_ERR;
  dmd->dmd_offset = 0;
  dmd->dmd_value = value;

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (kind, root, vlen + 1);
  ctf_list_append (&dtd->dtd_u.dtu_members, dmd);

  fp->ctf_dtvstrlen += strlen (s) + 1;
  fp->ctf_flags |= LCTF_DIRTY;

  return 0;
}

int
ctf_add_member_offset (ctf_file_t *fp, ctf_id_t souid, const char *name,
		       ctf_id_t type, unsigned long bit_offset)
{
  ctf_dtdef_t *dtd = ctf_dtd_lookup (fp, souid);
  ctf_dmdef_t *dmd;

  ssize_t msize, malign, ssize;
  uint32_t kind, vlen, root;
  char *s = NULL;

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (dtd == NULL)
    return (ctf_set_errno (fp, ECTF_BADID));

  kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);
  root = LCTF_INFO_ISROOT (fp, dtd->dtd_data.ctt_info);
  vlen = LCTF_INFO_VLEN (fp, dtd->dtd_data.ctt_info);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
    return (ctf_set_errno (fp, ECTF_NOTSOU));

  if (vlen == CTF_MAX_VLEN)
    return (ctf_set_errno (fp, ECTF_DTFULL));

  if (name != NULL)
    {
      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
	   dmd != NULL; dmd = ctf_list_next (dmd))
	{
	  if (dmd->dmd_name != NULL && strcmp (dmd->dmd_name, name) == 0)
	    return (ctf_set_errno (fp, ECTF_DUPLICATE));
	}
    }

  if ((msize = ctf_type_size (fp, type)) == CTF_ERR ||
      (malign = ctf_type_align (fp, type)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  if ((dmd = ctf_alloc (sizeof (ctf_dmdef_t))) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  if (name != NULL && (s = ctf_strdup (name)) == NULL)
    {
      ctf_free (dmd);
      return (ctf_set_errno (fp, EAGAIN));
    }

  dmd->dmd_name = s;
  dmd->dmd_type = type;
  dmd->dmd_value = -1;

  if (kind == CTF_K_STRUCT && vlen != 0)
    {
      if (bit_offset == (unsigned long) - 1)
	{
	  /* Natural alignment.  */

	  ctf_dmdef_t *lmd = ctf_list_prev (&dtd->dtd_u.dtu_members);
	  ctf_id_t ltype = ctf_type_resolve (fp, lmd->dmd_type);
	  size_t off = lmd->dmd_offset;

	  ctf_encoding_t linfo;
	  ssize_t lsize;

	  if (ctf_type_encoding (fp, ltype, &linfo) != CTF_ERR)
	    off += linfo.cte_bits;
	  else if ((lsize = ctf_type_size (fp, ltype)) != CTF_ERR)
	    off += lsize * NBBY;

	  /* Round up the offset of the end of the last member to
	     the next byte boundary, convert 'off' to bytes, and
	     then round it up again to the next multiple of the
	     alignment required by the new member.  Finally,
	     convert back to bits and store the result in
	     dmd_offset.  Technically we could do more efficient
	     packing if the new member is a bit-field, but we're
	     the "compiler" and ANSI says we can do as we choose.  */

	  off = roundup (off, NBBY) / NBBY;
	  off = roundup (off, MAX (malign, 1));
	  dmd->dmd_offset = off * NBBY;
	  ssize = off + msize;
	}
      else
	{
	  /* Specified offset in bits.  */

	  dmd->dmd_offset = bit_offset;
	  ssize = ctf_get_ctt_size (fp, &dtd->dtd_data, NULL, NULL);
	  ssize = MAX (ssize, (bit_offset / NBBY) + msize);
	}
    }
  else
    {
      dmd->dmd_offset = 0;
      ssize = ctf_get_ctt_size (fp, &dtd->dtd_data, NULL, NULL);
      ssize = MAX (ssize, msize);
    }

  if (ssize > CTF_MAX_SIZE)
    {
      dtd->dtd_data.ctt_size = CTF_LSIZE_SENT;
      dtd->dtd_data.ctt_lsizehi = CTF_SIZE_TO_LSIZE_HI (ssize);
      dtd->dtd_data.ctt_lsizelo = CTF_SIZE_TO_LSIZE_LO (ssize);
    }
  else
    dtd->dtd_data.ctt_size = (uint32_t) ssize;

  dtd->dtd_data.ctt_info = CTF_TYPE_INFO (kind, root, vlen + 1);
  ctf_list_append (&dtd->dtd_u.dtu_members, dmd);

  if (s != NULL)
    fp->ctf_dtvstrlen += strlen (s) + 1;

  fp->ctf_flags |= LCTF_DIRTY;
  return 0;
}

int
ctf_add_member_encoded (ctf_file_t *fp, ctf_id_t souid, const char *name,
			ctf_id_t type, unsigned long bit_offset,
			const ctf_encoding_t encoding)
{
  ctf_dtdef_t *dtd = ctf_dtd_lookup (fp, type);
  int kind = LCTF_INFO_KIND (fp, dtd->dtd_data.ctt_info);
  int otype = type;

  if ((kind != CTF_K_INTEGER) && (kind != CTF_K_FLOAT) && (kind != CTF_K_ENUM))
    return (ctf_set_errno (fp, ECTF_NOTINTFP));

  if ((type = ctf_add_slice (fp, CTF_ADD_NONROOT, otype, &encoding)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  return ctf_add_member_offset (fp, souid, name, type, bit_offset);
}

int
ctf_add_member (ctf_file_t *fp, ctf_id_t souid, const char *name,
		ctf_id_t type)
{
  return ctf_add_member_offset (fp, souid, name, type, (unsigned long) - 1);
}

int
ctf_add_variable (ctf_file_t *fp, const char *name, ctf_id_t ref)
{
  ctf_dvdef_t *dvd;
  ctf_file_t *tmp = fp;

  if (!(fp->ctf_flags & LCTF_RDWR))
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (ctf_dvd_lookup (fp, name) != NULL)
    return (ctf_set_errno (fp, ECTF_DUPLICATE));

  if (ctf_lookup_by_id (&tmp, ref) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((dvd = ctf_alloc (sizeof (ctf_dvdef_t))) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  if (name != NULL && (dvd->dvd_name = ctf_strdup (name)) == NULL)
    {
      ctf_free (dvd);
      return (ctf_set_errno (fp, EAGAIN));
    }
  dvd->dvd_type = ref;
  dvd->dvd_snapshots = fp->ctf_snapshots;

  ctf_dvd_insert (fp, dvd);

  fp->ctf_dtvstrlen += strlen (name) + 1;
  fp->ctf_flags |= LCTF_DIRTY;
  return 0;
}

/* Write the compressed CTF data stream to the specified gzFile descriptor.
   This is useful for saving the results of dynamic CTF containers.  */
int
ctf_gzwrite (ctf_file_t *fp, gzFile fd)
{
  const unsigned char *buf = fp->ctf_base;
  ssize_t resid = fp->ctf_size;
  ssize_t len;

  while (resid != 0)
    {
      if ((len = gzwrite (fd, buf, resid)) <= 0)
	return (ctf_set_errno (fp, errno));
      resid -= len;
      buf += len;
    }

  return 0;
}

/* Compress the specified CTF data stream and write it to the specified file
   descriptor.  */
int
ctf_compress_write (ctf_file_t *fp, int fd)
{
  unsigned char *buf;
  unsigned char *bp;
  ctf_header_t h;
  ctf_header_t *hp = &h;
  ssize_t header_len = sizeof (ctf_header_t);
  ssize_t compress_len;
  size_t max_compress_len = compressBound (fp->ctf_size - header_len);
  ssize_t len;
  int rc;
  int err = 0;

  memcpy (hp, fp->ctf_base, header_len);
  hp->cth_flags |= CTF_F_COMPRESS;

  if ((buf = ctf_data_alloc (max_compress_len)) == NULL)
    return (ctf_set_errno (fp, ECTF_ZALLOC));

  compress_len = max_compress_len;
  if ((rc = compress (buf, (uLongf *) & compress_len,
		      fp->ctf_base + header_len,
		      fp->ctf_size - header_len)) != Z_OK)
    {
      ctf_dprintf ("zlib deflate err: %s\n", zError (rc));
      err = ctf_set_errno (fp, ECTF_COMPRESS);
      goto ret;
    }

  while (header_len > 0)
    {
      if ((len = write (fd, hp, header_len)) < 0)
	{
	  err = ctf_set_errno (fp, errno);
	  goto ret;
	}
      header_len -= len;
      hp += len;
    }

  bp = buf;
  while (compress_len > 0)
    {
      if ((len = write (fd, bp, compress_len)) < 0)
	{
	  err = ctf_set_errno (fp, errno);
	  goto ret;
	}
      compress_len -= len;
      bp += len;
    }

ret:
  ctf_data_free (buf, max_compress_len);
  return err;
}

/* Write the uncompressed CTF data stream to the specified file descriptor.
   This is useful for saving the results of dynamic CTF containers.  */
int
ctf_write (ctf_file_t *fp, int fd)
{
  const unsigned char *buf = fp->ctf_base;
  ssize_t resid = fp->ctf_size;
  ssize_t len;

  while (resid != 0)
    {
      if ((len = write (fd, buf, resid)) < 0)
	return (ctf_set_errno (fp, errno));
      resid -= len;
      buf += len;
    }

  return 0;
}
