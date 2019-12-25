#include "redis_pool.h"
#include <sstream>
#include <config/config.h>
#include "redis_control.h"
int RedisPool::size() const {
  std::lock_guard<std::mutex> 
    lock(lock_);
  return context_pool_.size();
}
void RedisPool::Init() {
  for (int i = 0; i < normal_size_; i++) { 
    std::shared_ptr<RedisControl> control(new RedisControl(ip_, port_, db_, password_));
   if (!control->connect()) {
     return;
   }
   if (control->CheckValid()) {
     context_pool_.push_back(control);
   }
  }
  std::thread reap_thd(&RedisPool::ReepThd, this);
  reap_thd.detach();
}

void RedisPool::ReepThd() {
  while (1) {
    Reep();
    ::sleep(30);
  }
}
void RedisPool::Reep() {
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

void RedisPool::ReturnControl(std::shared_ptr<RedisControl> control) {
  control->set_last_access(time(NULL));
  std::lock_guard<std::mutex> lock(lock_);
  context_pool_.push_front(control);
  active_num_--;
}

std::shared_ptr<RedisControl> 
RedisPool::GetControl() {
  std::lock_guard<std::mutex> lock(lock_);
  while (!context_pool_.empty()) {
    auto control = 
      context_pool_.front();
    context_pool_.pop_front();
    if (control->CheckValid()) {
      active_num_++;
      return control;
    }
  } 

  if (active_num_ < max_size_) {
    std::shared_ptr<RedisControl> control(new RedisControl(ip_, port_, db_, password_));
    if (control->connect() && 
        control->CheckValid()) {
      active_num_++;
      return control;
    }
  }
  return NULL;
}

RedisPool::RedisPool(const RedisDataSource &dataSource) 
  : normal_size_(dataSource.minSize()), 
  max_size_(dataSource.maxSize()),
  active_num_(0),
  ip_(dataSource.ip()),
  port_(dataSource.port()),
  db_(dataSource.db()),
  password_(dataSource.password()) {
  }

RedisControlGuard::RedisControlGuard(RedisPool *pool)
  :
    redis_pool_(pool),
    redisControl_(NULL) {

}

std::shared_ptr<RedisControl> 
RedisControlGuard::GetControl() {
  if (redis_pool_ == NULL) {
    return NULL;
  }
  redisControl_ = 
    redis_pool_->GetControl();
  return redisControl_;
}

RedisControlGuard::~RedisControlGuard() {
  if (redis_pool_ && redisControl_ != NULL) {
    redis_pool_->ReturnControl(redisControl_);
  } 
}

