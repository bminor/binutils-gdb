struct small
{
  int i;
};

struct med
{
  struct small s1, s2;
};

struct large
{
  struct med m1, m2;
};

struct xlarge
{
  struct large l1, l2;
};

struct small ret_small ();
struct med ret_med ();
struct large ret_large ();
struct xlarge ret_xlarge ();

void print_small ();
void print_med ();
void print_large ();
void print_xlarge ();

main ()
{
  struct small s;
  struct med m;
  struct large l;
  struct xlarge x;

  s = ret_small (1);
  m = ret_med (s,s);
  l = ret_large (m, m);
  x = ret_xlarge (l, l);

  print_small (s);
  print_med (m);
  print_large (l);
  print_xlarge (x);
}

struct small ret_small (i)
     int i;
{
  struct small s;
  s.i = i;
  return s;
}

struct med ret_med (s1, s2)
     struct small s1, s2;
{
  struct med m;
  m.s1 = s1;
  m.s2 = s2;
  return m;
}

struct large ret_large (m1, m2)
     struct med m1, m2;
{
  struct large l;
  l.m1 = m1;
  l.m2 = m2;
  return l;
}

struct xlarge ret_xlarge (l1, l2)
     struct large l1, l2;
{
  struct xlarge x;
  x.l1 = l1;
  x.l2 = l2;
  return x;
}

void print_small (s)
     struct small s;
{
  printf ("small: s.i = %d\n", s.i);
}

void print_med (m)
     struct med m;
{
  printf ("med: m.s1.i = %d; m.s2.i = %d\n", m.s1.i, m.s2.i);
}

void print_large (l)
     struct large l;
{
  printf ("large: l.m1.s1.i = %d; l.m1.s2.i = %d; l.m2.s1.i = %d; l.m2.s2.i = %d\n",
	  l.m1.s1.i, l.m1.s2.i, l.m2.s1.i, l.m2.s2.i);
}

void print_xlarge (x)
     struct xlarge x;
{
  printf ("xlarge: x.l1: ");
  print_large (x.l1);
  printf ("xlarge: x.l2: ");
  print_large (x.l2);
}

