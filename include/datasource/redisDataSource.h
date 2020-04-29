#ifndef INCLUDE_REDIS_DATASOURCE_H
#define INCLUDE_REDIS_DATASOURCE_H
#include "config/config.h"
#include "dataSource.h"
#include <sstream>
class RedisDataSource : public DataSource{
 public:
  RedisDataSource(const kunyan::Config &config) : DataSource("redis", config) {
    std::string timeout = 
      config.get("redis", "timeout");
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

