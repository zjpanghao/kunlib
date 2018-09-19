#ifndef QUANT_THREAD_POOL_H
#define QUANT_THREAD_POOL_H
#include <thread>
#include <queue>
#include <thread>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>

class Runnable {
 public:
  virtual ~Runnable() {
  }
  virtual void Run() {
  }
};

class ExecutorService {
 public:
  ExecutorService(int normal) {
    for (int i = 0; i < normal; i++) {
      std::thread t(&ExecutorService::Run, this);
      t.detach();
    }
  }

  void Run() {
    while (true) {
      std::unique_lock<std::mutex>  ulock(lock_);
      cond_.wait(ulock, [this] { return !queue_.empty();});
      std::unique_ptr<Runnable> able = std::move(queue_.front());
      queue_.pop();
      ulock.unlock();
      able->Run();
    }

  }

  void Execute(std::unique_ptr<Runnable> able) {
    std::lock_guard<std::mutex> guard(lock_);
    queue_.push(std::move(able));
    cond_.notify_one();
  }
 private:
  std::queue<std::unique_ptr<Runnable> > queue_;
  std::condition_variable  cond_;
  std::mutex lock_;

};

class Executors {
 public:
  static ExecutorService *NewFixPool(int num) {
    return new ExecutorService(num);
  }
};

#endif
