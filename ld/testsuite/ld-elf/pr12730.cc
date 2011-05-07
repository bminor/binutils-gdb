#include <iostream>

class Hello
{
public:
   Hello ()
    {}

  ~Hello ()
    {}

  void act ()
    { std::cout << "Hello, world!" << std::endl; }
};


template <class T>
struct Foo
{
  T* _M_allocate_single_object ()
    {
      return new T;
    }
};

static void __attribute__ (( constructor )) PWLIB_StaticLoader() {
  Foo<Hello> allocator;
  Hello* salut = allocator._M_allocate_single_object ();
  salut->act ();
}


int
main (int /*argc*/,
      char* /*argv*/[])
{
  return 0;
}
