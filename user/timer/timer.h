#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H
#include <functional>
#include <vector>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <algorithm>

namespace kun {
class Timer {
 public:
   typedef std::function<void ()> TimerFunc; 
   class TimerTask {
     public:
       void run() {
         func();
       }
      bool operator < (const TimerTask &a) {
        return stamp > a.stamp;
      }

      long stamp;
      int seconds;
      TimerFunc func;
   };

   Timer() {
      start();
   }

   int addFunc(int sec, TimerFunc func);

   void getTimeoutTasks(
       std::vector<TimerTask> &tasks) ;

   void run();

   static Timer& getTimer() {
      static Timer tm;
      return tm;
   }


 private:
   void start() {
     t_= new std::thread(&Timer::run, this);   
     t_->detach();
   }
   std::vector<TimerTask> tasks_;
   std::thread *t_;
   std::mutex lock_;
};

}
#endif
