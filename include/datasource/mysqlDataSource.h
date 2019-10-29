#ifndef INCLUDE_MYSQL_DATASOURCE_H
#define INCLUDE_MYSQL_DATASOURCE_H
#include "config/config.h"
#include "dataSource.h"
#include <sstream>
class MysqlDataSource : public DataSource{
 public:
  MysqlDataSource(const std::string &ip,
             const int  port,
             const std::string &db,
             const std::string &user,
             const std::string &password)
    : DataSource(ip, port, db, user, password) {

  }

  MysqlDataSource(const kunyan::Config &config) : DataSource("mysql", config) {
  }
  MysqlDataSource(const std::string &item, const kunyan::Config &config) : DataSource(item, config) {
  }
};
#endif

