#ifndef INCLUDE_REDIS_CMD_H
#define INCLUDE_REDIS_CMD_H
#include "glog/logging.h"
#include "hiredis/hiredis.h"
#include <string>

class RedisCmd  {
  public:
    RedisCmd();

    ~RedisCmd();

    int keepAlive();

    bool setTimeout(struct timeval tv);

    bool Select(std::string db);

    bool Auth(std::string password);

    bool CheckValid();

    bool GetValue(const std::string key , 
        int *value);

    bool GetValue(const std::string key,
        std::string *value);

    bool GetHashValue(const std::string &hash, 
        const std::string &key, 
        std::string *value);

    bool SetExValue(const std::string &key, int seconds, int value);

    bool SetExValue(const std::string &key, int seconds, const std::string &value);

    bool SetValue(const std::string &key,
        int value) ;

    bool SetValue(const std::string &key, const std::string &value);

    bool Lpush(const std::string &key, const std::string &value);

    bool SetHashValue(const std::string &hash, const std::string &key, const std::string &value);

    int Connect(const std::string ip, int port, struct timeval tv);

  private:

    bool checkContext();

    redisContext *context_;
};
#endif
