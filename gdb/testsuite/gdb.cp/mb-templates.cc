
#include <iostream>
using namespace std;

template<class T>
void foo(T i)
{
  std::cout << "hi\n"; // set breakpoint here
}

template<typename T>
T bar (void)
{
  return -100;
}

template<typename T>
T bar(T i)
{
  return i;
}

template<typename T>
T bar(T i, T j)
{
  return i - j;
}

namespace N
{
  template<typename T, int V>
  T bar (void)
  {
    return static_cast<T> (V);
  }

  template<typename T, int V>
  T bar (T i)
  {
    return i - static_cast<T> (V);
  }

  template<typename T, int V>
  T bar (T i, T j)
  {
    return 100;
  }
};

template<class T>
void multi_line_foo(T i)
{
  std::cout // set multi-line breakpoint here
    << "hi\n";
}

int main()
{
    foo<int>(0);
    foo<double>(0);
    foo<int>(1);
    foo<double>(1);
    foo<int>(2);
    foo<double>(2);

    multi_line_foo<int>(0);
    multi_line_foo<double>(0);

    using namespace N;
    return bar<int> () + bar<int> (1) + bar<int> (2, 3)
      + bar<char> (1) + bar<char> (2, 3)
      + bar<int, 10> () + bar<int, 10> (0) + bar<int, 10> (1, 2)
      + bar<int, 0> ();
}
