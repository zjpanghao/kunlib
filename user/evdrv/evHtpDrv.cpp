#include "evHtpDrv.h"
#include <signal.h>
#include "json/json.h"
#include "urlMap.h"

EvHtpDrv& EvHtpDrv::getDrv() {
  static EvHtpDrv drv;
  return drv;
}

int EvHtpDrv::evReqMethod(EvHttpRequest*req) {
  return evhtp_request_get_method(req->htp);
}

void EvHtpDrv::generalCb(
    struct evhtp_request*req,
    void *arg) {
  auto hp = (HttpControl*)arg;
  EvHttpRequest request;
  request.htp = req;
  EvHtpDrv &drv = getDrv();
  drv.process(&request, hp);
}

void EvHtpDrv::sendResponse(EvHttpRequest *req,
    const Json::Value &value) {
  std::string s = value.toStyledString();
  evbuffer *response = req->htp->buffer_out;
  evbuffer_add_printf(response, "%s", s.c_str());
  evhtp_send_reply(req->htp, 200);
}

evbuffer *EvHtpDrv::getInputBuffer(EvHttpRequest *req) {
  return req->htp->buffer_in;
}

void EvHtpDrv::start(
       const std::string &ip,
       int port, 
       int nThread, 
       const std::vector<std::shared_ptr<UrlMap>> &controls) {
  if (signal(SIGPIPE, SIG_IGN) 
      == SIG_ERR) {
		return;
  }
  struct event_base *evbase 
    = event_base_new();
  struct evhtp *htp 
    = evhtp_new(evbase, NULL);
  for(auto &generalControl : controls) {
    for(auto &control : generalControl->getMapping()) {
      HttpControl *httpControl = new HttpControl(control);
      evhtp_set_cb(htp, 
          control.url.c_str()
          ,generalCb, (void*)httpControl);
      }
  }
  evhtp_use_threads_wexit(htp, NULL, NULL, nThread, NULL);
  evhtp_bind_socket(htp, ip.c_str(), port, 128);
  event_base_dispatch(evbase);
}
