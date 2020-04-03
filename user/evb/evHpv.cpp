#include "evHpv.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include "threadpool/thread_pool.h"
#include <iostream>
#include "hpvPool.h"
#include "parser.h"
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
    int size = atoi(tmp);
    conn->status = HpvConn::Status::RECV_BODY;
    conn->bodySize=size-4;
    len -=4;
  }

  if (len < conn->bodySize) {
    return;
  }
  bufferevent_read(bev, conn->buf, conn->bodySize);
  conn->status = HpvConn::Status::RECV_HEAD;
  (conn->parser)->run(conn->buf, conn->bodySize);

}

static void
 hpv_conn_writecb(
    struct bufferevent *bev, 
    void * arg){
   HpvConn*conn = (HpvConn*)arg;
   struct evbuffer *output = bufferevent_get_output(conn->bev);
	if (evbuffer_get_length(output) == 0) {
	//	bufferevent_free(conn->bev);
	}

}
static void
 hpv_conn_eventcb(
    struct bufferevent *bev, 
    short events,
    void * arg){
   HpvConn *conn = (HpvConn*)arg;
   conn->thr->setTaskCount(
       conn->thr->taskCount() - 1);
   delete conn->parser;
   while (true) {
    struct evbuffer *output = bufferevent_get_output(conn->bev);
	  if (evbuffer_get_length(output) == 0) {
      break;
    }
    char c;
     bufferevent_read(conn->bev, &c, 1);
   }
	 bufferevent_free(conn->bev);
   delete conn;
}

void hpv_accept_cb(
    struct evconnlistener *,
    int fd,
    struct sockaddr *, 
    int socklen, 
    void *userData) {
  printf("recv sock from %d\n", fd);
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
   conn->parser = new HpvParser(conn);
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
  // bufferevent_disable(bev, EV_WRITE);
   bufferevent_enable(bev, EV_READ);
  
}

