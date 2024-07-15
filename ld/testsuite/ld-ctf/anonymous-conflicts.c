struct A
{
  union
  {
    int a;
  };
  int b;
};

static struct A a __attribute__((used));
