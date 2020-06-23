#ifndef INCLUDE_MYSQL_DATASOURCE_H
#define INCLUDE_MYSQL_DATASOURCE_H
#include "config/config.h"
#include "dataSource.h"
#include <sstream>
class MysqlDataSource : public DataSource{
 public:
  MysqlDataSource(const kunyan::Config &config)
    : MysqlDataSource(config, "mysql") {
  }

  MysqlDataSource(const kunyan::Config &config,
      const std::string &tag) : DataSource(tag, config) {
  }
  MysqlDataSource(const std::string &item, const kunyan::Config &config) : DataSource(item, config) {
  }
};
#endif

