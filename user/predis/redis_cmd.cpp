#include "redis_cmd.h"
#include <memory>
#include <thread>

RedisCmd::RedisCmd(): context_(NULL) {
}

RedisCmd::~RedisCmd() {
  if (context_) {
    LOG(INFO) << "redis:" << context_ << "exit";
    redisFree(context_);
    context_ = NULL;
  }
}

int RedisCmd::keepAlive() {
  return redisEnableKeepAlive(context_);
}

bool RedisCmd::setTimeout(struct timeval tv) {
  return REDIS_ERR != 
    redisSetTimeout(context_, tv);
}

bool RedisCmd::Select(std::string db) {
  redisReply* reply;
  bool r = false;
  reply = (redisReply*)redisCommand(context_,"SELECT %s", db.c_str());
  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }
  if(reply->type == REDIS_REPLY_ERROR) {
    r = false;
    freeReplyObject(reply);
    return r;
  }
  if (reply->type == REDIS_REPLY_STATUS) {
    r = true;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::Auth(std::string password) {
  redisReply* reply;
  bool r = false;
  reply = (redisReply*)redisCommand(context_,"AUTH %s", password.c_str());
  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }
  if(reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    return false;
  }
  if (reply->type == REDIS_REPLY_STATUS && reply->str && strncmp(reply->str, "OK", 2) == 0) {
    return true;
  }
  freeReplyObject(reply);
  return false;
}

bool RedisCmd::CheckValid() {
  bool rc = false;
  if (!checkContext()) {
    return rc;
  }
  redisReply* reply;
  reply = (redisReply*)redisCommand(context_,"PING");
  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }
  if(reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    return rc;
  }
  if(reply->type == REDIS_REPLY_STATUS && reply->str && strncmp(reply->str,"PONG", 4) == 0){
    rc = true;
  }
  freeReplyObject(reply);
  return rc;
}

bool RedisCmd::GetValue(const std::string key , int *value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"GET %s", key.c_str());
  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }
  if(reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    return r;
  }
  r = true;
  if (reply->type == REDIS_REPLY_INTEGER 
      && reply->len) {
    *value = reply->integer;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::GetValue(const std::string key , std::string *value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"GET %s", key.c_str());
  if (reply == NULL) {
    LOG(ERROR) << "getValue error:" << context_->err 
      << context_->errstr <<
      (int*)context_ ;
    return false;
  }
  if(reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    LOG(ERROR)<< "getValue error:" << r <<(int*)context_ ;
    return r;
  }
  if (reply->type == REDIS_REPLY_STRING 
      && reply->len) {
    value->assign(reply->str, reply->len);
    r = true;
  } 
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::GetHashValue(const std::string &hash, const std::string &key, std::string *value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"HGET %s %s", hash.c_str(), key.c_str());
  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }
  if(reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    return r;
  }
  r = true;
  if (reply->type == REDIS_REPLY_STRING
      && reply->len) {
    value->assign(reply->str, reply->len);
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::SetExValue(const std::string &key, int seconds, int value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"SETEX %s %d %d", key.c_str(), seconds, value);
    
  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }

  if (reply->type == REDIS_REPLY_STATUS 
      && reply->len 
      && strncmp(reply->str, "OK", 2) == 0) {
    r = true;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::SetExValue(const std::string &key, int seconds, const std::string &value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"SETEX %s %d %s", key.c_str(), seconds, value.c_str());

  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }

  if (reply->type == REDIS_REPLY_STATUS && reply->len && strncmp(reply->str, "OK", 2) == 0) {
    r = true;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::SetValue(const std::string &key, int value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(
      context_,"SET %s %d", key.c_str(), value);

  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }

  if (reply->type == REDIS_REPLY_STATUS && reply->len && strncmp(reply->str, "OK", 2) == 0) {
    r = true;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::SetValue(const std::string &key, const std::string &value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"SET %s %s", key.c_str(), value.c_str());

  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }

  if (reply->type == REDIS_REPLY_STATUS && reply->len && strncmp(reply->str, "OK", 2) == 0) {
    r = true;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::Lpush(const std::string &key, const std::string &value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"LPUSH %s %s", key.c_str(), value.c_str());

  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }

  if (reply->type == REDIS_REPLY_STATUS && reply->integer > 0) {
    r = true;
  }
  freeReplyObject(reply);
  return r;
}

bool RedisCmd::SetHashValue(const std::string &hash, const std::string &key, const std::string &value) {
  redisReply* reply;
  bool r = false;
  if (!checkContext()) {
    return r;
  }
  reply = (redisReply*)redisCommand(context_,"HSET %s %s %s", hash.c_str(), key.c_str(), value.c_str());

  if (reply == NULL) {
    LOG(ERROR) << "error:" << context_->err << " "
      << context_->errstr;
    return false;
  }

  if(reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    return r;
  }

  if (reply->type == REDIS_REPLY_STATUS && reply->len && reply->str) {
    if (strncmp(reply->str, "OK", 2) == 0)
      r = true;
  }
  freeReplyObject(reply);
  return r;
}

int RedisCmd::Connect(const std::string ip, int port, struct timeval tv) {
  context_ = (redisContext*)redisConnectWithTimeout(ip.c_str(), port, tv);
  if (context_ == NULL) {
    return -1;
  }
  return context_->err;
}


bool RedisCmd::checkContext() {
  if (!context_) {
    return false;
  }
  if (context_->err == REDIS_ERR_IO) {
    int rt  = redisReconnect(context_);
    LOG(INFO) << "reconnect rc:" << rt 
      <<"context:" <<  (int*)context_;
    if (rt != 0) {
      return false;
    }
  }
  return true;
}
