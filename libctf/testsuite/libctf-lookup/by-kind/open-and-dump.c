#include <ctf-api.h>
#include <stdio.h>
#include <stdlib.h>

static void
dump_ctf_errs (ctf_dict_t *fp)
{
  ctf_next_t *it = NULL;
  char *errtext;
  int is_warning;
  int err;

  /* Dump accumulated errors and warnings.  */
  while ((errtext = ctf_errwarning_next (fp, &it, &is_warning, &err)) != NULL)
    {
      fprintf (stderr, "%s: %s\n", is_warning ? "warning": "error",
	       errtext);
      free (errtext);
    }
  if (err != ECTF_NEXT_END)
    {
      fprintf (stderr, "CTF error: cannot get CTF errors: `%s'",
	       ctf_errmsg (err));
    }
}

int main (int argc, char *argv[])
{
  ctf_archive_t *arc;
  ctf_dict_t *fp;
  ctf_next_t *i = NULL;
  const char *name;
  int err;

  if (argc != 2)
    {
      fprintf (stderr, "Syntax: open-and-dump FILE\n");
      exit (1);
    }

  if ((arc = ctf_open (argv[1], NULL, &err)) == NULL)
    goto open_err;

  while ((fp = ctf_archive_next (arc, &i, &name, 0, &err)) != NULL)
    {
      ctf_dump_state_t *s = NULL;
      char *item;
      int j;
      ctf_sect_names_t things[] = { CTF_SECT_HEADER, CTF_SECT_VAR, CTF_SECT_TYPE, 666 };
      const char *thing_names[] = { "Header", "Variables", "Types", NULL };

      fprintf (stderr, "\nArchive member: %s\n", name);

      for (j = 0; things[j] != 666; j++)
	{
	  printf ("\n%s: \n\n", thing_names[j]);
	  while ((item = ctf_dump (fp, &s, things[j], NULL, NULL)) != NULL)
	    {
	      printf ("%s", item);
	      free (item);
	    }

	  if (ctf_errno (fp))
	    {
	      fprintf (stderr, "Dumping failed: %s, %s\n", j,
		       ctf_errmsg (ctf_errno (fp)));
	      break;
	    }
	}
      dump_ctf_errs (fp);
      ctf_dict_close (fp);
    }
  if (err != ECTF_NEXT_END)
    {
      ctf_next_destroy (i);
      fprintf (stderr, "Iteration failed: %s\n", ctf_errmsg (ctf_errno (fp)));
    }

  ctf_close (arc);

  return 0;

 open_err:
  fprintf (stderr, "%s: cannot open: %s\n", argv[0], ctf_errmsg (err));
  return 1;
}
