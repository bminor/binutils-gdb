/* Make sure that errors are propagated properly from parent dicts to children
   when errors are encountered in child functions that can recurse to parents.

   We check specifically a subset of known-buggy functions.
   Functions that require a buggy linker to expose, or that only fail on
   assertion-failure-incurring corrupted dicts, are not tested.

   This is very much a whitebox test: we do things no legitimate client should
   do to be sure that we can get invalid type ID errors that you are normally
   blocked from getting.  */

#include "config.h"
#include <ctf-api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctf-impl.h"

static const char *desc;

static void
check_prop_err (ctf_dict_t *child, ctf_dict_t *parent, int expected)
{
  if (ctf_errno (child) == expected)
    return;

  if (ctf_errno (parent) == expected)
    {
      fprintf (stderr, "%s: error propagation failure: error \"%s\" not seen on child, "
	       "but instead on parent\n", desc, ctf_errmsg (ctf_errno (parent)));
      exit (1);
    }
  else
    {
      fprintf (stderr, "%s: expected error is entirely lost: "
	       "\"%s\" seen on parent, \"%s\" on child\n", desc,
	       ctf_errmsg (ctf_errno (parent)),
	       ctf_errmsg (ctf_errno (child)));
      exit (1);
    }
}

static void
no_prop_err (void)
{
  fprintf (stderr, "%s: expected error return not observed.\n", desc);
}

int main (void)
{
  ctf_dict_t *parent;
  ctf_dict_t *child;
  ctf_dict_t *wrong;
  ctf_dict_t *blank;
  ctf_dict_t tmp;
  ctf_id_t void_id;
  ctf_id_t wrong_id;
  ctf_id_t base;
  ctf_id_t slice;
  ctf_id_t function;
  ctf_id_t ptr;
  ctf_encoding_t long_encoding = { CTF_INT_SIGNED, 0, sizeof (long) };
  ctf_encoding_t void_encoding = { CTF_INT_SIGNED, 0, 0 };
  ctf_encoding_t foo;
  ctf_funcinfo_t fi;
  ctf_id_t bar;
  int err;

  if ((parent = ctf_create (&err)) == NULL
      || (child = ctf_create (&err)) == NULL
      || (wrong = ctf_create (&err)) == NULL
      || (blank = ctf_create (&err)) == NULL)
    {
      fprintf (stderr, "Cannot create dicts: %s\n", ctf_errmsg (err));
      return 1;
    }

  if ((ctf_import (child, parent)) < 0)
    {
      fprintf (stderr, "cannot import: %s\n", ctf_errmsg (ctf_errno (child)));
      return 1;
    }

  if ((ctf_import (blank, wrong)) < 0)
    {
      fprintf (stderr, "cannot import wrong-types dict: %s\n", ctf_errmsg (ctf_errno (blank)));
      return 1;
    }

  /* Populate two dicts, one with the same types in a different order: both have
     children, to ensure that all types in both dicts are provisional and have
     the same IDs.  This passes all ctf_import checks (type and strtab count),
     but will still induce errors due to type mismatches with the child.  In
     particular, base in the right parent is a non-integral type (a pointer) in
     the wrong one, and "void" in the parent is an unknown type in the wrong
     one.  */

  if ((ctf_add_unknown (parent, CTF_ADD_ROOT, "spacer")) /* 1 */
      == CTF_ERR)
    goto parent_err;

  if ((ctf_add_unknown (parent, CTF_ADD_ROOT, "spacer2")) /* 2 */
      == CTF_ERR)
    goto parent_err;

  if ((void_id = ctf_add_integer (parent, CTF_ADD_ROOT, "void", &void_encoding)) /* 3 */
      == CTF_ERR)
    goto parent_err;

  if ((base = ctf_add_integer (parent, CTF_ADD_ROOT, "long int", &long_encoding)) /* 4 */
      == CTF_ERR)
    goto parent_err;

  if ((ptr = ctf_add_pointer (parent, CTF_ADD_ROOT, void_id)) /* 5 */
      == CTF_ERR)
    goto parent_err;

  if ((ctf_add_integer (wrong, CTF_ADD_ROOT, "long int", &long_encoding)) /* 1 */
      == CTF_ERR)
    goto parent_err;

  if ((wrong_id = ctf_add_integer (wrong, CTF_ADD_ROOT, "void", &void_encoding)) /* 2 */
      == CTF_ERR)
    goto parent_err;

  if ((ptr = ctf_add_pointer (wrong, CTF_ADD_ROOT, wrong_id)) /* 3 */
      == CTF_ERR)
    goto parent_err;
  
  if ((ctf_add_unknown (wrong, CTF_ADD_ROOT, "spacer")) /* 4 */
      == CTF_ERR)
    goto parent_err;

  if ((ctf_add_unknown (wrong, CTF_ADD_ROOT, "spacer2")) /* 5 */
      == CTF_ERR)
    goto parent_err;

  foo.cte_format = 0;
  foo.cte_bits = 4;
  foo.cte_offset = 4;
  if ((slice = ctf_add_slice (child, CTF_ADD_ROOT, base, &foo)) == CTF_ERR)
    goto parent_err;

  /* Same name as a type: no change in strtab.strlen.  */
  if (ctf_add_variable (parent, "base", base) < 0)
    goto child_err;

  fi.ctc_return = void_id;
  fi.ctc_argc = 0;
  fi.ctc_flags = 0;
  if ((function = ctf_add_function (child, CTF_ADD_ROOT, &fi, NULL)) == CTF_ERR)
    goto child_err;

  desc = "func info lookup of non-function";
  if ((ctf_func_type_info (child, base, &fi)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NOTFUNC);

  desc = "func args lookup of non-function";
  if ((ctf_func_type_args (child, base, 0, &bar)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NOTFUNC);

  /* Swap the insides of "parent" and "wrong" so we get a parent dict with
     different types than it had.  */

  memcpy (&tmp, parent, sizeof (ctf_dict_t));
  memcpy (parent, wrong, sizeof (ctf_dict_t));
  memcpy (wrong, &tmp, sizeof (ctf_dict_t));

  /* This is testing ctf_type_resolve_unsliced(), which is called by the enum
     functions (which are not themselves buggy).  This type isn't an enum, but
     that's OK: we're after an error, after all.  */

  desc = "child slice resolution";
  if ((ctf_enum_value (child, slice, "foo", NULL)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NONREPRESENTABLE);

  desc = "child slice encoding lookup";
  if ((ctf_type_encoding (child, slice, &foo)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NONREPRESENTABLE);

  desc = "func info lookup of nonrepresentable function";
  if ((ctf_func_type_info (child, base, &fi)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NONREPRESENTABLE);

  desc = "func args lookup of nonrepresentable function";
  if ((ctf_func_type_args (child, base, 0, &bar)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NONREPRESENTABLE);

  desc = "child slice addition";
  if ((slice = ctf_add_slice (child, CTF_ADD_ROOT, base, &foo)) != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NONREPRESENTABLE);

  desc = "variable lookup";
  if (ctf_lookup_variable (child, "base") != CTF_ERR)
    no_prop_err ();
  check_prop_err (child, parent, ECTF_NOTYPEDAT);

  ctf_dict_close (child);
  ctf_dict_close (parent);
  ctf_dict_close (wrong);
  fprintf (stderr, "All done.\n");
  return 0;

 parent_err:
  fprintf (stderr, "cannot populate parent: %s\n", ctf_errmsg (ctf_errno (parent)));
  return 1;

 child_err:
  fprintf (stderr, "cannot populate child: %s\n", ctf_errmsg (ctf_errno (parent)));
  return 1;

}
