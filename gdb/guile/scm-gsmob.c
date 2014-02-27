/* GDB/Scheme smobs (gsmob is pronounced "jee smob")

   Copyright (C) 2014 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* See README file in this directory for implementation notes, coding
   conventions, et.al.  */

/* Smobs are Guile's "small object".
   They are used to export C structs to Scheme.

   Note: There's only room in the encoding space for 256, and while we won't
   come close to that, mixed with other libraries maybe someday we could.
   We don't worry about it now, except to be aware of the issue.
   We could allocate just a few smobs and use the unused smob flags field to
   specify the gdb smob kind, that is left for another day if it ever is
   needed.

   We want the objects we export to Scheme to be extensible by the user.
   A gsmob (gdb smob) adds a simple API on top of smobs to support this.
   This allows GDB objects to be easily extendable in a useful manner.
   To that end, all smobs in gdb have gdb_smob as the first member.

   On top of gsmobs there are "chained gsmobs".  They are used to assist with
   life-time tracking of GDB objects vs Scheme objects.  Gsmobs can "subclass"
   chained_gdb_smob, which contains a doubly-linked list to assist with
   life-time tracking.

   On top of gsmobs there are also "eqable gsmobs".  Gsmobs can "subclass"
   eqable_gdb_smob instead of gdb_smob, and is used to make gsmobs eq?-able.
   This is done by recording all gsmobs in a hash table and before creating a
   gsmob first seeing if it's already in the table.  Eqable gsmobs can also be
   used where lifetime-tracking is required.

   Gsmobs (and chained/eqable gsmobs) add an extra field that is used to
   record extra data: "properties".  It is a table of key/value pairs
   that can be set with set-gsmob-property!, gsmob-property.  */

#include "defs.h"
#include "hashtab.h"
#include "gdb_assert.h"
#include "objfiles.h"
#include "guile-internal.h"

/* We need to call this.  Undo our hack to prevent others from calling it.  */
#undef scm_make_smob_type

static htab_t registered_gsmobs;

/* Gsmob properties are initialize stored as an alist to minimize space
   usage: GDB can be used to debug some really big programs, and property
   lists generally have very few elements.  Once the list grows to this
   many elements then we switch to a hash table.
   The smallest Guile hashtable in 2.0 uses a vector of 31 elements.
   The value we use here is large enough to hold several expected uses,
   without being so large that we might as well just use a hashtable.  */
#define SMOB_PROP_HTAB_THRESHOLD 7

/* Hash function for registered_gsmobs hash table.  */

static hashval_t
hash_scm_t_bits (const void *item)
{
  uintptr_t v = (uintptr_t) item;

  return v;
}

/* Equality function for registered_gsmobs hash table.  */

static int
eq_scm_t_bits (const void *item_lhs, const void *item_rhs)
{
  return item_lhs == item_rhs;
}

/* Record GSMOB_CODE as being a gdb smob.
   GSMOB_CODE is the result of scm_make_smob_type.  */

static void
register_gsmob (scm_t_bits gsmob_code)
{
  void **slot;

  slot = htab_find_slot (registered_gsmobs, (void *) gsmob_code, INSERT);
  gdb_assert (*slot == NULL);
  *slot = (void *) gsmob_code;
}

/* Return non-zero if SCM is any registered gdb smob object.  */

static int
gdbscm_is_gsmob (SCM scm)
{
  void **slot;

  if (SCM_IMP (scm))
    return 0;
  slot = htab_find_slot (registered_gsmobs, (void *) SCM_TYP16 (scm),
			 NO_INSERT);
  return slot != NULL;
}

/* Call this to register a smob, instead of scm_make_smob_type.  */

scm_t_bits
gdbscm_make_smob_type (const char *name, size_t size)
{
  scm_t_bits result = scm_make_smob_type (name, size);

  register_gsmob (result);
  return result;
}

/* Initialize a gsmob.  */

void
gdbscm_init_gsmob (gdb_smob *base)
{
  base->properties = SCM_EOL;
}

/* Initialize a chained_gdb_smob.
   This is the same as gdbscm_init_gsmob except that it also sets prev,next
   to NULL.  */

void
gdbscm_init_chained_gsmob (chained_gdb_smob *base)
{
  gdbscm_init_gsmob ((gdb_smob *) base);
  base->prev = NULL;
  base->next = NULL;
}

/* Initialize an eqable_gdb_smob.
   This is the same as gdbscm_init_gsmob except that it also sets
   BASE->containing_scm to CONTAINING_SCM.  */

void
gdbscm_init_eqable_gsmob (eqable_gdb_smob *base, SCM containing_scm)
{
  gdbscm_init_gsmob ((gdb_smob *) base);
  base->containing_scm = containing_scm;
}

/* Call this from each smob's "mark" routine.
   In general, this should be called as:
   return gdbscm_mark_gsmob (base);  */

SCM
gdbscm_mark_gsmob (gdb_smob *base)
{
  /* Return the last one to mark as an optimization.
     The marking infrastructure will mark it for us.  */
  return base->properties;
}

/* Call this from each smob's "mark" routine.
   In general, this should be called as:
   return gdbscm_mark_chained_gsmob (base);  */

SCM
gdbscm_mark_chained_gsmob (chained_gdb_smob *base)
{
  /* Return the last one to mark as an optimization.
     The marking infrastructure will mark it for us.  */
  return base->properties;
}

/* Call this from each smob's "mark" routine.
   In general, this should be called as:
   return gdbscm_mark_eqable_gsmob (base);  */

SCM
gdbscm_mark_eqable_gsmob (eqable_gdb_smob *base)
{
  /* There's no need to mark containing_scm.
     Any references to it either come from Scheme in which case it will be
     marked through them, or there's a reference to the smob from gdb in
     which case the smob is GC-protected.  */

  /* Return the last one to mark as an optimization.
     The marking infrastructure will mark it for us.  */
  return base->properties;
}

/* gsmob accessors */

/* Return the gsmob in SELF.
   Throws an exception if SELF is not a gsmob.  */

static SCM
gsscm_get_gsmob_arg_unsafe (SCM self, int arg_pos, const char *func_name)
{
  SCM_ASSERT_TYPE (gdbscm_is_gsmob (self), self, arg_pos, func_name,
		   _("any gdb smob"));

  return self;
}

/* (gsmob-kind gsmob) -> symbol

   Note: While one might want to name this gsmob-class-name, it is named
   "-kind" because smobs aren't real GOOPS classes.  */

static SCM
gdbscm_gsmob_kind (SCM self)
{
  SCM smob, result;
  scm_t_bits smobnum;
  const char *name;
  char *kind;

  smob = gsscm_get_gsmob_arg_unsafe (self, SCM_ARG1, FUNC_NAME);

  smobnum = SCM_SMOBNUM (smob);
  name = SCM_SMOBNAME (smobnum);
  kind = xstrprintf ("<%s>", name);
  result = scm_from_latin1_symbol (kind);
  xfree (kind);

  return result;
}

/* (gsmob-property gsmob property) -> object
   If property isn't present then #f is returned.  */

static SCM
gdbscm_gsmob_property (SCM self, SCM property)
{
  SCM smob;
  gdb_smob *base;

  smob = gsscm_get_gsmob_arg_unsafe (self, SCM_ARG1, FUNC_NAME);
  base = (gdb_smob *) SCM_SMOB_DATA (self);

  /* Have we switched to a hash table?  */
  if (gdbscm_is_true (scm_hash_table_p (base->properties)))
    return scm_hashq_ref (base->properties, property, SCM_BOOL_F);

  return scm_assq_ref (base->properties, property);
}

/* (set-gsmob-property! gsmob property new-value) -> unspecified */

static SCM
gdbscm_set_gsmob_property_x (SCM self, SCM property, SCM new_value)
{
  SCM smob, alist;
  gdb_smob *base;

  smob = gsscm_get_gsmob_arg_unsafe (self, SCM_ARG1, FUNC_NAME);
  base = (gdb_smob *) SCM_SMOB_DATA (self);

  /* Have we switched to a hash table?  */
  if (gdbscm_is_true (scm_hash_table_p (base->properties)))
    {
      scm_hashq_set_x (base->properties, property, new_value);
      return SCM_UNSPECIFIED;
    }

  alist = scm_assq_set_x (base->properties, property, new_value);

  /* Did we grow the list?  */
  if (!scm_is_eq (alist, base->properties))
    {
      /* If we grew the list beyond a threshold in size,
	 switch to a hash table.  */
      if (scm_ilength (alist) >= SMOB_PROP_HTAB_THRESHOLD)
	{
	  SCM elm, htab;

	  htab = scm_c_make_hash_table (SMOB_PROP_HTAB_THRESHOLD);
	  for (elm = alist; elm != SCM_EOL; elm = scm_cdr (elm))
	    scm_hashq_set_x (htab, scm_caar (elm), scm_cdar (elm));
	  base->properties = htab;
	  return SCM_UNSPECIFIED;
	}
    }

  base->properties = alist;
  return SCM_UNSPECIFIED;
}

/* (gsmob-has-property? gsmob property) -> boolean */

static SCM
gdbscm_gsmob_has_property_p (SCM self, SCM property)
{
  SCM smob, handle;
  gdb_smob *base;

  smob = gsscm_get_gsmob_arg_unsafe (self, SCM_ARG1, FUNC_NAME);
  base = (gdb_smob *) SCM_SMOB_DATA (self);

  if (gdbscm_is_true (scm_hash_table_p (base->properties)))
    handle = scm_hashq_get_handle (base->properties, property);
  else
    handle = scm_assq (property, base->properties);

  return scm_from_bool (gdbscm_is_true (handle));
}

/* Helper function for gdbscm_gsmob_properties.  */

static SCM
add_property_name (void *closure, SCM handle)
{
  SCM *resultp = closure;

  *resultp = scm_cons (scm_car (handle), *resultp);
  return SCM_UNSPECIFIED;
}

/* (gsmob-properties gsmob) -> list
   The list is unsorted.  */

static SCM
gdbscm_gsmob_properties (SCM self)
{
  SCM smob, handle, result;
  gdb_smob *base;

  smob = gsscm_get_gsmob_arg_unsafe (self, SCM_ARG1, FUNC_NAME);
  base = (gdb_smob *) SCM_SMOB_DATA (self);

  result = SCM_EOL;
  if (gdbscm_is_true (scm_hash_table_p (base->properties)))
    {
      scm_internal_hash_for_each_handle (add_property_name, &result,
					 base->properties);
    }
  else
    {
      SCM elm;

      for (elm = base->properties; elm != SCM_EOL; elm = scm_cdr (elm))
	result = scm_cons (scm_caar (elm), result);
    }

  return result;
}

/* When underlying gdb data structures are deleted, we need to update any
   smobs with references to them.  There are several smobs that reference
   objfile-based data, so we provide helpers to manage this.  */

/* Add G_SMOB to the reference chain for OBJFILE specified by DATA_KEY.
   OBJFILE may be NULL, in which case just set prev,next to NULL.  */

void
gdbscm_add_objfile_ref (struct objfile *objfile,
			const struct objfile_data *data_key,
			chained_gdb_smob *g_smob)
{
  g_smob->prev = NULL;
  if (objfile != NULL)
    {
      g_smob->next = objfile_data (objfile, data_key);
      if (g_smob->next)
	g_smob->next->prev = g_smob;
      set_objfile_data (objfile, data_key, g_smob);
    }
  else
    g_smob->next = NULL;
}

/* Remove G_SMOB from the reference chain for OBJFILE specified
   by DATA_KEY.  OBJFILE may be NULL.  */

void
gdbscm_remove_objfile_ref (struct objfile *objfile,
			   const struct objfile_data *data_key,
			   chained_gdb_smob *g_smob)
{
  if (g_smob->prev)
    g_smob->prev->next = g_smob->next;
  else if (objfile != NULL)
    set_objfile_data (objfile, data_key, g_smob->next);
  if (g_smob->next)
    g_smob->next->prev = g_smob->prev;
}

/* Create a hash table for mapping a pointer to a gdb data structure to the
   gsmob that wraps it.  */

htab_t
gdbscm_create_eqable_gsmob_ptr_map (htab_hash hash_fn, htab_eq eq_fn)
{
  htab_t htab = htab_create_alloc (7, hash_fn, eq_fn,
				   NULL, xcalloc, xfree);

  return htab;
}

/* Return a pointer to the htab entry for the eq?-able gsmob BASE.
   If the entry is found, *SLOT is non-NULL.
   Otherwise *slot is NULL.  */

eqable_gdb_smob **
gdbscm_find_eqable_gsmob_ptr_slot (htab_t htab, eqable_gdb_smob *base)
{
  void **slot = htab_find_slot (htab, base, INSERT);

  return (eqable_gdb_smob **) slot;
}

/* Record BASE in SLOT.  SLOT must be the result of calling
   gdbscm_find_eqable_gsmob_ptr_slot on BASE (or equivalent for lookup).  */

void
gdbscm_fill_eqable_gsmob_ptr_slot (eqable_gdb_smob **slot,
				   eqable_gdb_smob *base)
{
  *slot = base;
}

/* Remove BASE from HTAB.
   BASE is a pointer to a gsmob that wraps a pointer to a GDB datum.
   This is used, for example, when an object is freed.

   It is an error to call this if PTR is not in HTAB (only because it allows
   for some consistency checking).  */

void
gdbscm_clear_eqable_gsmob_ptr_slot (htab_t htab, eqable_gdb_smob *base)
{
  void **slot = htab_find_slot (htab, base, NO_INSERT);

  gdb_assert (slot != NULL);
  htab_clear_slot (htab, slot);
}

/* Initialize the Scheme gsmobs code.  */

static const scheme_function gsmob_functions[] =
{
  { "gsmob-kind", 1, 0, 0, gdbscm_gsmob_kind,
    "\
Return the kind of the smob, e.g., <gdb:breakpoint>, as a symbol." },

  { "gsmob-property", 2, 0, 0, gdbscm_gsmob_property,
    "\
Return the specified property of the gsmob." },

  { "set-gsmob-property!", 3, 0, 0, gdbscm_set_gsmob_property_x,
    "\
Set the specified property of the gsmob." },

  { "gsmob-has-property?", 2, 0, 0, gdbscm_gsmob_has_property_p,
    "\
Return #t if the specified property is present." },

  { "gsmob-properties", 1, 0, 0, gdbscm_gsmob_properties,
    "\
Return an unsorted list of names of properties." },

  END_FUNCTIONS
};

void
gdbscm_initialize_smobs (void)
{
  registered_gsmobs = htab_create_alloc (10,
					 hash_scm_t_bits, eq_scm_t_bits,
					 NULL, xcalloc, xfree);

  gdbscm_define_functions (gsmob_functions, 1);
}
