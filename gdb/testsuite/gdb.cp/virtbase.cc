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

int main() {
  ph::Derived tst;
  tst.get_y();
  tst.get_z();

  E *e = new E;

  return 0;			// breakpoint 3
}
