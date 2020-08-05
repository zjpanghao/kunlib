#ifndef INCLUDE_TCPLIB_H
#define INCLUDE_TCPLIB_H
int tcp_client(const char *ip, int port);
int tcp_server(const char *ip, int port);
int readLine(int sock, char *buf, int len);
#endif
