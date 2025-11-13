/* Wrappers for C++ threading

   Copyright (C) 2025 Free Software Foundation, Inc.

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

#ifndef GDBSUPPORT_CXX_THREAD_H
#define GDBSUPPORT_CXX_THREAD_H

/* This header implements shims for the parts of the C++ threading
   library that are needed by gdb.

   The reason this exists is that some versions of libstdc++ do not
   supply a working C++ thread implementation.  In particular this was
   true for several versions of the Windows compiler.  See
   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93687.

   For systems where this works, this header just supplies aliases of
   the standard functionality, in the "gdb" namespace.  For example,
   "gdb::mutex" is an alias for "std::mutex".

   For non-working ports, shims are provided.  These are just the
   subset needed by gdb, and they generally do nothing, or as little
   as possible.  In particular they all simply assume single-threaded
   operation.  */

#if CXX_STD_THREAD

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

namespace gdb
{

using condition_variable = std::condition_variable;
using cv_status = std::cv_status;
using future_status = std::future_status;
using mutex = std::mutex;
using recursive_mutex = std::recursive_mutex;
using thread = std::thread;

namespace this_thread = std::this_thread;

template<typename T>
using lock_guard = std::lock_guard<T>;

template<typename T>
using unique_lock = std::unique_lock<T>;

template<typename T>
using future = std::future<T>;

} /* namespace gdb*/

#else

#include <chrono>

namespace gdb
{

/* A do-nothing replacement for std::mutex.  */
struct mutex
{
  mutex () = default;

  DISABLE_COPY_AND_ASSIGN (mutex);

  void lock ()
  {
  }

  void unlock ()
  {
  }
};

/* A do-nothing replacement for std::recursive_mutex.  */
struct recursive_mutex
{
  recursive_mutex () = default;

  DISABLE_COPY_AND_ASSIGN (recursive_mutex);

  void lock ()
  {
  }

  void unlock ()
  {
  }
};

/* A do-nothing replacement for std::lock_guard.  */
template<typename T>
struct lock_guard
{
  explicit lock_guard (T &m)
  {
  }

  DISABLE_COPY_AND_ASSIGN (lock_guard);
};

/* A do-nothing replacement for std::unique_lock.  */
template<typename T>
struct unique_lock
{
  explicit unique_lock (T &m)
  {
  }

  DISABLE_COPY_AND_ASSIGN (unique_lock);
};

/* A compatibility enum for std::cv_status.  */
enum class cv_status
{
  no_timeout,
  timeout,
};

/* A do-nothing replacement for std::condition_variable.  */
struct condition_variable
{
  condition_variable () = default;

  DISABLE_COPY_AND_ASSIGN (condition_variable);

  void notify_one () noexcept
  {
  }

  void wait (unique_lock<mutex> &lock)
  {
  }

  template<class Rep, class Period>
  cv_status wait_for (unique_lock<mutex> &lock,
		      const std::chrono::duration<Rep, Period> &rel_time)
  {
    return cv_status::no_timeout;
  }
};

/* A compatibility enum for std::future_status.  This is just the
   subset needed by gdb.  */
enum class future_status
{
  ready,
  timeout,
};

/* A compatibility implementation of std::future.  */
template<typename T>
class future
{
public:

  explicit future (T value)
    : m_value (std::move (value))
  {
  }

  future () = default;

  DISABLE_COPY_AND_ASSIGN (future);

  future (future &&other) = default;
  future &operator= (future &&other) = default;

  void wait () const { }

  template<class Rep, class Period>
  future_status wait_for (const std::chrono::duration<Rep,Period> &duration)
    const
  {
    return future_status::ready;
  }

  T get () { return std::move (m_value); }

private:

  T m_value;
};

/* A specialization for void.  */

template<>
class future<void>
{
public:
  void wait () const { }

  template<class Rep, class Period>
  future_status wait_for (const std::chrono::duration<Rep,Period> &duration)
    const
  {
    return future_status::ready;
  }

  void get () { }
};

/* Rather than try to write a gdb::thread class, we just use a
   namespace since only the 'id' type is needed.  Code manipulating
   actual std::thread objects has to be wrapped in a check anyway.  */
namespace thread
{
/* Replacement for std::thread::id.  */
using id = int;
}

/* Replacement for std::this_thread.  */
namespace this_thread
{
static inline thread::id
get_id ()
{
  return 0;
}
}

} /* namespace gdb */

#endif /* CXX_STD_THREAD */

#endif /* GDBSUPPORT_CXX_THREAD_H */
