
#include <stdio.h>

class Base 
{
public:
  Base(int k);
  ~Base();
  virtual void foo() {}
private:
  int k;
};

Base::Base(int k)
{
  this->k = k;
}

Base::~Base()
{
    printf("~Base\n");
}

class Derived : public virtual Base
{
public:
  Derived(int i);
  ~Derived();
private:
  int i;
};

Derived::Derived(int i) : Base(i)
{
  this->i = i;
}

Derived::~Derived()
{
    printf("~Derived\n");
}

class DeeplyDerived : public Derived
{
public:
  DeeplyDerived(int i) : Base(i), Derived(i) {}
};

int main()
{
  /* Invokes the Derived ctor that constructs both
     Derived and Base.  */
  Derived d(7);
  /* Invokes the Derived ctor that constructs only
     Derived. Base is constructed separately by
     DeeplyDerived's ctor.  */
  DeeplyDerived dd(15);
}
