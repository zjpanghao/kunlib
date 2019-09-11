#ifndef INCLUDE_REDIS_DATASOURCE_H
#define INCLUDE_REDIS_DATASOURCE_H
#include "config/config.h"
#include "dataSource.h"
#include <sstream>
class RedisDataSource : public DataSource{
 public:
  RedisDataSource(const std::string &ip,
             const int  port,
             const std::string &db,
             const std::string &user,
             const std::string &password)
    : DataSource(ip, port, db, user, password) {

  }

  RedisDataSource(const kunyan::Config &config) : DataSource("redis", config) {
    std::stringstream ss;
    ss << config.get("redis", "minSize");
    if (ss.str() != "") {
      ss >> minSize_;
    }

    ss.str("");
    ss.clear();
    ss << config.get("redis", "maxSize");
    if (ss.str() != "") {
      ss >> maxSize_;
    }
  }
  int minSize() const { return minSize_;}
  int maxSize() const { return maxSize_;}
  

 private:
  int minSize_{0};
  int maxSize_{10};
};
#endif

