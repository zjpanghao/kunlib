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
      bool operator < 
        (const TimerTask &a) const {
        return stamp > a.stamp;
      }

      long stamp;
      int seconds;
      bool everest{true};
      int callCnt{0};
      TimerFunc func;
   };

   Timer();
   ~Timer();
#if 0
   Timer(const Timer &timer);
   Timer& operator=(Timer &timer);
   Timer(Timer &&timer);
   Timer& operator=(Timer &&timer);
#endif

   int addFunc(int sec, int count, TimerFunc func);

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
   mutable std::mutex lock_;
};

}
#endif
