#include "config.h"
namespace kunyan {
Config::Config(const std::string &fileName) {
  FILE *fp = fopen(fileName.c_str(), "r");
  if (!fp) {
    return;
  }
  int c = 0;
  char line[MAX_CONFIG_LINE];
  int index = 0;
  while ((c=fgetc(fp)) != EOF) {
    if (index >= MAX_CONFIG_LINE) {
      return;
    }
    if (c == '\n') {
      line[index] = 0;
      processLine(line);
      index = 0;
    } else {
      line[index++] = c;
    }
  }
  fclose(fp);
  if (index > 0) {
    processLine(line);
  }
}

void Config::processLine(const char *line) {
  const char *p = strstr(line, "=");
  if (p) {
    if (p == line) {
      return;
    }
    if (*(p + 1) == '\0') {
      return;
    }
    std::string key(line, p - line);
    key = trim(key.c_str());
    if (key.length() == 0) {
      return;
    }
    std::string value(p + 1);

    value = trim(value.c_str());
    if (value.length() == 0) {
      return;
    }
    if (keyMap != NULL) {
      keyMap->insert(std::make_pair(key, value));
    }
  } else {
    const char *q = strstr(line, "[");
    const char *s = strstr(line, "]");
    if (s == NULL || q == NULL) {
      return;
    }
    if (s <= q || s - q >= MAX_CONFIG_LINE) {
      return;
    }
    segmentName.assign(q + 1, s - q - 1);
    segmentName = trim(segmentName.c_str());
    if (segmentName.length() > 0) {
      keyMap = new KeyValueMap();
      configMap[segmentName] = keyMap;
    }
  }
}

std::string Config::get(const std::string &segment, 
    const std::string &key) {
  auto it = configMap.find(segment);
  if (it == configMap.end()) {
    return "";
  }
  KeyValueMap *keyValueMap = it->second;
  auto kit = keyValueMap->find(key);
  if (kit == keyValueMap->end()) {
    return "";
  }
  return kit->second;
}

std::string Config::trim(const std::string &buf) {
  int len = buf.length();
  if (len == 0) {
    return "";
  }
  int i = 0;
  for (; i < len && buf[i] == ' '; i++) {
  }
  int j;
  for (j = len -1; j > i && buf[j] == ' '; j--) {

  }
  if (j < i) {
    return "";
  } 
  return buf.substr(i, j - i + 1);
}

} // namespace kunyan
