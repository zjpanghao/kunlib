#include "evDrv.h"
#include "signal.h"
#include "glog/logging.h"
#include "event2/http_struct.h"
#include "generalControl.h"
#include "config/config.h"

void EvDrv::startServer(const kunyan::Config &config,
   const std::vector<std::shared_ptr<GeneralControl>> &controls) {
  std::string portConfig = config.get("server", "port");
  std::string threadConfig = config.get("server", "thread");
  std::stringstream ss;
  ss << portConfig;
  int port;
  ss >> port;

  ss.clear();
  ss.str("");

  std::string ip = 
    config.get("server", "ip"); 
  if (ip == "") {
    ip = "127.0.0.1";
  }

  int threadNum = 1;
  if (!threadConfig.empty()) {
    ss << threadConfig;
    ss >> threadNum;
  }
  for (auto &control : controls) {
    control->init(config);
  }
  start(ip, port, threadNum, controls);
}

 bool EvDrv::getBufferJson(evbuffer *buffer, Json::Value &root) {
   std::string body = getBufferStr(buffer);
   if (body == "") {
     return false;
   }
   Json::Reader reader(Json::Features::strictMode());
   return reader.parse(body, root);
}

void EvDrv::process(EvHttpRequest *req, 
                    const HttpControl *control) {
    int rc = 0;
    Json::Value root;
    Json::Value result;
    LOG(INFO) << control->url << ",";
    getQueryJson(req, root);
    if (control->jsonCb) {
      control->jsonCb(root, result);
      sendResponse(req, result);
      return;
    }
    if (control->bodyCb) {
      std::string resBody;
      control->bodyCb(root, resBody);
      sendResponseBody(req, resBody);
      return;
    }
    return;
}

std::string EvDrv::getBufferStr(struct evbuffer *buf) {
  std::string result;
  int recvLen = evbuffer_get_length(buf);
  int allocLen = 256;
  while (allocLen < recvLen) {
    allocLen <<= 1;
  }
  result.reserve(allocLen);
  int rc = 0;
  int len = 0;
  int remain = 0;
  while ((remain = evbuffer_get_length(buf)) > 0) {
    int n;
    char cbuf[128];
    n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
    if (n > 0) {
      len += n;
      result += std::string(cbuf, n);
    }
  }
  return result;
}

