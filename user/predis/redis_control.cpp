#include "redis_control.h"
RedisControl::~RedisControl() {
}

time_t RedisControl::last_access() {
  return last_access_;
}

void RedisControl::set_last_access(time_t now) {
  last_access_ = now;
}

bool RedisControl::Timeout(time_t now) {
  return now > last_access_ && now - last_access_ > 60;
}

bool RedisControl::GetHashValue(const std::string &hash ,const std::string &key , std::string *value) {
  return cmd_.GetHashValue(hash, key, value);
}

bool RedisControl::Select(std::string db) {
  return cmd_.Select(db);
}
bool RedisControl::Auth(std::string password) {
  return cmd_.Auth(password);
}

bool RedisControl::GetValue(const std::string key , std::string *value) {
  return cmd_.GetValue(key, value);
}
bool RedisControl::GetValue(const std::string key , int *value) {
  return cmd_.GetValue(key, value);
}

bool RedisControl::SetHashValue(const std::string &hash, const std::string &key, const std::string &value) {
  return cmd_.SetHashValue(hash, key, value);
}
bool RedisControl::SetExValue(const std::string &key, int seconds, int value) {
  return cmd_.SetExValue(key, seconds, value);
}

bool RedisControl::Lpush(const std::string &key, const std::string &value) {
  return cmd_.Lpush(key, value);
}

bool RedisControl::SetValue(const std::string &key,       int value) {
  return cmd_.SetValue(key, value);
}

bool RedisControl::SetExValue(const std::string &key, int seconds, const std::string &value) {
  return cmd_.SetExValue(key, seconds, value);
}

bool RedisControl::SetValue(const std::string &key, const std::string &value) {
  return cmd_.SetValue(key, value);
}
bool RedisControl::CheckValid() {
  return cmd_.CheckValid();
}
RedisControl::RedisControl(
    const std::string &ip, 
    int port, 
    const std::string &db, 
    const std::string &password,
    int timeout) 
  : ip_(ip), 
  port_(port), 
  db_(db), 
  password_(password),
  timeout_(timeout),
  idle_(true) {
  }

bool RedisControl::connect() {
  struct timeval ta = {2, 0};
  int rc = 0;
  if ((rc = cmd_.Connect(ip_.c_str(), port_, ta))!= 0) {
    LOG(ERROR) << "redis connn error:" << rc << " ip:" << ip_ << "port:" <<  port_;
    return false;
  }

  if (password_ != "" && !Auth(password_)) {
    LOG(ERROR) << "connect error passwd:" << password_;
    return false;
  }

  if (!Select(db_)) {
    LOG(ERROR) << "connect error select:" << db_;
    return false;
  }
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = timeout_;
  cmd_.setTimeout(tv);
  cmd_.keepAlive();
  LOG(INFO) << "connect ok:";
  return true;
}
