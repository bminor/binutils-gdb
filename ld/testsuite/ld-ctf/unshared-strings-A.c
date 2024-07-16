struct A;
struct B
{
  int foo;
  struct A *a;
};
struct C;
typedef struct C aaa;
struct A
{
  aaa *wurblefrotz;
};
struct C
{
  struct B *foo;
};

static struct A *foo __attribute__((used));

