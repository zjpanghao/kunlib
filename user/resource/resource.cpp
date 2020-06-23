#include "resource.h"
#include "config/config.h"
#include "db/dbpool.h"
#include "predis/redis_pool.h"
Resource &Resource::getResource() {
  static Resource rs;
  return rs;
}

Resource::Resource(const kunyan::Config &config,
    const std::string &tag,
    ResourceType type) {
  switch(type) {
    case ResourceType::MYSQL:
      dbPool_ = std::make_shared<DBPool>();
      {
        MysqlDataSource ds(config, tag);
        dbPool_->PoolInit(&ds);
      }
      break;
    case ResourceType::REDIS:
      redisPool_ = 
        std::make_shared<RedisPool>(
            RedisDataSource(config, tag));
      redisPool_->Init();
      break;
  }
  
}

void Resource::init(const kunyan::Config &config) {
  redisPool_ = 
    std::make_shared<RedisPool>(RedisDataSource(config));
  redisPool_->Init();
  dbPool_ = std::make_shared<DBPool>();
  MysqlDataSource ds(config);
  dbPool_->PoolInit(&ds);
 }

