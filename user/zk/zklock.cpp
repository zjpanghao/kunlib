#include "zklock.h"
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>
#include <set>
#include <unistd.h>
#include <iostream>
static void watchFunc(zhandle_t *zh, int type,
        int state, const char *path,void *watcherCtx) {
  std::cout <<"callback:" <<  path <<  " type:" << type << " state:" << state << std::endl;
  ZKLock::ZkNodeCb *cb = static_cast<ZKLock::ZkNodeCb*> (watcherCtx);
  if (type == ZOO_DELETED_EVENT && state == ZOO_CONNECTED_STATE) {
    std::lock_guard<std::mutex> lock(cb->lock);
    cb->done = true;
    cb->cond.notify_one();
  }
}

void ZKLock::lock() {
  const clientid_t *cid = zoo_client_id(zh_);
  // get the session id
  int64_t session = cid->client_id;
  //std::cout << "session id:" << session << std::endl;
  char buf[512];
  std::string path;
  std::string fullPath;
  int  ret = 0;
  int tmCnt = 3;
  while(true)  {
    ret = zoo_create(zh_, path_.c_str() , NULL, 0,  &ZOO_OPEN_ACL_UNSAFE,
        ZOO_EPHEMERAL|ZOO_SEQUENCE, buf, (512 - 1));
    if (ret ==0 || ret == -110) {
      break;
    }
    sleep(1);
  }

  std::cout <<"cn  ret:" << ret << std::endl;
  path = path_ + "/";
  ret = zoo_create(zh_, path.c_str() , NULL, 0,  &ZOO_OPEN_ACL_UNSAFE,
      ZOO_EPHEMERAL|ZOO_SEQUENCE, buf, (512 - 1));
  std::cout << "create node :" << buf << std::endl;
  fullPath = buf;
  childPath_ = buf;
  int inx = fullPath.rfind("/");
  if (inx == -1) {
    return;
  }
  std::string createNode = fullPath.substr(path.length());
  std::atomic<bool> flag(false);
  while (true) {
    struct String_vector vectorst;
    vectorst.data = NULL;
    vectorst.count = 0;
    int rc = zoo_get_children(zh_, path_.c_str(), 0, &vectorst);
    if (rc < 0) {
      //std::cout << "get child error:" << rc << std::endl;
      sleep(1);
      continue;
    }
    std::set<std::string> datas;
    for (int i = 0; i < vectorst.count; i++) {
      datas.insert(vectorst.data[i]);
    }
    auto it = datas.begin();
    if (createNode == *it) {
      //std::cout << "equals" << std::endl;
      return;
    }
    zkNodeCb_.done = false;
    std::string pre = "";
    while (it != datas.end() && createNode != *it) {
      pre = *it;
      it++;
    }
    for (int i = 0; i < vectorst.count; i++) {
      free(vectorst.data[i]);
    }

    Stat stat;
    flag = false;
    rc = 0;
    std::string watchNode = path + pre;
    if (ZOK == (rc = zoo_wexists(zh_, watchNode.c_str(), watchFunc, (void*)&zkNodeCb_, &stat))) {
    //  std::cout << "wait:" << watchNode << std::endl;
      std::unique_lock<std::mutex> lock(zkNodeCb_.lock);
      zkNodeCb_.cond.wait(lock, [this]() {return zkNodeCb_.done;});
    } else {
      //std::cout << "wait failed rc :" << rc << " path:" << watchNode << std::endl;
    }
  }
  
}

ZKLock::~ZKLock() {
  zoo_delete(zh_, childPath_.c_str(), -1);
}

ZKLock::ZKLock(zhandle_t *zh, const std::string &path) : zh_(zh), path_(path) {
  lock();
}
