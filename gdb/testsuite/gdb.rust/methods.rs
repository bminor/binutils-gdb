// Copyright (C) 2016-2025 Free Software Foundation, Inc.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_assignments)]


pub trait Whatever {
    fn whatever(&self) -> i32;
    fn static_i32(x: i32) -> Self;
}

impl Whatever for i32 {
    fn whatever(&self) -> i32 {
        *self                   // set breakpoint 2 here
    }

    fn static_i32(x: i32) -> i32 {
        x
    }
}

// On i686-linux, for hasMethods::take the rust compiler generates code
// similar to what a c compiler generates for:
//   int foo (int val) { return val; }
// but gdb calls it as if it were:
//   void foo (int *res, int *val) { *res = *val; }
// By default, the rust compiler is free to optimize functions and data
// layout, so use repr(C) and extern "C" to force the compiler to present a
// C-like interface.

#[repr(C)]
pub struct HasMethods {
    value: i32
}

impl HasMethods {
    pub fn new() -> HasMethods {
        HasMethods { value: 0 }
    }

    pub fn incr(&mut self) -> &mut HasMethods {
        self.value += 1;
        self
    }

    pub extern "C" fn take(self) -> HasMethods {
        self
    }
}

impl Whatever for HasMethods {
    fn whatever(&self) -> i32 {
        self.value
    }

    fn static_i32(x: i32) -> HasMethods {
        HasMethods{value: x}
    }
}

enum SomeEnum {
    One,
    Two,
    Three(i32),
    Four{x: i32}
}

impl SomeEnum {
    fn value(&self) -> i32 {
        match *self {
            SomeEnum::Three(x) => x,
            SomeEnum::Four{x} => x,
            _ => 0
        }
    }

    fn mut_value(&mut self) -> i32 {
        match *self {
            SomeEnum::Three(x) => x,
            SomeEnum::Four{x} => x,
            _ => 0
        }
    }

    fn take_value(self) -> (i32, SomeEnum) {
        (match self {
            SomeEnum::Three(x) => x,
            SomeEnum::Four{x} => x,
            _ => 0
        }, self)
    }
}

enum SimpleEnum {
    One,
    Two,
    Three
}

impl SimpleEnum {
    fn value(&self) -> i32 {
        match *self {
            SimpleEnum::One => 1,
            SimpleEnum::Two => 2,
            SimpleEnum::Three => 452,
        }
    }
}

fn main() {
    let mut a = SomeEnum::Three(23);
    let av = a.value();
    let amv = (&mut a).mut_value();
    let atv = a.take_value();
    let b = SomeEnum::Four{x: 24};
    let bv = b.value();
    let c = SimpleEnum::Three;
    let d = c.value();
    let mut x = HasMethods::new();
    x.incr();               // set breakpoint 1 here
    (&mut x).incr();
    let y = 23i32.whatever();
    println!("{}", y);
    let z = x.take();
}
