#ifndef INCLUDE_HPV_H
#define  INCLUDE_HPV_H
struct evconnlistener;
struct event_base;
struct sockaddr;
struct bufferevent;
class ExecutorService;
class HpvParser;
struct HpvPool;
struct HpvThread;
struct Hpv {
  struct evconnlistener *server;
  struct event_base *base;
  HpvPool *pool;
};

struct HpvConn {
  enum Status {
   RECV_HEAD,
   RECV_BODY
  };
  int bodySize;
  char buf[1024];
  int fd;
  Hpv *hpv{nullptr};
  HpvThread *thr{nullptr};
  HpvParser *parser{nullptr};
  struct bufferevent *bev{nullptr};
  struct event_base *base;
  Status status{Status::RECV_HEAD};
};


void hpv_conn_cb(
    struct event_base *base,
    HpvThread*thd,
    void *conn);

void hpv_accept_cb(
    struct evconnlistener *,
    int fd,
    struct sockaddr *, 
    int socklen, 
    void *userData);
#endif
