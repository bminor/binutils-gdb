# This shell script emits a C file. -*- C -*-
#   Copyright 2006 Free Software Foundation, Inc.
#
# This file is part of GLD, the Gnu Linker.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
#

# This file is sourced from elf32.em, and defines extra spu specific
# features.
#
cat >>e${EMULATION_NAME}.c <<EOF
#include "ldctor.h"
#include "elf32-spu.h"

/* Non-zero if no overlay processing should be done.  */
static int no_overlays = 0;

/* Non-zero if we want stubs on all calls out of overlay regions.  */
static int non_overlay_stubs = 0;

/* Whether to emit symbols for stubs.  */
static int emit_stub_syms = 0;

/* Range of valid addresses for loadable sections.  */
static bfd_vma local_store_lo = 0;
static bfd_vma local_store_hi = 0x3ffff;

extern void *_binary_builtin_ovl_mgr_start;
extern void *_binary_builtin_ovl_mgr_end;

static const struct _ovl_stream ovl_mgr_stream = {
  &_binary_builtin_ovl_mgr_start,
  &_binary_builtin_ovl_mgr_end
};

static asection *toe = NULL;


static int
is_spu_target (void)
{
  extern const bfd_target bfd_elf32_spu_vec;

  return link_info.hash->creator == &bfd_elf32_spu_vec;
}

/* Create our note section.  */

static void
spu_after_open (void)
{
  if (is_spu_target ()
      && !link_info.relocatable
      && link_info.input_bfds != NULL
      && !spu_elf_create_sections (output_bfd, &link_info))
    einfo ("%X%P: can not create note section: %E\n");

  gld${EMULATION_NAME}_after_open ();
}

/* Add section S at the end of output section OUTPUT_NAME.

   Really, we should be duplicating ldlang.c map_input_to_output_sections
   logic here, ie. using the linker script to find where the section
   goes.  That's rather a lot of code, and we don't want to run
   map_input_to_output_sections again because most sections are already
   mapped.  So cheat, and put the section in a fixed place, ignoring any
   attempt via a linker script to put .stub, .ovtab, and built-in
   overlay manager code somewhere else.  */

static void
spu_place_special_section (asection *s, const char *output_name)
{
  lang_output_section_statement_type *os;

  os = lang_output_section_find (output_name);
  if (os == NULL)
    {
      const char *save = s->name;
      s->name = output_name;
      gld${EMULATION_NAME}_place_orphan (s);
      s->name = save;
    }
  else
    lang_add_section (&os->children, s, os);

  s->output_section->size += s->size;
}

/* Load built-in overlay manager, and tweak overlay section alignment.  */

static void
spu_elf_load_ovl_mgr (void)
{
  lang_output_section_statement_type *os;
  struct elf_link_hash_entry *h;

  h = elf_link_hash_lookup (elf_hash_table (&link_info),
			    "__ovly_load", FALSE, FALSE, FALSE);

  if (h != NULL
      && (h->root.type == bfd_link_hash_defined
	  || h->root.type == bfd_link_hash_defweak)
      && h->def_regular)
    {
      /* User supplied __ovly_load.  */
    }
  else
    {
      lang_input_statement_type *ovl_is;

      ovl_is = lang_add_input_file ("builtin ovl_mgr",
				    lang_input_file_is_file_enum,
				    NULL);

      if (!spu_elf_open_builtin_lib (&ovl_is->the_bfd, &ovl_mgr_stream))
	einfo ("%X%P: can not open built-in overlay manager: %E\n");
      else
	{
	  asection *in;

	  if (!load_symbols (ovl_is, NULL))
	    einfo ("%X%P: can not load built-in overlay manager: %E\n");

	  /* Map overlay manager sections to output sections.  */
	  for (in = ovl_is->the_bfd->sections; in != NULL; in = in->next)
	    if ((in->flags & (SEC_ALLOC | SEC_LOAD))
		== (SEC_ALLOC | SEC_LOAD))
	      spu_place_special_section (in, ".text");
	}
    }

  /* Ensure alignment of overlay sections is sufficient.  */
  for (os = &lang_output_section_statement.head->output_section_statement;
       os != NULL;
       os = os->next)
    if (os->bfd_section != NULL
	&& spu_elf_section_data (os->bfd_section) != NULL
	&& spu_elf_section_data (os->bfd_section)->ovl_index != 0)
      {
	if (os->bfd_section->alignment_power < 4)
	  os->bfd_section->alignment_power = 4;

	/* Also ensure size rounds up.  */
	os->block_value = 16;
      }
}

/* Go find if we need to do anything special for overlays.  */

static void
spu_before_allocation (void)
{
  if (is_spu_target ()
      && !link_info.relocatable
      && !no_overlays)
    {
      /* Size the sections.  This is premature, but we need to know the
	 rough layout so that overlays can be found.  */
      expld.phase = lang_mark_phase_enum;
      expld.dataseg.phase = exp_dataseg_none;
      one_lang_size_sections_pass (NULL, TRUE);

      /* Find overlays by inspecting section vmas.  */
      if (spu_elf_find_overlays (output_bfd, &link_info))
	{
	  asection *stub, *ovtab;

	  if (!spu_elf_size_stubs (output_bfd, &link_info, non_overlay_stubs,
				   &stub, &ovtab, &toe))
	    einfo ("%X%P: can not size overlay stubs: %E\n");

	  if (stub != NULL)
	    {
	      spu_place_special_section (stub, ".text");
	      spu_place_special_section (ovtab, ".data");
	      spu_place_special_section (toe, ".toe");

	      spu_elf_load_ovl_mgr ();
	    }
	}

      /* We must not cache anything from the preliminary sizing.  */
      lang_reset_memory_regions ();
    }

  gld${EMULATION_NAME}_before_allocation ();
}

/* Final emulation specific call.  */

static void
gld${EMULATION_NAME}_finish (void)
{
  int need_laying_out;

  need_laying_out = bfd_elf_discard_info (output_bfd, &link_info);

  gld${EMULATION_NAME}_map_segments (need_laying_out);

  if (is_spu_target () && local_store_lo < local_store_hi)
    {
      asection *s;

      s = spu_elf_check_vma (output_bfd, local_store_lo, local_store_hi);
      if (s != NULL)
	einfo ("%X%P: %A exceeds local store range\n", s);
    }

  if (toe != NULL
      && !spu_elf_build_stubs (&link_info,
			       emit_stub_syms || link_info.emitrelocations,
			       toe))
    einfo ("%X%P: can not build overlay stubs: %E\n");

  finish_default ();
}

EOF

# Define some shell vars to insert bits of code into the standard elf
# parse_args and list_options functions.
#
PARSE_AND_LIST_PROLOGUE='
#define OPTION_SPU_PLUGIN		301
#define OPTION_SPU_NO_OVERLAYS		(OPTION_SPU_PLUGIN + 1)
#define OPTION_SPU_STUB_SYMS		(OPTION_SPU_NO_OVERLAYS + 1)
#define OPTION_SPU_NON_OVERLAY_STUBS	(OPTION_SPU_STUB_SYMS + 1)
#define OPTION_SPU_LOCAL_STORE		(OPTION_SPU_NON_OVERLAY_STUBS + 1)
'

PARSE_AND_LIST_LONGOPTS='
  { "plugin", no_argument, NULL, OPTION_SPU_PLUGIN },
  { "no-overlays", no_argument, NULL, OPTION_SPU_NO_OVERLAYS },
  { "emit-stub-syms", no_argument, NULL, OPTION_SPU_STUB_SYMS },
  { "extra-overlay-stubs", no_argument, NULL, OPTION_SPU_NON_OVERLAY_STUBS },
  { "local-store", required_argument, NULL, OPTION_SPU_LOCAL_STORE },
'

PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("\
  --plugin              Make SPU plugin.\n\
  --no-overlays         No overlay handling.\n\
  --emit-stub-syms      Add symbols on overlay call stubs.\n\
  --extra-overlay-stubs Add stubs on all calls out of overlay regions.\n\
  --local-store=lo:hi   Valid address range.\n"
		   ));
'

PARSE_AND_LIST_ARGS_CASES='
    case OPTION_SPU_PLUGIN:
      spu_elf_plugin (1);
      break;

    case OPTION_SPU_NO_OVERLAYS:
      no_overlays = 1;
      break;

    case OPTION_SPU_STUB_SYMS:
      emit_stub_syms = 1;
      break;

    case OPTION_SPU_NON_OVERLAY_STUBS:
      non_overlay_stubs = 1;
      break;

    case OPTION_SPU_LOCAL_STORE:
      {
	char *end;
	local_store_lo = strtoul (optarg, &end, 0);
	if (*end == '\'':'\'')
	  {
	    local_store_hi = strtoul (end + 1, &end, 0);
	    if (*end == 0)
	      break;
	  }
	einfo (_("%P%F: invalid --local-store address range `%s'\''\n"), optarg);
      }
      break;
'

LDEMUL_AFTER_OPEN=spu_after_open
LDEMUL_BEFORE_ALLOCATION=spu_before_allocation
LDEMUL_FINISH=gld${EMULATION_NAME}_finish
