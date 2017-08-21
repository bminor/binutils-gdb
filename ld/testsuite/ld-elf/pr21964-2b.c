#include <dlfcn.h>
#include <stdio.h>

int main()
{
  void *dl;
  void *sym;
  int (*func) (void);

  dl = dlopen("pr21964-2.so", RTLD_LAZY);
  if (!dl)
    return 1;

  sym = dlsym(dl, "__start___verbose");
  if (!sym)
    return 2;

  func = dlsym(dl, "foo");
  if (!func)
    return 3;
  if (func () == 0)
    printf ("PASS\n");

  dlclose(dl);

  return 0;
}
