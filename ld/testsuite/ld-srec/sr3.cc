// This file is compiled and linked into the S-record format.

#define FOO_MSG_LEN 80

class Foo {
    static int foos;
    int i;
    const len = FOO_MSG_LEN;
    char message[len];
public:
    static void init_foo ();
    static int nb_foos() { return foos; }
    Foo();
    Foo( char* message);
    Foo(const Foo&);
    Foo & operator= (const Foo&);
    ~Foo ();
};

static Foo static_foo( "static_foo");

int
main ()
{
  Foo automatic_foo( "automatic_foo");
  return 0;
}

void
terminate(void)
{
  /* This recursive call prevents a compiler warning that the noreturn
     function terminate actually does return.  */
  terminate ();
}

extern "C" {
int
__main ()
{
}

int
__builtin_delete ()
{
}

int
__builtin_new ()
{
}

void
__throw ()
{
}

void
__terminate ()
{
}

void *__eh_pc;
}

int Foo::foos = 0;

void Foo::init_foo ()
{
  foos = 80;
}

Foo::Foo ()
{
  i = ++foos;
}

Foo::Foo (char* msg)
{
  i = ++foos;
}

Foo::Foo (const Foo& foo)
{
  i = ++foos;
  for (int k = 0; k < FOO_MSG_LEN; k++)
    message[k] = foo.message[k];
}

Foo& Foo::operator= (const Foo& foo)
{
  for (int k = 0; k < FOO_MSG_LEN; k++)
    message[k] = foo.message[k];
  return *this;
}

Foo::~Foo ()
{
  foos--;
}
