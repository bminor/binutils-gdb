// This test currently fails because the C++ front end emits `A' as
// the base class called rather than `B' as it ought.  At least it
// is erroring on the safe side...

struct A
{
  virtual void foo();
  virtual void bar();
};

void A::foo() { }			// loose
void A::bar() { }			// loose

struct B : public A
{
  virtual void foo();
};

void B::foo() { }			// keep

void _start() __asm__("_start");	// keep

A a;
B b;					// keep
B *getme() { return &b; }		// keep

void _start()
{
  getme()->foo();
}

extern "C" void __main() { }
