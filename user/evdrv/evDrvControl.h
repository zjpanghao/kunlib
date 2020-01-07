#ifndef INCLUDE_EV_DRV_CONTROL_H
#define INCLUDE_EV_DRV_CONTROL_H
#include <vector>
#include <memory>

namespace kunyan {
  class Config;
}
class GeneralControl;
class EvDrvControl {
  public:
   static void startServer(
       const kunyan::Config &confg,
       const std::vector<GeneralControl*> &controls);

};
#endif
