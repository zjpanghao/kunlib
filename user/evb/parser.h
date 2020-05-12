#ifndef INCLUDE_PARSER_H
#define INCLUDE_PARSER_H
#include <string>
struct HpvConn;
class ServerProtocol {
  private:
   static constexpr int MAX_SEND_LEN{1024};
  public:
    struct HpvPack {
      int len;
      short mode;
      short opcode;
      char buf[MAX_SEND_LEN];
    };
    int write16(char* data, int &len,
        short value);
    int writeString(char *data,
        const std::string &s);

    const char *read32(const char *data,
        int &len, 
        int &value);
    int read16(const char *data,
        int &len, 
        short &value);

    int readString(const char*buf,
        int &len,
        std::string &value);
    int sendPack(
        HpvConn*conn,
        const HpvPack &pack);

    int  sendPack(
        HpvConn* conn,
        short mode,
        short opcode,
        const char *buf,
        int len);

    void serv(HpvConn*conn, 
        short opcode,
        const char *data, int len);
    virtual void run(HpvConn*conn,
        short opcode,
        const char *data, int len) = 0; 

  private:
    HpvPack pack_;
    char sendBuf_[MAX_SEND_LEN];
    int sendLen_{0};
};
class HpvParser {
  public:
    static HpvParser* instance() {
      static HpvParser parser;
      return &parser;
    }

    void run(HpvConn *conn,const char *data, int len);

    void reg(
        int mode, ServerProtocol*pro);

    ServerProtocol* userProtocol_[1024]{nullptr};

};
#endif
