
#include <iostream>
using namespace std;

template<class T>
void foo(T i)
{
  std::cout << "hi\n"; // set breakpoint here
}

int main()
{
    foo<int>(0);
    foo<double>(0);
    foo<int>(1);
    foo<double>(1);
    foo<int>(2);
    foo<double>(2);
}
