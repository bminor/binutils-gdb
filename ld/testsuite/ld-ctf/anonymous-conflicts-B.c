struct A
{
  union
  {
    long a;
  };
  int b;
};

static struct A a __attribute__((used));
