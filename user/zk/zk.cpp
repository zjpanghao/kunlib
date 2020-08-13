#include <zookeeper.h>
#include <iostream>
#include <thread>
#include "zklock.h"
#include <unistd.h>
#include <vector>
void watcher(zhandle_t *zzh, int type, int state, const char *path, void *context) {
  std::cout <<"type:" <<  type << " state:" << state << " path:" << path << std::endl;
}

int main() {
  zhandle_t *zh = NULL;
  const char *hostPort = "192.168.1.113:2181";
  char buffer[512];
  zh = zookeeper_init(hostPort, watcher, 30000, 0, 0, 0);
 #if 0
  struct ACL CREATE_ONLY_ACL[] = {{ZOO_PERM_READ|ZOO_PERM_CREATE, ZOO_ANYONE_ID_UNSAFE /*ZOO_AUTH_IDS*/}};
   struct ACL_vector CREATE_ONLY = {1, CREATE_ONLY_ACL};
  int rc = zoo_create(zh,"/xyz","value", 5, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL|ZOO_SEQUENCE,
      buffer, sizeof(buffer) - 1);
  std::cout << "create buffer:" << buffer <<  " rc:" << rc << std::endl;
  struct Stat stat;
  int buflen;
  rc = zoo_get(zh, "/xyz", 1, buffer, &buflen, &stat);
  std::cout << "get buffer:" << buffer <<  " rc:" << rc << std::endl;

 std::vector<std::thread> tvec;
  for (int i = 0; i < 10; i++) {
    tvec.emplace_back([](zhandle_t *zha, int i) {
    ZKLock zk2(zha, "/xyz");
    std::cout << "zk" << i<< " lock ok" << std::endl; 
    sleep(5);
  }, zh, i);
  }
 
  for (auto &t : tvec) {
    t.join();
  }
 #endif
  ZKLock zk(zh, "/xyz");
  std::cout << "zk start working " << std::endl;
  sleep(60);
  int *p = (int*)NULL;
  *p = 0;
  return 0;
}
