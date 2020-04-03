#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "evHpv.h"
ServerProtocal* HpvParser::userProtocal_[]{nullptr};
HpvConn* HpvParser::conn() {
  return conn_;
}
HpvParser::HpvParser(HpvConn *conn):conn_(conn) {
}
void HpvParser::reg(int opcode, ServerProtocal*server) {
  userProtocal_[opcode] = server;
}

void HpvParser::run(
    const char *data, 
    int len) {
  if (len < 4) {
    return;
  }
  char buf[5] = {0};
  strncpy(buf, data, 4);
  int opcode = atoi(buf);
  //std::cout << buf << std::endl;
  //std::cout <<"opcode" << opcode << std::endl;
  auto protocal = userProtocal_[opcode];
  if (protocal != nullptr) {
    protocal->run(conn_,data +4, len-4);
  }
}

void ServerProtocal::serv(
    HpvConn*conn, 
    const char *data, 
    int len) {
  run(conn,  data, len);
}
