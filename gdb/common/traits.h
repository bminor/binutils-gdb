/* Copyright (C) 2017 Free Software Foundation, Inc.

   This file is part of GDB.

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

#ifndef COMMON_TRAITS_H
#define COMMON_TRAITS_H

#include <type_traits>

namespace gdb {

/* Pre C++14-safe (CWG 1558) version of C++17's std::void_t.  See
   <http://en.cppreference.com/w/cpp/types/void_t>.  */

template<typename... Ts>
struct make_void { typedef void type; };

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

/* A few trait helpers, mainly stolen from libstdc++.  Uppercase
   because "and" is a keyword.  */

template<typename...>
struct And;

template<>
struct And<> : public std::true_type
{};

template<typename B1>
struct And<B1> : public B1
{};

template<typename B1, typename B2>
struct And<B1, B2>
  : public std::conditional<B1::value, B2, B1>::type
{};

template<typename B1, typename B2, typename B3, typename... Bn>
struct And<B1, B2, B3, Bn...>
  : public std::conditional<B1::value, And<B2, B3, Bn...>, B1>::type
{};

}

#endif /* COMMON_TRAITS_H */
