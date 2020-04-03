#ifndef INCLUDE_PARSER_H
#define INCLUDE_PARSER_H
struct HpvConn;
class ServerProtocal {
  public:
   void serv(HpvConn*conn, 
       const char *data, int len);
   virtual void run(HpvConn*conn,const char *data, int len) = 0; 
};
class HpvParser {
 public:
   HpvParser(HpvConn *conn);
   void run(const char *data, int len);

   static void reg(int, ServerProtocal*server);

   static ServerProtocal* userProtocal_[1024];
   HpvConn *conn();
  private:
   HpvConn *conn_;
};
#endif
