#include "thread_pool.h"

namespace gdb {

thread_pool::~thread_pool ()
{
  {
    std::lock_guard<std::mutex> guard (m_tasks_mutex);
    m_shutdown = true;
    m_tasks_cv.notify_all ();
  }
  for (auto& t : m_threads)
    t.join();
}

void
thread_pool::start (size_t num_threads)
{
  for (size_t i = 0; i < num_threads; ++i)
    {
      m_threads.emplace_back (&thread_pool::thread_function, this);
    }
  m_started = true;
}


void
thread_pool::thread_function ()
{
  while (!m_shutdown)
    {
      task t;
      {
	std::unique_lock<std::mutex> guard (m_tasks_mutex);
	if (m_shutdown)
	  break;
	if (m_tasks.empty ())
	  m_tasks_cv.wait (guard);
	if (m_shutdown)
	  break;
	if (m_tasks.empty ())
	  continue;
	t = std::move (m_tasks.front());
	m_tasks.pop();
      }
      t ();
    }
}

}
