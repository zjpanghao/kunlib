#ifndef INCLUDE_DATASOURCE_H
#define INCLUDE_DATASOURCE_H
#include "config/config.h"
#include <sstream>
class DataSource {
 public:
  DataSource(const std::string &item, const kunyan::Config &config) {
    std::stringstream ss;
    ss.clear();
    ss.str("");
    ss << config.get(item, "ip");
    ss >> ip_;

    ss.clear();
    ss.str("");
    ss << config.get(item, "port");
    std::string portStr(ss.str());
    ss >> port_;

    ss.clear();
    ss.str("");
    ss << config.get(item, "db");
    ss >> db_;

    ss.clear();
    ss.str("");
    ss << config.get(item, "user");
    ss >> user_;

    ss.clear();
    ss.str("");
    ss << config.get(item, "pass");
    ss >> password_;

    ss.clear();
    ss.str("");

    ss << config.get(item, "initialSize");
    if (ss.str() != "") {
      ss >> initialSize_;
    }

    ss.str("");
    ss.clear();
    ss << config.get(item, "maxSize");
    if (ss.str() != "") {
      ss >> maxSize_;
    }

    ss.str("");
    ss.clear();
    ss << config.get(item, "reapSec");
    if (ss.str() != "") {
      ss >> reapSec_;
    }
  }

  virtual ~DataSource() = default;

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

  int initialSize() const { return initialSize_;}
  int maxSize() const { return maxSize_;}

  int reapSec() const {return reapSec_;}

 private:
   std::string ip_;
   int port_;
   std::string db_;
   std::string user_;
   std::string password_;
   int initialSize_{0};
   int maxSize_{10};
   int reapSec_{60};
};
#endif
