// This first batch of classes are for PR 11226.
namespace mc {
  class Base {
  protected:
    int x;
    
  public:
    Base(void) { x = 2; };
  };
}

namespace ph {
  class Middle: public virtual mc::Base {
  protected:
    int y;
    
  public:
    Middle(void): mc::Base() { y = 3; };

    int get_y(void)
    {
      return y;			// breakpoint 1
    };
  };

  class Derived: public virtual Middle {
  protected:
    int z;
    
  public:
    Derived(void): Middle() { z = 4; };

    int get_z(void)
    {
      return z;			// breakpoint 2
    };
  };
}

// These classes are for PR 9629.
struct A {};
struct B : virtual A {};

struct C {int v; C() {v=11;};};
struct D:virtual C{};

class E:B,D{};

// These classes are for another regression test, from
// https://bugzilla.redhat.com/show_bug.cgi?id=560741

class RHA
{
public:
  RHA() : mA(0xaaaaaaaa) {}
  virtual void a() = 0;  
  int mA;
};

class RHB
{
public:
  RHB() : mB(0xbbbbbbbb) {}
  virtual void b() = 0;
  int mB;
};

class RHC : public RHA,
	  public RHB
{
public:
  RHC() : RHA(), RHB() {}
  virtual void a() {}
  virtual void b() {}
};




int main() {
  ph::Derived tst;
  tst.get_y();
  tst.get_z();

  E *e = new E;
  RHB *b = new RHC();

  return 0;			// breakpoint 3
}
