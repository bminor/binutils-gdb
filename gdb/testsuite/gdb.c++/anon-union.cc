
struct Foo {
  union {
    int zero;
    unsigned int one;
  } num1;

  union {
    int pebble;
    struct {
      int rock;
      unsigned int rock2;
    } x;
    union {
      int qux;
      unsigned int mux;
    };
    unsigned int boulder;
  };
  union {
    int paper;
    unsigned int cloth;
  };
  union {
    int two;
    unsigned int three;
  } num2;
};

union Bar {
  int x;
  unsigned int y;
};

Foo foo;
Bar bar;

int main()
{
  foo.paper = 33;
  foo.pebble = 44;
  foo.mux = 55;

  bar.x = 33;

  union
  {
    int z;
    unsigned int w;
  };

  w = 45;

}
