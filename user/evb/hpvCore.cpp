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


static void signal_cb(evutil_socket_t, short, void *);

int HpvCore::coreLoop(
   std::vector<HpvApp*> apps, int port) {
  for (auto app : apps) {
    app->init();
  }
  Hpv hpv;
  hpv.pool = new HpvPool(5);
  struct event *signal_event;

  struct sockaddr_in sin;
  hpv.base = event_base_new();
  if (!hpv.base) {
    LOG(ERROR) << "Could not initialize libevent!";
    return 1;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  hpv.server = 
    evconnlistener_new_bind(hpv.base, 
        hpv_accept_cb, (void *)&hpv,
      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
      (struct sockaddr*)&sin,
      sizeof(sin));

  if (!hpv.server) {
    LOG(INFO) << "Could not create a listener!";
    return 1;
  }

  signal_event = evsignal_new(hpv.base, SIGINT, signal_cb, (void *)hpv.base);

  if (!signal_event || event_add(signal_event, NULL)<0) {
    fprintf(stderr, "Could not create/add a signal event!\n");
    return 1;
  }

  event_base_dispatch(hpv.base);

  evconnlistener_free(hpv.server);
  event_free(signal_event);
  event_base_free(hpv.base);
  return 0;
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

