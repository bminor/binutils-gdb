/*
 * nmtest.c -- this is the C code portion of tests for the GNU binutils
 */

main(argc, argv)
int argc;
char *argv[];
{
  char two();
  int one();

  one();
  two();
}

int
one ()
{
  int i, j;
  j = i++;
}

char
two ()
{
  int i, j;
  i = j++;

}

