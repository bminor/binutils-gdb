/* Type handling functions.
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
#include <assert.h>
#include <string.h>

#ifndef roundup
#define roundup(x, y)  ((((x) + ((y) - 1)) / (y)) * (y))
#endif

/* Determine whether a type is a parent or a child.  Bad IDs are not
   diagnosed!  */

ctf_bool_t
ctf_type_isparent (const ctf_dict_t *fp, ctf_id_t id)
{
  /* All types visible in the parent are parent types, by definition.  */

  if (!(fp->ctf_flags & LCTF_CHILD))
    return 1;

  /* Not imported: no provisional types are possible because no types can
     have been added.  Simple range check.  */

  if (!fp->ctf_parent)
    return (fp->ctf_header->cth_parent_ntypes >= id);

  /* Types in the parent's idmax range (which encompasses its stypes range) are
     in the parent.  */

  if (id <= fp->ctf_parent->ctf_idmax)
    return 1;

  /* Types in the provisional ID range are in the parent: otherwise, they are in
     the child.  */

  if (id >= fp->ctf_parent->ctf_provtypemax)
    return (ctf_dynhash_lookup (fp->ctf_dthash,
				(void *) (uintptr_t) id) == NULL);

  /* Child type.  */
  return 0;
}

ctf_bool_t
ctf_type_ischild (const ctf_dict_t *fp, ctf_id_t id)
{
  return (!ctf_type_isparent (fp, id));
}

/* Get the index in the internal type array (or otherwise) for a given type ID.
   Only ever called on the right dictionary for the type, and can fail otherwise.
   If called on an invalid type, may return an index that does not correspond to
   any type (such as -1), but will not return an index that does correspond to a
   type.  */

static uint32_t
ctf_type_to_index_internal (const ctf_dict_t *fp, ctf_id_t type)
{
  uint32_t idx = type;

  assert (((fp->ctf_flags & LCTF_CHILD) && (type > fp->ctf_header->cth_parent_ntypes)) ||
	  (!(fp->ctf_flags & LCTF_CHILD)));

  if (fp->ctf_flags & LCTF_CHILD)
    {
      /* Non-dynamic type in parent: no index permitted.  */

      assert (type > fp->ctf_header->cth_parent_ntypes);

      idx -= fp->ctf_header->cth_parent_ntypes;
    }

  if (idx <= fp->ctf_stypes)
    return idx;

  /* Dynamic types.  In children this is easy.  */

  if (fp->ctf_flags & LCTF_CHILD)
    return idx;

  /* For parents, there are three ranges of types: below stypes (static), above
     stypes and below typemax - nprovtypes (dynamic, non-provisional, added
     before any children were imported, type ID derived identically to stypes),
     and above that (provisional, running backwards from the top of the ID
     space).  We have already handled the first.  Once we start inserting
     provisional types, no further nonprovisional types can be inserted:
     typemax, provtypemax and nprovtypes will rise in concert.  */

  if (idx <= (fp->ctf_typemax - fp->ctf_nprovtypes))
    return type;
  else /* Provisional type.  */
    return fp->ctf_typemax - (type - fp->ctf_provtypemax);
}

/* Verification of type_to_index -> index_to_type roundtripping.
   Doubles the cost of this core operation, so done under
   hash debugging only.  */

uint32_t
ctf_type_to_index (const ctf_dict_t *fp, ctf_id_t type)
{
  uint32_t idx = ctf_type_to_index_internal (fp, type);

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
  assert (ctf_index_to_type (fp, idx) == type);
#endif
  return idx;
}

/* The inverse of ctf_type_to_index.  */
ctf_id_t
ctf_index_to_type (const ctf_dict_t *fp, uint32_t idx)
{
  if (fp->ctf_flags & LCTF_CHILD)
    return idx + fp->ctf_header->cth_parent_ntypes;

  if (idx <= (fp->ctf_typemax - fp->ctf_nprovtypes))
    return idx;
  else	/* Provisional type.  */
    return fp->ctf_provtypemax + (fp->ctf_typemax - idx);
}

/* Figure out the vlen and optionally vlen length for some type.  */

static unsigned char *
ctf_vlen (ctf_dict_t *fp, ctf_id_t type, const ctf_type_t *tp, size_t *vlen_len)
{
  ctf_dtdef_t *dtd;

  if ((dtd = ctf_dynamic_type (fp, type)) != NULL)
    {
      if (vlen_len)
	*vlen_len = LCTF_VLEN (fp, dtd->dtd_buf);
      return dtd->dtd_vlen;
    }
  else
    {
      ssize_t size, increment;

      ctf_get_ctt_size (fp, tp, &size, &increment);
      if (vlen_len)
	*vlen_len = LCTF_VLEN (fp, tp);
      return (unsigned char *) tp + increment;
    }
}

/* Iterate over the members of a STRUCT or UNION, returning each member's
   offset and optionally name and member type in turn.  On end-of-iteration,
   returns -1.  If FLAGS is CTF_MN_RECURSE, recurse into unnamed members.  */

ssize_t
ctf_member_next (ctf_dict_t *fp, ctf_id_t type, ctf_next_t **it,
		 const char **name, ctf_id_t *membtype,
		 int *bit_width, int flags)
{
  ctf_dict_t *ofp = fp;
  uint32_t kind;
  ssize_t offset;
  size_t nmemb;
  unsigned char *vlen;
  ctf_next_t *i = *it;
  const ctf_type_t *prefix, *tp;
  ctf_error_t err;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if (!i)
    {
      const ctf_type_t *tp;

      if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
	return -1;			/* errno is set for us.  */

      if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
	return -1;			/* errno is set for us.  */

      kind = LCTF_KIND (fp, tp);

      if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
	return (ctf_set_errno (ofp, ECTF_NOTSOU));

      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_errno (ofp, ENOMEM);

      i->ctn_tp = tp;
      i->cu.ctn_fp = ofp;
      i->u.ctn_dtd = ctf_dynamic_type (fp, type);

      i->ctn_iter_fun = (void (*) (void)) ctf_member_next;
      i->ctn_n = 0;
      *it = i;
    }

  if ((void (*) (void)) ctf_member_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (ofp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  /* Resolve to the native dict of this type.  */
  if ((fp = ctf_get_dict (ofp, type)) == NULL)
    {
      err = ECTF_NOPARENT;
      goto end;
    }

  vlen = ctf_vlen (fp, type, i->ctn_tp, &nmemb);

  /* Reset the tp on every iteration if this is a dynamic type: adding members
     can move it, and hunt down any CTF_K_BIG prefix.  */

  if (i->u.ctn_dtd)
    i->ctn_tp = i->u.ctn_dtd->dtd_buf;

  if ((prefix = ctf_find_prefix (fp, i->ctn_tp, CTF_K_BIG)) == NULL)
    prefix = i->ctn_tp;
  tp = prefix;

  while (LCTF_IS_PREFIXED_KIND (LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info)))
    tp++;

  /* When we hit an unnamed struct/union member, we set ctn_type to indicate
     that we are inside one, then return the unnamed member: on the next call,
     we must skip over top-level member iteration in favour of iteration within
     the sub-struct until it later turns out that that iteration has ended.  */

 retry:
  if (!i->i.ctn_type)
    {
      ctf_member_t *memb = (ctf_member_t *) vlen;
      const char *membname;

      if (i->ctn_n >= nmemb)
	{
	  err = ECTF_NEXT_END;
	  goto end;
	}

      membname = ctf_strptr (fp, memb[i->ctn_n].ctm_name);

      /* Skip nameless padding types.  */
      if (membname[0] == 0 && memb[i->ctn_n].ctm_type == 0)
	{
	  i->ctn_n++;
	  goto retry;
	}

      if (name)
	*name = membname;
      if (membtype)
	*membtype = memb[i->ctn_n].ctm_type;
      if (bit_width)
	{
	  if (CTF_INFO_KFLAG (tp->ctt_info))
	    *bit_width = CTF_MEMBER_BIT_SIZE (memb[i->ctn_n].ctm_offset);
	  else
	    *bit_width = 0;
	}

      if (CTF_INFO_KFLAG (tp->ctt_info))
	offset = CTF_MEMBER_BIT_OFFSET (memb[i->ctn_n].ctm_offset);
      else
	offset = memb[i->ctn_n].ctm_offset;

      /* CTF_K_BIG offsets are gap sizes: convert to offset-from-start.
	 Keep track of the offset-so-far in ctn_size.  */

      if (prefix != tp)
	{
	  i->ctn_size += offset;
	  offset = i->ctn_size;
	}

      if (membname[0] == 0)
	{
	  ctf_id_t resolved;

	  if ((resolved = ctf_type_resolve (fp, memb[i->ctn_n].ctm_type)) == CTF_ERR)
	    {
	      if (ctf_errno (fp) != ECTF_NONREPRESENTABLE)
		{
		  ctf_err_warn (fp, 0, 0, _("Error resolving type %lx during unnamed member lookup"),
				(ctf_id_t) memb[i->ctn_n].ctm_type);
		  err = ctf_errno (fp);
		  goto end;
		}
	      resolved = memb[i->ctn_n].ctm_type;
	    }

	  if (ctf_type_kind (fp, resolved) == CTF_K_STRUCT
	      || ctf_type_kind (fp, resolved) == CTF_K_UNION)
	    i->i.ctn_type = resolved;
	}
      i->ctn_n++;

      /* The callers might want automatic recursive sub-struct traversal.  */
      if (!(flags & CTF_MN_RECURSE))
	i->i.ctn_type = 0;

      /* Sub-struct traversal starting?  Take note of the offset of this member,
	 for later boosting of sub-struct members' offsets.  */
      if (i->i.ctn_type)
	i->ctn_increment = offset;
    }
  /* Traversing a sub-struct?  Just return it, with the offset adjusted.  */
  else
    {
      ssize_t ret = ctf_member_next (fp, i->i.ctn_type, &i->ctn_next, name,
				     membtype, bit_width, flags);

      if (ret >= 0)
	return ret + i->ctn_increment;

      if (ctf_errno (fp) != ECTF_NEXT_END)
	{
	  err = ctf_errno (fp);
	  goto end;
	}

      if (!ctf_assert (fp, (i->ctn_next == NULL)))
	{
	  err = ctf_errno (fp);
	  goto end;
	}

      i->i.ctn_type = 0;
      /* This sub-struct has ended: on to the next real member.  */
      goto retry;
    }

  return offset;

 end:
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_errno (ofp, err);
}

/* Iterate over the members of an enum TYPE, returning each enumerand's NAME or
   NULL at end of iteration or error, and optionally passing back the
   enumerand's integer VALue.  */

const char *
ctf_enum_next (ctf_dict_t *fp, ctf_id_t type, ctf_next_t **it,
	       int64_t *val)
{
  ctf_dict_t *ofp = fp;
  uint32_t kind;
  const char *name;
  ctf_next_t *i = *it;
  ctf_error_t err;

  if (fp->ctf_flags & LCTF_NO_STR)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  if (!i)
    {
      const ctf_type_t *tp;
      unsigned char *en;

      if ((type = ctf_type_resolve_unsliced (fp, type)) == CTF_ERR)
	return NULL;			/* errno is set for us.  */

      kind = ctf_type_kind (fp, type);

      if ((kind != CTF_K_ENUM) && (kind != CTF_K_ENUM64))
	{
	  ctf_next_destroy (i);
	  ctf_set_errno (ofp, ECTF_NOTENUM);
	  return NULL;
	}

      if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
	return NULL;			/* errno is set for us.  */

      if ((i = ctf_next_create ()) == NULL)
	{
	  ctf_set_errno (ofp, ENOMEM);
	  return NULL;
	}
      i->cu.ctn_fp = ofp;

      i->ctn_iter_fun = (void (*) (void)) ctf_enum_next;
      en = ctf_vlen (fp, type, tp, &i->ctn_n);

      if (kind == CTF_K_ENUM)
	i->u.ctn_en = (const ctf_enum_t *) en;
      else /* CTF_K_ENUM64 */
	i->u.ctn_en64 = (const ctf_enum64_t *) en;

      *it = i;
    }

  if ((void (*) (void)) ctf_enum_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (ofp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  /* Resolve to the native dict of this type.  */
  if ((fp = ctf_get_dict (ofp, type)) == NULL)
    {
      err = ECTF_NOPARENT;
      goto end;
    }

  if (i->ctn_n == 0)
    {
      err = ECTF_NEXT_END;
      goto end;
    }

  if (ctf_type_kind (fp, type) == CTF_K_ENUM)
    {
      name = ctf_strptr (fp, i->u.ctn_en->cte_name);
      if (val)
	*val = i->u.ctn_en->cte_value;
      i->u.ctn_en++;
    }
  else
    {
      name = ctf_strptr (fp, i->u.ctn_en64->cte_name);
      if (val)
	*val = ((uint64_t) i->u.ctn_en64->cte_val_high) << 32 | (i->u.ctn_en64->cte_val_low);
      i->u.ctn_en64++;
    }
  i->ctn_n--;

  return name;

 end:
  ctf_next_destroy (i);
  *it = NULL;
  ctf_set_errno (ofp, err);
  return NULL;
}

/* Iterate over every type in the given CTF dict, optionally including
   non-user-visible types, returning each type ID and hidden flag in turn.
   Returns CTF_ERR on end of iteration or error.

   Does not traverse parent types: you have to do that explicitly.  This is by
   design, to avoid traversing them more than once if traversing many children
   of a single parent.  */

ctf_id_t
ctf_type_next (ctf_dict_t *fp, ctf_next_t **it, int *flag, int want_hidden)
{
  ctf_next_t *i = *it;
  ctf_error_t err;

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_typed_errno (fp, ENOMEM);

      i->cu.ctn_fp = fp;
      i->i.ctn_idx = 1;
      i->ctn_iter_fun = (void (*) (void)) ctf_type_next;
      *it = i;
    }

  if ((void (*) (void)) ctf_type_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (fp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  while (i->i.ctn_idx <= fp->ctf_typemax)
    {
      const ctf_type_t *tp;

      if (i->i.ctn_idx > fp->ctf_stypes)
	tp = ctf_dtd_lookup (fp, ctf_index_to_type (fp, i->i.ctn_idx))->dtd_data;
      else
	tp = fp->ctf_txlate[i->i.ctn_idx];

      if ((!want_hidden) && (!LCTF_INFO_ISROOT (fp, tp->ctt_info)))
	{
	  i->i.ctn_idx++;
	  continue;
	}

      if (flag)
	*flag = !LCTF_INFO_ISROOT (fp, tp->ctt_info);
      return ctf_index_to_type (fp, i->i.ctn_idx++);
    }

  err = ECTF_NEXT_END;
  /* Fall through.  */

 end:
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_typed_errno (fp, err);
}

/* UPTODO optimize this */

/* Iterate over every type of a given kind in the given CTF dict, returning each
   type ID in turn.  Returns CTF_ERR on end of iteration or error.  you can also
   specifically traverse prefixed types by asking for CTF_K_BIG or CTF_K_HIDDEN.
   In this case, the kind of the type returned will be different (the kind
   prefixed).  Using CTF_K_BIG may have unexpected results if types have been
   added to this dict since it was opened, since types are only 'unbiggened'
   upon being written out.

   Does not traverse parent types: you have to do that explicitly.  This is by
   design, to avoid traversing them more than once if traversing many children
   of a single parent.  */

ctf_id_t
ctf_type_kind_next (ctf_dict_t *fp, ctf_next_t **it, int kind)
{
  ctf_next_t *i = *it;
  ctf_error_t err;

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_typed_errno (fp, ENOMEM);

      i->cu.ctn_fp = fp;
      i->i.ctn_idx = 1;
      i->ctn_iter_fun = (void (*) (void)) ctf_type_kind_next;
      *it = i;
    }

  if ((void (*) (void)) ctf_type_kind_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (fp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  while (i->i.ctn_idx <= fp->ctf_typemax)
    {
      ctf_id_t type = ctf_index_to_type (fp, i->i.ctn_idx);

      if (ctf_type_kind (i->cu.ctn_fp, type) != kind)
	{
	  i->i.ctn_idx++;
	  continue;
	}

      i->i.ctn_idx++;
      return type;
    }

  err = ECTF_NEXT_END;
  /* Fall through.  */

 end:
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_typed_errno (fp, err);
}

/* Iterate over every variable in the given CTF dict, in arbitrary order,
   returning the name and type of each variable in turn.  The name argument is
   not optional.  Returns CTF_ERR on end of iteration or error.  */

ctf_id_t
ctf_variable_next (ctf_dict_t *fp, ctf_next_t **it, const char **name)
{
  ctf_next_t *i = *it;
  ctf_id_t id;
  ctf_error_t err;

  /* (No need for a LCTF_NO_STR check: checking for a missing parent covers more
     cases, and we need to do that anyway.)  */

  if ((fp->ctf_flags & LCTF_CHILD) && (fp->ctf_parent == NULL))
    return (ctf_set_typed_errno (fp, ECTF_NOPARENT));

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_typed_errno (fp, ENOMEM);

      i->cu.ctn_fp = fp;
      i->ctn_iter_fun = (void (*) (void)) ctf_variable_next;
      i->ctn_next = NULL;
      *it = i;
    }

  if ((void (*) (void)) ctf_variable_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (fp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  if ((id = ctf_type_kind_next (fp, &i->ctn_next, CTF_K_VAR)) != CTF_ERR)
    {
      if (name)
	*name = ctf_type_name_raw (fp, id);
      return id;
    }
  err = ctf_errno (fp);
  /* Fall through.  */
end:
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_typed_errno (fp, err);
}

/* Iterate over every variable in the given CTF datasec, in arbitrary order,
   returning the name and type of each variable in turn.  Returns CTF_ERR on end
   of iteration or error.

   (The order is arbitrary so we don't need to worry about sorting unsorted
   datasecs.)  */

ctf_id_t
ctf_datasec_var_next (ctf_dict_t *fp, ctf_id_t datasec, ctf_next_t **it,
		      size_t *size, size_t *offset)
{
  ctf_next_t *i = *it;
  ctf_id_t type;
  ctf_error_t err;

  if (!i)
    {
      const ctf_type_t *tp;
      unsigned char *vlen;
      ctf_dict_t *ofp = fp;

      if ((datasec = ctf_type_resolve_unsliced (fp, datasec)) == CTF_ERR)
	return CTF_ERR;			/* errno is set for us.  */

      if (ctf_type_kind (fp, datasec) != CTF_K_DATASEC)
	return (ctf_set_typed_errno (ofp, ECTF_NOTDATASEC));

      if ((tp = ctf_lookup_by_id (&fp, datasec, NULL)) == NULL)
	return CTF_ERR;			/* errno is set for us.  */

      if ((i = ctf_next_create ()) == NULL)
	return (ctf_set_typed_errno (ofp, ENOMEM));

      i->cu.ctn_fp = ofp;
      i->ctn_iter_fun = (void (*) (void)) ctf_datasec_var_next;
      vlen = ctf_vlen (fp, datasec, tp, &i->ctn_n);

      i->u.ctn_datasec = (const ctf_var_secinfo_t *) vlen;

      *it = i;
    }

  if ((void (*) (void)) ctf_datasec_var_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (fp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  if (i->ctn_n == 0)
    {
      err = ECTF_NEXT_END;
      goto end;
    }

  if (size)
    *size = i->u.ctn_datasec->cvs_size;
  if (offset)
    *offset = i->u.ctn_datasec->cvs_offset;
  type = i->u.ctn_datasec->cvs_type;

  i->u.ctn_datasec++;
  i->ctn_n--;

  return type;

 end:
  ctf_next_destroy (i);
  *it = NULL;
  return (ctf_set_typed_errno (fp, err));
}

/* Iterate over all tags with the given TAG, returning the ID of each tag.  */

ctf_id_t
ctf_tag_next (ctf_dict_t *fp, const char *tag, ctf_next_t **it)
{
  ctf_next_t *i = *it;
  ctf_error_t err;
  void *type;

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	return (ctf_set_typed_errno (fp, ENOMEM));
      i->cu.ctn_fp = fp;
      i->ctn_iter_fun = (void (*) (void)) ctf_tag_next;

      i->cu.ctn_s = ctf_dynhash_lookup (fp->ctf_tags, tag);

      *it = i;
    }

  if ((void (*) (void)) ctf_tag_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (fp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  err = ctf_dynset_next (i->cu.ctn_s, &i->ctn_next, &type);

  if (err == 0)
    return (ctf_id_t) (uintptr_t) type;

 end:
  ctf_next_destroy (i);
  *it = NULL;
  return (ctf_set_typed_errno (fp, err));
}

/* Follow a given type through the graph for TYPEDEF, VOLATILE, CONST, and
   RESTRICT nodes until we reach a "base" type node.  This is useful when
   we want to follow a type ID to a node that has members or a size.  To guard
   against infinite loops, we implement simplified cycle detection and check
   each link against itself, the previous node, and the topmost node.

   Does not drill down through slices to their contained type.

   Callers of this function must not presume that a type it returns must have a
   valid ctt_size: forwards do not, and must be separately handled.  */

ctf_id_t
ctf_type_resolve (ctf_dict_t *fp, ctf_id_t type)
{
  return ctf_type_resolve_nonrepresentable (fp, type, 0);
}

/* As with ctf_type_resolve(), but optionally do not consider type 0
   to be ECTF_NONREPRESENTABLE.  Internal only.  */
ctf_id_t
ctf_type_resolve_nonrepresentable (ctf_dict_t *fp, ctf_id_t type, int allow_zero)
{
  ctf_id_t prev = type, otype = type;
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;

  if (type == 0)
    {
      if (allow_zero)
	return 0;
      else
	return (ctf_set_typed_errno (ofp, ECTF_NONREPRESENTABLE));
    }

  while ((tp = ctf_lookup_by_id (&fp, type, &suffix)) != NULL)
    {
      switch (LCTF_KIND (fp, tp))
	{
	case CTF_K_TYPEDEF:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	case CTF_K_VAR:
	  if (suffix->ctt_type == type || suffix->ctt_type == otype
	      || suffix->ctt_type == prev)
	    {
	      ctf_err_warn (ofp, 0, ECTF_CORRUPT, _("type %lx cycle detected"),
			    otype);
	      return (ctf_set_typed_errno (ofp, ECTF_CORRUPT));
	    }
	  prev = type;
	  type = suffix->ctt_type;
	  break;
	case CTF_K_UNKNOWN:
	  return (ctf_set_typed_errno (ofp, ECTF_NONREPRESENTABLE));
	default:
	  return type;
	}
      if (type == 0)
	{
	  if (allow_zero)
	    return 0;
	  else
	    return (ctf_set_typed_errno (ofp, ECTF_NONREPRESENTABLE));
	}
    }

  return CTF_ERR;		/* errno is set for us.  */
}

/* Like ctf_type_resolve(), but traverse down through slices to their contained
   type.  */

ctf_id_t
ctf_type_resolve_unsliced (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  ctf_id_t resolved_type;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return CTF_ERR;

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */
  resolved_type = type;

  do
    {
      type = resolved_type;

      if ((LCTF_KIND (fp, tp)) == CTF_K_SLICE)
	if ((type = ctf_type_reference (fp, type)) == CTF_ERR)
	  return (ctf_set_typed_errno (ofp, ctf_errno (fp)));

      if ((resolved_type = ctf_type_resolve (fp, type)) == CTF_ERR)
	return CTF_ERR;

      if ((tp = ctf_lookup_by_id (&fp, resolved_type, NULL)) == NULL)
	return CTF_ERR;		/* errno is set for us.  */
    }
  while (LCTF_KIND (fp, tp) == CTF_K_SLICE);

  return type;
}

/* Return the native dict of a given type: if called on a child and the
   type is in the parent, return the parent.  Needed if you plan to access
   the type directly, without using the API.  */
ctf_dict_t *
ctf_get_dict (const ctf_dict_t *fp, ctf_id_t type)
{
  if ((fp->ctf_flags & LCTF_CHILD) && ctf_type_isparent (fp, type))
    return fp->ctf_parent;

  return (ctf_dict_t *) fp;
}

/* Look up a name in the given name table, in the appropriate hash given the
   kind of the identifier.  The name is a raw, undecorated identifier.  */

ctf_id_t ctf_lookup_by_rawname (ctf_dict_t *fp, int kind, const char *name)
{
  return (ctf_id_t) (uintptr_t)
    ctf_dynhash_lookup (ctf_name_table (fp, kind), name);
}

/* Retrieve raw type data.  */

const ctf_type_t *
ctf_type_data (ctf_dict_t *fp, ctf_id_t type, int prefix)
{
  const ctf_type_t *tp, *suffix, *big;

  if (fp->ctf_flags & LCTF_NO_STR)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return NULL;				/* errno is set for us.  */

  if (!prefix && ((big = ctf_find_prefix (fp, tp, CTF_K_BIG)) != NULL)
      && ((CTF_INFO_VLEN (big->ctt_info) != 0) || (big->ctt_size != 0)))
    {
      ctf_set_errno (fp, ECTF_TOOLARGE);
      return NULL;
    }

  if (prefix)
    return tp;
  else
    return suffix;
}

/* Lookup the given type ID and return its name as a new dynamically-allocated
   string.  */

char *
ctf_type_aname (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_decl_t cd;
  ctf_decl_node_t *cdp;
  ctf_decl_prec_t prec, lp, rp;
  int ptr, arr;
  uint32_t k;
  char *buf;

  if (fp == NULL && type == CTF_ERR)
    return NULL;	/* Simplify caller code by permitting CTF_ERR.  */

  if (fp->ctf_flags & LCTF_NO_STR)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  ctf_decl_init (&cd);
  ctf_decl_push (&cd, fp, type);

  if (cd.cd_err != 0)
    {
      ctf_decl_fini (&cd);
      ctf_set_errno (fp, cd.cd_err);
      return NULL;
    }

  /* If the type graph's order conflicts with lexical precedence order
     for pointers or arrays, then we need to surround the declarations at
     the corresponding lexical precedence with parentheses.  This can
     result in either a parenthesized pointer (*) as in int (*)() or
     int (*)[], or in a parenthesized pointer and array as in int (*[])().  */

  ptr = cd.cd_order[CTF_PREC_POINTER] > CTF_PREC_POINTER;
  arr = cd.cd_order[CTF_PREC_ARRAY] > CTF_PREC_ARRAY;

  rp = arr ? CTF_PREC_ARRAY : ptr ? CTF_PREC_POINTER : -1;
  lp = ptr ? CTF_PREC_POINTER : arr ? CTF_PREC_ARRAY : -1;

  k = CTF_K_POINTER;		/* Avoid leading whitespace (see below).  */

  for (prec = CTF_PREC_BASE; prec < CTF_PREC_MAX; prec++)
    {
      for (cdp = ctf_list_next (&cd.cd_nodes[prec]);
	   cdp != NULL; cdp = ctf_list_next (cdp))
	{
	  ctf_dict_t *rfp = fp;
	  const ctf_type_t *tp = NULL;
	  const char *name;

	  ctf_lookup_by_id (&rfp, cdp->cd_type, &tp);
	  assert (tp);
	  name = ctf_strptr (rfp, tp->ctt_name);

	  if (k != CTF_K_POINTER && k != CTF_K_ARRAY)
	    ctf_decl_sprintf (&cd, " ");

	  if (lp == prec)
	    {
	      ctf_decl_sprintf (&cd, "(");
	      lp = -1;
	    }

	  switch (cdp->cd_kind)
	    {
	    case CTF_K_INTEGER:
	    case CTF_K_FLOAT:
	    case CTF_K_TYPEDEF:
	    case CTF_K_BTF_FLOAT:
	    case CTF_K_DATASEC:
	      /* Integers, floats, typedefs, and datasecs must always be named
		 types.  */

	      if (name[0] == '\0')
		{
		  ctf_set_errno (fp, ECTF_CORRUPT);
		  ctf_decl_fini (&cd);
		  return NULL;
		}

	      if (cdp->cd_kind != CTF_K_DATASEC)
		ctf_decl_sprintf (&cd, "%s", name);
	      else
		ctf_decl_sprintf (&cd, "DATASEC (\"%s\", %i)", name,
				  LCTF_VLEN (rfp, tp));
	      break;
	    case CTF_K_POINTER:
	      ctf_decl_sprintf (&cd, "*");
	      break;
	    case CTF_K_ARRAY:
	      ctf_decl_sprintf (&cd, "[%u]", cdp->cd_n);
	      break;
	    case CTF_K_FUNCTION:
	      {
		size_t i;
		ctf_funcinfo_t fi;
		ctf_id_t *argv = NULL;
		const char **arg_names = NULL;

		if (ctf_func_type_info (rfp, cdp->cd_type, &fi) < 0)
		  goto err;		/* errno is set for us.  */

		if ((argv = calloc (fi.ctc_argc, sizeof (ctf_id_t *))) == NULL)
		  {
		    ctf_set_errno (rfp, errno);
		    goto err;
		  }

		if ((arg_names = calloc (fi.ctc_argc, sizeof (const char *))) == NULL)
		  {
		    ctf_set_errno (rfp, errno);
		    goto err;
		  }

		if (ctf_func_type_args (rfp, cdp->cd_type,
					fi.ctc_argc, argv) < 0)
		  goto err;		/* errno is set for us.  */

		if (ctf_func_type_arg_names (rfp, cdp->cd_type,
					     fi.ctc_argc, arg_names) < 0)
		  goto err;		/* errno is set for us.  */

		ctf_decl_sprintf (&cd, "(*) (");
		for (i = 0; i < fi.ctc_argc; i++)
		  {
		    char *arg = ctf_type_aname (rfp, argv[i]);

		    if (arg == NULL)
		      goto err;		/* errno is set for us.  */
		    ctf_decl_sprintf (&cd, "%s%s%s", arg, arg_names[i] != 0 ?
				      " ":"", arg_names[i] != 0
				      ? arg_names[i] : "");
		    free (arg);

		    if ((i < fi.ctc_argc - 1)
			|| (fi.ctc_flags & CTF_FUNC_VARARG))
		      ctf_decl_sprintf (&cd, ", ");
		  }

		if (fi.ctc_flags & CTF_FUNC_VARARG)
		  ctf_decl_sprintf (&cd, "...");
		ctf_decl_sprintf (&cd, ")");

		free (argv);
		break;

	      err:
		ctf_set_errno (fp, ctf_errno (rfp));
		free (argv);
		free (arg_names);
		ctf_decl_fini (&cd);
		return NULL;
	      }
	      break;
	    case CTF_K_STRUCT:
	      ctf_decl_sprintf (&cd, "struct %s", name);
	      break;
	    case CTF_K_UNION:
	      ctf_decl_sprintf (&cd, "union %s", name);
	      break;
	    case CTF_K_ENUM:
	    case CTF_K_ENUM64:
	      ctf_decl_sprintf (&cd, "enum %s", name);
	      break;
	    case CTF_K_TYPE_TAG:
	      ctf_decl_sprintf (&cd, "%s", name);
	      break;
	    /* UPTODO: decl tags... I guess we print them when we print the
	       associated variable, somehow?  For now, just this...  */
	    case CTF_K_DECL_TAG:
	      ctf_decl_sprintf (&cd, "btf_decl_tag (\"%s\")", name);
	      break;
	    case CTF_K_FUNC_LINKAGE:
	    case CTF_K_VAR:
	      {
		int linkage;
		if ((linkage = ctf_type_linkage (fp, cdp->cd_type)) < 0)
		  {
		    ctf_set_errno (fp, ECTF_CORRUPT);
		    ctf_decl_fini (&cd);
		    return NULL;
		  }

		ctf_decl_sprintf (&cd, "%s%s", linkage == 0 ? "static "
				  : (linkage == 2 ? "extern " :
				     (linkage == 1 ? "" : "(invalid linkage) ")),
				  name);
		break;
	      }
	    case CTF_K_FORWARD:
	      {
		switch (ctf_type_kind_forwarded (fp, cdp->cd_type))
		  {
		  case CTF_K_STRUCT:
		    ctf_decl_sprintf (&cd, "struct %s", name);
		    break;
		  case CTF_K_UNION:
		    ctf_decl_sprintf (&cd, "union %s", name);
		    break;
		  case CTF_K_ENUM:
		  case CTF_K_ENUM64:
		    ctf_decl_sprintf (&cd, "enum %s", name);
		    break;
		  default:
		    ctf_set_errno (fp, ECTF_CORRUPT);
		    ctf_decl_fini (&cd);
		    return NULL;
		  }
		break;
	      }
	    case CTF_K_VOLATILE:
	      ctf_decl_sprintf (&cd, "volatile");
	      break;
	    case CTF_K_CONST:
	      ctf_decl_sprintf (&cd, "const");
	      break;
	    case CTF_K_RESTRICT:
	      ctf_decl_sprintf (&cd, "restrict");
	      break;

	    case CTF_K_UNKNOWN:
	      if (name[0] == '\0')
		ctf_decl_sprintf (&cd, _("(nonrepresentable type)"));
	      else
		ctf_decl_sprintf (&cd, _("(nonrepresentable type %s)"),
				  name);
	      break;
	    }

	  k = cdp->cd_kind;
	}

      if (rp == prec)
	ctf_decl_sprintf (&cd, ")");
    }

  if (cd.cd_enomem)
    (void) ctf_set_errno (fp, ENOMEM);

  buf = ctf_decl_buf (&cd);

  ctf_decl_fini (&cd);
  return buf;
}

/* Lookup the given type ID and print a string name for it into buf.  If buf
   is too small, return NULL: the ECTF_NAMELEN error is set on 'fp' for us.
   len is reset to the actual length of the string in this case.  */

char *
ctf_type_sname (ctf_dict_t *fp, ctf_id_t type, char *buf, size_t *len)
{
  char *str = ctf_type_aname (fp, type);
  size_t slen;

  if (str == NULL || len == NULL)
    {
      free (str);
      return NULL;		/* errno is set for us.  */
    }

  slen = strlen (str);

  if (slen >= *len)
    {
      free (str);
      ctf_set_errno (fp, ECTF_NAMELEN);
      *len = slen;
      return NULL;
    }

  snprintf (buf, *len, "%s", str);
  free (str);
  return buf;
}

/* Lookup the given type ID and return its raw, unadorned, undecorated name.
   The name will live as long as its ctf_dict_t does.

   The only decoration is that a NULL return always means an error: nameless
   types return a null string.  */

const char *
ctf_type_name_raw (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;

  if (fp->ctf_flags & LCTF_NO_STR)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  if (ctf_lookup_by_id (&fp, type, &tp) == NULL)
    return NULL;		/* errno is set for us.  */

  if (tp->ctt_name == 0)
    return "";

  return ctf_strraw (fp, tp->ctt_name);
}

/* Resolve the type down to a base type node, and then return the size
   of the type storage in bytes.  */

ssize_t
ctf_type_size (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  ssize_t size;
  ctf_arinfo_t ar;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  switch (LCTF_KIND (fp, tp))
    {
    case CTF_K_POINTER:
      return fp->ctf_dmodel->ctd_pointer;

    case CTF_K_FUNCTION:
    case CTF_K_FUNC_LINKAGE:
      return 0;			/* Function size is only known by symtab.  */

    case CTF_K_ARRAY:
      /* ctf_add_array() does not directly encode the element size, but
	 requires the user to multiply to determine the element size.

	 If ctf_get_ctt_size() returns nonzero, then use the recorded
	 size instead.  */

      if ((size = ctf_get_ctt_size (fp, tp, NULL, NULL)) > 0)
	return size;

      if (ctf_array_info (ofp, type, &ar) < 0
	  || (size = ctf_type_size (ofp, ar.ctr_contents)) < 0)
	return -1;		/* errno is set for us.  */

      return size * ar.ctr_nelems;

    case CTF_K_FORWARD:
      /* Forwards do not have a meaningful size.  */
      return (ctf_set_errno (ofp, ECTF_INCOMPLETE));

    default: /* including enums of all kinds, slices, etc */
      return (ctf_get_ctt_size (fp, tp, NULL, NULL));
    }
}

/* Determine the natural alignment (in bits) for some type, given the previous
   TYPE at BIT_OFFSET.

   Not public because doing this entirely right requires arch-dependent
   attention: this is just to reduce code repetition in ctf-create.c.

   Errors if the TYPE or PREV_TYPE are unsuitable for automatic alignment
   determination: in particular, you can insert incomplete or nonrepresentable
   TYPEs, but PREV_TYPE cannot be incomplete or nonrepresentable.  */

ssize_t
ctf_type_align_natural (ctf_dict_t *fp, ctf_id_t prev_type,
			ctf_id_t type, ssize_t bit_offset)
{
  ctf_encoding_t info;
  ssize_t size;
  ssize_t align;

  if ((prev_type = ctf_type_resolve (fp, prev_type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((align = ctf_type_align (fp, type)) < 0)
    {
      /* Ignore incompleteness and nonrepresentability of the type we're
	 inserting: just assume such a type has no alignment constraints of its
	 own.  */
      if (ctf_errno (fp) == ECTF_NONREPRESENTABLE
	  || ctf_errno (fp) == ECTF_INCOMPLETE)
	align = 0;
      else
	return -1;		/* errno is set for us.  */
    }

  if (ctf_type_encoding (fp, prev_type, &info) == 0)
    bit_offset += info.cte_bits;
  else if ((size = ctf_type_size (fp, prev_type)) > 0)
    bit_offset += size * CHAR_BIT;
  else if (size < 0)
    return -1;			/* errno is set for us.  */

  /* Round up the offset of the end of the last member to the next byte
     boundary, convert 'off' to bytes, and then round it up again to the next
     multiple of the alignment required by the new member.  Finally, convert
     back to bits and store the result.  Technically we could do more efficient
     packing within structs if the new member is a bit-field, but we're the
     "compiler" and the Standard says we can do as we choose.  */

  bit_offset = roundup (bit_offset, CHAR_BIT) / CHAR_BIT;
  bit_offset = roundup (bit_offset, MAX (align, 1));
  bit_offset *= CHAR_BIT;

  return bit_offset;
}

/* Resolve the type down to a base type node, and then return the alignment
   needed for the type storage in bytes.

   TODO may need arch-dependent attention.  */

ssize_t
ctf_type_align (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;
  ctf_dict_t *ofp = fp;
  int kind;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_KIND (fp, tp);
  switch (kind)
    {
    case CTF_K_POINTER:
    case CTF_K_FUNCTION:
    case CTF_K_FUNC_LINKAGE:
      return fp->ctf_dmodel->ctd_pointer;

    case CTF_K_ARRAY:
      {
	ctf_arinfo_t r;
	if (ctf_array_info (ofp, type, &r) < 0)
	  return -1;		/* errno is set for us.  */
	return (ctf_type_align (ofp, r.ctr_contents));
      }

    case CTF_K_STRUCT:
    case CTF_K_UNION:
      {
	size_t align = 0;
	unsigned char *vlen;
	uint32_t i = 0;
	size_t n;

	vlen = ctf_vlen (fp, type, tp, &n);

	if (kind == CTF_K_STRUCT)
	  n = MIN (n, 1);	/* Only use first member for structs.  */

	for (; n != 0; n--, i++)
	  {
	    ctf_member_t *memb = (ctf_member_t *) vlen;

	    ssize_t am = ctf_type_align (ofp, memb[i].ctm_type);
	    align = MAX (align, (size_t) am);
	  }
	return align;
      }

    case CTF_K_FORWARD:
      /* Forwards do not have a meaningful alignment.  */
      return (ctf_set_errno (ofp, ECTF_INCOMPLETE));

    default:  /* including enums of all kinds, slices, etc */
      return (ctf_get_ctt_size (fp, tp, NULL, NULL));
    }
}

/* Return the kind (CTF_K_* constant) for the specified type pointer.
   Internal use only.

   Pretend that all forwards are of type CTF_K_FORWARD, for ease of
   use and compatibility.  */

int
ctf_type_kind_unsliced_tp (ctf_dict_t *fp, const ctf_type_t *tp)
{
  if (LCTF_KIND (fp, tp) == CTF_K_ENUM
      && LCTF_VLEN (fp, tp) == 0)
    return CTF_K_FORWARD;

  return (LCTF_KIND (fp, tp));
}

/* Return the kind (CTF_K_* constant) for the specified type pointer.
   Slices are considered to be of the same kind as the type sliced.  */

int
ctf_type_kind_tp (ctf_dict_t *fp, const ctf_type_t *tp)
{
  int kind;

  if ((kind = ctf_type_kind_unsliced_tp (fp, tp)) < 0)
    return -1;			/* errno is set for us.  */

  if (kind == CTF_K_SLICE)
    {
      const ctf_slice_t *sp;
      ssize_t increment;

      ctf_get_ctt_size (fp, tp, NULL, &increment);
      sp = (const ctf_slice_t *) tp + increment;

      kind = ctf_type_kind_unsliced (fp, sp->cts_type);
    }

  return kind;
}

/* Return the kind of this type pointer, except, for forwards, return the kind
   of thing this is a forward to.  */
int
ctf_type_kind_forwarded_tp (ctf_dict_t *fp, const ctf_type_t *tp)
{
  int kind;

  if ((kind = ctf_type_kind_tp (fp, tp)) < 0)
    return -1;			/* errno is set for us.  */

  /* Covers forwards to enums, too.  */
  if (kind != CTF_K_FORWARD)
    return kind;

  while (LCTF_IS_PREFIXED_INFO (tp->ctt_info))
    tp++;

  if (CTF_INFO_KFLAG (tp->ctt_info))
    return CTF_K_UNION;
  else
    return CTF_K_STRUCT;
}

/* Return the kind (CTF_K_* constant) for the specified type ID.

   Pretend that all forwards are of type CTF_K_FORWARD, for ease of
   use and compatibility.  */

int
ctf_type_kind_unsliced (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  return (ctf_type_kind_unsliced_tp (fp, tp));
}

/* Return the kind (CTF_K_* constant) for the specified type ID.
   Slices are considered to be of the same kind as the type sliced.  */

int
ctf_type_kind (ctf_dict_t *fp, ctf_id_t type)
{
  int kind;

  if ((kind = ctf_type_kind_unsliced (fp, type)) < 0)
    return -1;

  if (kind == CTF_K_SLICE)
    {
      if ((type = ctf_type_reference (fp, type)) == CTF_ERR)
	return -1;		/* errno is set for us.  */
      kind = ctf_type_kind_unsliced (fp, type);
    }

  return kind;
}

/* Return the kind of this type, except, for forwards, return the kind of thing
   this is a forward to.  */
int
ctf_type_kind_forwarded (ctf_dict_t *fp, ctf_id_t type)
{
  int ret;
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return -1;			/* errno is set for us.  */

  if ((ret = ctf_type_kind_forwarded_tp (fp, tp)) < 0)
    return (ctf_set_errno (ofp, ctf_errno (fp)));

  return ret;
}

/* Return nonzero if this type is conflicting, nonzero if it's not, < 0 on
   error; if CUNAME is set, set it to the name of the conflicting compilation
   unit for the passed-in type (which may be a null string if the cuname is not
   known).  */

ctf_bool_t
ctf_type_conflicting (ctf_dict_t *fp, ctf_id_t type, const char **cuname)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;

  /* The unimplemented / void type is never conflicting.  */
  if (type == 0)
    return 0;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  if (cuname)
    *cuname = 0;

  if (LCTF_INFO_ISROOT (fp, tp->ctt_info))
    return 0;

  if (!cuname)
    return 1;

  while (LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info) != CTF_K_CONFLICTING
	 && LCTF_IS_PREFIXED_INFO (tp->ctt_info))
    tp++;

  /* We already checked that this is a non-root-visible type, so this must be
     CTF_K_CONFLICTING.  */
  if (!ctf_assert (ofp, LCTF_IS_PREFIXED_INFO (tp->ctt_info)))
    return -1;					/* errno is set for us.  */

  *cuname = ctf_strptr (fp, tp->ctt_name);
  return 1;
}

/* If the type is one that directly references another type (such as POINTER),
   then return the ID of the type to which it refers.  */

ctf_id_t
ctf_type_reference (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  switch (LCTF_KIND (fp, tp))
    {
    case CTF_K_POINTER:
    case CTF_K_TYPEDEF:
    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
    case CTF_K_TYPE_TAG:
    case CTF_K_DECL_TAG:
    case CTF_K_FUNCTION:
    case CTF_K_FUNC_LINKAGE:
    case CTF_K_VAR:
      return suffix->ctt_type;
      /* Slices store their type in an unusual place.  */
    case CTF_K_SLICE:
      {
	unsigned char *vlen;
	const ctf_slice_t *sp;

	vlen = ctf_vlen (fp, type, tp, NULL);
	sp = (const ctf_slice_t *) vlen;
	return sp->cts_type;
      }
    default:
      return (ctf_set_typed_errno (ofp, ECTF_NOTREF));
    }
}

/* Return the component ID and declaration to which a decl tag is attached.  */

ctf_id_t
ctf_decl_tag (ctf_dict_t *fp, ctf_id_t decl_tag, int64_t *component_idx)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;
  unsigned char *vlen;
  ctf_decl_tag_t *cdt;

  if ((tp = ctf_lookup_by_id (&fp, decl_tag, &suffix)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if (LCTF_KIND (fp, tp) != CTF_K_DECL_TAG)
    return (ctf_set_typed_errno (ofp, ECTF_NOTDECLTAG));

  vlen = ctf_vlen (fp, decl_tag, tp, NULL);
  cdt = (ctf_decl_tag_t *) vlen;

  *component_idx = cdt->cdt_component_idx;

  return suffix->ctt_type;
}

/* Return the type ID of the type to which a given type tag is attached, or of
   the type of the declaration to which a decl tag is attached (so a decl tag on
   a function parameter would return the type ID of the parameter's type).  */

ctf_id_t
ctf_tag (ctf_dict_t *fp, ctf_id_t tag)
{
  int kind = ctf_type_kind (fp, tag);
  int64_t component_idx;
  ctf_id_t ref;

  if (kind != CTF_K_TYPE_TAG && kind != CTF_K_DECL_TAG)
    return (ctf_set_typed_errno (fp, ECTF_NOTTAG));

  if ((ref = ctf_type_reference (fp, tag)) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  if (kind == CTF_K_TYPE_TAG)
    return ref;

  if (ctf_decl_tag (fp, tag, &component_idx) == CTF_ERR)
    return CTF_ERR;		/* errno is set for us.  */

  if (component_idx == -1)
    return ref;

  /* See ctf_add_tag.  */

  switch (ctf_type_kind (fp, ref))
    {
    case CTF_K_STRUCT:
    case CTF_K_UNION:
      {
	ctf_next_t *i = NULL;
	int64_t j = 0;
	ctf_id_t type;

	while (ctf_member_next (fp, ref, &i, NULL, &type, NULL, 0) >= 0)
	  {
	    if (j++ == component_idx)
	      {
		ctf_next_destroy (i);
		return type;
	      }
	  }
	if (ctf_errno (fp) != ECTF_NEXT_END)
	  return CTF_ERR;	/* errno is set for us.  */
      }
      break;
    case CTF_K_FUNC_LINKAGE:
    case CTF_K_FUNCTION:
      {
	ctf_funcinfo_t fi;
	ctf_id_t *args;
	ctf_id_t argtype;

	if (ctf_func_type_info (fp, ref, &fi) < 0)
	  return CTF_ERR;	/* errno is set for us.  */

	if (component_idx + 1 > (ssize_t) fi.ctc_argc)
	  break;

	if ((args = malloc ((component_idx + 1) * sizeof (ctf_id_t))) == NULL)
	  return (ctf_set_typed_errno (fp, ENOMEM));

	if (ctf_func_type_args (fp, ref, component_idx + 1, args))
	  {
	    free (args);
	    return CTF_ERR;	/* errno is set for us. */
	  }
	argtype = args[component_idx];
	free (args);
	return argtype;
      }

    default:
      return CTF_ERR;		/* errno is set for us.  */
    }

  ctf_err_warn (fp, 0, ECTF_NOTREF, _("decl tag %lx refers to type %lx, "
				      "component %" PRIi64 ", which does not exist"),
		tag, (long) ref, component_idx);

  return (ctf_set_typed_errno (fp, ECTF_NOTREF));
}

/* Find a pointer to type by looking in fp->ctf_ptrtab and fp->ctf_pptrtab.  If
   we can't find a pointer to the given type, see if we can compute a pointer to
   the type resulting from resolving the type down to its base type and use that
   instead.  This helps with cases where the CTF data includes "struct foo *"
   but not "foo_t *" and the user accesses "foo_t *" in the debugger.  */

ctf_id_t
ctf_type_pointer (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  ctf_id_t ntype;
  uint32_t idx;

  if (ctf_lookup_by_id (&fp, type, NULL) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  idx = ctf_type_to_index (fp, type);
  if ((ntype = fp->ctf_ptrtab[idx]) != 0)
    return (ctf_index_to_type (fp, ntype));

  if (idx < ofp->ctf_pptrtab_len
      && (ntype = ofp->ctf_pptrtab[idx]) != 0)
    return (ctf_index_to_type (fp, ntype));

  /* Try again after resolution.  */
  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return (ctf_set_typed_errno (ofp, ECTF_NOTYPE));

  if (ctf_lookup_by_id (&fp, type, NULL) == NULL)
    return (ctf_set_typed_errno (ofp, ECTF_NOTYPE));

  idx = ctf_type_to_index (fp, type);
  if ((ntype = fp->ctf_ptrtab[idx]) != 0)
    return (ctf_index_to_type (fp, ntype));

  if (idx < ofp->ctf_pptrtab_len
      && (ntype = ofp->ctf_pptrtab[idx]) != 0)
    return (ctf_index_to_type (fp, ntype));

  return (ctf_set_typed_errno (ofp, ECTF_NOTYPE));
}

/* Return the encoding for the specified INTEGER, FLOAT, or ENUM.  */

ctf_ret_t
ctf_type_encoding (ctf_dict_t *fp, ctf_id_t type, ctf_encoding_t *ep)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;
  const unsigned char *vlen;
  uint32_t data;

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return -1;			/* errno is set for us.  */

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return (ctf_set_typed_errno (ofp, ECTF_NOTYPE));

  vlen = ctf_vlen (fp, type, tp, NULL);

  switch (LCTF_KIND (fp, tp))
    {
    case CTF_K_INTEGER:
      data = *(const uint32_t *) vlen;
      ep->cte_format = CTF_INT_ENCODING (data);
      ep->cte_offset = CTF_INT_OFFSET (data);
      ep->cte_bits = CTF_INT_BITS (data);
      break;
    case CTF_K_FLOAT:
      data = *(const uint32_t *) vlen;
      ep->cte_format = CTF_FP_ENCODING (data);
      ep->cte_offset = CTF_FP_OFFSET (data);
      ep->cte_bits = CTF_FP_BITS (data);
      break;
    case CTF_K_ENUM:
    case CTF_K_ENUM64:
      ep->cte_format = CTF_INFO_KFLAG (suffix->ctt_info) ? CTF_INT_SIGNED : 0;
      ep->cte_offset = 0;
      ep->cte_bits = 0;
      break;
    case CTF_K_BTF_FLOAT:
      ep->cte_format = 0;
      ep->cte_offset = 0;
      ep->cte_bits = suffix->ctt_size * CHAR_BIT;
      break;
    case CTF_K_SLICE:
      {
	const ctf_slice_t *slice;
	ctf_encoding_t underlying_en;
	ctf_id_t underlying;

	slice = (ctf_slice_t *) vlen;
	if ((underlying = ctf_type_resolve (ofp, slice->cts_type)) == CTF_ERR)
	  return -1;

	if (ctf_type_encoding (ofp, underlying, &underlying_en) < 0)
	  return -1;				/* errno is set for us.  */

	ep->cte_format = underlying_en.cte_format;
	ep->cte_offset = slice->cts_offset;
	ep->cte_bits = slice->cts_bits;
	break;
      }
    default:
      return (ctf_set_errno (ofp, ECTF_NOTINTFP));
    }

  return 0;
}

int
ctf_type_cmp (ctf_dict_t *lfp, ctf_id_t ltype, ctf_dict_t *rfp,
	      ctf_id_t rtype)
{
  int rval;

  if (ltype < rtype)
    rval = -1;
  else if (ltype > rtype)
    rval = 1;
  else
    rval = 0;

  if (lfp == rfp)
    return rval;

  if (lfp->ctf_parent != NULL)
    lfp = ctf_get_dict (lfp, ltype);

  if (rfp->ctf_parent != NULL)
    rfp = ctf_get_dict (rfp, rtype);

  if (lfp < rfp)
    return -1;

  if (lfp > rfp)
    return 1;

  return rval;
}

/* Return a boolean value indicating if two types are compatible.  This function
   returns true if the two types are the same, or if they (or their ultimate
   base type) have the same encoding properties, or (for structs / unions /
   enums / forward declarations) if they have the same name and (for structs /
   unions) member count.  */

ctf_bool_t
ctf_type_compat (ctf_dict_t *lfp, ctf_id_t ltype,
		 ctf_dict_t *rfp, ctf_id_t rtype)
{
  const ctf_type_t *ltp, *rtp;
  ctf_encoding_t le, re;
  ctf_arinfo_t la, ra;
  int lkind, rkind;
  int same_names = 0;

  if (lfp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (lfp, ECTF_NOPARENT));

  if (rfp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (rfp, ECTF_NOPARENT));

  if (ctf_type_isparent (lfp, ltype) && !lfp->ctf_parent)
    return (ctf_set_errno (rfp, ECTF_NOPARENT));

  if (ctf_type_isparent (rfp, rtype) && !rfp->ctf_parent)
    return (ctf_set_errno (rfp, ECTF_NOPARENT));

  if (ctf_type_cmp (lfp, ltype, rfp, rtype) == 0)
    return 1;

  ltype = ctf_type_resolve (lfp, ltype);
  rtype = ctf_type_resolve (rfp, rtype);

  if (ltype == CTF_ERR || rtype == CTF_ERR)
    return -1;					/* errno is set for us.  */

  lkind = ctf_type_kind (lfp, ltype);
  rkind = ctf_type_kind (rfp, rtype);

  if (lkind < 0 || rkind < 0)
    return -1;					/* errno is set for us.  */

  if (ctf_lookup_by_id (&lfp, ltype, &ltp) != NULL &&
      ctf_lookup_by_id (&rfp, rtype, &rtp) != NULL
      && ltp != NULL && rtp != NULL)
    same_names = (strcmp (ctf_strptr (lfp, ltp->ctt_name),
			  ctf_strptr (rfp, rtp->ctt_name)) == 0);

  if ((((lkind == CTF_K_ENUM) || (lkind == CTF_K_ENUM64)) && (rkind == CTF_K_INTEGER)) ||
      (((rkind == CTF_K_ENUM) || (lkind == CTF_K_ENUM64)) && (lkind == CTF_K_INTEGER)))
    return 1;

  if (lkind != rkind)
    return 0;

  switch (lkind)
    {
    case CTF_K_INTEGER:
    case CTF_K_FLOAT:
      memset (&le, 0, sizeof (le));
      memset (&re, 0, sizeof (re));
      return (ctf_type_encoding (lfp, ltype, &le) == 0
	      && ctf_type_encoding (rfp, rtype, &re) == 0
	      && memcmp (&le, &re, sizeof (ctf_encoding_t)) == 0);
    case CTF_K_POINTER:
      {
	ctf_id_t lref = ctf_type_reference (lfp, ltype);
	ctf_id_t rref = ctf_type_reference (rfp, rtype);

	/* void * is only compatible with itself.  */

	if (lref == 0 && rref == 0)
	  return 1;
	else
	  return (ctf_type_compat (lfp, lref, rfp, rref));
      }
    case CTF_K_ARRAY:
      return (ctf_array_info (lfp, ltype, &la) == 0
	      && ctf_array_info (rfp, rtype, &ra) == 0
	      && la.ctr_nelems == ra.ctr_nelems
	      && ctf_type_compat (lfp, la.ctr_contents, rfp, ra.ctr_contents)
	      && ctf_type_compat (lfp, la.ctr_index, rfp, ra.ctr_index));
    case CTF_K_STRUCT:
    case CTF_K_UNION:
      return (same_names && (ctf_type_size (lfp, ltype)
			     == ctf_type_size (rfp, rtype)));
    case CTF_K_ENUM64:
    case CTF_K_ENUM:
      {
	ctf_ret_t lencoded, rencoded;
	lencoded = ctf_type_encoding (lfp, ltype, &le);
	rencoded = ctf_type_encoding (rfp, rtype, &re);

	if ((lencoded != rencoded) ||
	    ((lencoded == 0) && memcmp (&le, &re, sizeof (ctf_encoding_t)) != 0))
	  return 0;
      }
      /* FALLTHRU */
    case CTF_K_FORWARD:
      return same_names;   /* No other checks required for these type kinds.  */
    default:
      return 0;		      /* Should not get here since we did a resolve.  */
    }
}

/* Return the number of members in a STRUCT or UNION, or the number of
   enumerators in an ENUM.  The count does not include unnamed sub-members.  */

ssize_t
ctf_member_count (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  uint32_t kind;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_KIND (fp, tp);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION && kind != CTF_K_ENUM)
    return (ctf_set_errno (ofp, ECTF_NOTSUE));

  return LCTF_VLEN (fp, tp);
}

/* Return the type and offset for a given member of a STRUCT or UNION.  */

ctf_ret_t
ctf_member_info (ctf_dict_t *fp, ctf_id_t type, const char *name,
		 ctf_membinfo_t *mip)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;
  int big = 0;
  size_t total_offset = 0;
  unsigned char *vlen;
  uint32_t kind, i = 0;
  size_t n;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_KIND (fp, tp);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
    return (ctf_set_errno (ofp, ECTF_NOTSOU));

  vlen = ctf_vlen (fp, type, tp, &n);

  big = (ctf_find_prefix (fp, tp, CTF_K_BIG) != NULL);

  for (; n != 0; n--, i++)
    {
      ctf_member_t *memb = (ctf_member_t *) vlen;
      const char *membname;
      ctf_id_t resolved;
      size_t offset;
      int bit_width = 0;

      if (CTF_INFO_KFLAG (suffix->ctt_info))
	{
	  offset = CTF_MEMBER_BIT_OFFSET (memb->ctm_offset);
	  bit_width = CTF_MEMBER_BIT_SIZE (memb->ctm_offset);
	}
      else
	offset = memb->ctm_offset;

      total_offset += offset;

      if ((resolved = ctf_type_resolve (fp, memb->ctm_type)) == CTF_ERR)
	{
	  if (ctf_errno (fp) != ECTF_NONREPRESENTABLE)
	    return (ctf_set_errno (ofp, ctf_errno (fp)));
	  resolved = memb->ctm_type;
	}

      /* Unnamed struct/union member.  */
      membname = ctf_strptr (fp, memb->ctm_name);
      if (membname[0] == 0
	  && (ctf_type_kind (fp, resolved) == CTF_K_STRUCT
	      || ctf_type_kind (fp, resolved) == CTF_K_UNION)
	  && (ctf_member_info (fp, resolved, name, mip) == 0))
	{
	  mip->ctm_type = resolved;

	  if (!big)
	    mip->ctm_offset += total_offset;
	  else
	    mip->ctm_offset += offset;

	  return 0;
	}

      /* Ordinary member.  */
      if (strcmp (membname, name) == 0)
	{
	  mip->ctm_type = memb->ctm_type;
	  if (big)
	    mip->ctm_offset = total_offset + memb->ctm_offset;
	  else
	    mip->ctm_offset = memb->ctm_offset;
	  mip->ctm_bit_width = bit_width;

	  return 0;
	}
    }

  return (ctf_set_errno (ofp, ECTF_NOMEMBNAM));
}

/* Return the array type, index, and size information for the specified ARRAY.  */

ctf_ret_t
ctf_array_info (ctf_dict_t *fp, ctf_id_t type, ctf_arinfo_t *arp)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  unsigned char *vlen;
  const ctf_array_t *ap;

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  if (LCTF_KIND (fp, tp) != CTF_K_ARRAY)
    return (ctf_set_errno (ofp, ECTF_NOTARRAY));

  vlen = ctf_vlen (fp, type, tp, NULL);
  ap = (const ctf_array_t *) vlen;

  arp->ctr_contents = ap->cta_contents;
  arp->ctr_index = ap->cta_index;
  arp->ctr_nelems = ap->cta_nelems;

  return 0;
}

/* Convert the specified value to the corresponding enum tag name, if a
   matching name can be found.  Otherwise NULL is returned.  */

const char *
ctf_enum_name (ctf_dict_t *fp, ctf_id_t type, int64_t value)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  unsigned char *vlen;
  int kind;
  size_t n;

  if (fp->ctf_flags & LCTF_NO_STR)
    {
      ctf_set_errno (fp, ECTF_NOPARENT);
      return NULL;
    }

  if ((type = ctf_type_resolve_unsliced (fp, type)) == CTF_ERR)
    return NULL;		/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return NULL;		/* errno is set for us.  */

  kind = LCTF_KIND (fp, tp);
  if (kind != CTF_K_ENUM && kind != CTF_K_ENUM64)
    {
      ctf_set_errno (ofp, ECTF_NOTENUM);
      return NULL;
    }

  vlen = ctf_vlen (fp, type, tp, &n);

  if (kind == CTF_K_ENUM)
    {
      const ctf_enum_t *ep = (const ctf_enum_t *) vlen;

      for (; n != 0; n--, ep++)
	{
	  if (ep->cte_value == value)
	    return (ctf_strptr (fp, ep->cte_name));
	}
    }
  else
    {
      const ctf_enum64_t *ep = (const ctf_enum64_t *) vlen;

      for (; n != 0; n--, ep++)
	{
	  int64_t this_value = ((uint64_t) ep->cte_val_high << 32) | (ep->cte_val_low);

	  if (this_value == value)
	    return (ctf_strptr (fp, ep->cte_name));
	}
    }

  ctf_set_errno (ofp, ECTF_NOENUMNAM);
  return NULL;
}

/* Convert the specified enum tag name to the corresponding value, if a
   matching name can be found.  Otherwise CTF_ERR is returned.  */

ctf_ret_t
ctf_enum_value (ctf_dict_t *fp, ctf_id_t type, const char *name, int64_t *valp)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  unsigned char *vlen;
  int kind;
  size_t n;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if ((type = ctf_type_resolve_unsliced (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_KIND (fp, tp);
  if (kind != CTF_K_ENUM && kind != CTF_K_ENUM64)
    return ctf_set_errno (ofp, ECTF_NOTENUM);

  vlen = ctf_vlen (fp, type, tp, &n);

  if (kind == CTF_K_ENUM)
    {
      const ctf_enum_t *ep = (const ctf_enum_t *) vlen;

      for (; n != 0; n--, ep++)
	{
	  if (strcmp (ctf_strptr (fp, ep->cte_name), name) == 0)
	    {
	      if (valp != NULL)
		*valp = ep->cte_value;
	      return 0;
	    }
	}
    }
  else
    {
      const ctf_enum64_t *ep = (const ctf_enum64_t *) vlen;

      for (; n != 0; n--, ep++)
	{
	  if (strcmp (ctf_strptr (fp, ep->cte_name), name) == 0)
	    {
	      if (valp != NULL)
		*valp = ((uint64_t) ep->cte_val_high << 32) | ep->cte_val_low;
	      return 0;
	    }
	}
    }

  return ctf_set_errno (ofp, ECTF_NOENUMNAM);
}

/* Like ctf_enum_value, but returns an unsigned int64_t instead.  */
ctf_ret_t
ctf_enum_unsigned_value (ctf_dict_t *fp, ctf_id_t type, const char *name, uint64_t *valp)
{
  ctf_ret_t ret;
  int64_t retval;

  ret = ctf_enum_value (fp, type, name, &retval);
  *valp = (uint64_t) retval;
  return ret;
}

/* Determine whether an enum's values are signed.  */
ctf_bool_t
ctf_enum_unsigned (ctf_dict_t *fp, ctf_id_t type)
{
  int kind;
  const ctf_type_t *tp;		/* The suffixed kind, if prefixed */

  if ((kind = ctf_type_kind (fp, type)) < 0)
    return -1;			/* errno is set for us.  */

  if (kind != CTF_K_ENUM && kind != CTF_K_ENUM64)
    return (ctf_set_errno (fp, ECTF_NOTENUM));

  if (ctf_lookup_by_id (&fp, type, &tp) == NULL)
    return -1;			/* errno is set for us.  */

  return !CTF_INFO_KFLAG (tp->ctt_info);
}

/* Return nonzero if this struct or union uses bitfield encoding.  */
ctf_bool_t
ctf_struct_bitfield (ctf_dict_t * fp, ctf_id_t type)
{
  int kind;
  const ctf_type_t *tp;		/* The suffixed kind, if prefixed */

  if ((kind = ctf_type_kind (fp, type)) < 0)
    return -1;			/* errno is set for us.  */

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
    return (ctf_set_errno (fp, ECTF_NOTSOU));

  if (ctf_lookup_by_id (&fp, type, &tp) == NULL)
    return -1;			/* errno is set for us.  */

  return CTF_INFO_KFLAG (tp->ctt_info);
}

/* Given a type ID relating to a function type, return info on return types and
   arg counts for that function.  */

int
ctf_func_type_info (ctf_dict_t *fp, ctf_id_t type, ctf_funcinfo_t *fip)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp, *suffix;
  uint32_t kind;
  unsigned char *vlen;
  const ctf_param_t *args;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if (ctf_type_kind (fp, type) == CTF_K_FUNC_LINKAGE)
    type = ctf_type_reference (fp, type);

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_KIND (fp, tp);
  if (kind != CTF_K_FUNCTION)
    return (ctf_set_errno (ofp, ECTF_NOTFUNC));

  fip->ctc_return = suffix->ctt_type;
  fip->ctc_flags = 0;

  vlen = ctf_vlen (fp, type, tp, &fip->ctc_argc);
  args = (const ctf_param_t *) vlen;

  if (fip->ctc_argc != 0 && args[fip->ctc_argc - 1].cfp_type == 0)
    {
      fip->ctc_flags |= CTF_FUNC_VARARG;
      fip->ctc_argc--;
    }

  return 0;
}

/* Given a type ID relating to a function type, return the arguments for the
   function.  */

int
ctf_func_type_args (ctf_dict_t *fp, ctf_id_t type, uint32_t argc, ctf_id_t *argv)
{
  const ctf_type_t *tp;
  const ctf_param_t *args;
  unsigned char *vlen;
  ctf_funcinfo_t f;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if (ctf_type_kind (fp, type) == CTF_K_FUNC_LINKAGE)
    type = ctf_type_reference (fp, type);

  if (ctf_func_type_info (fp, type, &f) < 0)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  vlen = ctf_vlen (fp, type, tp, NULL);
  args = (const ctf_param_t *) vlen;

  for (argc = MIN (argc, f.ctc_argc); argc != 0; argc--)
    *argv++ = (args++)->cfp_type;

  return 0;
}

/* Given a type ID relating to a function type, return the argument names for
   the function.  */

int
ctf_func_type_arg_names (ctf_dict_t *fp, ctf_id_t type, uint32_t argc,
			 const char **arg_names)
{
  const ctf_type_t *tp;
  const ctf_param_t *args;
  unsigned char *vlen;
  ctf_funcinfo_t f;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if (ctf_type_kind (fp, type) == CTF_K_FUNC_LINKAGE)
    type = ctf_type_reference (fp, type);

  if (ctf_func_type_info (fp, type, &f) < 0)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  vlen = ctf_vlen (fp, type, tp, NULL);
  args = (const ctf_param_t *) vlen;

  for (argc = MIN (argc, f.ctc_argc); argc != 0; argc--)
    *arg_names++ = ctf_strptr (fp, (args++)->cfp_name);

  return 0;
}

/* Get the linkage of a CTF_K_FUNC_LINKAGE or variable.  */

int
ctf_type_linkage (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;
  const ctf_type_t *suffix;
  unsigned char *vlen;
  ctf_linkage_t *l;

  int kind;

  if ((tp = ctf_lookup_by_id (&fp, type, &suffix)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = ctf_type_kind_unsliced (fp, type);
  if (kind != CTF_K_FUNC_LINKAGE && kind != CTF_K_VAR)
    return ctf_set_errno (fp, ECTF_LINKKIND);

  if (kind == CTF_K_VAR)
    {
      vlen = ctf_vlen (fp, type, tp, NULL);
      l = (ctf_linkage_t *) vlen;

      return l->ctl_linkage;
    }

  /* CTF_K_FUNC_LINKAGE.  */
  return CTF_INFO_VLEN (suffix->ctt_info);
}

/* bsearch_r comparison function for datasec searches.  */
static int
search_datasec_by_offset (const void *key_, const void *arr_)
{
  uint32_t *key = (uint32_t *) key_;
  ctf_var_secinfo_t *arr = (ctf_var_secinfo_t *) arr_;

  if (*key < arr->cvs_offset)
    return -1;
  else if (*key > arr->cvs_offset)
    return 1;

  return 0;
}

/* Search a datasec for a variable covering a given offset.

   Errors with ECTF_NODATASEC if not found.  */

ctf_id_t
ctf_datasec_var_offset (ctf_dict_t *fp, ctf_id_t datasec, uint32_t offset)
{
  ctf_dtdef_t *dtd;
  const ctf_type_t *tp;
  unsigned char *vlen;
  size_t vlen_len;
  ctf_var_secinfo_t *sec;
  ctf_var_secinfo_t *el;
  ssize_t size;

  if ((tp = ctf_lookup_by_id (&fp, datasec, NULL)) == NULL)
    return -1;			/* errno is set for us.  */

  if (ctf_type_kind (fp, datasec) != CTF_K_DATASEC)
    return ctf_set_typed_errno (fp, ECTF_NOTDATASEC);

  if ((dtd = ctf_dynamic_type (fp, datasec)) != NULL)
    {
      if (dtd->dtd_flags & DTD_F_UNSORTED)
	ctf_datasec_sort (fp, dtd);
    }

  vlen = ctf_vlen (fp, datasec, tp, &vlen_len);
  sec = (ctf_var_secinfo_t *) vlen;

  if ((el = bsearch (&offset, sec, vlen_len, sizeof (ctf_var_secinfo_t),
		     search_datasec_by_offset)) == NULL)
    return ctf_set_typed_errno (fp, ECTF_NODATASEC);

  if (el->cvs_offset == offset)
    return el->cvs_type;

  if ((size = ctf_type_size (fp, el->cvs_type)) >= 0)
    if (el->cvs_offset < offset && el->cvs_offset + size > offset)
      return el->cvs_type;

  return ctf_set_typed_errno (fp, ECTF_NODATASEC);
}

/* Return the entry corresponding to a given component_idx in a datasec.

   Not currently public API.  */

ctf_var_secinfo_t *
ctf_datasec_entry (ctf_dict_t *fp, ctf_id_t datasec, int component_idx)
{
  const ctf_type_t *tp;
  unsigned char *vlen;
  size_t vlen_len;
  ctf_var_secinfo_t *sec;

  if ((tp = ctf_lookup_by_id (&fp, datasec, NULL)) == NULL)
    return NULL;		/* errno is set for us.  */

  /* No type kind check: internal function.  */
  vlen = ctf_vlen (fp, datasec, tp, &vlen_len);
  sec = (ctf_var_secinfo_t *) vlen;

  if (component_idx < 0 || (size_t) component_idx > vlen_len)
    {
      ctf_set_errno (fp, EOVERFLOW);
      return NULL;
    }

  return &sec[component_idx];
}

/* Return the datasec that a given variable appears in, or ECTF_NODATASEC if
   none.  */

ctf_id_t ctf_variable_datasec (ctf_dict_t *fp, ctf_id_t var)
{
  void *sec;

  if (ctf_type_kind (fp, var) != CTF_K_VAR)
    return (ctf_set_typed_errno (fp, ECTF_NOTVAR));

  if (ctf_dynhash_lookup_kv (fp->ctf_var_datasecs, (void *) (ptrdiff_t) var,
			     NULL, &sec))
    return (ctf_id_t) sec;

  return (ctf_set_typed_errno (fp, ECTF_NODATASEC));
}

/* Recursively visit the members of any type.  This function is used as the
   engine for ctf_type_visit, below.  We resolve the input type, recursively
   invoke ourself for each type member if the type is a struct or union, and
   then invoke the callback function on the current type.  If any callback
   returns non-zero, we abort and percolate the error code back up to the top.  */

static int
ctf_type_rvisit (ctf_dict_t *fp, ctf_id_t type, ctf_visit_f *func,
		 void *arg, const char *name, unsigned long offset,
		 int bit_width, int depth)
{
  ctf_id_t otype = type;
  int nonrepresentable = 0;
  uint32_t kind;
  ctf_next_t *it = NULL;
  ctf_id_t membtype;
  ssize_t this_offset;
  int this_bit_width;
  int rc;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR) {
    if (ctf_errno (fp) != ECTF_NONREPRESENTABLE)
      return -1;		/* errno is set for us.  */
    else
      nonrepresentable = 1;
  }

  if ((rc = func (fp, name, otype, offset, bit_width, depth, arg)) != 0)
    return rc;

  if (!nonrepresentable)
    kind = ctf_type_kind (fp, type);

  if (nonrepresentable || (kind != CTF_K_STRUCT && kind != CTF_K_UNION))
    return 0;

  while ((this_offset = ctf_member_next (fp, type, &it, &name, &membtype,
					 &this_bit_width, 0)) >= 0)
    {
      if ((rc = ctf_type_rvisit (fp, membtype, func, arg, name,
				 offset + this_offset, this_bit_width,
				 depth + 1)) != 0)
	return rc;
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return -1;			/* errno is set for us. */

  return 0;
}

/* Recursively visit the members of any type.  We pass the name, member
 type, and offset of each member to the specified callback function.  */
int
ctf_type_visit (ctf_dict_t *fp, ctf_id_t type, ctf_visit_f *func, void *arg)
{
  return (ctf_type_rvisit (fp, type, func, arg, "", 0, 0, 0));
}
