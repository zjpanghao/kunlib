#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "evHpv.h"
#include "glog/logging.h"
ServerProtocol* HpvParser::userProtocol_[]{nullptr};
int ServerProtocol::pack(
    HpvConn* conn,
    short mode,
    int errorCode,
    const char *errorMsg,
    const char *msg) {
  char *buf = conn->sendBuf;
  buf += sizeof(int);
  *(short*)buf = mode;
  buf += sizeof(short);
  *(short*)(buf) = errorCode;
  buf += sizeof(short);
  if (errorMsg != NULL) {
    *(short*)(buf) = strlen(errorMsg);
    buf += sizeof(short);
    strncpy(buf, errorMsg, strlen(errorMsg));
    buf += strlen(errorMsg);
   } else {
      *(short*)(buf) = 0;
      buf += sizeof(short);
   }
  if (msg != NULL) {
    *(short*)(buf) = strlen(msg);
    buf += sizeof(short);
    strncpy(buf, msg, strlen(msg));
    buf += strlen(msg);
  } else {
    *(short*)(buf) = 0;
    buf += sizeof(short);
  }
  *(int*)conn->sendBuf = buf - conn->sendBuf;
  conn->sendLen = buf - conn->sendBuf;
  return 0;
}
HpvConn* HpvParser::conn() {
  return conn_;
}
HpvParser::HpvParser(HpvConn *conn):conn_(conn) {
}
void HpvParser::reg(int opcode, ServerProtocol*server) {
  userProtocol_[opcode] = server;
}

void HpvParser::run(
    const char *data, 
    int len) {
  if (len < 4) {
    return;
  }
  short mode = *(short*)data;
  data += 2;
  
  LOG(INFO)  << "mode" << mode;
  auto protocal = userProtocol_[mode];
  if (protocal != nullptr) {
    protocal->run(conn_, data, len-2);
  }
}

void ServerProtocol::serv(
    HpvConn*conn, 
    const char *data, 
    int len) {
  run(conn,  data, len);
}
