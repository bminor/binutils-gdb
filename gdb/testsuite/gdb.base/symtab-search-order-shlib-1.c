static int static_global = 42;

int
shlib_1_func (void)
{
  return static_global;
}
