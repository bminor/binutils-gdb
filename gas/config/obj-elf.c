/* ELF object file format
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with GAS; see the file COPYING.  If not, write
   to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

/* HP PA-RISC support was contributed by the Center for Software Science
   at the University of Utah.  */

#include "as.h"
#include "aout/stab_gnu.h"
#include "obstack.h"

static void obj_elf_stab PARAMS ((int what));
static void obj_elf_line PARAMS ((void));
static void obj_elf_desc PARAMS ((void));
static void obj_elf_version PARAMS ((void));
static void obj_elf_section PARAMS ((int));
static void obj_elf_size PARAMS ((void));
static void obj_elf_type PARAMS ((void));
static void obj_elf_ident PARAMS ((void));

const pseudo_typeS obj_pseudo_table[] = {
  { "ident",	obj_elf_ident,		0 },
  { "section",	obj_elf_section,	0 },
  { "size",	obj_elf_size,		0 },
  { "type",	obj_elf_type,		0 },
  { "version",  obj_elf_version,	0 },

  /* These are used for stabs-in-elf configurations.  */
  { "desc",	obj_elf_desc,		0	},
  { "line",	obj_elf_line,		0	},
  { "stabd",	obj_elf_stab,		'd'	},
  { "stabn",	obj_elf_stab,		'n'	},
  { "stabs",	obj_elf_stab,		's'	},
  /* This is used on Solaris 2.x on SPARC, but not supported yet.  */
  { "xstabs",	s_ignore,		0	},

  { NULL}	/* end sentinel */
};

static void
obj_elf_section (xxx)
     int xxx;
{
  char *string;
  asection *sec;

  /* Initialize this with inclusive-or of all flags that can be cleared
     by attributes, but not set by them.  Also include flags that won't
     get set properly in the assembler, but which the user/compiler
     shouldn't be expected to set.  */
  flagword flags = SEC_READONLY | SEC_ALLOC | SEC_RELOC;
  /* Initialize this with the default flags to be used if none are
     specified.  */
  flagword default_flags = SEC_ALLOC | SEC_RELOC;

  string = demand_copy_C_string (&xxx);
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    flags = default_flags;
  while (*input_line_pointer == ',')
    {
      flagword bit;
      int len, inv;
      char *p, oldp;

      input_line_pointer++;
      if (*input_line_pointer != '#')
	{
	  as_bad ("unrecognized syntax in .section command");
	  ignore_rest_of_line ();
	  break;
	}
      input_line_pointer++;

#define CHECK(X,BIT,NEG)	\
      if (!strncmp(X,input_line_pointer,len = sizeof(X) - 1)) { \
	bit = BIT; inv = NEG; goto match; }

      CHECK ("write", SEC_READONLY, 1);
      CHECK ("alloc", SEC_ALLOC, 0);
#undef CHECK

      p = input_line_pointer;
      while (!is_end_of_line[*p] && *p != 0 && *p != ',')
	p++;
      *p = 0;
      oldp = *p;
      as_bad ("unrecognized section attribute `%s' ignored",
	      input_line_pointer);
      *p = oldp;
      continue;

    match:
      if (inv)
	flags &= ~bit;
      else
	flags |= bit;
      input_line_pointer += len;
    }
  demand_empty_rest_of_line ();

  sec = bfd_get_section_by_name (stdoutput, string);
  if (sec == 0)
    {
      sec = bfd_make_section_old_way (stdoutput, string);
      bfd_set_section_flags (stdoutput, sec,
			     /* @@ What should the flags be??  */
			     flags);
      sec->output_section = sec;
    }
  subseg_change (sec, 0);
}

#if 0
/* pa-spaces.c -- Space/subspace support for the HP PA-RISC version of GAS */

/* for space, subspace, and symbol maintenance on HP 9000 Series 800 */

space_dict_chainS *space_dict_root;
space_dict_chainS *space_dict_last;

space_dict_chainS *current_space;
subspace_dict_chainS *current_subspace;
#endif

symbolS *start_symbol_root;
symbolS *start_symbol_last;

#if 0 /* I really don't think this belongs in this file.  */
void pa_spaces_begin()
{
  space_dict_chainS *space;
  int i;
  subsegT now_subseg	= now_subseg;

  space_dict_root = NULL;
  space_dict_last = NULL;

  start_symbol_root = NULL;
  start_symbol_last = NULL;

  /* create default space and subspace dictionaries */

  i = 0;
  while ( pa_def_spaces[i].name ) {
	  if ( pa_def_spaces[i].alias )
	      pa_def_spaces[i].segment = subseg_new(pa_def_spaces[i].alias,0);
	  else
	      pa_def_spaces[i].segment = bfd_make_section_old_way(stdoutput,pa_def_spaces[i].name);

	  create_new_space(pa_def_spaces[i].name,pa_def_spaces[i].spnum,
			   pa_def_spaces[i].loadable,pa_def_spaces[i].defined,
			   pa_def_spaces[i].private,pa_def_spaces[i].sort,0,
			   pa_def_spaces[i].segment);
	  i++;
  }

  i = 0;
  while ( pa_def_subspaces[i].name ) {
	  space = pa_segment_to_space(pa_def_spaces[pa_def_subspaces[i].def_space_index].segment);
	  if ( space ) {
		  create_new_subspace(space,
				      pa_def_subspaces[i].name,pa_def_subspaces[i].defined,
				      pa_def_subspaces[i].loadable,
				      pa_def_subspaces[i].code_only,pa_def_subspaces[i].common,
				      pa_def_subspaces[i].dup_common,pa_def_subspaces[i].zero,
				      pa_def_subspaces[i].sort,pa_def_subspaces[i].access,
				      pa_def_subspaces[i].space_index,
				      pa_def_subspaces[i].alignment,
				      pa_def_subspaces[i].quadrant,
				      pa_def_spaces[pa_def_subspaces[i].def_space_index].segment);
		  subseg_new(pa_def_subspaces[i].name,pa_def_subspaces[i].subsegment);
	  }
	  else
	      as_fatal("Internal error: space missing for subspace \"%s\"\n",
		       pa_def_subspaces[i].name);
	  i++;
  }
}

space_dict_chainS *create_new_space(name,spnum,loadable,defined,private,sort,defined_in_file,seg)
     char *name;
     int spnum;
     char loadable;
     char defined;
     char private;
     char sort;
     char defined_in_file;
     asection * seg;

{
  Elf_Internal_Shdr *new_space;
  space_dict_chainS *chain_entry;

  new_space = (Elf_Internal_Shdr *)xmalloc(sizeof(Elf_Internal_Shdr));
  if ( !new_space )
    as_fatal("Out of memory: could not allocate new Elf_Internal_Shdr: %s\n",name);

  /*
  new_space->space_number = spnum;
  new_space->is_loadable = loadable & 1;
  new_space->is_defined = defined & 1;
  new_space->is_private = private & 1;
  new_space->sort_key = sort & 0xff;

  new_space->loader_fix_index = ~0;
  new_space->loader_fix_quantity = 0;
  new_space->init_pointer_index = ~0;
  new_space->init_pointer_quantity = 0;
  new_space->subspace_quantity = 0;
  */

  chain_entry = (space_dict_chainS *)xmalloc(sizeof(space_dict_chainS));
  if ( !chain_entry )
    as_fatal("Out of memory: could not allocate new space chain entry: %s\n",name);

  SPACE_NAME(chain_entry) = (char *)xmalloc(strlen(name)+1);
  strcpy(SPACE_NAME(chain_entry),name);

  chain_entry->sd_entry = new_space;
  chain_entry->sd_defined = defined_in_file;
  chain_entry->sd_seg = seg;
  chain_entry->sd_last_subseg = -1;
  chain_entry->sd_next = NULL;

  SPACE_SPNUM(chain_entry) = spnum;
  SPACE_LOADABLE(chain_entry) = loadable & 1;
  SPACE_DEFINED(chain_entry) = defined & 1;
  SPACE_PRIVATE(chain_entry) = private & 1;
  SPACE_SORT(chain_entry) = sort & 0xff;

  /* find spot for the new space based on its sort key */

  if ( !space_dict_last )
    space_dict_last = chain_entry;

  if ( space_dict_root == NULL )        /* if root is null, it is very easy */
    space_dict_root = chain_entry;
  else
    {
      space_dict_chainS *sdcP;
      space_dict_chainS *last_sdcP;

      sdcP = space_dict_root;
      last_sdcP = NULL;

      while ( sdcP ) {
	if ( SPACE_SORT(sdcP) < SPACE_SORT(chain_entry) ) {
	  last_sdcP = sdcP;
	  sdcP = sdcP->sd_next;
	}
	else if ( SPACE_SORT(sdcP) == SPACE_SORT(chain_entry) ) {
	  last_sdcP = sdcP;
	  sdcP = sdcP->sd_next;
	}
	else if ( SPACE_SORT(sdcP) > SPACE_SORT(chain_entry) ) {
	  break;
	}	
      }

      if ( last_sdcP ) {
	chain_entry->sd_next = sdcP;
	last_sdcP->sd_next = chain_entry;
      }
      else {
	space_dict_root = chain_entry;
	chain_entry->sd_next = sdcP;
      }

      if ( chain_entry->sd_next  == NULL )
	space_dict_last = chain_entry;
    }

  return chain_entry;
}

subspace_dict_chainS
*create_new_subspace(space,name,defined,loadable,code_only,common,dup_common,
		     is_zero,sort,access,space_index,alignment,quadrant,seg)
     space_dict_chainS *space;
     char *name;
     char defined,loadable,code_only,common,dup_common,is_zero;
     char sort;
     int access;
     int space_index;
     int alignment;
     int quadrant;
     asection * seg;
{
  Elf_Internal_Shdr * new_subspace;
  subspace_dict_chainS * chain_entry;
  symbolS * start_symbol;

  new_subspace = (Elf_Internal_Shdr *)xmalloc(sizeof(Elf_Internal_Shdr));
  if ( !new_subspace )
    as_fatal("Out of memory: could not allocate new Elf_Internal_Shdr: %s\n",
	    name);

  /*
  new_subspace->space_index = space_index;
  new_subspace->fixup_request_index = ~0;
  */

  chain_entry = (subspace_dict_chainS *)xmalloc(sizeof(subspace_dict_chainS));
  if ( !chain_entry )
    as_fatal("Out of memory: could not allocate new subspace chain entry: %s\n",name);

  chain_entry->ssd_entry = new_subspace;
  SUBSPACE_NAME(chain_entry) = (char *)xmalloc(strlen(name)+1);
  strcpy(SUBSPACE_NAME(chain_entry),name);

  SUBSPACE_ACCESS(chain_entry) = access & 0x7f;
  SUBSPACE_LOADABLE(chain_entry) = loadable & 1;
  SUBSPACE_COMMON(chain_entry) = common & 1;
  SUBSPACE_DUP_COMM(chain_entry) = dup_common & 1;
  SUBSPACE_SORT(chain_entry) = sort & 0xff;
  SET_SUBSPACE_CODE_ONLY(chain_entry,code_only & 1);
  SUBSPACE_ALIGN(chain_entry) = alignment & 0xffff;
  SUBSPACE_QUADRANT(chain_entry) = quadrant & 0x3;
  SUBSPACE_SUBSPACE_START(chain_entry) = pa_subspace_start(space,quadrant);

  chain_entry->ssd_defined = defined;
  chain_entry->ssd_space_number = space_index;
  chain_entry->ssd_subseg = pa_next_subseg(space);
  chain_entry->ssd_seg = seg;
  SUBSPACE_ZERO(chain_entry) = is_zero;
  chain_entry->ssd_last_align = 1;
  chain_entry->ssd_next = NULL;

  /* find spot for the new subspace based on its sort key */

  if ( space->sd_subspaces == NULL )        /* if root is null, it is very easy */
    space->sd_subspaces = chain_entry;
  else
    {
      subspace_dict_chainS *ssdcP;
      subspace_dict_chainS *last_ssdcP;

      ssdcP = space->sd_subspaces;
      last_ssdcP = NULL;

      while ( ssdcP ) {
	if ( SUBSPACE_SORT(ssdcP) < SUBSPACE_SORT(chain_entry) ) {
	  last_ssdcP = ssdcP;
	  ssdcP = ssdcP->ssd_next;
	}
	else if ( SUBSPACE_SORT(ssdcP) == SUBSPACE_SORT(chain_entry) ) {
	  last_ssdcP = ssdcP;
	  ssdcP = ssdcP->ssd_next;
	}
	else if ( SUBSPACE_SORT(ssdcP) > SUBSPACE_SORT(chain_entry) ) {
	  break;
	}	
      }

      if ( last_ssdcP ) {
	chain_entry->ssd_next = ssdcP;
	last_ssdcP->ssd_next = chain_entry;
      }
      else {
	space->sd_subspaces = chain_entry;
	chain_entry->ssd_next = ssdcP;
      }
    }

  start_symbol = pa_set_start_symbol(seg,space->sd_last_subseg);
  chain_entry->ssd_start_sym = start_symbol;
  return chain_entry;

}

subspace_dict_chainS
*update_subspace(name,defined,loadable,code_only,common,dup_common,sort,zero,
		 access,space_index,alignment,quadrant,subseg)
     char *name;
     char defined,loadable,code_only,common,dup_common,zero;
     char sort;
     int access;
     int space_index;
     int alignment;
     int quadrant;
     subsegT subseg;
{
  subspace_dict_chainS *chain_entry;
  subspace_dict_chainS *is_defined_subspace();

  if ( (chain_entry = is_defined_subspace(name,subseg)) ) {

    SUBSPACE_ACCESS(chain_entry) = access & 0x7f;
    SUBSPACE_LOADABLE(chain_entry) = loadable & 1;
    SUBSPACE_COMMON(chain_entry) = common & 1;
    SUBSPACE_DUP_COMM(chain_entry) = dup_common & 1;
    SET_SUBSPACE_CODE_ONLY(chain_entry,code_only & 1);
    SUBSPACE_SORT(chain_entry) = sort & 0xff;
    /* chain_entry->ssd_entry->space_index = space_index; */
    SUBSPACE_ALIGN(chain_entry) = alignment & 0xffff;
    SUBSPACE_QUADRANT(chain_entry) = quadrant & 0x3;

    chain_entry->ssd_defined = defined;
    chain_entry->ssd_space_number = space_index;
    SUBSPACE_ZERO(chain_entry) = zero;
  }
  else
    chain_entry = NULL;

  return chain_entry;

}

space_dict_chainS *is_defined_space(name)
     char *name;
{
  space_dict_chainS *spaceCh;

  for (spaceCh = space_dict_root;spaceCh;spaceCh=spaceCh->sd_next ) {
    if ( strcmp(SPACE_NAME(spaceCh),name) == 0 ) {
      return spaceCh;
    }
  }

  return NULL;
}

space_dict_chainS *pa_segment_to_space(seg)
     asection * seg;
{
  space_dict_chainS *spaceCh;

  for (spaceCh = space_dict_root;spaceCh;spaceCh=spaceCh->sd_next ) {
    if ( spaceCh->sd_seg == seg ) {
      return spaceCh;
    }
  }

  return NULL;
}

subspace_dict_chainS *is_defined_subspace(name,subseg)
     char *name;
     subsegT subseg;
{
  space_dict_chainS *spaceCh;
  subspace_dict_chainS *subspCh;

  for ( spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next ) {
    for ( subspCh = spaceCh->sd_subspaces; subspCh; subspCh=subspCh->ssd_next ) {
      /*
	if ( strcmp(SUBSPACE_NAME(subspCh),name) == 0 &&
	    subspCh->ssd_subseg == subseg ) {
       */
	if ( strcmp(SUBSPACE_NAME(subspCh),name) == 0 ) {
	  return subspCh;
	}
    }
  }
  return NULL;
}

subspace_dict_chainS *pa_subsegment_to_subspace(seg,subseg)
    asection * seg;
    subsegT subseg;
{
  space_dict_chainS *spaceCh;
  subspace_dict_chainS *subspCh;

  for ( spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next ) {
    if ( spaceCh->sd_seg == seg ) {
      for (subspCh = spaceCh->sd_subspaces;subspCh;subspCh=subspCh->ssd_next ) {
	if ( subspCh->ssd_subseg == (int)subseg ) {
	  return subspCh;
	}
      }
    }
  }

  return NULL;
}

space_dict_chainS *pa_find_space_by_number(number)
     int number;
{
  space_dict_chainS *spaceCh;

  for (spaceCh = space_dict_root;spaceCh;spaceCh=spaceCh->sd_next ) {
    if ( SPACE_SPNUM(spaceCh) == number ) {
      return spaceCh;
    }
  }

  return NULL;
}

unsigned int pa_subspace_start(space,quadrant)
     space_dict_chainS *space;
     int quadrant;
{
  if ( (strcasecmp(SPACE_NAME(space),"$PRIVATE$") == 0) &&
       quadrant == 1 ) {
    return 0x40000000;
  }
  else if ( space->sd_seg == data_section && quadrant == 1 ) {  /* in case name is */
                                                            /* already converted */
                                                            /* to a space dict- */
                                                            /* ionary index */
    return 0x40000000;
  }
  else
    return 0;
}

int pa_next_subseg(space)
     space_dict_chainS *space;
{

  space->sd_last_subseg++;
  return space->sd_last_subseg;
}

int is_last_defined_subspace(ssd)
     subspace_dict_chainS *ssd;
{

  for ( ;ssd; ssd = ssd->ssd_next ) {
    if ( ssd->ssd_defined )
      return FALSE;
  }

  return TRUE;
}

symbolS *pa_get_start_symbol(seg,subseg)
     asection * seg;
     subsegT subseg;
{
  symbolS *start_symbol;
  subspace_dict_chainS *ssd;

  start_symbol = NULL;

  /* each time a new space is created, build a symbol called LS$START_seg_subseg$ */
  /* where <space-name> is the name of the space */
  /* the start symbol will be SS_LOCAL and ST_CODE */

  if ( seg == bfd_make_section_old_way ( stdoutput, ".text" ) ||
       seg == bfd_make_section_old_way ( stdoutput, ".data" ) ||
       seg == bfd_make_section_old_way ( stdoutput, GDB_DEBUG_SPACE_NAME ) ) {
	  ssd = pa_subsegment_to_subspace(seg,subseg);
	  if ( ssd ) {
		  start_symbol = ssd->ssd_start_sym;
	  }
	  else
	      as_fatal("Internal error: missing subspace for (seg,subseg)=('%s',%d)",
		       seg->name,subseg);
  }
  else
      as_fatal("Internal error: attempt to find start symbol for unloadable segment: '%s'",
	       seg->name);

  return start_symbol;
}

/*
  Function to define a symbol whose address is the beginning of a subspace.
  This function assumes the symbol is to be defined for the current subspace.
 */

symbolS *pa_set_start_symbol(seg,subseg)
     asection * seg;
     subsegT subseg;
{
  symbolS *start_symbol;
  subspace_dict_chainS *ssd;
  char *symbol_name;

  symbol_name = (char *)xmalloc(strlen("LS$START__000000$")+strlen(seg->name)+1);

  sprintf(symbol_name,"LS$START_%s_%03d$",seg->name,subseg);

  start_symbol
      = symbol_new(symbol_name,seg,0,frag_now); /* XXX: not sure if value s.b. 0 or frag s.b. NULL */

  start_symbol->bsym->flags	= BSF_LOCAL;	/* XXX: isn't there a macro defined for this? */

  /* each time a new space is created, build a symbol called LS$START_seg_subseg$ */
  /* where <space-name> is the name of the space */
  /* the start symbol will be SS_LOCAL and ST_CODE */
  /* This function assumes that (seg,subseg) is a new subsegment(subspace) */

  if ( seg == bfd_make_section_old_way ( stdoutput, ".text" ) ||
       seg == bfd_make_section_old_way ( stdoutput, ".data" ) ||
       seg == bfd_make_section_old_way ( stdoutput, GDB_DEBUG_SPACE_NAME ) ) {
	  ssd = pa_subsegment_to_subspace(seg,subseg);
	  if ( ssd ) {
		  ssd->ssd_start_sym = start_symbol;
	  }
	  else
	      as_fatal("Internal error: missing subspace for (seg,subseg)=('%s',%d)",
		       seg,subseg);
  }
  else
      as_fatal("Internal error: attempt to define start symbol for unloadable segment: '%s'",
	       seg->name);

  return start_symbol;
}
#endif

int
obj_elf_frob_symbol (sym, punt)
     symbolS *sym;
     int *punt;
{

	/* If this is a local symbol, are there any relocations for */
	/* which need this symbol?	*/

	/* To find this out, we examine all relocations	in all bfd */
	/* sections that have relocations.  If there is one that */
	/* references this symbol, we need to keep this symbol.  In */
	/* this case, we return a true status.  In all other cases, we */
	/* return a false status.	*/

	if ( S_IS_LOCAL(sym) ) {
		asymbol *bsym = sym->bsym;	
		bfd *abfd = bsym->the_bfd;
		asection *bsec;

		for ( bsec = abfd->sections; bsec; bsec = bsec->next ) {
			struct reloc_cache_entry **rlocs = bsec->orelocation;
			int rcnt = bsec->reloc_count;

			if ( rlocs ) {
				int i;

				for ( i = 0; i < rcnt; i++ ) {
					if ( rlocs[i]->sym_ptr_ptr
					    && rlocs[i]->sym_ptr_ptr[0] == bsym )
						return 1;
				}
			}
		}
	}
	return 0;
}

static void obj_elf_line() {
	/* Assume delimiter is part of expression.
	   BSD4.2 as fails with delightful bug, so we
	   are not being incompatible here. */
	new_logical_line((char *)NULL, (int)(get_absolute_expression()));
	demand_empty_rest_of_line();
} /* obj_elf_line() */

/*
 *			stab()
 *
 * Handle .stabX directives, which used to be open-coded.
 * So much creeping featurism overloaded the semantics that we decided
 * to put all .stabX thinking in one place. Here.
 *
 * We try to make any .stabX directive legal. Other people's AS will often
 * do assembly-time consistency checks: eg assigning meaning to n_type bits
 * and "protecting" you from setting them to certain values. (They also zero
 * certain bits before emitting symbols. Tut tut.)
 *
 * If an expression is not absolute we either gripe or use the relocation
 * information. Other people's assemblers silently forget information they
 * don't need and invent information they need that you didn't supply.
 *
 * .stabX directives always make a symbol table entry. It may be junk if
 * the rest of your .stabX directive is malformed.
 */

/*
 *                      pa_stab_symbol_string()
 *
 * Build a string dictionary entry for a .stabX symbol.
 * The symbol is added to the .stabstr section.
 *
 */

static unsigned int gdb_string_index = 0;

static unsigned int
elf_stab_symbol_string(string)
     char *string;
{
  asection *save_seg;
  asection *seg;
  subsegT save_subseg;
  unsigned int length;
  unsigned int old_gdb_string_index;
  char *clengthP;
  int i;
  char c;

  old_gdb_string_index = 0;
  length = strlen(string);
  clengthP = (char *)&length;
  if ( length > 0 ) {			/* Ordinary case. */
    save_seg = now_seg;
    save_subseg = now_subseg;

    /* Create the stab sections, if they are not already created. */
    seg = bfd_get_section_by_name(stdoutput,".stabstr");
    if ( seg == 0 ) {
	    seg = bfd_make_section_old_way(stdoutput,".stabstr");
	    bfd_set_section_flags (stdoutput,
				   seg,
				   SEC_READONLY | SEC_ALLOC | SEC_LOAD );
    }
    subseg_new((char *) seg->name,save_subseg);
    old_gdb_string_index = gdb_string_index;
    i = 0;
    while ( (c = *string++) )
      {
	i++;
        gdb_string_index++;
	FRAG_APPEND_1_CHAR( c );
      }
    {
      FRAG_APPEND_1_CHAR( (char)0 );
      i++;
      gdb_string_index++;
    }
    while ( i%4 != 0 ) {
      FRAG_APPEND_1_CHAR( (char)0 );
      i++;
      gdb_string_index++;
    }
    subseg_new((char *) save_seg->name,save_subseg);
  }

  return old_gdb_string_index;
}

unsigned int
elf_stab_symbol(symbolP,stab_type)
     symbolS *symbolP;
     int stab_type;
{
  unsigned int length;
  int i;
  char c;
  char *toP;

  /* the string index portion of the stab */

  toP = frag_more( sizeof(long) +
		   sizeof(S_GET_TYPE(symbolP)) +
		   sizeof(S_GET_OTHER(symbolP)) +
		   sizeof(S_GET_DESC(symbolP)));
  md_number_to_chars(toP,symbolP->sy_name_offset,sizeof(long));
  md_number_to_chars(toP,
		     S_GET_TYPE(symbolP),
		     sizeof(S_GET_TYPE(symbolP)));
  md_number_to_chars(toP + sizeof(S_GET_TYPE(symbolP)),
		     S_GET_OTHER(symbolP),
		     sizeof(S_GET_OTHER(symbolP)));
  md_number_to_chars(toP + sizeof(S_GET_TYPE(symbolP))
		         + sizeof(S_GET_OTHER(symbolP)),
		     S_GET_DESC(symbolP),
		     sizeof(S_GET_DESC(symbolP)));

  /* n_value has to be relocated */

  /* Don't bother relocating if we're only adding in a constant. */

  if ((stab_type == 's' || stab_type == 'n') && symbolP->sy_forward == 0)
	  S_SET_VALUE(symbolP,S_GET_VALUE(symbolP->sy_forward));

  toP = frag_more(4);
  md_number_to_chars(toP, S_GET_VALUE(symbolP),
		     sizeof(S_GET_VALUE(symbolP)));

#if 0
  if ( (stab_type == 's' || stab_type == 'n') && symbolP->sy_forward )
    {
      i = S_GET_TYPE(symbolP) & N_TYPE;
      fix_new_hppa(frag_now,	/* which frag */
		   toP-frag_now->fr_literal, /* where */
		   4,		/* size */
		   symbolP->sy_forward,	/* addr of symbol for this stab */
		   (asymbol *)NULL,
		   0,
		   i == N_UNDF || i == N_ABS,	/* 1 if internal relocation */
		   R_HPPA,	/* reloc type */
		   e_fsel,	/* fixup field = F% */
		   32,
		   0,		/* arg_reloc descriptor */
		   (char *)0
		   );
    }
  else if ( stab_type == 'd' )
    {
      fix_new_hppa (frag_now,	/* which frag */
		    toP-frag_now->fr_literal, /* where */
		    4,		/* size */
		    symbolP,	/* addr of symbol for this stab */
		    (asymbol *)NULL,
		    0,
		    0,
		    R_HPPA,	/* reloc type */
		    e_fsel,	/* fixup field = F% */
		    32,
		    0,		/* arg_reloc descriptor */
		    (char *)0
		    );
    }
#else
  /* What needs to replace the above code?  */
  abort ();
#endif
}

static void obj_elf_stab(what)
     int what;
{
  extern int listing;

  symbolS *	symbolP = 0;
  char *	string;
  int saved_type = 0;
  int length;
  int goof = 0;
  long longint;
  asection *saved_seg = now_seg;
  asection *seg;
  subsegT subseg = now_subseg;

  seg = bfd_get_section_by_name(stdoutput,".stab");
  if ( seg == 0 )
    {
      seg = bfd_make_section_old_way(stdoutput,".stab");
      bfd_set_section_flags (stdoutput,
			     seg,
			     SEC_READONLY | SEC_ALLOC | SEC_LOAD | SEC_RELOC);
    }

  /*
   * Enter with input_line_pointer pointing past .stabX and any following
   * whitespace.
   */
  if (what == 's') {
      string = demand_copy_C_string(& length);
      SKIP_WHITESPACE();
      if (* input_line_pointer == ',')
       input_line_pointer ++;
      else
	{
	  as_bad("I need a comma after symbol's name");
	  goof = 1;
	}
    }
  else
    string = "";

  /*
   * Input_line_pointer->after ','.  String->symbol name.
   */
  if (! goof)
    {
      symbolP = symbol_new(string, &bfd_und_section, 0, (struct frag *)0);

      /* enter the string in the .stab string table (section .stabstr) */
      symbolP->sy_name_offset = elf_stab_symbol_string(string);

      switch (what)
	{
	case 'd':
	  S_SET_NAME(symbolP, NULL); /* .stabd feature. */
	  S_SET_VALUE(symbolP, obstack_next_free(&frags) - frag_now->fr_literal);
	  S_SET_SEGMENT(symbolP, now_seg);
	  symbolP->sy_frag = frag_now;
	  break;

	case 'n':
	  symbolP->sy_frag = &zero_address_frag;
	  break;

	case 's':
	  symbolP->sy_frag = & zero_address_frag;
	  break;

	default:
	  BAD_CASE(what);
	  break;
	}

      if (get_absolute_expression_and_terminator(&longint) == ',')
	{
	  saved_type = longint;
	  S_SET_TYPE (symbolP, saved_type);
	}
      else
	{
	  as_bad("I want a comma after the n_type expression");
	  goof = 1;
	  input_line_pointer --; /* Backup over a non-',' char. */
	}
    }

  if (!goof)
    {
      if (get_absolute_expression_and_terminator(&longint) == ',')
	S_SET_OTHER(symbolP, longint);
      else
	{
	  as_bad("I want a comma after the n_other expression");
	  goof = 1;
	  input_line_pointer--; /* Backup over a non-',' char. */
	}
    }

  if (!goof)
    {
      S_SET_DESC(symbolP, get_absolute_expression());
      if (what == 's' || what == 'n')
	{
	  if (*input_line_pointer != ',')
	    {
	      as_bad("I want a comma after the n_desc expression");
	      goof = 1;
	    }
	  else
	    {
	      input_line_pointer++;
	    }
	}
    }

  if ( !goof ) {
	  if ( what=='s' || what=='n' ) {
		  pseudo_set(symbolP);
		  S_SET_TYPE (symbolP, saved_type);
	  }
	  /* Emit the stab symbol. */
	  subseg_new((char *) seg->name,subseg);
	  elf_stab_symbol(symbolP, what);
	  subseg_new((char *) saved_seg->name,subseg);

	  if ( what=='s' || what=='n' && symbolP->sy_forward == NULL ) {
		  /* symbol is not needed in the regular symbol table */
		  symbol_remove(symbolP,&symbol_rootP,&symbol_lastP);
	  }

  }

#ifndef NO_LISTING
  if (listing && !goof)
    switch (S_GET_TYPE (symbolP))
      {
      case N_SLINE:
	listing_source_line (S_GET_DESC (symbolP));
	break;
      case N_SO:
      case N_SOL:
	listing_source_file (string);
	break;
      }
#endif

  if (goof)
    ignore_rest_of_line();
  else
    demand_empty_rest_of_line ();
} /* obj_elf_stab() */

static void obj_elf_desc() {
	char *name;
	char c;
	char *p;
	symbolS *symbolP;
	int temp;
	
	/*
	 * Frob invented at RMS' request. Set the n_desc of a symbol.
	 */
	name = input_line_pointer;
	c = get_symbol_end();
	p = input_line_pointer;
	* p = c;
	SKIP_WHITESPACE();
	if (*input_line_pointer != ',') {
		*p = 0;
		as_bad("Expected comma after name \"%s\"", name);
		*p = c;
		ignore_rest_of_line();
	} else {
		input_line_pointer ++;
		temp = get_absolute_expression();
		*p = 0;
		symbolP = symbol_find_or_make(name);
		*p = c;
		S_SET_DESC(symbolP,temp);
	}
	demand_empty_rest_of_line();
} /* obj_elf_desc() */

void obj_read_begin_hook()
{
}

void obj_symbol_new_hook(symbolP)
symbolS *symbolP;
{
  elf_symbol_type *esym = (elf_symbol_type *)symbolP;

  /* There is an Elf_Internal_Sym and an Elf_External_Sym.  For now,
     just zero them out.  */

  bzero((char *) &esym->internal_elf_sym,sizeof(Elf_Internal_Sym));
  bzero((char *) &esym->native_elf_sym,sizeof(Elf_External_Sym));
}

static void obj_elf_version()
{
    char *name;
    unsigned int c;
    char ch;
    char *p;
    int temp;
    symbolS *	symbolP;
    asection *seg = now_seg;
    subsegT subseg = now_subseg;
    Elf_Internal_Note i_note;
    Elf_External_Note e_note;
    asection *note_secp = (asection *)NULL;
    int i, len;

    SKIP_WHITESPACE();
    if (* input_line_pointer == '\"') {
      ++input_line_pointer; /* -> 1st char of string. */
      name = input_line_pointer;

      while ( is_a_char(c = next_char_of_string()) )
	      ;
      c = *input_line_pointer;
      *input_line_pointer = '\0';
      *(input_line_pointer-1) = '\0';
      *input_line_pointer = c;

      /* create the .note section if this is the first version string */

      note_secp = bfd_get_section_by_name(stdoutput,".note");
      if ( note_secp == (asection *)NULL ) {
	      note_secp = bfd_make_section_old_way(stdoutput,".note");
	      bfd_set_section_flags(stdoutput,
				    note_secp,
				    SEC_LOAD | SEC_ALLOC | SEC_HAS_CONTENTS);
      }

      /* process the version string */

      subseg_new((char *)note_secp->name, 0);
      len = strlen(name);

      i_note.namesz	= ((len + 1) + 3) & ~3;	/* round this to word boundary	*/
      i_note.descsz	= 0;			/* no description	*/
      i_note.type	= NT_VERSION;
      p = frag_more(sizeof(e_note.namesz));
      md_number_to_chars(p, i_note.namesz, 4);
      p = frag_more(sizeof(e_note.descsz));
      md_number_to_chars(p, i_note.descsz, 4);
      p = frag_more(sizeof(e_note.type));
      md_number_to_chars(p, i_note.type,   4);

      for ( i = 0; i < len; i++ ) {
	      ch = *(name + i);
	      {
		      FRAG_APPEND_1_CHAR( ch );
	      }
      }
      frag_align(2,0);

      subseg_new((char *)seg->name,subseg);
    }
    else {
      as_bad( "Expected \"-ed string" );
    }
    demand_empty_rest_of_line();
}

static void
obj_elf_size ()
{
  char *name = input_line_pointer;
  char c = get_symbol_end ();
  char *p;
  expressionS exp;
  segT sec;
  symbolS *sym;

  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      *p = 0;
      as_bad ("expected comma after name `%s' in .size directive", name);
      *p = c;
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  sec = expression (&exp);
  if (sec == absent_section)
    {
      as_bad ("missing expression in .size directive");
      exp.X_seg = absolute_section;
      exp.X_add_number = 0;
    }
  *p = 0;
  sym = symbol_find_or_make (name);
  *p = c;
  if (sec == absolute_section)
    S_SET_SIZE (sym, exp.X_add_number);
  else
    as_tsktsk (".size not yet supported, ignored");
  demand_empty_rest_of_line ();
}

static void
obj_elf_type ()
{
  char *name = input_line_pointer;
  char c = get_symbol_end ();
  char *p;
  int type;
  symbolS *sym;

  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_bad ("expected comma after name in .type directive");
    egress:
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != '#')
    {
      as_bad ("expected `#' after comma in .type directive");
      goto egress;
    }
  input_line_pointer++;
  if (!strncmp ("function", input_line_pointer, sizeof ("function") - 1))
    {
      type = BSF_FUNCTION;
      input_line_pointer += sizeof ("function") - 1;
    }
  else
    {
      as_bad ("unrecognized symbol type, ignored");
      goto egress;
    }
  demand_empty_rest_of_line ();
  *p = 0;
  sym = symbol_find_or_make (name);
  sym->bsym->flags = type;
}

static void
obj_elf_ident ()
{
  int xxx;
  char * string;

  string = demand_copy_C_string (&xxx);
  as_tsktsk (".ident not supported, ignoring");
}
