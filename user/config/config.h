#ifndef INCLUDE_CONFIG_H
#define INCLUDE_CONFIG_H
#define MAX_CONFIG_LINE 1024
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <glog/logging.h>

namespace kunyan {
typedef std::map<std::string, std::string> KeyValueMap;
class Config {
 public:
  Config(const std::string &fileName); 

  std::string get(const std::string &segment, 
                  const std::string &key) const;

 private:
  void processLine(const char *line); 
  std::string trim(const std::string &buf);
  KeyValueMap *keyMap;
  std::string segmentName;
  std::map<std::string, KeyValueMap*> configMap;
};
}
#endif



