namespace N1
{
  namespace N2
  {
    namespace N3
    {
      namespace N4
      {
        static int n4static = 400;

        struct S4
        {
          static int s4static;
          int s4int_;
          S4 (void) : s4int_ (4) {};
          ~S4 (void) { --s4static; }

         int get_var (void) { return s4int_; }
         static int get_svar (void) { return s4static; }
        };
        int S4::s4static = 40;
      }
    }
  }
}

int
main (void)
{
  using namespace N1::N2::N3::N4;

  S4 s;
  int var = 1234;

  var += s.s4int_; /* break here */
  return S4::get_svar () - 10 * s.get_var ();
}

