struct foo
{
  unsigned bar : 1;
  unsigned lose : 1;
};

main ()
{
  struct foo *win;

  printf ("%d, %d\n", win->bar, win->lose);
}
