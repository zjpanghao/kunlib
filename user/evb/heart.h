#ifndef INCLUDE_HEART_H
#define INCLUDE_HEART_H
#include <map>
#include <mutex>
struct HpvConn;
class HeartBeat {
 public:
   struct HeartStat{
      bool recvPack{false};
      int failCnt{0};
   };
   void updateHeartBeat(HpvConn *conn);
   int timeout(HpvConn *conn);

 private:
   std::mutex lock_;
   std::map<HpvConn*, HeartStat> hearts_;

};
#endif
