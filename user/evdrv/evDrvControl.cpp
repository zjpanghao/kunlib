#include "evDrvControl.h"
#include "evHtpDrv.h"
#include "evHttpDrv.h"
#include "config/config.h"

void EvDrvControl::startServer(
       const kunyan::Config &config,
       const std::vector<std::shared_ptr<GeneralControl>> &controls) {
  EvDrv *drv = NULL;
  if (config.get("server", "htp") 
      != "") {
    LOG(INFO) << "evhtp";
    drv = &EvHtpDrv::getDrv();
  } else {
    drv = &EvHttpDrv::getDrv();
  }
  drv->startServer(config, controls);
}
