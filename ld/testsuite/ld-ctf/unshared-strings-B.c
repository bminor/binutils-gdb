struct A;
struct B
{
  int foo;
  struct A *a;
};
struct C;
typedef struct C C1;
typedef C1 C2;
struct A
{
  C2 *wurblefritz;
};

struct C
{
  struct B *foo;
};

static struct A *foo __attribute__((used));

