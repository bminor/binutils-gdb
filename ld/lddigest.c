/* Linker command language support.
   Copyright (C) 1991-2023 Ulf Samuelsson <ulf@emagii.com>

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#define	DEBUG_CRC	0

#include "sysdep.h"
#include "bfd.h"
#include "safe-ctype.h"
#include "obstack.h"
#include "bfdlink.h"
#include "ctf-api.h"

#include "ld.h"
#include "ldmain.h"
#include "ldexp.h"
#include "ldlang.h"
#include <ldgram.h>
#include "ldlex.h"
#include "ldmisc.h"
#include "lddigest.h"

/* CRC calculation on output section */
asection *text_section;
unsigned char *text_contents = NULL;

char *CRC_ADDRESS = NULL;
char *CRC_START = NULL;
char *CRC_END = NULL;
char *CRC_TABLE = NULL;





const char *digest_section = ".text";
const char *digest_label = "___CRC_LABEL__";
bool digest_big_endian = false;
bool polynome_valid = false;

static bool
big_endian_host (void)
{
  union
  {
    uint32_t i;
    char c[4];
  } e = { 0x01000000 };

  return e.c[0];
}

#if     0
static bool
swap_endian (void)
{
  if (big_endian_host ())
    {
      return !link_info.big_endian;
    }
  else
    {
      return link_info.big_endian;
    }
}
#endif

/* ============ CRC-32 public functions ======================================*/

void
lang_add_crc32_syndrome (algorithm_desc_t * a)
{
  CRC_ADDRESS = CRC32_ADDRESS;
  CRC_START = CRC32_START;
  CRC_END = CRC32_END;
  CRC_TABLE = CRC32_TABLE;

  lang_add_data (LONG, exp_intop (0));	/* Reserve room for the ECC value */
  a->crc_tab = init_crc32_tab (a);
  if (a->crc_tab == NULL)
    {
      einfo (_("%F%P: can not allocate memory for CRC table: %E\n"));
      return;
    }
}

static void
lang_add_crc32_table (bool big_endian)
{
  uint32_t *crc32_table = algorithm.crc_tab;	/* Use a precomputed, if it exists */
  bool local_table = false;
  if (crc32_table == NULL)
    {				/* No luck, create a table */
      crc32_table = init_crc32_tab (&algorithm);
      if (crc32_table == NULL)
	{
	  einfo (_("%F%P: can not allocate memory for CRC table: %E\n"));
	  return;
	}
      local_table = true;
    }
  for (bfd_vma i = 0; i < 256; i++)
    {
      uint32_t elem = crc32_table[i];
      if (big_endian)
	{
	  elem = __builtin_bswap32 (elem);
	}
      lang_add_data (LONG, exp_intop (elem));
    }
  if (local_table)
    free (crc32_table);
}

/* ============ CRC-64 public functions ======================================*/

void
lang_add_crc64_syndrome (algorithm_desc_t * a)
{
  CRC_ADDRESS = CRC64_ADDRESS;
  CRC_START = CRC64_START;
  CRC_END = CRC64_END;
  CRC_TABLE = CRC64_TABLE;
  lang_add_data (QUAD, exp_intop (0));	/* Reserve room for the ECC value */
  a->crc_tab = init_crc64_tab (a);
  if (a->crc_tab == NULL)
    {
      einfo (_("%F%P: can not allocate memory for CRC table: %E\n"));
      return;
    }
}

#if (DEBUG_CRC == 1)
static void
print_hash64_table (algorithm_desc_t * a)
{
  uint64_t *crc_tab = a->crc_tab;
  uint32_t i;
  if (crc_tab == NULL)
    {
      printf ("%-20ssBad Table\n", a->name);
      return;

    }

  i = 0;
  printf ("%03d\t", i);
  printf ("0x%016lx, ", crc_tab[i + 0]);
  printf ("0x%016lx, ", crc_tab[i + 1]);
  printf ("0x%016lx, ", crc_tab[i + 2]);
  printf ("0x%016lx\n", crc_tab[i + 3]);
  printf ("\t...\n");
  i = 252;
  printf ("%03d\t", i);
  printf ("0x%016lx, ", crc_tab[i + 0]);
  printf ("0x%016lx, ", crc_tab[i + 1]);
  printf ("0x%016lx, ", crc_tab[i + 2]);
  printf ("0x%016lx\n", crc_tab[i + 3]);
  printf ("\n");
}
#else
#define print_hash64_table(x)
#endif

static void
lang_add_crc64_table (bool big_endian)
{
  bfd_vma *crc64_table = algorithm.crc_tab;	/* Use a precomputed, if it exists */
  bool local_table = false;
  if (crc64_table == NULL)
    {				/* No luck, create a table */
      crc64_table = init_crc64_tab (&algorithm);
      if (crc64_table == NULL)
	{
	  einfo (_("%F%P: can not allocate memory for CRC table: %E\n"));
	  return;
	}
      local_table = true;
    }
  print_hash64_table (&algorithm);
  for (bfd_vma i = 0; i < 256; i++)
    {
      bfd_vma elem = crc64_table[i];
      if (big_endian)
	{
	  elem = __builtin_bswap64 (elem);
	}
      if (link_info.big_endian)
	{
	  elem = __builtin_bswap64 (elem);
	}

      lang_add_data (QUAD, exp_intop (elem));
    }
  if (local_table)
    free (crc64_table);
}

/* ============ DIGEST COMMON functions ======================================*/

void
lang_add_digest (bfd_vma size,
		 bfd_vma poly,
		 bfd_vma initial,
		 bfd_vma xor_val,
		 bfd_vma ireflect, bfd_vma oreflect, bfd_vma reciprocal)
{
  if (algorithm.crc_algo != no_algo)	/* We only allow one CRC <polynom> */
    {
      einfo (_("%X%P: Duplicate digest \"%s\"\n"), "in POLY command");
      return;
    }

  algorithm.name = "CUSTOM";
  algorithm.big_endian = digest_big_endian;
  if (size == 64)
    {
      algorithm.crc_algo = crc_algo_64;
      algorithm.crc_size = size;
      algorithm.poly.d64 = poly;	/* Set the polynom */
      algorithm.initial.d64 = initial;	/* Set seed */
      algorithm.xor_val.d64 = xor_val;	/* final XOR value */
      algorithm.ireflect = ireflect;
      algorithm.oreflect = oreflect;
      algorithm.crc_tab = NULL;
      algorithm.reciprocal = reciprocal;
      algorithm.expected.d64 = 0;

      lang_add_crc64_syndrome (&algorithm);
    }
  else if (size == 32)
    {
      algorithm.crc_algo = crc_algo_32;
      algorithm.crc_size = size;
      algorithm.poly.d32._0 = poly;	/* Set the polynom */
      algorithm.initial.d32._0 = initial;	/* Set seed */
      algorithm.xor_val.d32._0 = xor_val;	/* final XOR value */
      algorithm.ireflect = ireflect;
      algorithm.oreflect = oreflect;
      algorithm.crc_tab = NULL;
      algorithm.reciprocal = reciprocal;
      algorithm.expected.d32._0 = 0;
      lang_add_crc32_syndrome (&algorithm);
    }
  else
    {
      einfo (_("%F%P: Illegal Size in DIGEST: %E\n"));
      return;
    }
}

static bool
id_start_char (char c)
{
  bool OK = false;
  if (('a' <= c) && (c <= 'z'))
    OK |= true;
  if (('A' <= c) && (c <= 'Z'))
    OK |= true;
  if ('_' == c)
    OK |= true;
  if ('-' == c)
    OK |= true;
  if ('.' == c)
    OK |= true;
  return OK;
}

static bool
id_char (char c)
{
  bool OK = false;
  if (('0' <= c) && (c <= '9'))
    OK |= true;
  OK |= id_start_char (c);
  return OK;
}

const char *
lang_get_label (const char *label, bool *big_endian)
{
  char *newlabel;
  const char *p = &label[1];
  char c;

  *big_endian = false;		/* unless told otherwise */
  if (command_line.endian == ENDIAN_BIG)
    {
      *big_endian = true;
    }
  if (command_line.endian == ENDIAN_LITTLE)
    {
      *big_endian = false;
    }
  c = *label;

  if (!id_start_char (c))
    {
      einfo (_("%X%P: Illegal label \"%s\" in digest command\n"), label);
      return "__CRC_LABEL__";
    }

  for (uint32_t i = 1; *p; i++)	/* ignore '.' in first character */
    {
      c = *p;
      if (c == '#')
	{
	  bool bad_endian = false;
	  newlabel = strndup (label, i);	/* Memory leak */
	  p++;
	  if (strlen (p) == 2)
	    {
	      char c0 = *p++;
	      char c1 = *p;
	      bool be = ((c0 == 'B') || (c0 == 'b')) &
		((c1 == 'E') || (c1 == 'e'));
	      bool le = ((c0 == 'L') || (c0 == 'l')) &
		((c1 == 'E') || (c1 == 'e'));
	      if (be)
		{
		  *big_endian = true;
		}
	      else if (le)
		{
		  *big_endian = false;
		}
	      else
		{
		  bad_endian = true;
		}
	    }
	  else
	    {
	      bad_endian = true;
	    }
	  if (bad_endian)
	    {
	      einfo (_("%X%P: bad 'endian' \"%s\" in digest label\n"), label);
	    }
	  return newlabel;
	}
      else if (!id_char (c))
	{
	  einfo (_("%X%P: Illegal label \"%s\" in digest command\n"), label);
	  return "__CRC_LABEL__";
	}
      else
	{
	  p++;
	}
    }
  return label;
}

bool
lang_set_digest (char *digest)
{
  if (algorithm.crc_algo != no_algo)	/* We only allow one CRC <polynom> */
    {
      einfo (_("%X%P: Duplicate digest \"%s\"\n"), digest);
      return false;
    }

  for (poly_t a = (poly_t) 0; a < MAXALGO; a++)
    {
#if (DEBUG_CRC == 1)
      printf ("Comparing \"%s\" with \"%s\": ", digest, algorithms[a].name);
#endif
      if (!strcmp (digest, algorithms[a].name))
	{
#if (DEBUG_CRC == 1)
	  printf ("OK\n");
#endif
	  memcpy (&algorithm, &algorithms[a], sizeof (algorithm_desc_t));
	  algorithm.big_endian = digest_big_endian;
	  if (algorithm.crc_size == 64)
	    {
	      lang_add_crc64_syndrome (&algorithm);
	    }
	  else if (algorithm.crc_size == 32)
	    {
	      lang_add_crc32_syndrome (&algorithm);
	    }

	  return true;
	}
#if (DEBUG_CRC == 1)
      else
	{
	  printf ("FAIL\n");
	}
#endif
    }
  einfo (_("%X%P: Unknown digest \"%s\"\n"), digest);
  return false;
}

void
lang_add_digest_table (bool big_endian)
{
  if (algorithm.crc_algo == crc_algo_32)
    {
      lang_add_crc32_table (big_endian);
    }
  else if (algorithm.crc_algo == crc_algo_64)
    {
      lang_add_crc64_table (big_endian);
    }
}

/* ============ CRC DEBUG functions ==========================================*/

#if (DEBUG_CRC == 1)
static void
debug_hex (char *prompt, unsigned char *section, bfd_vma address, bfd_vma sz)
{
  bfd_vma *vma_section = (bfd_vma *) section;
  bfd_vma size = (sz) / sizeof (bfd_vma);
  printf ("%s:\n", prompt);
  for (bfd_vma i = 0; i < size; i += 8)
    {
      printf ("0x%08lx: 0x%016lx 0x%016lx 0x%016lx 0x%016lx\n",
	      address + (i * sizeof (bfd_vma)),
	      vma_section[i + 0],
	      vma_section[i + 1], vma_section[i + 2], vma_section[i + 3]);
    }
}

static void
debug_crc_header (char *prompt)
{
  debug_hex (prompt, text_contents, text_section->vma, 64);
}

static void
debug_crc_update (bfd_vma crc, bfd_vma crc_addr)
{
  printf ("CRC [0x%016lx] update at 0x%08lx succeeded\n", crc, crc_addr);
}

static bool get_text_section_contents (void);
static void
debug_full_textsection (void)
{

  /* In order to see the updated content, we have to fetch it again */

  if (!get_text_section_contents ())
    {
      debug_crc_header ("After CRC");
      debug_hex ("Full Section After CRC",
		 text_contents, text_section->vma, text_section->size);
      free (text_contents);
    }
}


static void
print_crc64_algorithm (algorithm_desc_t * a, const unsigned char *crc_data,
		       uint64_t crc)
{
  printf ("64:%-16s, data=\"%s\", ", a->name, crc_data);
  printf ("poly=0x%016lx i=0x%1x xor=0x%1x .ir=%u .or=%u .rp=%u ",
	  a->poly.d64,
	  a->initial.d32._0 & 0xF,
	  a->xor_val.d32._0 & 0xF, a->ireflect, a->oreflect, a->reciprocal);
  printf ("checksum=0x%016lx, expected=0x%016lx\n", crc, a->expected.d64);
}

static void
print_section (asection * a)
{
  uint32_t len = strlen (a->name);
  char *name = malloc (len + 4);
  char *p = name;
  if (p != NULL)
    {
      *p++ = '"';
      for (uint32_t i = 0; i < len; i++)
	{
	  *p++ = a->name[i];
	}
      *p++ = '"';
      *p++ = ':';
      *p++ = '\0';
    }
  else
    {
      p = "\"?\"";
    }
  printf ("%-20s [0x%08lx .. 0x%08lx]\n", name, a->lma, a->lma + a->size - 1);
  free (name);
}

static void
print_section_list (void)
{
  bfd *list = link_info.output_bfd;
  asection *elem;
  if (strcmp (digest_section, ".foo"))
    return;

  for (elem = list->sections; elem != NULL; elem = elem->next)
    {
      if (elem->name != NULL)
	{
	  print_section (elem);
	}
    }
}

#else
#define	debug_hex(p,s,a,sz)
#define debug_crc_header(p)
#define debug_crc_update(c, a)
#define debug_full_textsection()
#define print_section(a)
#define print_section_list()
#endif

/* ============ Access functions for inserting checksum in text section=======*/

static bool
get_section_by_address (bfd_vma addr)
{
  bfd *list = link_info.output_bfd;
  for (asection * elem = list->sections; elem != NULL; elem = elem->next)
    {
      bfd_vma lma = elem->lma;
      bfd_vma end = lma + elem->size;

      if ((addr >= lma) && (addr < end))
	{
	  digest_section = elem->name;
	  return true;
	}
    }
  return false;
}

static bool
get_text_section_contents (void)
{
  /*
   * Get the '.text' section
   * Is there a risk that CRC needs to be calculated on more than .text?
   * We do not support that...
   */
  print_section_list ();

  text_section =
    bfd_get_section_by_name (link_info.output_bfd, digest_section);
  if (text_section == NULL)
    {
      einfo (_("%X%P: cannot retrieve '%s' section for CRC calculation\n"),
	     digest_section);
      return false;
    }

  /* Get the contents of the '.text' area so we can calculate the CRC */
  if (!bfd_malloc_and_get_section (link_info.output_bfd,
				   text_section->output_section,
				   (bfd_byte **) & text_contents))
    {
      einfo (_("%X%P: '&s' section contents unavailable\n"
	       "CRC generation aborted\n"), digest_section);
      return false;
    }

#if (DEBUG_CRC == 1)
  print_section (text_section);
/*
  printf ("%s: [0x%08lx .. 0x%08lx]\n",
	  text_section->name,
	  text_section->lma, text_section->lma + text_section->size - 1);
 */
#endif
  return true;
}

/* Set variable in the '.text' area */
static bool
set_crc32_checksum (uint32_t crc, bfd_vma addr)
{
  uint32_t real_crc = crc;
  if (big_endian_host ())
    {
      if (algorithm.big_endian)
	{
	  /* We are OK */
	}
      else
	{
	  real_crc = __builtin_bswap32 (crc);
	}
    }
  else
    {
      if (algorithm.big_endian)
	{
	  real_crc = __builtin_bswap32 (crc);
	}
      else
	{
	  /* We are OK */
	}
    }

  return (bfd_set_section_contents (link_info.output_bfd,
				    text_section->output_section,
				    &real_crc, addr, sizeof (uint32_t)));
}

static bool
set_crc64_checksum (uint64_t crc, bfd_vma addr)
{
  uint64_t real_crc = crc;
  if (big_endian_host ())
    {
      if (algorithm.big_endian)
	{
	  /* We are OK */
	}
      else
	{
	  real_crc = __builtin_bswap64 (crc);
	}
    }
  else
    {
      if (algorithm.big_endian)
	{
	  real_crc = __builtin_bswap64 (crc);
	}
      else
	{
	  /* We are OK */
	}
    }
  /* True if OK */
  return (bfd_set_section_contents (link_info.output_bfd,
				    text_section->output_section,
				    &real_crc, addr, sizeof (uint64_t)));
}

static bool
set_crc_checksum (bfd_vma crc, bfd_vma addr, bfd_vma size)
{
  bool status;
  if (size == 64)
    {
      status = set_crc64_checksum (crc, addr);
    }
  else
    {
      status = set_crc32_checksum ((uint32_t) crc, addr);
    }
  return status;
}

static bool
symbol_lookup (char *name, bfd_vma * val)
{
  struct bfd_link_hash_entry *h;
  *val = 0ull;
  h = bfd_link_hash_lookup (link_info.hash, name, false, false, true);
  if (h != NULL)
    {
      if (((h->type == bfd_link_hash_defined) ||
	   (h->type == bfd_link_hash_defweak)) &&
	  (h->u.def.section->output_section != NULL))
	{
	  *val = (h->u.def.value
		  + bfd_section_vma (h->u.def.section->output_section)
		  + h->u.def.section->output_offset);
	  return true;
	}
    }
  return false;
}

/* ============ CRC common functions =========================================*/
/*
 * Multiplexing function for calculating CRC with different algorithms
 * 'algorithm.crc_algo'
 */
static bfd_vma
calculate_crc (const unsigned char *input_str, size_t num_bytes)
{
  if (algorithm.crc_algo == crc_algo_64)
    {
      if (algorithm.crc_tab != NULL)
	{
	  return calc_crc64 (&algorithm, input_str, num_bytes);
	}
    }
  else if (algorithm.crc_algo == crc_algo_32)
    {
      if (algorithm.crc_tab != NULL)
	{
	  return calc_crc32 (&algorithm, input_str, num_bytes);
	}
    }
  /* This should never happen */
  return 0;
}

static bool
invalid_crc_parameters (bfd_vma crc_addr,
			bfd_vma crc_area_start, bfd_vma crc_area_end)
{
  bool crc_in_section;
  bfd_vma crc_size = algorithm.crc_size / 8;
  /* Get limits of '.text' section */
  bfd_vma text_start = text_section->lma;
  bfd_vma text_end = text_section->lma + text_section->size;

  /* All three symbols must be inside the '.text' section */
  crc_in_section =
    ((crc_addr >= text_start) && ((crc_addr + crc_size) <= text_end)) &&
    ((crc_area_start >= text_start) && (crc_area_start <= text_end)) &&
    ((crc_area_end >= text_start) && (crc_area_end <= text_end));

  if (!crc_in_section)
    {
      einfo (_("%X%P: The CRC digest and table should be inside the '%s' "
	       "section\n"), digest_section);
      /*
       * Maybe we should printout the text section start and end
       * as well as the boundaries of the CRC check area.
       */
      return true;
    }

  /*
   * CRC checksum must be outside the checked area
   * We must check that they do not overlap in the beginning
   */
  {
    bool crc_valid = false;
    if ((crc_addr + crc_size) < crc_area_start)
      {
	crc_valid = true;
      }
    else if (crc_addr >= crc_area_end)
      {
	crc_valid = true;
      }
    if (!crc_valid)
      {
	einfo (_("%X%P: CRC located inside checked area\n"), NULL);
	return true;
      }
  }

  if (crc_area_start > crc_area_end)
    {
      einfo (_("%X%P: CRC area starts after its end location\n"), NULL);
      return true;
    }

  return false;
}

void
lang_generate_crc (void)
{
  bfd_vma crc_addr, crc_area_start, crc_area_end;
  bfd_vma crc;
  bool can_do_crc;

  /* Return immediately, if CRC is not requested */
  if (algorithm.crc_algo == no_algo)
    return;

  /*
   * These symbols must be present, for CRC to be generated
   * They should all have been defined in a CRC## <syndrome> statement
   * If they are not defined, then there is an internal error.
   * Should we consider using symbols which cannot be parsed by the linker?
   * I.E. CRC-64 is never an identifier
   */
  can_do_crc = symbol_lookup (CRC_ADDRESS, &crc_addr) &&
    symbol_lookup (CRC_START, &crc_area_start) &&
    symbol_lookup (CRC_END, &crc_area_end);

  if (!can_do_crc)
    {
      einfo (_("%X%P: Internal Error - __CRC#___ symbols not defined\n"),
	     NULL);
      return;
    }

  if (!get_section_by_address (crc_addr))	/* update digest_section, if needed */
    {
      einfo (_("%X%P: The CRC digest and table must be inside the '%s' "
	       "section\n"), digest_section);
    }

  if (!get_text_section_contents ())
    {
      /* Error messages inside check */
      return;
    }

  /* Check that the addresses make sense */
  if (invalid_crc_parameters (crc_addr, crc_area_start, crc_area_end))
    {
      /* Error messages inside check */
      return;
    }

  /* Calculate and set the CRC */
  {
    /*
     * The '.text' area does not neccessarily start at 0x00000000,
     * so we have to shift the indices.
     */
    bfd_vma _crc_addr = crc_addr - text_section->vma;
    bfd_vma _crc_area_start = crc_area_start - text_section->vma;
    bfd_vma _crc_area_end = crc_area_end - text_section->vma;


    /* This is the CRC calculation which we worked so hard for */
    debug_crc_header ("Before CRC");

    print_hash64_table (&algorithm);

#if (DEBUG_CRC == 1)
    crc = calculate_crc ((const unsigned char *) "123456789", 9);
#else
    crc = calculate_crc (&text_contents[_crc_area_start],
			 _crc_area_end - _crc_area_start);
#endif

#if (DEBUG_CRC == 1)
    printf ("size  = 0x%08u\n", (uint32_t) (_crc_area_end - _crc_area_start));
    printf ("start = 0x%016lx\n", _crc_area_start);
    printf ("end   = 0x%016lx\n", _crc_area_end);
    printf ("crc   = 0x%016lx\n", crc);

    print_crc64_algorithm (&algorithm, &text_contents[_crc_area_start], crc);
#endif

    /*
     * The contents of the '.text' section are no longer needed.
     * It is a copy of the section contents, and will therefore be stale
     * after we updated the section with the CRC checksum.
     * Remove it before we set the CRC checksum, to simplify the code logic.
     */
    free (text_contents);
    if (set_crc_checksum (crc, _crc_addr, algorithm.crc_size))
      {
	debug_crc_update (crc, crc_addr);
      }
    else
      {
	einfo (_("%X%P: updating CRC in section '%s' failed\n"),
	       digest_section);
      }
  }

  debug_full_textsection ();
}

/* ============ END CRC common functions =====================================*/

void
lang_generate_digest (void)
{
  /* Return immediately, if CRC is not requested */
#if (DEBUG_CRC == 1)
  printf ("%s: BEGIN\n", __FUNCTION__);
#endif
  if (algorithm.crc_algo == no_algo)
    return;

  /* Handle 32/64-bit CRCs */
  if ((algorithm.crc_algo == crc_algo_32)
      || (algorithm.crc_algo == crc_algo_64))
    {
      lang_generate_crc ();
    }
#if (DEBUG_CRC == 1)
  printf ("%s: END\n", __FUNCTION__);
#endif
}
