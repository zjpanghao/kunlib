#ifndef INCLUDE_URL_MAP_H
#define INCLUDE_URL_MAP_H
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "json/json.h"

struct HttpControl {
  std::string url;
  int (*jsonCb)(const Json::Value &root,
     Json::Value &result); 
  void *arg;
};

class UrlMap {
 public:
  virtual std::vector<HttpControl> getMapping() = 0;
  virtual ~UrlMap() = default;  
};

#endif
