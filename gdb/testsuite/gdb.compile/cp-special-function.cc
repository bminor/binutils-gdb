/* Copyright 2015 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <cstddef>

class MyInteger;
static MyInteger *global_integer;

class MyInteger
{
public:
  MyInteger (int val) : pub_var (0), int_ (val) {}
  int get (void) const { return int_; }

  /* Don't assume that these operators do exactly what you
     think they will -- especially the unary versions of +,-,*,&.  */

  friend MyInteger operator+ (const MyInteger& i);
  friend int operator+ (const MyInteger& i1, const MyInteger& i2);
  friend int operator+ (const MyInteger& i1, int i2);
  friend int operator+ (int i1, const MyInteger& i2);

  friend MyInteger operator- (const MyInteger& i);
  friend int operator- (const MyInteger& i1, const MyInteger& i2);
  friend int operator- (const MyInteger& i1, int i2);
  friend int operator- (int i1, const MyInteger& i2);

  friend MyInteger operator& (const MyInteger& i);
  friend int operator& (const MyInteger& i1, const MyInteger& i2);
  friend int operator& (const MyInteger& i1, int i2);
  friend int operator& (int i1, const MyInteger& i2);

  friend MyInteger operator* (const MyInteger& i);
  friend int operator* (const MyInteger& i1, const MyInteger& i2);
  friend int operator* (const MyInteger& i1, int i2);
  friend int operator* (int i1, const MyInteger& i2);

  friend MyInteger operator~ (const MyInteger& i);

  friend int operator/ (const MyInteger& i1, const MyInteger& i2);
  friend int operator/ (const MyInteger& i1, int i2);
  friend int operator/ (int i1, const MyInteger& i2);

  friend int operator% (const MyInteger& i1, const MyInteger& i2);
  friend int operator% (const MyInteger& i1, int i2);
  friend int operator% (int i1, const MyInteger& i2);

  friend int operator| (const MyInteger& i1, const MyInteger& i2);
  friend int operator| (const MyInteger& i1, int i2);
  friend int operator| (int i1, const MyInteger& i2);

  friend int operator^ (const MyInteger& i1, const MyInteger& i2);
  friend int operator^ (const MyInteger& i1, int i2);
  friend int operator^ (int i1, const MyInteger& i2);

  void operator= (const MyInteger& i) { int_ = i.int_; }
  void operator= (int i) { int_ = i; }

  void operator+= (const MyInteger& i) { int_ += i.int_; }
  void operator+= (int i) { int_ += i; }

  void operator-= (const MyInteger& i) { int_ -= i.int_; }
  void operator-= (int i) { int_ -= i; }

  void operator*= (const MyInteger& i) { int_ *= i.int_; }
  void operator*= (int i) { int_ *= i; }

  void operator/= (const MyInteger& i) { int_ /= i.int_; }
  void operator/= (int i) { int_ /= i; }

  void operator%= (const MyInteger& i) { int_ %= i.int_; }
  void operator%= (int i) { int_ %= i; }

  void operator&= (const MyInteger& i) { int_ &= i.int_; }
  void operator&= (int i) { int_ &= i; }

  void operator|= (const MyInteger& i) { int_ |= i.int_; }
  void operator|= (int i) { int_ |= i; }

  void operator^= (const MyInteger& i) { int_ ^= i.int_; }
  void operator^= (int i) { int_ ^= i; }

  friend int operator<< (const MyInteger& i1, const MyInteger& i2);
  friend int operator<< (const MyInteger& i1, int i2);
  friend int operator<< (int i1, const MyInteger& i2);

  friend int operator>> (const MyInteger& i1, const MyInteger& i2);
  friend int operator>> (const MyInteger& i1, int i2);
  friend int operator>> (int i1, const MyInteger& i2);

  void operator<<= (const MyInteger& i) { int_ <<= i.int_; }
  void operator<<= (int i) { int_ <<= i; }

  void operator>>= (const MyInteger& i) { int_ >>= i.int_; }
  void operator>>= (int i) { int_ >>= i; }

  friend bool operator== (const MyInteger& i1, const MyInteger& i2);
  friend bool operator== (const MyInteger& i1, int i2);
  friend bool operator== (int i1, const MyInteger& i2);

  friend bool operator!= (const MyInteger& i1, const MyInteger& i2);
  friend bool operator!= (const MyInteger& i1, int i2);
  friend bool operator!= (int i1, const MyInteger& i2);

  friend bool operator< (const MyInteger& i1, const MyInteger& i2);
  friend bool operator< (const MyInteger& i1, int i2);
  friend bool operator< (int i1, const MyInteger& i2);

  friend bool operator> (const MyInteger& i1, const MyInteger& i2);
  friend bool operator> (const MyInteger& i1, int i2);
  friend bool operator> (int i1, const MyInteger& i2);

  friend bool operator<= (const MyInteger& i1, const MyInteger& i2);
  friend bool operator<= (const MyInteger& i1, int i2);
  friend bool operator<= (int i1, const MyInteger& i2);

  friend bool operator>= (const MyInteger& i1, const MyInteger& i2);
  friend bool operator>= (const MyInteger& i1, int i2);
  friend bool operator>= (int i1, const MyInteger& i2);

  friend int operator! (const MyInteger& i);

  friend bool operator&& (const MyInteger& i1, const MyInteger& i2);
  friend bool operator&& (const MyInteger& i1, int i2);
  friend bool operator&& (int i1, const MyInteger& i2);

  friend bool operator|| (const MyInteger& i1, const MyInteger& i2);
  friend bool operator|| (const MyInteger& i1, int i2);
  friend bool operator|| (int i1, const MyInteger& i2);

  MyInteger& operator++ (void) { ++int_; return *this; }
  MyInteger operator++ (int dummy)
  {
    MyInteger tmp (int_);
    operator++ ();
    return tmp;
  }

  MyInteger& operator-- (void) { --int_; return *this; }
  MyInteger operator-- (int dummy)
  {
    MyInteger tmp (int_);
    operator-- ();
    return tmp;
  }

  friend MyInteger& operator, (MyInteger& i1, MyInteger& i2);

  friend int operator->* (const MyInteger& i1, int i2);
  friend int operator->* (const MyInteger& i1, const MyInteger& i2);

  MyInteger* operator-> (void) { return global_integer; }

  int operator() (void) { return -int_; }
  int operator() (const MyInteger& i) { return int_ + i; }
  int operator() (int i) { return int_ + i; }

  int operator[] (const MyInteger& i) { return int_ - i; }
  int operator[] (int i) { return int_ - i; }

#if 1
  static void* operator new (std::size_t sz);
#endif

  operator int() const { return -int_; }
  operator char() const { return int_ & 0xff; }

public:
  int pub_var;
private:
  int int_;
};

MyInteger
operator+ (const MyInteger& i)
{
  return MyInteger (i + 10);
}

int
operator+ (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ + i2.int_;
}

int
operator+ (const MyInteger& i1, int i2)
{
  return i1.int_ + i2;
}

int
operator+ (int i1, const MyInteger&  i2)
{
  return operator+ (i2, i1);
}

MyInteger
operator- (const MyInteger& i)
{
  return MyInteger (i + 20);
}

int
operator- (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ - i2.int_;
}

int
operator- (const MyInteger& i1, int i2)
{
  return i1.int_ - i2;
}

int
operator- (int i1, const MyInteger&  i2)
{
  return i1 - i2.int_;
}

MyInteger
operator& (const MyInteger& i)
{
  return MyInteger (i + 30);
}

int
operator& (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ & i2.int_;
}

int
operator& (const MyInteger& i1, int i2)
{
  return i1.int_ & i2;
}

int
operator& (int i1, const MyInteger&  i2)
{
  return operator& (i2, i1);
}

MyInteger
operator* (const MyInteger& i)
{
  return MyInteger (i + 40);
}

int
operator* (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ * i2.int_;
}

int
operator* (const MyInteger& i1, int i2)
{
  return i1.int_ * i2;
}

int
operator* (int i1, const MyInteger&  i2)
{
  return operator* (i2, i1);
}

MyInteger
operator~ (const MyInteger& i)
{
  return MyInteger (~i.int_);
}

int
operator/ (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ / i2.int_;
}

int
operator/ (const MyInteger& i1, int i2)
{
  return i1.int_ / i2;
}

int
operator/ (int i1, const MyInteger& i2)
{
  return i1 / i2.int_;
}

int
operator% (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ % i2.int_;
}

int
operator% (const MyInteger& i1, int i2)
{
  return i1.int_ % i2;
}

int
operator% (int i1, const MyInteger& i2)
{
  return i1 % i2.int_;
}

int
operator| (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ | i2.int_;
}

int
operator| (const MyInteger& i1, int i2)
{
  return i1.int_ | i2;
}

int
operator| (int i1, const MyInteger& i2)
{
  return i1 | i2.int_;
}

int
operator^ (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ ^ i2.int_;
}

int
operator^ (const MyInteger& i1, int i2)
{
  return i1.int_ ^ i2;
}

int
operator^ (int i1, const MyInteger& i2)
{
  return i1 ^ i2.int_;
}

int
operator<< (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ << i2.int_;
}

int
operator<< (const MyInteger& i1, int i2)
{
  return i1.int_ << i2;
}

int
operator<< (int i1, const MyInteger& i2)
{
  return i1 << i2.int_;
}

int
operator>> (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ >> i2.int_;
}

int
operator>> (const MyInteger& i1, int i2)
{
  return i1.int_ >> i2;
}

int
operator>> (int i1, const MyInteger& i2)
{
  return i1 >> i2.int_;
}

bool
operator== (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ == i2.int_;
}

bool
operator== (const MyInteger& i1, int i2)
{
  return i1.int_ == i2;
}

bool
operator== (int i1, const MyInteger& i2)
{
  return operator== (i2, i1);
}

bool
operator!= (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ != i2.int_;
}

bool
operator!= (const MyInteger& i1, int i2)
{
  return i1.int_ != i2;
}

bool
operator!= (int i1, const MyInteger& i2)
{
  return operator!= (i2, i1);
}

bool
operator< (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ < i2.int_;
}

bool
operator< (const MyInteger& i1, int i2)
{
  return i1.int_ < i2;
}

bool
operator< (int i1, const MyInteger& i2)
{
  return i1 < i2.int_;
}

bool
operator> (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ > i2.int_;
}

bool
operator> (const MyInteger& i1, int i2)
{
  return i1.int_ > i2;
}

bool
operator> (int i1, const MyInteger& i2)
{
  return i1 > i2.int_;
}

bool
operator<= (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ <= i2.int_;
}

bool
operator<= (const MyInteger& i1, int i2)
{
  return i1.int_ <= i2;
}

bool
operator<= (int i1, const MyInteger& i2)
{
  return i1 <= i2.int_;
}

bool
operator>= (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ >= i2.int_;
}

bool
operator>= (const MyInteger& i1, int i2)
{
  return i1.int_ >= i2;
}

bool
operator>= (int i1, const MyInteger& i2)
{
  return i1 >= i2.int_;
}

int
operator! (const MyInteger& i)
{
  return !(i.int_);
}

bool
operator&& (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ && i2.int_;
}

bool
operator&& (const MyInteger& i1, int i2)
{
  return i1.int_ && i2;
}

bool
operator&& (int i1, const MyInteger& i2)
{
  return operator&& (i2, i1);
}

bool
operator|| (const MyInteger& i1, const MyInteger& i2)
{
  return i1.int_ || i2.int_;
}

bool
operator|| (const MyInteger& i1, int i2)
{
  return i1.int_ || i2;
}

bool
operator|| (int i1, const MyInteger& i2)
{
  return operator|| (i2, i1);
}

MyInteger&
operator, (MyInteger& i1, MyInteger& i2)
{
  return i1;
}

int
operator->* (const MyInteger& i1, int i2)
{
  return i1 + i2;
}

int
operator->* (const MyInteger& i1, const MyInteger& i2)
{
  return i1 + i2;
}

int
operator"" _MI (unsigned long long i)
{
  return 100;
}

int
operator"" _MI (char c)
{
  return 200;
}

int
operator"" _MI (const char *str, std::size_t len)
{
  return 300;
}

#if 1
static void*
MyInteger::operator new (std::size_t sz)
{
  void* p =  ::operator new (sz);
  MyInteger* obj = static_cast<MyInteger*> (p);

  obj->pub_var = 1234;
  return p;
}
#endif

int
main (void)
{
  char ch;
  int var;
  MyInteger a (1), b (2), c (-3), d (0);

  global_integer = new MyInteger (21);
  global_integer->pub_var = -21;
  ch = *global_integer;
  d = a;
  d = 0;
  d += a;
  d += 1;
  d -= a;
  d -= 1;
  d *= 1;
  d *= d;
  d = 2;
  d /= 1;
  d /= d;
  d %= 2;
  d %= a;
  d = 2;
  d &= 1;
  d &= a;
  d |= 2;
  d |= a;
  d ^= 1;
  d ^= b;
  d <<= 2;
  d <<= a;
  d >>= 2;
  d >>= a;
  d++;
  ++d;
  d--;
  --d;
  d = 1234;
  var = d;
  return +c + a + b + 3 + a.get ()
    - a - 2 - a - b + (-c)
    + a & b + a & 1
    * a * b * 2 * c
    + ~c
    + a / 1 + b / a + 1 / a
    + a % b + a % 1 + 2 % b
    + 1|b + a|2 + a|b
    + 1^a + b^2 + a^b
    + 1 << a + a << 1 + a << a
    + 10 >> a + b >> 1 + b >> a
    + a == 1 + a == b + 1 == a
    + a != 1 + a != b + 1 != a
    + (a < 1) + (a < b) + (2 < a)
    + (a > 1) + (a > b) + (2 > a)
    + (a <= 1) + (a <= b) + (2 <= a)
    + (a >= 1) + (a >= b) + (2 >= a)
    + (!a)
    + (a && 0) + (a && b) + (1 && a)
    + (a || 0) + (a || b) + (1 || a)
    + (a,b)
    + a->*1 + a->*b
    + a->pub_var
    + a () + a (a) + a (2)
    + a [1] + b [a]
    + 'a'_MI + 1234_MI + "hello"_MI
    ; // break here
}
