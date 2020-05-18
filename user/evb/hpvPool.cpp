#include "hpvPool.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include "evHpv.h"
#include "threadpool/thread_pool.h"
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
void HpvThread::execute(HpvPoolCb func) {
  taskCount_++;
  int rc = send(wdr_, &func, sizeof(func), 0);
  
}

void HpvPool::execute(HpvPoolCb func) {
  int min = 0;
  int backlog;
  HpvThread *thd = NULL;
  for(auto &u: threads_) {
    backlog = u->taskCount();
    if (thd == NULL || backlog < min) {
      min = backlog;
      thd = u.get();
    }
  }
  thd->execute(func);
}

HpvPool::HpvPool(int num) {
 for (int i = 0; i < num; i++) {
   std::unique_ptr<HpvThread> thd(new HpvThread());
       threads_.push_back(std::move(thd));

 }
}

void HpvThread::readCmd(int sock, short event, void *arg) {
  HpvThread *thread = (HpvThread*)arg;
  HpvPoolCb cb;
  int n = recv(sock, &cb, sizeof(cb), 0);
  cb.cb(thread->base_, thread, cb.arg);
}

void HpvThread::run() {
  base_ = event_base_new();
  int       fds[2];
  if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
    return;
  }
  evutil_make_socket_nonblocking(fds[0]);
  evutil_make_socket_nonblocking(fds[1]);
  rdr_ = fds[0];
  wdr_ = fds[1];
  event_ = event_new(base_, rdr_,
      EV_READ | EV_PERSIST, &HpvThread::readCmd, this);
  event_add(event_,NULL);
  event_base_dispatch(base_);
}

HpvThread::HpvThread() {
  t_.reset(new std::thread(&HpvThread::run,this));
}
