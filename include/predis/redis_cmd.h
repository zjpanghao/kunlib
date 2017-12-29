#ifndef INCLUDE_REDIS_CMD_H
#define INCLUDE_REDIS_CMD_H
class RedisCmd  {
  public:
    RedisCmd(): context_(NULL) {
    }

    ~RedisCmd() {
      if (context_) {
        redisFree(context_);
        context_ = NULL;
      }
    }

    bool Select(std::string db) {
      redisReply* reply;
      bool r = false;
      reply = (redisReply*)redisCommand(context_,"SELECT %s", db.c_str());
      if (reply == NULL) {
        setReplyFatalError();
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

    bool Auth(std::string password) {
      redisReply* reply;
      bool r = false;
      reply = (redisReply*)redisCommand(context_,"AUTH %s", password.c_str());
      if (reply == NULL) {
        setReplyFatalError();
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

    bool CheckValid() {
      bool rc = false;
      if (!checkContext()) {
        return rc;
      }
      redisReply* reply;
      reply = (redisReply*)redisCommand(context_,"PING");
      if (reply == NULL) {
        setReplyFatalError();
        return false;
      }
      if(reply->type == REDIS_REPLY_ERROR) {
        freeReplyObject(reply);
        return rc;
      }
      LOG(INFO) << "checkvalid" << reply->type;
      if(reply->type == REDIS_REPLY_STATUS && reply->str && strncmp(reply->str,"PONG", 3) == 0){
        rc = true;
      }
      freeReplyObject(reply);
      return rc;
    }


    bool GetValue(const std::string key , std::string *value) {
      redisReply* reply;
      bool r = false;
      if (!checkContext()) {
        return r;
      }
      reply = (redisReply*)redisCommand(context_,"GET %s", key.c_str());
      if (reply == NULL) {
        setReplyFatalError();
        return false;
      }
      if(reply->type == REDIS_REPLY_ERROR) {
        freeReplyObject(reply);
        return r;
      }
      r = true;
      LOG(INFO) << "Getvalue " << reply->type;
      if (reply->type == REDIS_REPLY_STRING && reply->len)
        value->assign(reply->str, reply->len);
      freeReplyObject(reply);
      return r;
    }

    bool GetHashValue(const std::string &hash, const std::string &key, std::string *value) {
      redisReply* reply;
      bool r = false;
      if (!checkContext()) {
        return r;
      }
      reply = (redisReply*)redisCommand(context_,"HGET %s %s", hash.c_str(), key.c_str());
      if (reply == NULL) {
        setReplyFatalError();
        return false;
      }
      if(reply->type == REDIS_REPLY_ERROR) {
        freeReplyObject(reply);
        return r;
      }
      r = true;
      if (reply->type == REDIS_REPLY_STRING && reply->len) {
        value->assign(reply->str, reply->len);
      }
      freeReplyObject(reply);
      return r;
    }

    bool SetValue(const std::string &key, const std::string &value) {
      redisReply* reply;
      bool r = false;
      if (!checkContext()) {
        return r;
      }
      reply = (redisReply*)redisCommand(context_,"SET %s %s", key.c_str(), value.c_str());

      if (reply == NULL) {
        setReplyFatalError();
        return false;
      }

      if (reply->type == REDIS_REPLY_STATUS && reply->len && strncmp(reply->str, "OK", 2) == 0) {
        r = true;
      }
      freeReplyObject(reply);
      return r;
    }

    void  setReplyFatalError() {
      fatal_error_ = true;
    }

    bool SetHashValue(const std::string &hash, const std::string &key, const std::string &value) {
      redisReply* reply;
      bool r = false;
      if (!checkContext()) {
        return r;
      }
      reply = (redisReply*)redisCommand(context_,"HSET %s %s %s", hash.c_str(), key.c_str(), value.c_str());

      if (reply == NULL) {
        setReplyFatalError();
        return false;
      }

      if(reply->type == REDIS_REPLY_ERROR) {
        freeReplyObject(reply);
        return r;
      }

      if (reply->type == REDIS_REPLY_STATUS && reply->len && reply->str) {
        //      LOG(INFO) << reply->str;
        if (strncmp(reply->str, "OK", 2) == 0)
          r = true;
      }
      freeReplyObject(reply);
      return r;
    }

    int Connect(const std::string ip, int port, struct timeval tv) {
      context_ = (redisContext*)redisConnectWithTimeout(ip.c_str(), port, tv);
      if (context_ == NULL) {
        return -1;
      }
      return context_->err;
    }

  private:

    bool checkContext() {
      if (!context_ || context_->err) {
        return false;
      }
      return true;
    }
    bool fatal_error_{false};
    redisContext *context_;
};
#endif
