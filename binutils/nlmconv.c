/* nlmconv.c -- NLM conversion program
   Copyright (C) 1993 Free Software Foundation, Inc.

This file is part of GNU Binutils.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Ian Lance Taylor <ian@cygnus.com>.

   This program can be used to convert any appropriate object file
   into a NetWare Loadable Module (an NLM).  It will accept a linker
   specification file which is identical to that accepted by the
   NetWare linker, NLMLINK, except that the INPUT command, normally
   used to give a list of object files to link together, is not used.
   This program will convert only a single object file.  */

#include <ansidecl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <getopt.h>
#include <bfd.h>
#include "sysdep.h"
#include "bucomm.h"
/* Internal BFD NLM header.  */
#include "libnlm.h"
#include "nlmconv.h"

/* If strerror is just a macro, we want to use the one from libiberty
   since it will handle undefined values.  */
#undef strerror
extern char *strerror ();

/* Global variables.  */

/* The name used to invoke the program.  */
char *program_name;

/* The version number.  */
extern char *program_version;

/* The symbols table.  */
static asymbol **symbols;

/* Local variables.  */

/* The list of long options.  */
static struct option long_options[] =
{
  { "header-info", required_argument, 0, 'T' },
  { "help", no_argument, 0, 'H' },
  { "input-format", required_argument, 0, 'I' },
  { "output-format", required_argument, 0, 'O' },
  { "version", no_argument, 0, 'v' },
  { NULL, no_argument, 0, 0 }
};

/* Local routines.  */

static void show_help PARAMS ((void));
static void show_usage PARAMS ((FILE *, int));
static const char *select_output_format PARAMS ((enum bfd_architecture,
						 long, boolean));
static void setup_sections PARAMS ((bfd *, asection *, PTR));
static void copy_sections PARAMS ((bfd *, asection *, PTR));
static void mangle_relocs PARAMS ((bfd *, arelent **,
				   bfd_size_type, char *,
				   bfd_size_type));
static void i386_mangle_relocs PARAMS ((bfd *, arelent **, bfd_size_type,
					char *, bfd_size_type));

/* The main routine.  */

int
main (argc, argv)
     int argc;
     char **argv;
{
  int opt;
  const char *input_format = NULL;
  const char *output_format = NULL;
  const char *header_file = NULL;
  bfd *inbfd;
  bfd *outbfd;
  unsigned int symcount;
  unsigned int i;
  char inlead, outlead;
  boolean gotstart, gotexit, gotcheck;
  struct stat st;
  FILE *custom_data, *help_data, *message_data, *rpc_data;
  bfd_size_type custom_size, help_size, message_size, module_size, rpc_size;
  asection *custom_section, *help_section, *message_section, *module_section;
  asection *rpc_section;
  int len;

  program_name = argv[0];

  bfd_init ();

  while ((opt = getopt_long (argc, argv, "HI:O:T:v", long_options, (int *) 0))
	 != EOF)
    {
      switch (opt)
	{
	case 'H':
	  show_help ();
	  /*NOTREACHED*/
	case 'I':
	  input_format = optarg;
	  break;
	case 'O':
	  output_format = optarg;
	  break;
	case 'T':
	  header_file = optarg;
	  break;
	case 'v':
	  printf ("GNU %s version %s\n", program_name, program_version);
	  exit (0);
	  /*NOTREACHED*/
	case 0:
	  break;
	default:
	  show_usage (stderr, 1);
	  /*NOTREACHED*/
	}
    }

  if (optind + 2 != argc)
    show_usage (stderr, 1);

  if (strcmp (argv[optind], argv[optind + 1]) == 0)
    {
      fprintf (stderr, "%s: input and output files must be different\n",
	       program_name);
      exit (1);
    }

  inbfd = bfd_openr (argv[optind], input_format);
  if (inbfd == NULL)
    bfd_fatal (argv[optind]);

  if (! bfd_check_format (inbfd, bfd_object))
    bfd_fatal (argv[optind]);

  if (output_format == NULL)
    output_format = select_output_format (bfd_get_arch (inbfd),
					  bfd_get_mach (inbfd),
					  inbfd->xvec->byteorder_big_p);

  assert (output_format != NULL);
  outbfd = bfd_openw (argv[optind + 1], output_format);
  if (outbfd == NULL)
    bfd_fatal (argv[optind + 1]);
  if (! bfd_set_format (outbfd, bfd_object))
    bfd_fatal (argv[optind + 1]);

  assert (outbfd->xvec->flavour == bfd_target_nlm_flavour);

  if (bfd_arch_get_compatible (inbfd, outbfd) == NULL)
    fprintf (stderr,
	     "%s: warning:input and output formats are not compatible\n",
	     program_name);

  /* Initialize the header information to default values.  */
  fixed_hdr = nlm_fixed_header (outbfd);
  var_hdr = nlm_variable_header (outbfd);
  version_hdr = nlm_version_header (outbfd);
  copyright_hdr = nlm_copyright_header (outbfd);
  extended_hdr = nlm_extended_header (outbfd);
  check_procedure = NULL;
  custom_file = NULL;
  debug_info = false;
  exit_procedure = "_Stop";
  export_symbols = NULL;
  map_file = NULL;
  full_map = false;
  help_file = NULL;
  import_symbols = NULL;
  message_file = NULL;
  modules = NULL;
  sharelib_file = NULL;
  start_procedure = "_Prelude";
  verbose = false;
  rpc_file = NULL;

  parse_errors = 0;

  /* Parse the header file (if there is one).  */
  if (header_file != NULL)
    {
      if (! nlmlex_file (header_file)
	  || yyparse () != 0
	  || parse_errors != 0)
	exit (1);
    }

  /* Start copying the input BFD to the output BFD.  */
  if (! bfd_set_file_flags (outbfd, bfd_get_file_flags (inbfd)))
    bfd_fatal (bfd_get_filename (outbfd));

  symbols = (asymbol **) xmalloc (get_symtab_upper_bound (inbfd));
  symcount = bfd_canonicalize_symtab (inbfd, symbols);

  /* Adjust symbol information.  */
  inlead = bfd_get_symbol_leading_char (inbfd);
  outlead = bfd_get_symbol_leading_char (outbfd);
  gotstart = false;
  gotexit = false;
  gotcheck = false;
  for (i = 0; i < symcount; i++)
    {
      register asymbol *sym;

      sym = symbols[i];

      /* Add or remove a leading underscore.  */
      if (inlead != outlead)
	{
	  if (inlead != '\0')
	    {
	      if (bfd_asymbol_name (sym)[0] == inlead)
		{
		  if (outlead == '\0')
		    ++sym->name;
		  else
		    {
		      char *new;

		      new = xmalloc (strlen (bfd_asymbol_name (sym)) + 1);
		      new[0] = outlead;
		      strcpy (new + 1, bfd_asymbol_name (sym) + 1);
		      sym->name = new;
		    }
		}
	    }
	  else
	    {
	      char *new;

	      new = xmalloc (strlen (bfd_asymbol_name (sym)) + 2);
	      new[0] = outlead;
	      strcpy (new + 1, bfd_asymbol_name (sym));
	      sym->name = new;
	    }
	}

      /* If this is a global symbol, see if it's in the export list.
	 Change the prefix if necessary.  */
      if ((sym->flags & (BSF_EXPORT | BSF_GLOBAL)) != 0
	  && export_symbols != NULL)
	{
	  register struct string_list *l;

	  for (l = export_symbols; l != NULL; l = l->next)
	    {
	      if (strcmp (l->string, bfd_asymbol_name (sym)) == 0)
		break;
	      else
		{
		  char *zbase;

		  zbase = strchr (l->string, '@');
		  if (zbase != NULL
		      && strcmp (zbase + 1, bfd_asymbol_name (sym)) == 0)
		    {
		      sym->name = l->string;
		      break;
		    }
		}
	    }
	  if (l == NULL)
	    fprintf (stderr,
		     "%s: warning: symbol %s exported but not in export list\n",
		     program_name, bfd_asymbol_name (sym));
	}

      /* If it's an undefined symbol, see if it's on the import list.
	 Change the prefix if necessary.  */
      if (bfd_get_section (sym) == &bfd_und_section
	  && import_symbols != NULL)
	{
	  register struct string_list *l;

	  for (l = import_symbols; l != NULL; l = l->next)
	    {
	      if (strcmp (l->string, bfd_asymbol_name (sym)) == 0)
		break;
	      else
		{
		  char *zbase;

		  zbase = strchr (l->string, '@');
		  if (zbase != NULL
		      && strcmp (zbase + 1, bfd_asymbol_name (sym)) == 0)
		    {
		      sym->name = l->string;
		      break;
		    }
		}
	    }
	  if (l == NULL)
	    fprintf (stderr,
		     "%s: warning: symbol %s imported but not in import list\n",
		     program_name, bfd_asymbol_name (sym));
	}
	
      /* See if it's one of the special named symbols.  */
      if (strcmp (bfd_asymbol_name (sym), start_procedure) == 0)
	{
	  if (! bfd_set_start_address (outbfd, bfd_asymbol_value (sym)))
	    bfd_fatal ("set start address");
	  gotstart = true;
	}
      if (strcmp (bfd_asymbol_name (sym), exit_procedure) == 0)
	{
	  nlm_fixed_header (outbfd)->exitProcedureOffset =
	    bfd_asymbol_value (sym);
	  gotexit = true;
	}
      if (check_procedure != NULL
	  && strcmp (bfd_asymbol_name (sym), check_procedure) == 0)
	{
	  nlm_fixed_header (outbfd)->checkUnloadProcedureOffset =
	    bfd_asymbol_value (sym);
	  gotcheck = true;
	}
    }

  bfd_set_symtab (outbfd, symbols, symcount);
    
  if (! gotstart)
    fprintf (stderr, "%s: warning: START procedure %s not defined\n",
	     program_name, start_procedure);
  if (! gotexit)
    fprintf (stderr, "%s: warning: EXIT procedure %s not defined\n",
	     program_name, exit_procedure);
  if (check_procedure != NULL
      && ! gotcheck)
    fprintf (stderr, "%s: warning: CHECK procedure %s not defined\n",
	     program_name, check_procedure);

  /* Set up the sections.  */
  bfd_map_over_sections (inbfd, setup_sections, (PTR) outbfd);

  /* Add additional sections required for the header information.  */
  if (custom_file != NULL)
    {
      custom_data = fopen (custom_file, "r");
      if (custom_data == NULL
	  || fstat (fileno (custom_data), &st) < 0)
	{
	  fprintf (stderr, "%s:%s: %s\n", program_name, custom_file,
		   strerror (errno));
	  custom_file = NULL;
	}
      else
	{
	  custom_size = st.st_size;
	  custom_section = bfd_make_section (outbfd, ".nlmcustom");
	  if (custom_section == NULL
	      || ! bfd_set_section_size (outbfd, custom_section, custom_size)
	      || ! bfd_set_section_flags (outbfd, custom_section,
					  SEC_HAS_CONTENTS))
	    bfd_fatal ("custom section");
	}
    }
  if (help_file != NULL)
    {
      help_data = fopen (help_file, "r");
      if (help_data == NULL
	  || fstat (fileno (help_data), &st) < 0)
	{
	  fprintf (stderr, "%s:%s: %s\n", program_name, help_file,
		   strerror (errno));
	  help_file = NULL;
	}
      else
	{
	  help_size = st.st_size;
	  help_section = bfd_make_section (outbfd, ".nlmhelp");
	  if (help_section == NULL
	      || ! bfd_set_section_size (outbfd, help_section, help_size)
	      || ! bfd_set_section_flags (outbfd, help_section,
					  SEC_HAS_CONTENTS))
	    bfd_fatal ("help section");
	  strncpy (nlm_extended_header (outbfd)->stamp, "MeSsAgEs", 8);
	}
    }
  if (message_file != NULL)
    {
      message_data = fopen (message_file, "r");
      if (message_data == NULL
	  || fstat (fileno (message_data), &st) < 0)
	{
	  fprintf (stderr, "%s:%s: %s\n", program_name, message_file,
		   strerror (errno));
	  message_file = NULL;
	}
      else
	{
	  message_size = st.st_size;
	  message_section = bfd_make_section (outbfd, ".nlmmessages");
	  if (message_section == NULL
	      || ! bfd_set_section_size (outbfd, message_section, message_size)
	      || ! bfd_set_section_flags (outbfd, message_section,
					  SEC_HAS_CONTENTS))
	    bfd_fatal ("message section");
	  strncpy (nlm_extended_header (outbfd)->stamp, "MeSsAgEs", 8);
	}
    }
  if (modules != NULL)
    {
      struct string_list *l;

      module_size = 0;
      for (l = modules; l != NULL; l = l->next)
	module_size += strlen (l->string) + 1;
      module_section = bfd_make_section (outbfd, ".nlmmodules");
      if (module_section == NULL
	  || ! bfd_set_section_size (outbfd, module_section, module_size)
	  || ! bfd_set_section_flags (outbfd, module_section,
				      SEC_HAS_CONTENTS))
	bfd_fatal ("module section");
    }
  if (rpc_file != NULL)
    {
      rpc_data = fopen (rpc_file, "r");
      if (rpc_data == NULL
	  || fstat (fileno (rpc_data), &st) < 0)
	{
	  fprintf (stderr, "%s:%s: %s\n", program_name, rpc_file,
		   strerror (errno));
	  rpc_file = NULL;
	}
      else
	{
	  rpc_size = st.st_size;
	  rpc_section = bfd_make_section (outbfd, ".nlmrpc");
	  if (rpc_section == NULL
	      || ! bfd_set_section_size (outbfd, rpc_section, rpc_size)
	      || ! bfd_set_section_flags (outbfd, rpc_section,
					  SEC_HAS_CONTENTS))
	    bfd_fatal ("rpc section");
	  strncpy (nlm_extended_header (outbfd)->stamp, "MeSsAgEs", 8);
	}
    }

  /* Copy over the sections.  */
  bfd_map_over_sections (inbfd, copy_sections, (PTR) outbfd);

  /* Finish up the header information.  */
  if (custom_file != NULL)
    {
      PTR data;

      data = xmalloc (custom_size);
      if (fread (data, 1, custom_size, custom_data) != custom_size)
	fprintf (stderr, "%s:%s: read: %s\n", program_name, custom_file,
		 strerror (errno));
      else
	{
	  if (! bfd_set_section_contents (outbfd, custom_section, data,
					  (file_ptr) 0, custom_size))
	    bfd_fatal ("custom section");
	  nlm_fixed_header (outbfd)->customDataOffset =
	    custom_section->filepos;
	  nlm_fixed_header (outbfd)->customDataSize = custom_size;
	}
      free (data);
    }
  if (! debug_info)
    {
      /* As a special hack, the backend recognizes a debugInfoOffset
	 of -1 to mean that it should not output any debugging
	 information.  This can not be handling by fiddling with the
	 symbol table because exported symbols appear in both the
	 export information and the debugging information.  */
      nlm_fixed_header (outbfd)->debugInfoOffset = (file_ptr) -1;
    }
  if (map_file != NULL)
    fprintf (stderr,
	     "%s: MAP and FULLMAP are not supported; try ld -M\n",
	     program_name);
  if (help_file != NULL)
    {
      PTR data;

      data = xmalloc (help_size);
      if (fread (data, 1, help_size, help_data) != help_size)
	fprintf (stderr, "%s:%s: read: %s\n", program_name, help_file,
		 strerror (errno));
      else
	{
	  if (! bfd_set_section_contents (outbfd, help_section, data,
					  (file_ptr) 0, help_size))
	    bfd_fatal ("help section");
	  nlm_extended_header (outbfd)->helpFileOffset =
	    help_section->filepos;
	  nlm_extended_header (outbfd)->helpFileLength = help_size;
	}
      free (data);
    }
  if (message_file != NULL)
    {
      PTR data;

      data = xmalloc (message_size);
      if (fread (data, 1, message_size, message_data) != message_size)
	fprintf (stderr, "%s:%s: read: %s\n", program_name, message_file,
		 strerror (errno));
      else
	{
	  if (! bfd_set_section_contents (outbfd, message_section, data,
					  (file_ptr) 0, message_size))
	    bfd_fatal ("message section");
	  nlm_extended_header (outbfd)->messageFileOffset =
	    message_section->filepos;
	  nlm_extended_header (outbfd)->messageFileLength = message_size;

	  /* FIXME: Are these offsets correct on all platforms?  Are
	     they 32 bits on all platforms?  What endianness?  */
	  nlm_extended_header (outbfd)->languageID =
	    bfd_h_get_32 (outbfd, (bfd_byte *) data + 106);
	  nlm_extended_header (outbfd)->messageCount =
	    bfd_h_get_32 (outbfd, (bfd_byte *) data + 110);
	}
      free (data);
    }
  if (modules != NULL)
    {
      PTR data;
      char *set;
      struct string_list *l;
      bfd_size_type c;

      data = xmalloc (module_size);
      c = 0;
      set = (char *) data;
      for (l = modules; l != NULL; l = l->next)
	{
	  *set = strlen (l->string);
	  strncpy (set + 1, l->string, *set);
	  set += *set + 1;
	  ++c;
	}
      if (! bfd_set_section_contents (outbfd, module_section, data,
				      (file_ptr) 0, module_size))
	bfd_fatal ("module section");
      nlm_fixed_header (outbfd)->moduleDependencyOffset =
	module_section->filepos;
      nlm_fixed_header (outbfd)->numberOfModuleDependencies = c;
    }
  if (rpc_file != NULL)
    {
      PTR data;

      data = xmalloc (rpc_size);
      if (fread (data, 1, rpc_size, rpc_data) != rpc_size)
	fprintf (stderr, "%s:%s: read: %s\n", program_name, rpc_file,
		 strerror (errno));
      else
	{
	  if (! bfd_set_section_contents (outbfd, rpc_section, data,
					  (file_ptr) 0, rpc_size))
	    bfd_fatal ("rpc section");
	  nlm_extended_header (outbfd)->RPCDataOffset =
	    rpc_section->filepos;
	  nlm_extended_header (outbfd)->RPCDataLength = rpc_size;
	}
      free (data);
    }
  len = strlen (argv[optind + 1]);
  if (len > NLM_MODULE_NAME_SIZE - 2)
    len = NLM_MODULE_NAME_SIZE - 2;
  nlm_fixed_header (outbfd)->moduleName[0] = len;
  strncpy (nlm_fixed_header (outbfd)->moduleName + 1, argv[optind + 1],
	   NLM_MODULE_NAME_SIZE - 2);
  nlm_fixed_header (outbfd)->moduleName[NLM_MODULE_NAME_SIZE - 1] = '\0';
  strncpy (nlm_variable_header (outbfd)->oldThreadName, " LONG",
	   NLM_OLD_THREAD_NAME_LENGTH);

  if (! bfd_close (outbfd))
    bfd_fatal (argv[optind + 1]);
  if (! bfd_close (inbfd))
    bfd_fatal (argv[optind]);

  return 0;
}

/* Display a help message and exit.  */

static void
show_help ()
{
  printf ("%s: Convert an object file into a NetWare Loadable Module\n",
	  program_name);
  show_usage (stdout, 0);
}

/* Show a usage message and exit.  */

static void
show_usage (file, status)
     FILE *file;
     int status;
{
  fprintf (file, "\
Usage: %s [-Hv] [-I format] [-O format] [-T header-file]\n\
       [--input-format=format] [--output-format=format]\n\
       [--header-file=file] [--help] [--version]\n\
       in-file out-file\n",
	   program_name);
  exit (status);
}

/* Select the output format based on the input architecture, machine,
   and endianness.  This chooses the appropriate NLM target.  */

static const char *
select_output_format (arch, mach, bigendian)
     enum bfd_architecture arch;
     long mach;
     boolean bigendian;
{
  switch (arch)
    {
    case bfd_arch_i386:
      return "nlm32-i386";
    default:
      fprintf (stderr, "%s: no default NLM format for %s\n",
	       program_name, bfd_printable_arch_mach (arch, mach));
      exit (1);
      /* Avoid warning.  */
      return NULL;
    }
  /*NOTREACHED*/
}

/* The BFD sections are copied in two passes.  This function sets up
   the section name, size, etc.  */

static void
setup_sections (inbfd, insec, data_ptr)
     bfd *inbfd;
     asection *insec;
     PTR data_ptr;
{
  bfd *outbfd = (bfd *) data_ptr;
  asection *outsec;

  outsec = bfd_get_section_by_name (outbfd, bfd_section_name (inbfd, insec));
  if (outsec == NULL)
    {
      outsec = bfd_make_section (outbfd, bfd_section_name (inbfd, insec));
      if (outsec == NULL)
	bfd_fatal ("make section");
    }

  insec->output_section = outsec;
  insec->output_offset = 0;

  if (! bfd_set_section_size (outbfd, outsec,
			      bfd_section_size (inbfd, insec)))
    bfd_fatal ("set section size");

  if (! bfd_set_section_vma (outbfd, outsec,
			     bfd_section_vma (inbfd, insec)))
    bfd_fatal ("set section vma");

  if (! bfd_set_section_alignment (outbfd, outsec,
				 bfd_section_alignment (inbfd, insec)))
    bfd_fatal ("set section alignment");

  if (! bfd_set_section_flags (outbfd, outsec,
			       bfd_get_section_flags (inbfd, insec)))
    bfd_fatal ("set section flags");
}

/* Copy the section contents.  */

static void
copy_sections (inbfd, insec, data_ptr)
     bfd *inbfd;
     asection *insec;
     PTR data_ptr;
{
  bfd *outbfd = (bfd *) data_ptr;
  asection *outsec;
  bfd_size_type size;
  PTR contents;
  bfd_size_type reloc_size;

  outsec = bfd_get_section_by_name (outbfd, bfd_section_name (inbfd, insec));
  assert (outsec != NULL);

  size = bfd_get_section_size_before_reloc (insec);
  if (size == 0)
    return;

  /* FIXME: Why are these necessary?  */
  insec->_cooked_size = insec->_raw_size;
  insec->reloc_done = true;

  if ((bfd_get_section_flags (inbfd, insec) & SEC_HAS_CONTENTS) == 0)
    contents = NULL;
  else
    {
      contents = xmalloc (size);
      if (! bfd_get_section_contents (inbfd, insec, contents,
				      (file_ptr) 0, size))
	bfd_fatal (bfd_get_filename (inbfd));
    }

  reloc_size = bfd_get_reloc_upper_bound (inbfd, insec);
  if (reloc_size == 0)
    bfd_set_reloc (outbfd, outsec, (arelent **) NULL, 0);
  else
    {
      arelent **relocs;
      bfd_size_type reloc_count;

      relocs = (arelent **) xmalloc (reloc_size);
      reloc_count = bfd_canonicalize_reloc (inbfd, insec, relocs, symbols);
      mangle_relocs (outbfd, relocs, reloc_count, (char *) contents, size);
      bfd_set_reloc (outbfd, outsec, relocs, reloc_count);
    }

  if (contents != NULL)
    {
      if (! bfd_set_section_contents (outbfd, outsec, contents,
				      (file_ptr) 0, size))
	bfd_fatal (bfd_get_filename (outbfd));
      free (contents);
    }
}

/* Some, perhaps all, NetWare targets require changing the relocs used
   by the input formats.  */

static void
mangle_relocs (outbfd, relocs, reloc_count, contents, contents_size)
     bfd *outbfd;
     arelent **relocs;
     bfd_size_type reloc_count;
     char *contents;
     bfd_size_type contents_size;
{
  switch (bfd_get_arch (outbfd))
    {
    case bfd_arch_i386:
      i386_mangle_relocs (outbfd, relocs, reloc_count, contents,
			  contents_size);
      break;
    default:
      break;
    }
}

/* NetWare on the i386 supports a restricted set of relocs, which are
   different from those used on other i386 targets.  This routine
   converts the relocs.  It is, obviously, very target dependent.  At
   the moment, the nlm32-i386 backend performs similar translations;
   however, it is more reliable and efficient to do them here.  */

static reloc_howto_type nlm_i386_pcrel_howto =
  HOWTO (1,			/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 0,			/* special_function */
	 "DISP32",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 true);			/* pcrel_offset */

static void
i386_mangle_relocs (outbfd, relocs, reloc_count, contents, contents_size)
     bfd *outbfd;
     arelent **relocs;
     bfd_size_type reloc_count;
     char *contents;
     bfd_size_type contents_size;
{
  while (reloc_count-- != 0)
    {
      arelent *rel;
      asymbol *sym;
      bfd_vma addend;

      rel = *relocs++;
      sym = *rel->sym_ptr_ptr;

      /* Note that no serious harm will ensue if we fail to change a
	 reloc.  The backend will fail when writing out the reloc.  */

      /* Make sure this reloc is within the data we have.  We use only
	 4 byte relocs here, so we insist on having 4 bytes.  */
      if (rel->address + 4 > contents_size)
	continue;

      /* NetWare doesn't support reloc addends, so we get rid of them
	 here by simply adding them into the object data.  We handle
	 the symbol value, if any, the same way.  */
      addend = rel->addend;
      if (! bfd_is_com_section (bfd_get_section (sym)))
	addend += sym->value;

      if (addend != 0
	  && rel->howto != NULL
	  && rel->howto->rightshift == 0
	  && rel->howto->size == 2
	  && rel->howto->bitsize == 32
	  && rel->howto->bitpos == 0
	  && rel->howto->src_mask == 0xffffffff
	  && rel->howto->dst_mask == 0xffffffff)
	{
	  bfd_vma val;

	  val = bfd_get_32 (outbfd, contents + rel->address);
	  val += addend;
	  bfd_put_32 (outbfd, val, contents + rel->address);

	  /* Adjust the reloc for the changes we just made.  */
	  rel->addend = 0;
	  if (! bfd_is_com_section (bfd_get_section (sym))
	      && sym->value != 0)
	    rel->sym_ptr_ptr = bfd_get_section (sym)->symbol_ptr_ptr;
	}

      /* NetWare uses a reloc with pcrel_offset set.  We adjust
	 pc_relative relocs accordingly.  We are going to change the
	 howto field, so we can only do this if the current one is
	 compatible.  We should check that special_function is NULL
	 here, but at the moment coff-i386 uses a special_function
	 which does not affect what we are doing here.  */
      if (rel->howto != NULL
	  && rel->howto->pc_relative
	  && ! rel->howto->pcrel_offset
	  && rel->howto->rightshift == 0
	  && rel->howto->size == 2
	  && rel->howto->bitsize == 32
	  && rel->howto->bitpos == 0
	  && rel->howto->src_mask == 0xffffffff
	  && rel->howto->dst_mask == 0xffffffff)
	{
	  bfd_vma val;

	  /* When pcrel_offset is not set, it means that the negative
	     of the address of the memory location is stored in the
	     memory location.  We must add it back in.  */
	  val = bfd_get_32 (outbfd, contents + rel->address);
	  val += rel->address;
	  bfd_put_32 (outbfd, val, contents + rel->address);

	  /* We must change to a new howto.  */
	  rel->howto = &nlm_i386_pcrel_howto;
	}
    }
}
