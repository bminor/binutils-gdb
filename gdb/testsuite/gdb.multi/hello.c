short hglob = 1;

short glob = 92;

int commonfun() { bar(); }

bar()
{
  if (glob == 0)
    exit(1);
}

hello(int x)
{
  x *= 2;
  return x + 45;
}

main()
{
  int tmpx;

  bar();
  tmpx = hello(glob);
  commonfun();
  glob = tmpx;
  commonfun();
}

