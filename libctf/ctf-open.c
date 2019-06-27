/* Opening CTF files.
   Copyright (C) 2019 Free Software Foundation, Inc.

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
#include <assert.h>
#include "swap.h"
#include <bfd.h>
#include <zlib.h>

#include "elf-bfd.h"

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
get_root_v1 (uint32_t info)
{
  return (CTF_V1_INFO_ISROOT (info));
}

static uint32_t
get_vlen_v1 (uint32_t info)
{
  return (CTF_V1_INFO_VLEN (info));
}

static uint32_t
get_kind_v2 (uint32_t info)
{
  return (CTF_V2_INFO_KIND (info));
}

static uint32_t
get_root_v2 (uint32_t info)
{
  return (CTF_V2_INFO_ISROOT (info));
}

static uint32_t
get_vlen_v2 (uint32_t info)
{
  return (CTF_V2_INFO_VLEN (info));
}

static inline ssize_t
get_ctt_size_common (const ctf_file_t *fp _libctf_unused_,
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
get_ctt_size_v1 (const ctf_file_t *fp, const ctf_type_t *tp,
		 ssize_t *sizep, ssize_t *incrementp)
{
  ctf_type_v1_t *t1p = (ctf_type_v1_t *) tp;

  return (get_ctt_size_common (fp, tp, sizep, incrementp,
			       CTF_TYPE_LSIZE (t1p), t1p->ctt_size,
			       sizeof (ctf_type_v1_t), sizeof (ctf_stype_v1_t),
			       CTF_LSIZE_SENT_V1));
}

/* Return the size that a v1 will be once it is converted to v2.  */

static ssize_t
get_ctt_size_v2_unconverted (const ctf_file_t *fp, const ctf_type_t *tp,
			     ssize_t *sizep, ssize_t *incrementp)
{
  ctf_type_v1_t *t1p = (ctf_type_v1_t *) tp;

  return (get_ctt_size_common (fp, tp, sizep, incrementp,
			       CTF_TYPE_LSIZE (t1p), t1p->ctt_size,
			       sizeof (ctf_type_t), sizeof (ctf_stype_t),
			       CTF_LSIZE_SENT));
}

static ssize_t
get_ctt_size_v2 (const ctf_file_t *fp, const ctf_type_t *tp,
		 ssize_t *sizep, ssize_t *incrementp)
{
  return (get_ctt_size_common (fp, tp, sizep, incrementp,
			       CTF_TYPE_LSIZE (tp), tp->ctt_size,
			       sizeof (ctf_type_t), sizeof (ctf_stype_t),
			       CTF_LSIZE_SENT));
}

static ssize_t
get_vbytes_common (unsigned short kind, ssize_t size _libctf_unused_,
		   size_t vlen)
{
  switch (kind)
    {
    case CTF_K_INTEGER:
    case CTF_K_FLOAT:
      return (sizeof (uint32_t));
    case CTF_K_SLICE:
      return (sizeof (ctf_slice_t));
    case CTF_K_ENUM:
      return (sizeof (ctf_enum_t) * vlen);
    case CTF_K_FORWARD:
    case CTF_K_UNKNOWN:
    case CTF_K_POINTER:
    case CTF_K_TYPEDEF:
    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
      return 0;
    default:
      ctf_dprintf ("detected invalid CTF kind -- %x\n", kind);
      return ECTF_CORRUPT;
    }
}

static ssize_t
get_vbytes_v1 (unsigned short kind, ssize_t size, size_t vlen)
{
  switch (kind)
    {
    case CTF_K_ARRAY:
      return (sizeof (ctf_array_v1_t));
    case CTF_K_FUNCTION:
      return (sizeof (unsigned short) * (vlen + (vlen & 1)));
    case CTF_K_STRUCT:
    case CTF_K_UNION:
      if (size < CTF_LSTRUCT_THRESH_V1)
	return (sizeof (ctf_member_v1_t) * vlen);
      else
	return (sizeof (ctf_lmember_v1_t) * vlen);
    }

  return (get_vbytes_common (kind, size, vlen));
}

static ssize_t
get_vbytes_v2 (unsigned short kind, ssize_t size, size_t vlen)
{
  switch (kind)
    {
    case CTF_K_ARRAY:
      return (sizeof (ctf_array_t));
    case CTF_K_FUNCTION:
      return (sizeof (uint32_t) * (vlen + (vlen & 1)));
    case CTF_K_STRUCT:
    case CTF_K_UNION:
      if (size < CTF_LSTRUCT_THRESH)
	return (sizeof (ctf_member_t) * vlen);
      else
	return (sizeof (ctf_lmember_t) * vlen);
    }

  return (get_vbytes_common (kind, size, vlen));
}

static const ctf_fileops_t ctf_fileops[] = {
  {NULL, NULL, NULL, NULL, NULL},
  /* CTF_VERSION_1 */
  {get_kind_v1, get_root_v1, get_vlen_v1, get_ctt_size_v1, get_vbytes_v1},
  /* CTF_VERSION_1_UPGRADED_3 */
  {get_kind_v2, get_root_v2, get_vlen_v2, get_ctt_size_v2, get_vbytes_v2},
  /* CTF_VERSION_2 */
  {get_kind_v2, get_root_v2, get_vlen_v2, get_ctt_size_v2, get_vbytes_v2},
  /* CTF_VERSION_3, identical to 2: only new type kinds */
  {get_kind_v2, get_root_v2, get_vlen_v2, get_ctt_size_v2, get_vbytes_v2},
};

/* Initialize the symtab translation table by filling each entry with the
  offset of the CTF type or function data corresponding to each STT_FUNC or
  STT_OBJECT entry in the symbol table.  */

static int
init_symtab (ctf_file_t *fp, const ctf_header_t *hp,
	     const ctf_sect_t *sp, const ctf_sect_t *strp)
{
  const unsigned char *symp = sp->cts_data;
  uint32_t *xp = fp->ctf_sxlate;
  uint32_t *xend = xp + fp->ctf_nsyms;

  uint32_t objtoff = hp->cth_objtoff;
  uint32_t funcoff = hp->cth_funcoff;

  uint32_t info, vlen;
  Elf64_Sym sym, *gsp;
  const char *name;

  /* The CTF data object and function type sections are ordered to match
     the relative order of the respective symbol types in the symtab.
     If no type information is available for a symbol table entry, a
     pad is inserted in the CTF section.  As a further optimization,
     anonymous or undefined symbols are omitted from the CTF data.  */

  for (; xp < xend; xp++, symp += sp->cts_entsize)
    {
      if (sp->cts_entsize == sizeof (Elf32_Sym))
	gsp = ctf_sym_to_elf64 ((Elf32_Sym *) (uintptr_t) symp, &sym);
      else
	gsp = (Elf64_Sym *) (uintptr_t) symp;

      if (gsp->st_name < strp->cts_size)
	name = (const char *) strp->cts_data + gsp->st_name;
      else
	name = _CTF_NULLSTR;

      if (gsp->st_name == 0 || gsp->st_shndx == SHN_UNDEF
	  || strcmp (name, "_START_") == 0 || strcmp (name, "_END_") == 0)
	{
	  *xp = -1u;
	  continue;
	}

      switch (ELF64_ST_TYPE (gsp->st_info))
	{
	case STT_OBJECT:
	  if (objtoff >= hp->cth_funcoff
	      || (gsp->st_shndx == SHN_EXTABS && gsp->st_value == 0))
	    {
	      *xp = -1u;
	      break;
	    }

	  *xp = objtoff;
	  objtoff += sizeof (uint32_t);
	  break;

	case STT_FUNC:
	  if (funcoff >= hp->cth_typeoff)
	    {
	      *xp = -1u;
	      break;
	    }

	  *xp = funcoff;

	  info = *(uint32_t *) ((uintptr_t) fp->ctf_buf + funcoff);
	  vlen = LCTF_INFO_VLEN (fp, info);

	  /* If we encounter a zero pad at the end, just skip it.  Otherwise
	     skip over the function and its return type (+2) and the argument
	     list (vlen).
	   */
	  if (LCTF_INFO_KIND (fp, info) == CTF_K_UNKNOWN && vlen == 0)
	    funcoff += sizeof (uint32_t);	/* Skip pad.  */
	  else
	    funcoff += sizeof (uint32_t) * (vlen + 2);
	  break;

	default:
	  *xp = -1u;
	  break;
	}
    }

  ctf_dprintf ("loaded %lu symtab entries\n", fp->ctf_nsyms);
  return 0;
}

/* Set the CTF base pointer and derive the buf pointer from it, initializing
   everything in the ctf_file that depends on the base or buf pointers.  */

static void
ctf_set_base (ctf_file_t *fp, const ctf_header_t *hp, void *base)
{
  fp->ctf_base = base;
  fp->ctf_buf = fp->ctf_base + sizeof (ctf_header_t);
  fp->ctf_vars = (ctf_varent_t *) ((const char *) fp->ctf_buf +
				   hp->cth_varoff);
  fp->ctf_nvars = (hp->cth_typeoff - hp->cth_varoff) / sizeof (ctf_varent_t);

  fp->ctf_str[CTF_STRTAB_0].cts_strs = (const char *) fp->ctf_buf
    + hp->cth_stroff;
  fp->ctf_str[CTF_STRTAB_0].cts_len = hp->cth_strlen;

  /* If we have a parent container name and label, store the relocated
     string pointers in the CTF container for easy access later. */

  /* Note: before conversion, these will be set to values that will be
     immediately invalidated by the conversion process, but the conversion
     process will call ctf_set_base() again to fix things up.  */

  if (hp->cth_parlabel != 0)
    fp->ctf_parlabel = ctf_strptr (fp, hp->cth_parlabel);
  if (hp->cth_parname != 0)
    fp->ctf_parname = ctf_strptr (fp, hp->cth_parname);

  ctf_dprintf ("ctf_set_base: parent name %s (label %s)\n",
	       fp->ctf_parname ? fp->ctf_parname : "<NULL>",
	       fp->ctf_parlabel ? fp->ctf_parlabel : "<NULL>");
}

/* Free a ctf_base pointer: the pointer passed, or (if NULL) fp->ctf_base.  */
static void
ctf_free_base (ctf_file_t *fp, unsigned char *ctf_base)
{
  unsigned char *base;

  if (ctf_base)
      base = ctf_base;
  else
      base = (unsigned char *) fp->ctf_base;

  if (base != fp->ctf_data.cts_data && base != NULL)
    ctf_free (base);
}

/* Set the version of the CTF file. */

/* When this is reset, LCTF_* changes behaviour, but there is no guarantee that
   the variable data list associated with each type has been upgraded: the
   caller must ensure this has been done in advance.  */

static void
ctf_set_version (ctf_file_t * fp, ctf_header_t * cth, int ctf_version)
{
  fp->ctf_version = ctf_version;
  cth->cth_version = ctf_version;
  fp->ctf_fileops = &ctf_fileops[ctf_version];
}

/* Upgrade the type table to CTF_VERSION_3 (really CTF_VERSION_1_UPGRADED_3).

   The upgrade is not done in-place: the ctf_base is moved.  ctf_strptr() must
   not be called before reallocation is complete.

   Type kinds not checked here due to nonexistence in older formats:
      CTF_K_SLICE.  */
static int
upgrade_types (ctf_file_t *fp, ctf_header_t *cth)
{
  const ctf_type_v1_t *tbuf;
  const ctf_type_v1_t *tend;
  unsigned char *ctf_base, *old_ctf_base = (unsigned char *) fp->ctf_base;
  ctf_type_t *t2buf;

  ssize_t increase = 0, size, increment, v2increment, vbytes, v2bytes;
  const ctf_type_v1_t *tp;
  ctf_type_t *t2p;
  ctf_header_t *new_cth;

  tbuf = (ctf_type_v1_t *) (fp->ctf_buf + cth->cth_typeoff);
  tend = (ctf_type_v1_t *) (fp->ctf_buf + cth->cth_stroff);

  /* Much like init_types(), this is a two-pass process.

     First, figure out the new type-section size needed.  (It is possible,
     in theory, for it to be less than the old size, but this is very
     unlikely.  It cannot be so small that cth_typeoff ends up of negative
     size.  We validate this with an assertion below.)

     We must cater not only for changes in vlen and types sizes but also
     for changes in 'increment', which happen because v2 places some types
     into ctf_stype_t where v1 would be forced to use the larger non-stype.  */

  for (tp = tbuf; tp < tend;
       tp = (ctf_type_v1_t *) ((uintptr_t) tp + increment + vbytes))
    {
      unsigned short kind = CTF_V1_INFO_KIND (tp->ctt_info);
      unsigned long vlen = CTF_V1_INFO_VLEN (tp->ctt_info);

      size = get_ctt_size_v1 (fp, (const ctf_type_t *) tp, NULL, &increment);
      vbytes = get_vbytes_v1 (kind, size, vlen);

      get_ctt_size_v2_unconverted (fp, (const ctf_type_t *) tp, NULL,
				   &v2increment);
      v2bytes = get_vbytes_v2 (kind, size, vlen);

      if ((vbytes < 0) || (size < 0))
	return ECTF_CORRUPT;

      increase += v2increment - increment;	/* May be negative.  */
      increase += v2bytes - vbytes;
    }

  /* Allocate enough room for the new buffer, then copy everything but the
     type section into place, and reset the base accordingly.  Leave the
     version number unchanged, so that LCTF_INFO_* still works on the
     as-yet-untranslated type info.  */

  if ((ctf_base = ctf_alloc (fp->ctf_size + increase)) == NULL)
    return ECTF_ZALLOC;

  memcpy (ctf_base, fp->ctf_base, sizeof (ctf_header_t) + cth->cth_typeoff);
  memcpy (ctf_base + sizeof (ctf_header_t) + cth->cth_stroff + increase,
	  fp->ctf_base + sizeof (ctf_header_t) + cth->cth_stroff,
	  cth->cth_strlen);

  memset (ctf_base + sizeof (ctf_header_t) + cth->cth_typeoff, 0,
	 cth->cth_stroff - cth->cth_typeoff + increase);

  /* The cth here is an automatic variable in ctf_bufopen(), and transient
     (a copy maintained because at that stage the header read out of the
     ctf file may be read-only). We make all modifications in the
     canonical copy at ctf_base (by now, writable), then copy it back into
     cth at the end.  */

  new_cth = (ctf_header_t *) ctf_base;
  new_cth->cth_stroff += increase;
  fp->ctf_size += increase;
  assert (new_cth->cth_stroff >= new_cth->cth_typeoff);
  ctf_set_base (fp, new_cth, ctf_base);

  t2buf = (ctf_type_t *) (fp->ctf_buf + new_cth->cth_typeoff);

  /* Iterate through all the types again, upgrading them.

     Everything that hasn't changed can just be outright memcpy()ed.
     Things that have changed need field-by-field consideration.  */

  for (tp = tbuf, t2p = t2buf; tp < tend;
       tp = (ctf_type_v1_t *) ((uintptr_t) tp + increment + vbytes),
       t2p = (ctf_type_t *) ((uintptr_t) t2p + v2increment + v2bytes))
    {
      unsigned short kind = CTF_V1_INFO_KIND (tp->ctt_info);
      int isroot = CTF_V1_INFO_ISROOT (tp->ctt_info);
      unsigned long vlen = CTF_V1_INFO_VLEN (tp->ctt_info);
      ssize_t v2size;
      void *vdata, *v2data;

      size = get_ctt_size_v1 (fp, (const ctf_type_t *) tp, NULL, &increment);
      vbytes = get_vbytes_v1 (kind, size, vlen);

      t2p->ctt_name = tp->ctt_name;
      t2p->ctt_info = CTF_TYPE_INFO (kind, isroot, vlen);

      switch (kind)
	{
	case CTF_K_FUNCTION:
	case CTF_K_FORWARD:
	case CTF_K_TYPEDEF:
	case CTF_K_POINTER:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	  t2p->ctt_type = tp->ctt_type;
	  break;
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	case CTF_K_ARRAY:
	case CTF_K_STRUCT:
	case CTF_K_UNION:
	case CTF_K_ENUM:
	case CTF_K_UNKNOWN:
	  if ((size_t) size <= CTF_MAX_SIZE)
	    t2p->ctt_size = size;
	  else
	    {
	      t2p->ctt_lsizehi = CTF_SIZE_TO_LSIZE_HI (size);
	      t2p->ctt_lsizelo = CTF_SIZE_TO_LSIZE_LO (size);
	    }
	  break;
	}

      v2size = get_ctt_size_v2 (fp, t2p, NULL, &v2increment);
      v2bytes = get_vbytes_v2 (kind, v2size, vlen);

      /* Catch out-of-sync get_ctt_size_*().  The count goes wrong if
	 these are not identical (and having them different makes no
	 sense semantically).  */

      assert (size == v2size);

      /* Now the varlen info.  */

      vdata = (void *) ((uintptr_t) tp + increment);
      v2data = (void *) ((uintptr_t) t2p + v2increment);

      switch (kind)
	{
	case CTF_K_ARRAY:
	  {
	    const ctf_array_v1_t *ap = (const ctf_array_v1_t *) vdata;
	    ctf_array_t *a2p = (ctf_array_t *) v2data;

	    a2p->cta_contents = ap->cta_contents;
	    a2p->cta_index = ap->cta_index;
	    a2p->cta_nelems = ap->cta_nelems;
	    break;
	  }
	case CTF_K_STRUCT:
	case CTF_K_UNION:
	  {
	    ctf_member_t tmp;
	    const ctf_member_v1_t *m1 = (const ctf_member_v1_t *) vdata;
	    const ctf_lmember_v1_t *lm1 = (const ctf_lmember_v1_t *) m1;
	    ctf_member_t *m2 = (ctf_member_t *) v2data;
	    ctf_lmember_t *lm2 = (ctf_lmember_t *) m2;
	    unsigned long i;

	    /* We walk all four pointers forward, but only reference the two
	       that are valid for the given size, to avoid quadruplicating all
	       the code.  */

	    for (i = vlen; i != 0; i--, m1++, lm1++, m2++, lm2++)
	      {
		size_t offset;
		if (size < CTF_LSTRUCT_THRESH_V1)
		  {
		    offset = m1->ctm_offset;
		    tmp.ctm_name = m1->ctm_name;
		    tmp.ctm_type = m1->ctm_type;
		  }
		else
		  {
		    offset = CTF_LMEM_OFFSET (lm1);
		    tmp.ctm_name = lm1->ctlm_name;
		    tmp.ctm_type = lm1->ctlm_type;
		  }
		if (size < CTF_LSTRUCT_THRESH)
		  {
		    m2->ctm_name = tmp.ctm_name;
		    m2->ctm_type = tmp.ctm_type;
		    m2->ctm_offset = offset;
		  }
		else
		  {
		    lm2->ctlm_name = tmp.ctm_name;
		    lm2->ctlm_type = tmp.ctm_type;
		    lm2->ctlm_offsethi = CTF_OFFSET_TO_LMEMHI (offset);
		    lm2->ctlm_offsetlo = CTF_OFFSET_TO_LMEMLO (offset);
		  }
	      }
	    break;
	  }
	case CTF_K_FUNCTION:
	  {
	    unsigned long i;
	    unsigned short *a1 = (unsigned short *) vdata;
	    uint32_t *a2 = (uint32_t *) v2data;

	    for (i = vlen; i != 0; i--, a1++, a2++)
	      *a2 = *a1;
	  }
	/* FALLTHRU */
	default:
	  /* Catch out-of-sync get_vbytes_*().  */
	  assert (vbytes == v2bytes);
	  memcpy (v2data, vdata, vbytes);
	}
    }

  /* Verify that the entire region was converted.  If not, we are either
     converting too much, or too little (leading to a buffer overrun either here
     or at read time, in init_types().) */

  assert ((size_t) t2p - (size_t) fp->ctf_buf == new_cth->cth_stroff);

  ctf_set_version (fp, (ctf_header_t *) ctf_base, CTF_VERSION_1_UPGRADED_3);
  ctf_free_base (fp, old_ctf_base);
  memcpy (cth, new_cth, sizeof (ctf_header_t));

  return 0;
}

/* Initialize the type ID translation table with the byte offset of each type,
   and initialize the hash tables of each named type.  Upgrade the type table to
   the latest supported representation in the process, if needed, and if this
   recension of libctf supports upgrading.  */

static int
init_types (ctf_file_t *fp, ctf_header_t *cth)
{
  const ctf_type_t *tbuf;
  const ctf_type_t *tend;

  unsigned long pop[CTF_K_MAX + 1] = { 0 };
  const ctf_type_t *tp;
  ctf_hash_t *hp;
  uint32_t id, dst;
  uint32_t *xp;

  /* We determine whether the container is a child or a parent based on
     the value of cth_parname.  */

  int child = cth->cth_parname != 0;
  int nlstructs = 0, nlunions = 0;
  int err;

  if (_libctf_unlikely_ (fp->ctf_version == CTF_VERSION_1))
    {
      int err;
      if ((err = upgrade_types (fp, cth)) != 0)
	return err;				/* Upgrade failed.  */
    }

  tbuf = (ctf_type_t *) (fp->ctf_buf + cth->cth_typeoff);
  tend = (ctf_type_t *) (fp->ctf_buf + cth->cth_stroff);

  /* We make two passes through the entire type section.  In this first
     pass, we count the number of each type and the total number of types.  */

  for (tp = tbuf; tp < tend; fp->ctf_typemax++)
    {
      unsigned short kind = LCTF_INFO_KIND (fp, tp->ctt_info);
      unsigned long vlen = LCTF_INFO_VLEN (fp, tp->ctt_info);
      ssize_t size, increment, vbytes;

      (void) ctf_get_ctt_size (fp, tp, &size, &increment);
      vbytes = LCTF_VBYTES (fp, kind, size, vlen);

      if (vbytes < 0)
	return ECTF_CORRUPT;

      if (kind == CTF_K_FORWARD)
	{
	  /* For forward declarations, ctt_type is the CTF_K_* kind for the tag,
	     so bump that population count too.  If ctt_type is unknown, treat
	     the tag as a struct.  */

	  if (tp->ctt_type == CTF_K_UNKNOWN || tp->ctt_type >= CTF_K_MAX)
	    pop[CTF_K_STRUCT]++;
	  else
	    pop[tp->ctt_type]++;
	}
      tp = (ctf_type_t *) ((uintptr_t) tp + increment + vbytes);
      pop[kind]++;
    }

  if (child)
    {
      ctf_dprintf ("CTF container %p is a child\n", (void *) fp);
      fp->ctf_flags |= LCTF_CHILD;
    }
  else
    ctf_dprintf ("CTF container %p is a parent\n", (void *) fp);

  /* Now that we've counted up the number of each type, we can allocate
     the hash tables, type translation table, and pointer table.  */

  if ((fp->ctf_structs = ctf_hash_create (pop[CTF_K_STRUCT], ctf_hash_string,
					  ctf_hash_eq_string)) == NULL)
    return ENOMEM;

  if ((fp->ctf_unions = ctf_hash_create (pop[CTF_K_UNION], ctf_hash_string,
					 ctf_hash_eq_string)) == NULL)
    return ENOMEM;

  if ((fp->ctf_enums = ctf_hash_create (pop[CTF_K_ENUM], ctf_hash_string,
					ctf_hash_eq_string)) == NULL)
    return ENOMEM;

  if ((fp->ctf_names = ctf_hash_create (pop[CTF_K_INTEGER] +
					pop[CTF_K_FLOAT] +
					pop[CTF_K_FUNCTION] +
					pop[CTF_K_TYPEDEF] +
					pop[CTF_K_POINTER] +
					pop[CTF_K_VOLATILE] +
					pop[CTF_K_CONST] +
					pop[CTF_K_RESTRICT],
					ctf_hash_string,
					ctf_hash_eq_string)) == NULL)
    return ENOMEM;

  fp->ctf_txlate = ctf_alloc (sizeof (uint32_t) * (fp->ctf_typemax + 1));
  fp->ctf_ptrtab = ctf_alloc (sizeof (uint32_t) * (fp->ctf_typemax + 1));

  if (fp->ctf_txlate == NULL || fp->ctf_ptrtab == NULL)
    return ENOMEM;		/* Memory allocation failed.  */

  xp = fp->ctf_txlate;
  *xp++ = 0;			/* Type id 0 is used as a sentinel value.  */

  memset (fp->ctf_txlate, 0, sizeof (uint32_t) * (fp->ctf_typemax + 1));
  memset (fp->ctf_ptrtab, 0, sizeof (uint32_t) * (fp->ctf_typemax + 1));

  /* In the second pass through the types, we fill in each entry of the
     type and pointer tables and add names to the appropriate hashes.  */

  for (id = 1, tp = tbuf; tp < tend; xp++, id++)
    {
      unsigned short kind = LCTF_INFO_KIND (fp, tp->ctt_info);
      unsigned short flag = LCTF_INFO_ISROOT (fp, tp->ctt_info);
      unsigned long vlen = LCTF_INFO_VLEN (fp, tp->ctt_info);
      ssize_t size, increment, vbytes;

      const char *name;

      (void) ctf_get_ctt_size (fp, tp, &size, &increment);
      name = ctf_strptr (fp, tp->ctt_name);
      vbytes = LCTF_VBYTES (fp, kind, size, vlen);

      switch (kind)
	{
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  /* Names are reused by bit-fields, which are differentiated by their
	     encodings, and so typically we'd record only the first instance of
	     a given intrinsic.  However, we replace an existing type with a
	     root-visible version so that we can be sure to find it when
	     checking for conflicting definitions in ctf_add_type().  */

	  if (((ctf_hash_lookup_type (fp->ctf_names, fp, name)) == 0)
	      || (flag & CTF_ADD_ROOT))
	    {
	      err = ctf_hash_define_type (fp->ctf_names, fp,
					  LCTF_INDEX_TO_TYPE (fp, id, child),
					  tp->ctt_name);
	      if (err != 0 && err != ECTF_STRTAB)
		return err;
	    }
	  break;

	  /* These kinds have no name, so do not need interning into any
	     hashtables.  */
	case CTF_K_ARRAY:
	case CTF_K_SLICE:
	  break;

	case CTF_K_FUNCTION:
	  err = ctf_hash_insert_type (fp->ctf_names, fp,
				      LCTF_INDEX_TO_TYPE (fp, id, child),
				      tp->ctt_name);
	  if (err != 0 && err != ECTF_STRTAB)
	    return err;
	  break;

	case CTF_K_STRUCT:
	  err = ctf_hash_define_type (fp->ctf_structs, fp,
				      LCTF_INDEX_TO_TYPE (fp, id, child),
				      tp->ctt_name);

	  if (err != 0 && err != ECTF_STRTAB)
	    return err;

	  if (size >= CTF_LSTRUCT_THRESH)
	    nlstructs++;
	  break;

	case CTF_K_UNION:
	  err = ctf_hash_define_type (fp->ctf_unions, fp,
				      LCTF_INDEX_TO_TYPE (fp, id, child),
				      tp->ctt_name);

	  if (err != 0 && err != ECTF_STRTAB)
	    return err;

	  if (size >= CTF_LSTRUCT_THRESH)
	    nlunions++;
	  break;

	case CTF_K_ENUM:
	  err = ctf_hash_define_type (fp->ctf_enums, fp,
				      LCTF_INDEX_TO_TYPE (fp, id, child),
				      tp->ctt_name);

	  if (err != 0 && err != ECTF_STRTAB)
	    return err;
	  break;

	case CTF_K_TYPEDEF:
	  err = ctf_hash_insert_type (fp->ctf_names, fp,
				      LCTF_INDEX_TO_TYPE (fp, id, child),
				      tp->ctt_name);
	  if (err != 0 && err != ECTF_STRTAB)
	    return err;
	  break;

	case CTF_K_FORWARD:
	  /* Only insert forward tags into the given hash if the type or tag
	     name is not already present.  */
	  switch (tp->ctt_type)
	    {
	    case CTF_K_STRUCT:
	      hp = fp->ctf_structs;
	      break;
	    case CTF_K_UNION:
	      hp = fp->ctf_unions;
	      break;
	    case CTF_K_ENUM:
	      hp = fp->ctf_enums;
	      break;
	    default:
	      hp = fp->ctf_structs;
	    }

	  if (ctf_hash_lookup_type (hp, fp, name) == 0)
	    {
	      err = ctf_hash_insert_type (hp, fp,
					  LCTF_INDEX_TO_TYPE (fp, id, child),
					  tp->ctt_name);
	      if (err != 0 && err != ECTF_STRTAB)
		return err;
	    }
	  break;

	case CTF_K_POINTER:
	  /* If the type referenced by the pointer is in this CTF container,
	     then store the index of the pointer type in
	     fp->ctf_ptrtab[ index of referenced type ].  */

	  if (LCTF_TYPE_ISCHILD (fp, tp->ctt_type) == child
	      && LCTF_TYPE_TO_INDEX (fp, tp->ctt_type) <= fp->ctf_typemax)
	    fp->ctf_ptrtab[LCTF_TYPE_TO_INDEX (fp, tp->ctt_type)] = id;
	 /*FALLTHRU*/

	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	  err = ctf_hash_insert_type (fp->ctf_names, fp,
				      LCTF_INDEX_TO_TYPE (fp, id, child),
				      tp->ctt_name);
	  if (err != 0 && err != ECTF_STRTAB)
	    return err;
	  break;
	default:
	  ctf_dprintf ("unhandled CTF kind in endianness conversion -- %x\n",
		       kind);
	  return ECTF_CORRUPT;
	}

      *xp = (uint32_t) ((uintptr_t) tp - (uintptr_t) fp->ctf_buf);
      tp = (ctf_type_t *) ((uintptr_t) tp + increment + vbytes);
    }

  ctf_dprintf ("%lu total types processed\n", fp->ctf_typemax);
  ctf_dprintf ("%u enum names hashed\n", ctf_hash_size (fp->ctf_enums));
  ctf_dprintf ("%u struct names hashed (%d long)\n",
	       ctf_hash_size (fp->ctf_structs), nlstructs);
  ctf_dprintf ("%u union names hashed (%d long)\n",
	       ctf_hash_size (fp->ctf_unions), nlunions);
  ctf_dprintf ("%u base type names hashed\n", ctf_hash_size (fp->ctf_names));

  /* Make an additional pass through the pointer table to find pointers that
     point to anonymous typedef nodes.  If we find one, modify the pointer table
     so that the pointer is also known to point to the node that is referenced
     by the anonymous typedef node.  */

  for (id = 1; id <= fp->ctf_typemax; id++)
    {
      if ((dst = fp->ctf_ptrtab[id]) != 0)
	{
	  tp = LCTF_INDEX_TO_TYPEPTR (fp, id);

	  if (LCTF_INFO_KIND (fp, tp->ctt_info) == CTF_K_TYPEDEF &&
	      strcmp (ctf_strptr (fp, tp->ctt_name), "") == 0 &&
	      LCTF_TYPE_ISCHILD (fp, tp->ctt_type) == child &&
	      LCTF_TYPE_TO_INDEX (fp, tp->ctt_type) <= fp->ctf_typemax)
	    fp->ctf_ptrtab[LCTF_TYPE_TO_INDEX (fp, tp->ctt_type)] = dst;
	}
    }

  return 0;
}

/* Endianness-flipping routines.

   We flip everything, mindlessly, even 1-byte entities, so that future
   expansions do not require changes to this code.  */

/* < C11? define away static assertions.  */

#if !defined (__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define _Static_assert(cond, err)
#endif

/* Swap the endianness of something.  */

#define swap_thing(x)							\
  do {									\
    _Static_assert (sizeof (x) == 1 || (sizeof (x) % 2 == 0		\
					&& sizeof (x) <= 8),		\
		    "Invalid size, update endianness code");		\
    switch (sizeof (x)) {						\
    case 2: x = bswap_16 (x); break;					\
    case 4: x = bswap_32 (x); break;					\
    case 8: x = bswap_64 (x); break;					\
    case 1: /* Nothing needs doing */					\
      break;								\
    }									\
  } while (0);

/* Flip the endianness of the CTF header.  */

static void
flip_header (ctf_header_t *cth)
{
  swap_thing (cth->cth_preamble.ctp_magic);
  swap_thing (cth->cth_preamble.ctp_version);
  swap_thing (cth->cth_preamble.ctp_flags);
  swap_thing (cth->cth_parlabel);
  swap_thing (cth->cth_parname);
  swap_thing (cth->cth_objtoff);
  swap_thing (cth->cth_funcoff);
  swap_thing (cth->cth_varoff);
  swap_thing (cth->cth_typeoff);
  swap_thing (cth->cth_stroff);
  swap_thing (cth->cth_strlen);
}

/* Flip the endianness of the label section, an array of ctf_lblent_t.  */

static void
flip_lbls (void *start, size_t len)
{
  ctf_lblent_t *lbl = start;

  for (ssize_t i = len / sizeof (struct ctf_lblent); i > 0; lbl++, i--)
    {
      swap_thing (lbl->ctl_label);
      swap_thing (lbl->ctl_type);
    }
}

/* Flip the endianness of the data-object or function sections, an array of
   uint32_t.  (The function section has more internal structure, but that
   structure is an array of uint32_t, so can be treated as one big array for
   byte-swapping.)  */

static void
flip_objts (void *start, size_t len)
{
  uint32_t *obj = start;

  for (ssize_t i = len / sizeof (uint32_t); i > 0; obj++, i--)
      swap_thing (*obj);
}

/* Flip the endianness of the variable section, an array of ctf_varent_t.  */

static void
flip_vars (void *start, size_t len)
{
  ctf_varent_t *var = start;

  for (ssize_t i = len / sizeof (struct ctf_varent); i > 0; var++, i--)
    {
      swap_thing (var->ctv_name);
      swap_thing (var->ctv_type);
    }
}

/* Flip the endianness of the type section, a tagged array of ctf_type or
   ctf_stype followed by variable data.  */

static int
flip_types (void *start, size_t len)
{
  ctf_type_t *t = start;

  while ((uintptr_t) t < ((uintptr_t) start) + len)
    {
      swap_thing (t->ctt_name);
      swap_thing (t->ctt_info);
      swap_thing (t->ctt_size);

      uint32_t kind = CTF_V2_INFO_KIND (t->ctt_info);
      size_t size = t->ctt_size;
      uint32_t vlen = CTF_V2_INFO_VLEN (t->ctt_info);
      size_t vbytes = get_vbytes_v2 (kind, size, vlen);

      if (_libctf_unlikely_ (size == CTF_LSIZE_SENT))
	{
	  swap_thing (t->ctt_lsizehi);
	  swap_thing (t->ctt_lsizelo);
	  size = CTF_TYPE_LSIZE (t);
	  t = (ctf_type_t *) ((uintptr_t) t + sizeof (ctf_type_t));
	}
      else
	t = (ctf_type_t *) ((uintptr_t) t + sizeof (ctf_stype_t));

      switch (kind)
	{
	case CTF_K_FORWARD:
	case CTF_K_UNKNOWN:
	case CTF_K_POINTER:
	case CTF_K_TYPEDEF:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	  /* These types have no vlen data to swap.  */
	  assert (vbytes == 0);
	  break;

	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	  {
	    /* These types have a single uint32_t.  */

	    uint32_t *item = (uint32_t *) t;

	    swap_thing (*item);
	    break;
	  }

	case CTF_K_FUNCTION:
	  {
	    /* This type has a bunch of uint32_ts.  */

	    uint32_t *item = (uint32_t *) t;

	    for (ssize_t i = vlen; i > 0; item++, i--)
	      swap_thing (*item);
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
	    /* This has an array of ctf_member or ctf_lmember, depending on
	       size.  We could consider it to be a simple array of uint32_t,
	       but for safety's sake in case these structures ever acquire
	       non-uint32_t members, do it member by member.  */

	    if (_libctf_unlikely_ (size >= CTF_LSTRUCT_THRESH))
	      {
		ctf_lmember_t *lm = (ctf_lmember_t *) t;
		for (ssize_t i = vlen; i > 0; i--, lm++)
		  {
		    swap_thing (lm->ctlm_name);
		    swap_thing (lm->ctlm_offsethi);
		    swap_thing (lm->ctlm_type);
		    swap_thing (lm->ctlm_offsetlo);
		  }
	      }
	    else
	      {
		ctf_member_t *m = (ctf_member_t *) t;
		for (ssize_t i = vlen; i > 0; i--, m++)
		  {
		    swap_thing (m->ctm_name);
		    swap_thing (m->ctm_offset);
		    swap_thing (m->ctm_type);
		  }
	      }
	    break;
	  }

	case CTF_K_ENUM:
	  {
	    /* This has an array of ctf_enum_t.  */

	    ctf_enum_t *item = (ctf_enum_t *) t;

	    for (ssize_t i = vlen; i > 0; item++, i--)
	      {
		swap_thing (item->cte_name);
		swap_thing (item->cte_value);
	      }
	    break;
	  }
	default:
	  ctf_dprintf ("unhandled CTF kind in endianness conversion -- %x\n",
		       kind);
	  return ECTF_CORRUPT;
	}

      t = (ctf_type_t *) ((uintptr_t) t + vbytes);
    }

  return 0;
}

/* Flip the endianness of BASE, given the offsets in the (already endian-
   converted) CTH.

   All of this stuff happens before the header is fully initialized, so the
   LCTF_*() macros cannot be used yet.  Since we do not try to endian-convert v1
   data, this is no real loss.  */

static int
flip_ctf (ctf_header_t *cth, unsigned char *base)
{
  base += sizeof (ctf_header_t);

  flip_lbls (base + cth->cth_lbloff, cth->cth_objtoff - cth->cth_lbloff);
  flip_objts (base + cth->cth_objtoff, cth->cth_funcoff - cth->cth_objtoff);
  flip_objts (base + cth->cth_funcoff, cth->cth_varoff - cth->cth_funcoff);
  flip_vars (base + cth->cth_varoff, cth->cth_typeoff - cth->cth_varoff);
  return flip_types (base + cth->cth_typeoff, cth->cth_stroff - cth->cth_typeoff);
}

/* Open a CTF file, mocking up a suitable ctf_sect.  */
ctf_file_t *ctf_simple_open (const char *ctfsect, size_t ctfsect_size,
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

/* Decode the specified CTF buffer and optional symbol table, and create a new
   CTF container representing the symbolic debugging information.  This code can
   be used directly by the debugger, or it can be used as the engine for
   ctf_fdopen() or ctf_open(), below.  */

ctf_file_t *
ctf_bufopen (const ctf_sect_t *ctfsect, const ctf_sect_t *symsect,
	     const ctf_sect_t *strsect, int *errp)
{
  const ctf_preamble_t *pp;
  ctf_header_t hp;
  ctf_file_t *fp;
  void *base;
  size_t size, hdrsz;
  int foreign_endian = 0;
  int err;

  libctf_init_debug();

  if (ctfsect == NULL || ((symsect == NULL) != (strsect == NULL)))
    return (ctf_set_open_errno (errp, EINVAL));

  if (symsect != NULL && symsect->cts_entsize != sizeof (Elf32_Sym) &&
      symsect->cts_entsize != sizeof (Elf64_Sym))
    return (ctf_set_open_errno (errp, ECTF_SYMTAB));

  if (symsect != NULL && symsect->cts_data == NULL)
    return (ctf_set_open_errno (errp, ECTF_SYMBAD));

  if (strsect != NULL && strsect->cts_data == NULL)
    return (ctf_set_open_errno (errp, ECTF_STRBAD));

  if (ctfsect->cts_size < sizeof (ctf_preamble_t))
    return (ctf_set_open_errno (errp, ECTF_NOCTFBUF));

  pp = (const ctf_preamble_t *) ctfsect->cts_data;

  ctf_dprintf ("ctf_bufopen: magic=0x%x version=%u\n",
	       pp->ctp_magic, pp->ctp_version);

  /* Validate each part of the CTF header.

     First, we validate the preamble (common to all versions).  At that point,
     we know the endianness and specific header version, and can validate the
     version-specific parts including section offsets and alignments.

     We specifically do not support foreign-endian old versions.  */

  if (_libctf_unlikely_ (pp->ctp_magic != CTF_MAGIC))
    {
      if (pp->ctp_magic == bswap_16 (CTF_MAGIC))
	{
	  if (pp->ctp_version != CTF_VERSION_3)
	    return (ctf_set_open_errno (errp, ECTF_CTFVERS));
	  foreign_endian = 1;
	}
      else
	return (ctf_set_open_errno (errp, ECTF_NOCTFBUF));
    }

  if (_libctf_unlikely_ ((pp->ctp_version < CTF_VERSION_1)
			 || (pp->ctp_version > CTF_VERSION_3)))
    return (ctf_set_open_errno (errp, ECTF_CTFVERS));

  if ((symsect != NULL) && (pp->ctp_version < CTF_VERSION_2))
    {
      /* The symtab can contain function entries which contain embedded ctf
	 info.  We do not support dynamically upgrading such entries (none
	 should exist in any case, since dwarf2ctf does not create them).  */

      ctf_dprintf ("ctf_bufopen: CTF version %d symsect not "
		   "supported\n", pp->ctp_version);
      return (ctf_set_open_errno (errp, ECTF_NOTSUP));
    }

  if (ctfsect->cts_size < sizeof (ctf_header_t))
    return (ctf_set_open_errno (errp, ECTF_NOCTFBUF));

  memcpy (&hp, ctfsect->cts_data, sizeof (hp));

  if (foreign_endian)
    flip_header (&hp);

  ctf_dprintf ("header offsets: %x/%x/%x/%x/%x/%x/%x\n",
	       hp.cth_lbloff, hp.cth_objtoff, hp.cth_funcoff, hp.cth_varoff,
	       hp.cth_typeoff, hp.cth_stroff, hp.cth_strlen);
  hdrsz = sizeof (ctf_header_t);

  size = hp.cth_stroff + hp.cth_strlen;

  ctf_dprintf ("ctf_bufopen: uncompressed size=%lu\n", (unsigned long) size);

  if (hp.cth_lbloff > size || hp.cth_objtoff > size
      || hp.cth_funcoff > size || hp.cth_typeoff > size || hp.cth_stroff > size)
    return (ctf_set_open_errno (errp, ECTF_CORRUPT));

  if (hp.cth_lbloff > hp.cth_objtoff
      || hp.cth_objtoff > hp.cth_funcoff
      || hp.cth_funcoff > hp.cth_typeoff
      || hp.cth_funcoff > hp.cth_varoff
      || hp.cth_varoff > hp.cth_typeoff || hp.cth_typeoff > hp.cth_stroff)
    return (ctf_set_open_errno (errp, ECTF_CORRUPT));

  if ((hp.cth_lbloff & 3) || (hp.cth_objtoff & 1)
      || (hp.cth_funcoff & 1) || (hp.cth_varoff & 3) || (hp.cth_typeoff & 3))
    return (ctf_set_open_errno (errp, ECTF_CORRUPT));

  /* Once everything is determined to be valid, attempt to decompress the CTF
     data buffer if it is compressed, or copy it into new storage if it is not
     compressed but needs endian-flipping.  Otherwise we just put the data
     section's buffer pointer into ctf_buf, below.  */

  /* Note: if this is a v1 buffer, it will be reallocated and expanded by
     init_types().  */

  if (hp.cth_flags & CTF_F_COMPRESS)
    {
      size_t srclen;
      uLongf dstlen;
      const void *src;
      int rc = Z_OK;
      void *buf;

      if ((base = ctf_alloc (size + hdrsz)) == NULL)
	return (ctf_set_open_errno (errp, ECTF_ZALLOC));

      memcpy (base, ctfsect->cts_data, hdrsz);
      ((ctf_preamble_t *) base)->ctp_flags &= ~CTF_F_COMPRESS;
      buf = (unsigned char *) base + hdrsz;

      src = (unsigned char *) ctfsect->cts_data + hdrsz;
      srclen = ctfsect->cts_size - hdrsz;
      dstlen = size;

      if ((rc = uncompress (buf, &dstlen, src, srclen)) != Z_OK)
	{
	  ctf_dprintf ("zlib inflate err: %s\n", zError (rc));
	  free (base);
	  return (ctf_set_open_errno (errp, ECTF_DECOMPRESS));
	}

      if ((size_t) dstlen != size)
	{
	  ctf_dprintf ("zlib inflate short -- got %lu of %lu "
		       "bytes\n", (unsigned long) dstlen, (unsigned long) size);
	  free (base);
	  return (ctf_set_open_errno (errp, ECTF_CORRUPT));
	}

    }
  else if (foreign_endian)
    {
      if ((base = ctf_alloc (size + hdrsz)) == NULL)
	return (ctf_set_open_errno (errp, ECTF_ZALLOC));
      memcpy (base, ctfsect->cts_data, size + hdrsz);
    }
  else
    base = (void *) ctfsect->cts_data;

  /* Flip the endianness of the copy of the header in the section, to avoid
     ending up with a partially-endian-flipped file.  */

  if (foreign_endian)
    flip_header ((ctf_header_t *) base);

  /* Once we have uncompressed and validated the CTF data buffer, we can
     proceed with allocating a ctf_file_t and initializing it.

     Nothing that depends on buf or base should be set directly in this function
     before the init_types() call, because it may be reallocated during
     transparent upgrade if this recension of libctf is so configured: see
     ctf_set_base() and ctf_realloc_base().  */

  if ((fp = ctf_alloc (sizeof (ctf_file_t))) == NULL)
    return (ctf_set_open_errno (errp, ENOMEM));

  memset (fp, 0, sizeof (ctf_file_t));
  ctf_set_version (fp, &hp, hp.cth_version);
  ctf_str_create_atoms (fp);

  if (_libctf_unlikely_ (hp.cth_version < CTF_VERSION_2))
    fp->ctf_parmax = CTF_MAX_PTYPE_V1;
  else
    fp->ctf_parmax = CTF_MAX_PTYPE;

  memcpy (&fp->ctf_data, ctfsect, sizeof (ctf_sect_t));

  if (symsect != NULL)
    {
      memcpy (&fp->ctf_symtab, symsect, sizeof (ctf_sect_t));
      memcpy (&fp->ctf_strtab, strsect, sizeof (ctf_sect_t));
    }

  if (fp->ctf_data.cts_name != NULL)
    fp->ctf_data.cts_name = ctf_strdup (fp->ctf_data.cts_name);
  if (fp->ctf_symtab.cts_name != NULL)
    fp->ctf_symtab.cts_name = ctf_strdup (fp->ctf_symtab.cts_name);
  if (fp->ctf_strtab.cts_name != NULL)
    fp->ctf_strtab.cts_name = ctf_strdup (fp->ctf_strtab.cts_name);

  if (fp->ctf_data.cts_name == NULL)
    fp->ctf_data.cts_name = _CTF_NULLSTR;
  if (fp->ctf_symtab.cts_name == NULL)
    fp->ctf_symtab.cts_name = _CTF_NULLSTR;
  if (fp->ctf_strtab.cts_name == NULL)
    fp->ctf_strtab.cts_name = _CTF_NULLSTR;

  if (strsect != NULL)
    {
      fp->ctf_str[CTF_STRTAB_1].cts_strs = strsect->cts_data;
      fp->ctf_str[CTF_STRTAB_1].cts_len = strsect->cts_size;
    }

  if (foreign_endian &&
      (err = flip_ctf (&hp, base)) != 0)
    {
      /* We can be certain that flip_ctf() will have endian-flipped everything
         other than the types table when we return.  In particular the header
         is fine, so set it, to allow freeing to use the usual code path.  */

      (void) ctf_set_open_errno (errp, err);
      ctf_set_base (fp, &hp, base);
      goto bad;
    }

  ctf_set_base (fp, &hp, base);
  fp->ctf_size = size + hdrsz;

  if ((err = init_types (fp, &hp)) != 0)
    {
      (void) ctf_set_open_errno (errp, err);
      goto bad;
    }

  /* If we have a symbol table section, allocate and initialize
     the symtab translation table, pointed to by ctf_sxlate.  */

  if (symsect != NULL)
    {
      fp->ctf_nsyms = symsect->cts_size / symsect->cts_entsize;
      fp->ctf_sxlate = ctf_alloc (fp->ctf_nsyms * sizeof (uint32_t));

      if (fp->ctf_sxlate == NULL)
	{
	  (void) ctf_set_open_errno (errp, ENOMEM);
	  goto bad;
	}

      if ((err = init_symtab (fp, &hp, symsect, strsect)) != 0)
	{
	  (void) ctf_set_open_errno (errp, err);
	  goto bad;
	}
    }

  /* Initialize the ctf_lookup_by_name top-level dictionary.  We keep an
     array of type name prefixes and the corresponding ctf_hash to use.
     NOTE: This code must be kept in sync with the code in ctf_update().  */
  fp->ctf_lookups[0].ctl_prefix = "struct";
  fp->ctf_lookups[0].ctl_len = strlen (fp->ctf_lookups[0].ctl_prefix);
  fp->ctf_lookups[0].ctl_hash = fp->ctf_structs;
  fp->ctf_lookups[1].ctl_prefix = "union";
  fp->ctf_lookups[1].ctl_len = strlen (fp->ctf_lookups[1].ctl_prefix);
  fp->ctf_lookups[1].ctl_hash = fp->ctf_unions;
  fp->ctf_lookups[2].ctl_prefix = "enum";
  fp->ctf_lookups[2].ctl_len = strlen (fp->ctf_lookups[2].ctl_prefix);
  fp->ctf_lookups[2].ctl_hash = fp->ctf_enums;
  fp->ctf_lookups[3].ctl_prefix = _CTF_NULLSTR;
  fp->ctf_lookups[3].ctl_len = strlen (fp->ctf_lookups[3].ctl_prefix);
  fp->ctf_lookups[3].ctl_hash = fp->ctf_names;
  fp->ctf_lookups[4].ctl_prefix = NULL;
  fp->ctf_lookups[4].ctl_len = 0;
  fp->ctf_lookups[4].ctl_hash = NULL;

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

bad:
  ctf_file_close (fp);
  return NULL;
}

/* Close the specified CTF container and free associated data structures.  Note
   that ctf_file_close() is a reference counted operation: if the specified file
   is the parent of other active containers, its reference count will be greater
   than one and it will be freed later when no active children exist.  */

void
ctf_file_close (ctf_file_t *fp)
{
  ctf_dtdef_t *dtd, *ntd;
  ctf_dvdef_t *dvd, *nvd;

  if (fp == NULL)
    return;		   /* Allow ctf_file_close(NULL) to simplify caller code.  */

  ctf_dprintf ("ctf_file_close(%p) refcnt=%u\n", (void *) fp, fp->ctf_refcnt);

  if (fp->ctf_refcnt > 1)
    {
      fp->ctf_refcnt--;
      return;
    }

  if (fp->ctf_dynparname != NULL)
    ctf_free (fp->ctf_dynparname);

  if (fp->ctf_parent != NULL)
    ctf_file_close (fp->ctf_parent);

  for (dtd = ctf_list_next (&fp->ctf_dtdefs); dtd != NULL; dtd = ntd)
    {
      ntd = ctf_list_next (dtd);
      ctf_dtd_delete (fp, dtd);
    }
  ctf_dynhash_destroy (fp->ctf_dthash);
  ctf_dynhash_destroy (fp->ctf_dtbyname);

  for (dvd = ctf_list_next (&fp->ctf_dvdefs); dvd != NULL; dvd = nvd)
    {
      nvd = ctf_list_next (dvd);
      ctf_dvd_delete (fp, dvd);
    }
  ctf_dynhash_destroy (fp->ctf_dvhash);
  ctf_str_free_atoms (fp);

  ctf_free (fp->ctf_tmp_typeslice);

  if (fp->ctf_data.cts_name != _CTF_NULLSTR &&
      fp->ctf_data.cts_name != NULL)
    ctf_free ((char *) fp->ctf_data.cts_name);

  if (fp->ctf_symtab.cts_name != _CTF_NULLSTR &&
      fp->ctf_symtab.cts_name != NULL)
    ctf_free ((char *) fp->ctf_symtab.cts_name);

  if (fp->ctf_strtab.cts_name != _CTF_NULLSTR &&
      fp->ctf_strtab.cts_name != NULL)
    ctf_free ((char *) fp->ctf_strtab.cts_name);

  else if (fp->ctf_data_mmapped)
    ctf_munmap (fp->ctf_data_mmapped, fp->ctf_data_mmapped_len);

  ctf_free_base (fp, NULL);

  if (fp->ctf_sxlate != NULL)
    ctf_free (fp->ctf_sxlate);

  if (fp->ctf_txlate != NULL)
    ctf_free (fp->ctf_txlate);

  if (fp->ctf_ptrtab != NULL)
    ctf_free (fp->ctf_ptrtab);

  ctf_hash_destroy (fp->ctf_structs);
  ctf_hash_destroy (fp->ctf_unions);
  ctf_hash_destroy (fp->ctf_enums);
  ctf_hash_destroy (fp->ctf_names);

  ctf_free (fp);
}

/* The converse of ctf_open().  ctf_open() disguises whatever it opens as an
   archive, so closing one is just like closing an archive.  */
void
ctf_close (ctf_archive_t *arc)
{
  ctf_arc_close (arc);
}

/* Get the CTF archive from which this ctf_file_t is derived.  */
ctf_archive_t *
ctf_get_arc (const ctf_file_t *fp)
{
  return fp->ctf_archive;
}

/* Return the ctfsect out of the core ctf_impl.  Useful for freeing the
   ctfsect's data * after ctf_file_close(), which is why we return the actual
   structure, not a pointer to it, since that is likely to become a pointer to
   freed data before the return value is used under the expected use case of
   ctf_getsect()/ ctf_file_close()/free().  */
extern ctf_sect_t
ctf_getdatasect (const ctf_file_t *fp)
{
  return fp->ctf_data;
}

/* Return the CTF handle for the parent CTF container, if one exists.
   Otherwise return NULL to indicate this container has no imported parent.  */
ctf_file_t *
ctf_parent_file (ctf_file_t *fp)
{
  return fp->ctf_parent;
}

/* Return the name of the parent CTF container, if one exists.  Otherwise
   return NULL to indicate this container is a root container.  */
const char *
ctf_parent_name (ctf_file_t *fp)
{
  return fp->ctf_parname;
}

/* Set the parent name.  It is an error to call this routine without calling
   ctf_import() at some point.  */
void
ctf_parent_name_set (ctf_file_t *fp, const char *name)
{
  if (fp->ctf_dynparname != NULL)
    ctf_free (fp->ctf_dynparname);

  fp->ctf_dynparname = ctf_strdup (name);
  fp->ctf_parname = fp->ctf_dynparname;
}

/* Import the types from the specified parent container by storing a pointer
   to it in ctf_parent and incrementing its reference count.  Only one parent
   is allowed: if a parent already exists, it is replaced by the new parent.  */
int
ctf_import (ctf_file_t *fp, ctf_file_t *pfp)
{
  if (fp == NULL || fp == pfp || (pfp != NULL && pfp->ctf_refcnt == 0))
    return (ctf_set_errno (fp, EINVAL));

  if (pfp != NULL && pfp->ctf_dmodel != fp->ctf_dmodel)
    return (ctf_set_errno (fp, ECTF_DMODEL));

  if (fp->ctf_parent != NULL)
    ctf_file_close (fp->ctf_parent);

  if (pfp != NULL)
    {
      fp->ctf_flags |= LCTF_CHILD;
      pfp->ctf_refcnt++;

      if (fp->ctf_parname == NULL)
	ctf_parent_name_set (fp, "PARENT");
    }
  fp->ctf_parent = pfp;
  return 0;
}

/* Set the data model constant for the CTF container.  */
int
ctf_setmodel (ctf_file_t *fp, int model)
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

/* Return the data model constant for the CTF container.  */
int
ctf_getmodel (ctf_file_t *fp)
{
  return fp->ctf_dmodel->ctd_code;
}

/* The caller can hang an arbitrary pointer off each ctf_file_t using this
   function.  */
void
ctf_setspecific (ctf_file_t *fp, void *data)
{
  fp->ctf_specific = data;
}

/* Retrieve the arbitrary pointer again.  */
void *
ctf_getspecific (ctf_file_t *fp)
{
  return fp->ctf_specific;
}
