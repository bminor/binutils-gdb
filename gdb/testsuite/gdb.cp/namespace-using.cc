namespace A
{
  int _a = 1;
  int x = 2;
}

int marker4(){
	using A::x;
	return 0;
}

int marker3(){
	return marker4();
}

int marker2()
{
  namespace B = A;
  B::_a;
  return marker3();
}

int marker1()
{
  int total = 0;
  {
    int b = 1;
    {
      using namespace A;
      int c = 2;
      {
        int d = 3;
        total = _a + b + c + d + marker2(); // marker1 stop
      }
    }
  }
  return total;
}

int main()
{
  using namespace A;
  _a;
  return marker1();
}
