#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename T> using Task = std::function<T()>;

template <typename T> class ThreadPool {
public:
  ThreadPool(std::size_t threads_count = std::thread::hardware_concurrency());
  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool<T> &operator=(const ThreadPool &other) = delete;
  ~ThreadPool();

  auto enqueue(const Task<T> &task);

private:
  bool terminate;
  std::condition_variable notifier;
  std::mutex mutex;
  std::vector<std::thread> threads;
  std::queue<Task<T>> tasks;

  void start();
  void stop();
  void loop();
};

template <typename T>
ThreadPool<T>::ThreadPool(std::size_t threads_count)
    : terminate(false), threads(std::vector<std::thread>(threads_count)) {
  start();
}

template <typename T> ThreadPool<T>::~ThreadPool() { stop(); }

template <typename T> auto ThreadPool<T>::enqueue(const Task<T> &task) {
  std::shared_ptr<std::packaged_task<decltype(task())()>> wrapper =
      std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));

  {
    std::unique_lock<std::mutex> lock(mutex);
    tasks.emplace([=] { (*wrapper)(); });
  }

  notifier.notify_one();
  return wrapper->get_future();
}

template <typename T> void ThreadPool<T>::start() {
  for (std::thread &thread : threads) {
    thread = std::thread(&ThreadPool<T>::loop, this);
  }
}

template <typename T> void ThreadPool<T>::stop() {
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (terminate) {
      return;
    }

    terminate = true;
  }

  notifier.notify_all();

  for (std::thread &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

template <typename T> void ThreadPool<T>::loop() {
  while (true) {
    Task<T> task;

    {
      std::unique_lock<std::mutex> lock(mutex);
      notifier.wait(lock, [this]() { return !tasks.empty() || terminate; });

      if (terminate && tasks.empty()) {
        return;
      }

      task = std::move(tasks.front());
      tasks.pop();
    }

    task();
  }
}

#endif