#ifndef INCLUDE_RESOURCE_H
#define INCLUDE_RESOURCE_H
#include <memory>

namespace kunyan {
  class Config;
}
class RedisPool;
class DBPool;
class Resource {
 public:
   static Resource& getResource();
   Resource() = default;
   void init(const kunyan::Config &config);
   std::shared_ptr<RedisPool> redisPool() {
     return redisPool_;
   }
   std::shared_ptr<DBPool> dbPool() {
     return dbPool_;
   }

 private:
   std::shared_ptr<RedisPool> redisPool_;
   std::shared_ptr<DBPool> dbPool_;
};
#endif
