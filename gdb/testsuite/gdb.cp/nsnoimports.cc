
namespace A
{
  int _a = 11;

  namespace B{

    int ab = 22;

    namespace C{

      int abc = 33;

      int second(){
        return 0;
      }

    }

    int first(){
      int unused1 = _a;
      int unused2 = ab;
      int unused3 = C::abc;
      return C::second();
    }
  }
}


int
main()
{
  int unused1 = A::_a;
  int unused2 = A::B::ab;
  int unused3 = A::B::C::abc;
  return A::B::first();
}
