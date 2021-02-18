/* Type handling functions.
   Copyright (C) 2019-2021 Free Software Foundation, Inc.

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

/* Determine whether a type is a parent or a child.  */

int
ctf_type_isparent (ctf_dict_t *fp, ctf_id_t id)
{
  return (LCTF_TYPE_ISPARENT (fp, id));
}

int
ctf_type_ischild (ctf_dict_t * fp, ctf_id_t id)
{
  return (LCTF_TYPE_ISCHILD (fp, id));
}

/* Iterate over the members of a STRUCT or UNION.  We pass the name, member
   type, and offset of each member to the specified callback function.  */

int
ctf_member_iter (ctf_dict_t *fp, ctf_id_t type, ctf_member_f *func, void *arg)
{
  ctf_next_t *i = NULL;
  ssize_t offset;
  const char *name;
  ctf_id_t membtype;

  while ((offset = ctf_member_next (fp, type, &i, &name, &membtype, 0)) >= 0)
    {
      int rc;
      if ((rc = func (name, membtype, offset, arg)) != 0)
	{
	  ctf_next_destroy (i);
	  return rc;
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return -1;					/* errno is set for us.  */

  return 0;
}

/* Iterate over the members of a STRUCT or UNION, returning each member's
   offset and optionally name and member type in turn.  On end-of-iteration,
   returns -1.  If FLAGS is CTF_MN_RECURSE, recurse into unnamed members.  */

ssize_t
ctf_member_next (ctf_dict_t *fp, ctf_id_t type, ctf_next_t **it,
		 const char **name, ctf_id_t *membtype, int flags)
{
  ctf_dict_t *ofp = fp;
  uint32_t kind;
  ssize_t offset;
  ctf_next_t *i = *it;

  if (!i)
    {
      const ctf_type_t *tp;
      ctf_dtdef_t *dtd;
      ssize_t increment;

      if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
	return -1;			/* errno is set for us.  */

      if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
	return -1;			/* errno is set for us.  */

      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_errno (ofp, ENOMEM);
      i->cu.ctn_fp = ofp;

      (void) ctf_get_ctt_size (fp, tp, &i->ctn_size, &increment);
      kind = LCTF_INFO_KIND (fp, tp->ctt_info);

      if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
	{
	  ctf_next_destroy (i);
	  return (ctf_set_errno (ofp, ECTF_NOTSOU));
	}

      dtd = ctf_dynamic_type (fp, type);
      i->ctn_iter_fun = (void (*) (void)) ctf_member_next;

      /* We depend below on the RDWR state indicating whether the DTD-related
	 fields or the DMD-related fields have been initialized.  */

      assert ((dtd && (fp->ctf_flags & LCTF_RDWR))
	      || (!dtd && (!(fp->ctf_flags & LCTF_RDWR))));

      if (dtd == NULL)
	{
	  i->ctn_n = LCTF_INFO_VLEN (fp, tp->ctt_info);

	  if (i->ctn_size < CTF_LSTRUCT_THRESH)
	    i->u.ctn_mp = (const ctf_member_t *) ((uintptr_t) tp + increment);
	  else
	    i->u.ctn_lmp = (const ctf_lmember_t *) ((uintptr_t) tp + increment);
	}
      else
	i->u.ctn_dmd = ctf_list_next (&dtd->dtd_u.dtu_members);

      *it = i;
    }

  if ((void (*) (void)) ctf_member_next != i->ctn_iter_fun)
    return (ctf_set_errno (ofp, ECTF_NEXT_WRONGFUN));

  if (ofp != i->cu.ctn_fp)
    return (ctf_set_errno (ofp, ECTF_NEXT_WRONGFP));

  /* Resolve to the native dict of this type.  */
  if ((fp = ctf_get_dict (ofp, type)) == NULL)
    return (ctf_set_errno (ofp, ECTF_NOPARENT));

  /* When we hit an unnamed struct/union member, we set ctn_type to indicate
     that we are inside one, then return the unnamed member: on the next call,
     we must skip over top-level member iteration in favour of iteration within
     the sub-struct until it later turns out that that iteration has ended.  */

 retry:
  if (!i->ctn_type)
    {
      if (!(fp->ctf_flags & LCTF_RDWR))
	{
	  if (i->ctn_n == 0)
	    goto end_iter;

	  if (i->ctn_size < CTF_LSTRUCT_THRESH)
	    {
	      const char *membname = ctf_strptr (fp, i->u.ctn_mp->ctm_name);

	      if (name)
		*name = membname;
	      if (membtype)
		*membtype = i->u.ctn_mp->ctm_type;
	      offset = i->u.ctn_mp->ctm_offset;

	      if (membname[0] == 0
		  && (ctf_type_kind (fp, i->u.ctn_mp->ctm_type) == CTF_K_STRUCT
		      || ctf_type_kind (fp, i->u.ctn_mp->ctm_type) == CTF_K_UNION))
		i->ctn_type = i->u.ctn_mp->ctm_type;

	      i->u.ctn_mp++;
	    }
	  else
	    {
	      const char *membname = ctf_strptr (fp, i->u.ctn_lmp->ctlm_name);

	      if (name)
		*name = membname;
	      if (membtype)
		*membtype = i->u.ctn_lmp->ctlm_type;
	      offset = (unsigned long) CTF_LMEM_OFFSET (i->u.ctn_lmp);

	      if (membname[0] == 0
		  && (ctf_type_kind (fp, i->u.ctn_lmp->ctlm_type) == CTF_K_STRUCT
		      || ctf_type_kind (fp, i->u.ctn_lmp->ctlm_type) == CTF_K_UNION))
		i->ctn_type = i->u.ctn_lmp->ctlm_type;

	      i->u.ctn_lmp++;
	    }
	  i->ctn_n--;
	}
      else
	{
	  if (i->u.ctn_dmd == NULL)
	    goto end_iter;
	  /* The dmd contains a NULL for unnamed dynamic members.  Don't inflict
	     this on our callers.  */
	  if (name)
	    {
	      if (i->u.ctn_dmd->dmd_name)
		*name = i->u.ctn_dmd->dmd_name;
	      else
		*name = "";
	    }
	  if (membtype)
	    *membtype = i->u.ctn_dmd->dmd_type;
	  offset = i->u.ctn_dmd->dmd_offset;

	  if (i->u.ctn_dmd->dmd_name == NULL
	      && (ctf_type_kind (fp, i->u.ctn_dmd->dmd_type) == CTF_K_STRUCT
		  || ctf_type_kind (fp, i->u.ctn_dmd->dmd_type) == CTF_K_UNION))
	    i->ctn_type = i->u.ctn_dmd->dmd_type;

	  i->u.ctn_dmd = ctf_list_next (i->u.ctn_dmd);
	}

      /* The callers might want automatic recursive sub-struct traversal.  */
      if (!(flags & CTF_MN_RECURSE))
	i->ctn_type = 0;

      /* Sub-struct traversal starting?  Take note of the offset of this member,
	 for later boosting of sub-struct members' offsets.  */
      if (i->ctn_type)
	i->ctn_increment = offset;
    }
  /* Traversing a sub-struct?  Just return it, with the offset adjusted.  */
  else
    {
      ssize_t ret = ctf_member_next (fp, i->ctn_type, &i->ctn_next, name,
				     membtype, flags);

      if (ret >= 0)
	return ret + i->ctn_increment;

      if (ctf_errno (fp) != ECTF_NEXT_END)
	{
	  ctf_next_destroy (i);
	  *it = NULL;
	  i->ctn_type = 0;
	  return ret;				/* errno is set for us.  */
	}

      if (!ctf_assert (fp, (i->ctn_next == NULL)))
	return -1;				/* errno is set for us.  */

      i->ctn_type = 0;
      /* This sub-struct has ended: on to the next real member.  */
      goto retry;
    }

  return offset;

 end_iter:
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_errno (ofp, ECTF_NEXT_END);
}

/* Iterate over the members of an ENUM.  We pass the string name and associated
   integer value of each enum element to the specified callback function.  */

int
ctf_enum_iter (ctf_dict_t *fp, ctf_id_t type, ctf_enum_f *func, void *arg)
{
  ctf_next_t *i = NULL;
  const char *name;
  int val;

  while ((name = ctf_enum_next (fp, type, &i, &val)) != NULL)
    {
      int rc;
      if ((rc = func (name, val, arg)) != 0)
	{
	  ctf_next_destroy (i);
	  return rc;
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return -1;					/* errno is set for us.  */

  return 0;
}

/* Iterate over the members of an enum TYPE, returning each enumerand's NAME or
   NULL at end of iteration or error, and optionally passing back the
   enumerand's integer VALue.  */

const char *
ctf_enum_next (ctf_dict_t *fp, ctf_id_t type, ctf_next_t **it,
	       int *val)
{
  ctf_dict_t *ofp = fp;
  uint32_t kind;
  const char *name;
  ctf_next_t *i = *it;

  if (!i)
    {
      const ctf_type_t *tp;
      ctf_dtdef_t *dtd;

      if ((type = ctf_type_resolve_unsliced (fp, type)) == CTF_ERR)
	return NULL;			/* errno is set for us.  */

      if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
	return NULL;			/* errno is set for us.  */

      if ((i = ctf_next_create ()) == NULL)
	{
	  ctf_set_errno (ofp, ENOMEM);
	  return NULL;
	}
      i->cu.ctn_fp = ofp;

      (void) ctf_get_ctt_size (fp, tp, NULL,
			       &i->ctn_increment);
      kind = LCTF_INFO_KIND (fp, tp->ctt_info);

      if (kind != CTF_K_ENUM)
	{
	  ctf_next_destroy (i);
	  ctf_set_errno (ofp, ECTF_NOTENUM);
	  return NULL;
	}

      dtd = ctf_dynamic_type (fp, type);
      i->ctn_iter_fun = (void (*) (void)) ctf_enum_next;

      /* We depend below on the RDWR state indicating whether the DTD-related
	 fields or the DMD-related fields have been initialized.  */

      assert ((dtd && (fp->ctf_flags & LCTF_RDWR))
	      || (!dtd && (!(fp->ctf_flags & LCTF_RDWR))));

      if (dtd == NULL)
	{
	  i->ctn_n = LCTF_INFO_VLEN (fp, tp->ctt_info);

	  i->u.ctn_en = (const ctf_enum_t *) ((uintptr_t) tp +
					      i->ctn_increment);
	}
      else
	i->u.ctn_dmd = ctf_list_next (&dtd->dtd_u.dtu_members);

      *it = i;
    }

  if ((void (*) (void)) ctf_enum_next != i->ctn_iter_fun)
    {
      ctf_set_errno (ofp, ECTF_NEXT_WRONGFUN);
      return NULL;
    }

  if (ofp != i->cu.ctn_fp)
    {
      ctf_set_errno (ofp, ECTF_NEXT_WRONGFP);
      return NULL;
    }

  /* Resolve to the native dict of this type.  */
  if ((fp = ctf_get_dict (ofp, type)) == NULL)
    {
      ctf_set_errno (ofp, ECTF_NOPARENT);
      return NULL;
    }

  if (!(fp->ctf_flags & LCTF_RDWR))
    {
      if (i->ctn_n == 0)
	goto end_iter;

      name = ctf_strptr (fp, i->u.ctn_en->cte_name);
      if (val)
	*val = i->u.ctn_en->cte_value;
      i->u.ctn_en++;
      i->ctn_n--;
    }
  else
    {
      if (i->u.ctn_dmd == NULL)
	goto end_iter;

      name = i->u.ctn_dmd->dmd_name;
      if (val)
	*val = i->u.ctn_dmd->dmd_value;
      i->u.ctn_dmd = ctf_list_next (i->u.ctn_dmd);
    }

  return name;

 end_iter:
  ctf_next_destroy (i);
  *it = NULL;
  ctf_set_errno (ofp, ECTF_NEXT_END);
  return NULL;
}

/* Iterate over every root (user-visible) type in the given CTF dict.
   We pass the type ID of each type to the specified callback function.

   Does not traverse parent types: you have to do that explicitly.  This is by
   design, to avoid traversing them more than once if traversing many children
   of a single parent.  */

int
ctf_type_iter (ctf_dict_t *fp, ctf_type_f *func, void *arg)
{
  ctf_next_t *i = NULL;
  ctf_id_t type;

  while ((type = ctf_type_next (fp, &i, NULL, 0)) != CTF_ERR)
    {
      int rc;
      if ((rc = func (type, arg)) != 0)
	{
	  ctf_next_destroy (i);
	  return rc;
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return -1;					/* errno is set for us.  */

  return 0;
}

/* Iterate over every type in the given CTF dict, user-visible or not.
   We pass the type ID of each type to the specified callback function.

   Does not traverse parent types: you have to do that explicitly.  This is by
   design, to avoid traversing them more than once if traversing many children
   of a single parent.  */

int
ctf_type_iter_all (ctf_dict_t *fp, ctf_type_all_f *func, void *arg)
{
  ctf_next_t *i = NULL;
  ctf_id_t type;
  int flag;

  while ((type = ctf_type_next (fp, &i, &flag, 1)) != CTF_ERR)
    {
      int rc;
      if ((rc = func (type, flag, arg)) != 0)
	{
	  ctf_next_destroy (i);
	  return rc;
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return -1;					/* errno is set for us.  */

  return 0;
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

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_errno (fp, ENOMEM);

      i->cu.ctn_fp = fp;
      i->ctn_type = 1;
      i->ctn_iter_fun = (void (*) (void)) ctf_type_next;
      *it = i;
    }

  if ((void (*) (void)) ctf_type_next != i->ctn_iter_fun)
    return (ctf_set_errno (fp, ECTF_NEXT_WRONGFUN));

  if (fp != i->cu.ctn_fp)
    return (ctf_set_errno (fp, ECTF_NEXT_WRONGFP));

  while (i->ctn_type <= fp->ctf_typemax)
    {
      const ctf_type_t *tp = LCTF_INDEX_TO_TYPEPTR (fp, i->ctn_type);

      if ((!want_hidden) && (!LCTF_INFO_ISROOT (fp, tp->ctt_info)))
	{
	  i->ctn_type++;
	  continue;
	}

      if (flag)
	*flag = LCTF_INFO_ISROOT (fp, tp->ctt_info);
      return LCTF_INDEX_TO_TYPE (fp, i->ctn_type++, fp->ctf_flags & LCTF_CHILD);
    }
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_errno (fp, ECTF_NEXT_END);
}

/* Iterate over every variable in the given CTF dict, in arbitrary order.
   We pass the name of each variable to the specified callback function.  */

int
ctf_variable_iter (ctf_dict_t *fp, ctf_variable_f *func, void *arg)
{
  ctf_next_t *i = NULL;
  ctf_id_t type;
  const char *name;

  while ((type = ctf_variable_next (fp, &i, &name)) != CTF_ERR)
    {
      int rc;
      if ((rc = func (name, type, arg)) != 0)
	{
	  ctf_next_destroy (i);
	  return rc;
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return -1;					/* errno is set for us.  */

  return 0;
}

/* Iterate over every variable in the given CTF dict, in arbitrary order,
   returning the name and type of each variable in turn.  The name argument is
   not optional.  Returns CTF_ERR on end of iteration or error.  */

ctf_id_t
ctf_variable_next (ctf_dict_t *fp, ctf_next_t **it, const char **name)
{
  ctf_next_t *i = *it;

  if ((fp->ctf_flags & LCTF_CHILD) && (fp->ctf_parent == NULL))
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	return ctf_set_errno (fp, ENOMEM);

      i->cu.ctn_fp = fp;
      i->ctn_iter_fun = (void (*) (void)) ctf_variable_next;
      if (fp->ctf_flags & LCTF_RDWR)
	i->u.ctn_dvd = ctf_list_next (&fp->ctf_dvdefs);
      *it = i;
    }

  if ((void (*) (void)) ctf_variable_next != i->ctn_iter_fun)
    return (ctf_set_errno (fp, ECTF_NEXT_WRONGFUN));

  if (fp != i->cu.ctn_fp)
    return (ctf_set_errno (fp, ECTF_NEXT_WRONGFP));

  if (!(fp->ctf_flags & LCTF_RDWR))
    {
      if (i->ctn_n >= fp->ctf_nvars)
	goto end_iter;

      *name = ctf_strptr (fp, fp->ctf_vars[i->ctn_n].ctv_name);
      return fp->ctf_vars[i->ctn_n++].ctv_type;
    }
  else
    {
      ctf_id_t id;

      if (i->u.ctn_dvd == NULL)
	goto end_iter;

      *name = i->u.ctn_dvd->dvd_name;
      id = i->u.ctn_dvd->dvd_type;
      i->u.ctn_dvd = ctf_list_next (i->u.ctn_dvd);
      return id;
    }

 end_iter:
  ctf_next_destroy (i);
  *it = NULL;
  return ctf_set_errno (fp, ECTF_NEXT_END);
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
  ctf_id_t prev = type, otype = type;
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;

  if (type == 0)
    return (ctf_set_errno (ofp, ECTF_NONREPRESENTABLE));

  while ((tp = ctf_lookup_by_id (&fp, type)) != NULL)
    {
      switch (LCTF_INFO_KIND (fp, tp->ctt_info))
	{
	case CTF_K_TYPEDEF:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	  if (tp->ctt_type == type || tp->ctt_type == otype
	      || tp->ctt_type == prev)
	    {
	      ctf_err_warn (ofp, 0, ECTF_CORRUPT, _("type %lx cycle detected"),
			    otype);
	      return (ctf_set_errno (ofp, ECTF_CORRUPT));
	    }
	  prev = type;
	  type = tp->ctt_type;
	  break;
	default:
	  return type;
	}
      if (type == 0)
	return (ctf_set_errno (ofp, ECTF_NONREPRESENTABLE));
    }

  return CTF_ERR;		/* errno is set for us.  */
}

/* Like ctf_type_resolve(), but traverse down through slices to their contained
   type.  */

ctf_id_t
ctf_type_resolve_unsliced (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((LCTF_INFO_KIND (fp, tp->ctt_info)) == CTF_K_SLICE)
    return ctf_type_reference (fp, type);
  return type;
}

/* Return the native dict of a given type: if called on a child and the
   type is in the parent, return the parent.  Needed if you plan to access
   the type directly, without using the API.  */
ctf_dict_t *
ctf_get_dict (ctf_dict_t *fp, ctf_id_t type)
{
    if ((fp->ctf_flags & LCTF_CHILD) && LCTF_TYPE_ISPARENT (fp, type))
      return fp->ctf_parent;

    return fp;
}

/* Look up a name in the given name table, in the appropriate hash given the
   kind of the identifier.  The name is a raw, undecorated identifier.  */

ctf_id_t ctf_lookup_by_rawname (ctf_dict_t *fp, int kind, const char *name)
{
  return ctf_lookup_by_rawhash (fp, ctf_name_table (fp, kind), name);
}

/* Look up a name in the given name table, in the appropriate hash given the
   readability state of the dictionary.  The name is a raw, undecorated
   identifier.  */

ctf_id_t ctf_lookup_by_rawhash (ctf_dict_t *fp, ctf_names_t *np, const char *name)
{
  ctf_id_t id;

  if (fp->ctf_flags & LCTF_RDWR)
    id = (ctf_id_t) (uintptr_t) ctf_dynhash_lookup (np->ctn_writable, name);
  else
    id = ctf_hash_lookup_type (np->ctn_readonly, fp, name);
  return id;
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
	  const ctf_type_t *tp = ctf_lookup_by_id (&rfp, cdp->cd_type);
	  const char *name = ctf_strptr (rfp, tp->ctt_name);

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
	      /* Integers, floats, and typedefs must always be named types.  */

	      if (name[0] == '\0')
		{
		  ctf_set_errno (fp, ECTF_CORRUPT);
		  ctf_decl_fini (&cd);
		  return NULL;
		}

	      ctf_decl_sprintf (&cd, "%s", name);
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

		if (ctf_func_type_info (rfp, cdp->cd_type, &fi) < 0)
		  goto err;		/* errno is set for us.  */

		if ((argv = calloc (fi.ctc_argc, sizeof (ctf_id_t *))) == NULL)
		  {
		    ctf_set_errno (rfp, errno);
		    goto err;
		  }

		if (ctf_func_type_args (rfp, cdp->cd_type,
					fi.ctc_argc, argv) < 0)
		  goto err;		/* errno is set for us.  */

		ctf_decl_sprintf (&cd, "(*) (");
		for (i = 0; i < fi.ctc_argc; i++)
		  {
		    char *arg = ctf_type_aname (rfp, argv[i]);

		    if (arg == NULL)
		      goto err;		/* errno is set for us.  */
		    ctf_decl_sprintf (&cd, "%s", arg);
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
		free (argv);
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
	      ctf_decl_sprintf (&cd, "enum %s", name);
	      break;
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

/* Lookup the given type ID and print a string name for it into buf.  Return
   the actual number of bytes (not including \0) needed to format the name.  */

ssize_t
ctf_type_lname (ctf_dict_t *fp, ctf_id_t type, char *buf, size_t len)
{
  char *str = ctf_type_aname (fp, type);
  size_t slen;

  if (str == NULL)
    return CTF_ERR;			/* errno is set for us.  */

  slen = strlen (str);
  snprintf (buf, len, "%s", str);
  free (str);

  if (slen >= len)
    (void) ctf_set_errno (fp, ECTF_NAMELEN);

  return slen;
}

/* Lookup the given type ID and print a string name for it into buf.  If buf
   is too small, return NULL: the ECTF_NAMELEN error is set on 'fp' for us.  */

char *
ctf_type_name (ctf_dict_t *fp, ctf_id_t type, char *buf, size_t len)
{
  ssize_t rv = ctf_type_lname (fp, type, buf, len);
  return (rv >= 0 && (size_t) rv < len ? buf : NULL);
}

/* Lookup the given type ID and return its raw, unadorned, undecorated name.
   The name will live as long as its ctf_dict_t does.

   The only decoration is that a NULL return always means an error: nameless
   types return a null string.  */

const char *
ctf_type_name_raw (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return NULL;		/* errno is set for us.  */

  if (tp->ctt_name == 0)
    return "";

  return ctf_strraw (fp, tp->ctt_name);
}

/* Lookup the given type ID and return its raw, unadorned, undecorated name as a
   new dynamically-allocated string.  */

char *
ctf_type_aname_raw (ctf_dict_t *fp, ctf_id_t type)
{
  const char *name = ctf_type_name_raw (fp, type);

  if (name != NULL)
    return strdup (name);

  return NULL;
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

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  switch (LCTF_INFO_KIND (fp, tp->ctt_info))
    {
    case CTF_K_POINTER:
      return fp->ctf_dmodel->ctd_pointer;

    case CTF_K_FUNCTION:
      return 0;		/* Function size is only known by symtab.  */

    case CTF_K_ENUM:
      return fp->ctf_dmodel->ctd_int;

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

    default: /* including slices of enums, etc */
      return (ctf_get_ctt_size (fp, tp, NULL, NULL));
    }
}

/* Resolve the type down to a base type node, and then return the alignment
   needed for the type storage in bytes.

   XXX may need arch-dependent attention.  */

ssize_t
ctf_type_align (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;
  ctf_dict_t *ofp = fp;
  int kind;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_INFO_KIND (fp, tp->ctt_info);
  switch (kind)
    {
    case CTF_K_POINTER:
    case CTF_K_FUNCTION:
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
	ctf_dtdef_t *dtd;

	if ((dtd = ctf_dynamic_type (ofp, type)) == NULL)
	  {
	    uint32_t n = LCTF_INFO_VLEN (fp, tp->ctt_info);
	    ssize_t size, increment;
	    const void *vmp;

	    (void) ctf_get_ctt_size (fp, tp, &size, &increment);
	    vmp = (unsigned char *) tp + increment;

	    if (kind == CTF_K_STRUCT)
	      n = MIN (n, 1);	/* Only use first member for structs.  */

	    if (size < CTF_LSTRUCT_THRESH)
	      {
		const ctf_member_t *mp = vmp;
		for (; n != 0; n--, mp++)
		  {
		    ssize_t am = ctf_type_align (ofp, mp->ctm_type);
		    align = MAX (align, (size_t) am);
		  }
	      }
	    else
	      {
		const ctf_lmember_t *lmp = vmp;
		for (; n != 0; n--, lmp++)
		  {
		    ssize_t am = ctf_type_align (ofp, lmp->ctlm_type);
		    align = MAX (align, (size_t) am);
		  }
	      }
	  }
	else
	  {
	      ctf_dmdef_t *dmd;

	      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
		   dmd != NULL; dmd = ctf_list_next (dmd))
		{
		  ssize_t am = ctf_type_align (ofp, dmd->dmd_type);
		  align = MAX (align, (size_t) am);
		  if (kind == CTF_K_STRUCT)
		    break;
		}
	  }

	return align;
      }

    case CTF_K_ENUM:
      return fp->ctf_dmodel->ctd_int;

    case CTF_K_FORWARD:
      /* Forwards do not have a meaningful alignment.  */
      return (ctf_set_errno (ofp, ECTF_INCOMPLETE));

    default:  /* including slices of enums, etc */
      return (ctf_get_ctt_size (fp, tp, NULL, NULL));
    }
}

/* Return the kind (CTF_K_* constant) for the specified type ID.  */

int
ctf_type_kind_unsliced (ctf_dict_t *fp, ctf_id_t type)
{
  const ctf_type_t *tp;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  return (LCTF_INFO_KIND (fp, tp->ctt_info));
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
	return -1;
      kind = ctf_type_kind_unsliced (fp, type);
    }

  return kind;
}

/* Return the kind of this type, except, for forwards, return the kind of thing
   this is a forward to.  */
int
ctf_type_kind_forwarded (ctf_dict_t *fp, ctf_id_t type)
{
  int kind;
  const ctf_type_t *tp;

  if ((kind = ctf_type_kind (fp, type)) < 0)
    return -1;			/* errno is set for us.  */

  if (kind != CTF_K_FORWARD)
    return kind;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  return tp->ctt_type;
}

/* If the type is one that directly references another type (such as POINTER),
   then return the ID of the type to which it refers.  */

ctf_id_t
ctf_type_reference (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  switch (LCTF_INFO_KIND (fp, tp->ctt_info))
    {
    case CTF_K_POINTER:
    case CTF_K_TYPEDEF:
    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
      return tp->ctt_type;
      /* Slices store their type in an unusual place.  */
    case CTF_K_SLICE:
      {
	ctf_dtdef_t *dtd;
	const ctf_slice_t *sp;

	if ((dtd = ctf_dynamic_type (ofp, type)) == NULL)
	  {
	    ssize_t increment;

	    (void) ctf_get_ctt_size (fp, tp, NULL, &increment);
	    sp = (const ctf_slice_t *) ((uintptr_t) tp + increment);
	  }
	else
	  sp = &dtd->dtd_u.dtu_slice;

	return sp->cts_type;
      }
    default:
      return (ctf_set_errno (ofp, ECTF_NOTREF));
    }
}

/* Find a pointer to type by looking in fp->ctf_ptrtab.  If we can't find a
   pointer to the given type, see if we can compute a pointer to the type
   resulting from resolving the type down to its base type and use that
   instead.  This helps with cases where the CTF data includes "struct foo *"
   but not "foo_t *" and the user accesses "foo_t *" in the debugger.

   XXX what about parent dicts?  */

ctf_id_t
ctf_type_pointer (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  ctf_id_t ntype;

  if (ctf_lookup_by_id (&fp, type) == NULL)
    return CTF_ERR;		/* errno is set for us.  */

  if ((ntype = fp->ctf_ptrtab[LCTF_TYPE_TO_INDEX (fp, type)]) != 0)
    return (LCTF_INDEX_TO_TYPE (fp, ntype, (fp->ctf_flags & LCTF_CHILD)));

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return (ctf_set_errno (ofp, ECTF_NOTYPE));

  if (ctf_lookup_by_id (&fp, type) == NULL)
    return (ctf_set_errno (ofp, ECTF_NOTYPE));

  if ((ntype = fp->ctf_ptrtab[LCTF_TYPE_TO_INDEX (fp, type)]) != 0)
    return (LCTF_INDEX_TO_TYPE (fp, ntype, (fp->ctf_flags & LCTF_CHILD)));

  return (ctf_set_errno (ofp, ECTF_NOTYPE));
}

/* Return the encoding for the specified INTEGER or FLOAT.  */

int
ctf_type_encoding (ctf_dict_t *fp, ctf_id_t type, ctf_encoding_t *ep)
{
  ctf_dict_t *ofp = fp;
  ctf_dtdef_t *dtd;
  const ctf_type_t *tp;
  ssize_t increment;
  uint32_t data;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  if ((dtd = ctf_dynamic_type (ofp, type)) != NULL)
    {
      switch (LCTF_INFO_KIND (fp, tp->ctt_info))
	{
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  *ep = dtd->dtd_u.dtu_enc;
	  break;
	case CTF_K_SLICE:
	  {
	    const ctf_slice_t *slice;
	    ctf_encoding_t underlying_en;
	    ctf_id_t underlying;

	    slice = &dtd->dtd_u.dtu_slice;
	    underlying = ctf_type_resolve (fp, slice->cts_type);
	    data = ctf_type_encoding (fp, underlying, &underlying_en);

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

  (void) ctf_get_ctt_size (fp, tp, NULL, &increment);

  switch (LCTF_INFO_KIND (fp, tp->ctt_info))
    {
    case CTF_K_INTEGER:
      data = *(const uint32_t *) ((uintptr_t) tp + increment);
      ep->cte_format = CTF_INT_ENCODING (data);
      ep->cte_offset = CTF_INT_OFFSET (data);
      ep->cte_bits = CTF_INT_BITS (data);
      break;
    case CTF_K_FLOAT:
      data = *(const uint32_t *) ((uintptr_t) tp + increment);
      ep->cte_format = CTF_FP_ENCODING (data);
      ep->cte_offset = CTF_FP_OFFSET (data);
      ep->cte_bits = CTF_FP_BITS (data);
      break;
    case CTF_K_SLICE:
      {
	const ctf_slice_t *slice;
	ctf_encoding_t underlying_en;
	ctf_id_t underlying;

	slice = (ctf_slice_t *) ((uintptr_t) tp + increment);
	underlying = ctf_type_resolve (fp, slice->cts_type);
	data = ctf_type_encoding (fp, underlying, &underlying_en);

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

  if (LCTF_TYPE_ISPARENT (lfp, ltype) && lfp->ctf_parent != NULL)
    lfp = lfp->ctf_parent;

  if (LCTF_TYPE_ISPARENT (rfp, rtype) && rfp->ctf_parent != NULL)
    rfp = rfp->ctf_parent;

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

int
ctf_type_compat (ctf_dict_t *lfp, ctf_id_t ltype,
		 ctf_dict_t *rfp, ctf_id_t rtype)
{
  const ctf_type_t *ltp, *rtp;
  ctf_encoding_t le, re;
  ctf_arinfo_t la, ra;
  uint32_t lkind, rkind;
  int same_names = 0;

  if (ctf_type_cmp (lfp, ltype, rfp, rtype) == 0)
    return 1;

  ltype = ctf_type_resolve (lfp, ltype);
  lkind = ctf_type_kind (lfp, ltype);

  rtype = ctf_type_resolve (rfp, rtype);
  rkind = ctf_type_kind (rfp, rtype);

  ltp = ctf_lookup_by_id (&lfp, ltype);
  rtp = ctf_lookup_by_id (&rfp, rtype);

  if (ltp != NULL && rtp != NULL)
    same_names = (strcmp (ctf_strptr (lfp, ltp->ctt_name),
			  ctf_strptr (rfp, rtp->ctt_name)) == 0);

  if (((lkind == CTF_K_ENUM) && (rkind == CTF_K_INTEGER)) ||
      ((rkind == CTF_K_ENUM) && (lkind == CTF_K_INTEGER)))
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
      return (ctf_type_compat (lfp, ctf_type_reference (lfp, ltype),
			       rfp, ctf_type_reference (rfp, rtype)));
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
    case CTF_K_ENUM:
      {
	int lencoded, rencoded;
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

int
ctf_member_count (ctf_dict_t *fp, ctf_id_t type)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  uint32_t kind;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  kind = LCTF_INFO_KIND (fp, tp->ctt_info);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION && kind != CTF_K_ENUM)
    return (ctf_set_errno (ofp, ECTF_NOTSUE));

  return LCTF_INFO_VLEN (fp, tp->ctt_info);
}

/* Return the type and offset for a given member of a STRUCT or UNION.  */

int
ctf_member_info (ctf_dict_t *fp, ctf_id_t type, const char *name,
		 ctf_membinfo_t *mip)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  ctf_dtdef_t *dtd;
  ssize_t size, increment;
  uint32_t kind, n;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  (void) ctf_get_ctt_size (fp, tp, &size, &increment);
  kind = LCTF_INFO_KIND (fp, tp->ctt_info);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
    return (ctf_set_errno (ofp, ECTF_NOTSOU));

  if ((dtd = ctf_dynamic_type (fp, type)) == NULL)
    {
      if (size < CTF_LSTRUCT_THRESH)
	{
	  const ctf_member_t *mp = (const ctf_member_t *) ((uintptr_t) tp +
							   increment);

	  for (n = LCTF_INFO_VLEN (fp, tp->ctt_info); n != 0; n--, mp++)
	    {
	      const char *membname = ctf_strptr (fp, mp->ctm_name);

	      if (membname[0] == 0
		  && (ctf_type_kind (fp, mp->ctm_type) == CTF_K_STRUCT
		      || ctf_type_kind (fp, mp->ctm_type) == CTF_K_UNION)
		  && (ctf_member_info (fp, mp->ctm_type, name, mip) == 0))
		return 0;

	      if (strcmp (membname, name) == 0)
		{
		  mip->ctm_type = mp->ctm_type;
		  mip->ctm_offset = mp->ctm_offset;
		  return 0;
		}
	    }
	}
      else
	{
	  const ctf_lmember_t *lmp = (const ctf_lmember_t *) ((uintptr_t) tp +
							      increment);

	  for (n = LCTF_INFO_VLEN (fp, tp->ctt_info); n != 0; n--, lmp++)
	    {
	      const char *membname = ctf_strptr (fp, lmp->ctlm_name);

	      if (membname[0] == 0
		  && (ctf_type_kind (fp, lmp->ctlm_type) == CTF_K_STRUCT
		      || ctf_type_kind (fp, lmp->ctlm_type) == CTF_K_UNION)
		  && (ctf_member_info (fp, lmp->ctlm_type, name, mip) == 0))
		return 0;

	      if (strcmp (membname, name) == 0)
		{
		  mip->ctm_type = lmp->ctlm_type;
		  mip->ctm_offset = (unsigned long) CTF_LMEM_OFFSET (lmp);
		  return 0;
		}
	    }
	}
    }
  else
    {
      ctf_dmdef_t *dmd;

      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
	   dmd != NULL; dmd = ctf_list_next (dmd))
	{
	  if (dmd->dmd_name == NULL
	      && (ctf_type_kind (fp, dmd->dmd_type) == CTF_K_STRUCT
		  || ctf_type_kind (fp, dmd->dmd_type) == CTF_K_UNION)
	      && (ctf_member_info (fp, dmd->dmd_type, name, mip) == 0))
	    return 0;

	  if (dmd->dmd_name != NULL
	      && strcmp (dmd->dmd_name, name) == 0)
	    {
	      mip->ctm_type = dmd->dmd_type;
	      mip->ctm_offset = dmd->dmd_offset;
	      return 0;
	    }
	}
    }

  return (ctf_set_errno (ofp, ECTF_NOMEMBNAM));
}

/* Return the array type, index, and size information for the specified ARRAY.  */

int
ctf_array_info (ctf_dict_t *fp, ctf_id_t type, ctf_arinfo_t *arp)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  const ctf_array_t *ap;
  const ctf_dtdef_t *dtd;
  ssize_t increment;

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  if (LCTF_INFO_KIND (fp, tp->ctt_info) != CTF_K_ARRAY)
    return (ctf_set_errno (ofp, ECTF_NOTARRAY));

  if ((dtd = ctf_dynamic_type (ofp, type)) != NULL)
    {
      *arp = dtd->dtd_u.dtu_arr;
      return 0;
    }

  (void) ctf_get_ctt_size (fp, tp, NULL, &increment);

  ap = (const ctf_array_t *) ((uintptr_t) tp + increment);
  arp->ctr_contents = ap->cta_contents;
  arp->ctr_index = ap->cta_index;
  arp->ctr_nelems = ap->cta_nelems;

  return 0;
}

/* Convert the specified value to the corresponding enum tag name, if a
   matching name can be found.  Otherwise NULL is returned.  */

const char *
ctf_enum_name (ctf_dict_t *fp, ctf_id_t type, int value)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  const ctf_enum_t *ep;
  const ctf_dtdef_t *dtd;
  ssize_t increment;
  uint32_t n;

  if ((type = ctf_type_resolve_unsliced (fp, type)) == CTF_ERR)
    return NULL;		/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return NULL;		/* errno is set for us.  */

  if (LCTF_INFO_KIND (fp, tp->ctt_info) != CTF_K_ENUM)
    {
      (void) ctf_set_errno (ofp, ECTF_NOTENUM);
      return NULL;
    }

  (void) ctf_get_ctt_size (fp, tp, NULL, &increment);

  if ((dtd = ctf_dynamic_type (ofp, type)) == NULL)
    {
      ep = (const ctf_enum_t *) ((uintptr_t) tp + increment);

      for (n = LCTF_INFO_VLEN (fp, tp->ctt_info); n != 0; n--, ep++)
	{
	  if (ep->cte_value == value)
	    return (ctf_strptr (fp, ep->cte_name));
	}
    }
  else
    {
      ctf_dmdef_t *dmd;

      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
	   dmd != NULL; dmd = ctf_list_next (dmd))
	{
	  if (dmd->dmd_value == value)
	    return dmd->dmd_name;
	}
    }

  (void) ctf_set_errno (ofp, ECTF_NOENUMNAM);
  return NULL;
}

/* Convert the specified enum tag name to the corresponding value, if a
   matching name can be found.  Otherwise CTF_ERR is returned.  */

int
ctf_enum_value (ctf_dict_t * fp, ctf_id_t type, const char *name, int *valp)
{
  ctf_dict_t *ofp = fp;
  const ctf_type_t *tp;
  const ctf_enum_t *ep;
  const ctf_dtdef_t *dtd;
  ssize_t increment;
  uint32_t n;

  if ((type = ctf_type_resolve_unsliced (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  if (LCTF_INFO_KIND (fp, tp->ctt_info) != CTF_K_ENUM)
    {
      (void) ctf_set_errno (ofp, ECTF_NOTENUM);
      return -1;
    }

  (void) ctf_get_ctt_size (fp, tp, NULL, &increment);

  ep = (const ctf_enum_t *) ((uintptr_t) tp + increment);

  if ((dtd = ctf_dynamic_type (ofp, type)) == NULL)
    {
      for (n = LCTF_INFO_VLEN (fp, tp->ctt_info); n != 0; n--, ep++)
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
      ctf_dmdef_t *dmd;

      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
	   dmd != NULL; dmd = ctf_list_next (dmd))
	{
	  if (strcmp (dmd->dmd_name, name) == 0)
	    {
	      if (valp != NULL)
		*valp = dmd->dmd_value;
	      return 0;
	    }
	}
    }

  (void) ctf_set_errno (ofp, ECTF_NOENUMNAM);
  return -1;
}

/* Given a type ID relating to a function type, return info on return types and
   arg counts for that function.  */

int
ctf_func_type_info (ctf_dict_t *fp, ctf_id_t type, ctf_funcinfo_t *fip)
{
  const ctf_type_t *tp;
  uint32_t kind;
  const uint32_t *args;
  const ctf_dtdef_t *dtd;
  ssize_t size, increment;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  (void) ctf_get_ctt_size (fp, tp, &size, &increment);
  kind = LCTF_INFO_KIND (fp, tp->ctt_info);

  if (kind != CTF_K_FUNCTION)
    return (ctf_set_errno (fp, ECTF_NOTFUNC));

  fip->ctc_return = tp->ctt_type;
  fip->ctc_flags = 0;
  fip->ctc_argc = LCTF_INFO_VLEN (fp, tp->ctt_info);

  if ((dtd = ctf_dynamic_type (fp, type)) == NULL)
    args = (uint32_t *) ((uintptr_t) tp + increment);
  else
    args = dtd->dtd_u.dtu_argv;

  if (fip->ctc_argc != 0 && args[fip->ctc_argc - 1] == 0)
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
  const uint32_t *args;
  const ctf_dtdef_t *dtd;
  ssize_t size, increment;
  ctf_funcinfo_t f;

  if (ctf_func_type_info (fp, type, &f) < 0)
    return -1;			/* errno is set for us.  */

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  (void) ctf_get_ctt_size (fp, tp, &size, &increment);

  if ((dtd = ctf_dynamic_type (fp, type)) == NULL)
    args = (uint32_t *) ((uintptr_t) tp + increment);
  else
    args = dtd->dtd_u.dtu_argv;

  for (argc = MIN (argc, f.ctc_argc); argc != 0; argc--)
    *argv++ = *args++;

  return 0;
}

/* Recursively visit the members of any type.  This function is used as the
   engine for ctf_type_visit, below.  We resolve the input type, recursively
   invoke ourself for each type member if the type is a struct or union, and
   then invoke the callback function on the current type.  If any callback
   returns non-zero, we abort and percolate the error code back up to the top.  */

static int
ctf_type_rvisit (ctf_dict_t *fp, ctf_id_t type, ctf_visit_f *func,
		 void *arg, const char *name, unsigned long offset, int depth)
{
  ctf_id_t otype = type;
  const ctf_type_t *tp;
  const ctf_dtdef_t *dtd;
  ssize_t size, increment;
  uint32_t kind, n;
  int rc;

  if ((type = ctf_type_resolve (fp, type)) == CTF_ERR)
    return -1;			/* errno is set for us.  */

  if ((tp = ctf_lookup_by_id (&fp, type)) == NULL)
    return -1;			/* errno is set for us.  */

  if ((rc = func (name, otype, offset, depth, arg)) != 0)
    return rc;

  kind = LCTF_INFO_KIND (fp, tp->ctt_info);

  if (kind != CTF_K_STRUCT && kind != CTF_K_UNION)
    return 0;

  (void) ctf_get_ctt_size (fp, tp, &size, &increment);

  if ((dtd = ctf_dynamic_type (fp, type)) == NULL)
    {
      if (size < CTF_LSTRUCT_THRESH)
	{
	  const ctf_member_t *mp = (const ctf_member_t *) ((uintptr_t) tp +
							   increment);

	  for (n = LCTF_INFO_VLEN (fp, tp->ctt_info); n != 0; n--, mp++)
	    {
	      if ((rc = ctf_type_rvisit (fp, mp->ctm_type,
					 func, arg, ctf_strptr (fp,
								mp->ctm_name),
					 offset + mp->ctm_offset,
					 depth + 1)) != 0)
		return rc;
	    }
	}
      else
	{
	  const ctf_lmember_t *lmp = (const ctf_lmember_t *) ((uintptr_t) tp +
							      increment);

	  for (n = LCTF_INFO_VLEN (fp, tp->ctt_info); n != 0; n--, lmp++)
	    {
	      if ((rc = ctf_type_rvisit (fp, lmp->ctlm_type,
					 func, arg, ctf_strptr (fp,
								lmp->ctlm_name),
					 offset + (unsigned long) CTF_LMEM_OFFSET (lmp),
					 depth + 1)) != 0)
		return rc;
	    }
	}
    }
  else
    {
      ctf_dmdef_t *dmd;

      for (dmd = ctf_list_next (&dtd->dtd_u.dtu_members);
	   dmd != NULL; dmd = ctf_list_next (dmd))
	{
	  if ((rc = ctf_type_rvisit (fp, dmd->dmd_type, func, arg,
				     dmd->dmd_name, dmd->dmd_offset,
				     depth + 1)) != 0)
	    return rc;
	}
    }

  return 0;
}

/* Recursively visit the members of any type.  We pass the name, member
 type, and offset of each member to the specified callback function.  */
int
ctf_type_visit (ctf_dict_t *fp, ctf_id_t type, ctf_visit_f *func, void *arg)
{
  return (ctf_type_rvisit (fp, type, func, arg, "", 0, 0));
}
