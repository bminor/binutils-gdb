extern int __start___verbose[];
extern int __stop___verbose[];
int
foo2 (void)
{
  static int my_var[10] __attribute__((used, section("__verbose")))
    = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
  if (__start___verbose == __stop___verbose
      || __start___verbose[0] != 10)
    return -1;
  else
    return 0;
}
