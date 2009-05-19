extern void lib1_default (void);
extern void lib2_default (void);

int
main (int argc __attribute__ ((unused)),
      char** argv __attribute__ ((unused)))
{
  lib1_default ();
  lib2_default ();
  return 0;
}

