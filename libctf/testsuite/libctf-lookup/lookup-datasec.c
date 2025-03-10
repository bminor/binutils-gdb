#include "config.h"
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
  ctf_id_t id = 0;
  ctf_next_t *it = NULL;
  const char *name = NULL;

  if (argc != 2)
    {
      fprintf (stderr, "Syntax: %s PROGRAM\n", argv[0]);
      exit(1);
    }

  if ((ctf = ctf_open (argv[1], NULL, &err)) == NULL)
    goto open_err;
  if ((fp = ctf_dict_open (ctf, NULL, &err)) == NULL)
    goto open_err;


  while ((id = ctf_type_next(fp, &it, NULL, 1)) != CTF_ERR)
    {
      printf(ctf_type_aname(fp, id));
      printf("\n");
    }

  ctf_dict_close(fp);
  ctf_close (ctf);
  return 0;
  
  open_err:
    fprintf (stderr, "%s: cannot open: %s\n", argv[0], ctf_errmsg (err));
    return 1;
  lookup_err:
    fprintf (stderr, "Lookup failed: %s\n", ctf_errmsg (ctf_errno (fp)));
    return 1;
}
