#include "timer.h"
#include <algorithm>
#include <glog/logging.h>
namespace kun {
  void Timer::run() {
    while (true) {
      sleep(1);
      std::vector<TimerTask> tasks;
      getTimeoutTasks(tasks);
      for (auto &task : tasks) {
        task.run();
      }
      for (auto &task :tasks) {
        addFunc(task.seconds, task.func);
      }
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

int Timer::addFunc(int sec, TimerFunc func) {
  TimerTask tb;
  tb.seconds = sec;
  tb.stamp = sec + time(NULL);
  tb.func = func;
  std::lock_guard<std::mutex> guard(lock_);
  tasks_.push_back(tb);
  std::push_heap(tasks_.begin(), 
      tasks_.end()
      );
  return 0;
}

}

void testFunc(void *a) {
  LOG(INFO) << time(NULL) << "test" << *(int*)a << std::endl;
}

int main() {
  kun::Timer &tm = kun::Timer::getTimer();
  
  int b = 3;
  int a = 2;;
//  tm->addFunc(10, &a, testFunc);
  std::function<void()> func = std::bind(testFunc, &a);
  tm.addFunc(90,  func);
  int cnt = 100;
  while (true) {
    sleep(cnt);
    int *d = new int;
    *d = cnt;
//    tm->addFunc(cnt, d, testFunc);
    cnt *= 2;
  }
  return 0;
}

