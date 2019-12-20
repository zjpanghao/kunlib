#ifndef INCLUDE_EV_DRV_H
#define INCLUDE_EV_DRV_H
#include <vector>
#include <memory>
#include "event2/http.h"
#include "evhtp/evhtp.h"

namespace Json {
 class Value;
};

class GeneralControl;

struct HttpControl;
struct evhtp_request;
struct evhttp_request;
union  EvHttpRequest{
  struct evhttp_request *http;
  struct evhtp_request *htp;
};
struct evbuffer;
namespace kunyan {
  class Config;
};

class EvDrv {
 public:
   void startServer(
       const kunyan::Config &confg,
       const std::vector<std::shared_ptr<GeneralControl>> &controls);
   virtual ~EvDrv() = default;
   virtual int evReqMethod(EvHttpRequest*req) = 0;
   virtual evbuffer *getInputBuffer(EvHttpRequest *req) = 0;
   virtual void sendResponse(EvHttpRequest *req,
       const Json::Value &value) = 0;
   virtual void sendResponseBody(EvHttpRequest *req,
       const std::string &value) = 0;
   virtual void start(
       const std::string &ip,
       int port, 
       int nThread, 
       const std::vector<std::shared_ptr<GeneralControl>> &controls) = 0;
  static std::string getBufferStr(struct evbuffer *buf);
  static bool getBufferJson(struct evbuffer *buf, Json::Value &root);

  virtual bool getQueryJson(EvHttpRequest*req, Json::Value &root) = 0;

 void process(EvHttpRequest *req, 
     const HttpControl *control);
};

#endif
