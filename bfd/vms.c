/* vms.c -- BFD back-end for VAX (openVMS/VAX) and
   EVAX (openVMS/Alpha) files.
   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2006, 2007, 2008, 2009 Free Software Foundation, Inc.

   Main file.

   Written by Klaus K"ampf (kkaempf@rmi.de)

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

#ifdef VMS
#include <rms.h>
#include <unixlib.h>
#include <starlet.h>
#define RME$C_SETRFM 0x00000001
#include <unistd.h>
#endif

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"

#include "vms.h"

static bfd_boolean vms_initialize (bfd *);
static bfd_boolean fill_section_ptr (struct bfd_hash_entry *, PTR);
static bfd_boolean vms_fixup_sections (bfd *);
static bfd_boolean copy_symbols (struct bfd_hash_entry *, PTR);
static bfd_reloc_status_type reloc_nil (bfd *, arelent *, asymbol *, PTR,
					asection *, bfd *, char **);
static int vms_slurp_module (bfd *abfd);
static int vms_slurp_image (bfd *abfd);
static const struct bfd_target *vms_object_p (bfd *abfd);
static bfd_boolean vms_mkobject (bfd *abfd);
static bfd_boolean vms_write_object_contents (bfd *abfd);
static void free_reloc_stream (bfd *abfd, asection *section, void *data);
static bfd_boolean vms_close_and_cleanup (bfd *abfd);
static bfd_boolean vms_new_section_hook (bfd *abfd, asection *section);
static bfd_boolean vms_get_section_contents
  (bfd *abfd, asection *section, PTR x1, file_ptr x2, bfd_size_type x3);
static long vms_get_symtab_upper_bound (bfd *abfd);
static long vms_canonicalize_symtab (bfd *abfd, asymbol **symbols);
static void vms_print_symbol (bfd *abfd, PTR file, asymbol *symbol,
			      bfd_print_symbol_type how);
static void vms_get_symbol_info (bfd *abfd, asymbol *symbol, symbol_info *ret);
static bfd_boolean vms_bfd_is_local_label_name (bfd *abfd, const char *);
static bfd_boolean vms_find_nearest_line
  (bfd *abfd, asection *section, asymbol **symbols, bfd_vma offset,
   const char **file, const char **func, unsigned int *line);
static void alloc_reloc_stream (bfd *abfd, asection *section,
				void *alloc_error);
static bfd_boolean vms_slurp_reloc_table (bfd *abfd, asection *section,
					  asymbol **symbols);
static long vms_get_reloc_upper_bound (bfd *abfd, asection *sect);
static long vms_canonicalize_reloc (bfd *abfd, asection *srcsec,
				    arelent **location, asymbol **symbols);
static const struct reloc_howto_struct *vms_bfd_reloc_type_lookup
  (bfd *abfd, bfd_reloc_code_real_type code);
static bfd_boolean vms_set_arch_mach
  (bfd *abfd, enum bfd_architecture arch, unsigned long mach);
static bfd_boolean vms_set_section_contents
  (bfd *abfd, asection *section, const PTR location, file_ptr offset,
   bfd_size_type count);

#define vms_bfd_is_target_special_symbol ((bfd_boolean (*) (bfd *, asymbol *)) bfd_false)
#define vms_make_empty_symbol             _bfd_generic_make_empty_symbol
#define vms_bfd_link_just_syms            _bfd_generic_link_just_syms
#define vms_bfd_copy_link_hash_symbol_type \
  _bfd_generic_copy_link_hash_symbol_type
#define vms_bfd_is_group_section          bfd_generic_is_group_section
#define vms_bfd_discard_group             bfd_generic_discard_group
#define vms_section_already_linked        _bfd_generic_section_already_linked
#define vms_bfd_define_common_symbol      bfd_generic_define_common_symbol
#define vms_bfd_copy_private_header_data  _bfd_generic_bfd_copy_private_header_data
#define vms_get_synthetic_symtab          _bfd_nodynamic_get_synthetic_symtab

#define vms_bfd_copy_private_bfd_data	  _bfd_generic_bfd_copy_private_bfd_data
#define vms_bfd_print_private_bfd_data	  _bfd_generic_bfd_print_private_bfd_data
#define vms_bfd_free_cached_info	  _bfd_generic_bfd_free_cached_info
#define vms_bfd_copy_private_section_data _bfd_generic_bfd_copy_private_section_data
#define vms_bfd_copy_private_symbol_data  _bfd_generic_bfd_copy_private_symbol_data
#define vms_bfd_set_private_flags         _bfd_generic_bfd_set_private_flags
#define vms_bfd_merge_private_bfd_data    _bfd_generic_bfd_merge_private_bfd_data
#define vms_get_section_contents_in_window _bfd_generic_get_section_contents_in_window
#define vms_read_minisymbols               _bfd_generic_read_minisymbols
#define vms_minisymbol_to_symbol           _bfd_generic_minisymbol_to_symbol
#define vms_get_lineno                     _bfd_nosymbols_get_lineno
#define vms_find_inliner_info              _bfd_nosymbols_find_inliner_info
#define vms_bfd_make_debug_symbol          _bfd_nosymbols_bfd_make_debug_symbol

#ifdef VMS_DEBUG
/* Cause debug info to be emitted for the structure.  */
struct vms_private_data_struct _vms_private_data_struct_dummy;
struct vms_section_data_struct _vms_section_data_struct_dummy;
#endif

extern const bfd_target vms_vax_vec;
extern const bfd_target vms_alpha_vec;

/* Initialize private data  */
static bfd_boolean
vms_initialize (bfd * abfd)
{
  bfd_size_type amt;

  bfd_set_start_address (abfd, (bfd_vma) -1);

  amt = sizeof (struct vms_private_data_struct);
  abfd->tdata.any = bfd_zalloc (abfd, amt);
  if (abfd->tdata.any == NULL)
    return FALSE;

  if (bfd_get_flavour (abfd) == bfd_target_ovax_flavour)
    PRIV (is_vax) = TRUE;

  PRIV (file_format) = FF_UNKNOWN;

  amt = sizeof (struct stack_struct) * STACKSIZE;
  PRIV (stack) = bfd_alloc (abfd, amt);
  if (PRIV (stack) == NULL)
    goto error_ret1;

  amt = sizeof (struct bfd_hash_table);
  PRIV (vms_symbol_table) = bfd_alloc (abfd, amt);
  if (PRIV (vms_symbol_table) == NULL)
    goto error_ret1;

  if (!bfd_hash_table_init (PRIV (vms_symbol_table), _bfd_vms_hash_newfunc,
			    sizeof (vms_symbol_entry)))
    goto error_ret1;

  amt = MAX_OUTREC_SIZE;
  PRIV (output_buf) = bfd_alloc (abfd, amt);
  if (PRIV (output_buf) == NULL)
    goto error_ret2;

  PRIV (length_pos) = 2;

  return TRUE;

 error_ret2:
  bfd_hash_table_free (PRIV (vms_symbol_table));
 error_ret1:
  bfd_release (abfd, abfd->tdata.any);
  abfd->tdata.any = NULL;
  return FALSE;
}

struct pair
{
  unsigned int section_count;
  asection **sections;
};

/* Fill symbol->section with section pointer.

   symbol->section is filled with the section index for defined symbols
   during reading the GSD/EGSD section.  But we need the pointer to the
   bfd section later.

   It has the correct value for referenced (undefined section) symbols.

   Called from bfd_hash_traverse in vms_fixup_sections.  */

static bfd_boolean
fill_section_ptr (struct bfd_hash_entry *entry, void *sections)
{
  asymbol *sym = ((vms_symbol_entry *)entry)->symbol;
  struct pair *data = (struct pair *)sections;
  unsigned long sec = (unsigned long)sym->section;

  vms_debug2 ((6, "fill_section_ptr: sym %p, sec %lu\n", sym, sec));

  if (sec < data->section_count)
    {
      sym->section = data->sections[sec];

      if (strcmp (sym->name, sym->section->name) == 0)
	sym->flags |= BSF_SECTION_SYM;
    }
  else if (sec == (unsigned long)-1)
    sym->section = &bfd_und_section;
    
  return TRUE;
}

/* Fixup section pointers in symbols.  */
static bfd_boolean
vms_fixup_sections (bfd * abfd)
{
  struct pair data;

  if (PRIV (fixup_done))
    return TRUE;

  data.section_count = PRIV (section_count);
  data.sections = PRIV (sections);
  bfd_hash_traverse (PRIV (vms_symbol_table), fill_section_ptr, &data);

  PRIV (fixup_done) = TRUE;
  return TRUE;
}

/* Slurp an ordered set of VMS object records.  */
int
_bfd_vms_slurp_object_records (bfd * abfd)
{
  int err, new_type, type = -1;

  do
    {
      vms_debug2 ((7, "reading at %08lx\n", bfd_tell (abfd)));

      new_type = _bfd_vms_get_object_record (abfd);
      if (new_type < 0)
	{
	  vms_debug2 ((2, "next_record failed\n"));
	  return -1;
	}

      if (type == EOBJ_S_C_EGSD && new_type != EOBJ_S_C_EGSD)
	{
	  if (! vms_fixup_sections (abfd))
	    {
	      vms_debug2 ((2, "vms_fixup_sections failed\n"));
	      return -1;
	    }
	}

      type = new_type;

      switch (type)
	{
	  case OBJ_S_C_HDR:
	  case EOBJ_S_C_EMH:
	    err = _bfd_vms_slurp_hdr (abfd, type);
	    break;
	  case OBJ_S_C_EOM:
	  case OBJ_S_C_EOMW:
	  case EOBJ_S_C_EEOM:
	    err = _bfd_vms_slurp_eom (abfd, type);
	    break;
	  case OBJ_S_C_GSD:
	  case EOBJ_S_C_EGSD:
	    err = _bfd_vms_slurp_gsd (abfd, type);
	    break;
	  case OBJ_S_C_TIR:
	  case EOBJ_S_C_ETIR:
	    err = _bfd_vms_slurp_tir (abfd, type);
	    break;
	  case OBJ_S_C_DBG:
	  case EOBJ_S_C_EDBG:
	    err = _bfd_vms_slurp_dbg (abfd, type);
	    PRIV (dst_ptr_end) = PRIV (image_ptr);
	    break;
	  case OBJ_S_C_TBT:
	  case EOBJ_S_C_ETBT:
	    err = _bfd_vms_slurp_tbt (abfd, type);
	    PRIV (dst_ptr_end) = PRIV (image_ptr);
	    break;
	  case OBJ_S_C_LNK:
	    err = _bfd_vms_slurp_lnk (abfd, type);
	    break;
	  default:
	    err = -1;
	}
      if (err != 0)
	{
	  vms_debug2 ((2, "slurp type %d failed with %d\n", type, err));
	  return err;
	}
    }
  while (type != EOBJ_S_C_EEOM && type != OBJ_S_C_EOM && type != OBJ_S_C_EOMW);

  return 0;
}

/* Slurp a VMS module and return an error status.  */

static int
vms_slurp_module (bfd *abfd)
{
  int type, err;

  if (PRIV (is_vax))
    type = PRIV (vms_rec)[0];
  else
    type = bfd_getl16 (PRIV (vms_rec));

  err = _bfd_vms_slurp_hdr (abfd, type);
  if (err != 0)
    {
      bfd_set_error (bfd_error_wrong_format);
      return err;
    }

  return _bfd_vms_slurp_object_records (abfd);
}

/* Slurp a VMS image and return an error status.  */

static int
vms_slurp_image (bfd *abfd)
{
  unsigned int isd_offset, ihs_offset;
  int err;

  err = _bfd_vms_slurp_ihd (abfd, &isd_offset, &ihs_offset);
  if (err != 0)
    {
      bfd_set_error (bfd_error_wrong_format);
      return err;
    }

  err = _bfd_vms_slurp_isd (abfd, isd_offset);
  if (err != 0)
    {
      bfd_set_error (bfd_error_wrong_format);
      return err;
    }

  return _bfd_vms_slurp_ihs (abfd, ihs_offset);
}

/* Check the format for a file being read.
   Return a (bfd_target *) if it's an object file or zero if not.  */

static const struct bfd_target *
vms_object_p (bfd *abfd)
{
  const struct bfd_target *target_vector;
  const bfd_arch_info_type *arch;
  PTR tdata_save = abfd->tdata.any;
  bfd_vma saddr_save = bfd_get_start_address (abfd);
  int err = 0;

  vms_debug2 ((1, "vms_object_p(%p)\n", abfd));

  if (!vms_initialize (abfd))
    goto error_ret;

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET))
    goto err_wrong_format;

  switch (_bfd_vms_get_first_record (abfd))
    {
    case FT_UNKNOWN:
    default:
      err = -1;
      break;

    case FT_MODULE:
      err = vms_slurp_module (abfd);
      break;

    case FT_IMAGE:
      err = vms_slurp_image (abfd);
      break;
    }

  if (err != 0)
    goto err_wrong_format;

  if (PRIV (is_vax))
    {
      if (! vms_fixup_sections (abfd))
	{
	  vms_debug2 ((2, "vms_fixup_sections failed\n"));
	  goto err_wrong_format;
	}

      target_vector = &vms_vax_vec;
      arch = bfd_scan_arch ("vax");

      vms_debug2 ((2, "arch is vax\n"));
    }
  else
    {
      /* Set arch_info to alpha.   */
      target_vector = &vms_alpha_vec;
      arch = bfd_scan_arch ("alpha");
      vms_debug2 ((2, "arch is alpha\n"));
    }

  abfd->arch_info = arch;
  return target_vector;

 err_wrong_format:
  bfd_set_error (bfd_error_wrong_format);

 error_ret:
  if (abfd->tdata.any != tdata_save && abfd->tdata.any != NULL)
    bfd_release (abfd, abfd->tdata.any);
  abfd->tdata.any = tdata_save;
  bfd_set_start_address (abfd, saddr_save);
  return NULL;
}

/* Set the format of a file being written.  */

static bfd_boolean
vms_mkobject (bfd * abfd)
{
  const bfd_arch_info_type *arch;

  vms_debug2 ((1, "vms_mkobject (%p)\n", abfd));

  if (!vms_initialize (abfd))
    return FALSE;

  if (PRIV (is_vax))
    arch = bfd_scan_arch ("vax");
  else
    arch = bfd_scan_arch ("alpha");

  if (arch == 0)
    {
      bfd_set_error(bfd_error_wrong_format);
      return FALSE;
    }

  abfd->arch_info = arch;
  return TRUE;
}

/* Write cached information into a file being written, at bfd_close.  */

static bfd_boolean
vms_write_object_contents (bfd * abfd)
{
  vms_debug2 ((1, "vms_write_object_contents (%p)\n", abfd));

  if (abfd->section_count > 0)			/* we have sections */
    {
      if (PRIV (is_vax))
	{
	  if (_bfd_vms_write_hdr (abfd, OBJ_S_C_HDR) != 0)
	    return FALSE;
	  if (_bfd_vms_write_gsd (abfd, OBJ_S_C_GSD) != 0)
	    return FALSE;
	  if (_bfd_vms_write_tir (abfd, OBJ_S_C_TIR) != 0)
	    return FALSE;
	  if (_bfd_vms_write_tbt (abfd, OBJ_S_C_TBT) != 0)
	    return FALSE;
	  if (_bfd_vms_write_dbg (abfd, OBJ_S_C_DBG) != 0)
	    return FALSE;
	  if (abfd->section_count > 255)
	    {
	      if (_bfd_vms_write_eom (abfd, OBJ_S_C_EOMW) != 0)
		return FALSE;
	    }
	  else
	    {
	      if (_bfd_vms_write_eom (abfd, OBJ_S_C_EOM) != 0)
		return FALSE;
	    }
	}
      else
	{
	  if (_bfd_vms_write_hdr (abfd, EOBJ_S_C_EMH) != 0)
	    return FALSE;
	  if (_bfd_vms_write_gsd (abfd, EOBJ_S_C_EGSD) != 0)
	    return FALSE;
	  if (_bfd_vms_write_tir (abfd, EOBJ_S_C_ETIR) != 0)
	    return FALSE;
	  if (_bfd_vms_write_tbt (abfd, EOBJ_S_C_ETBT) != 0)
	    return FALSE;
	  if (_bfd_vms_write_dbg (abfd, EOBJ_S_C_EDBG) != 0)
	    return FALSE;
	  if (_bfd_vms_write_eom (abfd, EOBJ_S_C_EEOM) != 0)
	    return FALSE;
	}
    }
  return TRUE;
}

/* 4.1, generic.  */

/* Free the reloc buffer for the specified section.  */

static void
free_reloc_stream (bfd *abfd ATTRIBUTE_UNUSED, asection *section,
		   void *data ATTRIBUTE_UNUSED)
{
  if (vms_section_data (section)->reloc_stream)
    free (vms_section_data (section)->reloc_stream);
}

#ifdef VMS
/* Convert the file to variable record length format. This is done
   using undocumented system call sys$modify().
   Pure VMS version.  */

static void
vms_convert_to_var (char *vms_filename)
{
  struct FAB fab = cc$rms_fab;

  fab.fab$l_fna = vms_filename;
  fab.fab$b_fns = strlen (vms_filename);
  fab.fab$b_fac = FAB$M_PUT;
  fab.fab$l_fop = FAB$M_ESC;
  fab.fab$l_ctx = RME$C_SETRFM;
  
  sys$open (&fab);
  
  fab.fab$b_rfm = FAB$C_VAR;
  
  sys$modify (&fab);
  sys$close (&fab);
}

static int
vms_convert_to_var_1 (char *filename, int type)
{
  if (type != DECC$K_FILE)
    return FALSE;
  vms_convert_to_var (filename);
  return TRUE;
}

/* Convert the file to variable record length format. This is done
   using undocumented system call sys$modify().
   Unix filename version.  */

static int
vms_convert_to_var_unix_filename (const char *unix_filename)
{
  if (decc$to_vms (unix_filename, &vms_convert_to_var_1, 0, 1) != 1)
    return FALSE;
  return TRUE;
}
#endif /* VMS */

/* Called when the BFD is being closed to do any necessary cleanup.  */

static bfd_boolean
vms_close_and_cleanup (bfd * abfd)
{
  vms_debug2 ((1, "vms_close_and_cleanup (%p)\n", abfd));

  if (abfd == NULL || abfd->tdata.any == NULL)
    return TRUE;

  if (PRIV (vms_buf) != NULL)
    free (PRIV (vms_buf));

  if (PRIV (sections) != NULL)
    free (PRIV (sections));

  if (PRIV (vms_symbol_table))
    bfd_hash_table_free (PRIV (vms_symbol_table));

  bfd_map_over_sections (abfd, free_reloc_stream, NULL);

  bfd_release (abfd, abfd->tdata.any);
  abfd->tdata.any = NULL;

#ifdef VMS
  if (abfd->direction == write_direction)
    {
      /* Last step on VMS is to convert the file to variable record length
	 format.  */
      if (bfd_cache_close (abfd) != TRUE)
	return FALSE;
      if (vms_convert_to_var_unix_filename (abfd->filename) != TRUE)
	return FALSE;
    }
#endif

  return TRUE;
}

/* Called when a new section is created.  */

static bfd_boolean
vms_new_section_hook (bfd * abfd, asection *section)
{
  bfd_size_type amt;

  /* Count hasn't been incremented yet.  */
  unsigned int section_count = abfd->section_count + 1;

  vms_debug2 ((1, "vms_new_section_hook (%p, [%d]%s), count %d\n",
               abfd, section->index, section->name, section_count));

  bfd_set_section_alignment (abfd, section, 0);

  if (section_count > PRIV (section_count))
    {
      amt = section_count;
      amt *= sizeof (asection *);
      PRIV (sections) = bfd_realloc_or_free (PRIV (sections), amt);
      if (PRIV (sections) == NULL)
	return FALSE;
      PRIV (section_count) = section_count;
    }

  vms_debug2 ((6, "section_count: %d\n", PRIV (section_count)));

  PRIV (sections)[section->index] = section;

  vms_debug2 ((7, "%d: %s\n", section->index, section->name));

  amt = sizeof (struct vms_section_data_struct);
  section->used_by_bfd = (PTR) bfd_zalloc (abfd, amt);
  if (section->used_by_bfd == NULL)
    return FALSE;

  return _bfd_generic_new_section_hook (abfd, section);
}

/* Read the contents of a section.
   buf points to a buffer of buf_size bytes to be filled with
   section data (starting at offset into section)  */

static bfd_boolean
vms_get_section_contents (bfd * abfd ATTRIBUTE_UNUSED,
			  asection *section ATTRIBUTE_UNUSED,
			  void * buf ATTRIBUTE_UNUSED,
			  file_ptr offset ATTRIBUTE_UNUSED,
			  bfd_size_type buf_size ATTRIBUTE_UNUSED)
{
  bfd_size_type size = section->size;

  vms_debug2 ((1, "vms_get_section_contents (%p, %s, %p, off %ld, size %d)\n",
               abfd, section->name, buf, offset, (int)buf_size));

  if (section->contents)
    abort ();

  section->contents = (unsigned char *) bfd_malloc (size);

  if (section->contents == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return FALSE;
    }

  if (bfd_seek (abfd, section->filepos, SEEK_SET))
    {
      bfd_set_error (bfd_error_file_truncated);
      return FALSE;
    }

  if (bfd_bread (section->contents, size, abfd) != size)
    {
      bfd_set_error (bfd_error_file_truncated);
      return FALSE;
    }

  section->flags |= SEC_IN_MEMORY;

  if (buf)
    memcpy (buf, section->contents + offset, (size_t) buf_size);

  return TRUE;
}

/* Part 4.5, symbols.  */

/* Return the number of bytes required to store a vector of pointers
   to asymbols for all the symbols in the BFD abfd, including a
   terminal NULL pointer. If there are no symbols in the BFD,
   then return 0.  If an error occurs, return -1.  */

static long
vms_get_symtab_upper_bound (bfd * abfd)
{
  vms_debug2 ((1, "vms_get_symtab_upper_bound (%p), %d symbols\n",
               abfd, PRIV (gsd_sym_count)));

  return (PRIV (gsd_sym_count) + 1) * sizeof (asymbol *);
}

/* Copy symbols from hash table to symbol vector

   called from bfd_hash_traverse in vms_canonicalize_symtab
   init counter to 0 if entry == 0.  */

static bfd_boolean
copy_symbols (struct bfd_hash_entry *entry, void * arg)
{
  bfd * abfd = (bfd *) arg;

  if (entry == NULL)	/* Init counter.  */
    PRIV (symnum) = 0;
  else			/* Fill vector, inc counter.  */
    PRIV (symcache)[PRIV (symnum)++] = ((vms_symbol_entry *)entry)->symbol;

  return TRUE;
}

/* Read the symbols from the BFD abfd, and fills in the vector
   location with pointers to the symbols and a trailing NULL.

   Return number of symbols read.   */

static long
vms_canonicalize_symtab (bfd * abfd, asymbol **symbols)
{
  vms_debug2 ((1, "vms_canonicalize_symtab (%p, <ret>)\n", abfd));

  /* Init counter.  */
  copy_symbols (NULL, abfd);

  /* Traverse table and fill symbols vector.  */
  PRIV (symcache) = symbols;
  bfd_hash_traverse (PRIV (vms_symbol_table), copy_symbols, abfd);

  symbols[PRIV (gsd_sym_count)] = NULL;

  return PRIV (gsd_sym_count);
}

/* Print symbol to file according to how. how is one of
   bfd_print_symbol_name	just print the name
   bfd_print_symbol_more	print more (???)
   bfd_print_symbol_all	print all we know, which is not much right now :-).  */

static void
vms_print_symbol (bfd * abfd,
		  void * file,
		  asymbol *symbol,
		  bfd_print_symbol_type how)
{
  vms_debug2 ((1, "vms_print_symbol (%p, %p, %p, %d)\n",
               abfd, file, symbol, how));

  switch (how)
    {
      case bfd_print_symbol_name:
      case bfd_print_symbol_more:
	fprintf ((FILE *)file," %s", symbol->name);
      break;

      case bfd_print_symbol_all:
	{
	  const char *section_name = symbol->section->name;

	  bfd_print_symbol_vandf (abfd, file, symbol);

	  fprintf ((FILE *) file," %-8s %s", section_name, symbol->name);
        }
      break;
    }
}

/* Return information about symbol in ret.

   fill type, value and name
   type:
	A	absolute
	B	bss segment symbol
	C	common symbol
	D	data segment symbol
	f	filename
	t	a static function symbol
	T	text segment symbol
	U	undefined
	-	debug.  */

static void
vms_get_symbol_info (bfd * abfd ATTRIBUTE_UNUSED,
		     asymbol *symbol,
		     symbol_info *ret)
{
  asection *sec;

  vms_debug2 ((1, "vms_get_symbol_info (%p, %p, %p)\n", abfd, symbol, ret));

  sec = symbol->section;

  if (ret == NULL)
    return;

  if (sec == 0)
    ret->type = 'U';
  else if (bfd_is_com_section (sec))
    ret->type = 'C';
  else if (bfd_is_abs_section (sec))
    ret->type = 'A';
  else if (bfd_is_und_section (sec))
    ret->type = 'U';
  else if (bfd_is_ind_section (sec))
    ret->type = 'I';
  else if (bfd_get_section_flags (abfd, sec) & SEC_CODE)
    ret->type = 'T';
  else if (bfd_get_section_flags (abfd, sec) & SEC_DATA)
    ret->type = 'D';
  else if (bfd_get_section_flags (abfd, sec) & SEC_ALLOC)
    ret->type = 'B';
  else
    ret->type = '-';

  if (ret->type != 'U')
    ret->value = symbol->value + symbol->section->vma;
  else
    ret->value = 0;
  ret->name = symbol->name;
}

/* Return TRUE if the given symbol sym in the BFD abfd is
   a compiler generated local label, else return FALSE.  */

static bfd_boolean
vms_bfd_is_local_label_name (bfd * abfd ATTRIBUTE_UNUSED,
			     const char *name)
{
  vms_debug2 ((1, "vms_bfd_is_local_label_name (%p, %s)\n", abfd, name));
  return name[0] == '$';
}

/* Provided a BFD, a section and an offset into the section, calculate and
   return the name of the source file and the line nearest to the wanted
   location.  */

static bfd_boolean
vms_find_nearest_line (bfd * abfd ATTRIBUTE_UNUSED,
		       asection *section ATTRIBUTE_UNUSED,
		       asymbol **symbols ATTRIBUTE_UNUSED,
		       bfd_vma offset ATTRIBUTE_UNUSED,
		       const char **file ATTRIBUTE_UNUSED,
		       const char **func ATTRIBUTE_UNUSED,
		       unsigned int *line ATTRIBUTE_UNUSED)
{
  vms_debug2 ((1, "vms_find_nearest_line (%p, %s, %p, %ld, ...)\n",
               abfd, section->name, symbols, (long int)offset));
  return _bfd_vms_find_nearest_dst_line (abfd, section, symbols, offset, file, func, line);
}

/* Part 4.6, relocations.  */

/* Allocate the reloc buffer for the specified section.  */

static void
alloc_reloc_stream (bfd *abfd ATTRIBUTE_UNUSED, asection *section,
		    void *alloc_error)
{
  unsigned char *ptr;

  /* If there were no relocations, there is nothing to do.  */
  if (section->reloc_count == 0)
    return;

  ptr = bfd_malloc (vms_section_data (section)->reloc_size);
  if (ptr == NULL)
    {
      *(bfd_boolean *)alloc_error = TRUE;
      return;
    }

  vms_section_data (section)->reloc_stream = ptr;
}

/* Read in the relocs for the specified section and internalize them.

   The implementation is loosely based on the SOM code and made up
   of 3 distinct phases:

   1. When the VMS object is opened and parsed, the number and the size
      of the relocations are computed for all sections.  This makes it
      possible to know upfront both which sections have no relocs and
      the size of the reloc buffers for the other sections, at virtually
      no cost for consumers that don't care about relocs at all.

   2. When vms_slurp_reloc_table is invoked for the first time on a section
      with relocs, the object is traversed and all the reloc information
      is saved in per-section reloc buffers.  It would be very inefficient
      to scan the whole file on each invocation, so we slurp for all the
      sections at once.

   3. On subsequent invocations of vms_slurp_reloc_table, the relocs for the
      specified section are fetched from the buffer, decoded and internalized.
      The buffer is then freed since the internalized relocs are attached to
      the section, turning additional invocations of vms_slurp_reloc_table
      on the same section into no-ops.

   Since VMS objects have very few sections, it could be profitable to merge
   phase #2 and phase #3, i.e. to decode and internalize the relocs for all
   the sections at once.  The current implementation is more elegant.  */

static bfd_boolean
vms_slurp_reloc_table (bfd *abfd, asection *section, asymbol **symbols)
{
  arelent *internal_relocs;
  bfd_size_type amt;
  int err;

  /* If there were no relocations, there is nothing to do.  */
  if (section->reloc_count == 0)
    return TRUE;

  /* Return saved information about the relocations if it is available.  */
  if (section->relocation != NULL)
    return TRUE;

  /* If the relocation stream has not been slurped, do it now.  */
  if (vms_section_data (section)->reloc_stream == NULL)
    {
      bfd_boolean alloc_error = FALSE;
      int type;

      /* Size the reloc buffer for each section.  */
      bfd_map_over_sections (abfd, alloc_reloc_stream, &alloc_error);
      if (alloc_error)
	return FALSE;

      if (bfd_seek (abfd, 0, SEEK_SET) != 0)
	return FALSE;

      /* Reset section pointer.  */
      PRIV (image_section) = NULL;

      do
	{
	  type = _bfd_vms_get_object_record (abfd);
	  if (type != EOBJ_S_C_ETIR
	      && type != EOBJ_S_C_EDBG
	      && type != EOBJ_S_C_ETBT)
	    continue;
	  err = _bfd_vms_slurp_relocs (abfd);
	  if (err != 0)
	    {
	      vms_debug2 ((2, "slurp relocs failed with %d\n", err));
	      return FALSE;
	    }
	}
      while (type != EOBJ_S_C_EEOM);
    }

  amt = section->reloc_count * sizeof (arelent);
  internal_relocs = (arelent *) bfd_zalloc (abfd, amt);
  if (internal_relocs == NULL)
    return FALSE;

  /* Decode and internalize the relocations.  */
  err = _bfd_vms_decode_relocs (abfd, internal_relocs, section, symbols);
  if (err != 0)
    {
      vms_debug2 ((2, "decode relocs failed with %d\n", err));
      return FALSE;
    }

  /* We're done with the external relocations.  Free them.  */
  free (vms_section_data (section)->reloc_stream);
  vms_section_data (section)->reloc_stream = NULL;

  /* Save our results and return success.  */
  section->relocation = internal_relocs;
  return TRUE;
}

/* Return the number of bytes required to store the relocation
   information associated with the given section.  */

static long
vms_get_reloc_upper_bound (bfd *abfd ATTRIBUTE_UNUSED, asection *section)
{
  return (section->reloc_count + 1) * sizeof (arelent *);  
}

/* Convert relocations from VMS (external) form into BFD internal
   form.  Return the number of relocations.  */

static long
vms_canonicalize_reloc (bfd *abfd, asection *section, arelent **relptr,
			asymbol **symbols)
{
  arelent *tblptr;
  int count;

  if (! vms_slurp_reloc_table (abfd, section, symbols))
    return -1;

  count = section->reloc_count;
  tblptr = section->relocation;

  while (count--)
    *relptr++ = tblptr++;

  *relptr = (arelent *) NULL;
  return section->reloc_count;
}

/* This is just copied from ecoff-alpha, needs to be fixed probably.  */

/* How to process the various reloc types.  */

static bfd_reloc_status_type
reloc_nil (bfd * abfd ATTRIBUTE_UNUSED,
	   arelent *reloc ATTRIBUTE_UNUSED,
	   asymbol *sym ATTRIBUTE_UNUSED,
	   void * data ATTRIBUTE_UNUSED,
	   asection *sec ATTRIBUTE_UNUSED,
	   bfd *output_bfd ATTRIBUTE_UNUSED,
	   char **error_message ATTRIBUTE_UNUSED)
{
#if VMS_DEBUG
  vms_debug (1, "reloc_nil (abfd %p, output_bfd %p)\n", abfd, output_bfd);
  vms_debug (2, "In section %s, symbol %s\n",
	sec->name, sym->name);
  vms_debug (2, "reloc sym %s, addr %08lx, addend %08lx, reloc is a %s\n",
		reloc->sym_ptr_ptr[0]->name,
		(unsigned long)reloc->address,
		(unsigned long)reloc->addend, reloc->howto->name);
  vms_debug (2, "data at %p\n", data);
  /*  _bfd_hexdump (2, data, bfd_get_reloc_size (reloc->howto), 0); */
#endif

  return bfd_reloc_ok;
}

/* In case we're on a 32-bit machine, construct a 64-bit "-1" value
   from smaller values.  Start with zero, widen, *then* decrement.  */
#define MINUS_ONE	(((bfd_vma)0) - 1)

static reloc_howto_type alpha_howto_table[] =
{
  HOWTO (ALPHA_R_IGNORE,	/* Type.  */
	 0,			/* Rightshift.  */
	 0,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 8,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "IGNORE",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 0,			/* Source mask */
	 0,			/* Dest mask.  */
	 TRUE),			/* PC rel offset.  */

  /* A 64 bit reference to a symbol.  */
  HOWTO (ALPHA_R_REFQUAD,	/* Type.  */
	 0,			/* Rightshift.  */
	 4,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 64,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_bitfield, /* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "REFQUAD",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 MINUS_ONE,		/* Source mask.  */
	 MINUS_ONE,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* A 21 bit branch.  The native assembler generates these for
     branches within the text segment, and also fills in the PC
     relative offset in the instruction.  */
  HOWTO (ALPHA_R_BRADDR,	/* Type.  */
	 2,			/* Rightshift.  */
	 2,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 21,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_signed, /* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "BRADDR",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 0x1fffff,		/* Source mask.  */
	 0x1fffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* A hint for a jump to a register.  */
  HOWTO (ALPHA_R_HINT,		/* Type.  */
	 2,			/* Rightshift.  */
	 1,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 14,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "HINT",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 0x3fff,		/* Source mask.  */
	 0x3fff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* 16 bit PC relative offset.  */
  HOWTO (ALPHA_R_SREL16,	/* Type.  */
	 0,			/* Rightshift.  */
	 1,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 16,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_signed, /* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "SREL16",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 0xffff,		/* Source mask.  */
	 0xffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* 32 bit PC relative offset.  */
  HOWTO (ALPHA_R_SREL32,	/* Type.  */
	 0,			/* Rightshift.  */
	 2,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 32,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_signed, /* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "SREL32",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* A 64 bit PC relative offset.  */
  HOWTO (ALPHA_R_SREL64,	/* Type.  */
	 0,			/* Rightshift.  */
	 4,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 64,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_signed, /* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "SREL64",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 MINUS_ONE,		/* Source mask.  */
	 MINUS_ONE,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* Push a value on the reloc evaluation stack.  */
  HOWTO (ALPHA_R_OP_PUSH,	/* Type.  */
	 0,			/* Rightshift.  */
	 0,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 0,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "OP_PUSH",		/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0,			/* Source mask.  */
	 0,			/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* Store the value from the stack at the given address.  Store it in
     a bitfield of size r_size starting at bit position r_offset.  */
  HOWTO (ALPHA_R_OP_STORE,	/* Type.  */
	 0,			/* Rightshift.  */
	 4,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 64,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "OP_STORE",		/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0,			/* Source mask.  */
	 MINUS_ONE,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* Subtract the reloc address from the value on the top of the
     relocation stack.  */
  HOWTO (ALPHA_R_OP_PSUB,	/* Type.  */
	 0,			/* Rightshift.  */
	 0,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 0,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "OP_PSUB",		/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0,			/* Source mask.  */
	 0,			/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* Shift the value on the top of the relocation stack right by the
     given value.  */
  HOWTO (ALPHA_R_OP_PRSHIFT,	/* Type.  */
	 0,			/* Rightshift.  */
	 0,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 0,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "OP_PRSHIFT",		/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0,			/* Source mask.  */
	 0,			/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* Hack. Linkage is done by linker.  */
  HOWTO (ALPHA_R_LINKAGE,	/* Type.  */
	 0,			/* Rightshift.  */
	 8,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 256,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "LINKAGE",		/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0,			/* Source mask.  */
	 0,			/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* A 32 bit reference to a symbol.  */
  HOWTO (ALPHA_R_REFLONG,	/* Type.  */
	 0,			/* Rightshift.  */
	 2,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 32,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_bitfield, /* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "REFLONG",		/* Name.  */
	 TRUE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  /* A 64 bit reference to a procedure, written as 32 bit value.  */
  HOWTO (ALPHA_R_CODEADDR,	/* Type.  */
	 0,			/* Rightshift.  */
	 4,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 64,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_signed,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "CODEADDR",		/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  HOWTO (ALPHA_R_NOP,		/* Type.  */
	 0,			/* Rightshift.  */
	 3,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 0,			/* Bitsize.  */
	 /* The following value must match that of ALPHA_R_BSR/ALPHA_R_BOH
	    because the calculations for the 3 relocations are the same.
	    See B.4.5.2 of the OpenVMS Linker Utility Manual.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.   */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "NOP",			/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  HOWTO (ALPHA_R_BSR,		/* Type.  */
	 0,			/* Rightshift.  */
	 3,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 0,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "BSR",			/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  HOWTO (ALPHA_R_LDA,		/* Type.  */
	 0,			/* Rightshift.  */
	 3,			/* Size (0 = byte, 1 = short, 2 = long).  */
	 0,			/* Bitsize.  */
	 FALSE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "LDA",			/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */

  HOWTO (ALPHA_R_BOH,		/* Type.  */
	 0,			/* Rightshift.  */
	 3,			/* Size (0 = byte, 1 = short, 2 = long, 3 = nil).  */
	 0,			/* Bitsize.  */
	 TRUE,			/* PC relative.  */
	 0,			/* Bitpos.  */
	 complain_overflow_dont,/* Complain_on_overflow.  */
	 reloc_nil,		/* Special_function.  */
	 "BOH",			/* Name.  */
	 FALSE,			/* Partial_inplace.  */
	 0xffffffff,		/* Source mask.  */
	 0xffffffff,		/* Dest mask.  */
	 FALSE),		/* PC rel offset.  */
};

/* Return a pointer to a howto structure which, when invoked, will perform
   the relocation code on data from the architecture noted.  */

static const struct reloc_howto_struct *
vms_bfd_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
			   bfd_reloc_code_real_type code)
{
  int alpha_type;

  vms_debug2 ((1, "vms_bfd_reloc_type_lookup (%p, %d)\t", abfd, code));

  switch (code)
    {
      case BFD_RELOC_16:		alpha_type = ALPHA_R_SREL16;	break;
      case BFD_RELOC_32:		alpha_type = ALPHA_R_REFLONG;	break;
      case BFD_RELOC_64:		alpha_type = ALPHA_R_REFQUAD;	break;
      case BFD_RELOC_CTOR:		alpha_type = ALPHA_R_REFQUAD;	break;
      case BFD_RELOC_23_PCREL_S2:	alpha_type = ALPHA_R_BRADDR;	break;
      case BFD_RELOC_ALPHA_HINT:	alpha_type = ALPHA_R_HINT;	break;
      case BFD_RELOC_16_PCREL:		alpha_type = ALPHA_R_SREL16;	break;
      case BFD_RELOC_32_PCREL:		alpha_type = ALPHA_R_SREL32;	break;
      case BFD_RELOC_64_PCREL:		alpha_type = ALPHA_R_SREL64;	break;
      case BFD_RELOC_ALPHA_LINKAGE:	alpha_type = ALPHA_R_LINKAGE;	break;
      case BFD_RELOC_ALPHA_CODEADDR:	alpha_type = ALPHA_R_CODEADDR;	break;
      case BFD_RELOC_ALPHA_NOP:		alpha_type = ALPHA_R_NOP;	break;
      case BFD_RELOC_ALPHA_BSR:		alpha_type = ALPHA_R_BSR;	break;
      case BFD_RELOC_ALPHA_LDA:		alpha_type = ALPHA_R_LDA;	break;
      case BFD_RELOC_ALPHA_BOH:		alpha_type = ALPHA_R_BOH;	break;
      default:
	(*_bfd_error_handler) ("reloc (%d) is *UNKNOWN*", code);
	return NULL;
    }
  vms_debug2 ((2, "reloc is %s\n", alpha_howto_table[alpha_type].name));
  return & alpha_howto_table[alpha_type];
}

static reloc_howto_type *
vms_bfd_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			   const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (alpha_howto_table) / sizeof (alpha_howto_table[0]);
       i++)
    if (alpha_howto_table[i].name != NULL
	&& strcasecmp (alpha_howto_table[i].name, r_name) == 0)
      return &alpha_howto_table[i];

  return NULL;
}

/* Part 4.7, writing an object file.  */

/* Set the architecture and machine type in BFD abfd to arch and mach.
   Find the correct pointer to a structure and insert it into the arch_info
   pointer.  */

static bfd_boolean
vms_set_arch_mach (bfd * abfd,
		   enum bfd_architecture arch ATTRIBUTE_UNUSED,
		   unsigned long mach ATTRIBUTE_UNUSED)
{
  vms_debug2 ((1, "vms_set_arch_mach (%p, %d, %ld)\n", abfd, arch, mach));

  if (arch != bfd_arch_alpha
      && arch != bfd_arch_vax
      && arch != bfd_arch_unknown)
    return FALSE;

  return bfd_default_set_arch_mach (abfd, arch, mach);
}

/* Sets the contents of the section section in BFD abfd to the data starting
   in memory at LOCATION. The data is written to the output section starting
   at offset offset for count bytes.

   Normally TRUE is returned, else FALSE. Possible error returns are:
   o bfd_error_no_contents - The output section does not have the
	SEC_HAS_CONTENTS attribute, so nothing can be written to it.
   o and some more too  */

static bfd_boolean
vms_set_section_contents (bfd * abfd,
			  asection *section,
			  const void * location,
			  file_ptr offset,
			  bfd_size_type count)
{
#if VMS_DEBUG
  vms_debug (1, "vms_set_section_contents (%p, sec %s, loc %p, off %ld, count %d)\n",
	     abfd, section->name, location, (long int)offset, (int)count);
  vms_debug (2, "size %d\n", (int) section->size);
#endif
  if (count == (bfd_size_type)0)
    return TRUE;

  if (section->contents == NULL)
    section->contents = bfd_alloc (abfd, section->size);
  if (section->contents == NULL)
    return FALSE;

  memcpy (section->contents + offset, location, (size_t) count);
  return TRUE;
}

const bfd_target vms_alpha_vec =
{
  "vms-alpha",			/* Name.  */
  bfd_target_evax_flavour,
  BFD_ENDIAN_LITTLE,		/* Data byte order is little.  */
  BFD_ENDIAN_LITTLE,		/* Header byte order is little.  */

  (HAS_RELOC | EXEC_P | HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS
   | WP_TEXT | D_PAGED),	/* Object flags.  */
  (SEC_ALLOC | SEC_LOAD | SEC_RELOC
   | SEC_READONLY | SEC_CODE | SEC_DATA
   | SEC_HAS_CONTENTS | SEC_IN_MEMORY),		/* Sect flags.  */
  0,				/* symbol_leading_char.  */
  ' ',				/* ar_pad_char.  */
  15,				/* ar_max_namelen.  */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
  bfd_getl32, bfd_getl_signed_32, bfd_putl32,
  bfd_getl16, bfd_getl_signed_16, bfd_putl16,
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
  bfd_getl32, bfd_getl_signed_32, bfd_putl32,
  bfd_getl16, bfd_getl_signed_16, bfd_putl16,

  {_bfd_dummy_target, vms_object_p,		/* bfd_check_format.  */
   _bfd_dummy_target, _bfd_dummy_target},
  {bfd_false, vms_mkobject,			/* bfd_set_format.  */
   bfd_false, bfd_false},
  {bfd_false, vms_write_object_contents,	/* bfd_write_contents.  */
   bfd_false, bfd_false},

  BFD_JUMP_TABLE_GENERIC (vms),
  BFD_JUMP_TABLE_COPY (vms),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (vms),
  BFD_JUMP_TABLE_RELOCS (vms),
  BFD_JUMP_TABLE_WRITE (vms),
  BFD_JUMP_TABLE_LINK (_bfd_nolink),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,

  (PTR) 0
};

const bfd_target vms_vax_vec =
{
  "vms-vax",			/* Name.  */
  bfd_target_ovax_flavour,
  BFD_ENDIAN_LITTLE,		/* Data byte order is little.  */
  BFD_ENDIAN_LITTLE,		/* Header byte order is little.  */

  (HAS_RELOC | HAS_SYMS 	/* Object flags.  */
   | WP_TEXT | D_PAGED
   | HAS_LINENO | HAS_DEBUG | HAS_LOCALS),

  (SEC_ALLOC | SEC_LOAD | SEC_RELOC
   | SEC_READONLY | SEC_CODE | SEC_DATA
   | SEC_HAS_CONTENTS | SEC_IN_MEMORY),		/* Sect flags.  */
  0,				/* symbol_leading_char */
  ' ',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
  bfd_getl32, bfd_getl_signed_32, bfd_putl32,
  bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* Data.  */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
  bfd_getl32, bfd_getl_signed_32, bfd_putl32,
  bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* Hdrs.  */

  {_bfd_dummy_target, vms_object_p,		/* bfd_check_format.  */
   _bfd_dummy_target, _bfd_dummy_target},
  {bfd_false, vms_mkobject,			/* bfd_set_format.  */
   bfd_false, bfd_false},
  {bfd_false, vms_write_object_contents,	/* bfd_write_contents.  */
   bfd_false, bfd_false},

  BFD_JUMP_TABLE_GENERIC (vms),
  BFD_JUMP_TABLE_COPY (vms),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (vms),
  BFD_JUMP_TABLE_RELOCS (vms),
  BFD_JUMP_TABLE_WRITE (vms),
  BFD_JUMP_TABLE_LINK (_bfd_nolink),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,

  (PTR) 0
};
