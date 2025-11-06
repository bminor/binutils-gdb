/* CTF dict creation.
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
#include <string.h>
#include <unistd.h>

#ifndef EOVERFLOW
#define EOVERFLOW ERANGE
#endif

#ifndef roundup
#define roundup(x, y)  ((((x) + ((y) - 1)) / (y)) * (y))
#endif

/* The initial size of a dynamic type's vlen in members.  Arbitrary: the bigger
   this is, the less allocation needs to be done for small structure
   initialization, and the more memory is wasted for small structures during CTF
   construction.  No effect on generated CTF or ctf_open()ed CTF. */
#define INITIAL_VLEN 16

/* Make sure the ptrtab has enough space for at least one more type.

   We start with 4KiB of ptrtab, enough for a thousand types, then grow it 25%
   at a time.  */

static int
ctf_grow_ptrtab (ctf_dict_t *fp)
{
  size_t new_ptrtab_len = fp->ctf_ptrtab_len;

  /* We allocate one more ptrtab entry than we need, for the initial zero,
     plus one because the caller will probably allocate a new type.

     Equally, if the ptrtab is small -- perhaps due to ctf_open of a small
     dict -- boost it by quite a lot at first, so we don't need to keep
     realloc()ing.  */

  if (fp->ctf_ptrtab == NULL || fp->ctf_ptrtab_len < 1024)
    new_ptrtab_len = 1024;
  else if ((fp->ctf_typemax + 2) > fp->ctf_ptrtab_len)
    new_ptrtab_len = fp->ctf_ptrtab_len * 1.25;

  if (new_ptrtab_len != fp->ctf_ptrtab_len)
    {
      uint32_t *new_ptrtab;

      if ((new_ptrtab = realloc (fp->ctf_ptrtab,
				 new_ptrtab_len * sizeof (uint32_t))) == NULL)
	return (ctf_set_errno (fp, ENOMEM));

      fp->ctf_ptrtab = new_ptrtab;
      memset (fp->ctf_ptrtab + fp->ctf_ptrtab_len, 0,
	      (new_ptrtab_len - fp->ctf_ptrtab_len) * sizeof (uint32_t));
      fp->ctf_ptrtab_len = new_ptrtab_len;
    }
  return 0;
}

/* Make sure a vlen has enough space: expand it otherwise.  Either grow it to
   roughly enough space for VBYTES, or add precisely VBYTES on to the space
   reserved for the vlen.  (In one mode, VBYTES is a minimum: in the other,
   it's an addend.  */

static int
ctf_add_vlen (ctf_dict_t *fp, ctf_dtdef_t *dtd, size_t vbytes, int additive)
{
  unsigned char *old = (unsigned char *) dtd->dtd_buf;

  size_t size = dtd->dtd_buf_size;
  size_t old_size = size;
  size_t vlen_size = dtd->dtd_vlen_size;
  size_t prefix_size = size - vlen_size;
  size_t old_data_index = dtd->dtd_data - dtd->dtd_buf;
  size_t old_vlen_offset = dtd->dtd_vlen - old;

  if (!additive)
    {
      if ((size - vlen_size) > vbytes)
	return 0;

      while (vlen_size < vbytes)
	{
	  size *= 2;
	  vlen_size = size - prefix_size;
	}
    }
  else
    {
      vlen_size += vbytes;
      size += vbytes;
    }

  if ((dtd->dtd_buf = realloc (dtd->dtd_buf, size)) == NULL)
    {
      dtd->dtd_buf = (ctf_type_t *) old;
      return (ctf_set_errno (fp, ENOMEM));
    }

  memset (((unsigned char *) dtd->dtd_buf) + old_size, 0, size - old_size);
  dtd->dtd_data = dtd->dtd_buf + old_data_index;
  dtd->dtd_vlen = ((unsigned char *) dtd->dtd_buf) + old_vlen_offset;
  dtd->dtd_buf_size = size;

  return 0;
}

/* Make sure a vlen has enough space: expand it otherwise.  Grow it in fairly
   big jumps, for amortized-constant-time growth.  */

static int
ctf_grow_vlen (ctf_dict_t *fp, ctf_dtdef_t *dtd, size_t vbytes)
{
  return ctf_add_vlen (fp, dtd, vbytes, 0);
}

/* Add a prefix to a given DTD, at the end of the prefix chain, and return it.
   Make sure the vlen has enough room for at least VBYTES bytes, too. */
static ctf_type_t *
ctf_add_prefix (ctf_dict_t *fp, ctf_dtdef_t *dtd, size_t vbytes)
{
  ctf_type_t *new_prefix;
  size_t old_buf_size = dtd->dtd_buf_size;

  /* Grow the type, then tweak the vlen forwards and move things around to leave
     a gap.  If we run off the end of the headers without finding a non-prefix,
     something is wrong.  */

  if (vbytes == 0)
    {
      if (ctf_add_vlen (fp, dtd, sizeof (ctf_type_t), 1) < 0)
	return NULL;				/* errno is set for us.  */
    }
  else
    {
      if (ctf_grow_vlen (fp, dtd, vbytes + sizeof (ctf_type_t)) < 0)
	return NULL;				/* errno is set for us.  */
    }

  new_prefix = dtd->dtd_data;
  memmove (dtd->dtd_data + 1, dtd->dtd_data, old_buf_size
	   - ((unsigned char *) dtd->dtd_data - (unsigned char *) dtd->dtd_buf));
  dtd->dtd_vlen += sizeof (ctf_type_t);
  dtd->dtd_data++;
  memset (new_prefix, 0, sizeof (ctf_type_t));

  return new_prefix;
}

/* To create an empty CTF dict, we just declare a zeroed header and call
   ctf_bufopen() on it.  If ctf_bufopen succeeds, we mark the new dict r/w and
   initialize the dynamic members.  We start assigning type IDs at 1 because
   type ID 0 is used as a sentinel and a not-found indicator.  */

ctf_dict_t *
ctf_create (ctf_error_t *errp)
{
  static ctf_header_t hdr =
    {
      .btf.bth_preamble = { CTF_BTF_MAGIC, CTF_BTF_VERSION, 0 },
      .btf.bth_hdr_len = sizeof (ctf_btf_header_t),
    };

  ctf_dynhash_t *structs = NULL, *unions = NULL, *enums = NULL, *names = NULL;
  ctf_dynhash_t *datasecs = NULL, *tags = NULL;
  ctf_sect_t cts;
  ctf_dict_t *fp;

  libctf_init_debug();

  hdr.cth_preamble.ctp_magic_version = (CTFv4_MAGIC << 16) | CTF_VERSION;

  structs = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
				NULL, NULL);
  unions = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			       NULL, NULL);
  enums = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			      NULL, NULL);
  names = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			      NULL, NULL);
  datasecs = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
				 NULL, NULL);
  tags = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			     NULL, (ctf_hash_free_fun) ctf_dynset_destroy);
  if (!structs || !unions || !enums || !names || !datasecs || !tags)
    {
      ctf_set_open_errno (errp, EAGAIN);
      goto err;
    }

  cts.cts_name = _CTF_SECTION;
  cts.cts_data = &hdr;
  cts.cts_size = sizeof (hdr);
  cts.cts_entsize = 1;

  if ((fp = ctf_bufopen (&cts, NULL, NULL, errp)) == NULL)
    goto err;

  /* These hashes will have been initialized with a starting size of zero,
     which is surely wrong.  Use ones with slightly larger sizes.  */
  ctf_dynhash_destroy (fp->ctf_structs);
  ctf_dynhash_destroy (fp->ctf_unions);
  ctf_dynhash_destroy (fp->ctf_enums);
  ctf_dynhash_destroy (fp->ctf_names);
  ctf_dynhash_destroy (fp->ctf_datasecs);
  ctf_dynhash_destroy (fp->ctf_tags);
  fp->ctf_structs = structs;
  fp->ctf_unions = unions;
  fp->ctf_enums = enums;
  fp->ctf_names = names;
  fp->ctf_datasecs = datasecs;
  fp->ctf_tags = tags;
  fp->ctf_dtoldid = 0;
  fp->ctf_snapshot_lu = 0;

  /* Make sure the ptrtab starts out at a reasonable size.  */

  ctf_set_ctl_hashes (fp);
  if (ctf_grow_ptrtab (fp) < 0)
    {
      ctf_set_open_errno (errp, ctf_errno (fp));
      ctf_dict_close (fp);
      return NULL;
    }

  return fp;

 err:
  ctf_dynhash_destroy (structs);
  ctf_dynhash_destroy (unions);
  ctf_dynhash_destroy (enums);
  ctf_dynhash_destroy (names);
  ctf_dynhash_destroy (datasecs);
  ctf_dynhash_destroy (tags);
  return NULL;
}

/* Compatibility: just update the threshold for ctf_discard.  */
ctf_ret_t
ctf_update (ctf_dict_t *fp)
{
  fp->ctf_dtoldid = fp->ctf_typemax;
  return 0;
}

ctf_dynhash_t *
ctf_name_table (ctf_dict_t *fp, int kind)
{
  switch (kind)
    {
    case CTF_K_STRUCT:
      return fp->ctf_structs;
    case CTF_K_UNION:
      return fp->ctf_unions;
    case CTF_K_ENUM:
    case CTF_K_ENUM64:
      return fp->ctf_enums;
    case CTF_K_TYPE_TAG:
    case CTF_K_DECL_TAG:
      return fp->ctf_tags;
    case CTF_K_DATASEC:
      return fp->ctf_datasecs;
    default:
      return fp->ctf_names;
    }
}

int
ctf_insert_type_decl_tag (ctf_dict_t *fp, ctf_id_t type, const char *name,
			  int kind)
{
  ctf_dynset_t *types;
  ctf_dynhash_t *h = ctf_name_table (fp, kind);
  ctf_error_t err;

  if ((types = ctf_dynhash_lookup (h, name)) == NULL)
    {
      types = ctf_dynset_create (htab_hash_pointer, htab_eq_pointer, NULL);

      if (!types)
	return (ctf_set_errno (fp, ENOMEM));

      err = ctf_dynhash_cinsert (h, name, types);
      if (err != 0)
	{
	  err *= -1;
	  return (ctf_set_errno (fp, err));
	}
    }

  if ((err = ctf_dynset_insert (types, (void *) (uintptr_t) type)) != 0)
    {
      err *= -1;
      return (ctf_set_errno (fp, err));
    }
  return 0;
}

int
ctf_dtd_insert (ctf_dict_t *fp, ctf_dtdef_t *dtd, int flag, int kind)
{
  const char *name;
  if (ctf_dynhash_insert (fp->ctf_dthash, (void *) (uintptr_t) dtd->dtd_type,
			  dtd) < 0)
    return ctf_set_errno (fp, ENOMEM);

  if (flag == CTF_ADD_ROOT && dtd->dtd_data->ctt_name
      && (name = ctf_strraw (fp, dtd->dtd_data->ctt_name)) != NULL)
    {
      /* Type and decl tags have unusual name tables, since their names are not
	 unique.  */

      if (kind != CTF_K_TYPE_TAG && kind != CTF_K_DECL_TAG)
	{
	  if (ctf_dynhash_insert (ctf_name_table (fp, kind),
				  (char *) name, (void *) (uintptr_t)
				  dtd->dtd_type) < 0)
	    {
	      ctf_dynhash_remove (fp->ctf_dthash, (void *) (uintptr_t)
				  dtd->dtd_type);
	      return ctf_set_errno (fp, ENOMEM);
	    }
	}
      else if (ctf_insert_type_decl_tag (fp, dtd->dtd_type, name, kind) < 0)
	return -1;			/* errno is set for us.  */
    }
  ctf_list_append (&fp->ctf_dtdefs, dtd);
  return 0;
}

void
ctf_dtd_delete (ctf_dict_t *fp, ctf_dtdef_t *dtd)
{
  const char *name = ctf_type_name_raw (fp, dtd->dtd_type);
  int name_kind = ctf_type_kind_forwarded (fp, dtd->dtd_type);

  /* Repeated calls should do nothing.  */
  if (name_kind < 0 && ctf_errno (fp) == ECTF_BADID)
    return;

  ctf_dynhash_remove (fp->ctf_dthash, (void *) (uintptr_t) dtd->dtd_type);

  if (name != NULL && name[0] != '\0'
      && LCTF_INFO_ISROOT (fp, dtd->dtd_buf->ctt_info))
    ctf_dynhash_remove (ctf_name_table (fp, name_kind), name);

  free (dtd->dtd_buf);
  ctf_list_delete (&fp->ctf_dtdefs, dtd);
  free (dtd);
}

ctf_dtdef_t *
ctf_dtd_lookup (const ctf_dict_t *fp, ctf_id_t type)
{
  fp = ctf_get_dict (fp, type);

  return (ctf_dtdef_t *)
    ctf_dynhash_lookup (fp->ctf_dthash, (void *) (uintptr_t) type);
}

ctf_dtdef_t *
ctf_dynamic_type (const ctf_dict_t *fp, ctf_id_t type)
{
  ctf_id_t idx;

  fp = ctf_get_dict (fp, type);

  idx = ctf_type_to_index (fp, type);

  if ((unsigned long) idx > fp->ctf_stypes)
    return ctf_dtd_lookup (fp, type);
  return NULL;
}

static int
ctf_static_type (const ctf_dict_t *fp, ctf_id_t type)
{
  ctf_id_t idx;

  fp = ctf_get_dict (fp, type);

  idx = ctf_type_to_index (fp, type);

  return ((unsigned long) idx <= fp->ctf_stypes);
}

/* Discard all of the dynamic type definitions that have been added to the dict
   since the last call to ctf_update().  We locate such types by scanning the
   dtd list and deleting elements that have indexes greater than ctf_dtoldid,
   which is set by ctf_update(), above.  */
ctf_ret_t
ctf_discard (ctf_dict_t *fp)
{
  ctf_snapshot_id_t last_update =
    { fp->ctf_dtoldid,
      fp->ctf_snapshot_lu + 1 };

  return (ctf_rollback (fp, last_update));
}

ctf_snapshot_id_t
ctf_snapshot (ctf_dict_t *fp)
{
  ctf_snapshot_id_t snapid;
  snapid.dtd_id = fp->ctf_typemax;
  snapid.snapshot_id = fp->ctf_snapshots++;
  return snapid;
}

/* Like ctf_discard(), only discards everything after a particular ID.  */
ctf_ret_t
ctf_rollback (ctf_dict_t *fp, ctf_snapshot_id_t id)
{
  ctf_dtdef_t *dtd, *ntd;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if (id.snapshot_id < fp->ctf_stypes)
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (fp->ctf_snapshot_lu >= id.snapshot_id)
    return (ctf_set_errno (fp, ECTF_OVERROLLBACK));

  for (dtd = ctf_list_next (&fp->ctf_dtdefs); dtd != NULL; dtd = ntd)
    {
      ntd = ctf_list_next (dtd);

      if (ctf_type_to_index (fp, dtd->dtd_type) <= id.dtd_id)
	continue;
      ctf_dtd_delete (fp, dtd);
    }

  fp->ctf_typemax = id.dtd_id;
  fp->ctf_snapshots = id.snapshot_id;

  return 0;
}

/* Assign an ID to a newly-created type.

   The type ID assignment scheme in libctf divides types into three
   classes.

   - static types are types read in from an already-existing dict.  They are
     stored only in the ctf_buf and have type indexes ranging from 1 up to
     fp->ctf_typemax (usually the same as fp->ctf_stypes, but may be differnt
     for newly-created children just imported to parents with already-present
     dynamic types).  Their IDs are derived from their index in the ctf_buf and
     are not explicitly assigned, though serialization tracks them in order to
     update type IDs that reference them.

     Type IDs in a child dict start from fp->ctf_header->ctf_parent_typemax
     (fp->ctf_stypes in the parent).  There is no gap as in CTFv3 and below:
     the IDs run continuously.

   - dynamic types are added by ctf_add_*() (ultimately, ctf_add_generic) and
     have DTDs: their type IDs are stored in dtd->dtd_type, and the DTD hashtab
     is indexed by type ID.

     The simplest form of these types, nonprovisionally-numbered dynamic types,
     have type IDs stretching from fp->ctf_stypes up to fp->ctf_idmax, and
     corresponding indexes.  Such types only exist for child dicts and for
     parent dicts which had types added before any children were imported.

   - As soon as a child is imported, the parent starts allocating provisionally-
     numbered dynamic types from the top of the type space down, updating
     ctf_provtypemax and ctf_nprovtypes as it goes, and bumping ctf_typemax:
     ctf_idmax is no longer bumped.  The child continues to allocate in lower
     type space starting from the parent's ctf_idmax + 1.  Obviously all
     references to provisional types can't stick around: so at serialization
     time we note down the position of every reference to a provisional type ID
     and all child type IDs, then lay out the type table by going over the
     nonprovisional types and then the provisional ones and dropping them in
     place in their serialized buffers, work out what the final type IDs will
     be, and update all the refs accordingly, changing every type ID that refers
     to the old type to refer to the new one instead.  (See ctf_serialize.)

     The indexes of provisional types run identically to the indexes of
     non-provisional types, i.e. straight upwards without breaks or
     discontinuities, even though this probably overlaps type IDs in the child.
     Indexes and type IDs are not the same!

   At serialization time, we track references to type IDs in the same dict via
   the refs system while the type table et al are being built (during
   preserialization), and update them with the real type IDs at final
   serialization time; the final type IDs are recorded in the dtd_final_type,
   and we assert if a future serialization would assign a different ID (which
   should be impossible).  When child dicts are serialized, references to parent
   types are updated with the dtd_final_type of that type whenever one is set.
   It is considered an error to try to serialize a child while its parent has
   provisional types that have not yet had IDs assigned.

   (The refs system is not employed to track references from child dicts to
   parents, since forward references are not possible between dicts: the parent
   dict must have been completely serialized when serializing a child.  We can't
   be halfway through, which is the case the refs system is there to handle:
   refs from structure members to types not yet known, etc.)

   Only parents have provisional type IDs!  Child IDs are always simply assigned
   straight in the child.  This means that the provisional ID space is not
   sparse, and we don't need to worry about child and parent IDs being
   interspersed in it.  (Not yet, anyway: if we get multilevel parents this will
   become a concern).

   Note that you can add types to a parent at any time, even after children have
   been serialized.  This works fine, except that you cannot use the
   newly-written dict as a parent for the same children, since they were written
   out assuming a smaller number of types in the parent.  */

static ctf_id_t
ctf_assign_id (ctf_dict_t *fp)
{
  uint32_t idx;

  /* All type additions increase the max index.  */

  idx = ++fp->ctf_typemax;

  /* Is this a parent with an attached child?  Provisional type.  */

  if (!(fp->ctf_flags & LCTF_CHILD) && (fp->ctf_max_children > 0))
    {
      fp->ctf_provtypemax--;
      fp->ctf_nprovtypes++;
    }
  else
    fp->ctf_idmax++;

  return ctf_index_to_type (fp, idx);
}

/* Note: vbytes is the amount of space needed by the vlen, plus as many as are
   needed by the PREFIXES, plus any further prefixes (e.g. for hidden types).
   It is required to be the amount of space used, as recorded in the per-kind
   info word.  vbytes_extra is some extra space that can be allocated to reduce
   realloc calls.

   TYPEP is a pointer to either the actual type structure with the name in it,
   or to the first prefix requested by PREFIXES, if nonzero.  */

static ctf_dtdef_t *
ctf_add_generic (ctf_dict_t *fp, uint32_t flag, const char *name, int kind,
		 int prefixes, size_t vbytes, size_t vbytes_extra,
		 ctf_type_t **typep)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type;
  ctf_dict_t *pfp = fp;

  if (fp->ctf_parent)
    pfp = fp->ctf_parent;

  if (flag != CTF_ADD_NONROOT && flag != CTF_ADD_ROOT)
    {
      ctf_set_errno (fp, EINVAL);
      return NULL;
    }

  if (fp->ctf_typemax + 1 >= pfp->ctf_provtypemax)
    {
      ctf_set_errno (fp, ECTF_FULL);
      return NULL;
    }

  /* Prohibit addition of types in the middle of serialization.  */

  if (fp->ctf_flags & LCTF_NO_TYPE)
    {
      ctf_set_errno (fp, ECTF_NOTSERIALIZED);
      return NULL;
    }

  if (fp->ctf_flags & LCTF_NO_STR)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  if (fp->ctf_flags & LCTF_CHILD && fp->ctf_parent == NULL)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  /* Prohibit addition of a root-visible type that is already present
     in the non-dynamic portion.  Two exceptions: type and decl tags,
     whose identifier tables are unusual (duplicates are expected).  */

  if (flag == CTF_ADD_ROOT && name != NULL && name[0] != '\0'
      && kind != CTF_K_TYPE_TAG && kind != CTF_K_DECL_TAG)
    {
      ctf_id_t existing;

      if (((existing = ctf_dynhash_lookup_type (ctf_name_table (fp, kind),
						name)) > 0)
	  && ctf_static_type (fp, existing))
	{
	  ctf_set_errno (fp, ECTF_RDONLY);
	  return NULL;
	}
    }

  /* Make sure ptrtab always grows to be big enough for all types.  */
  if (ctf_grow_ptrtab (fp) < 0)
    return NULL;				/* errno is set for us. */

  if ((dtd = calloc (1, sizeof (ctf_dtdef_t))) == NULL)
    {
      ctf_set_typed_errno (fp, EAGAIN);
      return NULL;
    }

  dtd->dtd_buf_size = vbytes + vbytes_extra + sizeof (ctf_type_t);
  dtd->dtd_vlen_size = vbytes;

  /* The non-root flag is implemented via prefixes.  */
  if (flag == CTF_ADD_NONROOT)
    dtd->dtd_buf_size += sizeof (ctf_type_t);

  if (prefixes)
    dtd->dtd_buf_size += (sizeof (ctf_type_t) * prefixes);

  if ((dtd->dtd_buf = calloc (1, dtd->dtd_buf_size)) == NULL)
    goto oom;
  dtd->dtd_vlen = ((unsigned char *) dtd->dtd_buf) + dtd->dtd_buf_size
    - vbytes - vbytes_extra;
  dtd->dtd_data = (ctf_type_t *) (dtd->dtd_vlen - sizeof (ctf_type_t));

  type = ctf_assign_id (fp);

  /* Populate a conflicting type kind if need be.  This has vlen-in-bytes filled
     in if small enough to fit, to help prefix-unaware clients skip the prefix
     easily, but the vlen is otherwise redundant (and not used by libctf).  */
  if (flag == CTF_ADD_NONROOT)
    dtd->dtd_buf->ctt_info = CTF_TYPE_INFO (CTF_K_CONFLICTING, 0,
					    dtd->dtd_vlen_size < 65536
					    ? dtd->dtd_vlen_size : 0);

  dtd->dtd_data->ctt_name = ctf_str_add (fp, name);
  dtd->dtd_type = type;

  if (dtd->dtd_data->ctt_name == 0 && name != NULL && name[0] != '\0')
    goto oom;

  if (ctf_dtd_insert (fp, dtd, flag, kind) < 0)
    goto err;					/* errno is set for us.  */

  /* Return a pointer to the first user-requested prefix, if any.  i.e., don't
     return a pointer to the non-root CONFLICTING header.  */

  if (typep)
    *typep = dtd->dtd_buf + (flag == CTF_ADD_NONROOT);

  fp->ctf_serialize.cs_initialized = 0;

  return dtd;

 oom:
  ctf_set_errno (fp, EAGAIN);
 err:
  free (dtd->dtd_buf);
  free (dtd);
  return NULL;
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

ctf_id_t
ctf_add_encoded (ctf_dict_t *fp, uint32_t flag,
		 const char *name, const ctf_encoding_t *ep, uint32_t kind)
{
  ctf_dtdef_t *dtd;
  uint32_t encoding;
  int vlen = sizeof (uint32_t);

  if (ep == NULL)
    return (ctf_set_typed_errno (fp, EINVAL));

  if (name == NULL || name[0] == '\0')
    return (ctf_set_typed_errno (fp, ECTF_NONAME));

  if (!ctf_assert (fp, kind == CTF_K_INTEGER || kind == CTF_K_FLOAT
		   || kind == CTF_K_BTF_FLOAT))
    return CTF_ERR;				/* errno is set for us.  */

  if (kind == CTF_K_BTF_FLOAT)
    vlen = 0;

  if ((dtd = ctf_add_generic (fp, flag, name, kind, 0, vlen, 0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, 0, 0);
  dtd->dtd_data->ctt_size = clp2 (P2ROUNDUP (ep->cte_bits, CHAR_BIT) / CHAR_BIT);

  if (kind != CTF_K_BTF_FLOAT)
    {
      encoding = ep->cte_format;
      if (kind == CTF_K_INTEGER)
	encoding = CTF_INT_DATA (ep->cte_format, ep->cte_offset, ep->cte_bits);

      memcpy (dtd->dtd_vlen, &encoding, sizeof (encoding));
    }

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_reftype (ctf_dict_t *fp, uint32_t flag, ctf_id_t ref, uint32_t kind)
{
  ctf_dtdef_t *dtd;
  ctf_dict_t *typedict = fp;
  ctf_dict_t *refdict = fp;
  int child = fp->ctf_flags & LCTF_CHILD;
  uint32_t type_idx;
  uint32_t ref_idx;

  if (ref == CTF_ERR || ref > CTF_MAX_TYPE)
    return (ctf_set_typed_errno (fp, EINVAL));

  if (ref != 0 && ctf_lookup_by_id (&refdict, ref, NULL) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((dtd = ctf_add_generic (fp, flag, NULL, kind, 0, 0, 0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, 0, 0);
  dtd->dtd_data->ctt_type = (uint32_t) ref;

  if (kind != CTF_K_POINTER)
    return dtd->dtd_type;

  /* If we are adding a pointer, update the ptrtab, pointing at this type from
     the type it points to.  Note that ctf_typemax is at this point one higher
     than we want to check against, because it's just been incremented for the
     addition of this type.  The pptrtab is lazily-updated as needed, so is not
     touched here.  */

  typedict = ctf_get_dict (fp, dtd->dtd_type);

  type_idx = ctf_type_to_index (typedict, dtd->dtd_type);
  ref_idx = ctf_type_to_index (refdict, ref);

  if (ctf_type_ischild (fp, ref) == child
      && ref_idx < fp->ctf_typemax)
    fp->ctf_ptrtab[ref_idx] = type_idx;

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_slice (ctf_dict_t *fp, uint32_t flag, ctf_id_t ref,
	       const ctf_encoding_t *ep)
{
  ctf_dtdef_t *dtd;
  ctf_slice_t slice;
  ctf_id_t resolved_ref = ref;
  int kind;
  ctf_dict_t *tmp = fp;

  if (ep == NULL)
    return (ctf_set_typed_errno (fp, EINVAL));

  if ((ep->cte_bits > 255) || (ep->cte_offset > 255))
    return (ctf_set_typed_errno (fp, ECTF_SLICEOVERFLOW));

  if (ref == CTF_ERR || ref > CTF_MAX_TYPE)
    return (ctf_set_typed_errno (fp, EINVAL));

  if (ref != 0 && (ctf_lookup_by_id (&tmp, ref, NULL) == NULL))
    return CTF_ERR;		/* errno is set for us.  */

  /* Make sure we ultimately point to an integral type.  We also allow slices to
     point to the unimplemented type, for now, because the compiler can emit
     such slices, though they're not very much use.  */

  if ((resolved_ref = ctf_type_resolve_unsliced (fp, ref)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */
  kind = ctf_type_kind_unsliced (fp, resolved_ref);

  if ((kind != CTF_K_INTEGER) && (kind != CTF_K_FLOAT) &&
      (kind != CTF_K_ENUM) && (kind != CTF_K_BTF_FLOAT)
      && (ref != 0))
    return (ctf_set_typed_errno (fp, ECTF_NOTINTFP));

  if ((dtd = ctf_add_generic (fp, flag, NULL, CTF_K_SLICE, 0,
			      sizeof (ctf_slice_t), 0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  memset (&slice, 0, sizeof (ctf_slice_t));

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_SLICE, 0, 0);
  dtd->dtd_data->ctt_size = clp2 (P2ROUNDUP (ep->cte_bits, CHAR_BIT) / CHAR_BIT);
  slice.cts_type = (uint32_t) ref;
  slice.cts_bits = ep->cte_bits;
  slice.cts_offset = ep->cte_offset;
  memcpy (dtd->dtd_vlen, &slice, sizeof (ctf_slice_t));

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_integer (ctf_dict_t *fp, uint32_t flag,
		 const char *name, const ctf_encoding_t *ep)
{
  return (ctf_add_encoded (fp, flag, name, ep, CTF_K_INTEGER));
}

ctf_id_t
ctf_add_float (ctf_dict_t *fp, uint32_t flag,
	       const char *name, const ctf_encoding_t *ep)
{
  return (ctf_add_encoded (fp, flag, name, ep, CTF_K_FLOAT));
}

ctf_id_t
ctf_add_btf_float (ctf_dict_t *fp, uint32_t flag,
		   const char *name, const ctf_encoding_t *ep)
{
  return (ctf_add_encoded (fp, flag, name, ep, CTF_K_BTF_FLOAT));
}

ctf_id_t
ctf_add_pointer (ctf_dict_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_POINTER));
}

ctf_id_t
ctf_add_array (ctf_dict_t *fp, uint32_t flag, const ctf_arinfo_t *arp)
{
  ctf_dtdef_t *dtd;
  ctf_array_t *cta;
  ctf_dict_t *tmp = fp;

  if (arp == NULL)
    return (ctf_set_typed_errno (fp, EINVAL));

  if (arp->ctr_contents != 0
      && ctf_lookup_by_id (&tmp, arp->ctr_contents, NULL) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  tmp = fp;
  if (ctf_lookup_by_id (&tmp, arp->ctr_index, NULL) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if (ctf_type_kind (fp, arp->ctr_index) == CTF_K_FORWARD)
    {
      ctf_err_warn (fp, 1, ECTF_INCOMPLETE,
		    _("ctf_add_array: index type %lx is incomplete"),
		    arp->ctr_contents);
      return (ctf_set_typed_errno (fp, ECTF_INCOMPLETE));
    }

  if ((dtd = ctf_add_generic (fp, flag, NULL, CTF_K_ARRAY, 0,
			      sizeof (ctf_array_t), 0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  cta = (ctf_array_t *) dtd->dtd_vlen;
  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_ARRAY, 0, 0);
  dtd->dtd_data->ctt_size = 0;
  cta->cta_contents = (uint32_t) arp->ctr_contents;
  cta->cta_index = (uint32_t) arp->ctr_index;
  cta->cta_nelems = arp->ctr_nelems;

  return dtd->dtd_type;
}

ctf_ret_t
ctf_set_array (ctf_dict_t *fp, ctf_id_t type, const ctf_arinfo_t *arp)
{
  ctf_dict_t *ofp = fp;
  ctf_dtdef_t *dtd;
  ctf_array_t *vlen;
  uint32_t idx;

  if (ctf_lookup_by_id (&fp, type, NULL) == NULL)
    return -1;				/* errno is set for us.  */

  idx = ctf_type_to_index (fp, type);
  dtd = ctf_dtd_lookup (fp, type);

  /* You can only call ctf_set_array on a type you have added, not a
     type that was read in via ctf_open.  */
  if (idx < fp->ctf_stypes)
    return (ctf_set_errno (ofp, ECTF_RDONLY));

  if (dtd == NULL
      || LCTF_KIND (fp, dtd->dtd_buf) != CTF_K_ARRAY)
    return (ctf_set_errno (ofp, ECTF_BADID));

  vlen = (ctf_array_t *) dtd->dtd_vlen;
  vlen->cta_contents = (uint32_t) arp->ctr_contents;
  vlen->cta_index = (uint32_t) arp->ctr_index;
  vlen->cta_nelems = arp->ctr_nelems;

  return 0;
}

/* Set this type as conflicting in compilation unit CUNAME.  */
ctf_ret_t
ctf_set_conflicting (ctf_dict_t *fp, ctf_id_t type, const char *cuname)
{
  ctf_dict_t *ofp = fp;
  ctf_dtdef_t *dtd;
  ctf_type_t *prefix;
  uint32_t idx;

  if (ctf_lookup_by_id (&fp, type, NULL) == NULL)
    return -1;				/* errno is set for us.  */

  idx = ctf_type_to_index (fp, type);
  dtd = ctf_dtd_lookup (fp, type);

  /* You can only call ctf_set_conflicting on a type you have added, not a type
     that was read in via ctf_open.  */
  if (idx < fp->ctf_stypes)
    return (ctf_set_errno (ofp, ECTF_RDONLY));

  if (dtd == NULL)
    return (ctf_set_errno (ofp, ECTF_BADID));

  if ((prefix = (ctf_type_t *) ctf_find_prefix (fp, dtd->dtd_buf,
						CTF_K_CONFLICTING)) == NULL)
    {
      if ((prefix = ctf_add_prefix (fp, dtd, 0)) == NULL)
	return -1;			/* errno is set for us.  */
    }
  prefix->ctt_name = ctf_str_add (fp, cuname);
  prefix->ctt_info = CTF_TYPE_INFO (CTF_K_CONFLICTING, 0,
				    dtd->dtd_vlen_size < 65536
				    ? dtd->dtd_vlen_size : 0);

  fp->ctf_serialize.cs_initialized = 0;

  return 0;
}

/* Add a type or decl tag applying to some whole type, or to some
   component of a type.  Component -1 is a whole type.  */

static ctf_id_t
ctf_add_tag (ctf_dict_t *fp, uint32_t flag, ctf_id_t type, const char *tag,
	     int is_decl, int32_t component_idx)
{
  ctf_dtdef_t *dtd;
  size_t vlen_size = 0;
  int kind = is_decl ? CTF_K_DECL_TAG : CTF_K_TYPE_TAG;
  int ref_kind = ctf_type_kind (fp, type);

  if (component_idx < -1)
    return (ctf_set_typed_errno (fp, ECTF_BADCOMPONENT));

  if (is_decl)
    {
      vlen_size = sizeof (ctf_decl_tag_t);

      /* Whole-type declarations.  */

      if (component_idx == 0)
	{
	  switch (ref_kind)
	    {
	    case CTF_K_TYPEDEF:
	    case CTF_K_STRUCT:
	    case CTF_K_UNION:
	    case CTF_K_VAR:
	      /* TODO: support addition and querying on CTF_K_FUNCTION too, chasing back
		 to relevant CTF_K_FUNC_LINKAGEs.  */
	    case CTF_K_FUNC_LINKAGE:
	      break;
	    default:
	      return (ctf_set_typed_errno (fp, ECTF_BADID));
	    }
	}
    }
  else if (component_idx != -1)
    {
      ctf_id_t func_type = type;

      /* Within-type declarations.  */

      switch (ref_kind)
	{
	case CTF_K_STRUCT:
	case CTF_K_UNION:

	  if (component_idx >= ctf_member_count (fp, type))
	    return (ctf_set_typed_errno (fp, ECTF_BADCOMPONENT));
	  break;

	case CTF_K_FUNC_LINKAGE:
	  if ((func_type = ctf_type_reference (fp, type)) == CTF_ERR)
	    return (ctf_set_typed_errno (fp, ECTF_BADID));
	  /* FALLTHRU */

	case CTF_K_FUNCTION:
	  {
	    ctf_funcinfo_t fi;

	    if (ctf_func_type_info (fp, func_type, &fi) < 0)
	      return -1;	/* errno is set for us.  */

	    if ((size_t) component_idx >= fi.ctc_argc)
	      return (ctf_set_typed_errno (fp, ECTF_BADCOMPONENT));

	    break;
	  }
	default:
	  return (ctf_set_typed_errno (fp, ECTF_BADCOMPONENT));
	}
    }

  if (tag == NULL || tag[0] == '\0')
    return (ctf_set_typed_errno (fp, ECTF_NONAME));

  if ((dtd = ctf_add_generic (fp, flag, tag, kind, 0, vlen_size,
			      0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, flag, 0);
  dtd->dtd_data->ctt_type = (uint32_t) type;

  if (is_decl)
    {
      ctf_decl_tag_t *vlen = (ctf_decl_tag_t *) dtd->dtd_vlen;
      vlen->cdt_component_idx = component_idx;
    }

  return dtd->dtd_type;
}

/* Create a type tag.  */

ctf_id_t
ctf_add_type_tag (ctf_dict_t *fp, uint32_t flag, ctf_id_t type, const char *tag)
{
  return ctf_add_tag (fp, flag, type, tag, 0, -1);
}

/* Create a decl tag applied to an entire type.  */

ctf_id_t
ctf_add_decl_type_tag (ctf_dict_t *fp, uint32_t flag, ctf_id_t type, const char *tag)
{
  return ctf_add_tag (fp, flag, type, tag, 1, -1);
}

/* Create a decl tag applied to one element of a type.
   component_idx must be >= 0.  */
ctf_id_t
ctf_add_decl_tag (ctf_dict_t *fp, uint32_t flag, ctf_id_t type, const char *tag,
		  int component_idx)
{
  return ctf_add_tag (fp, flag, type, tag, 1, component_idx);
}

ctf_id_t
ctf_add_function (ctf_dict_t *fp, uint32_t flag,
		  const ctf_funcinfo_t *ctc, const ctf_id_t *argv,
		  const char **arg_names)
{
  ctf_dtdef_t *dtd;
  uint32_t vlen;
  ctf_param_t *vdat;
  ctf_dict_t *tmp = fp;
  size_t i;

  if (ctc == NULL || (ctc->ctc_flags & ~CTF_FUNC_VARARG) != 0
      || (ctc->ctc_argc != 0 && argv == NULL))
    return (ctf_set_typed_errno (fp, EINVAL));

  vlen = ctc->ctc_argc;

  /* UPTODO: CTF_K_BIG prefix for big functions?  */
  if (vlen > 0xffff)
    return (ctf_set_typed_errno (fp, EOVERFLOW));

  if (ctc->ctc_flags & CTF_FUNC_VARARG)
    vlen++;	       /* Add trailing zero to indicate varargs (see below).  */

  if (ctc->ctc_return != 0
      && ctf_lookup_by_id (&tmp, ctc->ctc_return, NULL) == NULL)
    return CTF_ERR;				/* errno is set for us.  */

  for (i = 0; i < ctc->ctc_argc; i++)
    {
      tmp = fp;
      if (argv[i] != 0 && ctf_lookup_by_id (&tmp, argv[i], NULL) == NULL)
	return CTF_ERR;				/* errno is set for us.  */
    }

  if (vlen > CTF_MAX_VLEN)
    return (ctf_set_typed_errno (fp, EOVERFLOW));

  if ((dtd = ctf_add_generic (fp, flag, NULL, CTF_K_FUNCTION, 0,
			      sizeof (ctf_param_t) * vlen, 0, NULL)) == NULL)
    return CTF_ERR;				/* errno is set for us.  */

  vdat = (ctf_param_t *) dtd->dtd_vlen;

  for (i = 0; i < ctc->ctc_argc; i++)
    {
      vdat[i].cfp_name = ctf_str_add (fp, arg_names[i]);
      vdat[i].cfp_type = (uint32_t) argv[i];
    }

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_FUNCTION, 0, vlen);
  dtd->dtd_data->ctt_type = (uint32_t) ctc->ctc_return;

  if (ctc->ctc_flags & CTF_FUNC_VARARG)
    {
      vdat[vlen - 1].cfp_type = 0;   /* Add trailing zero to indicate varargs.  */
      vdat[vlen - 1].cfp_name = 0;
    }

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_function_linkage (ctf_dict_t *fp, uint32_t flag,
			  ctf_id_t ref, const char *name, int linkage)
{
  ctf_dtdef_t *dtd;
  ctf_dict_t *tmp = fp;

  if (ref == CTF_ERR || ref > CTF_MAX_TYPE)
    return (ctf_set_typed_errno (fp, EINVAL));

  if (linkage < 0 || linkage > 2)
    return (ctf_set_typed_errno (fp, ECTF_LINKAGE));

  if (ref != 0 && ctf_lookup_by_id (&tmp, ref, NULL) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if (ctf_type_kind (fp, ref) != CTF_K_FUNCTION)
    return (ctf_set_typed_errno (fp, ECTF_NOTFUNC));

  if ((dtd = ctf_add_generic (fp, flag, name, CTF_K_FUNC_LINKAGE, 0,
			      0, 0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_FUNC_LINKAGE, 0, linkage);
  dtd->dtd_data->ctt_type = (uint32_t) ref;

  return dtd->dtd_type;
}

static ctf_id_t
ctf_add_sou_sized (ctf_dict_t *fp, uint32_t flag, const char *name,
		   size_t size, int kind)
{
  ctf_dtdef_t *dtd;
  ctf_type_t *prefix;
  ctf_id_t type = 0;
  uint32_t idx;
  size_t initial_vbytes = sizeof (ctf_member_t) * INITIAL_VLEN;
  int root_flag = flag & (~CTF_ADD_STRUCT_BITFIELDS);

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  /* Promote root-visible forwards to structs/unions.  */
  if (name != NULL && root_flag == CTF_ADD_ROOT)
    type = ctf_lookup_by_rawname (fp, kind, name);

  if (type > 0)
    idx = ctf_type_to_index (fp, type);

  /* Prohibit promotion if this type was ctf_open()ed.  */
  if (type > 0 && idx < fp->ctf_stypes)
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (type != 0 && ctf_type_kind (fp, type) == CTF_K_FORWARD)
    {
      dtd = ctf_dtd_lookup (fp, type);

      if ((prefix = ctf_add_prefix (fp, dtd, initial_vbytes)) == NULL)
	return CTF_ERR;				/* errno is set for us.  */
    }
  else if ((dtd = ctf_add_generic (fp, root_flag, name, kind, 1, 0,
				   initial_vbytes, &prefix)) == NULL)
    return CTF_ERR;				/* errno is set for us.  */

  prefix->ctt_info = CTF_TYPE_INFO (CTF_K_BIG, 0, 0);
  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, !!(flag & CTF_ADD_STRUCT_BITFIELDS), 0);
  prefix->ctt_size = CTF_SIZE_TO_LSIZE_HI (size);
  dtd->dtd_data->ctt_size = CTF_SIZE_TO_LSIZE_LO (size);

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_struct_sized (ctf_dict_t *fp, uint32_t flag, const char *name,
		      size_t size)
{
  return ctf_add_sou_sized (fp, flag, name, size, CTF_K_STRUCT);
}

ctf_id_t
ctf_add_struct (ctf_dict_t *fp, uint32_t flag, const char *name)
{
  return (ctf_add_struct_sized (fp, flag, name, 0));
}

ctf_id_t
ctf_add_union_sized (ctf_dict_t *fp, uint32_t flag, const char *name,
		     size_t size)
{
  return ctf_add_sou_sized (fp, flag, name, size, CTF_K_UNION);
}

ctf_id_t
ctf_add_union (ctf_dict_t *fp, uint32_t flag, const char *name)
{
  return (ctf_add_union_sized (fp, flag, name, 0));
}

static ctf_id_t
ctf_add_enum_internal (ctf_dict_t *fp, uint32_t flag, const char *name,
		       int kind, int is_signed)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;
  size_t initial_vbytes;
  ctf_type_t *prefix;

  if (!ctf_assert (fp, kind == CTF_K_ENUM || kind == CTF_K_ENUM64))
    return -1;			/* errno is set for us. */

  if (kind == CTF_K_ENUM)
    initial_vbytes = sizeof (ctf_enum_t) * INITIAL_VLEN;
  else
    initial_vbytes = sizeof (ctf_enum64_t) * INITIAL_VLEN;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  /* Promote root-visible forwards to enums.  */
  if (name != NULL && flag == CTF_ADD_ROOT)
    type = ctf_lookup_by_rawname (fp, kind, name);

  /* Prohibit promotion if this type was ctf_open()ed.  */
  if (type > 0 && type < fp->ctf_stypes)
    return (ctf_set_errno (fp, ECTF_RDONLY));

  if (type != 0 && ctf_type_kind (fp, type) == CTF_K_FORWARD)
    {
      dtd = ctf_dtd_lookup (fp, type);

      if ((prefix = ctf_add_prefix (fp, dtd, initial_vbytes)) == NULL)
	return CTF_ERR;		/* errno is set for us.  */
    }
  else if ((dtd = ctf_add_generic (fp, flag, name, kind, 1, 0, initial_vbytes,
				   &prefix)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  prefix->ctt_info = CTF_TYPE_INFO (CTF_K_BIG, 0, 0);
  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, is_signed, 0);

  if (kind == CTF_K_ENUM)
    dtd->dtd_data->ctt_size = fp->ctf_dmodel->ctd_int;
  else
    dtd->dtd_data->ctt_size = 8;

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_enum (ctf_dict_t *fp, uint32_t flag, const char *name)
{
  return ctf_add_enum_internal (fp, flag, name, CTF_K_ENUM, 1);
}

ctf_id_t
ctf_add_enum64 (ctf_dict_t *fp, uint32_t flag, const char *name)
{
  return ctf_add_enum_internal (fp, flag, name, CTF_K_ENUM64, 1);
}

static ctf_id_t
ctf_add_enum_encoded_internal (ctf_dict_t *fp, uint32_t flag, const char *name,
			       int kind, const ctf_encoding_t *ep)
{
  ctf_id_t type = 0;
  int is_signed = ((ep->cte_format & CTF_INT_SIGNED) != 0);

  /* First, create the enum if need be, using most of the same machinery as
     ctf_add_enum(), to ensure that we do not allow things past that are not
     enums or forwards to them.  (This includes other slices: you cannot slice a
     slice, which would be a useless thing to do anyway.)  */

  if (name != NULL && flag == CTF_ADD_ROOT)
    type = ctf_lookup_by_rawname (fp, CTF_K_ENUM, name);

  if (type != 0)
    {
      if ((ctf_type_kind (fp, type) != CTF_K_FORWARD) &&
	  (ctf_type_kind_unsliced (fp, type) != CTF_K_ENUM) &&
	  (ctf_type_kind_unsliced (fp, type) != CTF_K_ENUM64))
	return (ctf_set_typed_errno (fp, ECTF_NOTINTFP));
    }
  else if ((type = ctf_add_enum_internal (fp, flag, name, kind, is_signed))
	   == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  /* If this is just changing the signedness of the enum, we don't need a
     slice.  */

  if ((ep->cte_format & ~CTF_INT_SIGNED) == 0
      && ep->cte_bits == 0
      && ep->cte_offset == 0)
    return type;

  /* Now attach a suitable slice to it.  */

  return ctf_add_slice (fp, flag, type, ep);
}

ctf_id_t
ctf_add_enum_encoded (ctf_dict_t *fp, uint32_t flag, const char *name,
		      const ctf_encoding_t *ep)
{
  return ctf_add_enum_encoded_internal (fp, flag, name, CTF_K_ENUM, ep);
}

ctf_id_t
ctf_add_enum64_encoded (ctf_dict_t *fp, uint32_t flag, const char *name,
			const ctf_encoding_t *ep)
{
  return ctf_add_enum_encoded_internal (fp, flag, name, CTF_K_ENUM64, ep);
}

ctf_id_t
ctf_add_forward (ctf_dict_t *fp, uint32_t flag, const char *name,
		 uint32_t kind)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;

  if (!ctf_forwardable_kind (kind))
    return (ctf_set_typed_errno (fp, ECTF_NOTSUE));

  if (name == NULL || name[0] == '\0')
    return (ctf_set_typed_errno (fp, ECTF_NONAME));

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  /* If the type is already defined or exists as a forward tag, just return
     the ctf_id_t of the existing definition.  Since this changes nothing,
     it's safe to do even on the read-only portion of the dict.  */

  type = ctf_lookup_by_rawname (fp, kind, name);

  if (type)
    return type;

  if ((dtd = ctf_add_generic (fp, flag, name, kind, 0, 0, 0, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if (kind != CTF_K_ENUM &&
      kind != CTF_K_ENUM64)
    {
      dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_FORWARD, 0, 0);
      dtd->dtd_data->ctt_type = kind;
    }
  else
    dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_ENUM, 0, 0);

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_unknown (ctf_dict_t *fp, uint32_t flag, const char *name)
{
  ctf_dtdef_t *dtd;
  ctf_id_t type = 0;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  /* If a type is already defined with this name, error (if not CTF_K_UNKNOWN)
     or just return it.  */

  if (name != NULL && name[0] != '\0' && flag == CTF_ADD_ROOT
      && (type = ctf_lookup_by_rawname (fp, CTF_K_UNKNOWN, name)))
    {
      if (ctf_type_kind (fp, type) == CTF_K_UNKNOWN)
	return type;
      else
	{
	  ctf_err_warn (fp, 1, ECTF_CONFLICT,
			_("ctf_add_unknown: cannot add unknown type "
			  "named %s: type of this name already defined"),
			name ? name : _("(unnamed type)"));
	  return (ctf_set_typed_errno (fp, ECTF_CONFLICT));
	}
    }

  if ((dtd = ctf_add_generic (fp, flag, name, CTF_K_UNKNOWN, 0, 0, 0,
			      NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_UNKNOWN, flag, 0);
  dtd->dtd_data->ctt_type = 0;

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_typedef (ctf_dict_t *fp, uint32_t flag, const char *name,
		 ctf_id_t ref)
{
  ctf_dtdef_t *dtd;
  ctf_dict_t *tmp = fp;

  if (ref == CTF_ERR || ref > CTF_MAX_TYPE)
    return (ctf_set_typed_errno (fp, EINVAL));

  if (name == NULL || name[0] == '\0')
    return (ctf_set_typed_errno (fp, ECTF_NONAME));

  if (ref != 0 && ctf_lookup_by_id (&tmp, ref, NULL) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((dtd = ctf_add_generic (fp, flag, name, CTF_K_TYPEDEF, 0, 0, 0,
			      NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_TYPEDEF, 0, 0);
  dtd->dtd_data->ctt_type = (uint32_t) ref;

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_volatile (ctf_dict_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_VOLATILE));
}

ctf_id_t
ctf_add_const (ctf_dict_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_CONST));
}

ctf_id_t
ctf_add_restrict (ctf_dict_t *fp, uint32_t flag, ctf_id_t ref)
{
  return (ctf_add_reftype (fp, flag, ref, CTF_K_RESTRICT));
}

ctf_ret_t
ctf_add_enumerator (ctf_dict_t *fp, ctf_id_t enid, const char *name,
		    int64_t value)
{
  ctf_dict_t *ofp = fp;
  ctf_dtdef_t *dtd;

  uint32_t kind, vlen, root, en_name;

  if (name == NULL)
    return (ctf_set_errno (fp, EINVAL));

  if ((enid = ctf_type_resolve_unsliced (fp, enid)) == CTF_ERR)
    return -1;					/* errno is set for us.  */

  dtd = ctf_dtd_lookup (fp, enid);
  fp = ctf_get_dict (fp, enid);

  if (enid < fp->ctf_stypes)
    return (ctf_set_errno (ofp, ECTF_RDONLY));

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if (dtd == NULL)
    return (ctf_set_errno (ofp, ECTF_BADID));

  kind = LCTF_KIND (fp, dtd->dtd_buf);
  root = LCTF_INFO_ISROOT (fp, dtd->dtd_buf->ctt_info);
  vlen = LCTF_VLEN (fp, dtd->dtd_buf);

  /* Enumeration constant names are only added, and only checked for duplicates,
     if the enum they are part of is a root-visible type.  */

  if (root && ctf_dynhash_lookup (fp->ctf_names, name))
    {
      if (fp->ctf_flags & LCTF_STRICT_NO_DUP_ENUMERATORS)
	return (ctf_set_errno (ofp, ECTF_DUPLICATE));

      if (ctf_track_enumerator (fp, enid, name) < 0)
	return (ctf_set_errno (ofp, ctf_errno (fp)));
    }

  if ((kind != CTF_K_ENUM) && (kind != CTF_K_ENUM64))
    return (ctf_set_errno (ofp, ECTF_NOTENUM));

  if (vlen == CTF_MAX_VLEN)
    return (ctf_set_errno (ofp, ECTF_DTFULL));

  if (kind == CTF_K_ENUM)
    {
      if (ctf_grow_vlen (fp, dtd, sizeof (ctf_enum_t) * (vlen + 1)) < 0)
	return -1;				/* errno is set for us.  */

      dtd->dtd_vlen_size += sizeof (ctf_enum_t);
    }
  else
    {
      if (ctf_grow_vlen (fp, dtd, sizeof (ctf_enum64_t) * (vlen + 1)) < 0)
	return -1;				/* errno is set for us.  */

      dtd->dtd_vlen_size += sizeof (ctf_enum64_t);
    }

  /* Check for constant duplication within any given enum: only needed for
     non-root-visible types, since the duplicate detection above does the job
     for root-visible types just fine.  */

  if (root == CTF_ADD_NONROOT && (fp->ctf_flags & LCTF_STRICT_NO_DUP_ENUMERATORS))
    {
      size_t i;

      if (kind == CTF_K_ENUM)
	{
	  ctf_enum_t *en = (ctf_enum_t *) dtd->dtd_vlen;

	  for (i = 0; i < vlen; i++)
	    if (strcmp (ctf_strptr (fp, en[i].cte_name), name) == 0)
	      return (ctf_set_errno (ofp, ECTF_DUPLICATE));
	}
      else
	{
	  ctf_enum64_t *en = (ctf_enum64_t *) dtd->dtd_vlen;

	  for (i = 0; i < vlen; i++)
	    if (strcmp (ctf_strptr (fp, en[i].cte_name), name) == 0)
	      return (ctf_set_errno (ofp, ECTF_DUPLICATE));

	}
    }

  if (kind == CTF_K_ENUM)
    {
      ctf_enum_t *en = (ctf_enum_t *) dtd->dtd_vlen;

      en[vlen].cte_name = ctf_str_add (fp, name);
      en[vlen].cte_value = value;

      en_name = en[vlen].cte_name;
    }
  else
    {
      ctf_enum64_t *en = (ctf_enum64_t *) dtd->dtd_vlen;

      en[vlen].cte_name = ctf_str_add (fp, name);
      en[vlen].cte_val_low = ((uint64_t) value) & 0xffffffff;
      en[vlen].cte_val_high = ((uint64_t) value) >> 32;

      en_name = en[vlen].cte_name;
    }

  if (en_name == 0 && name != NULL && name[0] != '\0')
    return (ctf_set_errno (ofp, ctf_errno (fp)));

  /* Put the newly-added enumerator name into the name table if this type is
     root-visible.  */

  if (root == CTF_ADD_ROOT)
    {
      if (ctf_dynhash_insert (fp->ctf_names,
			      (char *) ctf_strptr (fp, en_name),
			      (void *) (uintptr_t) enid) < 0)
	return ctf_set_errno (fp, ENOMEM);
    }

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, root, vlen + 1);

  return 0;
}

ctf_ret_t
ctf_add_member_bitfield (ctf_dict_t *fp, ctf_id_t souid, const char *name,
			 ctf_id_t type, unsigned long bit_offset,
			 int bit_width)
{
  ctf_dict_t *ofp = fp;
  ctf_dict_t *tmp = fp;
  ctf_dtdef_t *dtd;
  ctf_type_t *prefix;

  ssize_t msize, ssize;
  uint32_t kind, kflag;
  size_t vlen;
  size_t i;
  int is_incomplete = 0;
  ctf_member_t *memb;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if (fp->ctf_flags & LCTF_NO_TYPE)
    return (ctf_set_errno (fp, ECTF_NOTSERIALIZED));

  if ((fp->ctf_flags & LCTF_CHILD) && ctf_type_isparent (fp, souid))
    {
      /* Adding a child type to a parent, even via the child, is prohibited.
	 Otherwise, climb to the parent and do all work there.  */

      if (ctf_type_ischild (fp, type))
	return (ctf_set_errno (ofp, ECTF_BADID));

      fp = fp->ctf_parent;
    }

  dtd = ctf_dtd_lookup (fp, souid);

  if (souid < fp->ctf_stypes)
    return (ctf_set_errno (ofp, ECTF_RDONLY));

  if (dtd == NULL)
    return (ctf_set_errno (ofp, ECTF_BADID));

  if ((ctf_lookup_by_id (&tmp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  if (name != NULL && name[0] == '\0')
    name = NULL;

  if ((prefix = (ctf_type_t *) ctf_find_prefix (fp, dtd->dtd_buf, CTF_K_BIG)) == NULL)
    return (ctf_set_errno (ofp, ECTF_CORRUPT));

  kind = LCTF_KIND (fp, prefix);
  kflag = CTF_INFO_KFLAG (dtd->dtd_data->ctt_info);
  vlen = LCTF_VLEN (fp, prefix);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
    return (ctf_set_errno (ofp, ECTF_NOTSOU));

  if (!kflag && bit_width != 0)
    return (ctf_set_errno (ofp, ECTF_NOTBITSOU));

  if (vlen == CTF_MAX_VLEN)
    return (ctf_set_errno (ofp, ECTF_DTFULL));

  /* Figure out the offset of this field: all structures in DTDs
     are CTF_K_BIG, which means their offsets are all encoded as
     distances from the last field's.  */

  if (bit_offset != (unsigned long) -1)
    {
      if (bit_offset < dtd->dtd_last_offset)
	return (ctf_set_errno (ofp, ECTF_DESCENDING));

      bit_offset -= dtd->dtd_last_offset;
    }

  memb = (ctf_member_t *) dtd->dtd_vlen;

  if (name != NULL)
    {
      for (i = 0; i < vlen; i++)
	if (strcmp (ctf_strptr (fp, memb[i].ctm_name), name) == 0)
	  return (ctf_set_errno (ofp, ECTF_DUPLICATE));
    }

  if ((msize = ctf_type_size (fp, type)) < 0 ||
      (ctf_type_align (fp, type)) < 0)
    {
      /* The unimplemented type, and any type that resolves to it, has no size
	 and no alignment: it can correspond to any number of compiler-inserted
	 types.  We allow incomplete types through since they are routinely
	 added to the ends of structures, and can even be added elsewhere in
	 structures by the deduplicator and by the padding inserter below.  They
	 are assumed to be zero-size with no alignment: this is often wrong, but
	 problems can be avoided in this case by explicitly specifying the size
	 of the structure via the _sized functions.  The deduplicator always
	 does this.  */

      msize = 0;
      if (ctf_errno (fp) == ECTF_NONREPRESENTABLE)
	ctf_set_errno (fp, 0);
      else if (ctf_errno (fp) == ECTF_INCOMPLETE)
	is_incomplete = 1;
      else
	return -1;		/* errno is set for us.  */
    }

  /* Figure out the right offset for naturally-aligned types, if need be,
     and insert additional unnamed members as needed.  */

  if (kind == CTF_K_UNION || vlen == 0)
    {
      bit_offset = 0;
      ssize = ctf_get_ctt_size (fp, prefix, NULL, NULL);
      ssize = MAX (ssize, msize);
    }
  else 				/* Subsequent struct member. */
    {
      size_t bound;
      ssize_t off;
      int added_padding = 0;

      if (bit_offset == (unsigned long) - 1)
	{
	  /* Natural alignment.  */

	  if (is_incomplete)
	    {
	      ctf_err_warn (ofp, 1, ECTF_INCOMPLETE,
			    _("ctf_add_member: cannot add member %s of "
			      "incomplete type %lx to struct %lx without "
			      "specifying explicit offset\n"),
			    name ? name : _("(unnamed member)"), type, souid);
	      return (ctf_set_errno (ofp, ECTF_INCOMPLETE));
	    }

	  if ((off = ctf_type_align_natural (fp, memb[vlen - 1].ctm_type,
					     type, dtd->dtd_last_offset)) < 0)
	    {
	      if (ctf_errno (fp) == ECTF_INCOMPLETE)
		{
		  const char *lname = ctf_strraw (fp, memb[vlen - 1].ctm_name);

		  ctf_err_warn (ofp, 1, ECTF_INCOMPLETE,
				_("ctf_add_member_offset: cannot add member %s "
				  "of type %lx to struct %lx without "
				  "specifying explicit offset after member %s"
				  "of type %x, which is an incomplete type\n"),
				name ? name : _("(unnamed member)"), type, souid,
				lname ? lname : _("(unnamed member)"),
				memb[vlen -1].ctm_type);
		}
	      return (ctf_set_errno (ofp, ctf_errno (fp)));
	    }

	  /* Convert the offset to a gap-since-the-last.  */
	  off -= dtd->dtd_last_offset;
	  bit_offset = off;
	}

      /* Insert as many nameless members as needed.  */

      if (kflag)
	bound = CTF_MAX_BIT_OFFSET;
      else
	bound = CTF_MAX_SIZE;

      while (bit_offset > bound)
	{
	  added_padding = 1;

	  off = bound;
	  if (kflag)
	    off = CTF_MEMBER_BIT_OFFSET (bound);

	  if (ctf_add_member_bitfield (fp, souid, "", 0, off, 0) < 0)
	    return -1;		/* errno is set for us.  */

	  bit_offset =- off;
	}

      off = bit_offset;
      if (kflag)
	off = CTF_MEMBER_BIT_OFFSET (off);

      /* Possibly hunt down the prefix and member list again: they may have been
	 moved by the realloc()s involved in field additions.  */

      if (added_padding
	  && (prefix = (ctf_type_t *) ctf_find_prefix (fp, dtd->dtd_buf, CTF_K_BIG)) == NULL)
	return (ctf_set_errno (ofp, ECTF_CORRUPT));

      vlen = LCTF_VLEN (fp, prefix);
      memb = (ctf_member_t *) dtd->dtd_vlen;
      bit_offset = off;

      ssize = ctf_get_ctt_size (fp, prefix, NULL, NULL);
      ssize = MAX (ssize, ((signed) ((bit_offset + dtd->dtd_last_offset)) / CHAR_BIT) + msize);
    }

  if (kflag)
    memb[vlen].ctm_offset = CTF_MEMBER_MAKE_BIT_OFFSET (bit_width, bit_offset);
  else
    memb[vlen].ctm_offset = bit_offset;

  vlen = LCTF_VLEN (fp, prefix);

  if (ctf_grow_vlen (fp, dtd, sizeof (ctf_member_t) * (vlen + 1)) < 0)
    return (ctf_set_errno (ofp, ctf_errno (fp)));

  dtd->dtd_vlen_size += sizeof (ctf_member_t);

  /* Hunt down the prefix and member list yet again, since they may have been
     reallocated by ctf_grow_vlen.  */

  if ((prefix = (ctf_type_t *) ctf_find_prefix (fp, dtd->dtd_buf, CTF_K_BIG)) == NULL)
    return (ctf_set_errno (ofp, ECTF_CORRUPT));
  memb = (ctf_member_t *) dtd->dtd_vlen;

  memb[vlen].ctm_name = ctf_str_add (fp, name);
  memb[vlen].ctm_type = type;
  if (memb[vlen].ctm_name == 0 && name != NULL && name[0] != '\0')
    return -1;			/* errno is set for us.  */

  dtd->dtd_data->ctt_size = CTF_SIZE_TO_LSIZE_LO (ssize);
  prefix->ctt_size = CTF_SIZE_TO_LSIZE_HI (ssize);

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, kflag, CTF_VLEN_TO_VLEN_LO(vlen + 1));
  prefix->ctt_info = CTF_TYPE_INFO (CTF_K_BIG, 0, CTF_VLEN_TO_VLEN_HI(vlen + 1));

  dtd->dtd_last_offset += bit_offset;

  fp->ctf_serialize.cs_initialized = 0;

  return 0;
}

ctf_ret_t
ctf_add_member_encoded (ctf_dict_t *fp, ctf_id_t souid, const char *name,
			ctf_id_t type, unsigned long bit_offset,
			const ctf_encoding_t encoding)
{
  ctf_dtdef_t *dtd = ctf_dtd_lookup (fp, type);
  int kind;
  int otype = type;

  if (dtd == NULL)
    return (ctf_set_errno (fp, ECTF_BADID));

  kind = LCTF_KIND (fp, dtd->dtd_buf);

  if ((kind != CTF_K_INTEGER) && (kind != CTF_K_FLOAT) && (kind != CTF_K_ENUM))
    return (ctf_set_errno (fp, ECTF_NOTINTFP));

  /* Create a slice if need be.  */

  if (encoding.cte_offset != 0 ||
      encoding.cte_format != 0 ||
      (encoding.cte_bits != 0 && CTF_INFO_KFLAG (dtd->dtd_data->ctt_info) == 0))
    {
      if ((type = ctf_add_slice (fp, CTF_ADD_NONROOT, otype, &encoding)) == CTF_ERR)
	return -1;			/* errno is set for us.  */
    }
  else
    type = otype;

  return ctf_add_member_bitfield (fp, souid, name, type, bit_offset, 0);
}

ctf_ret_t
ctf_add_member_offset (ctf_dict_t *fp, ctf_id_t souid, const char *name,
		       ctf_id_t type, unsigned long bit_offset)
{
  return ctf_add_member_bitfield (fp, souid, name, type, bit_offset, 0);
}

ctf_ret_t
ctf_add_member (ctf_dict_t *fp, ctf_id_t souid, const char *name,
		ctf_id_t type)
{
  return ctf_add_member_offset (fp, souid, name, type, (unsigned long) - 1);
}

/* Add a DATASEC to hang variables off of.  */

static ctf_id_t
ctf_add_datasec (ctf_dict_t *fp, uint32_t flag, const char *datasec)
{
  ctf_dtdef_t *dtd;
  size_t initial_vlen = sizeof (ctf_var_secinfo_t) * INITIAL_VLEN;

  if ((dtd = ctf_add_generic (fp, flag, datasec, CTF_K_DATASEC,
			      0, 0, initial_vlen, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us. */

  dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_DATASEC, 0, 0);
  dtd->dtd_data->ctt_size = 0;

  return dtd->dtd_type;
}

ctf_id_t
ctf_add_variable (ctf_dict_t *fp, const char *name, int linkage, ctf_id_t ref)
{
  return ctf_add_section_variable (fp, CTF_ADD_ROOT, NULL, name, linkage, ref,
				   0, (unsigned long) -1);
}

/* Add variable, interning it in the specified DATASEC (which must be in the
   same dict, but which may be NULL, meaning "no datasec").  As with structs, an
   offset of -1 means "next natural alignment".  A size of zero means "get it
   from the type" and is the common case.  */
ctf_id_t
ctf_add_section_variable (ctf_dict_t *fp, uint32_t flag, const char *datasec,
			  const char *name, int linkage, ctf_id_t type,
			  size_t size, size_t offset)
{
  ctf_dtdef_t *sec_dtd = NULL;
  ctf_dtdef_t *var_dtd = NULL;

  uint32_t kind, kflag;
  size_t vlen;

  ctf_linkage_t *l;
  ctf_var_secinfo_t *sec;

  ctf_dict_t *tmp = fp;
  ctf_id_t datasec_id = 0;
  int is_incomplete = 0;
  ctf_snapshot_id_t err_snap = ctf_snapshot (fp);

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_typed_errno (fp, ECTF_NOPARENT));

  if (name == NULL || name[0] == '\0')
    return (ctf_set_typed_errno (fp, ECTF_NONAME));

  if (linkage < 0 || linkage > 2)
    return (ctf_set_typed_errno (fp, ECTF_LINKAGE));

  if (flag == CTF_ADD_ROOT && ctf_lookup_by_rawname (fp, CTF_K_VAR, name) != 0)
    return (ctf_set_typed_errno (fp, ECTF_DUPLICATE));

  /* First, create the variable.  Make sure its type exists.  */

  if (ctf_lookup_by_id (&tmp, type, NULL) == NULL)
    return CTF_ERR;			/* errno is set for us.  */

  /* Make sure this type is representable: if a variable is nonrepresentable
     there's nothing the end-user can do with it even if they know it's there.
     Allow type 0: this is used for const void variables in BTF input.  */

  if ((ctf_type_resolve_nonrepresentable (fp, type, 1) == CTF_ERR)
      && (ctf_errno (fp) == ECTF_NONREPRESENTABLE))
    return CTF_ERR;

  if ((var_dtd = ctf_add_generic (fp, flag, name, CTF_K_VAR, 0,
				  sizeof (ctf_linkage_t), 0, NULL)) == NULL)
    return CTF_ERR;			/* errno is set for us.  */

  l = (ctf_linkage_t *) var_dtd->dtd_vlen;
  var_dtd->dtd_data->ctt_info = CTF_TYPE_INFO (CTF_K_VAR, 0, 0);
  var_dtd->dtd_data->ctt_type = type;
  l->ctl_linkage = linkage;

  /* Add it to the datasec, if requested, creating the datasec if need be.  */

  if (!datasec)
    return var_dtd->dtd_type;

  if ((datasec_id = ctf_lookup_by_rawname (fp, CTF_K_DATASEC,
					   datasec)) == 0)
    {
      if ((datasec_id = ctf_add_datasec (fp, CTF_ADD_ROOT,
					 datasec)) == CTF_ERR)
	goto err;				/* errno is set for us.  */
    }

  sec_dtd = ctf_dtd_lookup (fp, datasec_id);

  kind = LCTF_KIND (fp, sec_dtd->dtd_buf);
  kflag = CTF_INFO_KFLAG (sec_dtd->dtd_data->ctt_info);
  vlen = LCTF_VLEN (fp, sec_dtd->dtd_buf);

  if (vlen == CTF_MAX_RAW_VLEN)
    {
      ctf_set_typed_errno (fp, ECTF_DTFULL);
      goto err;
    }

  /* DATASECs do not support CTF_K_BIG (yet).  */
  if (vlen == CTF_MAX_RAW_VLEN)
    {
      ctf_set_typed_errno (fp, ECTF_DTFULL);
      goto err;
    }

  /* Allow for variables of void-like types.  */
  if (type == 0)
    is_incomplete = 1;
  else if (ctf_type_align (fp, type) < 0)
    {
      /* See the comment in ctf_add_member_bitfield.  We don't need to worry
	 about norepresentable types, because we just added this one: we know
	 it's representable.  We do not know it's complete.  */

      if (ctf_errno (fp) == ECTF_INCOMPLETE)
	is_incomplete = 1;
      else
	goto err;			/* errno is set for us.  */
    }

  /* Here we just add the var info to the end of the datasec, naturally aligning
     the offset as with struct/union membership addition if no offset is
     specified.  */

  sec = (ctf_var_secinfo_t *) sec_dtd->dtd_vlen;

  if (vlen != 0)
    {
      /* To avoid causing trouble with existing code promiscuously adding
	 variables without caring about their types, if natural alignment fails
	 due to incomplete types, just set the next offset to something aligned
	 mod 8.  It might be a waste of space but it'll avoid an error and
	 should suffice for a long time to come.  */

      if (offset == (unsigned long) -1 && is_incomplete)
	offset = roundup (offset, 8);
      else if (offset == (unsigned long) -1)
	{
	  /* Natural alignment.  */

	  ssize_t bit_offset = offset * 8;

	  if ((bit_offset = ctf_type_align_natural (fp, sec[vlen - 1].cvs_type,
						    type, sec[vlen -1].cvs_offset)) < 0)
	    offset = roundup (offset, 8);
	  else
	    offset = bit_offset / CHAR_BIT;
	}

      /* This DTD may need sorting.  */

      if (offset < sec[vlen - 1].cvs_offset)
	sec_dtd->dtd_flags |= ~DTD_F_UNSORTED;

    } else if (offset == (unsigned long) -1)
	offset = 0;

  /* Remember the variable -> datasec mapping.  */

  if (ctf_dynhash_insert (fp->ctf_var_datasecs,
			  (void *) (ptrdiff_t) var_dtd->dtd_type,
			  (void *) (ptrdiff_t) datasec_id) != 0)
    {
      ctf_set_typed_errno (fp, ENOMEM);
      goto err;
    }

  if (ctf_grow_vlen (fp, sec_dtd, sizeof (ctf_var_secinfo_t) * (vlen + 1)) < 0)
    goto err;

  sec_dtd->dtd_vlen_size += sizeof (ctf_var_secinfo_t);
  sec = (ctf_var_secinfo_t *) sec_dtd->dtd_vlen;

  sec[vlen].cvs_type = (uint32_t) var_dtd->dtd_type;
  sec[vlen].cvs_offset = (uint32_t) offset;
  sec[vlen].cvs_size = (uint32_t) size;
  sec_dtd->dtd_data->ctt_info = CTF_TYPE_INFO (kind, kflag, vlen + 1);

  return var_dtd->dtd_type;

err:
  ctf_dynhash_remove (fp->ctf_var_datasecs,
		      (void *) (ptrdiff_t) var_dtd->dtd_type);
  ctf_rollback (fp, err_snap);
  return CTF_ERR;
}

/* Add a function or object symbol regardless of whether or not it is already
   present (already existing symbols are silently overwritten).

   Internal use only.  */
ctf_ret_t
ctf_add_funcobjt_sym_forced (ctf_dict_t *fp, int is_function, const char *name, ctf_id_t id)
{
  ctf_dict_t *tmp = fp;
  char *dupname;
  ctf_dynhash_t *h = is_function ? fp->ctf_funchash : fp->ctf_objthash;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if (fp->ctf_flags & LCTF_NO_TYPE)
    return (ctf_set_errno (fp, ECTF_NOTSERIALIZED));

  if (ctf_lookup_by_id (&tmp, id, NULL) == NULL)
    return -1;				/* errno is set for us.  */

  if (is_function && ctf_type_kind (fp, id) != CTF_K_FUNCTION)
    return (ctf_set_errno (fp, ECTF_NOTFUNC));

  if ((dupname = strdup (name)) == NULL)
    return (ctf_set_errno (fp, ENOMEM));

  if (ctf_dynhash_insert (h, dupname, (void *) (uintptr_t) id) < 0)
    {
      free (dupname);
      return (ctf_set_errno (fp, ENOMEM));
    }
  return 0;
}

ctf_ret_t
ctf_add_funcobjt_sym (ctf_dict_t *fp, int is_function, const char *name, ctf_id_t id)
{
  if (ctf_lookup_by_sym_or_name (fp, 0, name, 0, is_function) != CTF_ERR)
    return (ctf_set_errno (fp, ECTF_DUPLICATE));

  fp->ctf_serialize.cs_initialized = 0;

  return ctf_add_funcobjt_sym_forced (fp, is_function, name, id);
}

ctf_ret_t
ctf_add_objt_sym (ctf_dict_t *fp, const char *name, ctf_id_t id)
{
  return (ctf_add_funcobjt_sym (fp, 0, name, id));
}

ctf_ret_t
ctf_add_func_sym (ctf_dict_t *fp, const char *name, ctf_id_t id)
{
  return (ctf_add_funcobjt_sym (fp, 1, name, id));
}

/* Sort function used by ctf_datasec_sort.  */

static int
ctf_datasec_sort_ascending (const void *one_, const void *two_)
{
  ctf_var_secinfo_t *one = (ctf_var_secinfo_t *) one_;
  ctf_var_secinfo_t *two = (ctf_var_secinfo_t *) two_;

  if (one->cvs_offset < two->cvs_offset)
    return -1;
  else if (one->cvs_offset > two->cvs_offset)
    return 1;
  return 0;
}

/* Sort a datasec into order.  Needed before serialization or query
   operations.  */

void
ctf_datasec_sort (ctf_dict_t *fp, ctf_dtdef_t *dtd)
{
  size_t vlen;

  if (!(dtd->dtd_flags & DTD_F_UNSORTED))
    return;

  vlen = LCTF_VLEN (fp, dtd->dtd_buf);

  qsort (dtd->dtd_vlen, vlen, sizeof (ctf_var_secinfo_t),
	 ctf_datasec_sort_ascending);
  dtd->dtd_flags &= ~DTD_F_UNSORTED;
}

/* Add an enumeration constant observed in a given enum type as an identifier.
   They appear as names that cite the enum type.

   Constants that appear in more than one enum, or which are already the names
   of types, appear in ctf_conflicting_enums as well.

   This is done for all enumeration types at open time, and for newly-added ones
   as well: if the strict-enum flag is turned on, this table must be kept up to
   date with enums added in the interim.  */

ctf_ret_t
ctf_track_enumerator (ctf_dict_t *fp, ctf_id_t type, const char *cte_name)
{
  ctf_error_t err;

  if (ctf_dynhash_lookup_type (fp->ctf_names, cte_name) == 0)
    {
      uint32_t name = ctf_str_add (fp, cte_name);

      if (name == 0)
	return -1;				/* errno is set for us.  */

      err = ctf_dynhash_insert_type (fp, fp->ctf_names, type, name);
    }
  else
    {
      err = ctf_dynset_insert (fp->ctf_conflicting_enums, (void *)
			       cte_name);
      if (err != 0)
	ctf_set_errno (fp, err * -1);
    }
  if (err != 0)
    return -1;					/* errno is set for us.  */
  return 0;
}

typedef struct ctf_bundle
{
  ctf_dict_t *ctb_dict;		/* CTF dict handle.  */
  ctf_id_t ctb_type;		/* CTF type identifier.  */
  ctf_dtdef_t *ctb_dtd;		/* CTF dynamic type definition (if any).  */
} ctf_bundle_t;

static int
enumcmp (ctf_dict_t *fp, ctf_id_t type, ctf_bundle_t *ctb, ctf_dict_t *err_fp)
{
  ctf_next_t *i = NULL;
  const char *name;
  int64_t value;
  int64_t bvalue;

  while ((name = ctf_enum_next (fp, type, &i, &value)) != NULL)
    {
      if (ctf_enum_value (ctb->ctb_dict, ctb->ctb_type, name, &bvalue) < 0)
	{
	  ctf_next_destroy (i);
	  ctf_err_warn (err_fp, 0, ctf_errno (ctb->ctb_dict),
			_("conflict due to enum %s iteration error"), name);
	  return 1;
	}
      if (value != bvalue)
	{
	  ctf_next_destroy (i);
	  ctf_err_warn (err_fp, 1, ECTF_CONFLICT,
			_("conflict due to enum value change: %li versus %li"),
			value, bvalue);
	  return 1;
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    {
      ctf_err_warn (err_fp, 0, ctf_errno (fp),
		    _("conflict due to enum %s iteration error"), name);
      return 1;
    }

  return 0;
}

static ctf_ret_t
membcmp (const char *name, size_t offset, int bit_width, ctf_bundle_t *ctb)
{
  ctf_membinfo_t ctm;

  /* Don't check nameless members (e.g. anonymous structs/unions) against each
     other.  */
  if (name[0] == 0)
    return 0;

  if (ctf_member_info (ctb->ctb_dict, ctb->ctb_type, name, &ctm) < 0)
    {
      ctf_err_warn (ctb->ctb_dict, 0, 0,
		    _("conflict due to struct member %s iteration error"),
		    name);
      return -1;
    }
  if (ctm.ctm_offset != offset)
    {
      ctf_err_warn (ctb->ctb_dict, 1, ECTF_CONFLICT,
		    _("conflict due to struct member %s offset change: "
		      "%zx versus %zx"),
		    name, ctm.ctm_offset, offset);
      return -1;
    }
  if (ctm.ctm_bit_width != bit_width)
    {
      ctf_err_warn (ctb->ctb_dict, 1, ECTF_CONFLICT,
		    _("conflict due to struct member %s bit-width change: "
		      "%i versus %i"),
		    name, ctm.ctm_bit_width, bit_width);
      return -1;
    }
  return 0;
}

/* Record the correspondence between a source and ctf_add_type()-added
   destination type: both types are translated into parent type IDs if need be,
   so they relate to the actual dictionary they are in.  Outside controlled
   circumstances (like linking) it is probably not useful to do more than
   compare these pointers, since there is nothing stopping the user closing the
   source dict whenever they want to.

   Our OOM handling here is just to not do anything, because this is called deep
   enough in the call stack that doing anything useful is painfully difficult:
   the worst consequence if we do OOM is a bit of type duplication anyway.
   The non-imported checks are just paranoia and should never be able to
   happen, but if they do we don't want a coredump.  */

static void
ctf_add_type_mapping (ctf_dict_t *src_fp, ctf_id_t src_type,
		      ctf_dict_t *dst_fp, ctf_id_t dst_type)
{
  src_fp = ctf_get_dict (src_fp, src_type);
  dst_fp = ctf_get_dict (dst_fp, dst_type);

  /* Not imported? No mapping is meaningful.  */
  if (src_fp == NULL || dst_fp == NULL)
    return;

  src_type = ctf_type_to_index (src_fp, src_type);
  dst_type = ctf_type_to_index (dst_fp, dst_type);

  if (dst_fp->ctf_link_type_mapping == NULL)
    {
      ctf_hash_fun f = ctf_hash_type_key;
      ctf_hash_eq_fun e = ctf_hash_eq_type_key;

      if ((dst_fp->ctf_link_type_mapping = ctf_dynhash_create (f, e, free,
							       NULL)) == NULL)
	return;
    }

  ctf_link_type_key_t *key;
  key = calloc (1, sizeof (struct ctf_link_type_key));
  if (!key)
    return;

  key->cltk_fp = src_fp;
  key->cltk_idx = src_type;

  /* No OOM checking needed, because if this doesn't work the worst we'll do is
     add a few more duplicate types (which will probably run out of memory
     anyway).  */
  ctf_dynhash_insert (dst_fp->ctf_link_type_mapping, key,
		      (void *) (uintptr_t) dst_type);
}

/* Look up a type mapping: return 0 if none.  The DST_FP is modified to point to
   the parent if need be.  The ID returned is from the dst_fp's perspective.  */
static ctf_id_t
ctf_type_mapping (ctf_dict_t *src_fp, ctf_id_t src_type, ctf_dict_t **dst_fp)
{
  ctf_link_type_key_t key;
  ctf_dict_t *target_fp = *dst_fp;
  ctf_id_t dst_type = 0;

  src_fp = ctf_get_dict (src_fp, src_type);

  /* No mapping is possible if the parent is not imported.  */
  if (src_fp == NULL)
    return 0;

  src_type = ctf_type_to_index (src_fp, src_type);
  key.cltk_fp = src_fp;
  key.cltk_idx = src_type;

  if (target_fp->ctf_link_type_mapping)
    dst_type = (uintptr_t) ctf_dynhash_lookup (target_fp->ctf_link_type_mapping,
					       &key);

  if (dst_type != 0)
    {
      dst_type = ctf_index_to_type (target_fp, dst_type);
      *dst_fp = target_fp;
      return dst_type;
    }

  if (target_fp->ctf_parent)
    target_fp = target_fp->ctf_parent;
  else
    return 0;

  if (target_fp->ctf_link_type_mapping)
    dst_type = (uintptr_t) ctf_dynhash_lookup (target_fp->ctf_link_type_mapping,
					       &key);

  if (dst_type)
    dst_type = ctf_index_to_type (target_fp, dst_type);

  *dst_fp = target_fp;
  return dst_type;
}

/* The ctf_add_type routine is used to copy a type from a source CTF dictionary
   to a dynamic destination dictionary.  This routine operates recursively by
   following the source type's links and embedded member types.  If the
   destination dict already contains a named type which has the same attributes,
   then we succeed and return this type but no changes occur.  */
static ctf_id_t
ctf_add_type_internal (ctf_dict_t *dst_fp, ctf_dict_t *src_fp, ctf_id_t src_type,
		       ctf_dict_t *proc_tracking_fp)
{
  ctf_id_t dst_type = CTF_ERR;
  uint32_t dst_kind = CTF_K_UNKNOWN;
  ctf_dict_t *tmp_fp = dst_fp;
  ctf_id_t tmp;

  const char *name;
  uint32_t kind, forward_kind, flag, bitfields;
  uint32_t isroot;
  size_t vlen;

  const ctf_type_t *src_prefix, *src_tp, *dst_prefix;
  ctf_bundle_t src, dst;
  ctf_encoding_t src_en, dst_en;
  ctf_arinfo_t src_ar, dst_ar;

  ctf_id_t orig_src_type = src_type;

  if ((src_prefix = ctf_lookup_by_id (&src_fp, src_type, &src_tp)) == NULL)
    return (ctf_set_typed_errno (dst_fp, ctf_errno (src_fp)));

  if ((ctf_type_resolve_nonrepresentable (src_fp, src_type, 1) == CTF_ERR)
      && (ctf_errno (src_fp) == ECTF_NONREPRESENTABLE))
    return (ctf_set_typed_errno (dst_fp, ECTF_NONREPRESENTABLE));

  name = ctf_strptr (src_fp, src_tp->ctt_name);
  kind = ctf_type_kind (src_fp, src_type);
  isroot = LCTF_INFO_ISROOT (src_fp, src_prefix->ctt_info);
  flag = isroot ? CTF_ADD_ROOT : CTF_ADD_NONROOT;
  bitfields = CTF_INFO_KFLAG (src_tp->ctt_info);
  vlen = LCTF_VLEN (src_fp, src_prefix);

  /* If this is a type we are currently in the middle of adding, hand it
     straight back.  (This lets us handle self-referential structures without
     considering forwards and empty structures the same as their completed
     forms.)  */

  tmp = ctf_type_mapping (src_fp, src_type, &tmp_fp);

  if (tmp != 0)
    {
      if (ctf_dynhash_lookup (proc_tracking_fp->ctf_add_processing,
			      (void *) (uintptr_t) src_type))
	return tmp;

      /* If this type has already been added from this dictionary, and is the
	 same kind and (if a struct or union) has the same number of members,
	 hand it straight back.  */

      if (ctf_type_kind_unsliced (tmp_fp, tmp) == (int) kind)
	{
	  if (kind == CTF_K_STRUCT || kind == CTF_K_UNION
	      || kind == CTF_K_ENUM)
	    {
	      if ((dst_prefix = ctf_lookup_by_id (&tmp_fp, dst_type, NULL)) != NULL)
		if (vlen == LCTF_VLEN (tmp_fp, dst_prefix))
		  return tmp;
	    }
	  else
	    return tmp;
	}
    }

  forward_kind = kind;
  if (kind == CTF_K_FORWARD)
    forward_kind = ctf_type_kind_forwarded (src_fp, src_type);

  /* If the source type has a name and is a root type (visible at the top-level
     scope), look up the name in the destination dictionary and verify that it is
     of the same kind before we do anything else.  */

  if (isroot && name[0] != '\0'
      && (tmp = ctf_lookup_by_rawname (dst_fp, forward_kind, name)) != 0)
    {
      dst_type = tmp;
      dst_kind = ctf_type_kind_unsliced (dst_fp, dst_type);
    }

  /* If an identically named dst_type exists, fail with ECTF_CONFLICT
     unless dst_type is a forward declaration and src_type is a struct,
     union, or enum (i.e. the definition of the previous forward decl).

     We also allow addition in the opposite order (addition of a forward when a
     struct, union, or enum already exists), which is a NOP and returns the
     already-present struct, union, or enum.  */

  if (dst_type != CTF_ERR && dst_kind != kind)
    {
      if (kind == CTF_K_FORWARD
	  && (dst_kind == CTF_K_ENUM || dst_kind == CTF_K_STRUCT
	      || dst_kind == CTF_K_UNION))
	{
	  ctf_add_type_mapping (src_fp, src_type, dst_fp, dst_type);
	  return dst_type;
	}

      if (dst_kind != CTF_K_FORWARD
	  || (kind != CTF_K_ENUM && kind != CTF_K_STRUCT
	      && kind != CTF_K_UNION))
	{
	  ctf_err_warn (dst_fp, 1, ECTF_CONFLICT,
			_("ctf_add_type: conflict for type %s: "
			  "kinds differ, new: %i; old (ID %lx): %i"),
			name, kind, dst_type, dst_kind);
	  return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
	}
    }

  /* We take special action for an integer, float, or slice since it is
     described not only by its name but also its encoding.  For integers,
     bit-fields exploit this degeneracy.  */

  if (kind == CTF_K_INTEGER || kind == CTF_K_FLOAT || kind == CTF_K_SLICE)
    {
      if (ctf_type_encoding (src_fp, src_type, &src_en) != 0)
	return (ctf_set_typed_errno (dst_fp, ctf_errno (src_fp)));

      if (dst_type != CTF_ERR)
	{
	  ctf_dict_t *fp = dst_fp;

	  if ((dst_prefix = ctf_lookup_by_id (&fp, dst_type, NULL)) == NULL)
	    return CTF_ERR;

	  if (ctf_type_encoding (dst_fp, dst_type, &dst_en) != 0)
	    return CTF_ERR;			/* errno set for us.  */

	  if (LCTF_INFO_ISROOT (fp, dst_prefix->ctt_info))
	    {
	      /* The type that we found in the hash is also root-visible.  If
		 the two types match then use the existing one; otherwise,
		 declare a conflict.  Note: slices are not certain to match
		 even if there is no conflict: we must check the contained type
		 too.  */

	      if (memcmp (&src_en, &dst_en, sizeof (ctf_encoding_t)) == 0)
		{
		  if (kind != CTF_K_SLICE)
		    {
		      ctf_add_type_mapping (src_fp, src_type, dst_fp, dst_type);
		      return dst_type;
		    }
		}
	      else
		  {
		    return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
		  }
	    }
	  else
	    {
	      /* We found a non-root-visible type in the hash.  If its encoding
		 is the same, we can reuse it, unless it is a slice.  */

	      if (memcmp (&src_en, &dst_en, sizeof (ctf_encoding_t)) == 0)
		{
		  if (kind != CTF_K_SLICE)
		    {
		      ctf_add_type_mapping (src_fp, src_type, dst_fp, dst_type);
		      return dst_type;
		    }
		}
	    }
	}
    }

  src.ctb_dict = src_fp;
  src.ctb_type = src_type;
  src.ctb_dtd = NULL;

  dst.ctb_dict = dst_fp;
  dst.ctb_type = dst_type;
  dst.ctb_dtd = NULL;

  /* Now perform kind-specific processing.  If dst_type is CTF_ERR, then we add
     a new type with the same properties as src_type to dst_fp.  If dst_type is
     not CTF_ERR, then we verify that dst_type has the same attributes as
     src_type.  We recurse for embedded references.  Before we start, we note
     that we are processing this type, to prevent infinite recursion: we do not
     re-process any type that appears in this list.  The list is emptied
     wholesale at the end of processing everything in this recursive stack.  */

  if (ctf_dynhash_insert (proc_tracking_fp->ctf_add_processing,
			  (void *) (uintptr_t) src_type, (void *) 1) < 0)
    return ctf_set_typed_errno (dst_fp, ENOMEM);

  switch (kind)
    {
    case CTF_K_INTEGER:
      /*  If we found a match we will have either returned it or declared a
	  conflict.  */
      dst_type = ctf_add_integer (dst_fp, flag, name, &src_en);
      break;

    case CTF_K_FLOAT:
      /* If we found a match we will have either returned it or declared a
       conflict.  */
      dst_type = ctf_add_float (dst_fp, flag, name, &src_en);
      break;

    case CTF_K_SLICE:
      /* We have checked for conflicting encodings: now try to add the
	 contained type.  */
      src_type = ctf_type_reference (src_fp, src_type);
      src_type = ctf_add_type_internal (dst_fp, src_fp, src_type,
					proc_tracking_fp);

      if (src_type == CTF_ERR)
	return CTF_ERR;				/* errno is set for us.  */

      dst_type = ctf_add_slice (dst_fp, flag, src_type, &src_en);
      break;

    case CTF_K_POINTER:
    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
      src_type = ctf_type_reference (src_fp, src_type);
      src_type = ctf_add_type_internal (dst_fp, src_fp, src_type,
					proc_tracking_fp);

      if (src_type == CTF_ERR)
	return CTF_ERR;				/* errno is set for us.  */

      dst_type = ctf_add_reftype (dst_fp, flag, src_type, kind);
      break;

    case CTF_K_ARRAY:
      if (ctf_array_info (src_fp, src_type, &src_ar) != 0)
	return (ctf_set_typed_errno (dst_fp, ctf_errno (src_fp)));

      src_ar.ctr_contents =
	ctf_add_type_internal (dst_fp, src_fp, src_ar.ctr_contents,
			       proc_tracking_fp);
      src_ar.ctr_index = ctf_add_type_internal (dst_fp, src_fp,
						src_ar.ctr_index,
						proc_tracking_fp);
      src_ar.ctr_nelems = src_ar.ctr_nelems;

      if (src_ar.ctr_contents == CTF_ERR || src_ar.ctr_index == CTF_ERR)
	return CTF_ERR;				/* errno is set for us.  */

      if (dst_type != CTF_ERR)
	{
	  if (ctf_array_info (dst_fp, dst_type, &dst_ar) != 0)
	    return CTF_ERR;			/* errno is set for us.  */

	  if (memcmp (&src_ar, &dst_ar, sizeof (ctf_arinfo_t)))
	    {
	      ctf_err_warn (dst_fp, 1, ECTF_CONFLICT,
			    _("conflict for type %s against ID %lx: array info "
			      "differs, old %lx/%lx/%zx; new: %lx/%lx/%zx"),
			    name, dst_type, src_ar.ctr_contents,
			    src_ar.ctr_index, src_ar.ctr_nelems,
			    dst_ar.ctr_contents, dst_ar.ctr_index,
			    dst_ar.ctr_nelems);
	      return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
	    }
	}
      else
	dst_type = ctf_add_array (dst_fp, flag, &src_ar);
      break;

    /* UPTODO: FUNC_LINKAGE, DATASEC, VAR, *TAG */
    case CTF_K_FUNCTION:
      {
	ctf_funcinfo_t fi;
	ctf_id_t *argv;
	const char **arg_names;
	size_t i;

	if (ctf_func_type_info (src_fp, src_type, &fi) < 0)
	  return CTF_ERR;			/* errno is set for us. */

	fi.ctc_return = ctf_add_type_internal (dst_fp, src_fp,
					       src_tp->ctt_type,
					       proc_tracking_fp);

	if (fi.ctc_return == CTF_ERR)
	  return CTF_ERR;			/* errno is set for us.  */

	if ((argv = calloc (fi.ctc_argc, sizeof (ctf_id_t *))) == NULL)
	  {
	    ctf_set_errno (src_fp, errno);
	    return CTF_ERR;
	  }

	if (ctf_func_type_args (src_fp, src_type, fi.ctc_argc, argv) < 0)
	  {
	    free (argv);
	    return CTF_ERR;			/* errno is set for us. */
	  }

	for (i = 0; i < fi.ctc_argc; i++)
	  {
	    argv[i] = ctf_add_type_internal (dst_fp, src_fp,
					     argv[i],
					     proc_tracking_fp);
	    if (argv[i] == CTF_ERR)
	      {
		free (argv);
		return CTF_ERR;			/* errno is set for us.  */
	      }
	  }

	if ((arg_names = calloc (fi.ctc_argc, sizeof (const char **))) == NULL)
	  {
	    free (argv);
	    free (arg_names);
	    return CTF_ERR;			/* errno is set for us. */
	  }

	if (ctf_func_type_arg_names (src_fp, src_type, fi.ctc_argc,
				     arg_names) < 0)
	  {
	    free (argv);
	    free (arg_names);
	    return CTF_ERR;			/* errno is set for us. */
	  }

	dst_type = ctf_add_function (dst_fp, flag, &fi, argv, arg_names);
	free (argv);
	free (arg_names);
	break;
      }

    case CTF_K_STRUCT:
    case CTF_K_UNION:
      {
	ctf_next_t *i = NULL;
	ssize_t offset;
	const char *membname;
	ctf_id_t src_membtype;
	int bit_width;

	/* Technically to match a struct or union we need to check both ways
	   (src members vs. dst, dst members vs. src) but we make this cheaper
	   by only checking src vs. dst and comparing the total size of the
	   structure (which we must do anyway) which covers the possibility of
	   dst members not in src.  This optimization can be defeated for
	   unions, but is so pathological as to render it irrelevant for our
	   purposes.  */

	if (dst_type != CTF_ERR && kind != CTF_K_FORWARD
	    && dst_kind != CTF_K_FORWARD)
	  {
	    if (ctf_type_size (src_fp, src_type) !=
		ctf_type_size (dst_fp, dst_type))
	      {
		ctf_err_warn (dst_fp, 1, ECTF_CONFLICT,
			      _("conflict for type %s against ID %lx: union "
				"size differs, old %li, new %li"), name,
			      dst_type, (long) ctf_type_size (src_fp, src_type),
			      (long) ctf_type_size (dst_fp, dst_type));
		return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
	      }

	    while ((offset = ctf_member_next (src_fp, src_type, &i,
					      &membname, NULL, &bit_width, 0)) >= 0)
	      {
		if (membcmp (membname, offset, bit_width, &dst) < 0)
		  {
		    ctf_next_destroy (i);
		    ctf_err_warn (dst_fp, 1, ECTF_CONFLICT,
				  _("conflict for type %s against ID %lx: members "
				    "differ, see above"), name, dst_type);
		    return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
		  }
	      }
	    if (ctf_errno (src_fp) != ECTF_NEXT_END)
	      return CTF_ERR;			/* errno is set for us.  */

	    break;
	  }

	dst_type = ctf_add_struct_sized (dst_fp, flag
					 | (bitfields ? CTF_ADD_STRUCT_BITFIELDS : 0),
					 name, ctf_type_size (src_fp, src_type));
	if (dst_type == CTF_ERR)
	  return CTF_ERR;			/* errno is set for us.  */

	/* Pre-emptively add this struct to the type mapping so that
	   structures that refer to themselves work.  */
	ctf_add_type_mapping (src_fp, src_type, dst_fp, dst_type);

	while ((offset = ctf_member_next (src_fp, src_type, &i, &membname,
					  &src_membtype, &bit_width, 0)) >= 0)
	  {
	    ctf_dict_t *dst = dst_fp;
	    ctf_id_t dst_membtype = ctf_type_mapping (src_fp, src_membtype, &dst);

	    if (dst_membtype == 0)
	      {
		dst_membtype = ctf_add_type_internal (dst_fp, src_fp,
						      src_membtype,
						      proc_tracking_fp);
		if (dst_membtype == CTF_ERR)
		  {
		    if (ctf_errno (dst_fp) != ECTF_NONREPRESENTABLE)
		      {
			ctf_next_destroy (i);	/* errno is set for us.  */
			break;
		      }
		  }
	      }

	    if (ctf_add_member_bitfield (dst_fp, dst_type, membname,
					 dst_membtype, offset, bit_width) < 0)
	      {
		ctf_next_destroy (i);		/* errno is set for us.  */
		break;
	      }
	  }
	if (ctf_errno (src_fp) != ECTF_NEXT_END)
	  return CTF_ERR;			/* errno is set for us.  */
	break;
      }

    case CTF_K_ENUM:
    case CTF_K_ENUM64:
      if (dst_type != CTF_ERR && kind != CTF_K_FORWARD
	  && dst_kind != CTF_K_FORWARD)
	{
	  if (ctf_enum_unsigned (src_fp, src_type)
	      != ctf_enum_unsigned (dst_fp, dst_type))
	    {
	      ctf_err_warn (dst_fp, 1, ECTF_CONFLICT,
			    _("conflict for enum %s against ID %lx: member "
			      "signedness differs"), name, dst_type);
	      return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
	    }
	  if (enumcmp (src_fp, src_type, &dst, dst_fp)
	      || enumcmp (dst_fp, dst_type, &src, dst_fp))
	    {
	      ctf_err_warn (dst_fp, 1, ECTF_CONFLICT,
			    _("conflict for enum %s against ID %lx: members "
			      "differ, see above"), name, dst_type);
	      return (ctf_set_typed_errno (dst_fp, ECTF_CONFLICT));
	    }
	}
      else
	{
	  ctf_snapshot_id_t snap = ctf_snapshot (dst_fp);
	  ctf_next_t *it = NULL;
	  const char *enum_name;
	  int64_t value;

	  if (src_type == CTF_K_ENUM)
	    dst_type = ctf_add_enum (dst_fp, flag, name);
	  else
	    dst_type = ctf_add_enum64 (dst_fp, flag, name);
	  if (dst_type == CTF_ERR)
	    goto enum_err;

	  while ((enum_name = ctf_enum_next (src_fp, src_type,
					     &it, &value)) != NULL)
	    {
	      if (ctf_add_enumerator (dst_fp, dst_type, enum_name, value) < 0)
		goto enum_err;
	    }
	  if (ctf_errno (src_fp) != ECTF_NEXT_END)
	    {
	      ctf_set_errno (dst_fp, ctf_errno (src_fp));
	      goto enum_err;
	    }

	  break;

	enum_err:
	  {
	    ctf_next_destroy (it);
	    ctf_rollback (dst_fp, snap);
	    return CTF_ERR;			/* errno is set for us.  */
	  }
	}
      break;

    case CTF_K_FORWARD:
      if (dst_type == CTF_ERR)
	  dst_type = ctf_add_forward (dst_fp, flag, name, forward_kind);
      break;

    case CTF_K_TYPEDEF:
      src_type = ctf_type_reference (src_fp, src_type);
      src_type = ctf_add_type_internal (dst_fp, src_fp, src_type,
					proc_tracking_fp);

      if (src_type == CTF_ERR)
	return CTF_ERR;				/* errno is set for us.  */

      /* If dst_type is not CTF_ERR at this point, we should check if
	 ctf_type_reference(dst_fp, dst_type) != src_type and if so fail with
	 ECTF_CONFLICT.  However, this causes problems with bitness typedefs
	 that vary based on things like if 32-bit then pid_t is int otherwise
	 long.  We therefore omit this check and assume that if the identically
	 named typedef already exists in dst_fp, it is correct or
	 equivalent.  */

      if (dst_type == CTF_ERR)
	  dst_type = ctf_add_typedef (dst_fp, flag, name, src_type);

      break;

    default:
      return (ctf_set_typed_errno (dst_fp, ECTF_CORRUPT));
    }

  if (dst_type != CTF_ERR)
    ctf_add_type_mapping (src_fp, orig_src_type, dst_fp, dst_type);
  return dst_type;
}

ctf_id_t
ctf_add_type (ctf_dict_t *dst_fp, ctf_dict_t *src_fp, ctf_id_t src_type)
{
  ctf_id_t id;

  if ((src_fp->ctf_flags & LCTF_NO_STR) || (dst_fp->ctf_flags & LCTF_NO_STR)
      || ((src_fp->ctf_flags & LCTF_CHILD) && (src_fp->ctf_parent == NULL))
      || ((dst_fp->ctf_flags & LCTF_CHILD) && (dst_fp->ctf_parent == NULL)))
    return (ctf_set_typed_errno (dst_fp, ECTF_NOPARENT));

  if (!src_fp->ctf_add_processing)
    src_fp->ctf_add_processing = ctf_dynhash_create (ctf_hash_integer,
						     ctf_hash_eq_integer,
						     NULL, NULL);

  /* We store the hash on the source, because it contains only source type IDs:
     but callers will invariably expect errors to appear on the dest.  */
  if (!src_fp->ctf_add_processing)
    return (ctf_set_typed_errno (dst_fp, ENOMEM));

  id = ctf_add_type_internal (dst_fp, src_fp, src_type, src_fp);
  ctf_dynhash_empty (src_fp->ctf_add_processing);

  return id;
}
