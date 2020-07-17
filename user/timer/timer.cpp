#include "timer.h"
#include <algorithm>
#include <glog/logging.h>
#include <iostream>
namespace kun {
  Timer::Timer() = default;
  Timer::Timer(bool backThread) : backThread_(backThread) {
    if (backThread) {
     start();
    }
  }

  void Timer::runThd() {
    while (true) {
      run();
      std::unique_lock<std::mutex> ulock(lock_);
      int sec = getNextTimeout();
      if (sec < 0) {
        notEmpty_.wait(ulock);
      } else if (sec > 0){
        notEmpty_.wait_for(ulock, 
          std::chrono::seconds(sec));
      }
    }
  }

  Timer::~Timer() = default;

  void Timer::run() {
    std::vector<TimerTask> tasks;
    getTimeoutTasks(tasks);

    for (auto &task : tasks) {
      task.run();
    }

    for (auto &task :tasks) {
      if (!task.everest && --task.callCnt <= 0) {
        continue;
      }
      addFuncLocal(
          task.seconds, task.callCnt, task.func,
          false);
    }
  }

  int Timer::getNextTimeout() {
    int sec = 0;
    if (!tasks_.empty()) {
      sec = tasks_.front().stamp -time(NULL);
      if (sec < 0) { sec = 0;}
    } else {
      sec = -1;
    }
    return sec;
  }

  void Timer::getTimeoutTasks(
      std::vector<TimerTask> &tasks) {
    long current = time(NULL);
    std::lock_guard<std::mutex> guard(lock_);
    while (!tasks_.empty()) {
      auto &checkTask = tasks_.front();
      if (checkTask.stamp > current) {
        break;
      }
      auto task = tasks_.front();
      std::pop_heap(tasks_.begin(), tasks_.end());
      tasks_.pop_back();
      tasks.push_back(task);
    }

  }

int Timer::addFuncLocal(int sec,
    int count, 
    TimerFunc func,
    bool notify) {
  TimerTask tb;
  tb.seconds = sec;
  tb.stamp = sec + time(NULL);
  tb.func = func;
  tb.everest = count <= 0;
  tb.callCnt = count;
  std::lock_guard<std::mutex> guard(lock_);
  tasks_.push_back(tb);
  std::push_heap(tasks_.begin(), 
      tasks_.end()
      );
  if(backThread_ && notify) {
    notEmpty_.notify_one();
  }
  return 0;
}

int Timer::addFunc(int sec,
    int count, 
    TimerFunc func) {
  addFuncLocal(sec, count, func, true);
  return 0;
}

void Timer::start() {
  t_= new std::thread(&Timer::runThd, this);   
  t_->detach();
}

}

