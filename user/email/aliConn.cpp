#include "aliConn.h"
#include "utcp/tcplib.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

AliConn::AliConn() {
  struct hostent *ent =
    gethostbyname("smtp.mxhichina.com"); 
  std::cout << (ent->h_addrtype == AF_INET) <<" "
   << ent->h_length<< std::endl;
  char ip[64] ={0};
  for (int i = 0; i < ent->h_length; i++) {
    inet_ntop(ent->h_addrtype, ent->h_addr_list[i],
        ip,64);
    std::cout << ip << std::endl;
    break;
  }
  sock_ = tcp_client(ip,25);
  std::cout << "sock:" << sock_ << std::endl;
}

bool AliConn::sentryClose() {
 return 1 == readSentry(sock_);
}

AliConn::~AliConn() {
  close(sock_);
}
