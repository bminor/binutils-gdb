/* Test that things still (sort of) work when compiled without -g.  */

int dataglobal = 3;			/* Should go in global data */
static int datalocal = 4;		/* Should go in local data */
int bssglobal;				/* Should go in global bss */
static int bsslocal;			/* Should go in local bss */

int
inner (x)
     int x;
{
  return 2 * x;
}

static int
middle (x)
     int x;
{
  return 2 * inner (x);
}

int
top (x)
     int x;
{
  return 2 * middle (x);
}

int 
main (argc, argv)
     int argc;
     char **argv;
{
  return top (argc);
}
