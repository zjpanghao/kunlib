#ifndef INCLUDE_HPV_POOL_H
#define INCLUDE_HPV_POOL_H
#include <queue>
#include <memory>
#include <thread>
#include <functional>
#include <condition_variable>
#include <vector>
struct event_base;
struct event;
class  HpvThread;
typedef  void (*ConnCb) (struct event_base*,HpvThread*, void*);

struct  HpvPoolCb  {
  void *arg;
  ConnCb cb;
};

class HpvThread {
  public:
    HpvThread();
    void execute(HpvPoolCb func);
    int rdr() {return rdr_;}
    int taskCount() {return taskCount_;}
    void setTaskCount(int taskCount) {
      taskCount_ = taskCount;
    }

  private:
    int taskCount_{0};
    int rdr_;
    int wdr_;
    std::unique_ptr<std::thread> t_;
    struct event_base* base_;
    struct event *event_;
    void run();
   static void readCmd(int sock, short event, void *arg);
};

struct HpvPool {
 public:
   HpvPool(int num);
   void execute(HpvPoolCb func);

 private:
   std::vector<std::unique_ptr<HpvThread>> threads_;
};
#endif
