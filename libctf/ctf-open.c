/* Opening CTF files.
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
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <elf.h>
#include "swap.h"
#include <bfd.h>
#include <zlib.h>

static const ctf_dmodel_t _libctf_models[] = {
  {"ILP32", CTF_MODEL_ILP32, 4, 1, 2, 4, 4},
  {"LP64", CTF_MODEL_LP64, 8, 1, 2, 4, 8},
  {NULL, 0, 0, 0, 0, 0, 0}
};

const char _CTF_SECTION[] = ".ctf";
const char _CTF_NULLSTR[] = "";

/* Version-sensitive accessors.  */

static uint32_t
get_kind_v1 (uint32_t info)
{
  return (CTF_V1_INFO_KIND (info));
}

static uint32_t
get_prefixed_kind_v1 (const ctf_type_t *tp)
{
  return (CTF_V1_INFO_KIND (tp->ctt_info));
}

static uint32_t
get_root_v1 (uint32_t info)
{
  return !!(CTF_V1_INFO_ISROOT (info));
}

static uint32_t
get_vlen_v1 (uint32_t info)
{
  return (CTF_V1_INFO_VLEN (info));
}

static uint32_t
get_prefixed_vlen_v1 (const ctf_type_t *tp)
{
  return (CTF_V1_INFO_VLEN (tp->ctt_info));
}

static uint32_t
get_kind_v2 (uint32_t info)
{
  return (CTF_V2_INFO_KIND (info));
}

static uint32_t
get_prefixed_kind_v2 (const ctf_type_t *tp)
{
  return (CTF_V2_INFO_KIND (tp->ctt_info));
}

static uint32_t
get_root_v2 (uint32_t info)
{
  return !!(CTF_V2_INFO_ISROOT (info));
}

static uint32_t
get_vlen_v2 (uint32_t info)
{
  return (CTF_V2_INFO_VLEN (info));
}

static uint32_t
get_prefixed_vlen_v2 (const ctf_type_t *tp)
{
  return (CTF_V2_INFO_VLEN (tp->ctt_info));
}

static uint32_t
get_kind_v4 (uint32_t info)
{
  return (CTF_INFO_KIND (info));
}

static uint32_t
get_prefixed_kind_v4 (const ctf_type_t *tp)
{
  /* Resolve away as many prefixes as exist.  */

  while (LCTF_IS_PREFIXED_INFO (tp->ctt_info))
    tp++;

  return CTF_INFO_KIND (tp->ctt_info);
}

static uint32_t
get_root_v4 (uint32_t info)
{
  return (CTF_INFO_KIND (info) != CTF_K_CONFLICTING);
}

static uint32_t
get_vlen_v4 (uint32_t info)
{
  return (CTF_INFO_VLEN (info));
}

static uint32_t
get_prefixed_vlen_v4 (const ctf_type_t *tp)
{
  const ctf_type_t *suffix;

  /* Resolve away non-BIG prefixes (which have no affect on vlen).  */

  while (LCTF_IS_PREFIXED_INFO (tp->ctt_info)
	 && CTF_INFO_KIND (tp->ctt_info) != CTF_K_BIG)
    tp++;

  if (!LCTF_IS_PREFIXED_INFO (tp->ctt_info))
    return (CTF_INFO_VLEN (tp->ctt_info));

  suffix = tp + 1;

  /* Special case: CTF_K_FUNC_LINKAGE reuses the vlen field for the linkage: its
     vlen is always zero.  */
  if (CTF_INFO_KIND (suffix->ctt_info) == CTF_K_FUNC_LINKAGE)
    return 0;

  /* CTF_K_BIG.  */
  return (CTF_INFO_VLEN (tp->ctt_info) << 16 | CTF_INFO_VLEN (suffix->ctt_info));
}

static inline ssize_t
get_ctt_size_old (const ctf_dict_t *fp _libctf_unused_,
		  const ctf_type_t *tp _libctf_unused_,
		  ssize_t *sizep, ssize_t *incrementp, size_t lsize,
		  size_t csize, size_t ctf_type_size,
		  size_t ctf_stype_size, size_t ctf_lsize_sent)
{
  ssize_t size, increment;

  if (csize == ctf_lsize_sent)
    {
      size = lsize;
      increment = ctf_type_size;
    }
  else
    {
      size = csize;
      increment = ctf_stype_size;
    }

  if (sizep)
    *sizep = size;
  if (incrementp)
    *incrementp = increment;

  return size;
}

static ssize_t
get_ctt_size_v1 (const ctf_dict_t *fp, const ctf_type_t *tp,
		 ssize_t *sizep, ssize_t *incrementp)
{
  ctf_type_v1_t *t1p = (ctf_type_v1_t *) tp;

  return (get_ctt_size_old (fp, tp, sizep, incrementp,
			    CTF_V3_TYPE_LSIZE (t1p), t1p->ctt_size,
			    sizeof (ctf_type_v1_t), sizeof (ctf_stype_v1_t),
			    CTF_LSIZE_SENT_V1));
}

/* Return the size that a v1 will be once it is converted to v2.  */

ssize_t
get_ctt_size_v2_unconverted (const ctf_dict_t *fp, const ctf_type_t *tp,
			     ssize_t *sizep, ssize_t *incrementp)
{
  ctf_type_v1_t *t1p = (ctf_type_v1_t *) tp;

  return (get_ctt_size_old (fp, tp, sizep, incrementp,
			    CTF_V3_TYPE_LSIZE (t1p), t1p->ctt_size,
			    sizeof (ctf_type_v2_t), sizeof (ctf_stype_v2_t),
			    CTF_LSIZE_SENT));
}

static ssize_t
get_ctt_size_v2 (const ctf_dict_t *fp, const ctf_type_t *tp,
		 ssize_t *sizep, ssize_t *incrementp)
{
  ctf_type_v2_t *t2p = (ctf_type_v2_t *) tp;

  return (get_ctt_size_old (fp, tp, sizep, incrementp,
			    CTF_V3_TYPE_LSIZE (t2p), t2p->ctt_size,
			    sizeof (ctf_type_v2_t), sizeof (ctf_stype_v2_t),
			    CTF_LSIZE_SENT));
}

static ssize_t
get_ctt_size_v4 (const ctf_dict_t *fp _libctf_unused_, const ctf_type_t *tp,
		 ssize_t *sizep, ssize_t *incrementp)
{
  ssize_t size = 0;

  /* Figure out how many prefixes there are, and adjust the size appropriately
     if we pass a BIG.  */

  if (incrementp)
    *incrementp = 0;

  while (LCTF_IS_PREFIXED_INFO (tp->ctt_info))
    {
      if (CTF_INFO_KIND (tp->ctt_info) == CTF_K_BIG)
	size = ((ssize_t) tp->ctt_size) << 32;

      if (incrementp)
	*incrementp += sizeof (ctf_type_t);

      tp++;
    }

  if (incrementp)
    *incrementp += sizeof (ctf_type_t);

  size |= tp->ctt_size;

  if (sizep)
    *sizep = size;

  return size;
}

static ssize_t
get_vbytes_old (ctf_dict_t *fp, unsigned short kind, size_t vlen)
{
  switch (kind)
    {
    case CTF_V3_K_INTEGER:
    case CTF_V3_K_FLOAT:
      return (sizeof (uint32_t));
    case CTF_V3_K_SLICE:
      return (sizeof (ctf_slice_t));
    case CTF_V3_K_ENUM:
      return (sizeof (ctf_enum_t) * vlen);
    case CTF_V3_K_FORWARD:
    case CTF_V3_K_UNKNOWN:
    case CTF_V3_K_POINTER:
    case CTF_V3_K_TYPEDEF:
    case CTF_V3_K_VOLATILE:
    case CTF_V3_K_CONST:
    case CTF_V3_K_RESTRICT:
      return 0;
    default:
      ctf_set_errno (fp, ECTF_CORRUPT);
      ctf_err_warn (fp, 0, 0, _("detected invalid CTF kind: %x"), kind);
      return -1;
    }
}

static ssize_t
get_vbytes_v1 (ctf_dict_t *fp, const ctf_type_t *tp, ssize_t size)
{
  unsigned short kind = CTF_V1_INFO_KIND (tp->ctt_info);
  size_t vlen = CTF_V1_INFO_VLEN (tp->ctt_info);

  switch (kind)
    {
    case CTF_V3_K_ARRAY:
      return (sizeof (ctf_array_v1_t));
    case CTF_V3_K_FUNCTION:
      return (sizeof (unsigned short) * (vlen + (vlen & 1)));
    case CTF_V3_K_STRUCT:
    case CTF_V3_K_UNION:
      if (size < CTF_LSTRUCT_THRESH_V1)
	return (sizeof (ctf_member_v1_t) * vlen);
      else
	return (sizeof (ctf_lmember_v1_t) * vlen);
    }

  return (get_vbytes_old (fp, kind, vlen));
}

static ssize_t
get_vbytes_v2 (ctf_dict_t *fp, const ctf_type_t *tp, ssize_t size)
{
  unsigned short kind = CTF_V2_INFO_KIND (tp->ctt_info);
  size_t vlen = CTF_V2_INFO_VLEN (tp->ctt_info);

  switch (kind)
    {
    case CTF_V3_K_ARRAY:
      return (sizeof (ctf_array_t));
    case CTF_V3_K_FUNCTION:
      return (sizeof (uint32_t) * (vlen + (vlen & 1)));
    case CTF_V3_K_STRUCT:
    case CTF_V3_K_UNION:
      if (size < CTF_LSTRUCT_THRESH)
	return (sizeof (ctf_member_v2_t) * vlen);
      else
	return (sizeof (ctf_lmember_v2_t) * vlen);
    }

  return (get_vbytes_old (fp, kind, vlen));
}

static ssize_t
get_vbytes_v4 (ctf_dict_t *fp, const ctf_type_t *tp,
	       ssize_t size _libctf_unused_)
{
  unsigned short kind = LCTF_KIND (fp, tp);
  ssize_t vlen = LCTF_VLEN (fp, tp);

  switch (kind)
    {
    case CTF_K_INTEGER:
    case CTF_K_FLOAT:
      return (sizeof (uint32_t));
    case CTF_K_SLICE:
      return (sizeof (ctf_slice_t));
    case CTF_K_ENUM:
      return (sizeof (ctf_enum_t) * vlen);
    case CTF_K_ENUM64:
      return (sizeof (ctf_enum64_t) * vlen);
    case CTF_K_ARRAY:
      return (sizeof (ctf_array_t));
    case CTF_K_STRUCT:
    case CTF_K_UNION:
      return (sizeof (ctf_member_t) * vlen);
    case CTF_K_FUNCTION:
      return (sizeof (ctf_param_t) * vlen);
    case CTF_K_VAR:
      return (sizeof (ctf_linkage_t));
    case CTF_K_DATASEC:
      return (sizeof (ctf_var_secinfo_t) * vlen);
    case CTF_K_DECL_TAG:
      return (sizeof (ctf_decl_tag_t));
    case CTF_K_TYPE_TAG:
    case CTF_K_FORWARD:
    case CTF_K_UNKNOWN:
    case CTF_K_POINTER:
    case CTF_K_TYPEDEF:
    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
    case CTF_K_FUNC_LINKAGE:
    case CTF_K_BTF_FLOAT:
      return 0;
    /* These should have been resolved away by LCTF_KIND.
       If this somehow didn't work, fail.  */
    case CTF_K_BIG:
    case CTF_K_CONFLICTING:
      ctf_set_errno (fp, ECTF_INTERNAL);
      ctf_err_warn (fp, 0, 0, _("internal error: prefixed kind seen in get_vbytes_v4: %x"), kind);
      return -1;
    default:
      ctf_set_errno (fp, ECTF_CORRUPT);
      ctf_err_warn (fp, 0, 0, _("detected invalid CTF kind: %x"), kind);
      return -1;
    }
}

static const ctf_dictops_t ctf_dictops[] = {
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  /* CTF_VERSION_1 */
  {get_kind_v1, get_prefixed_kind_v1, get_root_v1, get_vlen_v1,
   get_prefixed_vlen_v1, get_ctt_size_v1, get_vbytes_v1},
  /* CTF_VERSION_1_UPGRADED_3 */
  {get_kind_v2, get_prefixed_kind_v2, get_root_v2, get_vlen_v2,
   get_prefixed_vlen_v2, get_ctt_size_v2, get_vbytes_v2},
  /* CTF_VERSION_2 */
  {get_kind_v2, get_prefixed_kind_v2, get_root_v2, get_vlen_v2,
   get_prefixed_vlen_v2, get_ctt_size_v2, get_vbytes_v2},
  /* CTF_VERSION_3, identical to 2: only new type kinds */
  {get_kind_v2, get_prefixed_kind_v2, get_root_v2, get_vlen_v2,
   get_prefixed_vlen_v2, get_ctt_size_v2, get_vbytes_v2},
  /* CTF_VERSION_4, always BTF-compatible.  */
  {get_kind_v4, get_prefixed_kind_v4, get_root_v4, get_vlen_v4,
   get_prefixed_vlen_v4, get_ctt_size_v4, get_vbytes_v4},
};

/* Initialize the symtab translation table as appropriate for its indexing
   state.  For unindexed symtypetabs, fill each entry with the offset of the CTF
   type or function data corresponding to each STT_FUNC or STT_OBJECT entry in
   the symbol table.  For indexed symtypetabs, do nothing: the needed
   initialization for indexed lookups may be quite expensive, so it is done only
   as needed, when lookups happen.  (In particular, the majority of indexed
   symtypetabs come from the compiler, and all the linker does is iteration over
   all entries, which doesn't need this initialization.)

   The SP symbol table section may be NULL if there is no symtab.

   If init_symtab works on one call, it cannot fail on future calls to the same
   fp: ctf_symsect_endianness relies on this.  */

static int
init_symtab (ctf_dict_t *fp, const ctf_header_t *hp, const ctf_sect_t *sp)
{
  const unsigned char *symp;
  int skip_func_info = 0;
  int i;
  uint32_t *xp = fp->ctf_sxlate;
  uint32_t *xend = PTR_ADD (xp, fp->ctf_nsyms);

  uint32_t objtoff = hp->cth_objt_off;
  uint32_t funcoff = hp->cth_func_off;

  /* If this is a v3 dict, and the CTF_F_NEWFUNCINFO flag is not set, pretend
     the func info section is empty: this compiler is too old to emit a function
     info section we understand.  */

  if (fp->ctf_v3_header && (fp->ctf_v3_header->cth_flags & CTF_F_NEWFUNCINFO))
    skip_func_info = 1;

  if (hp->cth_objtidx_len > 0)
    fp->ctf_objtidx_names = (uint32_t *) (fp->ctf_buf + hp->cth_objtidx_off);
  if (hp->cth_funcidx_len > 0 && !skip_func_info)
    fp->ctf_funcidx_names = (uint32_t *) (fp->ctf_buf + hp->cth_funcidx_off);

  /* Don't bother doing the rest if everything is indexed, or if we don't have a
     symbol table: we will never use it.  */
  if ((fp->ctf_objtidx_names && fp->ctf_funcidx_names) || !sp || !sp->cts_data)
    return 0;

  /* The CTF data object and function type sections are ordered to match the
     relative order of the respective symbol types in the symtab, unless there
     is an index section, in which case the order is arbitrary and the index
     gives the mapping.  If no type information is available for a symbol table
     entry, a pad is inserted in the CTF section.  As a further optimization,
     anonymous or undefined symbols are omitted from the CTF data.  If an
     index is available for function symbols but not object symbols, or vice
     versa, we populate the xslate table for the unindexed symbols only.  */

  for (i = 0, symp = sp->cts_data; xp < xend; xp++, symp += sp->cts_entsize,
	 i++)
    {
      ctf_link_sym_t sym;

      switch (sp->cts_entsize)
	{
	case sizeof (Elf64_Sym):
	  {
	    const Elf64_Sym *symp64 = (Elf64_Sym *) (uintptr_t) symp;
	    ctf_elf64_to_link_sym (fp, &sym, symp64, i);
	  }
	  break;
	case sizeof (Elf32_Sym):
	  {
	    const Elf32_Sym *symp32 = (Elf32_Sym *) (uintptr_t) symp;
	    ctf_elf32_to_link_sym (fp, &sym, symp32, i);
	  }
	  break;
	default:
	  return ECTF_SYMTAB;
	}

      /* This call may be led astray if our idea of the symtab's endianness is
	 wrong, but when this is fixed by a call to ctf_symsect_endianness,
	 init_symtab will be called again with the right endianness in
	 force.  */
      if (ctf_symtab_skippable (&sym))
	{
	  *xp = -1u;
	  continue;
	}

      switch (sym.st_type)
	{
	case STT_OBJECT:
	  if (fp->ctf_objtidx_names || (objtoff - hp->cth_objt_off) > hp->cth_objt_len)
	    {
	      *xp = -1u;
	      break;
	    }

	  *xp = objtoff;
	  objtoff += sizeof (uint32_t);
	  break;

	case STT_FUNC:
	  if (fp->ctf_funcidx_names || (funcoff - hp->cth_func_off) > hp->cth_func_len
	      || skip_func_info)
	    {
	      *xp = -1u;
	      break;
	    }

	  *xp = funcoff;
	  funcoff += sizeof (uint32_t);
	  break;

	default:
	  *xp = -1u;
	  break;
	}
    }

  ctf_dprintf ("loaded %lu symtab entries\n", fp->ctf_nsyms);
  return 0;
}

/* Reset the CTF base pointer and derive the buf pointer from it, initializing
   everything in the ctf_dict that depends on the base or buf pointers.

   The original gap between the buf and base pointers, if any -- the original,
   unconverted CTF header -- is kept, but its contents are not specified and are
   never used.  */

static void
ctf_set_base (ctf_dict_t *fp, const ctf_header_t *hp, unsigned char *base)
{
  fp->ctf_buf = base + (fp->ctf_buf - fp->ctf_base);
  fp->ctf_base = base;

  fp->ctf_str[CTF_STRTAB_0].cts_strs = (const char *) fp->ctf_buf
    + hp->btf.bth_str_off;
  fp->ctf_str[CTF_STRTAB_0].cts_len = hp->btf.bth_str_len;

  /* If we have a parent dict name and label, store the relocated string
     pointers in the CTF dict for easy access later. */

  /* Note: before conversion, these will be set to values that will be
     immediately invalidated by the conversion process, but the conversion
     process will call ctf_set_base() again to fix things up.

     These labels are explicitly constrained from being deduplicated (even though
     .ctf is usually a duplicated name), because they are the key to identifying
     the parent dict (and determining that this dict is a child) in the first
     place.  */

  if (hp->cth_parent_name != 0)
    fp->ctf_parent_name = ctf_strptr (fp, hp->cth_parent_name);
  if (hp->cth_cu_name != 0)
    fp->ctf_cu_name = ctf_strptr (fp, hp->cth_cu_name);

  if (fp->ctf_cu_name)
    ctf_dprintf ("ctf_set_base: CU name %s\n", fp->ctf_cu_name);
  if (fp->ctf_parent_name)
    ctf_dprintf ("ctf_set_base: parent name %s\n", fp->ctf_parent_name);
}

static int
init_static_types_names (ctf_dict_t *fp, ctf_header_t *cth, int is_btf);

/* Populate statically-defined types (those loaded from a saved buffer).

   Initialize the type ID translation table with the byte offset of each type,
   and initialize the hash tables of each named type.  Upgrade the type table to
   the latest supported representation in the process, if needed, and if this
   recension of libctf supports upgrading.

   Returns zero on success and a *positive* ECTF_* or errno value on error.

   This is a wrapper to simplify memory allocation on error in the _internal
   function that does all the actual work.  */

static int
init_static_types (ctf_dict_t *fp, ctf_header_t *cth, int is_btf)
{
  const ctf_type_t *tbuf;
  const ctf_type_t *tend;

  unsigned long pop[CTF_K_MAX + 1] = { 0 };
  int pop_enumerators = 0;
  const ctf_type_t *tp;
  unsigned long typemax = 0;

  /* Provisional types always start from the top of the type space and work
     down.  */

  fp->ctf_provtypemax = (uint32_t) -1;

  /* We determine whether the dict is a child or a parent based on the value of
     cth_parent_name: for BTF this is not enough, because there is no
     cth_parent_name: instead, we can check the first byte of the strtab, which
     is always 0 for parents and never 0 for children.  */

  int child = (cth->cth_parent_name != 0
	       || (fp->ctf_str[CTF_STRTAB_0].cts_len > 0
		   && fp->ctf_str[CTF_STRTAB_0].cts_strs[0] != 0));

  if (fp->ctf_version < CTF_VERSION_4)
    {
#if 0
      int err;

      if ((err = upgrade_types (fp, cth)) != 0)
	return err;				/* Upgrade failed.  */
#endif
      ctf_err_warn (NULL, 0, ECTF_INTERNAL, "Implementation of backward-compatible CTF reading still underway\n");
      return ECTF_INTERNAL;
    }

  tbuf = (ctf_type_t *) (fp->ctf_buf + cth->btf.bth_type_off);
  tend = (ctf_type_t *) ((uintptr_t) tbuf + cth->btf.bth_type_len);

  /* We make two passes through the entire type section, and more passes through
     parts of it: but only the first is guaranteed to happen at this stage.  The
     later passes require working string lookup, so in child dicts can only
     happen at ctf_import time.

     For prefixed kinds, we are interested in the thing we are prefixing:
     that is the true type.

     In this first pass, we count the number of each type and type-like
     identifier (like enumerators) and the total number of types.  */

  for (tp = tbuf; tp < tend; typemax++)
    {
      unsigned short kind = LCTF_INFO_UNPREFIXED_KIND (fp, tp->ctt_info);
      size_t vlen = LCTF_VLEN (fp, tp);
      int nonroot = 0;
      ssize_t size, increment, vbytes;
      const ctf_type_t *suffix = tp;

      (void) ctf_get_ctt_size (fp, tp, &size, &increment);

      /* Go to the first unprefixed type, incrementing all prefixed types'
	 popcounts along the way.  If we find a CTF_K_CONFLICTING, stop: these
	 counts are used to increment identifier hashtab sizes, and conflicting
	 types do not appear in identifier hashtabs.  */

      while (LCTF_IS_PREFIXED_KIND (kind))
	{
	  if (is_btf)
	    return ECTF_CORRUPT;

	  pop[suffix->ctt_type]++;

	  if (kind == CTF_K_CONFLICTING)
	    {
	      nonroot = 1;
	      break;
	    }

	  suffix++;
	  if (suffix >= tend)
	    return ECTF_CORRUPT;

	  kind = LCTF_INFO_UNPREFIXED_KIND (fp, suffix->ctt_info);
	}

      if (is_btf && kind > CTF_BTF_K_MAX)
	return ECTF_CORRUPT;

      vbytes = LCTF_VBYTES (fp, suffix, size);

      if (vbytes < 0)
	return ECTF_CORRUPT;

      if (!nonroot)
	{
	  /* For forward declarations, the kflag indicates what type to use, so bump
	     that population count too.  For enums, vlen 0 indicates a forward, so
	     bump the forward population count.  */
	  if (kind == CTF_K_FORWARD)
	    {
	      if (CTF_INFO_KFLAG (suffix->ctt_info))
		pop[CTF_K_UNION]++;
	      else
		pop[CTF_K_STRUCT]++;
	    }
	  else if (kind == CTF_K_ENUM && vlen == 0)
	    pop[CTF_K_FORWARD]++;

	  if (kind == CTF_K_ENUM || kind == CTF_K_ENUM64)
	    pop_enumerators += vlen;

	  pop[kind]++;
	}

      tp = (ctf_type_t *) ((uintptr_t) tp + increment + vbytes);
    }

  if (child)
    {
      ctf_dprintf ("CTF dict %p is a child\n", (void *) fp);
      fp->ctf_flags |= LCTF_CHILD;
    }
  else
    ctf_dprintf ("CTF dict %p is a parent\n", (void *) fp);

  /* Now that we've counted up the number of each type, we can allocate
     the hash tables, type translation table, and pointer table.  */

  if ((fp->ctf_structs
       = ctf_dynhash_create_sized (pop[CTF_K_STRUCT], ctf_hash_string,
				   ctf_hash_eq_string,
				   NULL, NULL, NULL)) == NULL)
    return ENOMEM;

  if ((fp->ctf_unions
       = ctf_dynhash_create_sized (pop[CTF_K_UNION], ctf_hash_string,
				   ctf_hash_eq_string,
				   NULL, NULL, NULL)) == NULL)
    return ENOMEM;

  if ((fp->ctf_enums
       = ctf_dynhash_create_sized (pop[CTF_K_ENUM], ctf_hash_string,
				   ctf_hash_eq_string,
				   NULL, NULL, NULL)) == NULL)
    return ENOMEM;

  if ((fp->ctf_datasecs
       = ctf_dynhash_create_sized (pop[CTF_K_DATASEC], ctf_hash_string,
				   ctf_hash_eq_string,
				   NULL, NULL, NULL)) == NULL)
    return ENOMEM;

  if ((fp->ctf_tags
       = ctf_dynhash_create_sized (pop[CTF_K_DECL_TAG] + pop [CTF_K_TYPE_TAG],
				   ctf_hash_string, ctf_hash_eq_string,
				   NULL, (ctf_hash_free_arg_fun) ctf_dynset_destroy_arg,
				   NULL)) == NULL)
    return ENOMEM;

  if ((fp->ctf_names
       = ctf_dynhash_create_sized (pop[CTF_K_UNKNOWN] +
				   pop[CTF_K_INTEGER] +
				   pop[CTF_K_FLOAT] +
				   pop[CTF_K_FUNC_LINKAGE] +
				   pop[CTF_K_TYPEDEF] +
				   pop_enumerators,
				   ctf_hash_string,
				   ctf_hash_eq_string,
				   NULL, NULL, NULL)) == NULL)
    return ENOMEM;

  if ((fp->ctf_var_datasecs
       = ctf_dynhash_create (htab_hash_pointer, htab_eq_pointer, NULL, NULL))
      == NULL)
    return ENOMEM;

  if ((fp->ctf_conflicting_enums
       = ctf_dynset_create (htab_hash_string, htab_eq_string, NULL)) == NULL)
    return ENOMEM;

  /* The ptrtab and txlate can be appropriately sized for precisely this set
     of types: the txlate because it is only used to look up static types,
     so dynamic types added later will never go through it, and the ptrtab
     because later-added types will call grow_ptrtab() automatically, as
     needed.  */

  fp->ctf_txlate = calloc (typemax + 1, sizeof (ctf_type_t *));
  fp->ctf_ptrtab = calloc (typemax + 1, sizeof (uint32_t));
  fp->ctf_ptrtab_len = typemax + 1;
  fp->ctf_stypes = typemax;
  fp->ctf_typemax = typemax;
  fp->ctf_idmax = typemax;

  if (fp->ctf_txlate == NULL || fp->ctf_ptrtab == NULL)
    return ENOMEM;		/* Memory allocation failed.  */

  if (child || cth->cth_parent_strlen != 0)
    {
      fp->ctf_flags |= LCTF_NO_STR;
      return 0;
    }

  ctf_dprintf ("%u types initialized (other than names)\n", fp->ctf_typemax);

  return init_static_types_names (fp, cth, is_btf);
}

static int
init_static_types_names_internal (ctf_dict_t *fp, ctf_header_t *cth, int is_btf,
				  ctf_dynset_t *all_enums);

/* A wrapper to simplify memory allocation.  */

static int
init_static_types_names (ctf_dict_t *fp, ctf_header_t *cth, int is_btf)
{
  ctf_dynset_t *all_enums;
  int err;

  if ((all_enums = ctf_dynset_create (htab_hash_pointer, htab_eq_pointer,
				      NULL)) == NULL)
    return ENOMEM;

  err = init_static_types_names_internal (fp, cth, is_btf, all_enums);
  ctf_dynset_destroy (all_enums);
  return err;
}

static int
init_void (ctf_dict_t *fp);

/* Initialize the parts of the CTF dict whose initialization depends on name or
   type lookup.  This happens at open time except for child dicts, when (for
   CTFv4+ dicts) it happens at ctf_import time instead, because before then the
   strtab and type tables are truncated at the start.

   As a function largely called at open time, this function does not reliably
   set the ctf_errno, but instead returns a positive error code.  */

static int
init_static_types_names_internal (ctf_dict_t *fp, ctf_header_t *cth, int is_btf,
				  ctf_dynset_t *all_enums)
{
  ctf_type_t *tbuf, *tend, *tp;
  ctf_type_t **xp;

  uint32_t id;
  ctf_id_t type;

  ctf_next_t *i = NULL;
  void *k;
  int err;

  /* See init_static_types.  */
  int child = (cth->cth_parent_name != 0
	       || (fp->ctf_str[CTF_STRTAB_0].cts_len > 0
		   && fp->ctf_str[CTF_STRTAB_0].cts_strs[0] != 0));

  tbuf = (ctf_type_t *) (fp->ctf_buf + cth->btf.bth_type_off);
  tend = (ctf_type_t *) ((uintptr_t) tbuf + cth->btf.bth_type_len);

  assert (!(fp->ctf_flags & LCTF_NO_STR));

  xp = &fp->ctf_txlate[1];

  /* In this second pass through the types, we fill in each entry of the type
     and pointer tables and add names to the appropriate hashes.

     (Not all names are added in this pass, only type names.  See below.)  */

  for (id = 1, tp = tbuf; tp < tend; xp++, id++)
    {
      unsigned short kind = LCTF_KIND (fp, tp);
      unsigned short isroot = LCTF_INFO_ISROOT (fp, tp->ctt_info);
      size_t vlen = LCTF_VLEN (fp, tp);
      ssize_t size, increment, vbytes;
      const ctf_type_t *suffix = tp;
      const char *name;

      /* Prefixed type: pull off the prefixes (for most purposes).  (We already
	 know the prefixes cannot overflow.)  */

      while (LCTF_IS_PREFIXED_INFO (suffix->ctt_info))
	{
	  if (is_btf)
	    return ECTF_CORRUPT;

	  suffix++;
	}

      (void) ctf_get_ctt_size (fp, tp, &size, &increment);
      name = ctf_strptr (fp, suffix->ctt_name);
      /* Cannot fail: shielded by call in init_static_types.  */
      vbytes = LCTF_VBYTES (fp, suffix, size);

      *xp = tp;

      switch (kind)
	{
	case CTF_K_UNKNOWN:
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  {
	    ctf_id_t existing;
	    ctf_encoding_t existing_en;
	    ctf_encoding_t this_en;

	    if (!isroot)
	      break;

	    /* Names are reused by bitfields, which are differentiated by
	       their encodings.  So check for the type already existing, and
	       iff the new type is a root-visible non-bitfield, replace the
	       old one.  It's a little hard to figure out whether a type is
	       a non-bitfield without already knowing that type's native
	       width, but we can converge on it by replacing an existing
	       type as long as the new type is zero-offset and has a
	       bit-width wider than the existing one, since the native type
	       must necessarily have a bit-width at least as wide as any
	       bitfield based on it.

	       BTF floats are more or less useless, having no encoding:
	       the ctf_type_encoding check here suffices to replace them.  */

	    if (((existing = ctf_dynhash_lookup_type (fp->ctf_names, name)) == 0)
		|| ctf_type_encoding (fp, existing, &existing_en) != 0
		|| (ctf_type_encoding (fp, ctf_index_to_type (fp, id), &this_en) == 0
		    && this_en.cte_offset == 0
		    && (existing_en.cte_offset != 0
			|| existing_en.cte_bits < this_en.cte_bits)))
	      {
		err = ctf_dynhash_insert_type (fp, fp->ctf_names,
					       ctf_index_to_type (fp, id),
					       suffix->ctt_name);
		if (err != 0)
		  return err * -1;
	      }
	    break;
	  }

	case CTF_K_BTF_FLOAT:
	  {
	    ctf_id_t existing;

	    if (!isroot)
	      break;

	    /* Don't allow a float to be overwritten by a BTF float.  */

	    if (((existing = ctf_dynhash_lookup_type (fp->ctf_names, name)) == 0)
		&& ctf_type_kind (fp, existing) == CTF_K_FLOAT)
	      break;

	    err = ctf_dynhash_insert_type (fp, fp->ctf_names,
					   ctf_index_to_type (fp, id),
					   suffix->ctt_name);
	    if (err != 0)
	      return err * -1;
	    break;
	  }

	  /* These kinds have no name, so do not need interning into any
	     hashtables.  */
	case CTF_K_ARRAY:
	case CTF_K_SLICE:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	case CTF_K_FUNCTION:
	  break;

	case CTF_K_FUNC_LINKAGE:
	  if (!isroot)
	    break;

	  err = ctf_dynhash_insert_type (fp, fp->ctf_names,
					 ctf_index_to_type (fp, id),
					 suffix->ctt_name);
	  if (err != 0)
	    return err * -1;
	  break;

	case CTF_K_STRUCT:
	  if (!isroot)
	    break;

	  err = ctf_dynhash_insert_type (fp, fp->ctf_structs,
					 ctf_index_to_type (fp, id),
					 suffix->ctt_name);

	  if (err != 0)
	    return err * -1;

	  break;

	case CTF_K_UNION:
	  if (!isroot)
	    break;

	  err = ctf_dynhash_insert_type (fp, fp->ctf_unions,
					 ctf_index_to_type (fp, id),
					 suffix->ctt_name);

	  if (err != 0)
	    return err * -1;
	  break;

	case CTF_K_ENUM:
	case CTF_K_ENUM64:
	  {
	    if (!isroot)
	      break;

	    /* Only insert forward types if the type is not already present.  */
	    if (vlen == 0
		&& ctf_dynhash_lookup_type (ctf_name_table (fp, kind),
					    name) != 0)
	      break;

	    err = ctf_dynhash_insert_type (fp, fp->ctf_enums,
					   ctf_index_to_type (fp, id),
					   suffix->ctt_name);

	    if (err != 0)
	      return err * -1;

	    /* Remember all non-forward enums for later rescanning.  */

	    if (vlen != 0)
	      {
		err = ctf_dynset_insert (all_enums, (void *) (ptrdiff_t)
					 ctf_index_to_type (fp, id));
		if (err != 0)
		  return err * -1;
	      }
	    break;
	  }

	case CTF_K_TYPEDEF:
	  if (!isroot)
	    break;

	  err = ctf_dynhash_insert_type (fp, fp->ctf_names,
					 ctf_index_to_type (fp, id),
					 suffix->ctt_name);
	  if (err != 0)
	    return err * -1;
	  break;

	case CTF_K_VAR:
	  if (!isroot)
	    break;

	  err = ctf_dynhash_insert_type (fp, fp->ctf_names,
					 ctf_index_to_type (fp, id),
					 suffix->ctt_name);
	  if (err != 0)
	    return err * -1;
	  break;

	case CTF_K_DATASEC:
	  if (!isroot)
	    break;

	  err = ctf_dynhash_insert_type (fp, fp->ctf_datasecs,
					 ctf_index_to_type (fp, id),
					 suffix->ctt_name);
	  if (err != 0)
	    return err * -1;

	  break;

	case CTF_K_DECL_TAG:
	case CTF_K_TYPE_TAG:
	  {
	    const char *str;

	    if ((str = ctf_strptr_validate (fp, suffix->ctt_name)) == NULL)
	      return ctf_errno (fp);

	    if (!isroot)
	      break;

	    if (ctf_insert_type_decl_tag (fp, id, name, kind) < 0)
	      return ctf_errno (fp);

	    break;
	  }

	case CTF_K_FORWARD:
	  {
	    int kflag = CTF_INFO_KFLAG (tp->ctt_info);
	    ctf_dynhash_t *h = ctf_name_table (fp, kflag == 1
					       ? CTF_K_UNION : CTF_K_STRUCT);

	    if (!isroot)
	      break;

	    /* Only insert forward tags into the given hash if the type or tag
	       name is not already present.  */
	    if (ctf_dynhash_lookup_type (h, name) == 0)
	      {
		err = ctf_dynhash_insert_type (fp, h, ctf_index_to_type (fp, id),
					       suffix->ctt_name);
		if (err != 0)
		  return err * -1;
	      }
	    break;
	  }

	case CTF_K_POINTER:
	  /* If the type referenced by the pointer is in this CTF dict, then
	     store the index of the pointer type in fp->ctf_ptrtab[ index of
	     referenced type ].  */

	  if (ctf_type_ischild (fp, suffix->ctt_type) == child
	      && ctf_type_to_index (fp, suffix->ctt_type) <= fp->ctf_typemax)
	    fp->ctf_ptrtab[ctf_type_to_index (fp, suffix->ctt_type)] = id;

	  break;

	default:
	  ctf_err_warn (fp, 0, ECTF_CORRUPT,
			_("init_static_types(): unhandled CTF kind: %x"), kind);
	  return ECTF_CORRUPT;
	}
      tp = (ctf_type_t *) ((uintptr_t) tp + increment + vbytes);
    }
  assert (fp->ctf_typemax == id - 1);

  ctf_dprintf ("%u total types processed\n", id - 1);

  if ((err = init_void (fp) < 0))
    return err;

  /* In the third pass, we traverse the enums we spotted earlier and track all
     the enumeration constants to aid in future detection of duplicates.

     Doing this in a third pass is necessary to avoid the case where an
     enum appears with a constant FOO, then later a type named FOO appears,
     too late to spot the conflict by checking the enum's constants.  */

  ctf_dprintf ("Extracting enumeration constants from %zu enums\n",
	       ctf_dynset_elements (all_enums));

  while ((err = ctf_dynset_next (all_enums, &i, &k)) == 0)
    {
      ctf_id_t enum_id = (uintptr_t) k;
      ctf_next_t *i_constants = NULL;
      const char *cte_name;

      while ((cte_name = ctf_enum_next (fp, enum_id, &i_constants, NULL)) != NULL)
	{
	  if (ctf_track_enumerator (fp, enum_id, cte_name) < 0)
	    {
	      ctf_next_destroy (i_constants);
	      ctf_next_destroy (i);
	      return ctf_errno (fp);
	    }
	}
      if (ctf_errno (fp) != ECTF_NEXT_END)
	{
	  ctf_next_destroy (i);
	  return ctf_errno (fp);
	}
    }
  if (err != ECTF_NEXT_END)
    return err;

  /* In the final pass, we traverse all datasecs and remember the variables in
     each, so we can rapidly map from variable back to datasec.  */

  ctf_dprintf ("Getting variable datasec membership\n");

  while ((type = ctf_type_kind_next (fp, &i, CTF_K_DATASEC)) != CTF_ERR)
    {
      ctf_next_t *i_sec = NULL;
      ctf_id_t var_type;

      while ((var_type = ctf_datasec_var_next (fp, type, &i_sec, NULL, NULL))
	     != CTF_ERR)
	{
	  err = ctf_dynhash_insert (fp->ctf_var_datasecs,
				    (void *) (ptrdiff_t) var_type,
				    (void *) (ptrdiff_t) type);
	  if (err != 0)
	    return err * -1;
	}
      if (ctf_errno (fp) != ECTF_NEXT_END)
	{
	  ctf_next_destroy (i);
	  return ctf_errno (fp);
	}
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    return ctf_errno (fp);

  ctf_dprintf ("%zu enum names hashed\n",
	       ctf_dynhash_elements (fp->ctf_enums));
  ctf_dprintf ("%zu conflicting enumerators identified\n",
	       ctf_dynset_elements (fp->ctf_conflicting_enums));
  ctf_dprintf ("%zu struct names hashed\n",
	       ctf_dynhash_elements (fp->ctf_structs));
  ctf_dprintf ("%zu union names hashed\n",
	       ctf_dynhash_elements (fp->ctf_unions));
  ctf_dprintf ("%zu base type names and identifiers hashed\n",
	       ctf_dynhash_elements (fp->ctf_names));

  return 0;
}

/* Prepare the void type.  If present, index 0 is pointed at it: otherwise, we
   make one in the ctf_void_type member and point index 0 at that.  Because this
   is index 0, it is not written out by serialization (which always starts at
   index 1): because it is type 0, it is the type expected by BTF consumers:
   because it is a real, queryable type, CTF consumers will get a proper type
   back that they can query the properties of.

   As an initialization function, this returns a positive error code, or
   zero.  */

static int
init_void (ctf_dict_t *fp)
{
  ctf_id_t void_type;
  ctf_type_t *void_tp;

  void_type = ctf_dynhash_lookup_type (ctf_name_table (fp, CTF_K_INTEGER), "void");

  if (void_type == 0)
    {
      uint32_t *vlen;

      if ((void_tp = calloc (1, sizeof (ctf_type_t) + sizeof (uint32_t))) == NULL)
	return ENOMEM;
      vlen = (uint32_t *) (void_tp + 1);

      void_tp->ctt_name = ctf_str_add (fp, "void");
      void_tp->ctt_info = CTF_TYPE_INFO (CTF_K_INTEGER, 0, 0);
      void_tp->ctt_size = 4;			/* (bytes)  */
      *vlen = CTF_INT_DATA (CTF_INT_SIGNED, 0, 0);

      fp->ctf_void_type = void_tp;
    }
  else
    {
      ctf_dict_t *tmp = fp;

      void_tp = (ctf_type_t *) ctf_lookup_by_id (&tmp, void_type, NULL);
      assert (void_tp != NULL);
    }

  fp->ctf_txlate[0] = void_tp;

  return 0;
}

/* Endianness-flipping routines.

   We flip everything, mindlessly, even 1-byte entities, so that future
   expansions do not require changes to this code.  */

/* Flip the endianness of the CTF header.  */

int
ctf_flip_header (void *cthp, int is_btf, int ctf_version)
{
  ctf_header_t *cth = (ctf_header_t *) cthp;

  if (is_btf)
    {
      ctf_btf_header_t *bth = (ctf_btf_header_t *) cthp;

      swap_thing (bth->bth_preamble.btf_magic);
      swap_thing (bth->bth_preamble.btf_version);
      swap_thing (bth->bth_preamble.btf_flags);
      swap_thing (bth->bth_hdr_len);
      swap_thing (bth->bth_type_off);
      swap_thing (bth->bth_type_len);
      swap_thing (bth->bth_str_off);
      swap_thing (bth->bth_str_len);

      /* Raw BTF?  */
      if (ctf_version == 1)
	return 0;
    }
  else
    {
#if 0
      ctf_header_v2_t *h2p = (ctf_header_v2_t *) cthp;
      ctf_header_v3_t *h3p = (ctf_header_v3_t *) cthp;
#endif

      /* Non-BTF-compatible: old CTF release.  */
      switch (ctf_version) {
      case CTF_VERSION_1:
      case CTF_VERSION_1_UPGRADED_3:
      case CTF_VERSION_2:
	ctf_err_warn (NULL, 0, ECTF_INTERNAL, "Implementation of backward-compatible CTF reading still underway\n");
	return -ECTF_INTERNAL;
/*	ctf_flip_header_v2 (h2p); */
      break;
      case CTF_VERSION_3:
	ctf_err_warn (NULL, 0, ECTF_INTERNAL, "Implementation of backward-compatible CTF reading still underway\n");
	return -ECTF_INTERNAL;

/*	ctf_flip_header_v3 (h3p); */
      }
      return 0;
    }

  /* CTFv4.  */

  swap_thing (cth->cth_preamble.ctp_magic_version);
  swap_thing (cth->cth_preamble.ctp_flags);
  swap_thing (cth->cth_cu_name);
  swap_thing (cth->cth_parent_name);
  swap_thing (cth->cth_parent_strlen);
  swap_thing (cth->cth_parent_ntypes);
  swap_thing (cth->cth_objt_off);
  swap_thing (cth->cth_objt_len);
  swap_thing (cth->cth_func_off);
  swap_thing (cth->cth_func_len);
  swap_thing (cth->cth_objtidx_off);
  swap_thing (cth->cth_objtidx_len);
  swap_thing (cth->cth_funcidx_off);
  swap_thing (cth->cth_funcidx_len);

  return 0;
}

/* Flip the endianness of the data-object or function sections or their indexes,
   all arrays of uint32_t.  */

static void
flip_objts (void *start, size_t len)
{
  uint32_t *obj = start;
  ssize_t i;

  for (i = len / sizeof (uint32_t); i > 0; obj++, i--)
      swap_thing (*obj);
}

/* Flip the endianness of the type section, a tagged array of ctf_type followed
   by variable data (which may itself be a ctf_type for prefixed kinds).  */

static int
flip_types (ctf_dict_t *fp, void *start, size_t len, int to_foreign)
{
  ctf_type_t *t = start;

  while ((uintptr_t) t < ((uintptr_t) start) + len)
    {
      uint32_t kind;
      uint32_t raw_kind;
      uint32_t vlen;
      size_t vbytes;
      ctf_type_t *tprefixed;

      if (to_foreign)
	{
	  kind = LCTF_KIND (fp, t);
	  raw_kind = LCTF_INFO_UNPREFIXED_KIND (fp, t->ctt_info);
	  vlen = LCTF_VLEN (fp, t);
	  vbytes = get_vbytes_v4 (fp, t, 0);
	}

      swap_thing (t->ctt_name);
      swap_thing (t->ctt_info);
      swap_thing (t->ctt_size);

      /* Prefixed kind: flip the kind being prefixed too (for as many prefixes
	 as there are).  */
      tprefixed = t;
      while (LCTF_IS_PREFIXED_KIND (raw_kind))
	{
	  tprefixed++;

	  if ((uintptr_t) tprefixed > ((uintptr_t) start) + len)
	    goto overflow;

	  swap_thing (tprefixed->ctt_name);
	  swap_thing (tprefixed->ctt_info);
	  swap_thing (tprefixed->ctt_size);
	  raw_kind = LCTF_INFO_UNPREFIXED_KIND (fp, tprefixed->ctt_info);
	}

      if (!to_foreign)
	{
	  kind = LCTF_KIND (fp, t);
	  raw_kind = LCTF_INFO_UNPREFIXED_KIND (fp, t->ctt_info);
	  vlen = LCTF_VLEN (fp, t);
	  vbytes = get_vbytes_v4 (fp, t, 0);
	}

      /* Move on to the vlen region.  */
      raw_kind = LCTF_INFO_UNPREFIXED_KIND (fp, t->ctt_info);
      while (LCTF_IS_PREFIXED_KIND (raw_kind))
	{
	  t++;
	  raw_kind = LCTF_INFO_UNPREFIXED_KIND (fp, t->ctt_info);
	}

      if (((uintptr_t) t) + sizeof (ctf_type_t) + vbytes
	  > ((uintptr_t) start) + len)
	goto overflow;

      t++;

      switch (kind)
	{
	case CTF_K_FORWARD:
	case CTF_K_UNKNOWN:
	case CTF_K_POINTER:
	case CTF_K_TYPEDEF:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	case CTF_K_TYPE_TAG:
	case CTF_K_BTF_FLOAT:
	case CTF_K_FUNC_LINKAGE:
	  /* These kinds have no vlen data to swap.  */
	  assert (vbytes == 0);
	  break;

	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  {
	    /* These kinds have a single uint32_t.  */

	    uint32_t *item = (uint32_t *) t;

	    swap_thing (*item);
	    break;
	  }

	case CTF_K_VAR:
	  {
	    /* This has a single ctf_linkage_t.  */
	    ctf_linkage_t *l = (ctf_linkage_t *) t;

	    swap_thing (l->ctl_linkage);
	    break;
	  }

	case CTF_K_DECL_TAG:
	  {
	    /* This has a single ctf_decl_tag_t.  */
	    ctf_decl_tag_t *d = (ctf_decl_tag_t *) t;

	    swap_thing (d->cdt_component_idx);
	    break;
	  }

	case CTF_K_FUNCTION:
	  {
	    /* This kind has a bunch of ctf_param_t's.  */

	    ctf_param_t *p = (ctf_param_t *) t;
	    ssize_t i;

	    for (i = vlen; i > 0; p++, i--)
	      {
		swap_thing (p->cfp_name);
		swap_thing (p->cfp_type);
	      }
	    break;
	  }
	case CTF_K_ARRAY:
	  {
	    /* This has a single ctf_array_t.  */

	    ctf_array_t *a = (ctf_array_t *) t;

	    assert (vbytes == sizeof (ctf_array_t));
	    swap_thing (a->cta_contents);
	    swap_thing (a->cta_index);
	    swap_thing (a->cta_nelems);

	    break;
	  }

	case CTF_K_SLICE:
	  {
	    /* This has a single ctf_slice_t.  */

	    ctf_slice_t *s = (ctf_slice_t *) t;

	    assert (vbytes == sizeof (ctf_slice_t));
	    swap_thing (s->cts_type);
	    swap_thing (s->cts_offset);
	    swap_thing (s->cts_bits);

	    break;
	  }

	case CTF_K_STRUCT:
	case CTF_K_UNION:
	  {
	    /* This has an array of ctf_member_t: the interpretation differs in
	       prefixed types, but for the purposes of byteswapping we don't
	       care.  We could consider it to be a simple array of uint32_t, but
	       for safety's sake in case these structures ever acquire
	       non-uint32_t members, do it member by member.  */

	    ctf_member_t *m = (ctf_member_t *) t;
	    ssize_t i;
	    for (i = vlen; i > 0; i--, m++)
	      {
		swap_thing (m->ctm_name);
		swap_thing (m->ctm_type);
		swap_thing (m->ctm_offset);
	      }
	    break;
	  }

	case CTF_K_ENUM:
	  {
	    /* This has an array of ctf_enum_t.  */

	    ctf_enum_t *item = (ctf_enum_t *) t;
	    ssize_t i;

	    for (i = vlen; i > 0; item++, i--)
	      {
		swap_thing (item->cte_name);
		swap_thing (item->cte_value);
	      }
	    break;
	  }

	case CTF_K_ENUM64:
	  {
	    /* This has an array of ctf_enum64_t.  */

	    ctf_enum64_t *item = (ctf_enum64_t *) t;
	    ssize_t i;

	    for (i = vlen; i > 0; item++, i--)
	      {
		swap_thing (item->cte_name);
		swap_thing (item->cte_val_low);
		swap_thing (item->cte_val_high);
	      }
	    break;
	  }

	case CTF_K_DATASEC:
	  {
	    /* This has an array of ctf_var_secinfo_t.  */

	    ctf_var_secinfo_t *item = (ctf_var_secinfo_t *) t;
	    ssize_t i;

	    for (i = vlen; i > 0; item++, i--)
	      {
		swap_thing (item->cvs_type);
		swap_thing (item->cvs_offset);
		swap_thing (item->cvs_size);
	      }
	    break;
	  }

	default:
	  ctf_err_warn (fp, 0, ECTF_CORRUPT,
			_("unhandled CTF kind in endianness conversion: %x"),
			kind);
	  return ECTF_CORRUPT;
	}

      t = (ctf_type_t *) ((uintptr_t) t + vbytes);
    }

  return 0;

 overflow:
  ctf_err_warn (fp, 0, ECTF_CORRUPT, _("overflow byteswapping CTF"));
  return ECTF_CORRUPT;
}

/* Flip the endianness of BUF, given the offsets in the (native-endianness) CTH.
   If TO_FOREIGN is set, flip to foreign-endianness; if not, flip away.  */

int
ctf_flip (ctf_dict_t *fp, ctf_header_t *cth, unsigned char *buf,
	  int is_btf, int to_foreign)
{
  ctf_dprintf ("Flipping endianness\n");

  if (!is_btf)
    {
      flip_objts (buf + cth->cth_objt_off, cth->cth_objt_len);
      flip_objts (buf + cth->cth_func_off, cth->cth_func_len);
      flip_objts (buf + cth->cth_objtidx_off, cth->cth_objtidx_len);
      flip_objts (buf + cth->cth_funcidx_off, cth->cth_funcidx_len);
    }
  return flip_types (fp, buf + cth->btf.bth_type_off, cth->btf.bth_type_len,
		     to_foreign);
}

/* Set up the ctl hashes in a ctf_dict_t.  Called by both writable and
   non-writable dictionary initialization.  */
void ctf_set_ctl_hashes (ctf_dict_t *fp)
{
  /* Initialize the ctf_lookup_by_name top-level dictionary.  We keep an
     array of type name prefixes and the corresponding ctf_hash to use.  */
  fp->ctf_lookups[0].ctl_prefix = "struct";
  fp->ctf_lookups[0].ctl_len = strlen (fp->ctf_lookups[0].ctl_prefix);
  fp->ctf_lookups[0].ctl_hash = fp->ctf_structs;
  fp->ctf_lookups[1].ctl_prefix = "union";
  fp->ctf_lookups[1].ctl_len = strlen (fp->ctf_lookups[1].ctl_prefix);
  fp->ctf_lookups[1].ctl_hash = fp->ctf_unions;
  fp->ctf_lookups[2].ctl_prefix = "enum";
  fp->ctf_lookups[2].ctl_len = strlen (fp->ctf_lookups[2].ctl_prefix);
  fp->ctf_lookups[2].ctl_hash = fp->ctf_enums;
  fp->ctf_lookups[3].ctl_prefix = "datasec";
  fp->ctf_lookups[3].ctl_len = strlen (fp->ctf_lookups[3].ctl_prefix);
  fp->ctf_lookups[3].ctl_hash = fp->ctf_datasecs;
  fp->ctf_lookups[4].ctl_prefix = _CTF_NULLSTR;
  fp->ctf_lookups[4].ctl_len = strlen (fp->ctf_lookups[4].ctl_prefix);
  fp->ctf_lookups[4].ctl_hash = fp->ctf_names;
  fp->ctf_lookups[5].ctl_prefix = NULL;
  fp->ctf_lookups[5].ctl_len = 0;
  fp->ctf_lookups[5].ctl_hash = NULL;
}

/* Open a CTF file, mocking up a suitable ctf_sect.  */

ctf_dict_t *ctf_simple_open (const char *ctfsect, size_t ctfsect_size,
			     const char *symsect, size_t symsect_size,
			     size_t symsect_entsize,
			     const char *strsect, size_t strsect_size,
			     int *errp)
{
  ctf_sect_t skeleton;

  ctf_sect_t ctf_sect, sym_sect, str_sect;
  ctf_sect_t *ctfsectp = NULL;
  ctf_sect_t *symsectp = NULL;
  ctf_sect_t *strsectp = NULL;

  skeleton.cts_name = _CTF_SECTION;
  skeleton.cts_entsize = 1;

  if (ctfsect)
    {
      memcpy (&ctf_sect, &skeleton, sizeof (struct ctf_sect));
      ctf_sect.cts_data = ctfsect;
      ctf_sect.cts_size = ctfsect_size;
      ctfsectp = &ctf_sect;
    }

  if (symsect)
    {
      memcpy (&sym_sect, &skeleton, sizeof (struct ctf_sect));
      sym_sect.cts_data = symsect;
      sym_sect.cts_size = symsect_size;
      sym_sect.cts_entsize = symsect_entsize;
      symsectp = &sym_sect;
    }

  if (strsect)
    {
      memcpy (&str_sect, &skeleton, sizeof (struct ctf_sect));
      str_sect.cts_data = strsect;
      str_sect.cts_size = strsect_size;
      strsectp = &str_sect;
    }

  return ctf_bufopen (ctfsectp, symsectp, strsectp, errp);
}

/* Decode the specified CTF or BTF buffer and optional symbol table, and create
   a new CTF dict representing the type information.  This code can be used
   directly by users, or it can be used as the engine for ctf_fdopen() or
   ctf_open(), below.  */

ctf_dict_t *
ctf_bufopen (const ctf_sect_t *ctfsect, const ctf_sect_t *symsect,
	     const ctf_sect_t *strsect, int *errp)
{
  const ctf_preamble_v3_t *pp;
  const ctf_btf_preamble_t *bp;
  size_t hdrsz = 0;
  ctf_header_t *hp;
  ctf_header_v3_t *header_v3 = NULL;
  ctf_dict_t *fp;
  void *fliphp;
  size_t ctf_adjustment = 0;

  /* These match the CTF_VERSION definitions up to IS_BTF.  */
  enum
    {
      IS_UNKNOWN = 0,
      IS_CTFv1,
      IS_CTFv1_UPGRADED_3,
      IS_CTFv2,
      IS_CTFv3,
      IS_BTF,
      IS_CTF
    } format = IS_UNKNOWN;
  int version = 0;

  const char *desc;
  int foreign_endian = 0;
  int err;

  libctf_init_debug();

  ctf_dprintf ("ctf_bufopen %zi+%zi+%zi bytes: validating\n",
	       ctfsect ? ctfsect->cts_size : 0,
	       symsect ? symsect->cts_size : 0,
	       strsect ? strsect->cts_size : 0);

  if ((ctfsect == NULL) || ((symsect != NULL) && (strsect == NULL)))
    return (ctf_set_open_errno (errp, EINVAL));

  if (symsect != NULL && symsect->cts_entsize != sizeof (Elf32_Sym) &&
      symsect->cts_entsize != sizeof (Elf64_Sym))
    return (ctf_set_open_errno (errp, ECTF_SYMTAB));

  if (symsect != NULL && symsect->cts_data == NULL)
    return (ctf_set_open_errno (errp, ECTF_SYMBAD));

  if (strsect != NULL && strsect->cts_data == NULL)
    return (ctf_set_open_errno (errp, ECTF_STRBAD));

  if (ctfsect->cts_data == NULL
      || ctfsect->cts_size < sizeof (ctf_btf_preamble_t))
    return (ctf_set_open_errno (errp, ECTF_NOCTFBUF));

  /* Validate each part of the CTF header.

     This is more complex than it used to be, because CTFv3 and below, BTF,
     and CTFv4 have distinct preambles with distinct layouts.

     First, we validate the preamble.  At that point, we know the endianness and
     specific header version, and can possibly swap and validate the
     version-specific parts including section offsets and alignments.  None of
     these portions are compressed, thankfully: compression starts after the
     headers.  */

  /* First, deal with the unprefixed cases: BTF, and CTFv1--v3: swap them into
     native endianness if need be.  Verify the prefixed endiannesses first.  */

  bp = (const ctf_btf_preamble_t *) ctfsect->cts_data;
  pp = (const ctf_preamble_v3_t *) ctfsect->cts_data; /* CTFv3 or below.  */

  if (_libctf_unlikely_ (bp->btf_magic != CTF_BTF_MAGIC))
    {
      if (bp->btf_magic == bswap_16 (CTF_BTF_MAGIC))
	{
	  format = IS_BTF;
	  foreign_endian = 1;
	  hdrsz = sizeof (struct ctf_btf_header);
	  version = bp->btf_version;
	}
    }
    else
      {
	format = IS_BTF;
	hdrsz = sizeof (struct ctf_btf_header);
	version = bp->btf_version;
      }

  if (format == IS_UNKNOWN)
    {
      if (pp->ctp_magic == CTF_MAGIC)
	format = pp->ctp_version;
      else if (format == IS_UNKNOWN && pp->ctp_magic == bswap_16 (CTF_MAGIC))
	{
	  format = pp->ctp_version;
	  foreign_endian = 1;
	}
      else
	return (ctf_set_open_errno (errp, ECTF_NOCTFBUF));
    }

  if (format != IS_UNKNOWN && format < IS_BTF)
    {
      switch (format)
	{
	case IS_CTFv1:
	case IS_CTFv2:
	  hdrsz = sizeof (ctf_header_v2_t);
	  break;
	case IS_CTFv1_UPGRADED_3:
	case IS_CTFv3:
	  hdrsz = sizeof (ctf_header_v3_t);
	  break;
	default:
	  /* Cannot happen: placate the compiler.  */
	  hdrsz = sizeof (ctf_header_t);
	}
      version = pp->ctp_version;
    }

  /* Check for modern CTF.  If found, set the adjustment value that lets us
     compensate for BTF offsets being relative to the end of the BTF header,
     while everything else is relative to the end of the CTF header.  */

  if (format == IS_BTF && ctfsect->cts_size >= sizeof (ctf_header_t))
    {
      hp = (ctf_header_t *) ctfsect->cts_data;

      if (CTH_MAGIC (hp) == CTFv4_MAGIC)
	{
	  format = IS_CTF;
	  version = CTH_VERSION (hp);
	  hdrsz = sizeof (ctf_header_t);
	  ctf_adjustment = sizeof (ctf_header_t) - sizeof (ctf_btf_header_t);
	}
      else if (bswap_64 (hp->cth_preamble.ctp_magic_version) >> 16 == CTFv4_MAGIC)
	{
	  format = IS_CTF;
	  foreign_endian = 1;
	  version = bswap_64 (CTH_VERSION (hp));
	  hdrsz = sizeof (ctf_header_t);
	  ctf_adjustment = sizeof (ctf_header_t) - sizeof (ctf_btf_header_t);
	}
      /* Neither hit: confirmed BTF, not CTFv4.  */
    }

  if (ctfsect->cts_size < hdrsz)
    {
      ctf_err_warn (NULL, 0, ECTF_NOCTFBUF,
		    _("ctf_bufopen: size %zi too small for expected header size %zi"),
		    ctfsect->cts_size, hdrsz);
      return (ctf_set_open_errno (errp, ECTF_NOCTFBUF));
    }

  switch (format)
    {
    case IS_CTFv1: desc = "CTFv1"; break;
    case IS_CTFv1_UPGRADED_3: desc = "CTFv1 upgraded to v3"; break;
    case IS_CTFv2: desc = "CTFv2"; break;
    case IS_CTFv3: desc = "CTFv3"; break;
    case IS_BTF: desc = "BTF"; break;
    case IS_CTF: desc = "CTFv4"; break;
    default: desc = "unknown";
    }

  ctf_dprintf ("ctf_bufopen: %s, swap=%i\n", desc, foreign_endian);

  /* Endian-flip and upgrade the header, if necessary.  Preserve it after
     flipping (for possible later dumping, etc).  */

  if ((fliphp = malloc (hdrsz)) == NULL)
    return (ctf_set_open_errno (errp, ENOMEM));

  memcpy (fliphp, ctfsect->cts_data, hdrsz);

  if (foreign_endian)
    {
      if (ctf_flip_header (fliphp, format >= IS_BTF, version) < 0)
	{
	  free (fliphp);
	  return (ctf_set_open_errno (errp, ECTF_INTERNAL));
	}
    }

  if ((hp = malloc (sizeof (ctf_header_t))) == NULL)
    {
      free (fliphp);
      return (ctf_set_open_errno (errp, ENOMEM));
    }

  memset (hp, 0, sizeof (ctf_header_t));
  memcpy (hp, fliphp, hdrsz);
  free (fliphp);

  if (format < CTF_VERSION_3)
    {
      ctf_err_warn (NULL, 0, ECTF_INTERNAL,
		    "Implementation of backward-compatible CTF reading still underway\n");
      return (ctf_set_open_errno (errp, ECTF_INTERNAL));
#if 0
      upgrade_header_v2 (hp); /* Upgrades to v3 */
#endif
    }

  if (format < CTF_VERSION_4)
    {
      header_v3 = (ctf_header_v3_t *) hp;

      if ((hp = malloc (sizeof (ctf_header_t))) == NULL)
	{
	  free (header_v3);
	  return (ctf_set_open_errno (errp, ENOMEM));
	}
      memcpy (hp, header_v3, sizeof (ctf_header_t));

      ctf_err_warn (NULL, 0, ECTF_INTERNAL,
		    "Implementation of backward-compatible CTF reading still underway\n");
      return (ctf_set_open_errno (errp, ECTF_INTERNAL));
#if 0
      upgrade_header_v3 (hp);
#endif
    }

  /* Validation.  */

  if (_libctf_unlikely_ (format == IS_UNKNOWN))
    {
      ctf_set_open_errno (errp, ECTF_CTFVERS);
      goto validation_fail;
    }

  if (_libctf_unlikely_ (format != IS_BTF
			 && ((version < CTF_VERSION_1)
			     || (version > CTF_VERSION_4))))
    {
      ctf_set_open_errno (errp, ECTF_CTFVERS);
      goto validation_fail;
    }

  if (_libctf_unlikely_ (format == IS_BTF
			 && (version != 1 || hp->btf.bth_hdr_len
			     != sizeof (struct ctf_btf_header))))
    {
      ctf_err_warn (NULL, 0, ECTF_CTFVERS,
		    _("BTF version %i or header length %i unknown: expecting 1, %zi\n"),
		   version, hp->btf.bth_hdr_len,
		   sizeof (struct ctf_btf_header));
      ctf_set_open_errno (errp, ECTF_CTFVERS);
      goto validation_fail;
    }

  if (_libctf_unlikely_ (format != IS_BTF && (symsect != NULL)
			 && (version < CTF_VERSION_2)))
    {
      /* The symtab can contain function entries which contain embedded CTF
	 info.  We do not support dynamically upgrading such entries (none
	 should exist in any case, since dwarf2ctf does not create them).  */

      ctf_err_warn (NULL, 0, ECTF_NOTSUP, _("ctf_bufopen: CTF version %d "
					    "symsect not supported"),
		    version);
      ctf_set_open_errno (errp, ECTF_NOTSUP);
      goto validation_fail;
    }

  if (_libctf_unlikely_ (format < IS_BTF && pp->ctp_flags > CTF_F_MAX_3))
    {
      ctf_err_warn (NULL, 0, ECTF_FLAGS, _("ctf_bufopen: invalid header "
					   "flags: %x"),
		    (unsigned int) pp->ctp_flags);
      ctf_set_open_errno (errp, ECTF_FLAGS);
      goto validation_fail;
    }

  if (_libctf_unlikely_ (format >= IS_BTF && bp->btf_flags != 0))
    {
      ctf_err_warn (NULL, 0, ECTF_FLAGS, _("ctf_bufopen: nonzero BTF header "
					   "flags: %x"),
		    (unsigned int) bp->btf_flags);
      ctf_set_open_errno (errp, ECTF_FLAGS);
      goto validation_fail;
    }

  if (_libctf_unlikely_ (format == IS_CTF && (hp->cth_flags & ~(CTF_F_MAX)) != 0))
    {
      ctf_err_warn (NULL, 0, ECTF_FLAGS, _("ctf_bufopen: invalid header "
					   "flags: %llx"),
		    (unsigned long long) hp->cth_flags);
      ctf_set_open_errno (errp, ECTF_FLAGS);
      goto validation_fail;
    }

  /* Check for overlaps. BTF and v4 have a lot more freedom to reorder sections
     than earlier versions do, but we assume for simplicity that they are
     always emitted in the same order.  Because all the offsets are relative to
     the end of the BTF header, we must also check that they don't overlap the
     CTF header that can follow it.  We do allow such overlaps if the section
     length is zero, since that lets us treat a BTF header stuffed into a
     ctf_header_t as valid (with all remaining offsets zero).  */

  if (_libctf_unlikely_
      (hp->cth_objt_off + hp->cth_objt_len > hp->cth_func_off
       || hp->cth_func_off + hp->cth_func_len > hp->cth_objtidx_off
       || hp->cth_objtidx_off + hp->cth_objtidx_len > hp->cth_funcidx_off
       || hp->cth_funcidx_off + hp->cth_funcidx_len > hp->btf.bth_type_off
       || hp->btf.bth_type_off + hp->btf.bth_type_len > hp->btf.bth_str_off
       || (hp->cth_objt_off < ctf_adjustment && hp->cth_objt_len != 0)
       || (hp->cth_func_off < ctf_adjustment && hp->cth_func_len != 0)
       || (hp->cth_objtidx_off < ctf_adjustment && hp->cth_objtidx_len != 0)
       || (hp->cth_funcidx_off < ctf_adjustment && hp->cth_funcidx_len != 0)
       || (hp->btf.bth_type_off < ctf_adjustment && hp->btf.bth_type_len != 0)
       || (hp->btf.bth_str_off < ctf_adjustment && hp->btf.bth_str_len != 0)))
    {
      ctf_err_warn (NULL, 0, ECTF_CORRUPT, _("overlapping or misordered BTF/CTF sections"));
      ctf_set_open_errno (errp, ECTF_CORRUPT);
      goto validation_fail;
    }

  if (_libctf_unlikely_
      ((hp->cth_objt_off & 2)
       || (hp->cth_func_off & 2) || (hp->cth_objtidx_off & 2)
       || (hp->cth_funcidx_off & 2) || (hp->btf.bth_type_off & 3)))
    {
      ctf_err_warn (NULL, 0, ECTF_CORRUPT,
		    _("CTF sections not properly aligned"));
      ctf_set_open_errno (errp, ECTF_CORRUPT);
      goto validation_fail;
    }

  /* This invariant may be lifted in v5, but for now it is true.  */

  if (_libctf_unlikely_ ((hp->cth_objtidx_len != 0) &&
			 (hp->cth_objtidx_len != hp->cth_objt_len)))
    {
      ctf_err_warn (NULL, 0, ECTF_CORRUPT,
		    _("Object index section is neither empty nor the "
		      "same length as the object section: %u versus %u "
		      "bytes"), hp->cth_objt_len, hp->cth_objtidx_len);
      ctf_set_open_errno (errp, ECTF_CORRUPT);
      goto validation_fail;
    }

  /* v3 only needs this invariant if CTF_F_NEWFUNCINFO is set: if it's not, the
     section is ignored anyway.  Always true for v4.  */
  if (_libctf_unlikely_ ((hp->cth_funcidx_len != 0) &&
			 (hp->cth_funcidx_len != hp->cth_func_len) &&
			 ((header_v3 && hp->cth_flags & CTF_F_NEWFUNCINFO)
			  || !header_v3)))
    {
      ctf_err_warn (NULL, 0, ECTF_CORRUPT,
		    _("Function index section is neither empty nor the "
		      "same length as the function section: %u versus %u "
		      "bytes"), hp->cth_func_len, hp->cth_funcidx_len);
      ctf_set_open_errno (errp, ECTF_CORRUPT);
      goto validation_fail;
    }

  if (_libctf_unlikely_
      (hp->cth_parent_strlen != 0 &&
       ((hp->cth_parent_name != 0 && hp->cth_parent_name < hp->cth_parent_strlen)
	|| (hp->cth_cu_name != 0 && hp->cth_cu_name < hp->cth_parent_strlen))))
    {
      ctf_err_warn (NULL, 0, ECTF_CORRUPT,
		    _("Parent dict or CU name string offsets "
		      "(at %x and %x, respectively) are themselves "
		      "within the parent (upper bound: %x), thus "
		      "unreachable.\n"), hp->cth_parent_name, hp->cth_cu_name,
		      hp->cth_parent_strlen);
      ctf_set_open_errno (errp, ECTF_CORRUPT);
      goto validation_fail;
    }

  /* Start to fill out the in-memory dict.  */

  if ((fp = malloc (sizeof (ctf_dict_t))) == NULL)
    return (ctf_set_open_errno (errp, ENOMEM));

  memset (fp, 0, sizeof (ctf_dict_t));

  fp->ctf_header = hp;
  fp->ctf_v3_header = header_v3;

  fp->ctf_openflags = hp->cth_flags;
  fp->ctf_opened_btf = (format == IS_BTF);

  /* The ctf_size includes the entire CTFv4 header excepting only the portion
     shared with BTF (see ctf_adjustment).  */
  fp->ctf_size = hp->btf.bth_str_off + hp->btf.bth_str_len;

  if (_libctf_unlikely_ (hp->cth_objt_off > fp->ctf_size
			 || hp->cth_func_off > fp->ctf_size
			 || hp->cth_objtidx_off > fp->ctf_size
			 || hp->cth_funcidx_off > fp->ctf_size
			 || hp->btf.bth_type_off > fp->ctf_size))
    {
      ctf_err_warn (NULL, 0, ECTF_CORRUPT, _("header offset or length exceeds CTF size"));
      err = ECTF_CORRUPT;
      goto bad;
    }

  ctf_dprintf ("ctf_bufopen: uncompressed size=%lu\n",
	       (unsigned long) fp->ctf_size);

  /* Once everything is determined to be valid, attempt to decompress the CTF
     data buffer if it is compressed, or copy it into new storage if it is not
     compressed but needs endian-flipping.  Otherwise we just put the data
     section's buffer pointer into ctf_buf, below.  */

  /* Note: if this is a v1 -- v3 buffer, it will be reallocated and expanded by
     upgrade_types(), invoked by init_static_types().  */

  if (hp->cth_flags & CTF_F_COMPRESS)
    {
      size_t srclen;
      uLongf dstlen;
      const void *src;
      int rc = Z_OK;

      if ((fp->ctf_base = malloc (fp->ctf_size)) == NULL)
	{
	  err = ECTF_ZALLOC;
	  goto bad;
	}
      fp->ctf_dynbase = fp->ctf_base;
      hp->cth_flags &= ~CTF_F_COMPRESS;

      src = (unsigned char *) ctfsect->cts_data + hdrsz;
      srclen = ctfsect->cts_size - hdrsz;
      dstlen = fp->ctf_size - ctf_adjustment;
      fp->ctf_buf = fp->ctf_base;

      /* Stick the CTF-only header portion into the buffer.  Not much use, but
	 it makes sure all the header offsets are right. */
      memcpy (fp->ctf_base, (unsigned char *) ctfsect->cts_data
	      + sizeof (ctf_btf_header_t), ctf_adjustment);

      if ((rc = uncompress (fp->ctf_base + ctf_adjustment, &dstlen,
			    src, srclen)) != Z_OK)
	{
	  ctf_err_warn (NULL, 0, ECTF_DECOMPRESS, _("zlib inflate err: %s"),
			zError (rc));
	  err = ECTF_DECOMPRESS;
	  goto bad;
	}

      if ((size_t) dstlen != (fp->ctf_size - ctf_adjustment))
	{
	  ctf_err_warn (NULL, 0, ECTF_CORRUPT,
			_("zlib inflate short: got %lu of %lu bytes"),
			(unsigned long) dstlen,
			(unsigned long) (fp->ctf_size - ctf_adjustment));
	  err = ECTF_CORRUPT;
	  goto bad;
	}
    }
  else
    {
      if (_libctf_unlikely_ (ctfsect->cts_size < fp->ctf_size + hdrsz - ctf_adjustment))
	{
	  ctf_err_warn (NULL, 0, ECTF_CORRUPT,
			_("%lu byte long CTF dictionary overruns %lu byte long CTF section"),
			(unsigned long) ctfsect->cts_size,
			(unsigned long) (fp->ctf_size + hdrsz - ctf_adjustment));
	  err = ECTF_CORRUPT;
	  goto bad;
	}

      if (foreign_endian)
	{
	  if ((fp->ctf_base = malloc (fp->ctf_size)) == NULL)
	    {
	      err = ECTF_ZALLOC;
	      goto bad;
	    }
	  fp->ctf_dynbase = fp->ctf_base;
	  memcpy (fp->ctf_base, ((unsigned char *) ctfsect->cts_data)
		  + sizeof (ctf_btf_header_t), fp->ctf_size);
	}
      else
	{
	  fp->ctf_base = (unsigned char *) ctfsect->cts_data;
	  fp->ctf_dynbase = NULL;
	}

      /* Ensure that the buffer we are using has offset 0 at the end of the BTF
	 header, if there is one, to avoid breaking all offset lookups.  */

      fp->ctf_buf = fp->ctf_base + hdrsz - ctf_adjustment;
    }

  /* Once we have uncompressed and validated the (BTF or) CTF data buffer, we
     can proceed with initializing the ctf_dict_t we allocated above.

     Nothing that depends on buf or base should be set directly in this function
     before the init_static_types() call, because it may be reallocated during
     transparent upgrade if this recension of libctf is so configured: see
     ctf_set_base().  */

  /* We declare BTF to be a kind of CTF so that we can use a single incrementing
     integer to describe all CTF field access operations.  This obliges CTF to
     remain BTF-compatible, come what may.  We also note that, as far as we
     know so far, this CTF dict can be written as BTF without losing
     information.  */

  if (format == IS_BTF)
    {
      hp->cth_preamble.ctp_magic_version = (CTFv4_MAGIC << 16) | CTF_VERSION;
      version = CTF_VERSION;
    }

  fp->ctf_version = version;
  fp->ctf_dictops = &ctf_dictops[fp->ctf_version];

  /* Temporary assignment, just enough to be able to initialize the atoms table.
     This does not guarantee that we can look up strings: in v4, child dicts
     cannot reliably look up strings until after ctf_import.  */

  fp->ctf_str[CTF_STRTAB_0].cts_strs = (const char *) fp->ctf_buf
    + hp->btf.bth_str_off;
  fp->ctf_str[CTF_STRTAB_0].cts_len = hp->btf.bth_str_len;
  if (ctf_str_create_atoms (fp) < 0)
    {
      err = ENOMEM;
      goto bad;
    }

  memcpy (&fp->ctf_data, ctfsect, sizeof (ctf_sect_t));

  if (symsect != NULL)
    {
      memcpy (&fp->ctf_ext_symtab, symsect, sizeof (ctf_sect_t));
      memcpy (&fp->ctf_ext_strtab, strsect, sizeof (ctf_sect_t));
    }

  if (fp->ctf_data.cts_name != NULL)
    if ((fp->ctf_data.cts_name = strdup (fp->ctf_data.cts_name)) == NULL)
      {
	err = ENOMEM;
	goto bad;
      }
  if (fp->ctf_ext_symtab.cts_name != NULL)
    if ((fp->ctf_ext_symtab.cts_name = strdup (fp->ctf_ext_symtab.cts_name)) == NULL)
      {
	err = ENOMEM;
	goto bad;
      }
  if (fp->ctf_ext_strtab.cts_name != NULL)
    if ((fp->ctf_ext_strtab.cts_name = strdup (fp->ctf_ext_strtab.cts_name)) == NULL)
      {
	err = ENOMEM;
	goto bad;
      }

  if (fp->ctf_data.cts_name == NULL)
    fp->ctf_data.cts_name = _CTF_NULLSTR;
  if (fp->ctf_ext_symtab.cts_name == NULL)
    fp->ctf_ext_symtab.cts_name = _CTF_NULLSTR;
  if (fp->ctf_ext_strtab.cts_name == NULL)
    fp->ctf_ext_strtab.cts_name = _CTF_NULLSTR;

  if (strsect != NULL)
    {
      fp->ctf_str[CTF_STRTAB_1].cts_strs = strsect->cts_data;
      fp->ctf_str[CTF_STRTAB_1].cts_len = strsect->cts_size;
    }

  /* Dynamic state, for dynamic addition to this dict after loading.  */

  fp->ctf_dthash = ctf_dynhash_create (ctf_hash_integer, ctf_hash_eq_integer,
				       NULL, NULL);
  fp->ctf_dvhash = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
				       NULL, NULL);
  fp->ctf_snapshots = 1;

  fp->ctf_objthash = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
					   free, NULL);
  fp->ctf_funchash = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
					 free, NULL);

  if (!fp->ctf_dthash || !fp->ctf_dvhash || !fp->ctf_snapshots ||
      !fp->ctf_objthash || !fp->ctf_funchash)
    {
      err = ENOMEM;
      goto bad;
    }

  if (foreign_endian &&
      (err = ctf_flip (fp, hp, fp->ctf_buf, format == IS_BTF, 0)) != 0)
    {
      /* We can be certain that ctf_flip() will have endian-flipped everything
	 other than the types table when we return.  In particular the header
	 is fine, so set it, to allow freeing to use the usual code path.  */

      ctf_set_base (fp, hp, fp->ctf_base);
      goto bad;
    }

  ctf_set_base (fp, hp, fp->ctf_base);

  if ((err = init_static_types (fp, hp, format == IS_BTF)) != 0)
    goto bad;

  /* Allocate and initialize the symtab translation table, pointed to by
     ctf_sxlate, and the corresponding index sections.  This table may be too
     large for the actual size of the object and function info sections: if so,
     ctf_nsyms will be adjusted and the excess will never be used.  It's
     possible to do indexed symbol lookups even without a symbol table, so check
     even in that case.  Initially, we assume the symtab is native-endian: if it
     isn't, the caller will inform us later by calling ctf_symsect_endianness.  */
#ifdef WORDS_BIGENDIAN
  fp->ctf_symsect_little_endian = 0;
#else
  fp->ctf_symsect_little_endian = 1;
#endif

  if (symsect != NULL)
    {
      fp->ctf_nsyms = symsect->cts_size / symsect->cts_entsize;
      fp->ctf_sxlate = malloc (fp->ctf_nsyms * sizeof (uint32_t));

      if (fp->ctf_sxlate == NULL)
	{
	  err = ENOMEM;
	  goto bad;
	}
    }

  if ((err = init_symtab (fp, hp, symsect)) != 0)
    goto bad;

  ctf_set_ctl_hashes (fp);

  if (symsect != NULL)
    {
      if (symsect->cts_entsize == sizeof (Elf64_Sym))
	(void) ctf_setmodel (fp, CTF_MODEL_LP64);
      else
	(void) ctf_setmodel (fp, CTF_MODEL_ILP32);
    }
  else
    (void) ctf_setmodel (fp, CTF_MODEL_NATIVE);

  fp->ctf_refcnt = 1;
  return fp;

validation_fail:
  free (header_v3);
  free (hp);
  return NULL;

bad:
  ctf_set_open_errno (errp, err);
  ctf_err_warn_to_open (fp);
  /* Without this, the refcnt is zero on entry and ctf_dict_close() won't
     actually do anything on the grounds that this is a recursive call via
     another dict being closed.  */
  fp->ctf_refcnt = 1;
  ctf_dict_close (fp);
  return NULL;
}

/* Bump the refcount on the specified CTF dict, to allow export of ctf_dict_t's
   from iterators that open and close the ctf_dict_t around the loop.  (This
   does not extend their lifetime beyond that of the ctf_archive_t in which they
   are contained.)  */

void
ctf_ref (ctf_dict_t *fp)
{
  fp->ctf_refcnt++;
}

/* Close the specified CTF dict and free associated data structures.  Note that
   ctf_dict_close() is a reference counted operation: if the specified file is
   the parent of other active dict, its reference count will be greater than one
   and it will be freed later when no active children exist.  */

void
ctf_dict_close (ctf_dict_t *fp)
{
  ctf_dtdef_t *dtd, *ntd;
  ctf_dvdef_t *dvd, *nvd;
  ctf_in_flight_dynsym_t *did, *nid;
  ctf_err_warning_t *err, *nerr;

  if (fp == NULL)
    return;		   /* Allow ctf_dict_close(NULL) to simplify caller code.  */

  ctf_dprintf ("ctf_dict_close(%p) refcnt=%u\n", (void *) fp, fp->ctf_refcnt);

  if (fp->ctf_refcnt > 1)
    {
      fp->ctf_refcnt--;
      return;
    }

  /* It is possible to recurse back in here, notably if dicts in the
     ctf_link_inputs or ctf_link_outputs cite this dict as a parent without
     using ctf_import_unref.  Do nothing in that case.  */
  if (fp->ctf_refcnt == 0)
    return;

  fp->ctf_refcnt--;
  if (fp->ctf_parent && !fp->ctf_parent_unreffed)
    ctf_dict_close (fp->ctf_parent);
  free (fp->ctf_dyn_cu_name);
  free (fp->ctf_dyn_parent_name);

  for (dtd = ctf_list_next (&fp->ctf_dtdefs); dtd != NULL; dtd = ntd)
    {
      ntd = ctf_list_next (dtd);
      ctf_dtd_delete (fp, dtd);
    }
  ctf_dynhash_destroy (fp->ctf_dthash);

  ctf_dynset_destroy (fp->ctf_conflicting_enums);
  ctf_dynhash_destroy (fp->ctf_structs);
  ctf_dynhash_destroy (fp->ctf_unions);
  ctf_dynhash_destroy (fp->ctf_enums);
  ctf_dynhash_destroy (fp->ctf_datasecs);
  ctf_dynhash_destroy (fp->ctf_tags);
  ctf_dynhash_destroy (fp->ctf_names);

  for (dvd = ctf_list_next (&fp->ctf_dvdefs); dvd != NULL; dvd = nvd)
    {
      nvd = ctf_list_next (dvd);
      ctf_dvd_delete (fp, dvd);
    }
  ctf_dynhash_destroy (fp->ctf_dvhash);
  ctf_dynhash_destroy (fp->ctf_var_datasecs);

  ctf_dynhash_destroy (fp->ctf_symhash_func);
  ctf_dynhash_destroy (fp->ctf_symhash_objt);
  free (fp->ctf_funcidx_sxlate);
  free (fp->ctf_objtidx_sxlate);
  ctf_dynhash_destroy (fp->ctf_objthash);
  ctf_dynhash_destroy (fp->ctf_funchash);
  free (fp->ctf_dynsymidx);
  ctf_dynhash_destroy (fp->ctf_dynsyms);
  for (did = ctf_list_next (&fp->ctf_in_flight_dynsyms); did != NULL; did = nid)
    {
      nid = ctf_list_next (did);
      ctf_list_delete (&fp->ctf_in_flight_dynsyms, did);
      free (did);
    }

  /* The lifetime rules here are delicate.  We must destroy the outputs before
     the atoms (since in a link the outputs contain references to the parent's
     atoms), but we must destroy the inputs after that (since many type strings
     ultimately come from the inputs).  In addition, if there are
     ctf_link_outputs, the parent dict's atoms table may have refs that refer to
     the outputs: so purge the refs first.  */

  if (fp->ctf_link_outputs && ctf_dynhash_elements (fp->ctf_link_outputs) > 0)
    ctf_str_purge_refs (fp);

  ctf_dynhash_destroy (fp->ctf_link_outputs);
  ctf_dynhash_destroy (fp->ctf_link_out_cu_mapping);

  ctf_str_free_atoms (fp);
  free (fp->ctf_tmp_typeslice);

  if (fp->ctf_data.cts_name != _CTF_NULLSTR)
    free ((char *) fp->ctf_data.cts_name);

  if (fp->ctf_ext_symtab.cts_name != _CTF_NULLSTR)
    free ((char *) fp->ctf_ext_symtab.cts_name);

  if (fp->ctf_ext_strtab.cts_name != _CTF_NULLSTR)
    free ((char *) fp->ctf_ext_strtab.cts_name);
  else if (fp->ctf_data_mmapped)
    ctf_munmap (fp->ctf_data_mmapped, fp->ctf_data_mmapped_len);

  free (fp->ctf_dynbase);

  ctf_dynhash_destroy (fp->ctf_link_inputs);
  ctf_dynhash_destroy (fp->ctf_link_type_mapping);
  ctf_dynhash_destroy (fp->ctf_link_in_cu_mapping);
  ctf_dynhash_destroy (fp->ctf_add_processing);
  ctf_dedup_fini (fp, NULL, 0);
  ctf_dynset_destroy (fp->ctf_dedup_atoms_alloc);

  for (err = ctf_list_next (&fp->ctf_errs_warnings); err != NULL; err = nerr)
    {
      nerr = ctf_list_next (err);
      ctf_list_delete (&fp->ctf_errs_warnings, err);
      free (err->cew_text);
      free (err);
    }

  free (fp->ctf_void_type);
  free (fp->ctf_sxlate);
  free (fp->ctf_txlate);
  free (fp->ctf_ptrtab);
  free (fp->ctf_pptrtab);
  free (fp->ctf_serializing_buf);

  free (fp->ctf_header);
  free (fp);
}

/* Backward compatibility.  */
void
ctf_file_close (ctf_file_t *fp)
{
  ctf_dict_close (fp);
}

/* The converse of ctf_open().  ctf_open() disguises whatever it opens as an
   archive, so closing one is just like closing an archive.  */
void
ctf_close (ctf_archive_t *arc)
{
  ctf_arc_close (arc);
}

/* Get the CTF archive from which this ctf_dict_t is derived.  */
ctf_archive_t *
ctf_get_arc (const ctf_dict_t *fp)
{
  return fp->ctf_archive;
}

/* Return the ctfsect out of the core ctf_impl.  Useful for freeing the
   ctfsect's data * after ctf_dict_close(), which is why we return the actual
   structure, not a pointer to it, since that is likely to become a pointer to
   freed data before the return value is used under the expected use case of
   ctf_getsect()/ ctf_dict_close()/free().  */
ctf_sect_t
ctf_getdatasect (const ctf_dict_t *fp)
{
  return fp->ctf_data;
}

ctf_sect_t
ctf_getsymsect (const ctf_dict_t *fp)
{
  return fp->ctf_ext_symtab;
}

ctf_sect_t
ctf_getstrsect (const ctf_dict_t *fp)
{
  return fp->ctf_ext_strtab;
}

/* Set the endianness of the symbol table attached to FP.  */
void
ctf_symsect_endianness (ctf_dict_t *fp, int little_endian)
{
  int old_endianness = fp->ctf_symsect_little_endian;

  fp->ctf_symsect_little_endian = !!little_endian;

  /* If we already have a symtab translation table, we need to repopulate it if
     our idea of the endianness has changed.  */

  if (old_endianness != fp->ctf_symsect_little_endian
      && fp->ctf_sxlate != NULL && fp->ctf_ext_symtab.cts_data != NULL)
    assert (init_symtab (fp, fp->ctf_header, &fp->ctf_ext_symtab) == 0);
}

/* Return the CTF handle for the parent CTF dict, if one exists.  Otherwise
   return NULL to indicate this dict has no imported parent.  */
ctf_dict_t *
ctf_parent_dict (ctf_dict_t *fp)
{
  return fp->ctf_parent;
}

/* Backward compatibility.  */
ctf_dict_t *
ctf_parent_file (ctf_dict_t *fp)
{
  return ctf_parent_dict (fp);
}

/* Return the name of the parent CTF dict, if one exists, or NULL otherwise.  */
const char *
ctf_parent_name (ctf_dict_t *fp)
{
  return fp->ctf_parent_name;
}

/* Set the parent name.  It is an error to call this routine without calling
   ctf_import() at some point.  */
int
ctf_parent_name_set (ctf_dict_t *fp, const char *name)
{
  if (fp->ctf_dyn_parent_name != NULL)
    free (fp->ctf_dyn_parent_name);

  if ((fp->ctf_dyn_parent_name = strdup (name)) == NULL)
    return (ctf_set_errno (fp, ENOMEM));
  fp->ctf_parent_name = fp->ctf_dyn_parent_name;
  return 0;
}

/* Return the name of the compilation unit this CTF file applies to.  Usually
   non-NULL only for non-parent dicts.  */
const char *
ctf_cuname (ctf_dict_t *fp)
{
  return fp->ctf_cu_name;
}

/* Set the compilation unit name.  */
int
ctf_cuname_set (ctf_dict_t *fp, const char *name)
{
  if (fp->ctf_dyn_cu_name != NULL)
    free (fp->ctf_dyn_cu_name);

  if ((fp->ctf_dyn_cu_name = strdup (name)) == NULL)
    return (ctf_set_errno (fp, ENOMEM));
  fp->ctf_cu_name = fp->ctf_dyn_cu_name;
  return 0;
}

static int
ctf_import_internal (ctf_dict_t *fp, ctf_dict_t *pfp, int unreffed)
{
  int err;
  int no_strings = fp->ctf_flags & LCTF_NO_STR;
  int old_flags = fp->ctf_flags;
  ctf_dict_t *old_parent = fp->ctf_parent;
  const char *old_parent_name = fp->ctf_parent_name;
  int old_unreffed = fp->ctf_parent_unreffed;

  if (pfp == NULL || pfp == fp->ctf_parent)
    return 0;

  if (fp == NULL || fp == pfp || pfp->ctf_refcnt == 0)
    return (ctf_set_errno (fp, EINVAL));

  if (pfp->ctf_dmodel != fp->ctf_dmodel)
    return (ctf_set_errno (fp, ECTF_DMODEL));

  if (fp->ctf_parent && fp->ctf_header->cth_parent_strlen != 0)
    return (ctf_set_errno (fp, ECTF_HASPARENT));

  if (fp->ctf_header->cth_parent_strlen != 0 &&
      pfp->ctf_header->cth_strlen != fp->ctf_header->cth_parent_strlen)
    {
      ctf_err_warn (fp, 0, ECTF_WRONGPARENT,
		    _("ctf_import: incorrect parent dict: %u bytes of strings expected, %u found"),
		    fp->ctf_header->cth_parent_strlen, pfp->ctf_header->cth_strlen);
      return (ctf_set_errno (fp, ECTF_WRONGPARENT));
    }

  /* If the child dict expects the parent to have types, make sure it has that
     number of types.  (Provisional types excepted: they go at the top of the
     type ID space, and will not overlap any child types.)  */

  if (pfp->ctf_idmax != fp->ctf_header->cth_parent_typemax)
    {
      if (fp->ctf_header->cth_parent_typemax != 0)
	{
	  ctf_err_warn (fp, 0, ECTF_WRONGPARENT,
			_("ctf_import: incorrect parent dict: %u types expected, %u found"),
			fp->ctf_header->cth_parent_typemax, pfp->ctf_idmax);
	  return (ctf_set_errno (fp, ECTF_WRONGPARENT));
	}
      else if (fp->ctf_header->cth_parent_typemax == 0)
	{
	  /* If we are importing into a parent dict, the child dict had better
	     be empty.  Set its starting type ID, which need not be zero: the
	     parent can already have types.  */

	  if (fp->ctf_typemax != 0)
	    {
	      ctf_err_warn (fp, 0, EINVAL,
			    _("ctf_import: dict already has %u types, cannot turn into a new child"),
			    fp->ctf_typemax);
	      return (ctf_set_errno (fp, EINVAL));
	    }
	  fp->ctf_header->cth_parent_typemax = pfp->ctf_typemax;
	}
    }

  /* We might in time be able to lift this restriction, but it is unlikely to be
     something anyone would want to do, so let's not bother for now.  */

  if (ctf_dynhash_elements (fp->ctf_prov_strtab) != 0)
    {
      ctf_err_warn (fp, 0, EINVAL,
		    _("ctf_import: child dict already has %zi bytes of strings, cannot import"),
		    ctf_dynhash_elements (fp->ctf_prov_strtab));
      return (ctf_set_errno (fp, EINVAL));
    }

  fp->ctf_parent = NULL;
  free (fp->ctf_pptrtab);
  fp->ctf_pptrtab = NULL;
  fp->ctf_pptrtab_len = 0;
  fp->ctf_pptrtab_typemax = 0;

  if (fp->ctf_parent_name == NULL)
    if ((err = ctf_parent_name_set (fp, "PARENT")) < 0)
      return err;				/* errno is set for us.  */

  if (!unreffed)
    pfp->ctf_refcnt++;

  fp->ctf_parent_unreffed = unreffed;
  fp->ctf_parent = pfp;

  /* If this is a dict that hasn't previously allowed string lookups,
     we can allow them now, and finish initialization.  */

  fp->ctf_flags |= LCTF_CHILD;
  fp->ctf_flags &= ~LCTF_NO_STR;

  if (no_strings && (err = init_static_types_names (fp, fp->ctf_header,
						    fp->ctf_opened_btf) < 0))
    {
      /* Undo everything other than cache flushing.  */

      fp->ctf_flags = old_flags;
      fp->ctf_parent_unreffed = old_unreffed;
      fp->ctf_parent = old_parent;
      fp->ctf_parent_name = old_parent_name;

      if (fp->ctf_parent_unreffed)
	old_parent->ctf_refcnt++;

      return (ctf_set_errno (fp, err));		/* errno is set for us.  */
    }

  /* Failure can now no longer happen, so we can close the old parent (which may
     deallocate it and is not easily reversible).  */

  if (old_parent && !old_unreffed)
    ctf_dict_close (old_parent);

  fp->ctf_parent->ctf_max_children++;
  return 0;
}

/* Import the types from the specified parent dict by storing a pointer to it in
   ctf_parent and incrementing its reference count.  You can only call this
   function once on serialized dicts: the parent cannot be replaced once set.
   (You can call it on unserialized dicts as often as you like.)

   The pptrtab is wiped, and will be refreshed by the next ctf_lookup_by_name
   call.

   You can call this with a parent of NULL as many times as you like (but
   it doesn't do much).  */
int
ctf_import (ctf_dict_t *fp, ctf_dict_t *pfp)
{
  return ctf_import_internal (fp, pfp, 0);
}

/* Like ctf_import, but does not increment the refcount on the imported parent
   or close it at any point: as a result it can go away at any time and the
   caller must do all freeing itself.  Used internally to avoid refcount
   loops.  */
int
ctf_import_unref (ctf_dict_t *fp, ctf_dict_t *pfp)
{
  return ctf_import_internal (fp, pfp, 1);
}

/* Set the data model constant for the CTF dict.  */
int
ctf_setmodel (ctf_dict_t *fp, int model)
{
  const ctf_dmodel_t *dp;

  for (dp = _libctf_models; dp->ctd_name != NULL; dp++)
    {
      if (dp->ctd_code == model)
	{
	  fp->ctf_dmodel = dp;
	  return 0;
	}
    }

  return (ctf_set_errno (fp, EINVAL));
}

/* Return the data model constant for the CTF dict.  */
int
ctf_getmodel (ctf_dict_t *fp)
{
  return fp->ctf_dmodel->ctd_code;
}

/* The caller can hang an arbitrary pointer off each ctf_dict_t using this
   function.  */
void
ctf_setspecific (ctf_dict_t *fp, void *data)
{
  fp->ctf_specific = data;
}

/* Retrieve the arbitrary pointer again.  */
void *
ctf_getspecific (ctf_dict_t *fp)
{
  return fp->ctf_specific;
}
