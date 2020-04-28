#ifndef INCLUDE_PARSER_H
#define INCLUDE_PARSER_H
struct HpvConn;
class ServerProtocol {
  public:
   int  pack(
       HpvConn* conn,
       short mode,
       int erroCode,
                 const char *errorMsg,
                 const char *msg);
                 
   void serv(HpvConn*conn, 
       const char *data, int len);
   virtual void run(HpvConn*conn,const char *data, int len) = 0; 
};
class HpvParser {
 public:
   HpvParser(HpvConn *conn);
   void run(const char *data, int len);

   static void reg(int, ServerProtocol*server);

   static ServerProtocol* userProtocol_[1024];
   HpvConn *conn();
  private:
   HpvConn *conn_;
};
#endif
