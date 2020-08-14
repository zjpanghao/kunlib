#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>

int tcp_client(const char *ip, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return sock;
  }
  struct sockaddr_in addr;
  addr.sin_family= AF_INET;
  addr.sin_addr.s_addr= inet_addr(ip);
  addr.sin_port = htons(port);
  int rc =connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  if (rc < 0) {
    return -1;
  }
  return sock;
}

int tcp_server(const char *ip, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return sock;
  }
  struct sockaddr_in addr;
  addr.sin_family= AF_INET;
  addr.sin_addr.s_addr= 
    inet_addr(!ip ? "0.0.0.0":ip);
  addr.sin_port = htons(port);
  int rc = bind(sock, 
      (struct sockaddr*)&addr, sizeof(addr));
  if (rc < 0) {
    return rc;
  }
  int use = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &use,
      sizeof(use));
  rc = listen(sock, 128);
  if (rc < 0) {
    return rc;
  }
  return sock;
}

int readSentry(int sock) {
  fd_set rset;
  struct timeval tv = {1, 0};
  while (true) {
    FD_ZERO(&rset);
    FD_SET(sock, &rset);
    int rc = select(sock + 1, 
        &rset, NULL, NULL, NULL);
    if (rc < 0) {
      return 1;
    } else if (rc == 0) {
      return 0;
    } else {
      if (FD_ISSET(sock, &rset)) {
        char c;
        int tmp =recv(sock, &c, 1, 0);
        if (tmp == 0) {
          return 1;
        } else if (tmp < 0 && errno != EINTR) {
          return 1;
        }
      }

    }
  }
  return 0;
}

int readLine(int sock, char *buffer, int len) {
  const int BUFSIZE = 3;
  char buf[BUFSIZE];
  int cnt = 0;
  char *bufp = NULL;
  const char *bufs = buffer;
  while (--len > 0) {
    if (--cnt <= 0 ) {
      do {
        cnt = recv(sock, buf, BUFSIZE, 0);
        if (cnt < 0 ) {
          if (errno == EINTR) {
            continue;
          }
          return -1;
        } else if (cnt == 0) {
          return 0;
        } 
      } while (0);
      bufp = buf;
    }
    *buffer++ = *bufp++;
    if (*(buffer-1) == '\n') {
      *buffer = 0;
      return buffer - bufs;
    }
  }
  return -1;
}


