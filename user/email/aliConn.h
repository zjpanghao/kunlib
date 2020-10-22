#ifndef INCLUDE_ALICONN_H
#define INCLUDE_ALICONN_H
#include "utcp/tcplib.h"
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
class AliMail;
class AliConn {
  public:
  typedef int(*AliCb)(const char *,AliMail*);
    AliConn();
    ~AliConn();
      int sendMess(const char *info, AliCb cb, AliMail* arg) {
        if (close_) {
          return -1;
        }
        char buf[128];
        if (info && info[strlen(info) -1] != '\n' &&
            info[strlen(info) - 2] != '\r') {
          snprintf(buf, sizeof(buf), "%s\r\n", info);
          info = buf;
        }
        if (info) {
          int cnt = 0;
          if ((cnt=send(sock_, info, strlen(info), 0)) != strlen(info)) {
            std::cout << "send error" << std::endl;
            close_ = true;
            return -2;
          }

        }
        int rc = 0;
        while (cb) {
          int n = readLine(sock_, buf, sizeof(buf));
          if (n < 0 && errno == EINTR) {
            continue;
          }
          if (n == 0) {
            close_ = true;
            rc = -1;
          }
          std::cout << "n is:" << n << std::endl;
          std::cout << "buf:" << buf << std::endl;
          if (n > 0 && cb && cb(buf, arg)) {
            std::cout << "continue" << std::endl;
            continue;
          }
          if (n < 0) {
            rc = -2;
          }
          std::cout << "break;" << n << std::endl;
          break;
        }
        return rc;
    }

    bool isClose() {
      return close_;
    }

    bool sentryClose();

  private:
    int sock_{0};
    bool close_{false};
};
#endif