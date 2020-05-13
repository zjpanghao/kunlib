#include "heart.h"
void HeartBeat::updateHeartBeat(HpvConn *conn) {
  std::lock_guard<std::mutex> guard(lock_);
  hearts_[conn].recvPack = true;
}

int HeartBeat::timeout(HpvConn *conn) {
  std::lock_guard<std::mutex> guard(lock_);
  auto it = hearts_.find(conn);
  if (it == hearts_.end()) {
    hearts_[conn].failCnt++;
    return 1;
  } else {
    if (it->second.recvPack) {
      it->second.failCnt = 0;
      it->second.recvPack = false;
    } else {
      it->second.failCnt++;
    }
    return it->second.failCnt;
  }
}
