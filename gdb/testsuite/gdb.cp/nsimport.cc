namespace A {
  int x = 11;
  namespace{
    int xx = 22;
  }
}

using namespace A;

namespace{
  int xxx = 33;
};

int main()
{
  int unused1 = x;
  int unused2 = xx;
  int unused3 = xxx;
  return 0;
}
