int
library_func1 (void)
{
  return 2;
}

int global = 1;

#ifdef WITH_IFUNC

static int minus_one (void) { return -1; }
static int zero (void) { return 0; }

void * library_func2_ifunc (void) __asm__ ("library_func2");
void * library_func2_ifunc (void) { return global ? minus_one : zero ; }
__asm__(".type library_func2, %gnu_indirect_function");

#else /* WITHOUT_IFUNC */

int
library_func2 (void)
{
  return 3;
}

#endif
