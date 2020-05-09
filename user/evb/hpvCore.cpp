#include "hpvCore.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include "evHpv.h"
#include "threadpool/thread_pool.h"
#include "hpvPool.h"
#include "glog/logging.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "timer/timer.h"


static void signal_cb(evutil_socket_t, short, void *);

HpvCore::HpvCore(
   std::vector<HpvApp*> apps, int port) {
  for (auto app : apps) {
    app->init();
  }
  hpv_ = new Hpv();
  hpv_->apps = apps;
  
  hpv_->pool = new HpvPool(5);

  struct sockaddr_in sin;
  hpv_->base = event_base_new();
  if (!hpv_->base) {
    LOG(ERROR) << "Could not initialize libevent!";
    return;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  if (port > 0) {
    hpv_->server = 
      evconnlistener_new_bind(hpv_->base, 
          hpv_accept_cb, (void *)hpv_,
          LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
          (struct sockaddr*)&sin,
          sizeof(sin));
  }


  hpv_->signal_event = evsignal_new(hpv_->base, SIGINT, signal_cb, (void *)hpv_->base);

  if (!hpv_->signal_event 
      || event_add(hpv_->signal_event, NULL)<0) {
    fprintf(stderr, "Could not create/add a signal event!\n");
    return;
  }

}

void HpvCore::loop() {
  event_base_dispatch(hpv_->base);
  if (hpv_->server) {
    evconnlistener_free(hpv_->server);
  }
  event_free(hpv_->signal_event);
  event_base_free(hpv_->base);
}

  static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
  struct event_base *base = 
    (struct event_base*)user_data;
  struct timeval delay = { 2, 0 };

  printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

  event_base_loopexit(base, &delay);
}

