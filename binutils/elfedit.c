/* elfedit.c -- Update the ELF header of an ELF format file
   Copyright 2010
   Free Software Foundation, Inc.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "config.h"
#include "sysdep.h"
#include <assert.h>
#include <sys/stat.h>

#if __GNUC__ >= 2
/* Define BFD64 here, even if our default architecture is 32 bit ELF
   as this will allow us to read in and parse 64bit and 32bit ELF files.
   Only do this if we believe that the compiler can support a 64 bit
   data type.  For now we only rely on GCC being able to do this.  */
#define BFD64
#endif

#include "bfd.h"
#include "bucomm.h"

#include "elf/common.h"
#include "elf/external.h"
#include "elf/internal.h"


#include "aout/ar.h"

#include "getopt.h"
#include "libiberty.h"
#include "safe-ctype.h"
#include "filenames.h"

char * program_name = "elfedit";
static long archive_file_offset;
static unsigned long archive_file_size;
static Elf_Internal_Ehdr elf_header;
static Elf32_External_Ehdr ehdr32;
static Elf64_External_Ehdr ehdr64;
static int input_elf_machine = -1;
static int output_elf_machine = -1;
static int input_elf_class = -1;

#define streq(a,b)	  (strcmp ((a), (b)) == 0)
#define strneq(a,b,n)	  (strncmp ((a), (b), (n)) == 0)
#define const_strneq(a,b) (strncmp ((a), (b), sizeof (b) - 1) == 0)

void
non_fatal (const char *message, ...)
{
  va_list args;

  va_start (args, message);
  fprintf (stderr, _("%s: Error: "), program_name);
  vfprintf (stderr, message, args);
  va_end (args);
}

#define BYTE_GET(field)		byte_get (field, sizeof (field))
#define BYTE_PUT(field, val)	byte_put (field, val, sizeof (field))

static bfd_vma (*byte_get) (unsigned char *, int);
static void (*byte_put) (unsigned char *, bfd_vma, int);

static bfd_vma
byte_get_little_endian (unsigned char *field, int size)
{
  switch (size)
    {
    case 1:
      return *field;

    case 2:
      return  ((unsigned int) (field[0]))
	|    (((unsigned int) (field[1])) << 8);

    case 4:
      return  ((unsigned long) (field[0]))
	|    (((unsigned long) (field[1])) << 8)
	|    (((unsigned long) (field[2])) << 16)
	|    (((unsigned long) (field[3])) << 24);

    case 8:
      if (sizeof (bfd_vma) == 8)
	return  ((bfd_vma) (field[0]))
	  |    (((bfd_vma) (field[1])) << 8)
	  |    (((bfd_vma) (field[2])) << 16)
	  |    (((bfd_vma) (field[3])) << 24)
	  |    (((bfd_vma) (field[4])) << 32)
	  |    (((bfd_vma) (field[5])) << 40)
	  |    (((bfd_vma) (field[6])) << 48)
	  |    (((bfd_vma) (field[7])) << 56);
      else if (sizeof (bfd_vma) == 4)
	/* We want to extract data from an 8 byte wide field and
	   place it into a 4 byte wide field.  Since this is a little
	   endian source we can just use the 4 byte extraction code.  */
	return  ((unsigned long) (field[0]))
	  |    (((unsigned long) (field[1])) << 8)
	  |    (((unsigned long) (field[2])) << 16)
	  |    (((unsigned long) (field[3])) << 24);

    default:
      non_fatal (_("Unhandled data length: %d\n"), size);
      abort ();
    }
}

static bfd_vma
byte_get_big_endian (unsigned char *field, int size)
{
  switch (size)
    {
    case 1:
      return *field;

    case 2:
      return ((unsigned int) (field[1])) | (((int) (field[0])) << 8);

    case 4:
      return ((unsigned long) (field[3]))
	|   (((unsigned long) (field[2])) << 8)
	|   (((unsigned long) (field[1])) << 16)
	|   (((unsigned long) (field[0])) << 24);

    case 8:
      if (sizeof (bfd_vma) == 8)
	return ((bfd_vma) (field[7]))
	  |   (((bfd_vma) (field[6])) << 8)
	  |   (((bfd_vma) (field[5])) << 16)
	  |   (((bfd_vma) (field[4])) << 24)
	  |   (((bfd_vma) (field[3])) << 32)
	  |   (((bfd_vma) (field[2])) << 40)
	  |   (((bfd_vma) (field[1])) << 48)
	  |   (((bfd_vma) (field[0])) << 56);
      else if (sizeof (bfd_vma) == 4)
	{
	  /* Although we are extracing data from an 8 byte wide field,
	     we are returning only 4 bytes of data.  */
	  field += 4;
	  return ((unsigned long) (field[3]))
	    |   (((unsigned long) (field[2])) << 8)
	    |   (((unsigned long) (field[1])) << 16)
	    |   (((unsigned long) (field[0])) << 24);
	}

    default:
      non_fatal (_("Unhandled data length: %d\n"), size);
      abort ();
    }
}

static void
byte_put_little_endian (unsigned char * field, bfd_vma value, int size)
{
  switch (size)
    {
    case 8:
      field[7] = (((value >> 24) >> 24) >> 8) & 0xff;
      field[6] = ((value >> 24) >> 24) & 0xff;
      field[5] = ((value >> 24) >> 16) & 0xff;
      field[4] = ((value >> 24) >> 8) & 0xff;
      /* Fall through.  */
    case 4:
      field[3] = (value >> 24) & 0xff;
      field[2] = (value >> 16) & 0xff;
      /* Fall through.  */
    case 2:
      field[1] = (value >> 8) & 0xff;
      /* Fall through.  */
    case 1:
      field[0] = value & 0xff;
      break;

    default:
      non_fatal (_("Unhandled data length: %d\n"), size);
      abort ();
    }
}

static void
byte_put_big_endian (unsigned char * field, bfd_vma value, int size)
{
  switch (size)
    {
    case 8:
      field[7] = value & 0xff;
      field[6] = (value >> 8) & 0xff;
      field[5] = (value >> 16) & 0xff;
      field[4] = (value >> 24) & 0xff;
      value >>= 16;
      value >>= 16;
      /* Fall through.  */
    case 4:
      field[3] = value & 0xff;
      field[2] = (value >> 8) & 0xff;
      value >>= 16;
      /* Fall through.  */
    case 2:
      field[1] = value & 0xff;
      value >>= 8;
      /* Fall through.  */
    case 1:
      field[0] = value & 0xff;
      break;

    default:
      non_fatal (_("Unhandled data length: %d\n"), size);
      abort ();
    }
}

static int
update_elf_header (const char *file_name, FILE *file)
{
  int status;

  if (elf_header.e_ident[EI_MAG0] != ELFMAG0
      || elf_header.e_ident[EI_MAG1] != ELFMAG1
      || elf_header.e_ident[EI_MAG2] != ELFMAG2
      || elf_header.e_ident[EI_MAG3] != ELFMAG3)
    {
      non_fatal
	(_("%s: Not an ELF file - wrong magic bytes at the start\n"),
	 file_name);
      return 0;
    }

  if (elf_header.e_ident[EI_VERSION] != EV_CURRENT)
    {
      non_fatal
	(_("%s: Unsupported EI_VERSION: %d is not %d\n"),
	 file_name, elf_header.e_ident[EI_VERSION],
	 EV_CURRENT);
      return 0;
    }

  /* Skip if class doesn't match. */
  if (input_elf_class == -1)
    input_elf_class = elf_header.e_ident[EI_CLASS];
  else
  if (elf_header.e_ident[EI_CLASS] != input_elf_class)
    {
      non_fatal
	(_("%s: Unmatched EI_CLASS: %d is not %d\n"),
	 file_name, elf_header.e_ident[EI_CLASS],
	 input_elf_class);
      return 0;
    }

  /* Return if e_machine is the same as output_elf_machine.  */
  if (output_elf_machine == elf_header.e_machine)
    return 1;

  /* Skip if e_machine doesn't match. */
  if (input_elf_machine == -1)
    input_elf_machine = elf_header.e_machine;
  else if (elf_header.e_machine != input_elf_machine)
    {
      non_fatal
	(_("%s: Unmatched e_machine: %d is not %d\n"),
	 file_name, elf_header.e_machine, input_elf_machine);
      return 0;
    }

  /* Update e_machine.  */
  switch (input_elf_class)
    {
    default:
      /* We should never get here.  */
      abort ();
      break;
    case ELFCLASS32:
      BYTE_PUT (ehdr32.e_machine, output_elf_machine);
      status = fwrite (&ehdr32, sizeof (ehdr32), 1, file) == 1;
      break;
    case ELFCLASS64:
      BYTE_PUT (ehdr64.e_machine, output_elf_machine);
      status = fwrite (&ehdr64, sizeof (ehdr64), 1, file) == 1;
      break;
    }

  if (status != 1)
    non_fatal (_("%s: Failed to update ELF header: %s\n"),
	       file_name, strerror (errno));

  return status;
}

static int
get_file_header (FILE * file)
{
  /* Read in the identity array.  */
  if (fread (elf_header.e_ident, EI_NIDENT, 1, file) != 1)
    return 0;

  /* Determine how to read the rest of the header.  */
  switch (elf_header.e_ident[EI_DATA])
    {
    default: /* fall through */
    case ELFDATANONE: /* fall through */
    case ELFDATA2LSB:
      byte_get = byte_get_little_endian;
      byte_put = byte_put_little_endian;
      break;
    case ELFDATA2MSB:
      byte_get = byte_get_big_endian;
      byte_put = byte_put_big_endian;
      break;
    }

  /* Read in the rest of the header.  For now we only support 32 bit
     and 64 bit ELF files.  */
  switch (elf_header.e_ident[EI_CLASS])
    {
    default:
      non_fatal (_("Unsupported EI_CLASS: %d\n"),
		 elf_header.e_ident[EI_CLASS]);
      return 0;

    case ELFCLASS32:
      if (fread (ehdr32.e_type, sizeof (ehdr32) - EI_NIDENT,
		 1, file) != 1)
	return 0;

      elf_header.e_type      = BYTE_GET (ehdr32.e_type);
      elf_header.e_machine   = BYTE_GET (ehdr32.e_machine);
      elf_header.e_version   = BYTE_GET (ehdr32.e_version);
      elf_header.e_entry     = BYTE_GET (ehdr32.e_entry);
      elf_header.e_phoff     = BYTE_GET (ehdr32.e_phoff);
      elf_header.e_shoff     = BYTE_GET (ehdr32.e_shoff);
      elf_header.e_flags     = BYTE_GET (ehdr32.e_flags);
      elf_header.e_ehsize    = BYTE_GET (ehdr32.e_ehsize);
      elf_header.e_phentsize = BYTE_GET (ehdr32.e_phentsize);
      elf_header.e_phnum     = BYTE_GET (ehdr32.e_phnum);
      elf_header.e_shentsize = BYTE_GET (ehdr32.e_shentsize);
      elf_header.e_shnum     = BYTE_GET (ehdr32.e_shnum);
      elf_header.e_shstrndx  = BYTE_GET (ehdr32.e_shstrndx);

      memcpy (&ehdr32, &elf_header, EI_NIDENT);
      break;

    case ELFCLASS64:
      /* If we have been compiled with sizeof (bfd_vma) == 4, then
	 we will not be able to cope with the 64bit data found in
	 64 ELF files.  Detect this now and abort before we start
	 overwriting things.  */
      if (sizeof (bfd_vma) < 8)
	{
	  non_fatal (_("This executable has been built without support for a\n\
64 bit data type and so it cannot process 64 bit ELF files.\n"));
	  return 0;
	}

      if (fread (ehdr64.e_type, sizeof (ehdr64) - EI_NIDENT,
		 1, file) != 1)
	return 0;

      elf_header.e_type      = BYTE_GET (ehdr64.e_type);
      elf_header.e_machine   = BYTE_GET (ehdr64.e_machine);
      elf_header.e_version   = BYTE_GET (ehdr64.e_version);
      elf_header.e_entry     = BYTE_GET (ehdr64.e_entry);
      elf_header.e_phoff     = BYTE_GET (ehdr64.e_phoff);
      elf_header.e_shoff     = BYTE_GET (ehdr64.e_shoff);
      elf_header.e_flags     = BYTE_GET (ehdr64.e_flags);
      elf_header.e_ehsize    = BYTE_GET (ehdr64.e_ehsize);
      elf_header.e_phentsize = BYTE_GET (ehdr64.e_phentsize);
      elf_header.e_phnum     = BYTE_GET (ehdr64.e_phnum);
      elf_header.e_shentsize = BYTE_GET (ehdr64.e_shentsize);
      elf_header.e_shnum     = BYTE_GET (ehdr64.e_shnum);
      elf_header.e_shstrndx  = BYTE_GET (ehdr64.e_shstrndx);

      memcpy (&ehdr64, &elf_header, EI_NIDENT);
      break;
    }
  return 1;
}

/* Process one ELF object file according to the command line options.
   This file may actually be stored in an archive.  The file is
   positioned at the start of the ELF object.  */

static int
process_object (const char *file_name, FILE *file)
{
  /* Rememeber where we are.  */
  long offset = ftell (file);

  if (! get_file_header (file))
    {
      non_fatal (_("%s: Failed to read ELF header\n"), file_name);
      return 1;
    }

  /* Go to the position of the ELF header.  */
  if (fseek (file, offset, SEEK_SET) != 0)
    {
      non_fatal (_("%s: Failed to seek to ELF header\n"), file_name);
    }

  if (! update_elf_header (file_name, file))
    return 1;

  return 0;
}

/* Return the path name for a proxy entry in a thin archive, adjusted relative
   to the path name of the thin archive itself if necessary.  Always returns
   a pointer to malloc'ed memory.  */

static char *
adjust_relative_path (const char *file_name, char * name, int name_len)
{
  char * member_file_name;
  const char * base_name = lbasename (file_name);

  /* This is a proxy entry for a thin archive member.
     If the extended name table contains an absolute path
     name, or if the archive is in the current directory,
     use the path name as given.  Otherwise, we need to
     find the member relative to the directory where the
     archive is located.  */
  if (IS_ABSOLUTE_PATH (name) || base_name == file_name)
    {
      member_file_name = malloc (name_len + 1);
      if (member_file_name == NULL)
        {
          non_fatal (_("Out of memory\n"));
          return NULL;
        }
      memcpy (member_file_name, name, name_len);
      member_file_name[name_len] = '\0';
    }
  else
    {
      /* Concatenate the path components of the archive file name
         to the relative path name from the extended name table.  */
      size_t prefix_len = base_name - file_name;
      member_file_name = malloc (prefix_len + name_len + 1);
      if (member_file_name == NULL)
        {
          non_fatal (_("Out of memory\n"));
          return NULL;
        }
      memcpy (member_file_name, file_name, prefix_len);
      memcpy (member_file_name + prefix_len, name, name_len);
      member_file_name[prefix_len + name_len] = '\0';
    }
  return member_file_name;
}

/* Structure to hold information about an archive file.  */

struct archive_info
{
  char * file_name;                     /* Archive file name.  */
  FILE * file;                          /* Open file descriptor.  */
  unsigned long index_num;              /* Number of symbols in table.  */
  unsigned long * index_array;          /* The array of member offsets.  */
  char * sym_table;                     /* The symbol table.  */
  unsigned long sym_size;               /* Size of the symbol table.  */
  char * longnames;                     /* The long file names table.  */
  unsigned long longnames_size;         /* Size of the long file names table.  */
  unsigned long nested_member_origin;   /* Origin in the nested archive of the current member.  */
  unsigned long next_arhdr_offset;      /* Offset of the next archive header.  */
  bfd_boolean is_thin_archive;          /* TRUE if this is a thin archive.  */
  struct ar_hdr arhdr;                  /* Current archive header.  */
};

/* Read the symbol table and long-name table from an archive.  */

static int
setup_archive (struct archive_info * arch, const char * file_name,
	       FILE * file, bfd_boolean is_thin_archive)
{
  size_t got;
  unsigned long size;

  arch->file_name = strdup (file_name);
  arch->file = file;
  arch->index_num = 0;
  arch->index_array = NULL;
  arch->sym_table = NULL;
  arch->sym_size = 0;
  arch->longnames = NULL;
  arch->longnames_size = 0;
  arch->nested_member_origin = 0;
  arch->is_thin_archive = is_thin_archive;
  arch->next_arhdr_offset = SARMAG;

  /* Read the first archive member header.  */
  if (fseek (file, SARMAG, SEEK_SET) != 0)
    {
      non_fatal (_("%s: failed to seek to first archive header\n"),
		 file_name);
      return 1;
    }
  got = fread (&arch->arhdr, 1, sizeof arch->arhdr, file);
  if (got != sizeof arch->arhdr)
    {
      if (got == 0)
	return 0;

      non_fatal (_("%s: failed to read archive header\n"), file_name);
      return 1;
    }

  /* See if this is the archive symbol table.  */
  if (const_strneq (arch->arhdr.ar_name, "/               ")
      || const_strneq (arch->arhdr.ar_name, "/SYM64/         "))
    {
      size = strtoul (arch->arhdr.ar_size, NULL, 10);
      size = size + (size & 1);

      arch->next_arhdr_offset += sizeof arch->arhdr + size;

      if (fseek (file, size, SEEK_CUR) != 0)
	{
	  non_fatal (_("%s: failed to skip archive symbol table\n"),
		     file_name);
	  return 1;
	}

      /* Read the next archive header.  */
      got = fread (&arch->arhdr, 1, sizeof arch->arhdr, file);
      if (got != sizeof arch->arhdr)
	{
	  if (got == 0)
            return 0;
	  non_fatal (_("%s: failed to read archive header following archive index\n"),
		     file_name);
	  return 1;
	}
    }

  if (const_strneq (arch->arhdr.ar_name, "//              "))
    {
      /* This is the archive string table holding long member names.  */
      arch->longnames_size = strtoul (arch->arhdr.ar_size, NULL, 10);
      arch->next_arhdr_offset += sizeof arch->arhdr + arch->longnames_size;

      arch->longnames = malloc (arch->longnames_size);
      if (arch->longnames == NULL)
	{
	  non_fatal (_("Out of memory reading long symbol names in archive\n"));
	  return 1;
	}

      if (fread (arch->longnames, arch->longnames_size, 1, file) != 1)
	{
	  free (arch->longnames);
	  arch->longnames = NULL;
	  non_fatal (_("%s: failed to read long symbol name string table\n")
		     , file_name);
	  return 1;
	}

      if ((arch->longnames_size & 1) != 0)
	getc (file);
    }

  return 0;
}

/* Release the memory used for the archive information.  */

static void
release_archive (struct archive_info * arch)
{
  if (arch->file_name != NULL)
    free (arch->file_name);
  if (arch->index_array != NULL)
    free (arch->index_array);
  if (arch->sym_table != NULL)
    free (arch->sym_table);
  if (arch->longnames != NULL)
    free (arch->longnames);
}

/* Open and setup a nested archive, if not already open.  */

static int
setup_nested_archive (struct archive_info * nested_arch, char * member_file_name)
{
  FILE * member_file;

  /* Have we already setup this archive?  */
  if (nested_arch->file_name != NULL
      && streq (nested_arch->file_name, member_file_name))
    return 0;

  /* Close previous file and discard cached information.  */
  if (nested_arch->file != NULL)
    fclose (nested_arch->file);
  release_archive (nested_arch);

  member_file = fopen (member_file_name, "r+b");
  if (member_file == NULL)
    return 1;
  return setup_archive (nested_arch, member_file_name, member_file,
			FALSE);
}

static char *
get_archive_member_name_at (struct archive_info *  arch,
			    unsigned long          offset,
			    struct archive_info *  nested_arch);

/* Get the name of an archive member from the current archive header.
   For simple names, this will modify the ar_name field of the current
   archive header.  For long names, it will return a pointer to the
   longnames table.  For nested archives, it will open the nested archive
   and get the name recursively.  NESTED_ARCH is a single-entry cache so
   we don't keep rereading the same information from a nested archive.  */

static char *
get_archive_member_name (struct archive_info *  arch,
                         struct archive_info *  nested_arch)
{
  unsigned long j, k;

  if (arch->arhdr.ar_name[0] == '/')
    {
      /* We have a long name.  */
      char * endp;
      char * member_file_name;
      char * member_name;

      arch->nested_member_origin = 0;
      k = j = strtoul (arch->arhdr.ar_name + 1, &endp, 10);
      if (arch->is_thin_archive && endp != NULL && * endp == ':')
        arch->nested_member_origin = strtoul (endp + 1, NULL, 10);

      while ((j < arch->longnames_size)
             && (arch->longnames[j] != '\n')
             && (arch->longnames[j] != '\0'))
        j++;
      if (arch->longnames[j-1] == '/')
        j--;
      arch->longnames[j] = '\0';

      if (!arch->is_thin_archive || arch->nested_member_origin == 0)
        return arch->longnames + k;

      /* This is a proxy for a member of a nested archive.
         Find the name of the member in that archive.  */
      member_file_name = adjust_relative_path (arch->file_name,
					       arch->longnames + k,
					       j - k);
      if (member_file_name != NULL
          && setup_nested_archive (nested_arch, member_file_name) == 0
          && (member_name = get_archive_member_name_at (nested_arch,
							arch->nested_member_origin,
							NULL)) != NULL)
        {
          free (member_file_name);
          return member_name;
        }
      free (member_file_name);

      /* Last resort: just return the name of the nested archive.  */
      return arch->longnames + k;
    }

  /* We have a normal (short) name.  */
  j = 0;
  while ((arch->arhdr.ar_name[j] != '/') && (j < 16))
    j++;
  arch->arhdr.ar_name[j] = '\0';
  return arch->arhdr.ar_name;
}

/* Get the name of an archive member at a given OFFSET within an
   archive ARCH.  */

static char *
get_archive_member_name_at (struct archive_info * arch,
                            unsigned long         offset,
			    struct archive_info * nested_arch)
{
  size_t got;

  if (fseek (arch->file, offset, SEEK_SET) != 0)
    {
      non_fatal (_("%s: failed to seek to next file name\n"),
		 arch->file_name);
      return NULL;
    }
  got = fread (&arch->arhdr, 1, sizeof arch->arhdr, arch->file);
  if (got != sizeof arch->arhdr)
    {
      non_fatal (_("%s: failed to read archive header\n"),
		 arch->file_name);
      return NULL;
    }
  if (memcmp (arch->arhdr.ar_fmag, ARFMAG, 2) != 0)
    {
      non_fatal (_("%s: did not find a valid archive header\n"),
		 arch->file_name);
      return NULL;
    }

  return get_archive_member_name (arch, nested_arch);
}

/* Construct a string showing the name of the archive member, qualified
   with the name of the containing archive file.  For thin archives, we
   use square brackets to denote the indirection.  For nested archives,
   we show the qualified name of the external member inside the square
   brackets (e.g., "thin.a[normal.a(foo.o)]").  */

static char *
make_qualified_name (struct archive_info * arch,
                     struct archive_info * nested_arch,
                     char * member_name)
{
  size_t len;
  char * name;

  len = strlen (arch->file_name) + strlen (member_name) + 3;
  if (arch->is_thin_archive && arch->nested_member_origin != 0)
    len += strlen (nested_arch->file_name) + 2;

  name = malloc (len);
  if (name == NULL)
    {
      non_fatal (_("Out of memory\n"));
      return NULL;
    }

  if (arch->is_thin_archive && arch->nested_member_origin != 0)
    snprintf (name, len, "%s[%s(%s)]", arch->file_name, nested_arch->file_name, member_name);
  else if (arch->is_thin_archive)
    snprintf (name, len, "%s[%s]", arch->file_name, member_name);
  else
    snprintf (name, len, "%s(%s)", arch->file_name, member_name);

  return name;
}

/* Process an ELF archive.
   On entry the file is positioned just after the ARMAG string.  */

static int
process_archive (const char * file_name, FILE * file,
		 bfd_boolean is_thin_archive)
{
  struct archive_info arch;
  struct archive_info nested_arch;
  size_t got;
  size_t file_name_size;
  int ret;

  /* The ARCH structure is used to hold information about this archive.  */
  arch.file_name = NULL;
  arch.file = NULL;
  arch.index_array = NULL;
  arch.sym_table = NULL;
  arch.longnames = NULL;

  /* The NESTED_ARCH structure is used as a single-item cache of information
     about a nested archive (when members of a thin archive reside within
     another regular archive file).  */
  nested_arch.file_name = NULL;
  nested_arch.file = NULL;
  nested_arch.index_array = NULL;
  nested_arch.sym_table = NULL;
  nested_arch.longnames = NULL;

  if (setup_archive (&arch, file_name, file, is_thin_archive) != 0)
    {
      ret = 1;
      goto out;
    }

  file_name_size = strlen (file_name);
  ret = 0;

  while (1)
    {
      char * name;
      size_t namelen;
      char * qualified_name;

      /* Read the next archive header.  */
      if (fseek (file, arch.next_arhdr_offset, SEEK_SET) != 0)
        {
          non_fatal (_("%s: failed to seek to next archive header\n"),
		     file_name);
          return 1;
        }
      got = fread (&arch.arhdr, 1, sizeof arch.arhdr, file);
      if (got != sizeof arch.arhdr)
        {
          if (got == 0)
	    break;
          non_fatal (_("%s: failed to read archive header\n"),
		     file_name);
          ret = 1;
          break;
        }
      if (memcmp (arch.arhdr.ar_fmag, ARFMAG, 2) != 0)
        {
          non_fatal (_("%s: did not find a valid archive header\n"),
		     arch.file_name);
          ret = 1;
          break;
        }

      arch.next_arhdr_offset += sizeof arch.arhdr;

      archive_file_size = strtoul (arch.arhdr.ar_size, NULL, 10);
      if (archive_file_size & 01)
        ++archive_file_size;

      name = get_archive_member_name (&arch, &nested_arch);
      if (name == NULL)
	{
	  non_fatal (_("%s: bad archive file name\n"), file_name);
	  ret = 1;
	  break;
	}
      namelen = strlen (name);

      qualified_name = make_qualified_name (&arch, &nested_arch, name);
      if (qualified_name == NULL)
	{
	  non_fatal (_("%s: bad archive file name\n"), file_name);
	  ret = 1;
	  break;
	}

      if (is_thin_archive && arch.nested_member_origin == 0)
        {
          /* This is a proxy for an external member of a thin archive.  */
          FILE *member_file;
          char *member_file_name = adjust_relative_path (file_name,
							 name, namelen);
          if (member_file_name == NULL)
            {
              ret = 1;
              break;
            }

          member_file = fopen (member_file_name, "r+b");
          if (member_file == NULL)
            {
              non_fatal (_("Input file '%s' is not readable\n"),
			 member_file_name);
              free (member_file_name);
              ret = 1;
              break;
            }

          archive_file_offset = arch.nested_member_origin;

          ret |= process_object (qualified_name, member_file);

          fclose (member_file);
          free (member_file_name);
        }
      else if (is_thin_archive)
        {
          /* This is a proxy for a member of a nested archive.  */
          archive_file_offset = arch.nested_member_origin + sizeof arch.arhdr;

          /* The nested archive file will have been opened and setup by
             get_archive_member_name.  */
          if (fseek (nested_arch.file, archive_file_offset,
		     SEEK_SET) != 0)
            {
              non_fatal (_("%s: failed to seek to archive member\n"),
			 nested_arch.file_name);
              ret = 1;
              break;
            }

          ret |= process_object (qualified_name, nested_arch.file);
        }
      else
        {
          archive_file_offset = arch.next_arhdr_offset;
          arch.next_arhdr_offset += archive_file_size;

          ret |= process_object (qualified_name, file);
        }

      free (qualified_name);
    }

 out:
  if (nested_arch.file != NULL)
    fclose (nested_arch.file);
  release_archive (&nested_arch);
  release_archive (&arch);

  return ret;
}

static int
check_file (const char *file_name, struct stat *statbuf_p)
{
  struct stat statbuf;

  if (statbuf_p == NULL)
    statbuf_p = &statbuf;

  if (stat (file_name, statbuf_p) < 0)
    {
      if (errno == ENOENT)
	non_fatal (_("'%s': No such file\n"), file_name);
      else
	non_fatal (_("Could not locate '%s'.  System error message: %s\n"),
		   file_name, strerror (errno));
      return 1;
    }

  if (! S_ISREG (statbuf_p->st_mode))
    {
      non_fatal (_("'%s' is not an ordinary file\n"), file_name);
      return 1;
    }

  return 0;
}

static int
process_file (const char *file_name)
{
  FILE * file;
  char armag[SARMAG];
  int ret;

  if (check_file (file_name, NULL))
    return 1;

  file = fopen (file_name, "r+b");
  if (file == NULL)
    {
      non_fatal (_("Input file '%s' is not readable\n"), file_name);
      return 1;
    }

  if (fread (armag, SARMAG, 1, file) != 1)
    {
      non_fatal (_("%s: Failed to read file's magic number\n"),
		 file_name);
      fclose (file);
      return 1;
    }

  if (memcmp (armag, ARMAG, SARMAG) == 0)
    ret = process_archive (file_name, file, FALSE);
  else if (memcmp (armag, ARMAGT, SARMAG) == 0)
    ret = process_archive (file_name, file, TRUE);
  else
    {
      rewind (file);
      archive_file_size = archive_file_offset = 0;
      ret = process_object (file_name, file);
    }

  fclose (file);

  return ret;
}

/* Return EM_XXX for a machine string, MACH.  */

static int
elf_machine (const char *mach)
{
  if (strcasecmp (mach, "l1om") == 0)
    return EM_L1OM;
  if (strcasecmp (mach, "x86_64") == 0)
    return EM_X86_64;
  if (strcasecmp (mach, "x86-64") == 0)
    return EM_X86_64;
  if (strcasecmp (mach, "none") == 0)
    return EM_NONE;

  non_fatal (_("Unknown machine type: %s\n"), mach);

  return -1;
}

/* Return ELF class for a machine type, MACH.  */

static int
elf_class (int mach)
{
  switch (mach)
    {
    case EM_L1OM:
    case EM_X86_64:
      return ELFCLASS64;
    case EM_NONE:
      return ELFCLASSNONE;
    default:
      non_fatal (_("Unknown machine type: %d\n"), mach);
      return -1;
    }
}

enum command_line_switch
  {
    OPTION_INPUT_MACH = 150,
    OPTION_OUTPUT_MACH
  };

static struct option options[] =
{
  {"input-mach",	required_argument, 0, OPTION_INPUT_MACH},
  {"output-mach",	required_argument, 0, OPTION_OUTPUT_MACH},
  {"version",		no_argument, 0, 'v'},
  {"help",		no_argument, 0, 'h'},
  {0,			no_argument, 0, 0}
};

static void
usage (FILE *stream, int exit_status)
{
  fprintf (stream, _("Usage: %s [option(s)] --output-mach <machine> elffile(s)\n"),
	   program_name);
  fprintf (stream, _(" Update the ELF header of ELF files\n"));
  fprintf (stream, _(" The options are:\n"));
  fprintf (stream, _("\
  --input-mach <machine>      Set input machine type to <machine>\n\
  --output-mach <machine>     Set output machine type to <machine>\n\
  -h --help                   Display this information\n\
  -v --version                Display the version number of %s\n\
"),
	   program_name);
  if (REPORT_BUGS_TO[0] && exit_status == 0)
    fprintf (stream, _("Report bugs to %s\n"), REPORT_BUGS_TO);
  exit (exit_status);
}

int
main (int argc, char ** argv)
{
  int c, status;

#if defined (HAVE_SETLOCALE) && defined (HAVE_LC_MESSAGES)
  setlocale (LC_MESSAGES, "");
#endif
#if defined (HAVE_SETLOCALE)
  setlocale (LC_CTYPE, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  expandargv (&argc, &argv);

  while ((c = getopt_long (argc, argv, "hv",
			   options, (int *) 0)) != EOF)
    {
      switch (c)
	{
	case OPTION_INPUT_MACH:
	  input_elf_machine = elf_machine (optarg);
	  if (input_elf_machine < 0)
	    return 1;
	  input_elf_class = elf_class (input_elf_machine);
	  if (input_elf_class < 0)
	    return 1;
	  break;

	case OPTION_OUTPUT_MACH:
	  output_elf_machine = elf_machine (optarg);
	  if (output_elf_machine < 0)
	    return 1;
	  break;

	case 'h':
	  usage (stdout, 0);

	case 'v':
	  print_version (program_name);
	  break;

	default:
	  usage (stderr, 1);
	}
    }

  if (optind == argc || output_elf_machine == -1)
    usage (stderr, 1);

  status = 0;
  while (optind < argc)
    status |= process_file (argv[optind++]);

  return status;
}
