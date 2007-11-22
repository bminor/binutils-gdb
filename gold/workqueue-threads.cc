// workqueue-threads.cc -- the threaded workqueue for gold

// Copyright 2007 Free Software Foundation, Inc.
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

// This file holds the workqueue implementation which may be used when
// using threads.

#include "gold.h"

#ifdef ENABLE_THREADS

#include <cstring>
#include <pthread.h>

#include "debug.h"
#include "gold-threads.h"
#include "workqueue.h"
#include "workqueue-internal.h"

namespace gold
{

// Class Workqueue_thread represents a single thread.  Creating an
// instance of this spawns a new thread.

class Workqueue_thread
{
 public:
  Workqueue_thread(Workqueue_runner_threadpool*);

  ~Workqueue_thread();

 private:
  // This class can not be copied.
  Workqueue_thread(const Workqueue_thread&);
  Workqueue_thread& operator=(const Workqueue_thread&);

  // Check for error from a pthread function.
  void
  check(const char* function, int err) const;

  // A function to pass to pthread_create.  This is called with a
  // pointer to an instance of this object.
  static void*
  thread_body(void*);

  // The main loop of the thread.
  void
  run();

  // A pointer to the threadpool that this thread is part of.
  Workqueue_runner_threadpool* threadpool_;
  // The thread ID.
  pthread_t tid_;
};

// Create the thread in the constructor.

Workqueue_thread::Workqueue_thread(Workqueue_runner_threadpool* threadpool)
  : threadpool_(threadpool)
{
  pthread_attr_t attr;
  int err = pthread_attr_init(&attr);
  this->check("pthread_attr_init", err);

  err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  this->check("pthread_attr_setdetachstate", err);

  err = pthread_create(&this->tid_, &attr, &Workqueue_thread::thread_body,
		       reinterpret_cast<void*>(this));
  this->check("pthread_create", err);

  err = pthread_attr_destroy(&attr);
  this->check("pthread_attr_destroy", err);
}

// The destructor will be called when the thread is exiting.

Workqueue_thread::~Workqueue_thread()
{
}

// Check for an error.

void
Workqueue_thread::check(const char* function, int err) const
{
  if (err != 0)
    gold_fatal(_("%s failed: %s"), function, strerror(err));
}

// Passed to pthread_create.

extern "C"
void*
Workqueue_thread::thread_body(void* arg)
{
  Workqueue_thread* pwt = reinterpret_cast<Workqueue_thread*>(arg);
  pwt->run();

  // Delete the thread object as we exit.
  delete pwt;

  return NULL;
}

// This is the main loop of a worker thread.  It picks up a new Task
// and runs it.

void
Workqueue_thread::run()
{
  Workqueue_runner_threadpool* threadpool = this->threadpool_;
  Workqueue* workqueue = threadpool->get_workqueue();

  while (true)
    {
      Task* t;
      Task_locker* tl;
      if (!threadpool->get_next(&t, &tl))
	return;

      gold_debug(DEBUG_TASK, "running   task %s", t->name().c_str());

      t->run(workqueue);
      threadpool->thread_completed(t, tl);
    }
}

// Class Workqueue_runner_threadpool.

// Constructor.

Workqueue_runner_threadpool::Workqueue_runner_threadpool(Workqueue* workqueue)
  : Workqueue_runner(workqueue),
    desired_thread_count_(0),
    lock_(),
    actual_thread_count_(0),
    running_thread_count_(0),
    task_queue_(),
    task_queue_condvar_(this->lock_)
{
}

// Destructor.

Workqueue_runner_threadpool::~Workqueue_runner_threadpool()
{
  // Tell the threads to exit.
  Hold_lock hl(this->lock_);
  this->desired_thread_count_ = 0;
  this->task_queue_condvar_.broadcast();
}

// Run a task.  This doesn't actually run the task: it pushes on the
// queue of tasks to run.  This is always called in the main thread.

void
Workqueue_runner_threadpool::run(Task* t, Task_locker* tl)
{
  Hold_lock hl(this->lock_);

  // This is where we create threads as needed, subject to the limit
  // of the desired thread count.
  gold_assert(this->desired_thread_count_ > 0);
  gold_assert(this->actual_thread_count_ >= this->running_thread_count_);
  if (this->actual_thread_count_ == this->running_thread_count_
      && this->actual_thread_count_ < this->desired_thread_count_)
    {
      // Note that threads delete themselves when they exit, so we
      // don't keep pointers to them.
      new Workqueue_thread(this);
      ++this->actual_thread_count_;
    }

  this->task_queue_.push(std::make_pair(t, tl));
  this->task_queue_condvar_.signal();
}

// Set the thread count.  This is only called in the main thread, and
// is only called when there are no threads running.

void
Workqueue_runner_threadpool::set_thread_count(int thread_count)
{
  gold_assert(this->running_thread_count_ <= 1);
  gold_assert(thread_count > 0);
  this->desired_thread_count_ = thread_count;
}

// Get the next task to run.  This is always called by an instance of
// Workqueue_thread, and is never called in the main thread.  It
// returns false if the calling thread should exit.

bool
Workqueue_runner_threadpool::get_next(Task** pt, Task_locker** ptl)
{
  Hold_lock hl(this->lock_);

  // This is where we destroy threads, by telling them to exit.
  gold_assert(this->actual_thread_count_ > this->running_thread_count_);
  if (this->actual_thread_count_ > this->desired_thread_count_)
    {
      --this->actual_thread_count_;
      return false;
    }

  while (this->task_queue_.empty() && this->desired_thread_count_ > 0)
    {
      // Wait for a new task to become available.
      this->task_queue_condvar_.wait();
    }

  // Check whether we are exiting.
  if (this->desired_thread_count_ == 0)
    {
      gold_assert(this->actual_thread_count_ > 0);
      --this->actual_thread_count_;
      return false;
    }

  *pt = this->task_queue_.front().first;
  *ptl = this->task_queue_.front().second;
  this->task_queue_.pop();

  ++this->running_thread_count_;

  return true;
}

// This is called when a thread completes its task.

void
Workqueue_runner_threadpool::thread_completed(Task* t, Task_locker* tl)
{
  {
    Hold_lock hl(this->lock_);
    gold_assert(this->actual_thread_count_ > 0);
    gold_assert(this->running_thread_count_ > 0);
    --this->running_thread_count_;
  }

  this->completed(t, tl);
}

} // End namespace gold.

#endif // defined(ENABLE_THREADS)
