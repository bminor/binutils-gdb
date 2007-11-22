// workqueue.cc -- the workqueue for gold

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

#include "gold.h"

#include "debug.h"
#include "workqueue.h"
#include "workqueue-internal.h"

namespace gold
{

// Task_token methods.

Task_token::Task_token()
  : is_blocker_(false), readers_(0), writer_(NULL)
{
}

Task_token::~Task_token()
{
  gold_assert(this->readers_ == 0 && this->writer_ == NULL);
}

bool
Task_token::is_readable() const
{
  gold_assert(!this->is_blocker_);
  return this->writer_ == NULL;
}

void
Task_token::add_reader()
{
  gold_assert(!this->is_blocker_);
  gold_assert(this->is_readable());
  ++this->readers_;
}

void
Task_token::remove_reader()
{
  gold_assert(!this->is_blocker_);
  gold_assert(this->readers_ > 0);
  --this->readers_;
}

bool
Task_token::is_writable() const
{
  gold_assert(!this->is_blocker_);
  return this->writer_ == NULL && this->readers_ == 0;
}

void
Task_token::add_writer(const Task* t)
{
  gold_assert(!this->is_blocker_);
  gold_assert(this->is_writable());
  this->writer_ = t;
}

void
Task_token::remove_writer(const Task* t)
{
  gold_assert(!this->is_blocker_);
  gold_assert(this->writer_ == t);
  this->writer_ = NULL;
}

bool
Task_token::has_write_lock(const Task* t)
{
  gold_assert(!this->is_blocker_);
  return this->writer_ == t;
}

// For blockers, we just use the readers_ field.

void
Task_token::add_blocker()
{
  if (this->readers_ == 0 && this->writer_ == NULL)
    this->is_blocker_ = true;
  else
    gold_assert(this->is_blocker_);
  ++this->readers_;
}

bool
Task_token::remove_blocker()
{
  gold_assert(this->is_blocker_ && this->readers_ > 0);
  --this->readers_;
  return this->readers_ == 0;
}

bool
Task_token::is_blocked() const
{
  gold_assert(this->is_blocker_
	      || (this->readers_ == 0 && this->writer_ == NULL));
  return this->readers_ > 0;
}

// The Task_block_token class.

Task_block_token::Task_block_token(Task_token& token, Workqueue* workqueue)
  : token_(token), workqueue_(workqueue)
{
  // We must increment the block count when the task is created and
  // put on the queue.  This object is created when the task is run,
  // so we don't increment the block count here.
  gold_assert(this->token_.is_blocked());
}

Task_block_token::~Task_block_token()
{
  if (this->token_.remove_blocker())
    {
      // Tell the workqueue that a blocker was cleared.  This is
      // always called in the main thread, so no locking is required.
      this->workqueue_->cleared_blocker();
    }
}

// The simple single-threaded implementation of Workqueue_runner.

class Workqueue_runner_single : public Workqueue_runner
{
 public:
  Workqueue_runner_single(Workqueue* workqueue)
    : Workqueue_runner(workqueue)
  { }
  ~Workqueue_runner_single()
  { }

  void
  run(Task*, Task_locker*);

  void
  set_thread_count(int);
};

void
Workqueue_runner_single::run(Task* t, Task_locker* tl)
{
  t->run(this->get_workqueue());
  this->completed(t, tl);
}

void
Workqueue_runner_single::set_thread_count(int thread_count)
{
  gold_assert(thread_count > 0);
}

// Workqueue methods.

Workqueue::Workqueue(const General_options& options)
  : tasks_lock_(),
    first_tasks_(),
    tasks_(),
    completed_lock_(),
    completed_(),
    running_(0),
    queued_(0),
    completed_condvar_(this->completed_lock_),
    cleared_blockers_(0),
    desired_thread_count_(1)
{
  bool threads = options.threads();
#ifndef ENABLE_THREADS
  threads = false;
#endif
  if (!threads)
    this->runner_ = new Workqueue_runner_single(this);
  else
    {
#ifdef ENABLE_THREADS
      this->runner_ = new Workqueue_runner_threadpool(this);
#else
      gold_unreachable();
#endif
    }
}

Workqueue::~Workqueue()
{
  gold_assert(this->first_tasks_.empty());
  gold_assert(this->tasks_.empty());
  gold_assert(this->completed_.empty());
  gold_assert(this->running_ == 0);
}

// Add a task to the queue.

void
Workqueue::queue(Task* t)
{
  {
    Hold_lock hl(this->tasks_lock_);
    this->tasks_.push_back(t);
  }
  {
    Hold_lock hl(this->completed_lock_);
    ++this->queued_;
  }
}

// Add a task to the front of the queue.

void
Workqueue::queue_front(Task* t)
{
  {
    Hold_lock hl(this->tasks_lock_);
    this->first_tasks_.push_front(t);
  }
  {
    Hold_lock hl(this->completed_lock_);
    ++this->queued_;
  }
}

// Clear the list of completed tasks.  Return whether we cleared
// anything.  The completed_lock_ must be held when this is called.

bool
Workqueue::clear_completed()
{
  if (this->completed_.empty())
    return false;
  do
    {
      delete this->completed_.front();
      this->completed_.pop_front();
    }
  while (!this->completed_.empty());
  return true;
}

// Find a runnable task in TASKS, which is non-empty.  Return NULL if
// none could be found.  The tasks_lock_ must be held when this is
// called.  Sets ALL_BLOCKED if all non-runnable tasks are waiting on
// a blocker.

Task*
Workqueue::find_runnable(Task_list* tasks, bool* all_blocked)
{
  Task* tlast = tasks->back();
  *all_blocked = true;
  Task* t;
  do
    {
      t = tasks->front();
      tasks->pop_front();

      Task::Is_runnable_type is_runnable = t->is_runnable(this);
      if (is_runnable == Task::IS_RUNNABLE)
	{
	  {
	    Hold_lock hl(this->completed_lock_);
	    --this->queued_;
	  }

	  return t;
	}

      if (is_runnable != Task::IS_BLOCKED)
	*all_blocked = false;

      tasks->push_back(t);
    }
  while (t != tlast);

  // We couldn't find any runnable task.
  return NULL;
}

// Process all the tasks on the workqueue.  This is the main loop in
// the linker.  Note that as we process tasks, new tasks will be
// added.

void
Workqueue::process()
{
  while (true)
    {
      Task* t;
      bool empty;
      bool all_blocked;

      // Don't start more tasks than desired.
      {
	Hold_lock hl(this->completed_lock_);

	this->clear_completed();
	while (this->running_ >= this->desired_thread_count_)
	  {
	    this->completed_condvar_.wait();
	    this->clear_completed();
	  }
      }

      {
	Hold_lock hl(this->tasks_lock_);

	bool first_empty;
	bool all_blocked_first;
	if (this->first_tasks_.empty())
	  {
	    t = NULL;
	    empty = true;
	    first_empty = true;
	    all_blocked_first = false;
	  }
	else
	  {
	    t = this->find_runnable(&this->first_tasks_, &all_blocked_first);
	    empty = false;
	    first_empty = false;
	  }

	if (t == NULL)
	  {
	    if (this->tasks_.empty())
	      all_blocked = false;
	    else
	      {
		t = this->find_runnable(&this->tasks_, &all_blocked);
		if (!first_empty && !all_blocked_first)
		  all_blocked = false;
		empty = false;
	      }
	  }
      }

      // If T != NULL, it is a task we can run.
      // If T == NULL && empty, then there are no tasks waiting to
      // be run.
      // If T == NULL && !empty, then there tasks waiting to be
      // run, but they are waiting for something to unlock.

      if (t != NULL)
	this->run(t);
      else if (!empty)
	{
	  {
	    Hold_lock hl(this->completed_lock_);

	    // There must be something for us to wait for, or we won't
	    // be able to make progress.
	    gold_assert(this->running_ > 0 || !this->completed_.empty());

	    if (all_blocked)
	      {
		this->cleared_blockers_ = 0;
		int queued = this->queued_;
		this->clear_completed();
		while (this->cleared_blockers_ == 0
		       && queued == this->queued_)
		  {
		    if (this->running_ <= 0)
		      {
			this->show_queued_tasks();
			gold_unreachable();
		      }
		    this->completed_condvar_.wait();
		    this->clear_completed();
		  }
	      }
	    else
	      {
		if (this->running_ > 0)
		  {
		    // Wait for a task to finish.
		    this->completed_condvar_.wait();
		  }
		this->clear_completed();
	      }
	  }
	}
      else
	{
	  {
	    Hold_lock hl(this->completed_lock_);

	    // If there are no running tasks, then we are done.
	    if (this->running_ == 0)
	      {
		this->clear_completed();
		return;
	      }

	    // Wait for a task to finish.  Then we have to loop around
	    // again in case it added any new tasks before finishing.
	    this->completed_condvar_.wait();
	    this->clear_completed();
	  }
	}
    }
}

// Run a task.  This is always called in the main thread.

void
Workqueue::run(Task* t)
{
  gold_debug(DEBUG_TASK, "starting  task %s", t->name().c_str());

  {
    Hold_lock hl(this->completed_lock_);
    ++this->running_;
  }
  this->runner_->run(t, t->locks(this));
}

// This is called when a task is completed to put the locks on the
// list to be released.  We use a list because we only want the locks
// to be released in the main thread.

void
Workqueue::completed(Task* t, Task_locker* tl)
{
  gold_debug(DEBUG_TASK, "completed task %s", t->name().c_str());

  {
    Hold_lock hl(this->completed_lock_);
    gold_assert(this->running_ > 0);
    --this->running_;
    this->completed_.push_back(tl);
    this->completed_condvar_.signal();
  }

  delete t;
}

// This is called when the last task for a blocker has completed.
// This is always called in the main thread.

void
Workqueue::cleared_blocker()
{
  ++this->cleared_blockers_;
}

// Set the number of threads to use for the workqueue, if we are using
// threads.

void
Workqueue::set_thread_count(int threads)
{
  gold_assert(threads > 0);
  this->desired_thread_count_ = threads;
  this->runner_->set_thread_count(threads);
}

// Dump the list of queued tasks and their current state, for
// debugging purposes.

void
Workqueue::show_queued_tasks()
{
  fprintf(stderr, _("gold task queue:\n"));
  Hold_lock hl(this->tasks_lock_);
  for (Task_list::const_iterator p = this->tasks_.begin();
       p != this->tasks_.end();
       ++p)
    {
      fprintf(stderr, "  %s ", (*p)->name().c_str());
      switch ((*p)->is_runnable(this))
	{
	case Task::IS_RUNNABLE:
	  fprintf(stderr, "runnable");
	  break;
	case Task::IS_BLOCKED:
	  fprintf(stderr, "blocked");
	  break;
	case Task::IS_LOCKED:
	  fprintf(stderr, "locked");
	  break;
	default:
	  gold_unreachable();
	}
      putc('\n', stderr);
    }
}

} // End namespace gold.
