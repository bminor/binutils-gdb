kind/* Opening CTF files: back-compatibility.
   (TODO: not yet implemented, not yet tied in to build system.)
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
#include "swap.h"
#include <bfd.h>
#include <zlib.h>

/* Upgrade the header to CTF_VERSION_4.  The upgrade is done in-place,
   end-to-start. UPTODO: redo this */
void
upgrade_header_v2 (ctf_header_t *hp)
{
  ctf_header_v2_t *oldhp = (ctf_header_v2_t *) hp;

  hp->cth_strlen = oldhp->cth_strlen;
  hp->cth_stroff = oldhp->cth_stroff;
  hp->cth_typeoff = oldhp->cth_typeoff;
  hp->cth_varoff = oldhp->cth_varoff;
  hp->cth_funcidxoff = hp->cth_varoff;		/* No index sections.  */
  hp->cth_objtidxoff = hp->cth_funcidxoff;
  hp->cth_funcoff = oldhp->cth_funcoff;
  hp->cth_objtoff = oldhp->cth_objtoff;
  hp->cth_lbloff = oldhp->cth_lbloff;
  hp->cth_parent_strlen = 0;			/* Strings start at offset 0.  */
  hp->cth_cu_name = 0;				/* No CU name.  */
}

/* Ditto, for CTFv3. UPTODO: redo this */
void
upgrade_header_v3 (ctf_header_t *hp)
{
  ctf_header_v3_t *oldhp = (ctf_header_v3_t *) hp;

  hp->btf.bth_str_len = oldhp->cth_strlen;
  hp->btf.bth_str_off = oldhp->cth_stroff;
  hp->btf.bth_type_off = oldhp->cth_typeoff;
  hp->cth_funcidx_off = oldhp->cth_funcidxoff;
  hp->cth_objtidx_off = oldhp->cth_objtidxoff;
  hp->cth_func_off = oldhp->cth_funcoff;
  hp->cth_objt_off = oldhp->cth_objtoff;
  /* lens */
  hp->cth_lbloff = oldhp->cth_lbloff;
  hp->cth_parent_strlen = 0;			/* Strings start at offset 0.  */
  hp->cth_cu_name = oldhp->cth_cuname;
  hp->cth_parent_name = oldhp->cth_parname;
  hp->cth_parent_ntypes = 0;
}

/* Set the version of the CTF file. */

/* When this is reset, LCTF_* changes behaviour, but there is no guarantee that
   the variable data list associated with each type has been upgraded: the
   caller must ensure this has been done in advance.  */

static void
ctf_set_version (ctf_dict_t *fp, ctf_header_t *cth, int ctf_version)
{
  fp->ctf_version = ctf_version;
  cth->cth_version = ctf_version;
  fp->ctf_dictops = &ctf_dictops[ctf_version];
}

/* Upgrade the type table to CTF_VERSION_3 (really CTF_VERSION_1_UPGRADED_3)
   from CTF_VERSION_1.

   The upgrade is not done in-place: the ctf_base is moved.  ctf_strptr() must
   not be called before reallocation is complete.

   Sections not checked here due to nonexistence or nonpopulated state in older
   formats: objtidx, funcidx.

   Type kinds not checked here due to nonexistence in older formats:
      CTF_K_SLICE.  */
static int
upgrade_types_v1 (ctf_dict_t *fp, ctf_header_t *cth)
{
  const ctf_type_v1_t *tbuf;
  const ctf_type_v1_t *tend;
  unsigned char *ctf_base, *old_ctf_base = (unsigned char *) fp->ctf_dynbase;
  ctf_type_t *t2buf;

  ssize_t increase = 0, size, increment, v2increment, vbytes, v2bytes;
  const ctf_type_v1_t *tp;
  ctf_type_t *t2p;

  tbuf = (ctf_type_v1_t *) (fp->ctf_buf + cth->cth_typeoff);
  tend = (ctf_type_v1_t *) (fp->ctf_buf + cth->cth_stroff);

  /* This is a two-pass process.

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
      ctf_kind_t kind = CTF_V1_INFO_KIND (tp->ctt_info);
      unsigned long vlen = CTF_V1_INFO_VLEN (tp->ctt_info);

      size = get_ctt_size_v1 (fp, (const ctf_type_t *) tp, NULL, &increment);
      vbytes = get_vbytes_v1 (fp, kind, size, vlen);

      get_ctt_size_v2_unconverted (fp, (const ctf_type_t *) tp, NULL,
				   &v2increment);
      v2bytes = get_vbytes_v2 (fp, kind, size, vlen);

      if ((vbytes < 0) || (size < 0))
	return ECTF_CORRUPT;

      increase += v2increment - increment;	/* May be negative.  */
      increase += v2bytes - vbytes;
    }

  /* Allocate enough room for the new buffer, then copy everything but the type
     section into place, and reset the base accordingly.  Leave the version
     number unchanged, so that LCTF_INFO_* still works on the
     as-yet-untranslated type info.  */

  if ((ctf_base = malloc (fp->ctf_size + increase)) == NULL)
    return ECTF_ZALLOC;

  /* Start at ctf_buf, not ctf_base, to squeeze out the original header: we
     never use it and it is unconverted.  */

  memcpy (ctf_base, fp->ctf_buf, cth->cth_typeoff);
  memcpy (ctf_base + cth->cth_stroff + increase,
	  fp->ctf_buf + cth->cth_stroff, cth->cth_strlen);

  memset (ctf_base + cth->cth_typeoff, 0, cth->cth_stroff - cth->cth_typeoff
	  + increase);

  cth->cth_stroff += increase;
  fp->ctf_size += increase;
  assert (cth->cth_stroff >= cth->cth_typeoff);
  fp->ctf_base = ctf_base;
  fp->ctf_buf = ctf_base;
  fp->ctf_dynbase = ctf_base;
  ctf_set_base (fp, cth, ctf_base);

  t2buf = (ctf_type_t *) (fp->ctf_buf + cth->cth_typeoff);

  /* Iterate through all the types again, upgrading them.

     Everything that hasn't changed can just be outright memcpy()ed.
     Things that have changed need field-by-field consideration.  */

  for (tp = tbuf, t2p = t2buf; tp < tend;
       tp = (ctf_type_v1_t *) ((uintptr_t) tp + increment + vbytes),
       t2p = (ctf_type_t *) ((uintptr_t) t2p + v2increment + v2bytes))
    {
      ctf_kind_t kind = CTF_V1_INFO_KIND (tp->ctt_info);
      int isroot = CTF_V1_INFO_ISROOT (tp->ctt_info);
      unsigned long vlen = CTF_V1_INFO_VLEN (tp->ctt_info);
      ssize_t v2size;
      void *vdata, *v2data;

      size = get_ctt_size_v1 (fp, (const ctf_type_t *) tp, NULL, &increment);
      vbytes = get_vbytes_v1 (fp, kind, size, vlen);

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
      v2bytes = get_vbytes_v2 (fp, kind, v2size, vlen);

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
     or at read time, in init_static_types().) */

  assert ((size_t) t2p - (size_t) fp->ctf_buf == cth->cth_stroff);

  ctf_set_version (fp, cth, CTF_VERSION_1_UPGRADED_3);
  free (old_ctf_base);

  return 0;
}

/* Upgrade from any earlier version.  */
static int
upgrade_types (ctf_dict_t *fp, ctf_header_t *cth)
{
  switch (cth->cth_version)
    {
      /* v1 requires a full pass and reformatting.  */
    case CTF_VERSION_1:
      upgrade_types_v1 (fp, cth);
      /* FALLTHRU */
      /* Already-converted v1 is just like later versions except that its
	 parent/child boundary is unchanged (and much lower).  */

    case CTF_VERSION_1_UPGRADED_3:
      fp->ctf_header->cth_parent_ntypes = CTF_MAX_PTYPE_V1;
      break;

      /* v2 and v3 are currently just the same as v4 except for new types and
	 sections: no upgrading required.

	 UPTODO: this is really going to change.  */
    case CTF_VERSION_2: ;
    case CTF_VERSION_3: ;
      fp->ctf_header->cth_parent_ntypes = CTF_MAX_PTYPE;
      /* FALLTHRU */
    }
  return 0;
}


/* Flip the endianness of the v3 type section, a tagged array of ctf_type or
   ctf_stype followed by variable data.  */

static int
flip_types_v3 (ctf_dict_t *fp, void *start, size_t len, int to_foreign)
{
  ctf_type_t *t = start;

  while ((uintptr_t) t < ((uintptr_t) start) + len)
    {
      ctf_kind_t kind;
      size_t size;
      uint32_t vlen;
      size_t vbytes;

      if (to_foreign)
	{
	  kind = CTF_V2_INFO_KIND (t->ctt_info);
	  size = t->ctt_size;
	  vlen = CTF_V2_INFO_VLEN (t->ctt_info);
	  vbytes = get_vbytes_v2 (fp, kind, size, vlen);
	}

      swap_thing (t->ctt_name);
      swap_thing (t->ctt_info);
      swap_thing (t->ctt_size);

      if (!to_foreign)
	{
	  kind = CTF_V2_INFO_KIND (t->ctt_info);
	  size = t->ctt_size;
	  vlen = CTF_V2_INFO_VLEN (t->ctt_info);
	  vbytes = get_vbytes_v2 (fp, kind, size, vlen);
	}

      if (_libctf_unlikely_ (size == CTF_LSIZE_SENT))
	{
	  if (to_foreign)
	    size = CTF_TYPE_LSIZE (t);

	  swap_thing (t->ctt_lsizehi);
	  swap_thing (t->ctt_lsizelo);

	  if (!to_foreign)
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
	    ssize_t i;

	    for (i = vlen; i > 0; item++, i--)
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
		ssize_t i;
		for (i = vlen; i > 0; i--, lm++)
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
		ssize_t i;
		for (i = vlen; i > 0; i--, m++)
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
	    ssize_t i;

	    for (i = vlen; i > 0; item++, i--)
	      {
		swap_thing (item->cte_name);
		swap_thing (item->cte_value);
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
}

/* Flip the endianness of the v3 variable section, an array of ctf_varent_t.  */

static void
flip_vars_v3 (void *start, size_t len)
{
  ctf_varent_t *var = start;
  ssize_t i;

  for (i = len / sizeof (struct ctf_varent); i > 0; var++, i--)
    {
      swap_thing (var->ctv_name);
      swap_thing (var->ctv_type);
    }
}

