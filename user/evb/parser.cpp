#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "evHpv.h"
#include "glog/logging.h"
#include "event2/bufferevent.h"
int ServerProtocol::readString(const char*buf,
    int &len,
    std::string &value) {
  short slen = 0;
  if (len < 2) {
    return 0;
  }
  int rlen = read16(buf, len, slen);
  if (rlen != sizeof(short)) {
    return 0;
  }
  if (slen > len) {
    return 0;
  }

  if (slen > 0) {
    value.assign(buf + sizeof(short), slen);
    len -= slen;
  }
  return sizeof(short) + slen;
}

int ServerProtocol::write16(char* data,
    int &len,
    short value) {
  if (len < 2) {
    return 0;
  }
  *(short*)data = value;
  len -= 2;
  return 2;
}

int ServerProtocol::writeString(char *data,
  const std::string &s) {
  if (s.length() > MAX_SEND_LEN) {
    return 0;
  }
  *(short*)data = s.length();
  strncpy(data + sizeof(short), s.c_str(), (int)s.length());
  return s.length()+ sizeof(short);
}

int ServerProtocol::read16(const char *data,
    int &len, short &value) {
  if (len < 2) {
    return 0;
  }
  value = *(short*)data;
  len -= 2;
  return 2;
}

const char * 
  ServerProtocol::read32(const char *data,
    int &len, int &value) {
  if (len < 4) {
    return NULL;
  }
  value = *(int*)data;
  len -= 4;
  return data + 4;
}

int ServerProtocol::sendPack(
    HpvConn* conn,
    const HpvPack &pack) {
  if (pack.len > 0 && pack.len <= sizeof(pack)) {
    bufferevent_write(conn->bev,
      &pack,
      pack.len);
    return 0;
  }
  return -1;
}

int ServerProtocol::sendPack(
    HpvConn* conn,
    short mode,
    short opcode,
    const char *buf,
    int len) {
  char *buffer = sendBuf_;
  buffer += sizeof(int);
  *(short*)(buffer) = mode;
  buffer += sizeof(short);
  *(short*)(buffer) = opcode;
  buffer += sizeof(short);
  if (buf && len > 0) {
    memcpy(buffer, buf, len);
    buffer += len;
  }
  *(int*)sendBuf_ = buffer - sendBuf_;
  sendLen_ = buffer - sendBuf_;
  bufferevent_write(conn->bev,
      sendBuf_,
      sendLen_);
  
  return 0;
}

void HpvParser::reg(int mode,
    ServerProtocol*server) {
  userProtocol_[mode] = server;
}

void HpvParser::run(
    HpvConn *conn,
    const char *data, 
    int len) {
  if (len < 4) {
    return;
  }
  short mode = *(short*)data;
  data += 2;
  short opcode = *(short*)data;
  data += 2;
#if DEBUG_EVB
  std::cout << "mode:" << mode
   << "opcode:" << opcode << std::endl; 
#endif
  auto protocal = userProtocol_[mode];
  if (protocal != nullptr) {
    protocal->run(conn, opcode, data, len-4);
  }
}

void ServerProtocol::serv(
    HpvConn*conn, 
    short opcode,
    const char *data, 
    int len) {
  run(conn,  opcode, data, len);
}
