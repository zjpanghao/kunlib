#ifndef INCLUDE_HPVCORE_H
#define INCLUDE_HPVCORE_H
#include <vector>

class HpvApp {
 public:
   virtual bool init() = 0;
};

class HpvCore {
  public:
   static int coreLoop(
      std::vector<HpvApp*> apps, int port);
};
#endif
