/* 32-bit ELF support for ARM
   Copyright 1998, 1999 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


typedef unsigned long int insn32;
typedef unsigned short int insn16;

static reloc_howto_type *elf32_arm_reloc_type_lookup
  PARAMS ((bfd * abfd, bfd_reloc_code_real_type code));
static void elf32_arm_info_to_howto
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));
static boolean elf32_arm_set_private_flags
  PARAMS ((bfd *, flagword));
static boolean elf32_arm_copy_private_bfd_data
  PARAMS ((bfd *, bfd *));
static boolean elf32_arm_merge_private_bfd_data
  PARAMS ((bfd *, bfd *));
static boolean elf32_arm_print_private_bfd_data
  PARAMS ((bfd *, PTR));
static int elf32_arm_get_symbol_type 
  PARAMS (( Elf_Internal_Sym *, int));
static struct bfd_link_hash_table *elf32_arm_link_hash_table_create
  PARAMS ((bfd *));


static insn32 insert_thumb_branch
  PARAMS ((insn32, int));
static struct elf_link_hash_entry *find_thumb_glue
  PARAMS ((struct bfd_link_info *, CONST char *, bfd *));
static struct elf_link_hash_entry *find_arm_glue
  PARAMS ((struct bfd_link_info *, CONST char *, bfd *));
static void record_arm_to_thumb_glue
  PARAMS ((struct bfd_link_info *, struct elf_link_hash_entry *));
static void record_thumb_to_arm_glue
  PARAMS ((struct bfd_link_info *, struct elf_link_hash_entry *));

/* The linker script knows the section names for placement.
   The entry_names are used to do simple name mangling on the stubs.
   Given a function name, and its type, the stub can be found. The
   name can be changed. The only requirement is the %s be present.
 */

#define INTERWORK_FLAG( abfd )   (elf_elfheader (abfd)->e_flags & EF_INTERWORK)

#define THUMB2ARM_GLUE_SECTION_NAME ".glue_7t"
#define THUMB2ARM_GLUE_ENTRY_NAME   "__%s_from_thumb"

#define ARM2THUMB_GLUE_SECTION_NAME ".glue_7"
#define ARM2THUMB_GLUE_ENTRY_NAME   "__%s_from_arm"

/* Get the ARM elf linker hash table from a link_info structure.  */
#define elf32_arm_hash_table(info) \
  ((struct elf32_arm_link_hash_table *) ((info)->hash))

/* ARM ELF linker hash table */
struct elf32_arm_link_hash_table
  {
    /* The main hash table.  */
    struct elf_link_hash_table root;

    /* The size in bytes of the section containg the Thumb-to-ARM glue.  */
    long int thumb_glue_size;

    /* The size in bytes of the section containg the ARM-to-Thumb glue.  */
    long int arm_glue_size;

    /* An arbitary input BFD chosen to hold the glue sections.  */
    bfd *bfd_of_glue_owner;

  };



/* Create an ARM elf linker hash table */

static struct bfd_link_hash_table *
elf32_arm_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct elf32_arm_link_hash_table *ret;

  ret = ((struct elf32_arm_link_hash_table *)
	 bfd_alloc (abfd, sizeof (struct elf32_arm_link_hash_table)));
  if (ret == (struct elf32_arm_link_hash_table *) NULL)
    return NULL;

  if (!_bfd_elf_link_hash_table_init (&ret->root, abfd,
				      _bfd_elf_link_hash_newfunc))
    {
      bfd_release (abfd, ret);
      return NULL;
    }

  ret->thumb_glue_size = 0;
  ret->arm_glue_size = 0;
  ret->bfd_of_glue_owner = NULL;

  return &ret->root.root;
}

static struct elf_link_hash_entry *
find_thumb_glue (link_info, name, input_bfd)
     struct bfd_link_info *link_info;
     CONST char *name;
     bfd *input_bfd;
{
  char *tmp_name;
  struct elf_link_hash_entry *hash;
  struct elf32_arm_link_hash_table *hash_table;

  /* We need a pointer to the armelf specific hash table.  */
  hash_table = elf32_arm_hash_table (link_info);


  tmp_name = ((char *)
       bfd_malloc (strlen (name) + strlen (THUMB2ARM_GLUE_ENTRY_NAME) + 1));

  BFD_ASSERT (tmp_name);

  sprintf (tmp_name, THUMB2ARM_GLUE_ENTRY_NAME, name);

  hash = elf_link_hash_lookup
    (&(hash_table)->root, tmp_name, false, false, true);

  if (hash == NULL)
    /* xgettext:c-format */
    _bfd_error_handler (_ ("%s: unable to find THUMB glue '%s' for `%s'"),
			bfd_get_filename (input_bfd), tmp_name, name);

  free (tmp_name);

  return hash;
}

static struct elf_link_hash_entry *
find_arm_glue (link_info, name, input_bfd)
     struct bfd_link_info *link_info;
     CONST char *name;
     bfd *input_bfd;
{
  char *tmp_name;
  struct elf_link_hash_entry *myh;
  struct elf32_arm_link_hash_table *hash_table;

  /* We need a pointer to the elfarm specific hash table.  */
  hash_table = elf32_arm_hash_table (link_info);

  tmp_name = ((char *)
       bfd_malloc (strlen (name) + strlen (ARM2THUMB_GLUE_ENTRY_NAME) + 1));

  BFD_ASSERT (tmp_name);

  sprintf (tmp_name, ARM2THUMB_GLUE_ENTRY_NAME, name);

  myh = elf_link_hash_lookup
    (&(hash_table)->root, tmp_name, false, false, true);

  if (myh == NULL)
    /* xgettext:c-format */
    _bfd_error_handler (_ ("%s: unable to find ARM glue '%s' for `%s'"),
			bfd_get_filename (input_bfd), tmp_name, name);

  free (tmp_name);

  return myh;
}

/*
   ARM->Thumb glue:

   .arm
   __func_from_arm:
   ldr r12, __func_addr
   bx  r12
   __func_addr:
   .word func    @ behave as if you saw a ARM_32 reloc
 */

#define ARM2THUMB_GLUE_SIZE 12
static const insn32 a2t1_ldr_insn = 0xe59fc000;
static const insn32 a2t2_bx_r12_insn = 0xe12fff1c;
static const insn32 a2t3_func_addr_insn = 0x00000001;

/*
   Thumb->ARM:                          Thumb->(non-interworking aware) ARM

   .thumb                               .thumb
   .align 2                             .align 2
   __func_from_thumb:              __func_from_thumb:
   bx pc                                push {r6, lr}
   nop                                  ldr  r6, __func_addr
   .arm                                         mov  lr, pc
   __func_change_to_arm:                        bx   r6
   b func                       .arm
   __func_back_to_thumb:
   ldmia r13! {r6, lr}
   bx    lr
   __func_addr:
   .word        func 
 */

#define THUMB2ARM_GLUE_SIZE 8
static const insn16 t2a1_bx_pc_insn = 0x4778;
static const insn16 t2a2_noop_insn = 0x46c0;
static const insn32 t2a3_b_insn = 0xea000000;

static const insn16 t2a1_push_insn = 0xb540;
static const insn16 t2a2_ldr_insn = 0x4e03;
static const insn16 t2a3_mov_insn = 0x46fe;
static const insn16 t2a4_bx_insn = 0x4730;
static const insn32 t2a5_pop_insn = 0xe8bd4040;
static const insn32 t2a6_bx_insn = 0xe12fff1e;

boolean
bfd_elf32_arm_allocate_interworking_sections (info)
     struct bfd_link_info *info;
{
  asection *s;
  bfd_byte *foo;
  struct elf32_arm_link_hash_table *globals;

  globals = elf32_arm_hash_table (info);

  BFD_ASSERT (globals != NULL);

  if (globals->arm_glue_size != 0)
    {
      BFD_ASSERT (globals->bfd_of_glue_owner != NULL);

      s = bfd_get_section_by_name
	(globals->bfd_of_glue_owner, ARM2THUMB_GLUE_SECTION_NAME);

      BFD_ASSERT (s != NULL);

      foo = (bfd_byte *) bfd_alloc
	(globals->bfd_of_glue_owner, globals->arm_glue_size);

      s->_raw_size = s->_cooked_size = globals->arm_glue_size;
      s->contents = foo;
    }

  if (globals->thumb_glue_size != 0)
    {
      BFD_ASSERT (globals->bfd_of_glue_owner != NULL);

      s = bfd_get_section_by_name
	(globals->bfd_of_glue_owner, THUMB2ARM_GLUE_SECTION_NAME);

      BFD_ASSERT (s != NULL);

      foo = (bfd_byte *) bfd_alloc
	(globals->bfd_of_glue_owner, globals->thumb_glue_size);

      s->_raw_size = s->_cooked_size = globals->thumb_glue_size;
      s->contents = foo;
    }

  return true;
}

static void
record_arm_to_thumb_glue (link_info, h)
     struct bfd_link_info *link_info;
     struct elf_link_hash_entry *h;
{
  const char *name = h->root.root.string;
  register asection *s;
  char *tmp_name;
  struct elf_link_hash_entry *myh;
  struct elf32_arm_link_hash_table *globals;

  globals = elf32_arm_hash_table (link_info);

  BFD_ASSERT (globals != NULL);
  BFD_ASSERT (globals->bfd_of_glue_owner != NULL);

  s = bfd_get_section_by_name
    (globals->bfd_of_glue_owner, ARM2THUMB_GLUE_SECTION_NAME);


  BFD_ASSERT (s != NULL);

  tmp_name = ((char *)
       bfd_malloc (strlen (name) + strlen (ARM2THUMB_GLUE_ENTRY_NAME) + 1));

  BFD_ASSERT (tmp_name);

  sprintf (tmp_name, ARM2THUMB_GLUE_ENTRY_NAME, name);

  myh = elf_link_hash_lookup
    (&(globals)->root, tmp_name, false, false, true);

  if (myh != NULL)
    {
      free (tmp_name);
      return;			/* we've already seen this guy */
    }

  /* The only trick here is using hash_table->arm_glue_size as the value. Even
     though the section isn't allocated yet, this is where we will be putting
     it.  */

  _bfd_generic_link_add_one_symbol (link_info, globals->bfd_of_glue_owner, tmp_name,
				    BSF_GLOBAL,
				    s, globals->arm_glue_size + 1,
				    NULL, true, false,
				    (struct bfd_link_hash_entry **) &myh);

  free (tmp_name);

  globals->arm_glue_size += ARM2THUMB_GLUE_SIZE;

  return;
}

static void
record_thumb_to_arm_glue (link_info, h)
     struct bfd_link_info *link_info;
     struct elf_link_hash_entry *h;
{
  const char *name = h->root.root.string;
  register asection *s;
  char *tmp_name;
  struct elf_link_hash_entry *myh;
  struct elf32_arm_link_hash_table *hash_table;
  char bind;

  hash_table = elf32_arm_hash_table (link_info);

  BFD_ASSERT (hash_table != NULL);
  BFD_ASSERT (hash_table->bfd_of_glue_owner != NULL);

  s = bfd_get_section_by_name
    (hash_table->bfd_of_glue_owner, THUMB2ARM_GLUE_SECTION_NAME);

  BFD_ASSERT (s != NULL);

  tmp_name = (char *) bfd_malloc (strlen (name) + strlen (THUMB2ARM_GLUE_ENTRY_NAME) + 1);

  BFD_ASSERT (tmp_name);

  sprintf (tmp_name, THUMB2ARM_GLUE_ENTRY_NAME, name);

  myh = elf_link_hash_lookup
    (&(hash_table)->root, tmp_name, false, false, true);

  if (myh != NULL)
    {
      free (tmp_name);
      return;			/* we've already seen this guy */
    }

  _bfd_generic_link_add_one_symbol (link_info, hash_table->bfd_of_glue_owner, tmp_name,
			     BSF_GLOBAL, s, hash_table->thumb_glue_size + 1,
				    NULL, true, false,
				    (struct bfd_link_hash_entry **) &myh);

  /* If we mark it 'thumb', the disassembler will do a better job.  */
  bind = ELF_ST_BIND (myh->type);
  myh->type = ELF_ST_INFO (bind, STT_ARM_TFUNC);

  free (tmp_name);

  /* Allocate another symbol to mark where we switch to arm mode.  */

#define CHANGE_TO_ARM "__%s_change_to_arm"
#define BACK_FROM_ARM "__%s_back_from_arm"

  tmp_name = (char *) bfd_malloc (strlen (name) + strlen (CHANGE_TO_ARM) + 1);

  BFD_ASSERT (tmp_name);

  sprintf (tmp_name, CHANGE_TO_ARM, name);

  myh = NULL;

  _bfd_generic_link_add_one_symbol (link_info, hash_table->bfd_of_glue_owner, tmp_name,
			      BSF_LOCAL, s, hash_table->thumb_glue_size + 4,
				    NULL, true, false,
				    (struct bfd_link_hash_entry **) &myh);

  free (tmp_name);

  hash_table->thumb_glue_size += THUMB2ARM_GLUE_SIZE;

  return;
}

/* Select a BFD to be used to hold the sections used by the glue code.
   This function is called from the linker scripts in ld/emultempl/
   {armelf/pe}.em  */
boolean
bfd_elf32_arm_get_bfd_for_interworking (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  struct elf32_arm_link_hash_table *globals;
  flagword flags;
  asection *sec;

  /* If we are only performing a partial link do not bother
     getting a bfd to hold the glue.  */
  if (info->relocateable)
    return true;

  globals = elf32_arm_hash_table (info);

  BFD_ASSERT (globals != NULL);

  if (globals->bfd_of_glue_owner != NULL)
    return true;

  sec = bfd_get_section_by_name (abfd, ARM2THUMB_GLUE_SECTION_NAME);

  if (sec == NULL)
    {
      flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY;

      sec = bfd_make_section (abfd, ARM2THUMB_GLUE_SECTION_NAME);

      if (sec == NULL
	  || !bfd_set_section_flags (abfd, sec, flags)
	  || !bfd_set_section_alignment (abfd, sec, 2))
	return false;
    }

  sec = bfd_get_section_by_name (abfd, THUMB2ARM_GLUE_SECTION_NAME);

  if (sec == NULL)
    {
      flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY;

      sec = bfd_make_section (abfd, THUMB2ARM_GLUE_SECTION_NAME);

      if (sec == NULL
	  || !bfd_set_section_flags (abfd, sec, flags)
	  || !bfd_set_section_alignment (abfd, sec, 2))
	return false;
    }

  /* Save the bfd for later use.  */
  globals->bfd_of_glue_owner = abfd;

  return true;
}

boolean
bfd_elf32_arm_process_before_allocation (abfd, link_info)
     bfd *abfd;
     struct bfd_link_info *link_info;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *free_relocs = NULL;
  Elf_Internal_Rela *irel, *irelend;
  bfd_byte *contents = NULL;
  bfd_byte *free_contents = NULL;
  Elf32_External_Sym *extsyms = NULL;
  Elf32_External_Sym *free_extsyms = NULL;

  asection *sec;
  struct elf32_arm_link_hash_table *globals;

  /* If we are only performing a partial link do not bother
     to construct any glue.  */
  if (link_info->relocateable)
    return true;

  /* Here we have a bfd that is to be included on the link.  We have a hook
     to do reloc rummaging, before section sizes are nailed down.  */

  globals = elf32_arm_hash_table (link_info);

  BFD_ASSERT (globals != NULL);
  BFD_ASSERT (globals->bfd_of_glue_owner != NULL);

  /* Rummage around all the relocs and map the glue vectors.  */
  sec = abfd->sections;

  if (sec == NULL)
    return true;

  for (; sec != NULL; sec = sec->next)
    {
      if (sec->reloc_count == 0)
	continue;

      symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
      /* Load the relocs.  */

      irel = (_bfd_elf32_link_read_relocs (abfd, sec, (PTR) NULL,
					(Elf_Internal_Rela *) NULL, false));

      BFD_ASSERT (irel != 0);

      irelend = irel + sec->reloc_count;
      for (; irel < irelend; irel++)
	{
	  long r_type;
	  unsigned long r_index;
	  unsigned char code;

	  struct elf_link_hash_entry *h;

	  r_type = ELF32_R_TYPE (irel->r_info);
	  r_index = ELF32_R_SYM (irel->r_info);

	  /* These are the only relocation types we care about */
	  if (r_type != R_ARM_PC24
	      && r_type != R_ARM_THM_PC22)
	    continue;

	  /* Get the section contents if we haven't done so already.  */
	  if (contents == NULL)
	    {
	      /* Get cached copy if it exists.  */
	      if (elf_section_data (sec)->this_hdr.contents != NULL)
		contents = elf_section_data (sec)->this_hdr.contents;
	      else
		{
		  /* Go get them off disk.  */
		  contents = (bfd_byte *) bfd_malloc (sec->_raw_size);
		  if (contents == NULL)
		    goto error_return;
		  free_contents = contents;

		  if (!bfd_get_section_contents (abfd, sec, contents,
					      (file_ptr) 0, sec->_raw_size))
		    goto error_return;
		}
	    }

	  /* Read this BFD's symbols if we haven't done so already.  */
	  if (extsyms == NULL)
	    {
	      /* Get cached copy if it exists.  */
	      if (symtab_hdr->contents != NULL)
		extsyms = (Elf32_External_Sym *) symtab_hdr->contents;
	      else
		{
		  /* Go get them off disk.  */
		  extsyms = ((Elf32_External_Sym *)
			     bfd_malloc (symtab_hdr->sh_size));
		  if (extsyms == NULL)
		    goto error_return;
		  free_extsyms = extsyms;
		  if (bfd_seek (abfd, symtab_hdr->sh_offset, SEEK_SET) != 0
		      || (bfd_read (extsyms, 1, symtab_hdr->sh_size, abfd)
			  != symtab_hdr->sh_size))
		    goto error_return;
		}
	    }

	  /* If the relocation is not against a symbol it cannot concern us. */

	  h = NULL;

	  /* We don't care about local symbols */
	  if (r_index < symtab_hdr->sh_info)
	    continue;

	  /* This is an external symbol */
	  r_index -= symtab_hdr->sh_info;
	  h = (struct elf_link_hash_entry *)
	    elf_sym_hashes (abfd)[r_index];

	  /* If the relocation is against a static symbol it must be within
	     the current section and so cannot be a cross ARM/Thumb relocation.  */
	  if (h == NULL)
	    continue;

	  switch (r_type)
	    {
	    case R_ARM_PC24:
	      /* This one is a call from arm code.  We need to look up
	         the target of the call. If it is a thumb target, we
	         insert glue.  */

	      if (ELF_ST_TYPE(h->type) == STT_ARM_TFUNC)
		record_arm_to_thumb_glue (link_info, h);
	      break;

	    case R_ARM_THM_PC22:
	      /* This one is a call from thumb code.  We look 
	         up the target of the call. If it is not a thumb
                 target, we insert glue. */ 

	      if (ELF_ST_TYPE (h->type) != STT_ARM_TFUNC)
		record_thumb_to_arm_glue (link_info, h);
	      break;

	    default:
	      break;
	    }
	}
    }

  return true;
error_return:
  if (free_relocs != NULL)
    free (free_relocs);
  if (free_contents != NULL)
    free (free_contents);
  if (free_extsyms != NULL)
    free (free_extsyms);
  return false;

}

struct elf32_arm_reloc_map
  {
    unsigned char bfd_reloc_val;
    unsigned char elf_reloc_val;
  };

static const struct elf32_arm_reloc_map elf32_arm_reloc_map[] =
{
  {BFD_RELOC_NONE, R_ARM_NONE,},
  {BFD_RELOC_ARM_PCREL_BRANCH, R_ARM_PC24,},
  {BFD_RELOC_32, R_ARM_ABS32,},
  {BFD_RELOC_32_PCREL, R_ARM_REL32,},
  {BFD_RELOC_8, R_ARM_ABS8,},
  {BFD_RELOC_16, R_ARM_ABS16,},
  {BFD_RELOC_ARM_OFFSET_IMM, R_ARM_ABS12,},
  {BFD_RELOC_ARM_THUMB_OFFSET, R_ARM_THM_ABS5,},
  {BFD_RELOC_THUMB_PCREL_BRANCH23, R_ARM_THM_PC22,},
  {BFD_RELOC_VTABLE_INHERIT, R_ARM_GNU_VTINHERIT },
  {BFD_RELOC_VTABLE_ENTRY, R_ARM_GNU_VTENTRY },
  {BFD_RELOC_NONE, R_ARM_SBREL32,},
  {BFD_RELOC_NONE, R_ARM_AMP_VCALL9,},
  {BFD_RELOC_THUMB_PCREL_BRANCH12, R_ARM_THM_PC11,},
  {BFD_RELOC_THUMB_PCREL_BRANCH9, R_ARM_THM_PC9,}
};

static reloc_howto_type *
elf32_arm_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
     i < sizeof (elf32_arm_reloc_map) / sizeof (struct elf32_arm_reloc_map);
       i++)
    {
      if (elf32_arm_reloc_map[i].bfd_reloc_val == code)
	return &elf32_arm_howto_table[elf32_arm_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* The thumb form of a long branch is a bit finicky, because the offset
   encoding is split over two fields, each in it's own instruction. They
   can occur in any order. So given a thumb form of long branch, and an 
   offset, insert the offset into the thumb branch and return finished
   instruction. 

   It takes two thumb instructions to encode the target address. Each has 
   11 bits to invest. The upper 11 bits are stored in one (identifed by
   H-0.. see below), the lower 11 bits are stored in the other (identified 
   by H-1). 

   Combine together and shifted left by 1 (it's a half word address) and 
   there you have it.

   Op: 1111 = F,
   H-0, upper address-0 = 000
   Op: 1111 = F,
   H-1, lower address-0 = 800

   They can be ordered either way, but the arm tools I've seen always put 
   the lower one first. It probably doesn't matter. krk@cygnus.com

   XXX:  Actually the order does matter.  The second instruction (H-1)
   moves the computed address into the PC, so it must be the second one
   in the sequence.  The problem, however is that whilst little endian code
   stores the instructions in HI then LOW order, big endian code does the
   reverse.  nickc@cygnus.com  */

#define LOW_HI_ORDER 0xF800F000
#define HI_LOW_ORDER 0xF000F800

static insn32
insert_thumb_branch (br_insn, rel_off)
     insn32 br_insn;
     int rel_off;
{
  unsigned int low_bits;
  unsigned int high_bits;


  BFD_ASSERT ((rel_off & 1) != 1);

  rel_off >>= 1;		/* half word aligned address */
  low_bits = rel_off & 0x000007FF;	/* the bottom 11 bits */
  high_bits = (rel_off >> 11) & 0x000007FF;	/* the top 11 bits */

  if ((br_insn & LOW_HI_ORDER) == LOW_HI_ORDER)
    br_insn = LOW_HI_ORDER | (low_bits << 16) | high_bits;
  else if ((br_insn & HI_LOW_ORDER) == HI_LOW_ORDER)
    br_insn = HI_LOW_ORDER | (high_bits << 16) | low_bits;
  else
    abort ();			/* error - not a valid branch instruction form */

  /* FIXME: abort is probably not the right call. krk@cygnus.com */

  return br_insn;
}

/* Thumb code calling an ARM function */
int
elf32_thumb_to_arm_stub (info, name, input_bfd, output_bfd, input_section,
			 hit_data, sym_sec, offset, addend, val)
     struct bfd_link_info *info;
     char *name;
     bfd *input_bfd;
     bfd *output_bfd;
     asection *input_section;
     bfd_byte *hit_data;
     asection *sym_sec;
     int offset;
     int addend;
     bfd_vma val;
{
  asection *s = 0;
  long int my_offset;
  unsigned long int tmp;
  long int ret_offset;
  struct elf_link_hash_entry *myh;
  struct elf32_arm_link_hash_table *globals;

  myh = find_thumb_glue (info, name, input_bfd);
  if (myh == NULL)
    return false;

  globals = elf32_arm_hash_table (info);

  BFD_ASSERT (globals != NULL);
  BFD_ASSERT (globals->bfd_of_glue_owner != NULL);

  my_offset = myh->root.u.def.value;

  s = bfd_get_section_by_name (globals->bfd_of_glue_owner,
			       THUMB2ARM_GLUE_SECTION_NAME);

  BFD_ASSERT (s != NULL);
  BFD_ASSERT (s->contents != NULL);
  BFD_ASSERT (s->output_section != NULL);

  if ((my_offset & 0x01) == 0x01)
    {
      if (sym_sec != NULL
	  && sym_sec->owner != NULL
	  && !INTERWORK_FLAG (sym_sec->owner))
	{
	  _bfd_error_handler
	    (_ ("%s(%s): warning: interworking not enabled."),
	     bfd_get_filename (sym_sec->owner), name);
	  _bfd_error_handler
	    (_ ("  first occurrence: %s: thumb call to arm"),
	     bfd_get_filename (input_bfd));

	  return false;
	}

      --my_offset;
      myh->root.u.def.value = my_offset;

      bfd_put_16 (output_bfd, t2a1_bx_pc_insn,
		  s->contents + my_offset);

      bfd_put_16 (output_bfd, t2a2_noop_insn,
		  s->contents + my_offset + 2);

      ret_offset =
	((bfd_signed_vma) val)	/* Address of destination of the stub */
	- ((bfd_signed_vma)
	   (s->output_offset	/* Offset from the start of the current section to the start of the stubs.  */
	    + my_offset		/* Offset of the start of this stub from the start of the stubs.  */
	    + s->output_section->vma)	/* Address of the start of the current section.  */
	   + 4			/* The branch instruction is 4 bytes into the stub.  */
	   + 8);		/* ARM branches work from the pc of the instruction + 8.  */

      bfd_put_32 (output_bfd,
		  t2a3_b_insn | ((ret_offset >> 2) & 0x00FFFFFF),
		  s->contents + my_offset + 4);
    }

  BFD_ASSERT (my_offset <= globals->thumb_glue_size);

  /* Now go back and fix up the original BL insn to point
     to here.  */
  ret_offset =
    s->output_offset
    + my_offset
    - (input_section->output_offset
       + offset + addend)
    - 4;

  tmp = bfd_get_32 (input_bfd, hit_data
		    - input_section->vma);

  bfd_put_32 (output_bfd,
	      insert_thumb_branch (tmp, ret_offset),
	      hit_data - input_section->vma);

  return true;
}

/* Arm code calling a Thumb function */
int
elf32_arm_to_thumb_stub (info, name, input_bfd, output_bfd, input_section,
			 hit_data, sym_sec, offset, addend, val)

     struct bfd_link_info *info;
     char *name;
     bfd *input_bfd;
     bfd *output_bfd;
     asection *input_section;
     bfd_byte *hit_data;
     asection *sym_sec;
     int offset;
     int addend;
     bfd_vma val;
{
  unsigned long int tmp;
  long int my_offset;
  asection *s;
  long int ret_offset;
  struct elf_link_hash_entry *myh;
  struct elf32_arm_link_hash_table *globals;

  myh = find_arm_glue (info, name, input_bfd);
  if (myh == NULL)
    return false;

  globals = elf32_arm_hash_table (info);

  BFD_ASSERT (globals != NULL);
  BFD_ASSERT (globals->bfd_of_glue_owner != NULL);

  my_offset = myh->root.u.def.value;
  s = bfd_get_section_by_name (globals->bfd_of_glue_owner,
			       ARM2THUMB_GLUE_SECTION_NAME);
  BFD_ASSERT (s != NULL);
  BFD_ASSERT (s->contents != NULL);
  BFD_ASSERT (s->output_section != NULL);

  if ((my_offset & 0x01) == 0x01)
    {
      if (sym_sec != NULL
	  && sym_sec->owner != NULL
	  && !INTERWORK_FLAG (sym_sec->owner))
	{
	  _bfd_error_handler
	    (_ ("%s(%s): warning: interworking not enabled."),
	     bfd_get_filename (sym_sec->owner), name);
	  _bfd_error_handler
	    (_ ("  first occurrence: %s: arm call to thumb"),
	     bfd_get_filename (input_bfd));
	}
      --my_offset;
      myh->root.u.def.value = my_offset;

      bfd_put_32 (output_bfd, a2t1_ldr_insn,
		  s->contents + my_offset);

      bfd_put_32 (output_bfd, a2t2_bx_r12_insn,
		  s->contents + my_offset + 4);

      /* It's a thumb address.  Add the low order bit.  */
      bfd_put_32 (output_bfd, val | a2t3_func_addr_insn,
		  s->contents + my_offset + 8);
    }

  BFD_ASSERT (my_offset <= globals->arm_glue_size);

  tmp = bfd_get_32 (input_bfd, hit_data);
  tmp = tmp & 0xFF000000;

  /* Somehow these are both 4 too far, so subtract 8. */
  ret_offset = s->output_offset
    + my_offset
    + s->output_section->vma
    - (input_section->output_offset
       + input_section->output_section->vma
       + offset + addend)
    - 8;

  tmp = tmp | ((ret_offset >> 2) & 0x00FFFFFF);

  bfd_put_32 (output_bfd, tmp, hit_data
	      - input_section->vma);


  return true;
}

/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
elf32_arm_final_link_relocate (howto, input_bfd, output_bfd,
			       input_section, contents, offset, value,
			       addend, info, sym_sec, sym_name, sym_flags)
     reloc_howto_type *howto;
     bfd *input_bfd;
     bfd *output_bfd;
     asection *input_section;
     bfd_byte *contents;
     bfd_vma offset;
     bfd_vma value;
     bfd_vma addend;
     struct bfd_link_info *info;
     asection *sym_sec;
     const char *sym_name;
     unsigned char sym_flags;
{
  unsigned long r_type = howto->type;
  bfd_byte *hit_data = contents + offset;

  switch (r_type)
    {

    case R_ARM_NONE:
      return bfd_reloc_ok;

    case R_ARM_PC24:
      /* Arm B/BL instruction */

#ifdef USE_REL
      addend = (bfd_get_32 (input_bfd, hit_data) & howto->src_mask);
#endif
      /* check for arm calling thumb function */
      if (sym_flags == STT_ARM_TFUNC)
	{
	  elf32_arm_to_thumb_stub (info, sym_name, input_bfd, output_bfd,
		   input_section, hit_data, sym_sec, offset, addend, value);
	  return bfd_reloc_ok;
	}

      value = value + addend;
      value -= (input_section->output_section->vma
		+ input_section->output_offset + 8);
      value -= offset;
      value = value >> howto->rightshift;

      value &= 0xffffff;
      value |= (bfd_get_32 (input_bfd, hit_data) & 0xff000000);
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS32:
#ifdef USE_REL
      addend = (bfd_get_32 (input_bfd, hit_data) & howto->src_mask);
#endif
      value += addend;
      if (sym_flags == STT_ARM_TFUNC)
	value |= 1;
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_REL32:
#ifdef USE_REL
      addend = (bfd_get_32 (input_bfd, hit_data) & howto->src_mask);
#endif
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value += addend;

      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS8:
#ifdef USE_REL
     addend = (bfd_get_32 (input_bfd, hit_data) & howto->src_mask);
#endif
      value += addend;
      if ((long) value > 0x7f || (long) value < -0x80)
	return bfd_reloc_overflow;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS16:
#ifdef USE_REL
      addend = (bfd_get_32 (input_bfd, hit_data) & howto->src_mask);
#endif
      value += addend;

      if ((long) value > 0x7fff || (long) value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_ABS12:
      /* Support ldr and str instruction for the arm */
      /* Also thumb b (unconditional branch) */
#ifdef USE_REL
      addend = (bfd_get_32 (input_bfd, hit_data) & howto->src_mask);
#endif
      value += addend;

      if ((long) value > 0x7ff || (long) value < -0x800)
	return bfd_reloc_overflow;

      value |= (bfd_get_32 (input_bfd, hit_data) & 0xfffff000);
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_ARM_THM_ABS5:
      /* Support ldr and str instructions for the thumb. */
      value += addend;

      if ((long) value > 0x1f || (long) value < -0x10)
	return bfd_reloc_overflow;

      value |= bfd_get_16 (input_bfd, hit_data) & 0xf82f;
      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;


    case R_ARM_THM_PC22:
      /* thumb BL (branch long instruction). */
      {
	bfd_vma relocation;
	boolean overflow = false;
	bfd_vma insn = bfd_get_32 (input_bfd, hit_data);
	bfd_vma src_mask = 0x007FFFFE;
	bfd_signed_vma reloc_signed_max = (1 << (howto->bitsize - 1)) - 1;
	bfd_signed_vma reloc_signed_min = ~reloc_signed_max;
	bfd_vma check;
	bfd_signed_vma signed_check;
	bfd_vma add;
	bfd_signed_vma signed_add;

        /* If it's not a call to thumb, assume call to arm */
	if (sym_flags != STT_ARM_TFUNC)
	  {
	    if (elf32_thumb_to_arm_stub
		(info, sym_name, input_bfd, output_bfd, input_section,
		 hit_data, sym_sec, offset, addend, value))
	      return bfd_reloc_ok;
	    else
	      return bfd_reloc_dangerous;
	  }

	relocation = value + addend;
	relocation -= (input_section->output_section->vma + input_section->output_offset);
	relocation -= offset;

	check = relocation >> howto->rightshift;

	/* If this is a signed value, the rightshift just dropped
	   leading 1 bits (assuming twos complement).  */
	if ((bfd_signed_vma) relocation >= 0)
	  signed_check = check;
	else
	  signed_check = (check | ((bfd_vma) - 1 & ~((bfd_vma) - 1 >> howto->rightshift)));

	/* Get the value from the object file.  */
	if (bfd_big_endian (input_bfd))
	  add = (((insn) & 0x07ff0000) >> 4) | (((insn) & 0x7ff) << 1);
	else
	  add = ((((insn) & 0x7ff) << 12) | (((insn) & 0x07ff0000) >> 15));

	/* Get the value from the object file with an appropriate sign.
	   The expression involving howto->src_mask isolates the upper
	   bit of src_mask.  If that bit is set in the value we are
	   adding, it is negative, and we subtract out that number times
	   two.  If src_mask includes the highest possible bit, then we
	   can not get the upper bit, but that does not matter since
	   signed_add needs no adjustment to become negative in that case.  */

	signed_add = add;

	if ((add & (((~src_mask) >> 1) & src_mask)) != 0)
	  signed_add -= (((~src_mask) >> 1) & src_mask) << 1;

	/* Add the value from the object file, shifted so that it is a
	   straight number.  */
	/* howto->bitpos == 0 */

	signed_check += signed_add;
	relocation += signed_add;

	/* Assumes two's complement.  */
	if (signed_check > reloc_signed_max
	    || signed_check < reloc_signed_min)
	  overflow = true;

	/* Put RELOCATION into the correct bits:  */

	if (bfd_big_endian (input_bfd))
	  relocation = (((relocation & 0xffe) >> 1) | ((relocation << 4) & 0x07ff0000));
	else
	  relocation = (((relocation & 0xffe) << 15) | ((relocation >> 12) & 0x7ff));

	/* Add RELOCATION to the correct bits of X:  */
	insn = ((insn & ~howto->dst_mask) | relocation);

	/* Put the relocated value back in the object file:  */
	bfd_put_32 (input_bfd, insn, hit_data);

	return (overflow ? bfd_reloc_overflow : bfd_reloc_ok);
      }
      break;

    case R_ARM_GNU_VTINHERIT:
    case R_ARM_GNU_VTENTRY:
      return bfd_reloc_ok;

    case R_ARM_SBREL32:
      return bfd_reloc_notsupported;

    case R_ARM_AMP_VCALL9:
      return bfd_reloc_notsupported;

    case R_ARM_RSBREL32:
      return bfd_reloc_notsupported;

    case R_ARM_THM_RPC22:
      return bfd_reloc_notsupported;

    case R_ARM_RREL32:
      return bfd_reloc_notsupported;

    case R_ARM_RABS32:
      return bfd_reloc_notsupported;

    case R_ARM_RPC24:
      return bfd_reloc_notsupported;

    case R_ARM_RBASE:
      return bfd_reloc_notsupported;

    default:
      return bfd_reloc_notsupported;
    }
}


/* Relocate an ARM ELF section.  */
static boolean
elf32_arm_relocate_section (output_bfd, info, input_bfd, input_section,
			    contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel, *relend;
  const char *name;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);

      if (r_type == R_ARM_GNU_VTENTRY
          || r_type == R_ARM_GNU_VTINHERIT )
        continue;

      howto = elf32_arm_howto_table + r_type;

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;
	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections[r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else
	    {
	      if (!((*info->callbacks->undefined_symbol)
		    (info, h->root.root.string, input_bfd,
		     input_section, rel->r_offset)))
		return false;
	      relocation = 0;
	    }
	}

      if (h != NULL)
	name = h->root.root.string;
      else
	{
	  name = (bfd_elf_string_from_elf_section
		  (input_bfd, symtab_hdr->sh_link, sym->st_name));
	  if (name == NULL || *name == '\0')
	    name = bfd_section_name (input_bfd, sec);
	}
      
      r = elf32_arm_final_link_relocate (howto, input_bfd, output_bfd,
					 input_section,
					 contents, rel->r_offset,
					 relocation, rel->r_addend,
					 info, sec, name,
					 (h ? ELF_ST_TYPE (h->type) :
					  ELF_ST_TYPE (sym->st_info)));

      if (r != bfd_reloc_ok)
	{
	  const char * msg = (const char *) 0;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (!((*info->callbacks->reloc_overflow)
		    (info, name, howto->name, (bfd_vma) 0,
		     input_bfd, input_section, rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_undefined:
	      if (!((*info->callbacks->undefined_symbol)
		    (info, name, input_bfd, input_section,
		     rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = _ ("internal error: out of range error");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = _ ("internal error: unsupported relocation error");
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = _ ("internal error: dangerous error");
	      goto common_error;

	    default:
	      msg = _ ("internal error: unknown error");
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, msg, name, input_bfd, input_section,
		     rel->r_offset)))
		return false;
	      break;
	    }
	}
    }

  return true;
}

/* Function to keep ARM specific flags in the ELF header. */
static boolean
elf32_arm_set_private_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  if (elf_flags_init (abfd)
      && elf_elfheader (abfd)->e_flags != flags)
    {
      if (flags & EF_INTERWORK)
	_bfd_error_handler (_ ("\
Warning: Not setting interwork flag of %s since it has already been specified as non-interworking"),
			    bfd_get_filename (abfd));
      else
	_bfd_error_handler (_ ("\
Warning: Clearing the interwork flag of %s due to outside request"),
			    bfd_get_filename (abfd));
    }
  else
    {
      elf_elfheader (abfd)->e_flags = flags;
      elf_flags_init (abfd) = true;
    }

  return true;
}

/* Copy backend specific data from one object module to another */
static boolean
elf32_arm_copy_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  flagword in_flags;
  flagword out_flags;

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  in_flags = elf_elfheader (ibfd)->e_flags;
  out_flags = elf_elfheader (obfd)->e_flags;

  if (elf_flags_init (obfd) && in_flags != out_flags)
    {
      /* Cannot mix PIC and non-PIC code.  */
      if ((in_flags & EF_PIC) != (out_flags & EF_PIC))
	return false;

      /* Cannot mix APCS26 and APCS32 code.  */
      if ((in_flags & EF_APCS_26) != (out_flags & EF_APCS_26))
	return false;

      /* Cannot mix float APCS and non-float APCS code.  */
      if ((in_flags & EF_APCS_FLOAT) != (out_flags & EF_APCS_FLOAT))
	return false;

      /* If the src and dest have different interworking flags
         then turn off the interworking bit.  */
      if ((in_flags & EF_INTERWORK) != (out_flags & EF_INTERWORK))
	{
	  if (out_flags & EF_INTERWORK)
	    _bfd_error_handler (_ ("\
Warning: Clearing the interwork flag in %s because non-interworking code in %s has been linked with it"),
			  bfd_get_filename (obfd), bfd_get_filename (ibfd));

	  in_flags &= ~EF_INTERWORK;
	}
    }

  elf_elfheader (obfd)->e_flags = in_flags;
  elf_flags_init (obfd) = true;

  return true;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */
static boolean
elf32_arm_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  flagword out_flags;
  flagword in_flags;

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  /* The input BFD must have had its flags initialised.  */
  /* The following seems bogus to me -- The flags are initialized in
     the assembler but I don't think an elf_flags_init field is
     written into the object */
  /* BFD_ASSERT (elf_flags_init (ibfd)); */

  in_flags = elf_elfheader (ibfd)->e_flags;
  out_flags = elf_elfheader (obfd)->e_flags;

  if (!elf_flags_init (obfd))
    {
      /* If the input is the default architecture then do not
         bother setting the flags for the output architecture,
         instead allow future merges to do this.  If no future
         merges ever set these flags then they will retain their
         unitialised values, which surprise surprise, correspond
         to the default values.  */
      if (bfd_get_arch_info (ibfd)->the_default)
	return true;

      elf_flags_init (obfd) = true;
      elf_elfheader (obfd)->e_flags = in_flags;

      if (bfd_get_arch (obfd) == bfd_get_arch (ibfd)
	  && bfd_get_arch_info (obfd)->the_default)
	return bfd_set_arch_mach (obfd, bfd_get_arch (ibfd), bfd_get_mach (ibfd));

      return true;
    }

  /* Check flag compatibility.  */
  if (in_flags == out_flags)
    return true;

  /* Complain about various flag mismatches.  */

  if ((in_flags & EF_APCS_26) != (out_flags & EF_APCS_26))
    _bfd_error_handler (_ ("\
Error: %s compiled for APCS-%d, whereas %s is compiled for APCS-%d"),
			bfd_get_filename (ibfd),
			in_flags & EF_APCS_26 ? 26 : 32,
			bfd_get_filename (obfd),
			out_flags & EF_APCS_26 ? 26 : 32);

  if ((in_flags & EF_APCS_FLOAT) != (out_flags & EF_APCS_FLOAT))
    _bfd_error_handler (_ ("\
Error: %s passes floats in %s registers, whereas %s passes them in %s registers"),
			bfd_get_filename (ibfd),
		     in_flags & EF_APCS_FLOAT ? _ ("float") : _ ("integer"),
			bfd_get_filename (obfd),
		      out_flags & EF_APCS_26 ? _ ("float") : _ ("integer"));

  if ((in_flags & EF_PIC) != (out_flags & EF_PIC))
    _bfd_error_handler (_ ("\
Error: %s is compiled as position %s code, whereas %s is not"),
			bfd_get_filename (ibfd),
		    in_flags & EF_PIC ? _ ("independent") : _ ("dependent"),
			bfd_get_filename (obfd));

  /* Interworking mismatch is only a warning. */
  if ((in_flags & EF_INTERWORK) != (out_flags & EF_INTERWORK))
    {
      _bfd_error_handler (_ ("\
Warning: %s %s interworking, whereas %s %s"),
			  bfd_get_filename (ibfd),
	  in_flags & EF_INTERWORK ? _ ("supports") : _ ("does not support"),
			  bfd_get_filename (obfd),
		    out_flags & EF_INTERWORK ? _ ("does not") : _ ("does"));
      return true;
    }

  return false;
}

/* Display the flags field */
static boolean
elf32_arm_print_private_bfd_data (abfd, ptr)
     bfd *abfd;
     PTR ptr;
{
  FILE *file = (FILE *) ptr;

  BFD_ASSERT (abfd != NULL && ptr != NULL);

  /* Print normal ELF private data.  */
  _bfd_elf_print_private_bfd_data (abfd, ptr);

  /* Ignore init flag - it may not be set, despite the flags field containing valid data.  */

  /* xgettext:c-format */
  fprintf (file, _ ("private flags = %lx:"), elf_elfheader (abfd)->e_flags);

  if (elf_elfheader (abfd)->e_flags & EF_INTERWORK)
    fprintf (file, _ (" [interworking enabled]"));
  else
    fprintf (file, _ (" [interworking not enabled]"));

  if (elf_elfheader (abfd)->e_flags & EF_APCS_26)
    fprintf (file, _ (" [APCS-26]"));
  else
    fprintf (file, _ (" [APCS-32]"));

  if (elf_elfheader (abfd)->e_flags & EF_APCS_FLOAT)
    fprintf (file, _ (" [floats passed in float registers]"));
  else
    fprintf (file, _ (" [floats passed in integer registers]"));

  if (elf_elfheader (abfd)->e_flags & EF_PIC)
    fprintf (file, _ (" [position independent]"));
  else
    fprintf (file, _ (" [absolute position]"));

  fputc ('\n', file);

  return true;
}

static int
elf32_arm_get_symbol_type (elf_sym, type)
     Elf_Internal_Sym * elf_sym;
     int type;
{
  if (ELF_ST_TYPE (elf_sym->st_info) == STT_ARM_TFUNC)
    return ELF_ST_TYPE (elf_sym->st_info);
  else
    return type;
}
    
static asection *
elf32_arm_gc_mark_hook (abfd, info, rel, h, sym)
       bfd *abfd;
       struct bfd_link_info *info;
       Elf_Internal_Rela *rel;
       struct elf_link_hash_entry *h;
       Elf_Internal_Sym *sym;
{
  if (h != NULL)
    {
      switch (ELF32_R_TYPE (rel->r_info))
      {
      case R_ARM_GNU_VTINHERIT:
      case R_ARM_GNU_VTENTRY:
        break;

      default:
        switch (h->root.type)
          {
          case bfd_link_hash_defined:
          case bfd_link_hash_defweak:
            return h->root.u.def.section;

          case bfd_link_hash_common:
            return h->root.u.c.p->section;
          }
       }
     }
   else
     {
       if (!(elf_bad_symtab (abfd)
           && ELF_ST_BIND (sym->st_info) != STB_LOCAL)
         && ! ((sym->st_shndx <= 0 || sym->st_shndx >= SHN_LORESERVE)
                && sym->st_shndx != SHN_COMMON))
          {
            return bfd_section_from_elf_index (abfd, sym->st_shndx);
          }
      }
  return NULL;
}

static boolean
elf32_arm_gc_sweep_hook (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  /* we don't use got and plt entries for armelf */
  return true;
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */
 
static boolean
elf32_arm_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
 
  if (info->relocateable)
    return true;
 
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end = sym_hashes + symtab_hdr->sh_size/sizeof(Elf32_External_Sym);
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;
 
  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;
 
      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
        h = NULL;
      else
        h = sym_hashes[r_symndx - symtab_hdr->sh_info];
 
      switch (ELF32_R_TYPE (rel->r_info))
        {
        /* This relocation describes the C++ object vtable hierarchy.
           Reconstruct it for later use during GC.  */
        case R_ARM_GNU_VTINHERIT:
          if (!_bfd_elf32_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
            return false;
          break;
 
        /* This relocation describes which C++ vtable entries are actually
           used.  Record for later use during GC.  */
        case R_ARM_GNU_VTENTRY:
          if (!_bfd_elf32_gc_record_vtentry (abfd, sec, h, rel->r_addend))
            return false;
          break;
        }
    }
 
  return true;
}

       
/* Find the nearest line to a particular section and offset, for error
   reporting.   This code is a duplicate of the code in elf.c, except
   that it also accepts STT_ARM_TFUNC as a symbol that names a function. */

boolean
elf32_arm_find_nearest_line
  (abfd, section, symbols, offset, filename_ptr, functionname_ptr, line_ptr)
     bfd *          abfd;
     asection *     section;
     asymbol **     symbols;
     bfd_vma        offset;
     CONST char **  filename_ptr;
     CONST char **  functionname_ptr;
     unsigned int * line_ptr;
{
  boolean      found;
  const char * filename;
  asymbol *    func;
  bfd_vma      low_func;
  asymbol **   p;

  if (_bfd_dwarf2_find_nearest_line (abfd, section, symbols, offset,
				     filename_ptr, functionname_ptr, 
				     line_ptr))
    return true;

  if (! _bfd_stab_section_find_nearest_line (abfd, symbols, section, offset,
					     &found, filename_ptr,
					     functionname_ptr, line_ptr,
					     &elf_tdata (abfd)->line_info))
    return false;
  
  if (found)
    return true;

  if (symbols == NULL)
    return false;

  filename = NULL;
  func = NULL;
  low_func = 0;

  for (p = symbols; *p != NULL; p++)
    {
      elf_symbol_type *q;

      q = (elf_symbol_type *) *p;

      if (bfd_get_section (&q->symbol) != section)
	continue;

      switch (ELF_ST_TYPE (q->internal_elf_sym.st_info))
	{
	default:
	  break;
	case STT_FILE:
	  filename = bfd_asymbol_name (&q->symbol);
	  break;
	case STT_NOTYPE:
	case STT_FUNC:
	case STT_ARM_TFUNC:
	  if (q->symbol.section == section
	      && q->symbol.value >= low_func
	      && q->symbol.value <= offset)
	    {
	      func = (asymbol *) q;
	      low_func = q->symbol.value;
	    }
	  break;
	}
    }

  if (func == NULL)
    return false;

  *filename_ptr = filename;
  *functionname_ptr = bfd_asymbol_name (func);
  *line_ptr = 0;
  
  return true;
}

#define ELF_ARCH			bfd_arch_arm
#define ELF_MACHINE_CODE		EM_ARM

#define bfd_elf32_bfd_reloc_type_lookup 	elf32_arm_reloc_type_lookup
#define elf_backend_relocate_section    	elf32_arm_relocate_section
#define bfd_elf32_bfd_copy_private_bfd_data 	elf32_arm_copy_private_bfd_data
#define bfd_elf32_bfd_merge_private_bfd_data 	elf32_arm_merge_private_bfd_data
#define bfd_elf32_bfd_set_private_flags		elf32_arm_set_private_flags
#define bfd_elf32_bfd_print_private_bfd_data	elf32_arm_print_private_bfd_data
#define bfd_elf32_bfd_link_hash_table_create    elf32_arm_link_hash_table_create
#define bfd_elf32_find_nearest_line	        elf32_arm_find_nearest_line
#define elf_backend_get_symbol_type             elf32_arm_get_symbol_type
#define elf_backend_gc_mark_hook                elf32_arm_gc_mark_hook
#define elf_backend_gc_sweep_hook               elf32_arm_gc_sweep_hook
#define elf_backend_check_relocs                elf32_arm_check_relocs

#define elf_backend_can_gc_sections 1

#include "elf32-target.h"
