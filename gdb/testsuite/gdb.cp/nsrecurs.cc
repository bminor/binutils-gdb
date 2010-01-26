namespace A{
  int ax = 9;
}

namespace B{
  using namespace A;
}

namespace C{
  using namespace B;
}

//---------------
namespace D{
  using namespace D;
  int dx = 99;
}
using namespace C;

//---------------
namespace{
  namespace{
    int xx = 999;
  }
}

int main(){
  using namespace D;
  return ax + dx + xx;
}
