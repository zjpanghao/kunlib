#include "resource.h"
#include "config/config.h"
#include "db/dbpool.h"
#include "predis/redis_pool.h"
Resource &Resource::getResource() {
  static Resource rs;
  return rs;
}

void Resource::init(const kunyan::Config &config) {
  redisPool_ = 
    std::make_shared<RedisPool>(RedisDataSource(config));
  dbPool_ = std::make_shared<DBPool>();
  MysqlDataSource ds(config);
  dbPool_->PoolInit(&ds);
 }

