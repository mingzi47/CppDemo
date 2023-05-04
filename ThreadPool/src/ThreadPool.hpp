#pragma once
#include "SafeQueue.hpp"
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

namespace ctp {

class ThreadPool {
private:

  size_t threads_;
  bool shutdown_;
  SafeQueue<std::function<void()>> tasks_;
  std::vector<std::thread> works_;
  std::condition_variable condition_;
  std::mutex mutex_;

public:
  ThreadPool(size_t threads = 2) { init(threads); };
  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool(ThreadPool &&other) = delete;
  ThreadPool &operator=(const ThreadPool &other) = delete;
  ThreadPool &operator=(ThreadPool &&other) = delete;
  ~ThreadPool() { shutdown(); };

  void init(size_t threads);
  void shutdown();
  template <typename F, typename... Arg>
  auto submit(F &&f, Arg &&...args) -> std::future<decltype(f(args...))>;
};


void ThreadPool::init(size_t threads) {

  threads_ = threads;
  shutdown_ = false;
  while (tasks_.size() != 0) {
    tasks_.pop();
  }
  {
    std::vector<std::thread> tmp_works{};
    works_.swap(tmp_works);
  }
  works_.reserve(threads_);
  works_.resize(threads_);
  for (size_t i = 0; i < threads_; ++i) {
    works_[i] = std::thread([this] {
      std::function<void()> func;
      for (;;) {
        {
          std::unique_lock lock(mutex_);
          condition_.wait(lock, [this] {
            return shutdown_ || tasks_.size() > 0;
          });
          if (shutdown_ && tasks_.size() == 0) {
            return;
          }
          func = std::move(tasks_.pop());
        }
        func();
      }
    });
  }
}

void ThreadPool::shutdown() {

  if (shutdown_) {
    return;
  }
  shutdown_ = true;
  condition_.notify_all();
  for (size_t i = 0; i < threads_; ++i) {
    if (works_[i].joinable()) {
      works_[i].join();
    }
  }
  threads_ = 0;
}

template <typename F, typename... Arg>
auto ThreadPool::submit(F &&f, Arg &&...args)
    -> std::future<decltype(f(args...))> {

  std::function<decltype(f(args...))()> func =
      std::bind(std::forward<F>(f), std::forward<Arg>(args)...);

  auto task_ptr =
      std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

  std::function<void()> wrap_task = [task_ptr] { (*task_ptr)(); };

  tasks_.push(wrap_task);

  condition_.notify_one();

  return task_ptr->get_future();
}

} // namespace ctp
