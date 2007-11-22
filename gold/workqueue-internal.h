// workqueue-internal.h -- internal work queue header for gold   -*- C++ -*-

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef GOLD_WORKQUEUE_INTERNAL_H
#define GOLD_WORKQUEUE_INTERNAL_H

#include <queue>

#include "gold-threads.h"
#include "workqueue.h"

// This is an internal header file for different gold workqueue
// implementations.

namespace gold
{

class Workqueue_thread;

// The Workqueue_runner abstract class.  This is the interface used by
// the general workqueue code to actually run a task.

class Workqueue_runner
{
 public:
  Workqueue_runner(Workqueue* workqueue)
    : workqueue_(workqueue)
  { }
  virtual ~Workqueue_runner()
  { }

  // Run a task.  This is always called in the main thread.
  virtual void
  run(Task*, Task_locker*) = 0;

  // Set the number of threads to use.  This is ignored when not using
  // threads.
  virtual void
  set_thread_count(int) = 0;

 protected:
  // This is called by an implementation when a task is completed.
  void completed(Task* t, Task_locker* tl)
  { this->workqueue_->completed(t, tl); }

  Workqueue* get_workqueue() const
  { return this->workqueue_; }

 private:
  Workqueue* workqueue_;
};

// The threaded instantiation of Workqueue_runner.

class Workqueue_runner_threadpool : public Workqueue_runner
{
 public:
  Workqueue_runner_threadpool(Workqueue* workqueue);

  ~Workqueue_runner_threadpool();

  void
  run(Task*, Task_locker*);

  void
  set_thread_count(int);

 private:
  // This class can not be copied.
  Workqueue_runner_threadpool(const Workqueue_runner_threadpool&);
  Workqueue_runner_threadpool& operator=(const Workqueue_runner_threadpool&);

  // Return the next Task and Task_locker to run.  This returns false
  // if the calling thread should simply exit.
  bool
  get_next(Task**, Task_locker**);

  // This is called when the thread completes a task.
  void
  thread_completed(Task*, Task_locker*);

  // The Workqueue_thread class calls functions from this and from the
  // parent Workqueue_runner.
  friend class Workqueue_thread;

  // An entry on the queue of tasks to run.
  typedef std::pair<Task*, Task_locker*> Task_queue_entry;

  // A queue of tasks to run.
  typedef std::queue<Task_queue_entry> Task_queue;

  // The number of threads we want to create.  This is only changed in
  // the main thread or when only one thread is running.  This is set
  // to zero when all threads should exit.
  int desired_thread_count_;
  // A lock controlling access to the remaining fields.
  Lock lock_;
  // The number of threads we have created.
  int actual_thread_count_;
  // The number of threads which are running a task.
  int running_thread_count_;
  // A queue of tasks to run.
  Task_queue task_queue_;
  // A condition variable which signals when the task_queue_ changed.
  Condvar task_queue_condvar_;
};

} // End namespace gold.

#endif // !defined(GOLD_WORKQUEUE_INTERNAL_H)
