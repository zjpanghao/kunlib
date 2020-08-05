#include "redis_pool.h"
#include <sstream>
#include <config/config.h>
#include "redis_control.h"
int RedisPool::size() const {
  std::lock_guard<std::mutex> 
    lock(lock_);
  return contextPool_.size();
}

int RedisPool::activeSize() const {
  std::lock_guard<std::mutex> 
    lock(lock_);
  return activeSize_;
}

void RedisPool::Init() {
  for (int i = 0; i < initialSize_; i++) { 
    std::shared_ptr<RedisControl> control(new RedisControl(ip_, port_, 
          db_, password_, timeout_));
   if (!control->connect()) {
     return;
   }
   if (control->CheckValid()) {
     contextPool_.push_back(control);
   }
  }
  std::thread reap_thd(&RedisPool::ReepThd, this);
  reap_thd.detach();
}

void RedisPool::ReepThd() {
  while (1) {
    Reep();
    ::sleep(60);
  }
}
void RedisPool::Reep() {
  std::lock_guard<std::mutex> lock(lock_);
  int nums = contextPool_.size()  - initialSize_;
  if (nums <= 0) {
    return;
  }
  int inx = reepInx_ % contextPool_.size();
  reepInx_ = (reepInx_ + 1) % contextPool_.size();
  auto it = contextPool_.begin();
  time_t now = time(NULL);
  while (inx-- > 0 && it != contextPool_.end()) {
      it++;
      continue;
  }
  if (it != contextPool_.end()) {
    auto &control = *it;
    if (!control->CheckValid() 
        || control->Timeout(now)) {
      it = contextPool_.erase(it);
    } 
  }

}

void RedisPool::ReturnControl(std::shared_ptr<RedisControl> control) {
  control->set_last_access(time(NULL));
  std::lock_guard<std::mutex> lock(lock_);
  contextPool_.push_front(control);
  activeSize_--;
}

std::shared_ptr<RedisControl> 
RedisPool::GetControlInner() {
  while (!contextPool_.empty()) {
    auto control = 
      contextPool_.front();
    contextPool_.pop_front();
    if (control->CheckValid()) {
      activeSize_++;
      return control;
    }
  } 

  if (activeSize_ < maxSize_) {
    std::shared_ptr<RedisControl> control(new RedisControl(ip_, port_, db_, password_,
          timeout_));
    if (control->connect() && 
        control->CheckValid()) {
      activeSize_++;
      return control;
    }
  }
  return nullptr;
}

std::shared_ptr<RedisControl>  
RedisPool::GetControl() {
  std::shared_ptr<RedisControl> control(nullptr);
  std::unique_lock<std::mutex> 
    lock(lock_);
  control= GetControlInner();

  return control;
}

RedisPool::RedisPool(const RedisDataSource &dataSource) 
  : initialSize_(dataSource.initialSize()), 
  maxSize_(dataSource.maxSize()),
  activeSize_(0),
  ip_(dataSource.ip()),
  port_(dataSource.port()),
  db_(dataSource.db()),
  password_(dataSource.password()),
  timeout_(dataSource.timeout()){
  }

RedisControlGuard::RedisControlGuard(RedisPool *pool)
  :
    redisPool_(pool),
    redisControl_(NULL) {

    }

std::shared_ptr<RedisControl> 
RedisControlGuard::GetControl() {
  if (redisPool_ == NULL) {
    return NULL;
  }
  redisControl_ = 
    redisPool_->GetControl();
  return redisControl_;
}

RedisControlGuard::~RedisControlGuard() {
  if (redisPool_ && redisControl_ != NULL) {
    redisPool_->ReturnControl(redisControl_);
  } 
}

