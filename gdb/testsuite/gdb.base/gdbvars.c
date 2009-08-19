/* Simple program to help exercise gdb's convenience variables.  */

typedef void *ptr;

ptr p = &p;

int
main ()
{
#ifdef usestubs
    set_debug_traps ();
    breakpoint ();
#endif

  return 0;
}
