namespace AAA {
  char c;
  int i;
  int A_foo (int);
  char foo (char);
  class inA {
  public:
    int xx;
    int fum (int);
  };
};

int AAA::inA::fum (int i)
{
  return 10 + i;
}

namespace BBB {
  char c;
  int i;
  int B_foo (int);
  char foo (char);

  namespace CCC {
    char foo (char);
  };

  class Class {
  public:
    char foo (char);
    int dummy;
  };
};

int AAA::A_foo (int x)
{
  return 2 * x;
}

char AAA::foo (char c)
{
  return 'a';
}


int BBB::B_foo (int x)
{
  return 3 * x;
}

char BBB::foo (char c)
{
  return 'b';
}

char BBB::CCC::foo (char c)
{
  return 'z';
}

char BBB::Class::foo (char c)
{
  return 'o';
}

void marker1(void)
{
  return;
}


int main ()
{
  using AAA::inA;
  char c1;

  using namespace BBB;
  
  c1 = foo ('x');
  c1 = AAA::foo ('x');
  c1 = BBB::CCC::foo ('m');
  
  inA ina;

  ina.xx = 33;

  int y;

  y = AAA::A_foo (33);
  y += B_foo (44);

  BBB::Class cl;

  c1 = cl.foo('e');

  marker1();
  
}

  



