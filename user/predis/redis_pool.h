#ifndef INCLUDE_REDIS_POOL_H
#define INCLUDE_REDIS_POOL_H
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
#include "datasource/redisDataSource.h"
#include <condition_variable>
namespace kunyan{
  class Config;
  class RedisDataSource;
}
class RedisControl;
#if 0
    class RedisControl {
     public:
      bool connect() {
        struct timeval ta = {2, 0};
        int rc = 0;
        if ((rc = cmd_.Connect(ip_.c_str(), port_, ta))!= 0) {
          LOG(ERROR) << "redis connn error " << rc << " " << "ip:" << ip_ << "port:" <<  port_;
          return false;
        }

        if (password_ != "" && !Auth(password_)) {
          LOG(ERROR) << "error passwd" << password_;
        }

        if (!Select(db_)) {
          LOG(ERROR) << "error select" << db_;
        }
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200000;
        cmd_.setTimeout(tv);
        return true;
      }

      RedisControl(std::string ip, int port, std::string db, std::string password) : ip_(ip), port_(port), db_(db), 
      password_(password),
      idle_(true) {
      }
       
      ~RedisControl() {
      }

      bool idle() {
        return idle_;
      }
      
      void set_idle(bool idle) {
        idle_ = idle;
      }

      bool CheckValid() {
        return cmd_.CheckValid();
      }
      
      bool SetValue(const std::string &key, const std::string &value) {
        return cmd_.SetValue(key, value);
      }

      bool SetValue(const std::string &key,       int value) {
        return cmd_.SetValue(key, value);
      }

      bool SetExValue(const std::string &key, int seconds, const std::string &value) {
        return cmd_.SetExValue(key, seconds, value);
      }

      bool SetExValue(const std::string &key, int seconds, int value) {
        return cmd_.SetExValue(key, seconds, value);
      }

      bool Lpush(const std::string &key, const std::string &value) {
        return cmd_.Lpush(key, value);
      }

      bool SetHashValue(const std::string &hash, const std::string &key, const std::string &value) {
        return cmd_.SetHashValue(hash, key, value);
      }
      
      bool Select(std::string db) {
        return cmd_.Select(db);
      }
      bool Auth(std::string password) {
        return cmd_.Auth(password);
      }

      bool GetValue(const std::string key , std::string *value) {
        return cmd_.GetValue(key, value);
      }

      bool GetValue(const std::string key , int *value) {
        return cmd_.GetValue(key, value);
      }

      bool GetHashValue(const std::string &hash ,const std::string &key , std::string *value) {
        return cmd_.GetHashValue(hash, key, value);
      }

      time_t lastAccess() {
        return lastAccess_;
      }
      
      void setLastAccess(time_t now) {
        lastAccess_ = now;
      }

      bool Timeout(time_t now) {
        return now > lastAccess_ && now - lastAccess_ > 60;
      }

     private:
      RedisCmd   cmd_;
      std::string ip_;
      int port_;
      std::string password_;
      std::string db_;
      bool idle_;
      time_t lastAccess_;
};
#endif

    class RedisPool {
     public:
      RedisPool(const RedisDataSource &dataSource);

      std::shared_ptr<RedisControl> 
        GetControl();
      std::shared_ptr<RedisControl> 
        GetControlInner();

      void ReturnControl(std::shared_ptr<RedisControl> control);
      
      void Reep();

      void ReepThd();

      void Init();

      int size() const ;

      int activeSize() const;

     private:
      mutable std::mutex  lock_;
      std::list<std::shared_ptr<RedisControl> > contextPool_;
      int initialSize_;
      int maxSize_;
      int activeSize_;
      std::string ip_;
      int port_;
      std::string db_;
      std::string password_;
      int timeout_{100000};
      int reepInx_{0};
      //std::condition_variable notEmpty_;
    };

class RedisControlGuard {
  public:
   RedisControlGuard(RedisPool *pool);

   std::shared_ptr<RedisControl> 
     GetControl();
   ~RedisControlGuard();

  private:
   std::shared_ptr<RedisControl> redisControl_;
   RedisPool *redisPool_;
};

#endif
