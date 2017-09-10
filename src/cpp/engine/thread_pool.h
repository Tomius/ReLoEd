// based on https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace std {

template<>
struct less<std::pair<int, std::function<void()>>> {
  constexpr bool operator()(const std::pair<int, std::function<void()>>& lhs,
                            const std::pair<int, std::function<void()>>& rhs) {
    return lhs.first < rhs.first;
  }
};

}

class ThreadPool {
public:
    ThreadPool(size_t);
    void enqueue(int priority, const std::function<void()>& task);
    void clear();
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::priority_queue<std::pair<int, std::function<void()>>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable task_to_do;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
  for(size_t i = 0; i<threads; ++i) {
    workers.emplace_back([this] {
      for(;;) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->task_to_do.wait(lock, [&] { return this->stop || !this->tasks.empty(); });
            if (this->stop) {
              return;
            }
            task = std::move(this->tasks.top().second);
            this->tasks.pop();
        }

        task();
      }
    });
  }
}

// add new work item to the pool
inline void ThreadPool::enqueue(int priority, const std::function<void()>& task) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);

    // don't allow enqueueing after stopping the pool
    if (stop) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    tasks.push(std::make_pair(priority, task));
  }
  task_to_do.notify_one();
}

inline void ThreadPool::clear() {
  std::unique_lock<std::mutex> lock(queue_mutex);
  while (!tasks.empty()) {
    tasks.pop();
  }
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  task_to_do.notify_all();
  for (std::thread &worker: workers) {
    worker.join();
  }
}

#endif
