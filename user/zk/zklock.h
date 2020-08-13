#ifndef INCLUDE_ZKLOCK_H  
#define INCLUDE_ZKLOCK_H  
#include <string>
#include "zookeeper/zookeeper.h"
#include <mutex>
#include <condition_variable>
class ZKHandle {
  public:
    ZKHandle(const std::string &server) 
      : server_(server) {
        char buffer[512];
        zh_ = zookeeper_init(server.c_str(), 
              NULL, 30000, 0, 0, 0);
    }

    zhandle_t *handle() {
      return zh_;
    }

  private:
    int status;
    zhandle_t *zh_;
    std::string server_;
};

class ZKLock  {
 public:
   struct ZkNodeCb {
     std::mutex lock;
     std::condition_variable cond;
     bool done{false};
   } zkNodeCb_;
   ZKLock(zhandle_t *zh, const std::string &path);
   ~ZKLock();

 private:
   void lock();
   int unlock();
   zhandle_t *zh_;
   std::string path_;
   std::string childPath_;
};
#endif
