struct foo
{
  int a : 5, : 4, b : 5;
  char c;
  int : 3, d : 8, : 0, e : 5;
};

struct foo x;

main ()
{
  printf (x);
}
