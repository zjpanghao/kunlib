#ifndef INCLUDE_EV_HTTP_DRV_H
#define INCLUDE_EV_HTTP_DRV_H
#include "evDrv.h"

struct HttpParam {
  struct event_base *base;
};

class EvHttpDrv : public EvDrv{
  public:
    static void httpThread(void *param);
    virtual bool getQueryJson(EvHttpRequest*req, Json::Value &root) override;
    virtual evbuffer *getInputBuffer(EvHttpRequest *req) override;
    virtual void sendResponse(
        EvHttpRequest *req,
        const Json::Value &value) override;
    virtual void sendResponseBody(EvHttpRequest *req,
        const std::string &value) override;

    virtual void start(
        const std::string &ip,
        int port, 
        int nThread, 
        const std::vector<GeneralControl*> &controls) override;
   virtual int evReqMethod(EvHttpRequest*req) override;
    static void generalCb(struct evhttp_request*req, void *arg);
    static EvHttpDrv& getDrv();
};
#endif
