#ifndef QUANT_THREAD_POOL_H
#define QUANT_THREAD_POOL_H
#include <thread>
#include <queue>
#include <thread>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#define MAX_NUM_SAVE 5000
class Runnable {
 public:
  virtual ~Runnable() {
  }
  virtual void Run() {
  }
  virtual void ErrorMsg(int code, const std::string &message) {

  }
};

class ExecutorService {
 public:
  ExecutorService(int normal) : ExecutorService(normal,MAX_NUM_SAVE) {
  }

  ExecutorService(int normal, int queueSize) 
    : queueSize_(queueSize) {
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

  bool Execute(std::unique_ptr<Runnable> able) {
    std::lock_guard<std::mutex> guard(lock_);
    if (queue_.size() > MAX_NUM_SAVE) {
      int half = MAX_NUM_SAVE / 2;
      while (half-- > 0) {
        queue_.pop();
      }
      static const std::string msg = "queue full";
      able->ErrorMsg(1, msg);
      return false;
    }
    queue_.push(std::move(able));
    cond_.notify_one();
    return true;
  }
 private:
  std::queue<std::unique_ptr<Runnable> > queue_;
  std::condition_variable  cond_;
  std::mutex lock_;
  int queueSize_;

};

class Executors {
 public:
  static ExecutorService *NewFixPool(int num) {
    return new ExecutorService(num);
  }
  static ExecutorService *NewFixPool(int num, int queueSize) {
    return new ExecutorService(num, queueSize);
  }
};

#endif
