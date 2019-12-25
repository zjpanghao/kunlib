#ifndef INCLUDE_REDIS_CONTROL_H
#define INCLUDE_REDIS_CONTROL_H
#include <mutex>
#include <memory>
#include <thread>
#include <string>
#include <list>
#include <vector>
#include <hiredis/hiredis.h>
#include <string.h>
#include <glog/logging.h>
#include "redis_cmd.h"
#include "redis_pool.h"
class RedisControl {
  public:
    bool connect();
    RedisControl(const std::string &ip, 
        int port, 
        const std::string &db, 
        const std::string &password);

    ~RedisControl();

    bool CheckValid();

    bool SetValue(const std::string &key, const std::string &value);

    bool SetValue(const std::string &key,       int value);

    bool SetExValue(const std::string &key, int seconds, const std::string &value); 

    bool SetExValue(const std::string &key, int seconds, int value);

    bool Lpush(const std::string &key, const std::string &value);

    bool SetHashValue(const std::string &hash, const std::string &key, const std::string &value); 

    bool Select(std::string db);

    bool Auth(std::string password); 

    bool GetValue(const std::string key , std::string *value);

    bool GetValue(const std::string key , int *value);

    bool GetHashValue(const std::string &hash ,const std::string &key , std::string *value);

    time_t last_access();

    void set_last_access(time_t now);

    bool Timeout(time_t now);

  private:
    // redisContext *context_;
    RedisCmd   cmd_;
    std::string ip_;
    int port_;
    std::string password_;
    std::string db_;
    bool idle_;
    time_t last_access_;
};
#endif
