#ifndef INCLUDE_HPVCORE_H
#define INCLUDE_HPVCORE_H
#include <vector>

struct HpvConn;
class Hpv;
class HpvApp;
class HpvCore {
  public:
   HpvCore(std::vector<HpvApp*> app, int port);
   void loop();
   Hpv *hpv() {
     return hpv_;
   }

  private:
   Hpv *hpv_;
};
#endif
