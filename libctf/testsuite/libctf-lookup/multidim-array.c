#include <ctf-api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  ctf_archive_t *ctf;
  ctf_dict_t *fp;
  int err;
  ctf_dump_state_t *dump_state = NULL;
  char *dumpstr;
  ctf_next_t *it = NULL;
  ctf_id_t type;
  int flagged = 0;

  if ((ctf = ctf_open (argv[1], NULL, &err)) == NULL)
    goto open_err;
  if ((fp = ctf_dict_open (ctf, NULL, &err)) == NULL)
    goto open_err;

  /* First, check for signs that the compiler is fixed but not emitting the
     relevant flag yet.  This combination is not expected to work right.  */

  while ((dumpstr = ctf_dump (fp, &dump_state, CTF_SECT_HEADER,
			      NULL, NULL)) != NULL)
    {
      if (strstr (dumpstr, "CTF_F_ARRNELEMS") != NULL)
	flagged = 1;
      free (dumpstr);
    }

  if (!flagged)
    {
      ctf_arinfo_t ar;

      if ((type = ctf_lookup_by_symbol_name (fp, "a")) == CTF_ERR)
	goto unexpected;

      if (ctf_array_info (fp, type, &ar) < 0)
	goto unexpected;

      if (ar.ctr_nelems == 3)
	{
	  fprintf (stderr, "UNSUPPORTED: compiler has GCC PR114186 fixed but "
		   "no indicative flag\n");
	  return 0;
	}
    }

  /* Now check for the actual bug.  */

  while ((type = ctf_type_next (fp, &it, NULL, 1)) != -1)
    printf ("%s\n", ctf_type_aname (fp, type));

  ctf_dict_close (fp);
  ctf_close (ctf);

  return 0;

 open_err:
  fprintf (stderr, "%s: cannot open: %s\n", argv[0], ctf_errmsg (err));
  return 1;

 unexpected:
  fprintf (stderr, "Cannot look up symbol to determine compiler bugginess: %s\n",
	   ctf_errmsg (ctf_errno (fp)));
  return 1;
}

