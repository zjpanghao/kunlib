#ifndef INCLUDE_JSON_UTIL_H
#define INCLUDE_JSON_UTIL_H
#include <glog/logging.h>
#include <json/json.h>
class JsonUtil {
  public:
    template <typename T, class = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
    static void getJsonValue(const Json::Value &value, const std::string &key, T &t) {
      if (value.isNull() 
        || !value.isMember(key) 
        || !value[key].isString()) {
        return;
      }
      std::stringstream ss;
      ss <<value[key].asString();
      ss >> t;
    }

    template <typename T, class = typename std::enable_if<std::is_same<T, std::string>::value>::type>
    static void getJsonStringValue(const Json::Value &value, const std::string &key, T &t) {
      if (value.isNull() 
        || !value.isMember(key) 
        || !value[key].isString()) {
        return;
      }
      t = value[key].asString();
    }

    static void getJsonInt(const Json::Value
        &value, const std::string &key, int &t) {
      if (value.isNull() 
        || !value.isMember(key) 
        || !value[key].isInt()) {
        return;
      }
      t = value[key].asInt();
    }
};

#endif
