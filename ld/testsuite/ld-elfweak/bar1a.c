#pragma weak deallocate_foo

int deallocate_foo = 1;

int *
bar()
{
  return &deallocate_foo;
}
