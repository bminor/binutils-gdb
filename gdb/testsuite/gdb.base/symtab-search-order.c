extern int shlib_1_func (void);

int
main ()
{
  /* We need a reference to shlib_1_func to make sure its shlib is
     not discarded from the link.  This happens on windows.  */
  int x = shlib_1_func ();

  return 0;
}
