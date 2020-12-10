#ifndef INCLUDE_GENERAL_CONTROL_H
#define  INCLUDE_GENERAL_CONTROL_H
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "json/json.h"
#include <functional>

namespace kunyan {
  class Config;
}

struct HttpControl {
  std::string url;
  std::function<int(const pson::Json::Value &root,
      pson::Json::Value &result)> jsonCb;
  //int (*jsonCb)(const Json::Value &root,
   //  Json::Value &result); 
  int (*bodyCb)(const pson::Json::Value &root,
     std::string &result); 
  void *arg;
};

class GeneralControl{
 public:
  virtual std::vector<HttpControl> getMapping() = 0;
   virtual ~GeneralControl() = default;
   virtual int init(const kunyan::Config &config);
};
#endif
