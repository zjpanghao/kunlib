#ifndef INCLUDE_JSON_UTIL_H
#define INCLUDE_JSON_UTIL_H
#include <glog/logging.h>
#include <json/json.h>
class JsonUtil {
  public:
    template <typename T, class = typename std::enable_if<std::is_same<T, std::string>::value>::type>
    static void getJsonString(const pson::Json::Value &value, const std::string &key, T &t) {
      if (value.isNull() 
        || !value.isMember(key) 
        || !value[key].isString()) {
        return;
      }
      t = value[key].asString();
    }

    static void getJsonInt(const pson::Json::Value
        &value, const std::string &key, int &t) {
      if (value.isNull() 
        || !value.isMember(key) 
        || !value[key].isInt()) {
        return;
      }
      t = value[key].asInt();
    }
    
    static void getJsonDouble(const pson::Json::Value
        &value, const std::string &key, double &t) {
      if (value.isNull() 
        || !value.isMember(key) 
        || !value[key].isDouble()) {
        return;
      }
      t = value[key].asDouble();
    }
};

#endif
