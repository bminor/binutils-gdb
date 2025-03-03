/* Test parent / child ID assignment.  */

#include <ctf-api.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
test (int empty_parent, int unserialized_parent)
{
  ctf_dict_t *parent;
  ctf_dict_t *child;
  ctf_id_t pint = 0, pprovint = 0, pptr = 0, parray = 0, pfunction = 0;
  ctf_id_t ctype, ctype2, cslice, ctypedef, cfunction, cself;
  ctf_id_t foo;
  ctf_encoding_t encoding = { CTF_INT_SIGNED, 0, (sizeof (char) * 8) - 1 };
  ctf_encoding_t slice_encoding = { CTF_INT_SIGNED, 1, (sizeof (char) * 8) - 1 };
  ctf_encoding_t out;
  unsigned char *pbuf = NULL, *cbuf = NULL, *pbuf2 = NULL, *cbuf2 = NULL;
  size_t psize, csize;
  int err;
  ctf_id_t first_child_type = 1;
  ctf_membinfo_t memb;
  ctf_arinfo_t ar;
  ctf_funcinfo_t func;
  ctf_funcinfo_t pfunc, cfunc;
  ctf_id_t args[2], pargs[2], cargs[2];

  printf ("Testing with %s, %s parent\n", empty_parent ? "empty" : "nonempty",
	  unserialized_parent ? "unserialized" : "serialized");

  if ((parent = ctf_create (&err)) == NULL)
    goto create_err;

  if ((child = ctf_create (&err)) == NULL)
    goto create_err;

  /* Try some tests with a parent that already has some types in it (thus, a
     nonempty stypes range).  */
  if (!empty_parent)
    {
      if ((pint = ctf_add_integer (parent, CTF_ADD_ROOT, "int", &encoding)) == CTF_ERR)
	goto parent_add_err;
      first_child_type++;
    }

  if (!unserialized_parent)
    {
      if ((pbuf = ctf_write_mem (parent, &psize, -1)) == NULL)
	goto parent_write_err;

      ctf_dict_close (parent);

      if ((parent = ctf_simple_open ((char *) pbuf, psize, NULL, 0, 0, NULL, 0, &err)) == NULL)
	goto parent_open_err;

      if (!empty_parent)
	{
	  /* Look up int again: its ID will have changed.  */
	  if ((pint = ctf_lookup_by_name (parent, "int")) == CTF_ERR)
	    {
	      fprintf (stderr, "Cannot look up int in parent: %s\n", ctf_errmsg (ctf_errno (parent)));
	      exit (1);
	    }
	}
    }

  if (ctf_import (child, parent) < 0)
    goto import_err;

  /* Add some types that should end up with provisional IDs and be reassigned on
     writeout, with all references to them in all dicts following along.  */

  if (!empty_parent)
    {
      if ((pprovint = ctf_add_integer (parent, CTF_ADD_ROOT, "provint", &encoding)) == CTF_ERR)
	goto parent_add_err;
      first_child_type++;

      if ((pptr = ctf_add_pointer (parent, CTF_ADD_ROOT, pint)) == CTF_ERR)
	goto parent_add_err;
      first_child_type++;

      ar.ctr_contents = pint;
      ar.ctr_index = pint;
      ar.ctr_nelems = 666;

      if ((parray = ctf_add_array (parent, CTF_ADD_ROOT, &ar)) == CTF_ERR)
	goto parent_add_err;
      first_child_type++;

      func.ctc_argc = 2;
      func.ctc_flags = 0;
      func.ctc_return = pprovint;
      args[0] = pptr;
      args[1] = parray;

      if ((pfunction = ctf_add_function (parent, CTF_ADD_ROOT, &func, args)) == CTF_ERR)
	goto parent_add_err;
      first_child_type++;
    }

  if ((ctype = ctf_add_enum (child, CTF_ADD_ROOT, "wombat")) == CTF_ERR)
    goto child_add_err;

  if ((ctype2 = ctf_add_struct (child, CTF_ADD_ROOT, "foo")) == CTF_ERR)
    goto child_add_err;

  if (ctf_add_member (child, ctype2, "wombat_member", ctype) < 0)
    goto child_add_memb_err;

  if (!empty_parent)
    {
      /* pint is still valid: nonprovisional type.  */

      if (ctf_add_member (child, ctype2, "a", pint) < 0)
	goto child_add_memb_err;

      /* (pptr is provisional.) */

      if (ctf_add_member (child, ctype2, "b", pptr) < 0)
	goto child_add_memb_err;

      if ((cself = ctf_add_pointer (child, CTF_ADD_ROOT, ctype2)) == CTF_ERR)
	goto child_add_err;

      if (ctf_add_member (child, ctype2, "self", cself) < 0)
	goto child_add_memb_err;

      /* Make sure types are distinct.  */

      if (pint == pptr || pint == ctype || pint == ctype2 ||
	  pptr == ctype || pptr == ctype2 || ctype == ctype2)
	goto overlapping_err;

      if (pint > pptr || ctype > pptr || ctype2 > pptr)
	goto provisional_too_low_err;

      /* Add an instance of every other serialized type-referencing type,
	 referencing a type provisional in the parent.  */

      if (ctf_add_typedef (child, CTF_ADD_ROOT, "td", parray) == CTF_ERR)
	goto child_add_err;

      if ((cslice = ctf_add_slice (child, CTF_ADD_ROOT, pprovint, &slice_encoding)) == CTF_ERR)
	goto child_add_err;
	   
      if (ctf_add_member (child, ctype2, "c", cslice) < 0)
	goto child_add_memb_err;

      if (ctf_add_member (child, ctype2, "pfunc", pfunction) < 0)
	goto child_add_memb_err;

      func.ctc_argc = 2;
      func.ctc_flags = 0;
      func.ctc_return = pprovint;
      args[0] = pptr;
      args[1] = parray;

      if ((cfunction = ctf_add_function (parent, CTF_ADD_ROOT, &func, args)) == CTF_ERR)
	goto child_add_err;
      first_child_type++;

      if (ctf_add_member (child, ctype2, "cfunc", pfunction) < 0)
	goto child_add_memb_err;
    }

  /* Make sure we can't write out the child before the parent.  */
  if (!empty_parent)
    {
      if ((cbuf = ctf_write_mem (child, &csize, -1)) != NULL)
	{
	  fprintf (stderr, "writing child before parent works unexpectedly\n");
	  exit (1);
	}
      if (ctf_errno (child) != ECTF_NOTSERIALIZED)
	{
	  fprintf (stderr, "writing child before parent: unexpected error %s\n",
		   ctf_errmsg (ctf_errno (child)));
	  exit (1);
	}
    }

  /* Write out the parent, then the child: read both back in, reimport them,
     do some lookups, make sure they work.  Make sure we can't write the parent
     out if it was already serialized, unless it was empty when that happened */

  if (!empty_parent && !unserialized_parent)
    {
      if ((pbuf2 = ctf_write_mem (parent, &psize, -1)) != NULL)
	{
	  fprintf (stderr, "Writing out modified already-serialized parent succeeded unexpectedly\n");
	  exit (1);
	}

      /* Nothing more to test in this case.  */
      ctf_dict_close (child);
      ctf_dict_close (parent);
      free (pbuf);
      free (pbuf2);
      return 0;
    }

  if ((pbuf2 = ctf_write_mem (parent, &psize, -1)) == NULL)
    goto parent_write_err;

  if ((cbuf2 = ctf_write_mem (child, &csize, -1)) == NULL)
    goto child_write_err;

  ctf_dict_close (child);
  ctf_dict_close (parent);
  free (pbuf);
  free (cbuf);

  if ((parent = ctf_simple_open ((char *) pbuf2, psize, NULL, 0, 0, NULL, 0, &err)) == NULL)
    goto parent_open_err;

  if ((child = ctf_simple_open ((char *) cbuf2, csize, NULL, 0, 0, NULL, 0, &err)) == NULL)
    goto child_open_err;

  if (ctf_import (child, parent) < 0)
    goto import_err;

  if (!empty_parent)
    {
      ctf_id_t foo2;

      if ((foo = ctf_lookup_by_name (parent, "int")) == CTF_ERR)
	{
	  fprintf (stderr, "Cannot look up int in parent: %s\n", ctf_errmsg (ctf_errno (parent)));
	  exit (1);
	}

      if ((foo2 = ctf_lookup_by_name (child, "int")) == CTF_ERR)
	{
	  fprintf (stderr, "Cannot look up int in child: %s\n", ctf_errmsg (ctf_errno (parent)));
	  exit (1);
	}

      if (foo != foo2)
	{
	  fprintf (stderr, "int in parent and child have different IDs: %lx versus %lx\n", foo, foo2);
	  exit (1);
	}

      /* Verify that ctf_type_pointer still works: it saw changes as part of
	 the CTFv4 type ID rework.  In particular it works on parent types now
	 too.  */

      if ((foo2 = ctf_type_pointer (parent, foo)) == CTF_ERR)
	{
	  fprintf (stderr, "pointer lookup in parent failed: %s\n", ctf_errmsg (ctf_errno (parent)));
	  exit (1);
	}
      if (ctf_type_kind (parent, foo2) != CTF_K_POINTER)
	{
	  fprintf (stderr, "pointer lookup in parent yields non-pointer: %i\n", ctf_type_kind (parent, foo2));
	  exit (1);
	}
      if ((foo2 = ctf_type_pointer (child, foo)) == CTF_ERR)
	{
	  fprintf (stderr, "pointer lookup in child failed: %s\n", ctf_errmsg (ctf_errno (child)));
	  exit (1);
	}
      if (ctf_type_kind (child, foo2) != CTF_K_POINTER)
	{
	  fprintf (stderr, "pointer lookup in child yields non-pointer: %i\n", ctf_type_kind (child, foo2));
	  exit (1);
	}
    }

  if ((ctype = ctf_lookup_by_name (child, "enum wombat")) == CTF_ERR)
    {
      fprintf (stderr, "Cannot look up enum wombat in child: %s\n", ctf_errmsg (ctf_errno (child)));
      exit (1);
    }

  /* Check consecutiveness.  */

  if (ctype != first_child_type)
    {
      fprintf (stderr, "expected first child type to be ID %lx but is %lx\n", first_child_type, ctype);
      exit (1);
    }

  if ((ctype2 = ctf_lookup_by_name (child, "struct foo")) == CTF_ERR)
    {
      fprintf (stderr, "Cannot look up struct foo in child: %s\n", ctf_errmsg (ctf_errno (child)));
      exit (1);
    }
  
  /* Check consecutiveness.  */

  if (ctype2 != ctype + 1)
    {
      fprintf (stderr, "expected second child type to be ID %lx but is %lx\n", ctype + 1, ctype2);
      exit (1);
    }

  if (!empty_parent)
    {
      if ((ctypedef = ctf_lookup_by_name (child, "td")) == CTF_ERR)
	goto typedef_td_err;

      if ((parray = ctf_type_reference (child, ctypedef)) == CTF_ERR)
	goto typedef_err;

      if (ctf_array_info (child, parray, &ar) < 0)
	goto array_err;

      char *name;
      if ((name = ctf_type_aname (child, ar.ctr_contents)) == NULL)
	goto type_name_err;

      if (strcmp (name, "int") != 0)
	{
	  fprintf (stderr, "expected array member to be int, but was %s\n", name);
	  exit (1);
	}
      free (name);

      if (ar.ctr_contents != ar.ctr_index)
	{
	  fprintf (stderr, "array contents and index are not the same type: %lx versus %lx",
		   ar.ctr_contents, ar.ctr_index);
	  exit (1);
	}
    }

  /* Check membership links.  */

  if (ctf_member_info (child, ctype2, "wombat_member", &memb) < 0)
    goto memb_err;

  if (memb.ctm_type != ctype)
    {
      fprintf (stderr, "child enum member lookup yielded %lx, not %lx\n", memb.ctm_type, ctype);
      exit (1);
    }

  if (!empty_parent)
    {
      if (ctf_member_info (child, ctype2, "a", &memb) < 0)
	goto memb_err;

      if (ctf_type_kind (child, memb.ctm_type) != CTF_K_INTEGER)
	{
	  fprintf (stderr, "parent member integer lookup yielded %lx, not %x\n", memb.ctm_type, CTF_K_INTEGER);
	  exit (1);
	}

      if (ctf_member_info (child, ctype2, "b", &memb) < 0)
	goto memb_err;

      if (ctf_type_kind (child, memb.ctm_type) != CTF_K_POINTER)
	goto memb_ptr_err;

      if ((foo = ctf_type_reference (child, memb.ctm_type)) == CTF_ERR)
	goto memb_err;

      if ((ctf_type_kind (child, foo)) != CTF_K_INTEGER)
	{
	  fprintf (stderr, "parent member pointer final lookup yielded kind %x, not %x\n", ctf_type_kind (child, foo), CTF_K_INTEGER);
	  exit (1);
	}

      if (ctf_member_info (child, ctype2, "c", &memb) < 0)
	goto memb_err;

      if (ctf_type_encoding (child, memb.ctm_type, &out) < 0)
	goto encoding_err;

      if (memcmp (&out, &slice_encoding, sizeof (out)) != 0)
	{
	  fprintf (stderr, "slice encoding differs\n");
	  exit (1);
	}

      if (ctf_type_kind (child, memb.ctm_type) != CTF_K_INTEGER)
	{
	  fprintf (stderr, "parent member slice final lookup yielded kind %x, not %x\n", ctf_type_kind (child, memb.ctm_type), CTF_K_INTEGER);
	  exit (1);
	}

      if (ctf_member_info (child, ctype2, "pfunc", &memb) < 0)
	goto memb_err;

      if (ctf_type_kind (child, memb.ctm_type) != CTF_K_FUNCTION)
	goto func_err;
      pfunction = memb.ctm_type;

      if (ctf_member_info (child, ctype2, "cfunc", &memb) < 0)
	goto memb_err;

      if (ctf_type_kind (child, memb.ctm_type) != CTF_K_FUNCTION)
	goto func_err;
      cfunction = memb.ctm_type;

      if (ctf_func_type_info (child, pfunction, &pfunc) < 0 ||
	  ctf_func_type_info (child, cfunction, &cfunc) < 0)
	{
	  fprintf (stderr, "func info lookup failed: %s\n", ctf_errmsg (ctf_errno (child)));
	  exit (1);
	}

      if (memcmp (&pfunc, &cfunc, sizeof (pfunc)) != 0)
	{
	  fprintf (stderr, "parent and child funcs differ\n");
	  exit (1);
	}

      if (ctf_type_kind (child, pfunc.ctc_return) != CTF_K_INTEGER)
	{
	  fprintf (stderr, "func return type lookup yielded kind %x, not %x\n", ctf_type_kind (child, pfunc.ctc_return), CTF_K_INTEGER);
	  exit (1);
	}

      /* This isn't a type ID, so we're not really expecting problems here, but if
	 there are problems, rather an error message than a buffer overrun.  */
      if (pfunc.ctc_argc != 2)
	{
	  fprintf (stderr, "func has %i args, not 2\n", pfunc.ctc_argc);
	  exit (1);
	}

      if (ctf_func_type_args (child, pfunction, pfunc.ctc_argc, pargs) < 0 ||
	  ctf_func_type_args (child, cfunction, cfunc.ctc_argc, cargs) < 0)
	{
	  fprintf (stderr, "func arg lookup failed: %s\n", ctf_errmsg (ctf_errno (child)));
	  exit (1);
	}

      if (memcmp (pargs, cargs, sizeof (pargs)) != 0)
	{
	  fprintf (stderr, "parent and child func args differ\n");
	  exit (1);
	}
      if (ctf_type_kind (child, pargs[0]) != CTF_K_POINTER ||
	  ctf_type_kind (child, pargs[1]) != CTF_K_ARRAY)
	{
	  fprintf (stderr, "func args lookup not as expected\n");
	  exit (1);
	}

      if (ctf_member_info (child, ctype2, "self", &memb) < 0)
	goto memb_err;

      if (ctf_type_kind (child, memb.ctm_type) != CTF_K_POINTER)
	goto memb_ptr_err;

      if (ctf_type_reference (child, memb.ctm_type) != ctype2)
	{
	  fprintf (stderr, "structure self-ref yields type %lx, not %lx as expected\n",
		   ctf_type_reference (child, memb.ctm_type), ctype2);
	  exit (1);
	}
    }

  ctf_dict_close (child);
  ctf_dict_close (parent);
  free (cbuf2);
  free (pbuf2);

  return 0;

 create_err:
  fprintf (stderr, "Cannot create: %s\n", ctf_errmsg (err));
  exit (1);

 parent_write_err:
  fprintf (stderr, "Cannot serialize parent: %s\n", ctf_errmsg (ctf_errno (parent)));
  exit (1);

 child_write_err:
  fprintf (stderr, "Cannot serialize child: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 parent_open_err:
  fprintf (stderr, "Cannot open parent: %s\n", ctf_errmsg (err));
  exit (1);

 child_open_err:
  fprintf (stderr, "Cannot open chile: %s\n", ctf_errmsg (err));
  exit (1);

 import_err:
  fprintf (stderr, "Cannot import: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 parent_add_err:
  fprintf (stderr, "Cannot add parent types: %s\n", ctf_errmsg (ctf_errno (parent)));
  exit (1);

 child_add_err:
  fprintf (stderr, "Cannot add child types: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 child_add_memb_err:
  fprintf (stderr, "Cannot add child members: %s (%i)\n", ctf_errmsg (ctf_errno (child)), ctf_errno (child));
  exit (1);

 overlapping_err:
  fprintf (stderr, "type IDs overlap\n");
  exit (1);

 provisional_too_low_err:
  fprintf (stderr, "provisional ID %lx is too low\n", pptr);
  exit (1);

 memb_err:
  fprintf (stderr, "cannot look up members: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 memb_ptr_err:
  fprintf (stderr, "parent member pointer lookup yielded %lx, not %x\n", memb.ctm_type, CTF_K_POINTER);
  exit (1);

 typedef_td_err:
  fprintf (stderr, "Cannot look up typedef td in child: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 typedef_err:
  fprintf (stderr, "Cannot look up typedef array ref in child: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 array_err:
  fprintf (stderr, "Cannot look up array in child: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 type_name_err:
  fprintf (stderr, "Cannot look up type name in child: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 encoding_err:
  fprintf (stderr, "cannot get type encoding in child: %s\n", ctf_errmsg (ctf_errno (child)));
  exit (1);

 func_err: 
  fprintf (stderr, "parent member function final lookup yielded kind %x, not %x\n", ctf_type_kind (child, memb.ctm_type), CTF_K_FUNCTION);
  exit (1);
}

int main (void)
{
  test (1, 1);
  test (1, 0);
  test (0, 1);
  test (0, 0);

  printf ("All done.\n");
}
