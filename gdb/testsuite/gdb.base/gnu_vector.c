/* This testcase is part of GDB, the GNU debugger.

   Copyright 2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Contributed by Ken Werner <ken.werner@de.ibm.com>  */

int __attribute__ ((vector_size (4 * sizeof(int)))) i4a = {2, 4, 8, 16};
int __attribute__ ((vector_size (4 * sizeof(int)))) i4b = {1, 2, 8, 4};
float __attribute__ ((vector_size (4 * sizeof(float)))) f4a = {2, 4, 8, 16};
float __attribute__ ((vector_size (4 * sizeof(float)))) f4b = {1, 2, 8, 4};
unsigned int __attribute__ ((vector_size (4 * sizeof(unsigned int)))) ui4 = {2, 4, 8, 16};
int __attribute__ ((vector_size (2 * sizeof(int)))) i2 = {1, 2};
long long __attribute__ ((vector_size (2 * sizeof(long long)))) ll2 = {1, 2};
float __attribute__ ((vector_size (2 * sizeof(float)))) f2 = {1, 2};
double __attribute__ ((vector_size (2 * sizeof(double)))) d2 = {1, 2};

int
main ()
{
  return 0;
}
