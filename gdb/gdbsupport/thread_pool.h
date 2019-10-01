#ifndef PARALLEL_FOR_H
#define PARALLEL_FOR_H

#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace gdb {

class thread_pool {
 public:
  thread_pool() : m_started (false), m_shutdown (false) {}
  ~thread_pool();

  bool started() const { return m_started; }

  void start(size_t num_threads);

  typedef std::function<void ()> task;
  void post_task(task t) {
    std::lock_guard<std::mutex> guard (m_tasks_mutex);
    m_tasks.push (t);
    m_tasks_cv.notify_one ();
  }

 private:
  void thread_function();

  bool m_started;
  std::vector<std::thread> m_threads;
  std::atomic<bool> m_shutdown;
  std::queue<task> m_tasks;
  std::condition_variable m_tasks_cv;
  std::mutex m_tasks_mutex;
};

}

#endif
