#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H
#include <functional>
#include <vector>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <algorithm>
#include <condition_variable>
struct event_base;
struct event;

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

   Timer(bool backThread);
   Timer();
   ~Timer();
   static void timeoutCb(int, short, void*);

   int addFunc(int sec, int count, TimerFunc func);
   int addFuncLocal(int sec, int count, TimerFunc func,
       bool notify);

   void getTimeoutTasks(
       std::vector<TimerTask> &tasks) ;

   void run();

   static Timer& getTimer() {
      static Timer tm(true);
      return tm;
   }

   int getNextTimeout();

 private:
   void runThd();
   struct event_base *base_;
   struct event *ev_;

   void start();

   std::vector<TimerTask> tasks_;
   std::thread *t_;
   mutable std::mutex lock_;
   std::condition_variable notEmpty_;
   bool backThread_{false};
};

}
#endif
