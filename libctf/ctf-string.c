/* CTF string table management.
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

#include <assert.h>
#include <ctf-impl.h>
#include <ctf-ref.h>
#include <string.h>

static ctf_str_atom_t *
ctf_str_add_ref_internal (ctf_dict_t *fp, const char *str,
			  int flags, uint32_t *ref);

/* Get the provisional offset, possibly climbing to the parent to do so.  */
static uint32_t
get_prov_offset (ctf_dict_t *fp)
{
  if (fp->ctf_parent)
    return fp->ctf_parent->ctf_str_prov_offset;
  else
    return fp->ctf_str_prov_offset;
}

/* Similarly, set it.  */
static void
set_prov_offset (ctf_dict_t *fp, uint32_t prov_offset)
{
  if (fp->ctf_parent)
    fp->ctf_parent->ctf_str_prov_offset = prov_offset;
  else
    fp->ctf_str_prov_offset = prov_offset;
}

/* Convert an encoded CTF string name into a pointer to a C string by looking
  up the appropriate string table buffer and then adding the offset.  */
const char *
ctf_strraw (ctf_dict_t *fp, uint32_t name)
{
  int stid_tab = CTF_NAME_STID (name);
  ctf_strs_t *ctsp = &fp->ctf_str[stid_tab];
  uint32_t prov_offset;

  /* Special case: "" is at position zero.  */

  if (name == 0)
    return "";

  /* If the name (adjusted to allow for names in the parent) is in the internal
     strtab, and the name offset is at least the ctf_str_prov_offset, this is a
     provisional string added by ctf_str_add*() but not yet built into a real
     strtab: get the value out of the ctf_prov_strtab.  This value is not
     adjusted to account for parent lengths or anything, it just descends from
     the top of the non-external string offset space, intermingling parent and
     child strings.  */

  prov_offset = get_prov_offset (fp);

  if (prov_offset < fp->ctf_str[CTF_STRTAB_0].cts_len)
    {
      ctf_set_errno (fp, ECTF_INTERNAL);
      ctf_err_warn (fp, 0, 0, _("internal error: overlapping strtabs!"));
    }

  /* Provisional strings may be in the parent as well as the child: check
     both.  (Provisional offsets cannot appear in both.)  */

  if (stid_tab == CTF_STRTAB_0 && name >= prov_offset)
    {
      const char *str;

      str = ctf_dynhash_lookup (fp->ctf_prov_strtab,
				(void *) (uintptr_t) name);
      if (!str && fp->ctf_parent)
	str = ctf_dynhash_lookup (fp->ctf_parent->ctf_prov_strtab,
				  (void *) (uintptr_t) name);
      return str;
    }

  /* Nonprovisional string.

     For dicts in a parent/child relationship, there are two phases to string
     lookup: before writeout, fp->ctf_parent->cts_len is 0, and the parent and
     child are uncorrelated and lookups start at offset 0; and after writeout,
     the parent's strings are incorporated into the child and further
     modification of the parent's strtab (even the addition of new strings) is
     prohibited.  */

  if (stid_tab == CTF_STRTAB_0)
    {
      if (_libctf_unlikely_ (fp->ctf_flags & LCTF_NO_STR))
	{
	  ctf_set_errno (fp, ECTF_NOPARENT);
	  ctf_err_warn (fp, 0, 0, _("internal error: attempt to look up strings in child before parent is imported"));
	  return NULL;
	}

      if (fp->ctf_parent
	  && _libctf_unlikely_ (fp->ctf_header->cth_parent_strlen != 0
				&& fp->ctf_header->cth_parent_strlen !=
				fp->ctf_parent->ctf_str[CTF_STRTAB_0].cts_len))
	{
	  ctf_set_errno (fp, ECTF_BADNAME);
	  ctf_err_warn (fp, 0, 0, _("lookup of string in child with wrongly-associated parent: "
				    "child dict's parent strtab offset: %x; "
				    "actual parent strtab offset: %zx"),
			fp->ctf_header->cth_parent_strlen,
			fp->ctf_parent->ctf_str[CTF_STRTAB_0].cts_len);
	  return NULL;
	}

      if (name < fp->ctf_header->cth_parent_strlen)
	ctsp = &fp->ctf_parent->ctf_str[CTF_STRTAB_0];
      else
	{
	  name -= fp->ctf_header->cth_parent_strlen;
	  ctsp = &fp->ctf_str[CTF_STRTAB_0];
	}
    }

  /* If this name is in the external strtab, and there is a synthetic strtab,
     and we are not serializing BTF right now, use it in preference.  (This is
     used to add the set of strings -- symbol names, etc -- the linker knows
     about before the strtab is written out.  The set is added to every dict, so
     we don't need to scan the parent.  Preventing this from operating during
     BTF serialization is sufficient to prevent external refs from appearing in
     BTF, because every string gets rescanned at that stage.)  */

  if (stid_tab == CTF_STRTAB_1 && fp->ctf_syn_ext_strtab != NULL
      && !fp->ctf_serialize.cs_is_btf)
    return ctf_dynhash_lookup (fp->ctf_syn_ext_strtab,
			       (void *) (uintptr_t) name);

  if (ctsp->cts_strs != NULL && CTF_NAME_OFFSET (name) < ctsp->cts_len)
    return (ctsp->cts_strs + CTF_NAME_OFFSET (name));

  ctf_err_warn (fp, 1, 0, _("offset %x: strtab not found or corrupt offset: cts_len is %zx, parent strlen is %u, cts_strs is %p, prov offset is %x, stid_tab is %u"),
		CTF_NAME_OFFSET (name), ctsp->cts_len, fp->ctf_header->cth_parent_strlen, ctsp->cts_strs, prov_offset, stid_tab);

  /* String table not loaded or corrupt offset.  */
  return NULL;
}

/* Return a guaranteed-non-NULL pointer to the string with the given CTF
   name.  */
const char *
ctf_strptr (ctf_dict_t *fp, uint32_t name)
{
  const char *s = ctf_strraw (fp, name);
  return (s != NULL ? s : "(?)");
}

/* As above, but return info on what is wrong in more detail.
   (Used for type lookups.) */

const char *
ctf_strptr_validate (ctf_dict_t *fp, uint32_t name)
{
  const char *str;

  ctf_set_errno (fp, 0);
  str = ctf_strraw (fp, name);

  /* Only report errors if ctf_strraw() didn't already.  */

  if (str == NULL && ctf_errno (fp) == 0)
    {
      if (CTF_NAME_STID (name) == CTF_STRTAB_1
	  && fp->ctf_syn_ext_strtab == NULL
	  && fp->ctf_str[CTF_NAME_STID (name)].cts_strs == NULL)
	{
	  ctf_set_errno (fp, ECTF_STRTAB);
	  return NULL;
	}

      ctf_set_errno (fp, ECTF_BADNAME);
      return NULL;
    }
  return str;
}

/* Free an atom.  */
static void
ctf_str_free_atom (void *a, void *fp_)
{
  ctf_str_atom_t *atom = a;
  ctf_dict_t *fp = fp_;

  ctf_purge_ref_list (fp, &atom->csa_refs);

  if (atom->csa_flags & CTF_STR_ATOM_FREEABLE)
    free (atom->csa_str);

  free (atom);
}

/* Create the atoms table.  There is always at least one atom in it, the null
   string: but also pull in atoms from the internal strtab.  (We rely on
   calls to ctf_str_add_external to populate external strtab entries, since
   these are often not quite the same as what appears in any external
   strtab, and the external strtab is often huge and best not aggressively
   pulled in.)

   Note that the *final strtab* may be entirely empty, if all its strings are
   shared with the parent: the atoms table is a superset.  (But this will never
   happen in practice, because some header fields are explicitly never
   deduplicated.)  */
int
ctf_str_create_atoms (ctf_dict_t *fp)
{
  size_t i;

  fp->ctf_str_atoms = ctf_dynhash_create_arg (ctf_hash_string, ctf_hash_eq_string,
					      NULL, ctf_str_free_atom, fp);
  if (!fp->ctf_str_atoms)
    return -ENOMEM;

  if (!fp->ctf_prov_strtab)
    fp->ctf_prov_strtab = ctf_dynhash_create (ctf_hash_integer,
					      ctf_hash_eq_integer,
					      NULL, NULL);
  if (!fp->ctf_prov_strtab)
    goto oom_prov_strtab;

  /* Pull in all the strings in the strtab as new atoms.  The provisional strtab
     must be empty at this point, so there is no need to populate atoms from it
     as well.  Types in this subset are frozen and readonly, so the refs list
     need not be populated.  The offsets are not parent-relative, so we don't
     need to have imported any dicts at this stage, and the parent need not be
     considered.  */

  for (i = 0; i < fp->ctf_str[CTF_STRTAB_0].cts_len;
       i += strlen (&fp->ctf_str[CTF_STRTAB_0].cts_strs[i]) + 1)
    {
      ctf_str_atom_t *atom;

      if (fp->ctf_str[CTF_STRTAB_0].cts_strs[i] == 0)
	continue;

      atom = ctf_str_add_ref_internal (fp, &fp->ctf_str[CTF_STRTAB_0].cts_strs[i],
				       0, 0);

      if (!atom)
	goto oom_str_add;

      atom->csa_offset = i;
    }

  /* Provisional offsets start from the offset before the STID-1 range and count
     down.  */
  fp->ctf_str_prov_offset = (1U << 31) - 1;
  fp->ctf_str_prov_len = 0;

  return 0;

 oom_str_add:
  ctf_dynhash_destroy (fp->ctf_prov_strtab);
  fp->ctf_prov_strtab = NULL;
 oom_prov_strtab:
  ctf_dynhash_destroy (fp->ctf_str_atoms);
  fp->ctf_str_atoms = NULL;
  return -ENOMEM;
}

/* Destroy the atoms table and associated refs.  */
void
ctf_str_free_atoms (ctf_dict_t *fp)
{
  ctf_dynhash_destroy (fp->ctf_prov_strtab);
  ctf_dynhash_destroy (fp->ctf_syn_ext_strtab);
  ctf_dynhash_destroy (fp->ctf_str_atoms);
  if (fp->ctf_dynstrtab)
    {
      free (fp->ctf_dynstrtab->cts_strs);
      free (fp->ctf_dynstrtab);
    }
}

#define CTF_STR_ADD_REF		0x1
#define CTF_STR_PROVISIONAL	0x2
#define CTF_STR_COPY		0x4
#define CTF_STR_NO_DEDUP	0x8

/* Add a string to the atoms table, copying the passed-in string if
   necessary.  Return the atom added. Return NULL only when out of memory
   (and do not touch the passed-in string in that case).

   Possibly add a provisional entry for this string to the provisional
   strtab.  If the string is in the provisional strtab, update its ref list
   with the passed-in ref, causing the ref to be updated when the strtab is
   written out.  */

static ctf_str_atom_t *
ctf_str_add_ref_internal (ctf_dict_t *fp, const char *str,
			  int flags, uint32_t *ref)
{
  char *newstr = NULL;
  ctf_str_atom_t *atom = NULL;
  int added = 0;
  ctf_dict_t *lookup_fp = fp;

  /* Check for existing atoms in the parent as well.  */

  atom = ctf_dynhash_lookup (fp->ctf_str_atoms, str);

  if (!atom && fp->ctf_parent)
    {
      lookup_fp = fp->ctf_parent;
      atom = ctf_dynhash_lookup (lookup_fp->ctf_str_atoms, str);
    }

  /* Existing atoms get refs added only if they are provisional:
     non-provisional strings already have a fixed strtab offset, and just
     get their ref updated immediately, since its value cannot change.  */

  if (atom)
    {
      if (flags & CTF_STR_NO_DEDUP)
	atom->csa_flags |= CTF_STR_ATOM_NO_DEDUP;

      if (atom->csa_offset < get_prov_offset (fp)
	  || (!fp->ctf_serialize.cs_is_btf && atom->csa_external_offset))
	{
	  if (flags & CTF_STR_ADD_REF)
	    {
	      if (!fp->ctf_serialize.cs_is_btf && atom->csa_external_offset)
		*ref = atom->csa_external_offset;
	      else
		*ref = atom->csa_offset + lookup_fp->ctf_header->cth_parent_strlen;
	    }
	  return atom;
	}

      if (flags & CTF_STR_ADD_REF)
	{
	  if (!ctf_create_ref (lookup_fp, &atom->csa_refs, ref))
	    {
	      ctf_set_errno (fp, ENOMEM);
	      return NULL;
	    }
	}

      return atom;
    }

  /* New atom.  */

  if (fp->ctf_str[CTF_STRTAB_0].cts_len != 0
      && fp->ctf_max_children != 0
      && !(flags & CTF_STR_PROVISIONAL))
    {
      ctf_set_errno (fp, ECTF_RDONLY);
      ctf_err_warn (fp, 0, 0, _("attempt to add non-provisional strings to an "
				"already-serialized parent dict"));
      return NULL;
    }

  if (flags & CTF_STR_PROVISIONAL)
    {
      if (get_prov_offset (fp) < fp->ctf_header->cth_parent_strlen
	  + fp->ctf_str[CTF_STRTAB_0].cts_len)
	{
	  ctf_set_errno (fp, ECTF_FULL);
	  ctf_err_warn (fp, 0, 0, _("strtab is full: cannot add more strings"));
	  return NULL;
	}
    }

  if ((atom = malloc (sizeof (struct ctf_str_atom))) == NULL)
    goto oom;
  memset (atom, 0, sizeof (struct ctf_str_atom));

  if (flags & CTF_STR_NO_DEDUP)
    atom->csa_flags |= CTF_STR_ATOM_NO_DEDUP;

  /* Special case: there is always only one "", and it is always in the parent
     if there is a parent/child relationship in force (even though it is
     explicitly skipped in the deduplicator; see ctf_dedup_strings).  */
  if (str[0] == 0)
    atom->csa_flags |= CTF_STR_ATOM_IN_PARENT;

  /* Don't allocate new strings if this string is within an mmapped
     strtab, unless forced.  */

  if (flags & CTF_STR_COPY
      || ((unsigned char *) str < (unsigned char *) fp->ctf_data_mmapped
	  || (unsigned char *) str > (unsigned char *) fp->ctf_data_mmapped + fp->ctf_data_mmapped_len))
    {
      if ((newstr = strdup (str)) == NULL)
	goto oom;
      atom->csa_flags |= CTF_STR_ATOM_FREEABLE;
      atom->csa_str = newstr;
    }
  else
    atom->csa_str = (char *) str;

  if (ctf_dynhash_insert (fp->ctf_str_atoms, atom->csa_str, atom) < 0)
    goto oom;
  added = 1;

  atom->csa_snapshot_id = fp->ctf_snapshots;

  /* New atoms marked provisional go into the provisional strtab, and get a ref
     added.  Provisional offsets are shared among the parent and all children.

     Special-case "" again: it gets a real offset of zero, not a high
     provisional one.  This atom's offset is never returned (see the special
     case in ctf_strraw) and mostly exists for the sake of the deduplicator.  */

  if (flags & CTF_STR_PROVISIONAL)
    {
      if (str[0] == 0)
	atom->csa_offset = 0;
      else
	{
	  set_prov_offset (fp, get_prov_offset (fp) - strlen (atom->csa_str) - 1);
	  atom->csa_offset = get_prov_offset (fp);
	  fp->ctf_str_prov_len += strlen (atom->csa_str) + 1;
	}

      if (ctf_dynhash_insert (fp->ctf_prov_strtab, (void *) (uintptr_t)
			      atom->csa_offset, (void *) atom->csa_str) < 0)
	goto oom;

      if (flags & CTF_STR_ADD_REF)
      {
	if (!ctf_create_ref (fp, &atom->csa_refs, ref))
	  goto oom;
      }
    }

  return atom;

 oom:
  if (added)
    ctf_dynhash_remove (fp->ctf_str_atoms, atom->csa_str);
  free (atom);
  free (newstr);
  ctf_set_errno (fp, ENOMEM);
  return NULL;
}

static uint32_t
ctf_str_add_flagged (ctf_dict_t *fp, const char *str, uint32_t *ref,
		     int flags)
{
  ctf_str_atom_t *atom;
  uint32_t offset;

  if (!str)
    str = "";

  atom = ctf_str_add_ref_internal (fp, str, flags, ref);
  /* TODO handle failure better */
  if (!atom)
    return 0;

  if (!fp->ctf_serialize.cs_is_btf && atom->csa_external_offset)
    offset = atom->csa_external_offset;
  else
    offset = atom->csa_offset;

  return offset;
}

/* Add a string to the atoms table, without augmenting the ref list for this
   string: if the string is not already known, return a 'provisional offset'
   which can be used to return this string until ctf_str_write_strtab is called,
   or 0 on failure.  (Everywhere the provisional offset is assigned to should be
   added as a ref using ctf_str_add_ref() as well.)

   If this atom is already known to have an external offset, the external offset
   is simply returned unchanged.  */
uint32_t
ctf_str_add (ctf_dict_t *fp, const char *str)
{
  return ctf_str_add_flagged (fp, str, 0, CTF_STR_PROVISIONAL);
}

/* Like ctf_str_add, but always take a copy of the string rather than using a
   reference into an mmapped region where possible.  Useful only when sharing
   strings between dicts (which is rare indeed).  */
uint32_t
ctf_str_add_copy (ctf_dict_t *fp, const char *str)
{
  return ctf_str_add_flagged (fp, str, 0, CTF_STR_PROVISIONAL | CTF_STR_COPY);
}

/* Like ctf_str_add(), but additionally augment the atom's refs list with the
   passed-in ref, whether or not the string is already present.  There is no
   attempt to deduplicate the refs list (but duplicates are harmless).  */
uint32_t
ctf_str_add_ref (ctf_dict_t *fp, const char *str, uint32_t *ref)
{
  return ctf_str_add_flagged (fp, str, ref,
			      CTF_STR_ADD_REF | CTF_STR_PROVISIONAL);
}

/* Like ctf_str_add_ref(), but prevent this string from being deduplicated.  */
uint32_t
ctf_str_add_no_dedup_ref (ctf_dict_t *fp, const char *str, uint32_t *ref)
{
  return ctf_str_add_flagged (fp, str, ref,
			      CTF_STR_ADD_REF | CTF_STR_PROVISIONAL
			      | CTF_STR_NO_DEDUP);
}

/* Add an external strtab reference at OFFSET.  Returns zero if the addition
   failed, nonzero otherwise.  */
int
ctf_str_add_external (ctf_dict_t *fp, const char *str, uint32_t offset)
{
  ctf_str_atom_t *atom;

  if (!str)
    str = "";

  atom = ctf_str_add_ref_internal (fp, str, 0, 0);
  if (!atom)
    return 0;

  atom->csa_external_offset = CTF_SET_STID (offset, CTF_STRTAB_1);

  /* The "synthetic external strtab" contains all strings that the linker has
     told us about, kept around so that we can look them up by external offset
     even in situations in which no ELF information is available, such as
     during late serialization.  */

  if (!fp->ctf_syn_ext_strtab)
    fp->ctf_syn_ext_strtab = ctf_dynhash_create (ctf_hash_integer,
						 ctf_hash_eq_integer,
						 NULL, NULL);
  if (!fp->ctf_syn_ext_strtab)
    {
      ctf_set_errno (fp, ENOMEM);
      return 0;
    }

  if (ctf_dynhash_insert (fp->ctf_syn_ext_strtab,
			  (void *) (uintptr_t)
			  atom->csa_external_offset,
			  (void *) atom->csa_str) < 0)
    {
      ctf_dynhash_destroy (fp->ctf_syn_ext_strtab);
      fp->ctf_syn_ext_strtab = NULL;

      ctf_set_errno (fp, ENOMEM);
      return 0;
    }

  return 1;
}

/* A ctf_dynhash_iter_remove() callback that removes atoms later than a given
   snapshot ID.  External atoms are never removed, because they came from the
   linker string table and are still present even if you roll back type
   additions.  */
static int
ctf_str_rollback_atom (void *key _libctf_unused_, void *value, void *arg)
{
  ctf_str_atom_t *atom = (ctf_str_atom_t *) value;
  ctf_snapshot_id_t *id = (ctf_snapshot_id_t *) arg;

  return (atom->csa_snapshot_id > id->snapshot_id)
    && (atom->csa_external_offset == 0);
}

/* Roll back, deleting all (internal) atoms created after a particular ID.  */
void
ctf_str_rollback (ctf_dict_t *fp, ctf_snapshot_id_t id)
{
  ctf_dynhash_iter_remove (fp->ctf_str_atoms, ctf_str_rollback_atom, &id);
}

/* An adaptor around ctf_purge_ref_list.  */
static void
ctf_str_purge_one_atom_refs (void *key _libctf_unused_, void *value,
			     void *arg)
{
  ctf_str_atom_t *atom = (ctf_str_atom_t *) value;
  ctf_dict_t *fp = (ctf_dict_t *) arg;

  ctf_purge_ref_list (fp, &atom->csa_refs);
}

/* Remove all the recorded refs from the atoms table.  */
void
ctf_str_purge_refs (ctf_dict_t *fp)
{
  ctf_dynhash_iter (fp->ctf_str_atoms, ctf_str_purge_one_atom_refs, fp);
}

/* Sort the strtab.  */
static int
ctf_str_sort_strtab (const void *a, const void *b)
{
  ctf_str_atom_t **one = (ctf_str_atom_t **) a;
  ctf_str_atom_t **two = (ctf_str_atom_t **) b;

  return (strcmp ((*one)->csa_str, (*two)->csa_str));
}

/* Write out and return a strtab containing all strings with recorded refs,
   adjusting the refs to refer to the corresponding string.  The returned
   strtab is already assigned to strtab 0 in this dict, is owned by this
   dict, and may be NULL on error.  Also populate the synthetic strtab with
   mappings from external strtab offsets to names, so we can look them up
   with ctf_strptr().  Only external strtab offsets with references are
   added.

   As a side effect, replaces the strtab of the current dict with the newly-
   generated strtab.  This is an exception to the general rule that
   serialization does not change the dict passed in, because the alternative
   is to copy the entire atoms table on every reserialization just to avoid
   modifying the original, which is excessively costly for minimal gain.
   There can be no references to the strings in the newly-added portion
   of the strtab on return, though some may appear at a later date.

   We use the lazy man's approach and double memory costs by always storing
   atoms as individually allocated entities whenever they come from anywhere
   but a freshly-opened, mmapped dict, even though after serialization there
   is another copy in the strtab; this ensures that ctf_strptr()-returned
   pointers to them remain valid for the lifetime of the dict.

   This is all rendered more complex because if a dict is ctf_open()ed it
   will have a bunch of strings in its strtab already, and their strtab
   offsets can never change (without piles of complexity to rescan the
   entire dict just to get all the offsets to all of them into the atoms
   table).  Entries below the existing strtab limit are just copied into the
   new dict: entries above it are new, and are are sorted first, then
   appended to it.  The sorting is purely a compression-efficiency
   improvement, and we get nearly as good an improvement from sorting big
   chunks like this as we would from sorting the whole thing.  */

const ctf_strs_writable_t *
ctf_str_write_strtab (ctf_dict_t *fp)
{
  ctf_strs_writable_t *strtab;
  size_t strtab_count = 0;
  uint32_t cur_stroff = 0;
  ctf_str_atom_t **sorttab;
  ctf_next_t *it = NULL;
  size_t i;
  void *v;
  int err;
  int new_strtab = 0;
  uint32_t prov_offset;

  /* Writing a full v4 shared-with-parent child strtab is possible only if the
     parent has already been written out.  */

  if (fp->ctf_parent && fp->ctf_header->cth_parent_strlen != 0)
    {
      if (fp->ctf_parent->ctf_str_prov_len != 0)
	{
	  ctf_set_errno (fp, ECTF_NOTSERIALIZED);
	  ctf_err_warn (fp, 0, 0, _("attempt to write strtab with unserialized parent"));
	  return NULL;
	}

      /* Writing such a child strtab is possible only if the parent strtab has not
	 changed length.  */

      if (fp->ctf_header->cth_parent_strlen != fp->ctf_parent->ctf_str[CTF_STRTAB_0].cts_len)
	{
	  ctf_set_errno (fp, ECTF_WRONGPARENT);
	  ctf_err_warn (fp, 0, 0, _("cannot serialize child strtab: "
				    "parent strtab has changed length from %x to %zx\n"),
			fp->ctf_header->cth_parent_strlen,
			fp->ctf_parent->ctf_str[CTF_STRTAB_0].cts_len);
	  return NULL;
	}
    }

  strtab = calloc (1, sizeof (ctf_strs_writable_t));
  if (!strtab)
    return NULL;

  /* The strtab contains the existing string table at its start: figure out how
     many new strings we need to add.  We only need to add new strings that have
     no external offset (or are BTF, for which external offsets are ignored),
     that have refs, and that are found in the provisional strtab.  If the
     existing strtab is empty and has no parent strings, we also need to add the
     null string at its start.  (Dicts promoted from CTFv3 and below always have
     no parent strings in this sense.)  */

  strtab->cts_len = fp->ctf_str[CTF_STRTAB_0].cts_len;

  if (strtab->cts_len == 0 && fp->ctf_header->cth_parent_strlen == 0)
    {
      new_strtab = 1;
      strtab->cts_len++; 			/* For the \0.  */
    }

  /* Count new entries in the strtab: i.e. entries in the provisional strtab, in
     the provisional range.  Ignore any entry for \0, entries which ended up in
     the external strtab, and unreferenced entries.  */

  prov_offset = get_prov_offset (fp);

  while ((err = ctf_dynhash_next (fp->ctf_prov_strtab, &it, NULL, &v)) == 0)
    {
      const char *str = (const char *) v;
      ctf_str_atom_t *atom;

      atom = ctf_dynhash_lookup (fp->ctf_str_atoms, str);
      if (!ctf_assert (fp, atom))
	goto err_strtab;

      if (atom->csa_str[0] == 0
	  || (!fp->ctf_serialize.cs_is_btf && atom->csa_external_offset)
	  || atom->csa_offset < prov_offset
	  || ctf_list_empty_p (&atom->csa_refs))
	continue;

      strtab->cts_len += strlen (atom->csa_str) + 1;
      strtab_count++;
    }
  if (err != ECTF_NEXT_END)
    {
      ctf_dprintf ("ctf_str_write_strtab: error counting strtab entries: %s\n",
		   ctf_errmsg (err));
      goto err_strtab;
    }

  ctf_dprintf ("%lu bytes of strings in strtab: %lu pre-existing.\n",
	       (unsigned long) strtab->cts_len,
	       (unsigned long) fp->ctf_str[CTF_STRTAB_0].cts_len);

  /* Sort the new part of the strtab.  */

  sorttab = calloc (strtab_count, sizeof (ctf_str_atom_t *));
  if (!sorttab)
    {
      ctf_set_errno (fp, ENOMEM);
      goto err_strtab;
    }

  i = 0;
  while ((err = ctf_dynhash_next (fp->ctf_prov_strtab, &it, NULL, &v)) == 0)
    {
      ctf_str_atom_t *atom;

      atom = ctf_dynhash_lookup (fp->ctf_str_atoms, v);
      if (!ctf_assert (fp, atom))
	goto err_sorttab;

      if (atom->csa_str[0] == 0
	  || (!fp->ctf_serialize.cs_is_btf && atom->csa_external_offset)
	  || atom->csa_offset < prov_offset
	  || ctf_list_empty_p (&atom->csa_refs))
	continue;

      sorttab[i++] = atom;
    }

  qsort (sorttab, strtab_count, sizeof (ctf_str_atom_t *),
	 ctf_str_sort_strtab);

  if ((strtab->cts_strs = malloc (strtab->cts_len)) == NULL)
    goto err_sorttab;

  cur_stroff = fp->ctf_str[CTF_STRTAB_0].cts_len;

  if (new_strtab)
    {
      strtab->cts_strs[0] = 0;
      cur_stroff++;
    }
  else
    memcpy (strtab->cts_strs, fp->ctf_str[CTF_STRTAB_0].cts_strs,
	    fp->ctf_str[CTF_STRTAB_0].cts_len);

  /* Work over the sorttab, add its strings to the strtab, and remember
     where they are in the csa_offset for the appropriate atom.  No ref
     updating is done at this point, because refs might well relate to
     already-existing strings, or external strings, which do not need adding
     to the strtab and may not be in the sorttab.  */

  for (i = 0; i < strtab_count; i++)
    {
      sorttab[i]->csa_offset = cur_stroff;
      strcpy (&strtab->cts_strs[cur_stroff], sorttab[i]->csa_str);
      cur_stroff += strlen (sorttab[i]->csa_str) + 1;
    }
  free (sorttab);
  sorttab = NULL;

  /* Update all refs (incorporating any parent strtab offset adjustment), then
     purge them as no longer necessary: also update the strtab appropriately.
     Some atoms (with refs updated after the parent was serialized) may be in
     the parent: use the parent's csa_offset instead -- but not its ref list,
     which will already have been updated and emptied. */

  while ((err = ctf_dynhash_next (fp->ctf_str_atoms, &it, NULL, &v)) == 0)
    {
      ctf_str_atom_t *atom = (ctf_str_atom_t *) v;
      uint32_t offset;

      if (ctf_list_empty_p (&atom->csa_refs))
	continue;

      if (!fp->ctf_serialize.cs_is_btf && atom->csa_external_offset)
	offset = atom->csa_external_offset;
      else
	{
	  if (atom->csa_flags & CTF_STR_ATOM_IN_PARENT
	      && fp->ctf_parent)
	    {
	      ctf_str_atom_t *parent_atom;

	      parent_atom = ctf_dynhash_lookup (fp->ctf_parent->ctf_str_atoms,
						atom->csa_str);
	      if (parent_atom)
		offset = parent_atom->csa_offset;
	      else
		offset = atom->csa_offset + fp->ctf_header->cth_parent_strlen;

	      atom->csa_flags &= ~CTF_STR_ATOM_IN_PARENT;
	    }
	  else
	    offset = atom->csa_offset + fp->ctf_header->cth_parent_strlen;
	}

      if (!ctf_assert (fp, offset < prov_offset))
	goto err_strtab;

      ctf_update_refs (&atom->csa_refs, offset);
    }
  if (err != ECTF_NEXT_END)
    {
      ctf_dprintf ("ctf_str_write_strtab: error iterating over atoms while updating refs: %s\n",
		   ctf_errmsg (err));
      goto err_strtab;
    }
  ctf_str_purge_refs (fp);

  /* Replace the old strtab with the new one in this dict.  */

  if (fp->ctf_dynstrtab)
    {
      free (fp->ctf_dynstrtab->cts_strs);
      free (fp->ctf_dynstrtab);
    }

  fp->ctf_dynstrtab = strtab;
  fp->ctf_str[CTF_STRTAB_0].cts_strs = strtab->cts_strs;
  fp->ctf_str[CTF_STRTAB_0].cts_len = strtab->cts_len;

  /* Note that all strings have been written out.  */
  fp->ctf_str_prov_len = 0;

  return strtab;

 err_sorttab:
  free (sorttab);
 err_strtab:
  free (strtab);
  return NULL;
}
