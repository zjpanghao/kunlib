#include "evHttpDrv.h"
#include <thread>
#include <signal.h>
#include "json/json.h"
#include "event2/http_struct.h"
#include "generalControl.h"
#include <glog/logging.h>

evbuffer *EvHttpDrv::getInputBuffer(
    EvHttpRequest *req) {
  return req->http->input_buffer;
}

void EvHttpDrv::generalCb(struct evhttp_request*req, void *arg) {
  auto hp = (HttpControl*)arg;
  EvHttpRequest request;
  request.http = req;
  EvHttpDrv &drv = getDrv();
  drv.process(&request, hp);
}

void EvHttpDrv::httpThread(void *param) {
  LOG(INFO) << "new Thread";
  struct event_base *base = ((struct HttpParam *)param)->base;
  event_base_dispatch(base);
}

void EvHttpDrv::start(
    const std::string &ip,
    int port, 
    int nThread, 
    const std::vector<std::shared_ptr<GeneralControl>> &controls) {
  int fd = 0;
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    return;
  }
  //std::map<std::string, HttpControl*> cmap;
  for (int i  =0; i < nThread; i++) {
  struct event_base *evbase = event_base_new();
  struct evhttp *http    = evhttp_new(evbase);
  for (auto &generalControl : controls) {
    auto gcnts = generalControl->getMapping();
    for (auto &control : 
        gcnts) {
      HttpControl *cnt = new HttpControl(control);
      evhttp_set_cb(http, 
          cnt->url.c_str(), 
          generalCb, 
          cnt);
    }
  }
  if (i == 0) {
    struct evhttp_bound_socket *bound;
    bound = evhttp_bind_socket_with_handle(http,
        ip.c_str(), port);
    if (!bound) {
        return;
    }
      fd = evhttp_bound_socket_get_fd(bound);
    } else {
      evhttp_accept_socket(http, fd);
    }
    HttpParam *param = new HttpParam();
    param->base = evbase;
    std::thread t(httpThread, param);
    t.detach();    
  }
}

void EvHttpDrv::sendResponse(EvHttpRequest *req,
    const pson::Json::Value &value) {
  std::string s = value.toStyledString();
  evbuffer *response = evbuffer_new();
  evbuffer_add_printf(response, "%s", s.c_str());
  evhttp_send_reply(req->http, 200, "OK", response);
  evbuffer_free(response);
}

void EvHttpDrv::sendResponseBody(EvHttpRequest *req,
    const std::string &value) {
  evbuffer *response = evbuffer_new();
  evbuffer_add_printf(response, "%s", value.c_str());
  evhttp_send_reply(req->http, 200, "OK", response);
  evbuffer_free(response);
}

int EvHttpDrv::evReqMethod(EvHttpRequest*req) {
  return evhttp_request_get_command(req->http);
}

bool EvHttpDrv::getQueryJson(EvHttpRequest*req, pson::Json::Value &root) {
  if (evReqMethod(req) == EVHTTP_REQ_POST) {
    EvDrv::getBufferJson(
        getInputBuffer(req), 
        root);
  }
  return false;
}

EvHttpDrv &EvHttpDrv::getDrv() {
  static EvHttpDrv drv;
  return drv;
}
