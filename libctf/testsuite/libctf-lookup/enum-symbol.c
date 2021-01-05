#include "config.h"
#include <bfd.h>
#include <elf.h>
#include <ctf-api.h>
#include <swap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssize_t symidx_64 (ctf_sect_t *s, ctf_sect_t *strsect, int little_endian, const char *name)
{
  const char *strs = (const char *) strsect->cts_data;
  size_t i;
  Elf64_Sym *sym = (Elf64_Sym *) s->cts_data;
  for (i = 0; i < s->cts_size / s->cts_entsize; i++, sym++)
    {
      Elf64_Word nameoff = sym->st_name;
#ifdef WORDS_BIGENDIAN
      if (little_endian)
	swap_thing (nameoff);
#else
      if (!little_endian)
	swap_thing (nameoff);
#endif
      if (strcmp (strs + nameoff, name) == 0)
	return i;
    }
  return -1;
}

ssize_t symidx_32 (ctf_sect_t *s, ctf_sect_t *strsect, int little_endian, const char *name)
{
  const char *strs = (const char *) strsect->cts_data;
  size_t i;
  Elf32_Sym *sym = (Elf32_Sym *) s->cts_data;
  for (i = 0; i < s->cts_size / s->cts_entsize; i++, sym++)
    {
      Elf32_Word nameoff = sym->st_name;
#ifdef WORDS_BIGENDIAN
      if (little_endian)
	swap_thing (nameoff);
#else
      if (!little_endian)
	swap_thing (nameoff);
#endif
      if (strcmp (strs + nameoff, name) == 0)
	return i;
    }
  return -1;
}

int
main (int argc, char *argv[])
{
  ctf_dict_t *fp;
  bfd *abfd;
  ctf_archive_t *ctf;
  ctf_sect_t symsect;
  ctf_sect_t strsect;
  ssize_t symidx;
  int err;
  ctf_id_t type;
  ctf_next_t *i = NULL;
  const char *name;
  int val;
  int little_endian;

  ssize_t (*get_sym) (ctf_sect_t *s, ctf_sect_t *strsect, int little_endian, const char *name);

  if (argc != 2)
    {
      fprintf (stderr, "Syntax: %s PROGRAM\n", argv[0]);
      exit(1);
    }

  /* Figure out the endianness of the symtab(s).  */
  if ((abfd = bfd_openr (argv[1], NULL)) == NULL
      || !bfd_check_format (abfd, bfd_object))
    goto bfd_open_err;
  little_endian = bfd_little_endian (abfd);
  bfd_close_all_done (abfd);

  if ((ctf = ctf_open (argv[1], NULL, &err)) == NULL)
    goto open_err;

  if ((fp = ctf_dict_open (ctf, NULL, &err)) == NULL)
    goto open_err;

  symsect = ctf_getsymsect (fp);
  strsect = ctf_getstrsect (fp);
  if (symsect.cts_data == NULL
      || strsect.cts_data == NULL)
    {
      fprintf (stderr, "%s: no symtab or strtab\n", argv[0]);
      return 1;
    }

  ctf_dict_close (fp);

  if (symsect.cts_entsize != sizeof (Elf64_Sym) &&
      symsect.cts_entsize != sizeof (Elf32_Sym))
    {
      fprintf (stderr, "%s: unknown symsize: %lx\n", argv[0],
	       symsect.cts_size);
      return 1;
    }

  switch (symsect.cts_entsize)
    {
    case sizeof (Elf64_Sym): get_sym = symidx_64; break;
    case sizeof (Elf32_Sym): get_sym = symidx_32; break;
    }

  if ((symidx = get_sym (&symsect, &strsect, little_endian, "primary1")) < 0)
    {
      fprintf (stderr, "%s: symbol not found: primary1\n", argv[0]);
      return 1;
    }

  /* Fish it out, then fish out all its enumerand/value pairs.  */

  if ((fp = ctf_arc_lookup_symbol (ctf, symidx, &type, &err)) == NULL)
    goto sym_err;

  while ((name = ctf_enum_next (fp, type, &i, &val)) != NULL)
    {
      printf ("%s has value %i\n", name, val);
    }
  if (ctf_errno (fp) != ECTF_NEXT_END)
    goto nerr;

  ctf_dict_close (fp);
  ctf_close (ctf);

  return 0;

 bfd_open_err:
  fprintf (stderr, "%s: cannot open: %s\n", argv[0], bfd_errmsg (bfd_get_error ()));
  return 1;

 open_err:
  fprintf (stderr, "%s: cannot open: %s\n", argv[0], ctf_errmsg (err));
  return 1;
 sym_err:
  fprintf (stderr, "%s: Symbol lookup error: %s\n", argv[0], ctf_errmsg (err));
  return 1;
 err:
  fprintf (stderr, "Lookup failed: %s\n", ctf_errmsg (ctf_errno (fp)));
  return 1;
 nerr:
  fprintf (stderr, "iteration failed: %s\n", ctf_errmsg (ctf_errno (fp)));
  return 1;
}
