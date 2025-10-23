/* Copyright (C) 2019-2025 Free Software Foundation, Inc.

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

#ifndef GDBSUPPORT_SCOPE_EXIT_H
#define GDBSUPPORT_SCOPE_EXIT_H

#include <functional>
#include <type_traits>
#include "gdbsupport/preprocessor.h"

/* scope_exit is a general-purpose scope guard that calls its exit
   function at the end of the current scope.  A scope_exit may be
   canceled by calling the "release" method.  The API is modeled on
   P0052R5 - Generic Scope Guard and RAII Wrapper for the Standard
   Library, which is itself based on Andrej Alexandrescu's
   ScopeGuard/SCOPE_EXIT.

   There are two forms available:

   - The "make_scope_exit" form allows canceling the scope guard.  Use
     it like this:

     auto cleanup = make_scope_exit ( <function, function object, lambda> );
     ...
     cleanup.release (); // cancel

   - If you don't need to cancel the guard, you can use the SCOPE_EXIT
     macro, like this:

     SCOPE_EXIT
       {
	 // any code you like here.
       }

   See also forward_scope_exit.
*/

/* A template class that implements 'scope_exit' like things.  The POLICY
   type is used as base class, and its constructors are pulled into
   scope_exit_base.

   When scope_exit_base is destructed, POLICY::on_exit is called so long as
   release has not first been called.

   Private inheritance from POLICY as we don't want to leak any of its
   details to the public API.  */

template<typename POLICY>
class scope_exit_base : private POLICY
{
public:
  using POLICY::POLICY;

  ~scope_exit_base ()
  {
    if (!m_released)
      POLICY::on_exit ();
  }

  DISABLE_COPY_AND_ASSIGN (scope_exit_base);

  /* If this is called, then on_exit will not be invoked from the dtor.  */
  void release () noexcept
  {
    m_released = true;
  }

private:
  /* Only when false will on_exit be invoked from the dtor.  */
  bool m_released = false;
};

namespace detail
{

/* Policy class to use with scope_exit_base that will be used to implement
   the scope_exit type described in the comment at the head of this file.  */

template<typename EF>
class scope_exit_policy
{
public:
  template<typename EFP,
	   typename = gdb::Requires<std::is_constructible<EF, EFP>>>
  scope_exit_policy (EFP &&f)
    try : m_exit_function ((!std::is_lvalue_reference<EFP>::value
			    && std::is_nothrow_constructible<EF, EFP>::value)
			   ? std::move (f)
			   : f)
  {
  }
  catch (...)
    {
      /* "If the initialization of exit_function throws an exception,
	 calls f()."  */
      f ();
      /* "throws: Nothing, unless the initialization of exit_function
	 throws."  */
      throw;
    }

  DISABLE_COPY_AND_ASSIGN (scope_exit_policy);

  /* Called at scope exit unless 'release' (see scope_exit_base) was
     called.  */
  void on_exit ()
  {
    m_exit_function ();
  }

private:
  /* The function to call on scope exit.  */
  EF m_exit_function;
};

template<typename EF>
using scope_exit = scope_exit_base<::detail::scope_exit_policy<EF>>;

enum class scope_exit_lhs {};

template<typename EF>
scope_exit<typename std::decay<EF>::type>
operator+ (scope_exit_lhs, EF &&rhs)
{
  return scope_exit<typename std::decay<EF>::type> (std::forward<EF> (rhs));
}

}

template <typename EF>
::detail::scope_exit<typename std::decay<EF>::type>
make_scope_exit (EF &&f)
{
  return ::detail::scope_exit<typename std::decay<EF>::type>
    (std::forward<EF> (f));
}

/* Register a block of code to run on scope exit.  Note that the local
   context is captured by reference, which means you should be careful
   to avoid inadvertently changing a captured local's value before the
   scope exit runs.  */

#define SCOPE_EXIT \
  auto CONCAT(scope_exit_, __LINE__) = ::detail::scope_exit_lhs () + [&] ()

#endif /* GDBSUPPORT_SCOPE_EXIT_H */
