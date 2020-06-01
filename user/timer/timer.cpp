#include "timer.h"
#include <algorithm>
#include <glog/logging.h>
#include <iostream>
namespace kun {
  Timer::Timer() = default;
  Timer::Timer(bool backThread) {
    if (backThread) {
      start();
    }
  }

  void Timer::runThd() {
    while (true) {
      run();
      sleep(1);
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
      addFunc(task.seconds, task.callCnt, task.func);
    }
  }

  void Timer::getTimeoutTasks(std::vector<TimerTask> &tasks) {
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

int Timer::addFunc(int sec,
    int count, 
    TimerFunc func) {
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
  return 0;
}

}

void testFunc(void *a) {
  std::cout << time(NULL) << "test" << *(int*)a << std::endl;
}

int main() {
  kun::Timer &tm = kun::Timer::getTimer();
  
  int b = 3;
  int a = 2;;
  std::function<void()> func = std::bind(testFunc, &a);
  tm.addFunc(10, 10, func);
  int cnt = 100;
  while (true) {
    sleep(1);
  }
  return 0;
}

