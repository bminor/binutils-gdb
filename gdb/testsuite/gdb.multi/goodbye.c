int gglob = 2;

int glob = 45;

int verylongfun()
{
  glob += 2;
  glob *= 2;
  glob += 3;
  glob *= 3;
  glob += 4;
  glob *= 4;
  glob += 5;
  glob *= 5;
  glob += 6;
  glob *= 6;
  glob += 7;
  glob *= 7;
  glob += 8;
  glob *= 8;
  glob += 9;
  glob *= 9;
}

main() {
  mailand();
  foo(glob);
  verylongfun();
  goodbye();
}

foo(int x) {
  return x + 92;
}

mailand()
{
  glob = 46;
}

void commonfun() { mailand(); }

goodbye() {
  ++glob;
}
