#include "evHpv.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "threadpool/thread_pool.h"
#include <iostream>
#include "hpvPool.h"
#include "parser.h"
#include "glog/logging.h"
#include "timer/timer.h"

static void
hpv_conn_readcb(
    struct bufferevent *bev, 
    void * arg){
  HpvConn *conn = (HpvConn*)arg;
  struct evbuffer *buffer = bufferevent_get_input(bev);
  int len = evbuffer_get_length(buffer);
  if (conn->status == HpvConn::Status::RECV_HEAD) {
    if (len < 4) {
      return;
    }
    char tmp[9] = {0};
    bufferevent_read(bev, tmp, 4);
    int size = *(int*)tmp;
    conn->status = HpvConn::Status::RECV_BODY;
    conn->bodySize=size-4;
    len -=4;
  }

  if (len < conn->bodySize) {
    return;
  }
  bufferevent_read(bev, conn->buf, conn->bodySize);
  conn->status = HpvConn::Status::RECV_HEAD;
  (conn->parser)->run(conn, conn->buf, conn->bodySize);
  bufferevent_enable(bev, EV_WRITE);

}

static void
 hpv_conn_writecb(
    struct bufferevent *bev, 
    void * arg){
   HpvConn*conn = (HpvConn*)arg;
   struct evbuffer *output = bufferevent_get_output(conn->bev);
  int len;
  if (conn->needClose) {
	  hpv_conn_close(conn);
  }

}

void hpv_conn_close(HpvConn* conn) {
   conn->thr->setTaskCount(
       conn->thr->taskCount() - 1);
	 bufferevent_free(conn->bev);
   delete conn;
   LOG(INFO) << "delete :" << conn;
}

static void doConnectWork(HpvConn *conn) {
  conn->timer.reset(new kun::Timer());
  for (auto &app : conn->hpv->apps) {
    int code;
    ServerProtocol *prot;
    app->reg(&code, &prot);
    conn->parser->reg(code, prot);
    app->conn(conn);
  }

}

static void
 hpv_conn_eventcb(
    struct bufferevent *bev, 
    short events,
    void * arg){
   //LOG(INFO) << "conn event:" << events;
   HpvConn *conn = (HpvConn*)arg;
   if (events & BEV_EVENT_EOF) {
     LOG(INFO) << "onclose" << conn->hpv->apps.size();
     for (auto &app : conn->hpv->apps) {
       app->onClose(conn);
     }
     hpv_conn_close(conn);
   }
   if (events & BEV_EVENT_CONNECTED) {
     doConnectWork(conn);
   }
   //}
if (events &BEV_EVENT_TIMEOUT) {
  if (conn->timer != nullptr) {
    conn->timer->run();
  }
  bufferevent_enable(conn->bev, EV_READ);
  //   bufferevent_enable(conn->bev, EV_WRITE);
}
}

std::shared_ptr<HpvConn>
addServer(Hpv *hpv,
    HpvServer &server) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int flag = fcntl(sockfd,F_GETFL,0);
  fcntl(sockfd,F_SETFL,flag|O_NONBLOCK);
  HpvConn *conn = new HpvConn;
  strcpy(conn->ip, server.ip);
  conn->port = server.port;
  conn->fd = sockfd;
  conn->server = false;
  conn->hpv = hpv;
  HpvPoolCb poolCb;
  poolCb.arg = (void*)conn;
  poolCb.cb = hpv_conn_cb;
  hpv->pool->execute(poolCb);
  return nullptr;
}

void hpv_accept_cb(
    struct evconnlistener *,
    int fd,
    struct sockaddr *, 
    int socklen, 
    void *userData) {
  Hpv* hpv = (struct Hpv*)userData; 
  HpvConn *conn = new HpvConn;
  conn->hpv = hpv;
  conn->fd = fd;
  HpvPoolCb poolCb;
  poolCb.arg = (void*)conn;
  poolCb.cb = hpv_conn_cb;
  hpv->pool->execute(poolCb);

}

void hpv_conn_cb(
    struct event_base *base,
    HpvThread*thd,
    void *arg) {
   HpvConn *conn = (HpvConn*) arg;
   conn->thr = thd;
   conn->base = base;
   conn->parser = HpvParser::instance();
   struct bufferevent *bev;
   bev = bufferevent_socket_new(
       conn->base, 
       conn->fd, 
       BEV_OPT_DEFER_CALLBACKS|BEV_OPT_CLOSE_ON_FREE);
   if (!bev) {
     return;
   }
   conn->bev=bev;
   bufferevent_setcb(
       bev, 
       hpv_conn_readcb, 
       hpv_conn_writecb, 
       hpv_conn_eventcb, 
       conn);
  struct timeval tv{1,0};
  bufferevent_set_timeouts(bev, &tv, NULL);
   if (conn->server) {
     doConnectWork(conn);
   }
   if (!conn->server) {
     struct sockaddr_in serv_addr;
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = inet_addr(conn->ip);
     serv_addr.sin_port = htons(conn->port);
     bufferevent_socket_connect(bev,
         (struct sockaddr*)&serv_addr,
         sizeof(sockaddr));
   }
   
   bufferevent_enable(bev, EV_READ);
   bufferevent_enable(bev, EV_WRITE);
}

void HpvApp::appSend(HpvConn* conn,
    const char *buf,
    int len) {
  bufferevent_write(conn->bev, 
      buf, 
      len);
}

void hpv_send(HpvConn* conn,
    const char *buf, 
    int len) {
  bufferevent_write(conn->bev, buf, len);
}

