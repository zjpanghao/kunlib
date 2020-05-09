#ifndef INCLUDE_HPV_H
#define  INCLUDE_HPV_H
#include <memory>
#include <vector>
#include <string.h>
struct evconnlistener;
struct event_base;
struct sockaddr;
struct bufferevent;
class ExecutorService;
class HpvParser;
struct HpvPool;
struct HpvThread;
class HpvApp;
class ServerProtocol;
namespace kun {
 class Timer;
};
struct Hpv {
  struct evconnlistener *server{NULL};
  struct event_base *base;
  HpvPool *pool;
  struct event *signal_event;
  std::vector<HpvApp*> apps;
};
struct HpvServer {
  const char *ip;
  int port;
  int id;
};

struct HpvConn {
  enum Status {
   RECV_HEAD,
   RECV_BODY
  };
  int bodySize;
  char buf[1024];
  char sendBuf[1024];
  int sendLen;
  int fd;
  bool needClose{false};
  Hpv *hpv{nullptr};
  HpvThread *thr{nullptr};
  HpvParser *parser{nullptr};
  struct bufferevent *bev{nullptr};
  struct event_base *base;
  Status status{Status::RECV_HEAD};
  char ip[20];
  int port;
  bool server{true};
  std::shared_ptr<kun::Timer> timer{nullptr};
};

void hpv_send(HpvConn* conn, 
    const char *buf,
    int len);


class HpvApp {
  public:
    virtual void reg(int* code, 
        ServerProtocol **pro) = 0;
    virtual bool init() {};
    virtual void conn(HpvConn*conn){};
    virtual void onClose(HpvConn*conn){};
    virtual void recv(HpvConn*conn, 
        const char *buf,
        int len) {}
    void appSend(HpvConn*conn,
        const char *buf,
        int len);
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

void hpv_conn_close(HpvConn*conn);

std::shared_ptr<HpvConn>
  addServer(Hpv *hpv,
  HpvServer &server) ;
#endif
