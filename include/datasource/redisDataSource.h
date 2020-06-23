#ifndef INCLUDE_REDIS_DATASOURCE_H
#define INCLUDE_REDIS_DATASOURCE_H
#include "config/config.h"
#include "dataSource.h"
#include <sstream>
class RedisDataSource : public DataSource{
 public:
  RedisDataSource(const kunyan::Config &config)
    :RedisDataSource(config, "redis") {
    }
  RedisDataSource(const kunyan::Config &config,
      const std::string &tag) : DataSource(tag, config) {
    std::string timeout = 
      config.get(tag, "timeout");
    if (!timeout.empty()) {
      std::stringstream ss;
      ss << timeout;
      ss >> timeout_;
    }
  }

  int timeout() const {
    return timeout_;
  }

 private:
  int timeout_{100000};


};
#endif

