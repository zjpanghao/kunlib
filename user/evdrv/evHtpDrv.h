#ifndef INCLUDE_EV_HTP_H
#define INCLUDE_EV_HTP_H
#include "evDrv.h"

class EvHtpDrv : public EvDrv{
  public:
    static EvHtpDrv &getDrv();
    virtual evbuffer *getInputBuffer(EvHttpRequest *req) override;
    virtual void sendResponse(EvHttpRequest *req,
        const Json::Value &value) override;
    virtual void start(
        const std::string &ip,
        int port, 
        int nThread, 
        const std::vector<std::shared_ptr<UrlMap>> &controls) override;
    static void generalCb(struct evhtp_request*req, void *arg);
    virtual int evReqMethod(EvHttpRequest*req) override;
};
#endif
