#ifndef INCLUDE_DATASOURCE_H
#define INCLUDE_DATASOURCE_H
#include "config/config.h"
#include <sstream>
class DataSource {
 public:
  DataSource(const std::string &ip,
             const int  port,
             const std::string &db,
             const std::string &user,
             const std::string &password)
    : ip_(ip), port_(port), db_(db), user_(user), password_(password) {

    }
  DataSource(const kunyan::Config &config) {
    // mongo
    std::stringstream ss;
    ss.clear();
    ss.str("");
    ss << config.get("mysql", "ip");
    ss >> ip_;

    ss.clear();
    ss.str("");
    ss << config.get("mysql", "port");
    std::string portStr(ss.str());
    ss >> port_;

    ss.clear();
    ss.str("");
    ss << config.get("mysql", "db");
    ss >> db_;

    ss.clear();
    ss.str("");
    ss << config.get("mysql", "user");
    ss >> user_;

    ss.clear();
    ss.str("");
    ss << config.get("mysql", "pass");
    ss >> password_;
  }

  std::string ip() const {
    return ip_;
  }

  int port() const {
    return port_;
  }
  
  std::string db() const {
    return db_;
  }

  std::string user() const {
    return user_;
  }
  
  std::string password() const {
    return password_;
  }

 private:
   std::string ip_;
   int port_;
   std::string db_;
   std::string user_;
   std::string password_;
};
#endif
