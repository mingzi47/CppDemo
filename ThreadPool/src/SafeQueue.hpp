#pragma once
#include <mutex>
#include <queue>
#include <shared_mutex>

namespace ctp {

template <typename T> class SafeQueue {
private:
  std::queue<T> que_;
  std::shared_mutex mutex_;

public:
  void push(T &t);
  T pop();
  size_t size(); 
};

template<typename T>
void SafeQueue<T>::push(T &t) {
    std::unique_lock lock(mutex_);
    que_.emplace(t);
}

template<typename T>
T SafeQueue<T>::pop() {
    std::unique_lock lock(mutex_);
    T ret = que_.front();
    que_.pop();
    return ret;
}

template<typename T>
size_t SafeQueue<T>::size() {
    std::shared_lock lock(mutex_);
    return que_.size();
}

} // namespace ctp
