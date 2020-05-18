#ifndef INCLUDE_HPVCORE_H
#define INCLUDE_HPVCORE_H
#include <memory>
#include <vector>

struct HpvConn;
class Hpv;
class HpvApp;
struct HpvServer;
class HpvCore {
  public:
   HpvCore(std::vector<HpvApp*> app, int port);
   void loop();
   Hpv *hpv() {
     return hpv_;
   }

  HpvConn *
  addServer(HpvServer &server) ;

  private:
   bool server_;
   Hpv *hpv_;
};
#endif
