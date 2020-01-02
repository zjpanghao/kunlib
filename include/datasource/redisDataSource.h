#ifndef INCLUDE_REDIS_DATASOURCE_H
#define INCLUDE_REDIS_DATASOURCE_H
#include "config/config.h"
#include "dataSource.h"
#include <sstream>
class RedisDataSource : public DataSource{
 public:
  RedisDataSource(const kunyan::Config &config) : DataSource("redis", config) {
  }

};
#endif

