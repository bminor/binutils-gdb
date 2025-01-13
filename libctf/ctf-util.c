/* Simple data structure utilities and helpers.
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
#include "ctf-endian.h"

/* Simple doubly-linked list append routine.  This implementation assumes that
   each list element contains an embedded ctf_list_t as the first member.
   An additional ctf_list_t is used to store the head (l_next) and tail
   (l_prev) pointers.  The current head and tail list elements have their
   previous and next pointers set to NULL, respectively.  */

void
ctf_list_append (ctf_list_t *lp, void *newp)
{
  ctf_list_t *p = lp->l_prev;	/* p = tail list element.  */
  ctf_list_t *q = newp;		/* q = new list element.  */

  lp->l_prev = q;
  q->l_prev = p;
  q->l_next = NULL;

  if (p != NULL)
    p->l_next = q;
  else
    lp->l_next = q;
}

/* Prepend the specified existing element to the given ctf_list_t.  The
   existing pointer should be pointing at a struct with embedded ctf_list_t.  */

void
ctf_list_prepend (ctf_list_t * lp, void *newp)
{
  ctf_list_t *p = newp;		/* p = new list element.  */
  ctf_list_t *q = lp->l_next;	/* q = head list element.  */

  lp->l_next = p;
  p->l_prev = NULL;
  p->l_next = q;

  if (q != NULL)
    q->l_prev = p;
  else
    lp->l_prev = p;
}

/* Delete the specified existing element from the given ctf_list_t.  The
   existing pointer should be pointing at a struct with embedded ctf_list_t.  */

void
ctf_list_delete (ctf_list_t *lp, void *existing)
{
  ctf_list_t *p = existing;

  if (p->l_prev != NULL)
    p->l_prev->l_next = p->l_next;
  else
    lp->l_next = p->l_next;

  if (p->l_next != NULL)
    p->l_next->l_prev = p->l_prev;
  else
    lp->l_prev = p->l_prev;
}

/* Return 1 if the list is empty.  */

int
ctf_list_empty_p (ctf_list_t *lp)
{
  return (lp->l_next == NULL && lp->l_prev == NULL);
}

/* Splice one entire list onto the end of another one.  The existing list is
   emptied.  */

void
ctf_list_splice (ctf_list_t *lp, ctf_list_t *append)
{
  if (ctf_list_empty_p (append))
    return;

  if (lp->l_prev != NULL)
    lp->l_prev->l_next = append->l_next;
  else
    lp->l_next = append->l_next;

  append->l_next->l_prev = lp->l_prev;
  lp->l_prev = append->l_prev;
  append->l_next = NULL;
  append->l_prev = NULL;
}

/* A string appender working on dynamic strings.  Returns NULL on OOM.  */

char *
ctf_str_append (char *s, const char *append)
{
  size_t s_len = 0;

  if (append == NULL)
    return s;

  if (s != NULL)
    s_len = strlen (s);

  size_t append_len = strlen (append);

  if ((s = realloc (s, s_len + append_len + 1)) == NULL)
    return NULL;

  memcpy (s + s_len, append, append_len);
  s[s_len + append_len] = '\0';

  return s;
}

/* A version of ctf_str_append that returns the old string on OOM.  */

char *
ctf_str_append_noerr (char *s, const char *append)
{
  char *new_s;

  new_s = ctf_str_append (s, append);
  if (!new_s)
    return s;
  return new_s;
}

/* Initialize the ref system.  */
int
ctf_init_refs (ctf_dict_t *fp)
{
  fp->ctf_movable_refs = ctf_dynhash_create (ctf_hash_integer,
					     ctf_hash_eq_integer,
					     NULL, NULL);
  if (!fp->ctf_movable_refs)
    return -ENOMEM;
  return 0;
}
/* Destroy the ref system.  */
void
ctf_free_refs (ctf_dict_t *fp)
{
  ctf_dynhash_destroy (fp->ctf_movable_refs);
}

/* Allocate a ref and bind it into a ref list.  Does not actually
   initialize anything through the ref: the caller must do that.  */

ctf_ref_t *
ctf_create_ref (ctf_dict_t *fp, ctf_list_t *reflist, uint32_t *ref, int movable)
{
  ctf_ref_t *aref;

  aref = malloc (sizeof (struct ctf_ref));

  if (!aref)
    return NULL;

  aref->cre_ref = ref;

  /* Movable refs get a backpointer to them in ctf_movable_refs: they can be
     moved later in batches via a call to ctf_move_refs.  */

  if (movable)
    {
      if (ctf_dynhash_insert (fp->ctf_movable_refs, ref, aref) < 0)
	{
	  free (aref);
	  return NULL;
	}
    }

  ctf_list_append (reflist, aref);

  return aref;
}

/* Note that refs have moved from (SRC, LEN) to DEST.  We use the movable
   refs backpointer for this, because it is done an amortized-constant
   number of times during structure member and enumerand addition, and if we
   did a linear search this would turn such addition into an O(n^2)
   operation.  */
int
ctf_move_refs (ctf_dict_t *fp, void *src, size_t len, void *dest)
{
  uintptr_t p;

  if (src == dest)
    return 0;

  for (p = (uintptr_t) src; p - (uintptr_t) src < len; p++)
    {
      ctf_ref_t *ref;

      if ((ref = ctf_dynhash_lookup (fp->ctf_movable_refs,
				     (ctf_ref_t *) p)) != NULL)
	{
	  int out_of_memory;

	  ref->cre_ref = (uint32_t *) (((uintptr_t) ref->cre_ref +
					(uintptr_t) dest - (uintptr_t) src));
	  ctf_dynhash_remove (fp->ctf_movable_refs, (ctf_ref_t *) p);
	  out_of_memory = ctf_dynhash_insert (fp->ctf_movable_refs,
					      ref->cre_ref, ref);
	  assert (out_of_memory == 0);
	}
    }

  return 0;
}

/* Remove a single ref.  */
void
ctf_remove_ref (ctf_dict_t *fp, ctf_list_t *reflist, uint32_t *ref)
{
  ctf_ref_t *aref, *anext;

  for (aref = ctf_list_next (reflist); aref != NULL; aref = anext)
    {
      anext = ctf_list_next (aref);
      if (aref->cre_ref == ref)
	{
	  ctf_list_delete (reflist, aref);
	  ctf_dynhash_remove (fp->ctf_movable_refs, ref);
	  free (aref);
	}
    }
}


/* Remove all refs to a given entity.  */
void
ctf_purge_ref_list (ctf_dict_t *fp, ctf_list_t *reflist)
{
  ctf_ref_t *ref, *next;

  for (ref = ctf_list_next (reflist); ref != NULL; ref = next)
    {
      next = ctf_list_next (ref);
      ctf_list_delete (reflist, ref);
      ctf_dynhash_remove (fp->ctf_movable_refs, ref);
      free (ref);
    }
}


/* Update a list of refs to the specified value. */
void
ctf_update_refs (ctf_list_t *reflist, uint32_t value)
{
  ctf_ref_t *ref;

  for (ref = ctf_list_next (reflist); ref != NULL;
       ref = ctf_list_next (ref))
    *(ref->cre_ref) = value;
}

/* Create a ctf_next_t.  */

ctf_next_t *
ctf_next_create (void)
{
  return calloc (1, sizeof (struct ctf_next));
}

/* Destroy a ctf_next_t, for early exit from iterators.  */

void
ctf_next_destroy (ctf_next_t *i)
{
  if (i == NULL)
    return;

  if (i->ctn_iter_fun == (void (*) (void)) ctf_dynhash_next_sorted)
    free (i->u.ctn_sorted_hkv);
  if (i->ctn_next)
    ctf_next_destroy (i->ctn_next);
  if (i->ctn_next_inner)
    ctf_next_destroy (i->ctn_next_inner);
  free (i);
}

/* Copy a ctf_next_t.  */

ctf_next_t *
ctf_next_copy (ctf_next_t *i)
{
  ctf_next_t *i2;

  if ((i2 = ctf_next_create()) == NULL)
    return NULL;
  memcpy (i2, i, sizeof (struct ctf_next));

  if (i2->ctn_next)
    {
      i2->ctn_next = ctf_next_copy (i2->ctn_next);
      if (i2->ctn_next == NULL)
	goto err_next;
    }

  if (i2->ctn_next_inner)
    {
      i2->ctn_next_inner = ctf_next_copy (i2->ctn_next_inner);
      if (i2->ctn_next_inner == NULL)
	goto err_next_inner;
    }

  if (i2->ctn_iter_fun == (void (*) (void)) ctf_dynhash_next_sorted)
    {
      size_t els = ctf_dynhash_elements ((ctf_dynhash_t *) i->cu.ctn_h);
      if ((i2->u.ctn_sorted_hkv = calloc (els, sizeof (ctf_next_hkv_t))) == NULL)
	goto err_sorted_hkv;
      memcpy (i2->u.ctn_sorted_hkv, i->u.ctn_sorted_hkv,
	      els * sizeof (ctf_next_hkv_t));
    }
  return i2;

 err_sorted_hkv:
  ctf_next_destroy (i2->ctn_next_inner);
 err_next_inner:
  ctf_next_destroy (i2->ctn_next);
 err_next:
  ctf_next_destroy (i2);
  return NULL;
}
