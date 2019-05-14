#include "redis_pool.h"
#include <sstream>
#include <config/config.h>
RedisPool *pool_g;

void initRedisPool(const kunyan::Config &config) {
  int port;
  std::string ip = config.get("redis", "ip");
  std::string db = config.get("redis", "db");
  std::string password = config.get("redis", "password");
  std::stringstream ss;
  ss << config.get("redis", "port");
  ss >> port;
  if (ip.length() > 0) {
      pool_g = new RedisPool(ip, 
                port, 
                2, 
                10,
                db,
                password);
  }
}

RedisPool* getRedisPool() {
  return pool_g;
}
