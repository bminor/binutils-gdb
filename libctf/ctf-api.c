/* Miscellaneous dict-and library-wide API functions.
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
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#ifndef ENOTSUP
#define ENOTSUP ENOSYS
#endif

static int _libctf_version = CTF_VERSION;     /* Library client version.  */
static size_t _btf_hdr_len = sizeof (ctf_btf_header_t);
static int _libctf_debug = 0;		      /* Debugging messages enabled.  */

ctf_btf_mode_t _libctf_btf_mode = LIBCTF_BTM_POSSIBLE;	/* BTF writeout mode.  */

/* Set the CTF library client version to the specified version: this is the
   version of dicts written out by the ctf_write* functions.  If version is
   zero, we just return the default library version number.  The BTF version
   (for CTFv4 and above) is indicated via btf_hdr_len, also zero for "no
   change".

   btf_mode has three levels:

    - LIBCTF_BTM_ALWAYS writes out full-blown CTFv4 at all times
    - LIBCTF_BTM_POSSIBLE writes out CTFv4 if needed to avoid
      information loss, BTF otherwise (and always writes out CTFv4
      if compressing)
    - LIBCTF_BTM_BTF writes out BTF always, and errors otherwise (e.g.
      if compressing)

    You can influence what type kinds are written out to a CTFv4 dict via the
    ctf_write_suppress_kind() function.  */
int
ctf_version (int ctf_version_, size_t btf_hdr_len, ctf_btf_mode_t btf_mode)
{
  if (ctf_version_ < 0 || btf_mode < 0 || btf_mode > 2)
    {
      errno = EINVAL;
      return -1;
    }

  if (ctf_version_ > 0)
    {
      /*  Dynamic version switching is not presently supported. */
      if (ctf_version_ != _libctf_version)
	goto err;

      ctf_dprintf ("ctf_version: client using version %i\n", ctf_version_);
    }

  if (btf_hdr_len > 0)
    {
      /*  Dynamic version switching is not presently supported. */
      if (btf_hdr_len != _btf_hdr_len)
	goto err;

      ctf_dprintf ("ctf_version: client using BTF header length %zi\n", btf_hdr_len);
    }

  _libctf_btf_mode = btf_mode;

  return ctf_version_;

 err:
  errno = ENOTSUP;
  return -1;
}

/* Store the specified error code into errp if it is non-NULL, and then
   return NULL for the benefit of the caller.  */

void *
ctf_set_open_errno (ctf_error_t *errp, ctf_error_t error)
{
  if (errp != NULL)
    *errp = error;
  return NULL;
}

/* See ctf-inlines.h.  */

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
ctf_ret_t
ctf_set_errno (ctf_dict_t *fp, ctf_error_t err)
{
  fp->ctf_errno = err;
  /* Don't rely on CTF_ERR here as it will not properly sign extend on 64-bit
     Windows ABI.  */
  return -1;
}

ctf_id_t
ctf_set_typed_errno (ctf_dict_t *fp, ctf_error_t err)
{
  fp->ctf_errno = err;
  return CTF_ERR;
}
#endif

/* Get and set CTF dict-wide flags.  We are fairly strict about returning
   errors here, to make it easier to determine programmatically which flags are
   valid.  */

ctf_ret_t
ctf_dict_set_flag (ctf_dict_t *fp, ctf_dict_flags_t flag, int set)
{
  if (set < 0 || set > 1)
    return (ctf_set_errno (fp, ECTF_BADFLAG));

  switch (flag)
    {
    case CTF_STRICT_NO_DUP_ENUMERATORS:
      if (set)
	fp->ctf_flags |= LCTF_STRICT_NO_DUP_ENUMERATORS;
      else
	fp->ctf_flags &= ~LCTF_STRICT_NO_DUP_ENUMERATORS;
      break;
    default:
      return (ctf_set_errno (fp, ECTF_BADFLAG));
    }
  return 0;
}

ctf_bool_t
ctf_dict_flag (ctf_dict_t *fp, ctf_dict_flags_t flag)
{
  switch (flag)
    {
    case CTF_STRICT_NO_DUP_ENUMERATORS:
      return (fp->ctf_flags & LCTF_STRICT_NO_DUP_ENUMERATORS) != 0;
    default:
      return (ctf_set_errno (fp, ECTF_BADFLAG));
    }
  return 0;
}

void
libctf_init_debug (void)
{
  static int inited;
  if (!inited)
    {
      _libctf_debug = getenv ("LIBCTF_DEBUG") != NULL;
      inited = 1;
    }
}

void ctf_setdebug (ctf_bool_t debug)
{
  /* Ensure that libctf_init_debug() has been called, so that we don't get our
     debugging-on-or-off smashed by the next call.  */

  libctf_init_debug();
  _libctf_debug = (debug != 0);
  ctf_dprintf ("CTF debugging set to %i\n", debug);
}

_libctf_printflike_ (1, 2)
void ctf_dprintf (const char *format, ...)
{
  if (_libctf_unlikely_ (_libctf_debug))
    {
      va_list alist;

      va_start (alist, format);
      fflush (stdout);
      (void) fputs ("libctf DEBUG: ", stderr);
      (void) vfprintf (stderr, format, alist);
      va_end (alist);
    }
}

/* This needs more attention to thread-safety later on.  */
static ctf_list_t open_errors;

ctf_err_locus_t
ctf_err_locus_internal (ctf_dict_t *err_fp, ctf_dict_t *type_fp, int input_num,
			ctf_id_t type, const char *func)
{
  ctf_err_locus_t l;
  l.err_fp = err_fp;
  l.type_fp = type_fp;
  l.input_num = input_num;
  l.type = type;
  l.func = func;
  return l;
}

/* Errors and warnings.  Reports warnings or errors to lists in a particular
   error fp (or the open errors list if NULL), associated with a given ERR and
   possibly type fp and type ID.

   Lowest-level function: do not call directly.  */
_libctf_printflike_ (4, 0)
static void
ctf_errwarn_internal (ctf_err_locus_t l, int is_warning, ctf_error_t err,
		      const char *format, va_list alist)
{
  char *text;
  char *msg = NULL;
  char *input_str = NULL;
  const char *cuname = ".ctf", *errmsg = NULL;
  ctf_err_warning_t *cew;

  /* When linking, use ctf_link_input_name() to get the name of the linker
     input, not a likely-useless name like ".ctf".  */
  if (l.type_fp != l.err_fp)
    cuname = ctf_link_input_name (l.type_fp);
  if (l.type_fp && ctf_dict_cuname (l.type_fp))
    cuname = ctf_dict_cuname (l.type_fp);

  /* If the link input number is given, turn it into a string.  If this fails,
     just don't print it: it's a nicety.  */
  if (l.input_num >= 0)
    if (asprintf (&input_str, "link input %i:", l.input_num) < 0)
      input_str = NULL;

  if (!cuname)
    cuname = "unnamed-CU";

  /* Two fps may be provided, the 'err fp' on which the error message is wanted,
     and the 'type fp' on which the type was found.  (This is usually used
     during linking and deduplication, when errors often appear on input fps
     but the actual error is only going to be seen if it's on the output fp.)

     If this is not a warning, the error passed in or the error on the type fp
     can be automatically derived from the type fp if it is already set on there,
     and propagated to the err fp.  Warnings are presumed unlikely to be
     associated with errors on the fp, and so they are left untouched.  */
  if (!is_warning)
    {
      if (l.type_fp && err == 0 && ctf_errno (l.type_fp) != 0)
	err = ctf_errno (l.type_fp);

      if (l.err_fp && err != 0
	  && (l.err_fp != l.type_fp || ctf_errno (l.err_fp) == 0))
	ctf_set_errno (l.err_fp, err);
    }

  errmsg = ctf_errmsg (err);

  if (format)
    if (vasprintf (&msg, format, alist) < 0)
      return;

  /* Type 0 == "not related to any particular type".  */
  if (l.type_fp && l.type != 0)
    {
      if (asprintf (&text, "%s(%s\"%s\", 0x%lx): %s%s%s",
		    l.func, input_str ? input_str : "", cuname,
		    (unsigned long) l.type,
		    msg ? msg : "", msg && err ? ": " : "",
		    err ? "" : errmsg) < 0)
	{
	  free (msg);
	  return;
	}
    }
  else if (l.type_fp && l.type == 0)
    {
      if (asprintf (&text, "%s(%s\"%s\"): %s%s%s",
		    l.func, input_str ? input_str : "", cuname,
		    msg ? msg : "", msg && err ? ": " : "",
		    err ? "" : errmsg) < 0)
	{
	  free (msg);
	  return;
	}
    }
  else
    {
      /* No fp, no type: just decorate with func/error/warning.  */

      if (asprintf (&text, "%s(): %s%s%s",
		    l.func,
		    msg ? msg : "", msg && err ? ": " : "",
		    err ? "" : errmsg) < 0)
	{
	  free (msg);
	  return;
	}
    }

  ctf_dprintf ("%s: %s\n", is_warning ? _("warning: "): _("error: "), text);

  /* Don't bother reporting errors here: we can't do much about them if they
     happen.  */

  if ((cew = malloc (sizeof (ctf_err_warning_t))) == NULL)
    return;

  cew->cew_is_warning = is_warning;
  cew->cew_err = err;
  cew->cew_text = text;

  if (l.err_fp != NULL)
    ctf_list_append (&l.err_fp->ctf_errs_warnings, cew);
  else
    ctf_list_append (&open_errors, cew);

  free (msg);
}

/* Emit a warning or error associated with a particular fp / type / function
   (see err_locus) into the error stream for that fp (or the global stream if fp
   is NULL).  */

_libctf_printflike_ (3, 4)
ctf_ret_t
ctf_err (ctf_err_locus_t l, ctf_error_t err, const char *format, ...)
{
  va_list alist;

  va_start (alist, format);
  ctf_errwarn_internal (l, 0, err, format, alist);
  va_end (alist);
  return -1;
}

/* Like ctf_err, but returns a ctf_id_t error value.  */
_libctf_printflike_ (3, 4)
ctf_id_t
ctf_typed_err (ctf_err_locus_t l, ctf_error_t err, const char *format, ...)
{
  va_list alist;

  va_start (alist, format);
  ctf_errwarn_internal (l, 0, err, format, alist);
  va_end (alist);
  return CTF_ERR;
}

_libctf_printflike_ (3, 4)
void
ctf_warn (ctf_err_locus_t l, ctf_error_t err, const char *format, ...)
{
  va_list alist;

  va_start (alist, format);
  ctf_errwarn_internal (l, 1, err, format, alist);
  va_end (alist);
}

/* Move all the errors/warnings from an fp into the open_errors.  */
void
ctf_err_warn_to_open (ctf_dict_t *fp)
{
  ctf_list_splice (&open_errors, &fp->ctf_errs_warnings);
}

/* Copy all the errors/warnings from one fp to another one, and the error code
   as well.  */
void
ctf_err_copy (ctf_dict_t *dest, ctf_dict_t *src)
{
  ctf_err_warning_t *cew;
  for (cew = ctf_list_next (&src->ctf_errs_warnings); cew != NULL;
       cew = ctf_list_next (cew))
    {
      ctf_err_warning_t *new_cew;

      if ((new_cew = malloc (sizeof (ctf_err_warning_t))) == NULL)
	goto oom;
      memcpy (new_cew, cew, sizeof (ctf_err_warning_t));
      if ((new_cew->cew_text = strdup (new_cew->cew_text)) == NULL)
	{
	  free (new_cew);
	  goto oom;
	}
      ctf_list_append (&dest->ctf_errs_warnings, new_cew);
    }
  ctf_set_errno (dest, ctf_errno (src));
  return;
 oom:
  ctf_set_errno (dest, ENOMEM);
}

/* Upgrade the most recent warning or error to an error.  */
void
ctf_err_upgrade (ctf_dict_t *fp)
{
  ctf_err_warning_t *cew = ctf_list_prev (&fp->ctf_errs_warnings);

  if (cew && cew->cew_is_warning)
    {
      cew->cew_is_warning = 0;
      if (cew->cew_err != 0)
	ctf_set_errno (fp, cew->cew_err);
    }
}

/* Error-warning reporting: an 'iterator' that returns errors and warnings from
   the error/warning list, in order of emission.  Errors and warnings are popped
   after return: the caller must free the returned error-text pointer.  The
   optional errp argument is overwritten with the associated error code, if
   non-NULL: it is also used to report errors with this function itself.

   An fp of NULL returns CTF-open-time errors from the open_errors variable
   above.

   The treatment of errors from this function itself is somewhat unusual: it
   will often be called on an error path, so we don't want to overwrite the
   ctf_errno unless we have no choice.  So the errp argument is preferentially
   used to store errors, if it is provided.  The pointer is optional: if not
   set, errors are reported via the fp (if non-NULL).  Calls with neither fp nor
   errp set are mildly problematic because there is no clear way to report
   end-of-iteration: you just have to assume that a NULL return means the end,
   and not an iterator error.

   ERRP is also used to report the error code associated with a reported
   error; error returns from this function always return NULL and set ERRP;
   non-error returns never return NULL.  */

char *
ctf_errwarning_next (ctf_dict_t *fp, ctf_next_t **it, int *is_warning,
		     ctf_error_t *errp)
{
  ctf_next_t *i = *it;
  char *ret;
  ctf_list_t *errlist;
  ctf_err_warning_t *cew;
  ctf_error_t err;
  char *prepended_text;

  if (fp)
    errlist = &fp->ctf_errs_warnings;
  else
    errlist = &open_errors;

  if (!i)
    {
      if ((i = ctf_next_create ()) == NULL)
	{
	  if (errp)
	    *errp = ENOMEM;
	  else if (fp)
	    ctf_set_errno (fp, ENOMEM);
	  return NULL;
	}

      i->cu.ctn_fp = fp;
      i->ctn_iter_fun = (void (*) (void)) ctf_errwarning_next;
      *it = i;
    }

  if ((void (*) (void)) ctf_errwarning_next != i->ctn_iter_fun)
    {
      err = ECTF_NEXT_WRONGFUN;
      goto end;
    }

  if (fp != i->cu.ctn_fp)
    {
      err = ECTF_NEXT_WRONGFP;
      goto end;
    }

  cew = ctf_list_next (errlist);

  if (!cew)
    {
      err = ECTF_NEXT_END;
      goto end;
    }

  if (is_warning)
    *is_warning = cew->cew_is_warning;
  if (errp)
    *errp = cew->cew_err;

  /* Augment the string with an error/warning indication, if there is memory.
     (We do it this late to allow for ctf_err_upgrade().)  */
  if ((asprintf (&prepended_text, "%s%s\n", is_warning ? _("warning: "): _("error: "),
		 cew->cew_text)) < 0)
    ret = cew->cew_text;
  else
    {
      ret = prepended_text;
      free (cew->cew_text);
    }
  ctf_list_delete (errlist, cew);
  free (cew);
  return ret;

 end:
  ctf_next_destroy (i);
  *it = NULL;
  if (errp)
    *errp = err;
  else if (fp)
    ctf_set_errno (fp, err);
  return NULL;
}

/* Remove the most recent instance of some error (that you have already
   handled and do not want reported) from the errwarning stream.  If no
   such error exists, does nothing.  */

void
ctf_errwarning_remove (ctf_dict_t *fp, int err)
{
  ctf_list_t *errlist;
  ctf_err_warning_t *cew;

  if (fp)
    errlist = &fp->ctf_errs_warnings;
  else
    errlist = &open_errors;

  for (cew = ctf_list_prev (errlist); cew != NULL; cew = ctf_list_prev (cew))
    {
      if (cew->cew_err == err)
	{
	  ctf_list_delete (errlist, cew);
	  free (cew->cew_text);
	  free (cew);
	  return;
	}
    }
}

void
ctf_assert_fail_internal (ctf_dict_t *fp, const char *file, size_t line,
			  const char *func, const char *exprstr)
{
  ctf_err (ctf_err_locus_internal (fp, fp, -1, 0, func), ECTF_INTERNAL,
	   _("%s: %lu: libctf assertion failed: %s"),
	   file, (long unsigned int) line, exprstr);
}
