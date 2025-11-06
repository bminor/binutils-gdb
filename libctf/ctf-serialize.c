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
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

#include <elf.h>
#include "elf-bfd.h"

#include <ctf-ref.h>

/* Functions in this file are roughly divided into two types: sizing functions,
   which work out the size of various structures in the final serialized
   representation, and emission functions that actually emit data into them.

   When the sizing functions are called, the buffer into which the output will
   be serialized has not yet been created: so no functions which create
   references into that buffer (notably, ctf_*_add_ref) should be called.

   This requirement is to some degree enforced by ctf_assert calls.  */

/* Symtypetab sections.  */

/* Symtypetab emission flags.  */

#define CTF_SYMTYPETAB_EMIT_FUNCTION 0x1
#define CTF_SYMTYPETAB_EMIT_PAD 0x2
#define CTF_SYMTYPETAB_FORCE_INDEXED 0x4

/* Properties of symtypetab emission, shared by symtypetab section
   sizing and symtypetab emission itself.  */

typedef struct emit_symtypetab_state
{
  /* True if linker-reported symbols are being filtered out.  symfp is set if
     this is true: otherwise, indexing is forced and the symflags indicate as
     much. */
  int filter_syms;

  /* True if symbols are being sorted.  */
  int sort_syms;

  /* Flags for symtypetab emission.  */
  int symflags;

  /* The dict to which the linker has reported symbols.  */
  ctf_dict_t *symfp;

  /* The maximum number of objects seen.  */
  size_t maxobjt;

  /* The maximum number of func info entris seen.  */
  size_t maxfunc;
} emit_symtypetab_state_t;

/* Emit a ref to a type in this dict.  As with string refs, this ref can be
   updated later on to change the type ID recorded in this location.  The ref
   may not be emitted if the value is already known and cannot change.

   All refs must point within the ctf_serialize.cs_buf.  */

static int
ctf_type_add_ref (ctf_dict_t *fp, uint32_t *ref)
{
  ctf_dtdef_t *dtd;

  /* Type in the static portion: cannot change, value already correct.  */
  if (*ref <= fp->ctf_stypes)
    return 0;

  dtd = ctf_dtd_lookup (fp, *ref);

  if (!ctf_assert (fp, dtd))
    return 0;

  if (!ctf_assert (fp, fp->ctf_serialize.cs_buf != NULL
		   && (unsigned char *) ref > fp->ctf_serialize.cs_buf
		   && (unsigned char *) ref < fp->ctf_serialize.cs_buf + fp->ctf_serialize.cs_buf_size))
    return -1;

  /* Simple case: final ID different from what is recorded, but already known.
     Just set it.  */
  if (dtd->dtd_final_type)
    *ref = dtd->dtd_final_type;
  /* Otherwise, create a ref to it so we can set it later.  */
  else if (!ctf_create_ref (fp, &dtd->dtd_refs, ref))
    return (ctf_set_errno (fp, ENOMEM));

  return 0;
}

/* Purge all refs to this dict's dynamic types (all refs added by
   ctf_type_add_ref while serializing this dict).  */
static void
ctf_type_purge_refs (ctf_dict_t *fp)
{
  ctf_dtdef_t *dtd;

  for (dtd = ctf_list_next (&fp->ctf_dtdefs); dtd != NULL;
       dtd = ctf_list_next (dtd))
    ctf_purge_ref_list (fp, &dtd->dtd_refs);
}

/* Determine if a symbol is "skippable" and should never appear in the
   symtypetab sections.  */

int
ctf_symtab_skippable (ctf_link_sym_t *sym)
{
  /* Never skip symbols whose name is not yet known.  */
  if (sym->st_nameidx_set)
    return 0;

  return (sym->st_name == NULL || sym->st_name[0] == 0
	  || sym->st_shndx == SHN_UNDEF
	  || strcmp (sym->st_name, "_START_") == 0
	  || strcmp (sym->st_name, "_END_") == 0
	  || strcmp (sym->st_name, "_DYNAMIC") == 0
	  || strcmp (sym->st_name, "_GLOBAL_OFFSET_TABLE_") == 0
	  || strcmp (sym->st_name, "_PROCEDURE_LINKAGE_TABLE_") == 0
	  || strcmp (sym->st_name, "_edata") == 0
	  || strcmp (sym->st_name, "_end") == 0
	  || strcmp (sym->st_name, "_etext") == 0
	  || (sym->st_type == STT_OBJECT && sym->st_shndx == SHN_EXTABS
	      && sym->st_value == 0));
}

/* Get the number of symbols in a symbol hash, the count of symbols, the maximum
   seen, the eventual size, without any padding elements, of the func/data and
   (if generated) index sections, and the size of accumulated padding elements.
   The linker-reported set of symbols is found in SYMFP: it may be NULL if
   symbol filtering is not desired, in which case CTF_SYMTYPETAB_FORCE_INDEXED
   will always be set in the flags.

   Also figure out if any symbols need to be moved to the variable section, and
   add them (if not already present).

   This is a sizing function, called before the output buffer is
   constructed.  Do not add any refs in this function!  */

_libctf_nonnull_ ((1,3,4,5,6,7,8))
static int
symtypetab_density (ctf_dict_t *fp, ctf_dict_t *symfp, ctf_dynhash_t *symhash,
		    size_t *count, size_t *max, size_t *unpadsize,
		    size_t *padsize, size_t *idxsize, int flags)
{
  ctf_next_t *i = NULL;
  const void *name;
  const void *ctf_sym;
  ctf_dynhash_t *linker_known = NULL;
  ctf_error_t err;
  int beyond_max = 0;

  *count = 0;
  *max = 0;
  *unpadsize = 0;
  *idxsize = 0;
  *padsize = 0;

  if (!(flags & CTF_SYMTYPETAB_FORCE_INDEXED))
    {
      /* Make a dynhash citing only symbols reported by the linker of the
	 appropriate type, then traverse all potential-symbols we know the types
	 of, removing them from linker_known as we go.  Once this is done, the
	 only symbols remaining in linker_known are symbols we don't know the
	 types of: we must emit pads for those symbols that are below the
	 maximum symbol we will emit (any beyond that are simply skipped).

	 If there are none, this symtypetab will be empty: just report that.  */

      if (!symfp->ctf_dynsyms)
	return 0;

      if ((linker_known = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
					      NULL, NULL)) == NULL)
	return (ctf_set_errno (fp, ENOMEM));

      while ((err = ctf_dynhash_cnext (symfp->ctf_dynsyms, &i,
				       &name, &ctf_sym)) == 0)
	{
	  ctf_link_sym_t *sym = (ctf_link_sym_t *) ctf_sym;

	  if (((flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
	       && sym->st_type != STT_FUNC)
	      || (!(flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
		  && sym->st_type != STT_OBJECT))
	    continue;

	  if (ctf_symtab_skippable (sym))
	    continue;

	  /* This should only be true briefly before all the names are
	     finalized, long before we get this far.  */
	  if (!ctf_assert (fp, !sym->st_nameidx_set))
	    {
	      ctf_dynhash_destroy (linker_known);
	      ctf_next_destroy (i);
	      return -1;			/* errno is set for us.  */
	    }

	  if (ctf_dynhash_cinsert (linker_known, name, ctf_sym) < 0)
	    {
	      ctf_dynhash_destroy (linker_known);
	      return (ctf_set_errno (fp, ENOMEM));
	    }
	}
      if (err != ECTF_NEXT_END)
	{
	  ctf_err_warn (fp, 0, err, _("iterating over linker-known symbols during "
				  "serialization"));
	  ctf_dynhash_destroy (linker_known);
	  return (ctf_set_errno (fp, err));
	}
    }

  while ((err = ctf_dynhash_cnext (symhash, &i, &name, NULL)) == 0)
    {
      ctf_link_sym_t *sym;

      if (!(flags & CTF_SYMTYPETAB_FORCE_INDEXED))
	{
	  /* Linker did not report symbol in symtab.  Remove it from the
	     set of known data symbols and continue.  */
	  if ((sym = ctf_dynhash_lookup (symfp->ctf_dynsyms, name)) == NULL)
	    {
	      ctf_dynhash_remove (symhash, name);
	      continue;
	    }

	  /* We don't remove skippable symbols from the symhash because we don't
	     want them to be migrated into variables.  */
	  if (ctf_symtab_skippable (sym))
	    continue;

	  if ((flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
	      && sym->st_type != STT_FUNC)
	    {
	      ctf_err_warn (fp, 1, 0, _("symbol %s (%x) added to CTF as a "
					"function but is of type %x.  "
					"The symbol type lookup tables "
					"are probably corrupted"),
			    sym->st_name, sym->st_symidx, sym->st_type);
	      ctf_dynhash_remove (symhash, name);
	      continue;
	    }
	  else if (!(flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
		   && sym->st_type != STT_OBJECT)
	    {
	      ctf_err_warn (fp, 1, 0, _("symbol %s (%x) added to CTF as a "
					"data object but is of type %x.  "
					"The symbol type lookup tables "
					"are probably corrupted"),
			    sym->st_name, sym->st_symidx, sym->st_type);
	      ctf_dynhash_remove (symhash, name);
	      continue;
	    }

	  ctf_dynhash_remove (linker_known, name);

	  if (*max < sym->st_symidx)
	    *max = sym->st_symidx;
	}
      else
	(*max)++;

      *unpadsize += sizeof (uint32_t);
      (*count)++;
    }
  if (err != ECTF_NEXT_END)
    {
      ctf_err_warn (fp, 0, err, _("iterating over CTF symtypetab during "
				  "serialization"));
      ctf_dynhash_destroy (linker_known);
      return (ctf_set_errno (fp, err));
    }

  if (!(flags & CTF_SYMTYPETAB_FORCE_INDEXED))
    {
      while ((err = ctf_dynhash_cnext (linker_known, &i, NULL, &ctf_sym)) == 0)
	{
	  ctf_link_sym_t *sym = (ctf_link_sym_t *) ctf_sym;

	  if (sym->st_symidx > *max)
	    beyond_max++;
	}
      if (err != ECTF_NEXT_END)
	{
	  ctf_err_warn (fp, 0, err, _("iterating over linker-known symbols "
				      "during CTF serialization"));
	  ctf_dynhash_destroy (linker_known);
	  return (ctf_set_errno (fp, err));
	}
    }

  *idxsize = *count * sizeof (uint32_t);
  if (!(flags & CTF_SYMTYPETAB_FORCE_INDEXED))
    *padsize = (ctf_dynhash_elements (linker_known) - beyond_max) * sizeof (uint32_t);

  ctf_dynhash_destroy (linker_known);
  return 0;
}

/* Emit an objt or func symtypetab into DP in a particular order defined by an
   array of ctf_link_sym_t or symbol names passed in.  The index has NIDX
   elements in it: unindexed output would terminate at symbol OUTMAX and is in
   any case no larger than SIZE bytes.  Some index elements are expected to be
   skipped: see symtypetab_density.  The linker-reported set of symbols (if any)
   is found in SYMFP.

   Note down type ID refs as we go.  */

static int
emit_symtypetab (ctf_dict_t *fp, ctf_dict_t *symfp, uint32_t *dp,
		 ctf_link_sym_t **idx, const char **nameidx, uint32_t nidx,
		 uint32_t outmax, int size, int flags)
{
  uint32_t i;
  uint32_t *dpp = dp;
  ctf_dynhash_t *symhash;

  ctf_dprintf ("Emitting table of size %i, outmax %u, %u symtypetab entries, "
	       "flags %i\n", size, outmax, nidx, flags);

  /* Empty table? Nothing to do.  */
  if (size == 0)
    return 0;

  if (flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
    symhash = fp->ctf_funchash;
  else
    symhash = fp->ctf_objthash;

  for (i = 0; i < nidx; i++)
    {
      const char *sym_name;
      void *type;

      /* If we have a linker-reported set of symbols, we may be given that set
	 to work from, or a set of symbol names.  In both cases we want to look
	 at the corresponding linker-reported symbol (if any).  */
      if (!(flags & CTF_SYMTYPETAB_FORCE_INDEXED))
	{
	  ctf_link_sym_t *this_link_sym;

	  if (idx)
	    this_link_sym = idx[i];
	  else
	    this_link_sym = ctf_dynhash_lookup (symfp->ctf_dynsyms, nameidx[i]);

	  /* Unreported symbol number.  No pad, no nothing.  */
	  if (!this_link_sym)
	    continue;

	  /* Symbol of the wrong type, or skippable?  This symbol is not in this
	     table.  */
	  if (((flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
	       && this_link_sym->st_type != STT_FUNC)
	      || (!(flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
		  && this_link_sym->st_type != STT_OBJECT))
	    continue;

	  if (ctf_symtab_skippable (this_link_sym))
	    continue;

	  sym_name = this_link_sym->st_name;

	  /* Linker reports symbol of a different type to the symbol we actually
	     added?  Skip the symbol.  No pad, since the symbol doesn't actually
	     belong in this table at all.  (Warned about in
	     symtypetab_density.)  */
	  if ((this_link_sym->st_type == STT_FUNC)
	      && (ctf_dynhash_lookup (fp->ctf_objthash, sym_name)))
	    continue;

	  if ((this_link_sym->st_type == STT_OBJECT)
	      && (ctf_dynhash_lookup (fp->ctf_funchash, sym_name)))
	    continue;
	}
      else
	sym_name = nameidx[i];

      /* Symbol in index but no type set? Silently skip and (optionally)
	 pad.  (In force-indexed mode, this is also where we track symbols of
	 the wrong type for this round of insertion.)  */
      if ((type = ctf_dynhash_lookup (symhash, sym_name)) == NULL)
	{
	  if (flags & CTF_SYMTYPETAB_EMIT_PAD)
	    *dpp++ = 0;
	  continue;
	}

      if (!ctf_assert (fp, (((char *) dpp) - (char *) dp) < size))
	return -1;				/* errno is set for us.  */

      *dpp = (ctf_id_t) (uintptr_t) type;
      if (ctf_type_add_ref (fp, dpp++) < 0)
	return -1;				/* errno is set for us.  */

      /* When emitting unindexed output, all later symbols are pads: stop
	 early.  */
      if ((flags & CTF_SYMTYPETAB_EMIT_PAD) && idx[i]->st_symidx == outmax)
	break;
    }

  return 0;
}

/* Emit an objt or func symtypetab index into DP in a paticular order defined by
   an array of symbol names passed in.  Stop at NIDX.  The linker-reported set
   of symbols (if any) is found in SYMFP. */
static int
emit_symtypetab_index (ctf_dict_t *fp, ctf_dict_t *symfp, uint32_t *dp,
		       const char **idx, uint32_t nidx, int size, int flags)
{
  uint32_t i;
  uint32_t *dpp = dp;
  ctf_dynhash_t *symhash;

  ctf_dprintf ("Emitting index of size %i, %u entries reported by linker, "
	       "flags %i\n", size, nidx, flags);

  /* Empty table? Nothing to do.  */
  if (size == 0)
    return 0;

  if (flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
    symhash = fp->ctf_funchash;
  else
    symhash = fp->ctf_objthash;

  /* Indexes should always be unpadded.  */
  if (!ctf_assert (fp, !(flags & CTF_SYMTYPETAB_EMIT_PAD)))
    return -1;					/* errno is set for us.  */

  for (i = 0; i < nidx; i++)
    {
      const char *sym_name;
      void *type;

      if (!(flags & CTF_SYMTYPETAB_FORCE_INDEXED))
	{
	  ctf_link_sym_t *this_link_sym;

	  this_link_sym = ctf_dynhash_lookup (symfp->ctf_dynsyms, idx[i]);

	  /* This is an index: unreported symbols should never appear in it.  */
	  if (!ctf_assert (fp, this_link_sym != NULL))
	    return -1;				/* errno is set for us.  */

	  /* Symbol of the wrong type, or skippable?  This symbol is not in this
	     table.  */
	  if (((flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
	       && this_link_sym->st_type != STT_FUNC)
	      || (!(flags & CTF_SYMTYPETAB_EMIT_FUNCTION)
		  && this_link_sym->st_type != STT_OBJECT))
	    continue;

	  if (ctf_symtab_skippable (this_link_sym))
	    continue;

	  sym_name = this_link_sym->st_name;

	  /* Linker reports symbol of a different type to the symbol we actually
	     added?  Skip the symbol.  */
	  if ((this_link_sym->st_type == STT_FUNC)
	      && (ctf_dynhash_lookup (fp->ctf_objthash, sym_name)))
	    continue;

	  if ((this_link_sym->st_type == STT_OBJECT)
	      && (ctf_dynhash_lookup (fp->ctf_funchash, sym_name)))
	    continue;
	}
      else
	sym_name = idx[i];

      /* Symbol in index and reported by linker, but no type set? Silently skip
	 and (optionally) pad.  (In force-indexed mode, this is also where we
	 track symbols of the wrong type for this round of insertion.)  */
      if ((type = ctf_dynhash_lookup (symhash, sym_name)) == NULL)
	continue;

      ctf_str_add_ref (fp, sym_name, dpp++);

      if (!ctf_assert (fp, (((char *) dpp) - (char *) dp) <= size))
	return -1;				/* errno is set for us.  */
    }

  return 0;
}

/* Figure out the sizes of the symtypetab sections, their indexed state,
   etc.

   This is a sizing function, called before the output buffer is
   constructed.  Do not add any refs in this function!  */

static int
ctf_symtypetab_sect_sizes (ctf_dict_t *fp, emit_symtypetab_state_t *s,
			   ctf_header_t *hdr, size_t *objt_size,
			   size_t *func_size, size_t *objtidx_size,
			   size_t *funcidx_size)
{
  size_t nfuncs, nobjts;
  size_t objt_unpadsize, func_unpadsize, objt_padsize, func_padsize;

  /* If doing a writeout as part of linking, and the link flags request it,
     filter out all unreported symbols from the symtypetab sections.  (If we are
     not linking, the symbols are sorted; if we are linking, don't bother
     sorting if we are not filtering out reported symbols: this is almost
     certainly an ld -r and only the linker is likely to consume these
     symtypetabs again.  The linker doesn't care what order the symtypetab
     entries are in, since it only iterates over symbols and does not use the
     ctf_lookup_by_symbol* API.)  */

  s->sort_syms = 1;
  if (fp->ctf_flags & LCTF_LINKING)
    {
      s->filter_syms = !(fp->ctf_link_flags & CTF_LINK_NO_FILTER_REPORTED_SYMS);
      if (!s->filter_syms)
	s->sort_syms = 0;
    }

  /* Find the dict to which the linker has reported symbols, if any.  */

  if (s->filter_syms)
    {
      if (!fp->ctf_dynsyms && fp->ctf_parent && fp->ctf_parent->ctf_dynsyms)
	s->symfp = fp->ctf_parent;
      else
	s->symfp = fp;
    }

  /* If not filtering, keep all potential symbols in an unsorted, indexed
     dict.  */
  if (!s->filter_syms)
    s->symflags = CTF_SYMTYPETAB_FORCE_INDEXED;
  else
    hdr->cth_flags |= CTF_F_IDXSORTED;

  if (!ctf_assert (fp, (s->filter_syms && s->symfp)
		   || (!s->filter_syms && !s->symfp
		       && ((s->symflags & CTF_SYMTYPETAB_FORCE_INDEXED) != 0))))
    return -1;

  /* Work out the sizes of the object and function sections, and work out the
     number of pad (unassigned) symbols in each, and the overall size of the
     sections.  */

  if (symtypetab_density (fp, s->symfp, fp->ctf_objthash, &nobjts, &s->maxobjt,
			  &objt_unpadsize, &objt_padsize, objtidx_size,
			  s->symflags) < 0)
    return -1;					/* errno is set for us.  */

  ctf_dprintf ("Object symtypetab: %i objects, max %i, unpadded size %i, "
	       "%i bytes of pads, index size %i\n", (int) nobjts,
	       (int) s->maxobjt, (int) objt_unpadsize, (int) objt_padsize,
	       (int) *objtidx_size);

  if (symtypetab_density (fp, s->symfp, fp->ctf_funchash, &nfuncs, &s->maxfunc,
			  &func_unpadsize, &func_padsize, funcidx_size,
			  s->symflags | CTF_SYMTYPETAB_EMIT_FUNCTION) < 0)
    return -1;					/* errno is set for us.  */

  ctf_dprintf ("Function symtypetab: %i functions, max %i, unpadded size %i, "
	       "%i bytes of pads, index size %i\n", (int) nfuncs,
	       (int) s->maxfunc, (int) func_unpadsize, (int) func_padsize,
	       (int) *funcidx_size);

  /* It is worth indexing each section if it would save space to do so, due to
     reducing the number of pads sufficiently.  A pad is the same size as a
     single index entry: but index sections compress relatively poorly compared
     to constant pads, so it takes a lot of contiguous padding to equal one
     index section entry.  It would be nice to be able to *verify* whether we
     would save space after compression rather than guessing, but this seems
     difficult, since it would require complete reserialization.  Regardless, if
     the linker has not reported any symbols (e.g. if this is not a final link
     but just an ld -r), we must emit things in indexed fashion just as the
     compiler does.  */

  *objt_size = objt_unpadsize;
  if (!(s->symflags & CTF_SYMTYPETAB_FORCE_INDEXED)
      && ((objt_padsize + objt_unpadsize) * CTF_INDEX_PAD_THRESHOLD
	  > objt_padsize))
    {
      *objt_size += objt_padsize;
      *objtidx_size = 0;
    }

  *func_size = func_unpadsize;
  if (!(s->symflags & CTF_SYMTYPETAB_FORCE_INDEXED)
      && ((func_padsize + func_unpadsize) * CTF_INDEX_PAD_THRESHOLD
	  > func_padsize))
    {
      *func_size += func_padsize;
      *funcidx_size = 0;
    }

  return 0;
}

/* Emit the symtypetab sections.  */

static int
ctf_emit_symtypetab_sects (ctf_dict_t *fp, emit_symtypetab_state_t *s,
			   unsigned char **tptr, size_t objt_size,
			   size_t func_size, size_t objtidx_size,
			   size_t funcidx_size)
{
  unsigned char *t = *tptr;
  size_t nsymtypes = 0;
  const char **sym_name_order = NULL;
  ctf_error_t err;

  /* Sort the linker's symbols into name order if need be.  */

  if ((objtidx_size != 0) || (funcidx_size != 0))
    {
      ctf_next_t *i = NULL;
      void *symname;
      const char **walk;

      if (s->filter_syms)
	{
	  if (s->symfp->ctf_dynsyms)
	    nsymtypes = ctf_dynhash_elements (s->symfp->ctf_dynsyms);
	  else
	    nsymtypes = 0;
	}
      else
	nsymtypes = ctf_dynhash_elements (fp->ctf_objthash)
	  + ctf_dynhash_elements (fp->ctf_funchash);

      if ((sym_name_order = calloc (nsymtypes, sizeof (const char *))) == NULL)
	goto oom;

      walk = sym_name_order;

      if (s->filter_syms)
	{
	  if (s->symfp->ctf_dynsyms)
	    {
	      while ((err = ctf_dynhash_next_sorted (s->symfp->ctf_dynsyms, &i,
						     &symname, NULL,
						     ctf_dynhash_sort_by_name,
						     NULL)) == 0)
		*walk++ = (const char *) symname;
	      if (err != ECTF_NEXT_END)
		goto symerr;
	    }
	}
      else
	{
	  ctf_hash_sort_f sort_fun = NULL;

	  /* Since we partition the set of symbols back into objt and func,
	     we can sort the two independently without harm.  */
	  if (s->sort_syms)
	    sort_fun = ctf_dynhash_sort_by_name;

	  while ((err = ctf_dynhash_next_sorted (fp->ctf_objthash, &i, &symname,
						 NULL, sort_fun, NULL)) == 0)
	    *walk++ = (const char *) symname;
	  if (err != ECTF_NEXT_END)
	    goto symerr;

	  while ((err = ctf_dynhash_next_sorted (fp->ctf_funchash, &i, &symname,
						 NULL, sort_fun, NULL)) == 0)
	    *walk++ = (const char *) symname;
	  if (err != ECTF_NEXT_END)
	    goto symerr;
	}
    }

  /* Emit the object and function sections, and if necessary their indexes.
     Emission is done in symtab order if there is no index, and in index
     (name) order otherwise.  */

  if ((objtidx_size == 0) && s->symfp && s->symfp->ctf_dynsymidx)
    {
      ctf_dprintf ("Emitting unindexed objt symtypetab\n");
      if (emit_symtypetab (fp, s->symfp, (uint32_t *) t,
			   s->symfp->ctf_dynsymidx, NULL,
			   s->symfp->ctf_dynsymmax + 1, s->maxobjt,
			   objt_size, s->symflags | CTF_SYMTYPETAB_EMIT_PAD) < 0)
	goto err;				/* errno is set for us.  */
    }
  else
    {
      ctf_dprintf ("Emitting indexed objt symtypetab\n");
      if (emit_symtypetab (fp, s->symfp, (uint32_t *) t, NULL,
			   sym_name_order, nsymtypes, s->maxobjt,
			   objt_size, s->symflags) < 0)
	goto err;				/* errno is set for us.  */
    }

  t += objt_size;

  if ((funcidx_size == 0) && s->symfp && s->symfp->ctf_dynsymidx)
    {
      ctf_dprintf ("Emitting unindexed func symtypetab\n");
      if (emit_symtypetab (fp, s->symfp, (uint32_t *) t,
			   s->symfp->ctf_dynsymidx, NULL,
			   s->symfp->ctf_dynsymmax + 1, s->maxfunc,
			   func_size, s->symflags | CTF_SYMTYPETAB_EMIT_FUNCTION
			   | CTF_SYMTYPETAB_EMIT_PAD) < 0)
	goto err;				/* errno is set for us.  */
    }
  else
    {
      ctf_dprintf ("Emitting indexed func symtypetab\n");
      if (emit_symtypetab (fp, s->symfp, (uint32_t *) t, NULL, sym_name_order,
			   nsymtypes, s->maxfunc, func_size,
			   s->symflags | CTF_SYMTYPETAB_EMIT_FUNCTION) < 0)
	goto err;				/* errno is set for us.  */
    }

  t += func_size;

  if (objtidx_size > 0)
    if (emit_symtypetab_index (fp, s->symfp, (uint32_t *) t, sym_name_order,
			       nsymtypes, objtidx_size, s->symflags) < 0)
      goto err;

  t += objtidx_size;

  if (funcidx_size > 0)
    if (emit_symtypetab_index (fp, s->symfp, (uint32_t *) t, sym_name_order,
			       nsymtypes, funcidx_size,
			       s->symflags | CTF_SYMTYPETAB_EMIT_FUNCTION) < 0)
      goto err;

  t += funcidx_size;
  free (sym_name_order);
  *tptr = t;

  return 0;

 oom:
  ctf_set_errno (fp, EAGAIN);
  goto err;
symerr:
  ctf_err_warn (fp, 0, err, _("error serializing symtypetabs"));
 err:
  free (sym_name_order);
  return -1;
}

/* Type section.  */

/* Kind suppression.  */

ctf_ret_t
ctf_write_suppress_kind (ctf_dict_t *fp, int kind, int prohibited)
{
  ctf_dynset_t *set;

  if (kind < CTF_K_UNKNOWN || kind > CTF_K_MAX)
    return (ctf_set_errno (fp, EINVAL));

  if (prohibited)
    set = fp->ctf_write_prohibitions;
  else
    set = fp->ctf_write_suppressions;

  if (!set)
    {
      set = ctf_dynset_create (htab_hash_pointer, htab_eq_pointer, NULL);
      if (!set)
	return (ctf_set_errno (fp, errno));

      if (prohibited)
	fp->ctf_write_prohibitions = set;
      else
	fp->ctf_write_suppressions = set;
    }

  if ((ctf_dynset_cinsert (set, (const void *) (uintptr_t) kind)) < 0)
    return (ctf_set_errno (fp, errno));

  return 0;
}

/* Figure out whether we can elide a given type prefix as unnecessary.

   Prefixes can be elided iff they are BIG and zero-size and zero-vlen and (if
   structs) the type as a whole is smallifiable.  If not elided, this prefix is
   included.  */
static int
ctf_prefix_elidable (ctf_dict_t *fp, uint32_t kind, ctf_dtdef_t *dtd,
		     ctf_type_t *prefix)
{
  int prefix_kind = LCTF_INFO_UNPREFIXED_KIND (fp, prefix->ctt_info);

  if ((prefix_kind == CTF_K_BIG) && prefix->ctt_size == 0
      && LCTF_INFO_UNPREFIXED_VLEN (fp, prefix->ctt_info) == 0)
    {
      ssize_t size;

      if (kind != CTF_K_STRUCT)
	return 1;

      /* For bitfields, we must check if the individual member offsets will
	 still fit if they are encoded as offsets rather than offset-since-last.
	 We can check this for both cases without checking individual fields by
	 looking at the ctt_size.  We need not check for nameless padding
	 members, because these can only be produced at all if the struct would
	 require CTF_K_BIG in the first place.  */

      size = ctf_get_ctt_size (fp, dtd->dtd_buf, NULL, NULL);

      if (CTF_INFO_KFLAG (dtd->dtd_data->ctt_info))
	return (size <= CTF_MAX_BIT_OFFSET);
      else
	return (size <= CTF_MAX_SIZE);
    }
  return 0;
}

/* Determine whether newly-defined or modified types indicate that we will be
   able to emit a BTF type section or must emit a CTF one.  Only called if we
   have already verified that the CTF-specific sections (typetabs, etc) will be
   empty, and that the input dict was freshly-created or read in from BTF.  */

static int
ctf_type_sect_is_btf (ctf_dict_t *fp, int force_ctf)
{
  ctf_dtdef_t *dtd;
  ctf_next_t *i = NULL;
  ctf_next_t *prohibit_i = NULL;
  void *pkind;
  ctf_error_t err;

  /* Verify prohibitions.  Do this first, for a fast return if a kind is
     prohibited.  */

  if (fp->ctf_write_prohibitions)
    {
      while ((err = ctf_dynset_next (fp->ctf_write_prohibitions,
				     &prohibit_i, &pkind)) == 0)
	{
	  int kind = (uintptr_t) pkind;

	  if (ctf_type_kind_next (fp, &i, kind) != CTF_ERR)
	    {
	      ctf_next_destroy (i);
	      ctf_next_destroy (prohibit_i);
	      ctf_err_warn (fp, 0, ECTF_KIND_PROHIBITED,
			    _("Attempt to write out kind %i, which is prohibited by ctf_write_suppress_kind"),
			    kind);
	      return (ctf_set_errno (fp, ECTF_KIND_PROHIBITED));
	    }
	}
      if (err != ECTF_NEXT_END)
	{
	  ctf_next_destroy (prohibit_i);
	  ctf_err_warn (fp, 0, err, _("ctf_write: iteration error checking prohibited kinds"));
	  return (ctf_set_errno (fp, err));
	}
    }

  /* Prohibitions checked: if the user requested CTF come what may, we know this
     cannot be BTF.  */

  if (force_ctf)
    return 0;

  /* Check all types for invalid-in-BTF features.  */

  for (dtd = ctf_list_next (&fp->ctf_dtdefs);
       dtd != NULL; dtd = ctf_list_next (dtd))
    {
      ctf_type_t *tp = dtd->dtd_buf;
      uint32_t kind = LCTF_KIND (fp, dtd->dtd_buf);
      uint32_t prefix_kind;

      /* Any un-suppressed prefixes other than an empty/redundant CTF_K_BIG must
	 be CTF. (Redundant CTF_K_BIGs will be elided instead.)  */

      while (prefix_kind = LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info),
	     LCTF_IS_PREFIXED_KIND (prefix_kind))
	{
	  if (!ctf_prefix_elidable (fp, kind, dtd, tp))
	    if (!fp->ctf_write_suppressions
		|| ctf_dynset_lookup (fp->ctf_write_suppressions,
				      (const void *) (uintptr_t) prefix_kind) == NULL)
	    return 0;

	  tp++;
	}

      /* Prefixes checked.  If this kind is suppressed, it won't influence the
	 result.  */

      if (fp->ctf_write_suppressions
	  && ctf_dynset_lookup (fp->ctf_write_suppressions,
				(const void *) (uintptr_t) kind))
	continue;

      if (kind == CTF_K_FLOAT || kind == CTF_K_SLICE)
	return 0;
    }

  return 1;
}

/* Iterate through the static types and the dynamic type definition list and
   compute the size of the CTF type section.

   This is a sizing function, called before the output buffer is
   constructed.  Do not add any refs in this function!  */

static size_t
ctf_type_sect_size (ctf_dict_t *fp)
{
  ctf_dtdef_t *dtd;
  size_t type_size = 0;

  for (dtd = ctf_list_next (&fp->ctf_dtdefs);
       dtd != NULL; dtd = ctf_list_next (dtd))
    {
      uint32_t kind = LCTF_KIND (fp, dtd->dtd_buf);
      uint32_t prefix_kind;
      ctf_type_t *tp = dtd->dtd_buf;

      /* Check for suppressions: a suppression consumes precisely one ctf_type_t
	 record of space.  A suppressed prefix suppresses the whole type.  */

      if (fp->ctf_write_suppressions)
	{
	  int suppress = 0;

	  while (prefix_kind = LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info),
		 LCTF_IS_PREFIXED_KIND (prefix_kind))
	    {
	      if (!ctf_prefix_elidable (fp, kind, dtd, tp)
		  && (ctf_dynset_lookup (fp->ctf_write_suppressions,
					 (const void *) (uintptr_t) prefix_kind) != NULL))
		{
		  type_size += sizeof (ctf_type_t);
		  suppress = 1;
		  break;
		}

	      tp++;
	    }
	  if (suppress)
	    continue;
	}

      /* Type headers: elide CTF_K_BIG from types if possible.  */

      tp = dtd->dtd_buf;
      while (prefix_kind = LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info),
	     LCTF_IS_PREFIXED_KIND (prefix_kind))
	{
	  if (!ctf_prefix_elidable (fp, kind, dtd, tp))
	    type_size += sizeof (ctf_type_t);
	  tp++;
	}
      type_size += sizeof (ctf_type_t);
      type_size += dtd->dtd_vlen_size;
    }

  return type_size + fp->ctf_header->btf.bth_type_len;
}

/* Take a final lap through the dynamic type definition list and copy the
   appropriate type records to the output buffer, noting down the strings
   and type IDs as we go.

   By this stage we no longer need to worry about CTF-versus-BTF, only about
   whether a type has been suppressed or not.  */

static int
ctf_emit_type_sect (ctf_dict_t *fp, unsigned char **tptr,
		    size_t expected_size)
{
  unsigned char *t = *tptr;
  ctf_dtdef_t *dtd;
  ctf_id_t id;

  id = fp->ctf_stypes + 1;

  if (fp->ctf_flags & LCTF_CHILD)
    id += fp->ctf_parent->ctf_typemax;

  for (dtd = ctf_list_next (&fp->ctf_dtdefs);
       dtd != NULL; dtd = ctf_list_next (dtd), id++)
    {
      uint32_t prefix_kind;
      uint32_t kind = LCTF_KIND (fp, dtd->dtd_buf);
      size_t vlen = LCTF_VLEN (fp, dtd->dtd_buf);
      ctf_type_t *tp = dtd->dtd_buf;
      ctf_type_t *copied;
      const char *name;
      int suppress = 0;
      int big = 0, big_elided = 0;
      size_t i;

      /* Make sure the ID hasn't changed, if already assigned by a previous
	 serialization.  */

      if (dtd->dtd_final_type != 0
	  && !ctf_assert (fp, dtd->dtd_final_type == id))
	return -1;				/* errno is set for us.  */

      /* Suppress everything if this kind is suppressed.  */

      while (prefix_kind = LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info),
	     LCTF_IS_PREFIXED_KIND (prefix_kind))
	{
	  /* Don't worry about BIGs that will be elided.  */
	  if (ctf_prefix_elidable (fp, kind, dtd, tp))
	    {
	      tp++;
	      continue;
	    }

	  if (!fp->ctf_write_suppressions
	      || ctf_dynset_lookup (fp->ctf_write_suppressions,
				    (const void *) (uintptr_t) prefix_kind) == NULL)
	    {
	      if (_libctf_btf_mode == LIBCTF_BTM_BTF)
		{
		  ctf_err_warn (fp, 0, ECTF_NOTBTF, _("type %lx: Attempt to write out CTF-specific kind %i as BTF"),
				id, prefix_kind);
		  return (ctf_set_errno (fp, ECTF_NOTBTF));
		}
	    }
	  else
	    {
	      suppress = 1;
	      break;
	    }
	  tp++;
	}

      if (fp->ctf_write_suppressions
	  && ctf_dynset_lookup (fp->ctf_write_suppressions,
				(const void *) (uintptr_t) kind) != NULL)
	suppress = 1;

      if (suppress)
	{
	  ctf_type_t suppressed = { 0 };

	  if (!ctf_assert (fp, t - *tptr + sizeof (ctf_type_t) <= expected_size))
	    return -1; 				/* errno is set for us.  */

	  suppressed.ctt_info = CTF_TYPE_INFO (CTF_K_UNKNOWN, 0, 0);
	  memcpy (t, &suppressed, sizeof (ctf_type_t));
	  t += sizeof (ctf_type_t);

	  continue;
	}

      /* Write out all the type headers, eliding empty CTF_K_BIG, and noting if
	 this type is BIG.  */

      tp = dtd->dtd_buf;
      while (prefix_kind = LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info),
	     LCTF_IS_PREFIXED_KIND (prefix_kind))
	{
	  if (prefix_kind == CTF_K_BIG)
	    {
	      big = 1;

	      if (ctf_prefix_elidable (fp, kind, dtd, tp))
		{
		  big_elided = 1;
		  tp++;
		  continue;
		}
	    }

	  if (!ctf_assert (fp, t - *tptr + sizeof (ctf_type_t) <= expected_size))
	    return -1;				/* errno is set for us.  */

	  memcpy (t, tp, sizeof (ctf_type_t));
	  copied = (ctf_type_t *) t;

	  /* CTF_K_CONFLICTING has a name to keep track of.  */

	  if (copied->ctt_name
	      && (name = ctf_strraw (fp, copied->ctt_name)) != NULL)
	    ctf_str_add_ref (fp, name, &copied->ctt_name);

	  /* No prefixed kinds have any ctt_types to deal with. */

	  tp++;
	  t += sizeof (ctf_type_t);
	}

      if (!ctf_assert (fp, t - *tptr + sizeof (ctf_type_t) <= expected_size))
	return -1; 				/* errno is set for us.  */

      memcpy (t, tp, sizeof (ctf_type_t));
      copied = (ctf_type_t *) t;
      if (copied->ctt_name
	  && (name = ctf_strraw (fp, copied->ctt_name)) != NULL)
        ctf_str_add_ref (fp, name, &copied->ctt_name);
      t += sizeof (ctf_type_t);

      if (!ctf_assert (fp, t - *tptr + dtd->dtd_vlen_size <= expected_size))
	return -1; 				/* errno is set for us.  */

      memcpy (t, dtd->dtd_vlen, dtd->dtd_vlen_size);

      switch (kind)
	{
	case CTF_K_ARRAY:
	  {
	    ctf_array_t *array = (ctf_array_t *) t;

	    if (ctf_type_add_ref (fp, &array->cta_contents) < 0)
	      return -1;			/* errno is set for us.  */

	    if (ctf_type_add_ref (fp, &array->cta_index) < 0)
	      return -1;			/* errno is set for us.  */
	  }
	  break;

	case CTF_K_POINTER:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	case CTF_K_TYPEDEF:
	case CTF_K_FUNC_LINKAGE:
	case CTF_K_TYPE_TAG:
	case CTF_K_DECL_TAG:
	case CTF_K_VAR:
	  if (ctf_type_add_ref (fp, &copied->ctt_type) < 0)
	    return -1;				/* errno is set for us.  */
	  break;

	case CTF_K_SLICE:
	  {
	    ctf_slice_t *slice = (ctf_slice_t *) t;

	    if (ctf_type_add_ref (fp, &slice->cts_type) < 0)
	      return -1;			/* errno is set for us. */
	  }

	  t += sizeof (struct ctf_slice);

	  break;

	case CTF_K_FUNCTION:
	  {
	    ctf_param_t *args = (ctf_param_t *) t;
	    ctf_param_t *dtd_args = (ctf_param_t *) dtd->dtd_vlen;

	    if (ctf_type_add_ref (fp, &copied->ctt_type) < 0)
	      return -1;			/* errno is set for us.  */

	    for (i = 0; i < vlen; i++)
	      {
		const char *name = ctf_strraw (fp, args[i].cfp_name);

		ctf_str_add_ref (fp, name, &args[i].cfp_name);
		ctf_str_add_ref (fp, name, &dtd_args[i].cfp_name);

		if (ctf_type_add_ref (fp, &args[i].cfp_type) < 0)
		  return -1;			/* errno is set for us.  */
	      }
	    break;
	  }

	  /* These may need all their offsets adjusting.  */
	case CTF_K_STRUCT:
	case CTF_K_UNION:
	  {
	    size_t offset = 0;
	    int bitwise = CTF_INFO_KFLAG (tp->ctt_info);
	    int struct_is_prefixed_big = big;

	    ctf_member_t *memb = (ctf_member_t *) t;
	    ctf_member_t *dtd_memb = (ctf_member_t *) dtd->dtd_vlen;

	    /* All structs and unions in a DTD must always be BIG.  */

	    if (!ctf_assert (fp, struct_is_prefixed_big))
	      return -1;			/* errno is set for us.  */

	    for (i = 0; i < vlen; i++)
	      {
		const char *name = ctf_strraw (fp, memb[i].ctm_name);

		ctf_str_add_ref (fp, name, &memb[i].ctm_name);
		ctf_str_add_ref (fp, name, &dtd_memb[i].ctm_name);

		/* If we are reducing a struct to non-big, we must convert its
		   offsets back to offset-from-start.  */

		if (big_elided && kind == CTF_K_STRUCT)
		  {
		    size_t this_offset, this_size;

		    if (bitwise)
		      {
			this_offset = CTF_MEMBER_BIT_OFFSET (memb[i].ctm_offset);
			this_size = CTF_MEMBER_BIT_SIZE (memb[i].ctm_offset);
			offset += this_offset;
			memb[i].ctm_offset = CTF_MEMBER_MAKE_BIT_OFFSET (this_size,
									 offset);
		      }
		    else
		      {
			offset += memb[i].ctm_offset;
			memb[i].ctm_offset = offset;
		      }
		  }

		if (ctf_type_add_ref (fp, &memb[i].ctm_type) < 0)
		  return -1;			/* errno is set for us.  */
	      }
	  }
	  break;

	case CTF_K_ENUM:
	  {
	    ctf_enum_t *dtd_vlen = (ctf_enum_t *) dtd->dtd_vlen;
	    ctf_enum_t *t_vlen = (ctf_enum_t *) t;

	    for (i = 0; i < vlen; i++)
	      {
		const char *name = ctf_strraw (fp, dtd_vlen[i].cte_name);

		ctf_str_add_ref (fp, name, &dtd_vlen[i].cte_name);
		ctf_str_add_ref (fp, name, &t_vlen[i].cte_name);
	      }

	    break;
	  }

	case CTF_K_ENUM64:
	  {
	    ctf_enum64_t *dtd_vlen = (ctf_enum64_t *) dtd->dtd_vlen;
	    ctf_enum64_t *t_vlen = (ctf_enum64_t *) t;

	    for (i = 0; i < vlen; i++)
	      {
		const char *name = ctf_strraw (fp, dtd_vlen[i].cte_name);

		ctf_str_add_ref (fp, name, &dtd_vlen[i].cte_name);
		ctf_str_add_ref (fp, name, &t_vlen[i].cte_name);
	      }

	    break;
	  }
	case CTF_K_DATASEC:
	  {
	    ctf_var_secinfo_t *sec = (ctf_var_secinfo_t *) t;

	    if (dtd->dtd_flags & DTD_F_UNSORTED)
	      ctf_datasec_sort (fp, dtd);

	    for (i = 0; i < vlen; i++)
	      {
		if (ctf_type_add_ref (fp, &sec[i].cvs_type) < 0)
		  return -1;			/* errno is set for us.  */
	      }
	    break;
	  }
	}

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
      if (dtd->dtd_type != id)
	ctf_dprintf ("%p: provisional ID assignment: %lx -> %lx\n", (void *) fp,
		     dtd->dtd_type, id);
#endif

      t += dtd->dtd_vlen_size;
      dtd->dtd_final_type = id;
    }

  *tptr = t;

  return 0;
}

/* Overall serialization.  */

/* Determine the output format.  Returns 0 on successful determination or -1 and
   an error if an attempt is being made to write out a CTF dict but the library
   state prohibits it, or a per-dict prohibition is preventing the writeout of a
   type kind that this dict contains.  */

int
ctf_serialize_output_format (ctf_dict_t *fp, int force_ctf)
{
  int ctf_needed = 0;

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  /* Complain if we're asked to emit BTF only, but we have types that call for
     CTFv4 extensions, or we are forced to emit CTF because the caller requested
     compression.  */

  if (force_ctf)
    ctf_needed = 1;

  if (ctf_needed && _libctf_btf_mode == LIBCTF_BTM_BTF)
    goto err_not_btf;

  /* Relatively expensive, so done after cheap checks.  */

  if (!ctf_type_sect_is_btf (fp, force_ctf))
    ctf_needed = 1;

  if (ctf_needed && _libctf_btf_mode == LIBCTF_BTM_BTF)
    goto err_not_btf;

  if (_libctf_btf_mode == LIBCTF_BTM_ALWAYS
      || (_libctf_btf_mode == LIBCTF_BTM_POSSIBLE && ctf_needed))
    fp->ctf_serialize.cs_is_btf = 0;
  else
    fp->ctf_serialize.cs_is_btf = 1;

  fp->ctf_serialize.cs_initialized = 1;

  return 0;

 err_not_btf:
  ctf_set_errno (fp, ECTF_NOTBTF);
  /* TODO: a little more info?  */
  if (force_ctf)
    ctf_err_warn (fp, 0, 0, _("Cannot write out dict as BTF: compression requested"));
  else
    ctf_err_warn (fp, 0, 0, _("Cannot write out dict as BTF: would lose information"));
  return -1;
}

/* Do all aspects of serialization up to strtab writeout, including final type
   ID assignment.  The resulting dict will have the LCTF_PRESERIALIZED flag on
   and must not be modified in any way before serialization.  (This is only
   lightly enforced, as this feature is internal-only, employed by the linker
   machinery.)

   If FORCE_CTF is enabled, always emit CTF in LIBCTF_BTM_POSSIBLE mode, and
   error in LIBCTF_BTM_BTF mode.  */

int
ctf_preserialize (ctf_dict_t *fp, int force_ctf)
{
  ctf_header_t hdr, *hdrp;
  ctf_dtdef_t *dtd;
  int sym_functions = 0;
  size_t hdr_len;
  int ctf_adjustment = 0;

  unsigned char *t;
  size_t buf_size, type_size, objt_size, func_size;
  size_t funcidx_size, objtidx_size;
  unsigned char *buf = NULL;

  emit_symtypetab_state_t symstate;
  memset (&symstate, 0, sizeof (emit_symtypetab_state_t));

  ctf_dprintf ("Preserializing dict for %s\n", ctf_cuname (fp));

  if (fp->ctf_flags & LCTF_NO_STR)
    return (ctf_set_errno (fp, ECTF_NOPARENT));

  /* Make sure that any parents have been serialized at least once since the
     last type was added to them, so we have known final IDs for all their
     types.  */

  if (fp->ctf_parent)
    {
      if (fp->ctf_parent->ctf_nprovtypes > 0)
	{
	  ctf_dtdef_t *dtd;

	  dtd = ctf_list_prev (&fp->ctf_parent->ctf_dtdefs);

	  if (dtd && dtd->dtd_final_type == 0)
	    {
	      ctf_set_errno (fp, ECTF_NOTSERIALIZED);
	      ctf_err_warn (fp, 0, 0, _("cannot write out child dict: write out the parent dict first"));
	      return -1;			/* errno is set for us.  */
	    }
	}

      /* Prohibit serialization of a dict which has already been serialized and
	 whose parent has had more types added to it since then: this dict would
	 have overlapping types if serialized, since we only pass through
	 newly-added types to renumber them, not already-existing types in the
	 read-in buffer.  You can emit such dicts using ctf_link, which can
	 change type IDs arbitrarily, resolving all overlaps.  */

      if (fp->ctf_header->btf.bth_str_len > 0 &&
	  fp->ctf_header->cth_parent_ntypes < fp->ctf_parent->ctf_typemax)
	{
	  ctf_set_errno (fp, ECTF_NOTSERIALIZED);
	  ctf_err_warn (fp, 0, 0, _("cannot write out already-written child dict: parent has had %u types added"),
			fp->ctf_parent->ctf_typemax - fp->ctf_header->cth_parent_ntypes);
	  return -1;				/* errno is set for us.  */
	}
    }
  else
    {
      /* Prohibit serialization of a parent dict which has already been
	 serialized, has children, and has had strings added since the last
	 serialization: because we update strtabs in the dict itself, not just
	 the serialized copy, this would cause overlapping strtabs.

	 TODO: lift this restriction.  */

      if (fp->ctf_str[CTF_STRTAB_0].cts_len != 0
	  && fp->ctf_max_children > 0
	  && fp->ctf_str_prov_len != 0)
	{
	  ctf_set_errno (fp, EINVAL);
	  ctf_err_warn (fp, 0, 0, _("cannot write out already-written dict with children and newly-added strings"));
	  return -1;
	}
    }

  /* Fill in an initial CTF header.  The type section begins at a 4-byte aligned
     boundary past the CTF header itself (at relative offset zero).

     It is quite possible that we will only write out the leading
     ctf_btf_header_t portion of this structure.  */

  memset (&hdr, 0, sizeof (hdr));
  hdr.btf.bth_preamble.btf_magic = CTF_BTF_MAGIC;
  hdr.btf.bth_preamble.btf_version = CTF_BTF_VERSION;
  hdr.btf.bth_preamble.btf_flags = 0;
  hdr.btf.bth_hdr_len = sizeof (ctf_btf_header_t);
  hdr.cth_preamble.ctp_magic_version = (CTFv4_MAGIC << 16) | CTF_VERSION;

  /* Propagate all symbols in the symtypetabs into the dynamic state, so that we
     can put them back in the right order during sizing.  Symbols already in the
     dynamic state, likely due to repeated serialization, are left
     unchanged.  */
  do
    {
      ctf_next_t *it = NULL;
      const char *sym_name;
      ctf_id_t sym;

      while ((sym = ctf_symbol_next_static (fp, &it, &sym_name,
					    sym_functions)) != CTF_ERR)
	if ((ctf_add_funcobjt_sym_forced (fp, sym_functions, sym_name, sym)) < 0)
	  if (ctf_errno (fp) != ECTF_DUPLICATE)
	    {
	      ctf_next_destroy (it);
	      return -1;			/* errno is set for us.  */
	    }

      if (ctf_errno (fp) != ECTF_NEXT_END)
	return -1;				/* errno is set for us.  */
    } while (sym_functions++ < 1);

  /* Figure out how big the symtypetabs are now.  */

  if (ctf_symtypetab_sect_sizes (fp, &symstate, &hdr, &objt_size, &func_size,
				 &objtidx_size, &funcidx_size) < 0)
    return -1;					/* errno is set for us.  */

  if (!fp->ctf_serialize.cs_initialized)
    {
      if (ctf_serialize_output_format (fp, force_ctf) < 0)
	return -1;				/* errno is set for us.  */
    }

  type_size = ctf_type_sect_size (fp);

  /* Compute the size of the CTF buffer we need, sans only the string table,
     then allocate a new buffer and memcpy the finished header to the start of
     the buffer.  (We will adjust this later with strtab length info.)

     Offsets in the BTF and CTF headers are relative to the end of te header in
     question.  */

  if (fp->ctf_serialize.cs_is_btf)
    {
      ctf_dprintf ("Writing out as BTF\n");

      hdr_len = sizeof (ctf_btf_header_t);
    }
  else
    {
      ctf_dprintf ("Writing out as CTF\n");

      hdr_len = sizeof (ctf_header_t);
      ctf_adjustment = sizeof (ctf_header_t) - sizeof (ctf_btf_header_t);
    }

  hdr.cth_objt_off = 0;
  hdr.cth_objt_len = objt_size;
  hdr.cth_func_off = hdr.cth_objt_off + objt_size;
  hdr.cth_func_len = func_size;
  hdr.cth_objtidx_off = hdr.cth_func_off + func_size;
  hdr.cth_objtidx_len = objtidx_size;
  hdr.cth_funcidx_off = hdr.cth_objtidx_off + objtidx_size;
  hdr.cth_funcidx_len = funcidx_size;
  hdr.btf.bth_type_off = hdr.cth_funcidx_off + funcidx_size + ctf_adjustment;
  hdr.btf.bth_type_len = type_size;
  hdr.btf.bth_str_off = hdr.btf.bth_type_off + type_size;
  hdr.btf.bth_str_len = 0;
  hdr.cth_parent_strlen = 0;
  if (fp->ctf_parent)
    hdr.cth_parent_ntypes = fp->ctf_parent->ctf_typemax;

  /* No strings yet.  */
  buf_size = sizeof (ctf_btf_header_t) + hdr.btf.bth_str_off;

  if ((buf = malloc (buf_size)) == NULL)
    return (ctf_set_errno (fp, EAGAIN));

  fp->ctf_serialize.cs_buf = buf;
  fp->ctf_serialize.cs_buf_size = buf_size;

  memcpy (buf, &hdr, hdr_len);
  t = (unsigned char *) buf + hdr_len + hdr.cth_objt_off;

  hdrp = (ctf_header_t *) buf;

  if (!fp->ctf_serialize.cs_is_btf)
    {
      if ((fp->ctf_flags & LCTF_CHILD) && (fp->ctf_parent_name != NULL))
	ctf_str_add_no_dedup_ref (fp, fp->ctf_parent_name,
				  &hdrp->cth_parent_name);
      if (fp->ctf_cu_name != NULL)
	ctf_str_add_no_dedup_ref (fp, fp->ctf_cu_name, &hdrp->cth_cu_name);

      if (ctf_emit_symtypetab_sects (fp, &symstate, &t, objt_size, func_size,
				     objtidx_size, funcidx_size) < 0)
	goto err;
    }
  assert (t == (unsigned char *) buf + sizeof (ctf_btf_header_t)
	  + hdr.btf.bth_type_off);

  /* Copy in existing static types, then emit new dynamic types.  */

  memcpy (t, fp->ctf_buf + fp->ctf_header->btf.bth_type_off,
	  fp->ctf_header->btf.bth_type_len);
  t += fp->ctf_header->btf.bth_type_len;

  if (ctf_emit_type_sect (fp, &t, hdr.btf.bth_str_off
			  - fp->ctf_header->btf.bth_type_len) < 0)
    goto err;

  assert (t == (unsigned char *) buf + sizeof (ctf_btf_header_t)
	  + hdr.btf.bth_str_off);

  /* All types laid out: update all refs to types to cite the final IDs.  */

  for (dtd = ctf_list_next (&fp->ctf_dtdefs);
       dtd != NULL; dtd = ctf_list_next (dtd))
    {
      if (!ctf_assert (fp, dtd->dtd_type != 0 && dtd->dtd_final_type != 0))
	goto err;

      ctf_update_refs (&dtd->dtd_refs, dtd->dtd_final_type);
    }

  ctf_type_purge_refs (fp);

  /* Prohibit type and string additions from this point on.  */

  fp->ctf_flags |= LCTF_NO_STR | LCTF_NO_TYPE;

  return 0;

 err:
  fp->ctf_serialize.cs_initialized = 0;
  fp->ctf_serialize.cs_buf = NULL;
  fp->ctf_serialize.cs_buf_size = 0;

  free (buf);
  ctf_str_purge_refs (fp);
  ctf_type_purge_refs (fp);

  return -1;				/* errno is set for us.  */
}

/* Undo preserialization (called on error).  */
void
ctf_depreserialize (ctf_dict_t *fp)
{
  ctf_str_purge_refs (fp);
  ctf_type_purge_refs (fp);

  fp->ctf_serialize.cs_initialized = 0;
  free (fp->ctf_serialize.cs_buf);
  fp->ctf_serialize.cs_buf = NULL;
  fp->ctf_serialize.cs_buf_size = 0;

  fp->ctf_flags &= ~(LCTF_NO_STR | LCTF_NO_TYPE);
}

/* Emit a new CTF dict which is a serialized copy of this one: also reify the
   string table and update all offsets in the newly-serialized dict suitably.
   (This simplifies ctf-string.c a little, at the cost of storing a second copy
   of the strtab during serialization.)

   Other aspects of the existing dict are unchanged, although some static
   entries may be duplicated in the dynamic state (which should have no effect
   on visible operation).  */

static unsigned char *
ctf_serialize (ctf_dict_t *fp, size_t *bufsiz, int force_ctf)
{
  const ctf_strs_writable_t *strtab;
  unsigned char *buf, *newbuf;
  ctf_btf_header_t *hdrp;

  /* Stop unstable file formats (subject to change) getting out into the
     wild.  */
#if CTF_VERSION != CTF_STABLE_VERSION
  if (!getenv ("I_KNOW_LIBCTF_IS_UNSTABLE"))
    {
      ctf_depreserialize (fp);
      ctf_set_errno (fp, ECTF_UNSTABLE);
      return NULL;
    }
#endif

  /* Preserialize, if we need to.  */

  if (!fp->ctf_serialize.cs_buf)
    if (ctf_preserialize (fp, force_ctf) < 0)
      return NULL;				/* errno is set for us.  */

  /* Allow string lookup again.  */
  fp->ctf_flags &= ~LCTF_NO_STR;

  /* Construct the final string table and fill out all the string refs with the
     final offsets.  At link time, before the strtab can be constructed, child
     dicts also need their cth_parent_strlen header field updated to match the
     parent's.  (These are always newly-created dicts, so we don't need to worry
     about the upgraded-from-v3 case, which must always retain a
     cth_parent_strlen value of 0.)  */

  if ((fp->ctf_flags & LCTF_LINKING) && fp->ctf_parent)
    fp->ctf_header->cth_parent_strlen = fp->ctf_parent->ctf_str[CTF_STRTAB_0].cts_len;

  ctf_dprintf ("Writing strtab for %s\n", ctf_cuname (fp));
  strtab = ctf_str_write_strtab (fp);

  if (strtab == NULL)
    goto err;

  if ((newbuf = realloc (fp->ctf_serialize.cs_buf, fp->ctf_serialize.cs_buf_size
			 + strtab->cts_len)) == NULL)
    goto oom;

  fp->ctf_serialize.cs_buf = newbuf;
  memcpy (fp->ctf_serialize.cs_buf + fp->ctf_serialize.cs_buf_size, strtab->cts_strs,
	  strtab->cts_len);

  hdrp = (ctf_btf_header_t *) fp->ctf_serialize.cs_buf;
  hdrp->bth_str_len = strtab->cts_len;
  fp->ctf_serialize.cs_buf_size += hdrp->bth_str_len;

  if (!fp->ctf_serialize.cs_is_btf)
    {
      ctf_header_t *ctf_hdrp;

      ctf_hdrp = (ctf_header_t *) (void *) hdrp;
      ctf_hdrp->cth_parent_strlen = fp->ctf_header->cth_parent_strlen;
    }

  *bufsiz = fp->ctf_serialize.cs_buf_size;

  buf = fp->ctf_serialize.cs_buf;

  fp->ctf_serialize.cs_buf = NULL;
  fp->ctf_serialize.cs_buf_size = 0;
  fp->ctf_flags &= ~LCTF_NO_TYPE;

  return buf;

oom:
  ctf_set_errno (fp, EAGAIN);
err:
  ctf_depreserialize (fp);
  return NULL;					/* errno is set for us.  */
}

/* File writing.  */

/* Write the compressed CTF data stream to the specified gzFile descriptor.  The
   whole stream is compressed, and cannot be read by CTF opening functions in
   this library until it is decompressed.  (The functions below this one leave
   the header uncompressed, and the CTF opening functions work on them without
   manual decompression.)

   No support for (testing-only) endian-flipping or pure BTF writing.  */
int
ctf_gzwrite (ctf_dict_t *fp, gzFile fd)
{
  unsigned char *buf;
  unsigned char *p;
  size_t bufsiz;
  size_t len, written = 0;

  if ((buf = ctf_serialize (fp, &bufsiz, 1)) == NULL)
    return -1;					/* errno is set for us.  */

  p = buf;
  while (written < bufsiz)
    {
      if ((len = gzwrite (fd, p, bufsiz - written)) <= 0)
	{
	  free (buf);
	  return (ctf_set_errno (fp, errno));
	}
      written += len;
      p += len;
    }

  free (buf);
  return 0;
}

/* Optionally compress the specified CTF data stream and return it as a new
   dynamically-allocated string.  Possibly write it with reversed
   endianness.  */
unsigned char *
ctf_write_mem (ctf_dict_t *fp, size_t *size, size_t threshold)
{
  unsigned char *rawbuf;
  unsigned char *buf = NULL;
  unsigned char *bp;
  ctf_header_t *rawhp, *hp;
  unsigned char *src;
  size_t rawbufsiz;
  size_t alloc_len = 0;
  size_t hdrlen;
  int uncompressed = 0;
  int flip_endian;
  int rc;

  flip_endian = getenv ("LIBCTF_WRITE_FOREIGN_ENDIAN") != NULL;

  if ((rawbuf = ctf_serialize (fp, &rawbufsiz,
			       threshold != (size_t) -1)) == NULL)
    return NULL;				/* errno is set for us.  */

  if (fp->ctf_serialize.cs_is_btf)
    hdrlen = sizeof (ctf_btf_header_t);
  else
    hdrlen = sizeof (ctf_header_t);

  if (!ctf_assert (fp, rawbufsiz >= hdrlen))
    goto err;

  if (rawbufsiz >= threshold && !fp->ctf_serialize.cs_is_btf)
    alloc_len = compressBound (rawbufsiz - sizeof (ctf_header_t))
      + sizeof (ctf_header_t);

  /* Trivial operation if the buffer is too small to bother compressing, and
     we're not doing a forced write-time flip.  */

  if (rawbufsiz < threshold || fp->ctf_serialize.cs_is_btf)
    {
      alloc_len = rawbufsiz;
      uncompressed = 1;
    }

  if (!flip_endian && uncompressed)
    {
      *size = rawbufsiz;
      return rawbuf;
    }

  if ((buf = malloc (alloc_len)) == NULL)
    {
      ctf_set_errno (fp, ENOMEM);
      ctf_err_warn (fp, 0, 0, _("ctf_write_mem: cannot allocate %li bytes"),
		    (unsigned long) (alloc_len));
      goto err;
    }

  rawhp = (ctf_header_t *) rawbuf;
  hp = (ctf_header_t *) buf;

  memcpy (hp, rawbuf, hdrlen);
  bp = buf + hdrlen;
  *size = hdrlen;

  if (!uncompressed && !fp->ctf_serialize.cs_is_btf)
    hp->cth_flags |= CTF_F_COMPRESS;

  src = rawbuf + hdrlen;

  if (flip_endian)
    {
      if (ctf_flip_header (hp, fp->ctf_serialize.cs_is_btf, 0) < 0)
	goto err;				/* errno is set for us.  */
      if (ctf_flip (fp, rawhp, src, fp->ctf_serialize.cs_is_btf, 1) < 0)
	goto err;				/* errno is set for us.  */
    }

  /* Must be CTFv4.  */
  if (!uncompressed)
    {
      size_t compress_len = alloc_len - sizeof (ctf_header_t);

      if ((rc = compress (bp, (uLongf *) &compress_len,
			  src, rawbufsiz - hdrlen)) != Z_OK)
	{
	  ctf_set_errno (fp, ECTF_COMPRESS);
	  ctf_err_warn (fp, 0, 0, _("zlib deflate err: %s"), zError (rc));
	  goto err;
	}
      *size += compress_len;
    }
  else
    {
      memcpy (bp, src, rawbufsiz - hdrlen);
      *size += rawbufsiz - hdrlen;
    }

  free (rawbuf);
  return buf;
err:
  free (buf);
  free (rawbuf);
  return NULL;
}

/* Write the compressed CTF data stream to the specified file descriptor,
   possibly compressed.  Internal only (for now).  */
int
ctf_write_thresholded (ctf_dict_t *fp, int fd, size_t threshold)
{
  unsigned char *buf;
  unsigned char *bp;
  size_t tmp;
  ssize_t buf_len;
  ssize_t len;
  ctf_error_t err = 0;

  if ((buf = ctf_write_mem (fp, &tmp, threshold)) == NULL)
    return -1;					/* errno is set for us.  */

  buf_len = tmp;
  bp = buf;

  while (buf_len > 0)
    {
      if ((len = write (fd, bp, buf_len)) < 0)
	{
	  err = ctf_set_errno (fp, errno);
	  ctf_err_warn (fp, 0, 0, _("ctf_compress_write: error writing"));
	  goto ret;
	}
      buf_len -= len;
      bp += len;
    }

ret:
  free (buf);
  return err;
}

/* Compress the specified CTF data stream and write it to the specified file
   descriptor.  */
int
ctf_compress_write (ctf_dict_t *fp, int fd)
{
  return ctf_write_thresholded (fp, fd, 0);
}

/* Write the uncompressed CTF data stream to the specified file descriptor.  */
int
ctf_write (ctf_dict_t *fp, int fd)
{
  return ctf_write_thresholded (fp, fd, (size_t) -1);
}
