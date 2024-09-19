/* This testcase is part of GDB, the GNU debugger.

   Copyright 2024 Free Software Foundation, Inc.

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

/* PR 25987 */
struct MyClass;
struct ptr {
    MyClass* get() { return t; }     /* line 21 */
    MyClass* t;
};
struct MyClass { void call(); };
void MyClass::call() {
    *(volatile char*)-1 = 1;         /* line 26 */
}
static void intermediate(ptr p) {
    p.get()->call();                 /* line 29 */
}
int main() {
    intermediate(ptr{new MyClass});
}
