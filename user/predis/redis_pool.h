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
namespace kunyan{
  class Config;
  class RedisDataSource;
}

    class RedisControl {
     public:
      RedisControl(std::string ip, int port, std::string db, std::string password) : idle_(true) {
        struct timeval ta = {2, 0};
        int rc = 0;
        if ((rc = cmd_.Connect(ip.c_str(), port, ta))!= 0) {
          LOG(ERROR) << "redis connn error " << rc << " " << "ip:" << ip << "port:" <<  port;
          return;
        }

        if (password != "" && !Auth(password)) {
          LOG(ERROR) << "error passwd" << password;
        }

        if (!Select(db)) {
          LOG(ERROR) << "error select" << db;
        }
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

      bool SetExValue(const std::string &key, int seconds, const std::string &value) {
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

      bool GetHashValue(const std::string &hash ,const std::string &key , std::string *value) {
        return cmd_.GetHashValue(hash, key, value);
      }

      time_t last_access() {
        return last_access_;
      }
      
      void set_last_access(time_t now) {
        last_access_ = now;
      }

      bool Timeout(time_t now) {
        return now > last_access_ && now - last_access_ > 60;
      }
     private:
      // redisContext *context_;
      RedisCmd   cmd_;
      bool idle_;
      time_t last_access_;
};

    class RedisPool {
     public:
      RedisPool(const std::string &ip, 
                int port, 
                int normal_size, 
                int max_size,
                std::string db,
                std::string password) 
          : normal_size_(normal_size), 
            max_size_(max_size),
            active_num_(0),
            ip_(ip),
            port_(port),
            db_(db),
            password_(password) {
        Init();
      }

      RedisPool(const RedisDataSource &dataSource) 
          : normal_size_(dataSource.minSize()), 
            max_size_(dataSource.maxSize()),
            active_num_(0),
            ip_(dataSource.ip()),
            port_(dataSource.port()),
            db_(dataSource.db()),
            password_(dataSource.password()) {


      }

      std::shared_ptr<RedisControl> GetControl() {
        std::lock_guard<std::mutex> lock(lock_);
        while (!context_pool_.empty()) {
          auto control = std::move(context_pool_.front());
          context_pool_.pop_front();
          if (control->CheckValid()) {
            active_num_++;
            return control;
          }
        } 

        if (active_num_ < max_size_) {
          std::shared_ptr<RedisControl> control(new RedisControl(ip_, port_, db_, password_));
          if (control->CheckValid()) {
            context_pool_.push_back(control);
            active_num_++;
            return control;
          }
        }
        return NULL;
      }

      void ReturnControl(std::shared_ptr<RedisControl> control) {
        control->set_last_access(time(NULL));
        std::lock_guard<std::mutex> lock(lock_);
        context_pool_.push_back(control);
        active_num_--;
      }
      
      void Reep() {
        std::lock_guard<std::mutex> lock(lock_);
        int nums = context_pool_.size() - normal_size_;
        auto it = context_pool_.begin(); 
        time_t now = time(NULL);
        while (nums > 0 && it != context_pool_.end()) {
          auto &control = *it;
          if (!control->CheckValid() || control->Timeout(now)) {
            it = context_pool_.erase(it);
            nums--;
          } else {
            it++;
          }
        }
      }

      void ReepThd() {
        while (1) {
          Reep();
          ::sleep(30);
        }
      }

      void Init() {
        for (int i = 0; i < normal_size_; i++) { 
          std::shared_ptr<RedisControl> control(new RedisControl(ip_, port_, db_, password_));
          if (control->CheckValid()) 
            context_pool_.push_back(control);
        }
        std::thread reap_thd(&RedisPool::ReepThd, this);
        reap_thd.detach();
      }

     private:
      std::mutex  lock_;
      std::list<std::shared_ptr<RedisControl> > context_pool_;
      int normal_size_;
      int max_size_;
      int active_num_;
      std::string ip_;
      int port_;
      std::string db_;
      std::string password_;
    };

class RedisControlGuard {
  public:
   RedisControlGuard(RedisPool *pool)
       :
     redis_pool_(pool),
     redisControl_(NULL) {
     
   }

   std::shared_ptr<RedisControl> GetControl() {
     if (redis_pool_ == NULL) {
       return NULL;
     }
     redisControl_ = redis_pool_->GetControl();
     return redisControl_;
   }

   ~RedisControlGuard() {
     if (redis_pool_ && redisControl_ != NULL) {
       redis_pool_->ReturnControl(redisControl_);
     } 
   }

  private:
   std::shared_ptr<RedisControl> redisControl_;
   RedisPool *redis_pool_;
};

void initRedisPool(const kunyan::Config &config); 
RedisPool* getRedisPool();
#endif
