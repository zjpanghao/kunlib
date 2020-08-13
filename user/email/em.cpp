#include "utcp/tcplib.h"
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <functional>
#include "pbase64/base64.h"
#include <vector>
template<class F, class ... Args>
int mess(int sock,
    const char *info, F cb, Args ... args) {
  char buf[128];
  if (info && info[strlen(info) -1] != '\n' &&
      info[strlen(info) - 2] != '\r') {
    snprintf(buf, sizeof(buf), "%s\r\n", info);
    info = buf;
  }
  if (info) {
    int cnt = 0;
    if ((cnt=send(sock, info, strlen(info), 0)) != strlen(info)) {
      std::cout << "send error" << std::endl;
      return -2;
    }
      
  }
  int n = readLine(sock, buf, sizeof(buf));
  if (n > 0) {
    return cb(std::forward<Args>(args)..., buf);
  }
  return -1;
}
int main() {
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
  int sock = tcp_client(ip,25);
  std::cout << "sock:" << sock << std::endl;
  auto pf2 = [](int code, const char *res) {
    std::cout <<"code:" << code << 
      "res:" << res << std::endl;
    return 0;
  };
  char buf[128] = "EHLO centos\r\n";
  mess(sock, "EHLO centos\r\n", pf2, -1);
  std::string account;
  std::string s = "panghao@221data.com";
  std::vector<unsigned char> data(s.begin(), s.end());
  Base64::getBase64().encode(data,
      account);
  for (int i = 0; i < 15; i++) {
    bool f = false;
    mess(sock, NULL, [&f](const char *info) {
        if (strstr(info, "DSN")) {
          f = true;
        }
        return 0;
        });
    if (f) {
      break;
    }
  }
  mess(sock, "AUTH LOGIN\r\n", pf2, -1);
  mess(sock, account.c_str(), pf2, -1);
  std::string pass;
  s = "3351970-ph";
  data.assign(s.begin(), s.end());
  Base64::getBase64().encode(data,
      pass);
  std::cout << "pass:" << pass << std::endl;
  auto pf = std::bind(pf2, 2, std::placeholders::_1);
  mess(sock, pass.c_str(), pf);
  mess(sock, "MAIL FROM:<panghao@221data.com>", pf);
  mess(sock, "RCPT TO:<panghao@221data.com>", pf);
  mess(sock, "DATA\r\n", pf);
  mess(sock, "subject:testmail\r\nfrom:<panghao@221data.com>\r\nto:<panghao@221data.com>\r\n\r\nThis is test mail\r\n.\r\n", pf);
  mess(sock, "QUIT\r\n", pf);
  return 0;
}
