#define BYTES_IN_WORD 4
#define ARCH 32
/* #define ENTRY_CAN_BE_ZERO */
#define N_HEADER_IN_TEXT(x) 1
#define N_SHARED_LIB(x) 0
#define N_TXTADDR(x) \
    (N_MAGIC(x) != ZMAGIC ? (x).a_entry :	/* object file or NMAGIC */\
	    TEXT_START_ADDR + EXEC_BYTES_SIZE	/* no padding */\
    )
#define N_DATADDR(x) (N_TXTADDR(x)+N_TXTSIZE(x))
#define TEXT_START_ADDR 4096
#define PAGE_SIZE 4096
#define SEGMENT_SIZE PAGE_SIZE
#define DEFAULT_ARCH bfd_arch_mips
#define MY_symbol_leading_char '\0'

#define MY(OP) CAT(mipsbsd_,OP)

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libaout.h"

#define SET_ARCH_MACH(ABFD, EXEC) \
  MY(set_arch_mach)(ABFD, N_MACHTYPE (EXEC)); \
  MY(choose_reloc_size)(ABFD);
void MY(set_arch_mach) PARAMS ((bfd *abfd, int machtype));
static void MY(choose_reloc_size) PARAMS ((bfd *abfd));

#define MY_write_object_contents MY(write_object_contents)
static boolean MY(write_object_contents) PARAMS ((bfd *abfd));

#define MY_reloc_howto_type_lookup MY(reloc_howto_type_lookup)
#define MY_canonicalize_reloc MY(canonicalize_reloc)

#define MY_backend_data &MY(backend_data)
#define MY_BFD_TARGET

#include "aout-target.h"

/*
 * These really should be common to all BSD systems.
 * Also, to reduce space, should ifdef the individual cases if MINIMIZE=1.
 */
void
MY(set_arch_mach) (abfd, machtype)
     bfd *abfd;
     int machtype;
{
  enum bfd_architecture arch;
  long machine;

  /* Determine the architecture and machine type of the object file. */
  switch (machtype) {

  case M_68010:
  case M_HP200:
    arch = bfd_arch_m68k;
    machine = 68010;
    break;

  case M_68020:
  case M_HP300:
    arch = bfd_arch_m68k;
    machine = 68020;
    break;

  case M_SPARC:
    arch = bfd_arch_sparc;
    machine = 0;
    break;

  case M_386:
    arch = bfd_arch_i386;
    machine = 0;
    break;

  case M_29K:
    arch = bfd_arch_a29k;
    machine = 0;
    break;

  case M_HPUX:
    arch = bfd_arch_m68k;
    machine = 0;
    break;

  case M_MIPS1:
    arch = bfd_arch_mips;
    machine = 3000;
    break;

  case M_MIPS2:
    arch = bfd_arch_mips;
    machine = 4000;
    break;

  default:
    arch = bfd_arch_obscure;
    machine = 0;
    break;
  }
  bfd_set_arch_mach(abfd, arch, machine);  
}

/* Determine the size of a relocation entry, based on the architecture */
static void
MY(choose_reloc_size) (abfd)
     bfd *abfd;
{
  switch (bfd_get_arch(abfd)) {
  case bfd_arch_sparc:
  case bfd_arch_a29k:
  case bfd_arch_mips:
    obj_reloc_entry_size (abfd) = RELOC_EXT_SIZE;
    break;
  default:
    obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
    break;
  }
}

/* Write an object file in BSD a.out format.
  Section contents have already been written.  We write the
  file header, symbols, and relocation.  */

static boolean
MY(write_object_contents) (abfd)
     bfd *abfd;
{
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);

  /* Magic number, maestro, please!  */
  switch (bfd_get_arch(abfd)) {
  case bfd_arch_m68k:
    switch (bfd_get_mach(abfd)) {
    case 68010:
      N_SET_MACHTYPE(*execp, M_68010);
      break;
    default:
    case 68020:
      N_SET_MACHTYPE(*execp, M_68020);
      break;
    }
    break;
  case bfd_arch_sparc:
    N_SET_MACHTYPE(*execp, M_SPARC);
    break;
  case bfd_arch_i386:
    N_SET_MACHTYPE(*execp, M_386);
    break;
  case bfd_arch_a29k:
    N_SET_MACHTYPE(*execp, M_29K);
    break;
  case bfd_arch_mips:
    switch (bfd_get_mach(abfd)) {
    case 4000:
    case 6000:
      N_SET_MACHTYPE(*execp, M_MIPS2);
      break;
    default:
      N_SET_MACHTYPE(*execp, M_MIPS1);
      break;
    }
    break;
  default:
    N_SET_MACHTYPE(*execp, M_UNKNOWN);
  }

  MY(choose_reloc_size)(abfd);

  WRITE_HEADERS(abfd, execp);

  return true;
}

/*
 * MIPS relocation types.
 */
#define MIPS_RELOC_32		0
#define MIPS_RELOC_JMP		1
#define MIPS_RELOC_WDISP16	2
#define MIPS_RELOC_HI16		3
#define MIPS_RELOC_HI16_S	4
#define MIPS_RELOC_LO16		5

/*
 * This is only called when performing a BFD_RELOC_HI16_S relocation.
 * We need to see if bit 15 is set in the result. If it is, we add
 * 0x10000 and continue normally. This will compensate for the sign extension
 * when the low bits are added at run time.
 */
bfd_reloc_status_type
mips_fix_hi16_s (abfd,reloc_entry,symbol,data,input_section,output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     struct symbol_cache_entry *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  bfd_vma relocation;
 
  /* If this is a partial relocation, just continue. */
  if (output_bfd != (bfd *)NULL)
    return bfd_reloc_continue;

  /* 
   * Work out which section the relocation is targetted at and the
   * initial relocation command value.
   */
  if (symbol->section == &bfd_com_section)
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;

  if (relocation & 0x8000)
    reloc_entry->addend += 0x10000;

  return bfd_reloc_continue;
}

static reloc_howto_type mips_howto_table_ext[] = {
  MIPS_RELOC_32,      0, 2, 32, false, 0, true,  true, 0,
	"32",       false, 0, 0xffffffff, false,
  MIPS_RELOC_JMP,     2, 2, 26, false, 0, false, true, 0,
	"MIPS_JMP", false, 0, 0x03ffffff, false,
  MIPS_RELOC_WDISP16, 2, 1, 16, true,  0, false, true, 0,
	"WDISP16",  false, 0, 0x0000ffff, false,
  MIPS_RELOC_HI16,   16, 1, 16, false, 0, false, true, 0,
	"HI16",     false, 0, 0x0000ffff, false,
  MIPS_RELOC_HI16_S, 16, 1, 16, false, 0, false, true, mips_fix_hi16_s,
	"HI16_S",   false, 0, 0x0000ffff, false,
  MIPS_RELOC_LO16,    0, 1, 16, false, 0, false, true, 0,
	"LO16",     false, 0, 0x0000ffff, false,
};

static reloc_howto_type *
MY(reloc_howto_type_lookup) (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  extern reloc_howto_type NAME(aout,ext_howto_table)[];

  switch (bfd_get_arch (abfd))
    {
    case bfd_arch_sparc:
      switch (code)
	{
	default:
	  return 0;
#define IDX(i,j)	case i: return &NAME(aout,ext_howto_table)[j]
	IDX (BFD_RELOC_CTOR, 2);
	IDX (BFD_RELOC_32, 2);
	IDX (BFD_RELOC_HI22, 8);
	IDX (BFD_RELOC_LO10, 11);
	IDX (BFD_RELOC_32_PCREL_S2, 6);
	}
    case bfd_arch_mips:
      switch (code)
	{
	case BFD_RELOC_32:
	  return (&mips_howto_table_ext[MIPS_RELOC_32]);
	case BFD_RELOC_MIPS_JMP:
	  return (&mips_howto_table_ext[MIPS_RELOC_JMP]);
	case BFD_RELOC_16_PCREL_S2:
	  return (&mips_howto_table_ext[MIPS_RELOC_WDISP16]);
	case BFD_RELOC_HI16:
	  return (&mips_howto_table_ext[MIPS_RELOC_HI16]);
	case BFD_RELOC_HI16_S:
	  return (&mips_howto_table_ext[MIPS_RELOC_HI16_S]);
	case BFD_RELOC_LO16:
	  return (&mips_howto_table_ext[MIPS_RELOC_LO16]);
	default:
	  return 0;
	}
    default:
      return 0;
    }
}

/*
 * This is just like the standard aoutx.h version but we need to do our
 * own mapping of external reloc type values to howto entries.
 */
unsigned int
MY(canonicalize_reloc)(abfd, section, relptr, symbols)
      bfd *abfd;
      sec_ptr section;
      arelent **relptr;
      asymbol **symbols;
{
  arelent *tblptr = section->relocation;
  unsigned int count, c;
  extern reloc_howto_type NAME(aout,ext_howto_table)[];

  /* If we have already read in the relocation table, return the values. */
  if (section->flags & SEC_CONSTRUCTOR) {
    arelent_chain *chain = section->constructor_chain;

    for (count = 0; count < section->reloc_count; count++) {
      *relptr++ = &chain->relent;
      chain = chain->next;
    }
    *relptr = 0;
    return section->reloc_count;
  }
  if (tblptr && section->reloc_count) {
    for (count = 0; count++ < section->reloc_count;) 
      *relptr++ = tblptr++;
    *relptr = 0;
    return section->reloc_count;
  }

  if (!NAME(aout,slurp_reloc_table)(abfd, section, symbols))
    return 0;
  tblptr = section->relocation;
  if (!tblptr)
    return 0;

  /* fix up howto entries */
  for (count = 0; count++ < section->reloc_count;) 
    {
      c = tblptr->howto - NAME(aout,ext_howto_table);
      tblptr->howto = &mips_howto_table_ext[c];

      *relptr++ = tblptr++;
    }
  *relptr = 0;
  return section->reloc_count;
}

static CONST struct aout_backend_data MY(backend_data) = {
  0,				/* zmagic contiguous */
  1,				/* text incl header */
  PAGE_SIZE,			/* text vma */
  MY_set_sizes,
  0,				/* text size includes exec header */
};

bfd_target aout_mips_little_vec =
{
  "aout-mips-little",		/* name */
  bfd_target_aout_flavour,
  false,			/* target byte order (little) */
  false,			/* target headers byte order (little) */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  MY_symbol_leading_char,
  ' ',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  1,				/* minimum alignment */
  _do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* data */
  _do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* hdrs */
    {_bfd_dummy_target, MY_object_p, /* bfd_check_format */
       bfd_generic_archive_p, MY_core_file_p},
    {bfd_false, MY_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, MY_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

  MY_core_file_failing_command,
  MY_core_file_failing_signal,
  MY_core_file_matches_executable_p,
  MY_slurp_armap,
  MY_slurp_extended_name_table,
  MY_truncate_arname,
  MY_write_armap,
  MY_close_and_cleanup,
  MY_set_section_contents,
  MY_get_section_contents,
  MY_new_section_hook,
  MY_get_symtab_upper_bound,
  MY_get_symtab,
  MY_get_reloc_upper_bound,
  MY_canonicalize_reloc,
  MY_make_empty_symbol,
  MY_print_symbol,
  MY_get_lineno,
  MY_set_arch_mach,
  MY_openr_next_archived_file,
  MY_find_nearest_line,
  MY_generic_stat_arch_elt,
  MY_sizeof_headers,
  MY_bfd_debug_info_start,
  MY_bfd_debug_info_end,
  MY_bfd_debug_info_accumulate,
  bfd_generic_get_relocated_section_contents,
  bfd_generic_relax_section,
  bfd_generic_seclet_link,
  MY_reloc_howto_type_lookup,
  MY_make_debug_symbol,
  (PTR) MY_backend_data,
};

bfd_target aout_mips_big_vec =
{
  "aout-mips-big",		/* name */
  bfd_target_aout_flavour,
  true,				/* target byte order (big) */
  true,				/* target headers byte order (big) */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  MY_symbol_leading_char,
  ' ',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  1,				/* minimum alignment */
  _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
  _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */
    {_bfd_dummy_target, MY_object_p, /* bfd_check_format */
       bfd_generic_archive_p, MY_core_file_p},
    {bfd_false, MY_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, MY_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

  MY_core_file_failing_command,
  MY_core_file_failing_signal,
  MY_core_file_matches_executable_p,
  MY_slurp_armap,
  MY_slurp_extended_name_table,
  MY_truncate_arname,
  MY_write_armap,
  MY_close_and_cleanup,
  MY_set_section_contents,
  MY_get_section_contents,
  MY_new_section_hook,
  MY_get_symtab_upper_bound,
  MY_get_symtab,
  MY_get_reloc_upper_bound,
  MY_canonicalize_reloc,
  MY_make_empty_symbol,
  MY_print_symbol,
  MY_get_lineno,
  MY_set_arch_mach,
  MY_openr_next_archived_file,
  MY_find_nearest_line,
  MY_generic_stat_arch_elt,
  MY_sizeof_headers,
  MY_bfd_debug_info_start,
  MY_bfd_debug_info_end,
  MY_bfd_debug_info_accumulate,
  bfd_generic_get_relocated_section_contents,
  bfd_generic_relax_section,
  bfd_generic_seclet_link,
  MY_reloc_howto_type_lookup,
  MY_make_debug_symbol,
  (PTR) MY_backend_data,
};
