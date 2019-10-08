#ifndef PARALLEL_FOR_H
#define PARALLEL_FOR_H

#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>

namespace gdb {

class thread_pool {
 public:
  thread_pool() : m_started (false), m_shutdown (false) {}
  ~thread_pool();

  bool started() const { return m_started; }

  void start(size_t num_threads);

  typedef std::packaged_task<void()> task;
  std::future<void> post_task(std::function<void ()> func) {
    task t(func);
    std::future<void> f = t.get_future();
    std::lock_guard<std::mutex> guard (m_tasks_mutex);
    m_tasks.push (std::move (t));
    m_tasks_cv.notify_one ();
    return f;
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
